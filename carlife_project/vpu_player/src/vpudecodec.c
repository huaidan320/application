#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <asm/types.h>
#include <linux/videodev2.h>

#include "vpudecodec.h"
#include "circle_buffer.h"

static CIRCLE_RET_NUM ret_num = CIRCLE_RET_WAIT;

//#define STREAM_BUF_SIZE		0x200000
#define STREAM_BUF_SIZE		0x20000
#define PS_SAVE_SIZE		0x080000
#define STREAM_END_SIZE		0
//#define STREAM_FILL_SIZE	0x40000
#define STREAM_FILL_SIZE	0

#define PATH_V4L2	0
#define PATH_FILE	1
#define PATH_NET	2
#define PATH_IPU	3

#if 0
static int vpu_read(vpu_decode_t *dec, char *buf, int n)
{
    int nleft = 0;
    int nread = 0;
    char  *ptr;

    if (dec->src_scheme == PATH_NET) {
        return 0;//udp_recv(cmd, fd, buf, n);
    }else{
        ptr = buf;
        nleft = n;
        while (nleft > 0) {
            if ( (nread = read(dec->fd, ptr, nleft)) <= 0) {
                if (nread == 0)
                    return (n - nleft);

                perror("read");
                return (-1);			/* error EINTR XXX */
            }

            nleft -= nread;
            ptr   += nread;
        }

        return (n - nleft);
    }
}
#endif

static int vpu_read(vpu_decode_t *dec, char *buf, int n)
{ 
    if (dec->src_scheme == PATH_NET)
    {
        return 0;//udp_recv(cmd, fd, buf, n);
    }
    else
    {  
       do
       {
            ret_num = read_circle_buffer(buf, n);
       }while(ret_num == CIRCLE_RET_WAIT);

       return n;
    }
}

static int dec_fill_bsbuffer(vpu_decode_t *dec,
                             Uint32 defaultsize,
                             int *eos, int *fill_end_bs)
{
    RetCode ret;
    PhysicalAddress pa_read_ptr, pa_write_ptr;
    int size;
    int nread, room;
    Uint32 space;
    unsigned int target_addr;
    unsigned int bs_va_startaddr = dec->mem_desc.virt_uaddr;
    unsigned int bs_va_endaddr = dec->mem_desc.virt_uaddr + STREAM_BUF_SIZE;
    unsigned int bs_pa_startaddr = dec->mem_desc.phy_addr;
    *eos = 0;

    ret = vpu_DecGetBitstreamBuffer(dec->handle, &pa_read_ptr,
                                    &pa_write_ptr, &space);
    if (ret != RETCODE_SUCCESS) {
        err_msg("vpu_DecGetBitstreamBuffer failed\n");
        return -1;
    }

    /* Decoder bitstream buffer is full */
    if (space <= 0) {
        warn_msg("space %lu <= 0\n", space);
        return 0;
    }

    if (defaultsize > 0) {
        if (space < defaultsize)
            return 0;

        size = defaultsize;
    } else {
        size = ((space >> 9) << 9);
    }

    if (size == 0) {
        warn_msg("size == 0, space %lu\n", space);
        return 0;
    }

    /* Fill the bitstream buffer */
    target_addr = bs_va_startaddr + (pa_write_ptr - bs_pa_startaddr);
    if ( (target_addr + size) > bs_va_endaddr) {
        room = bs_va_endaddr - target_addr;
        nread = vpu_read(dec, (char *)target_addr, room);
        if (nread <= 0) {
            /* EOF or error */
            if (nread < 0) {
                if (errno == EAGAIN)
                    return 0;

                err_msg("nread %d < 0\n", nread);
                return -1;
            }

            *eos = 1;
        } else {
            /* unable to fill the requested size, so back off! */
            if (nread != room)
                goto update;

            /* read the remaining */
            space = nread;
            nread = vpu_read(dec, (char *)bs_va_startaddr,
                             (size - room));
            if (nread <= 0) {
                /* EOF or error */
                if (nread < 0) {
                    if (errno == EAGAIN)
                        return 0;

                    err_msg("nread %d < 0\n", nread);
                    return -1;
                }

                *eos = 1;
            }

            nread += space;
        }
    } else {
        nread = vpu_read(dec, (char *)target_addr, size);
        if (nread <= 0) {
            /* EOF or error */
            if (nread < 0) {
                if (errno == EAGAIN)
                    return 0;

                err_msg("nread %d < 0\n", nread);
                return -1;
            }

            *eos = 1;
        }
    }

update:
    if (*eos == 0) {
        ret = vpu_DecUpdateBitstreamBuffer(dec->handle, nread);
        if (ret != RETCODE_SUCCESS) {
            err_msg("vpu_DecUpdateBitstreamBuffer failed\n");
            return -1;
        }
        *fill_end_bs = 0;
    } else {
        if (!*fill_end_bs) {
            ret = vpu_DecUpdateBitstreamBuffer(dec->handle,
                                               STREAM_END_SIZE);
            if (ret != RETCODE_SUCCESS) {
                err_msg("vpu_DecUpdateBitstreamBuffer failed"
                        "\n");
                return -1;
            }
            *fill_end_bs = 1;
        }

    }

    return nread;
}

int decoder_common_init()
{
    RetCode ret;
    vpu_versioninfo ver;
    ret = vpu_Init(NULL);
    if (ret) {
        err_msg("VPU Init Failure.\n");
        return -1;
    }

    ret = vpu_GetVersionInfo(&ver);
    if (ret) {
        err_msg("Cannot get version info, err:%d\n", ret);
        vpu_UnInit();
        return -1;
    }

    info_msg("VPU firmware version: %d.%d.%d_r%d\n", ver.fw_major, ver.fw_minor,
             ver.fw_release, ver.fw_code);
    info_msg("VPU library version: %d.%d.%d\n", ver.lib_major, ver.lib_minor,
             ver.lib_release);
    return 0;
}

void decoder_common_uninit()
{
    vpu_UnInit();
}

int decoder_config(vpu_decode_t **phande)
{
    int ret = 0;
    vpu_decode_t *dec;

    dec = (vpu_decode_t *)malloc(sizeof(vpu_decode_t));
    if(NULL == dec){
        err_msg("Failed to allocate decode structure\n");
        ret = -1;
        goto err;
    }
    memset(dec, 0, sizeof(vpu_decode_t));

    dec->format = STD_AVC;
    dec->chromaInterleave = 0;
    dec->deblock_en = 0;
    dec->mapType = 0;
    dec->tiled2LinearEnable = 0;
    dec->dst_scheme = PATH_V4L2;
    dec->src_scheme = PATH_FILE;
    //dec->mp4_h264Class = 0;

    dec->mem_desc.size = STREAM_BUF_SIZE;
    ret = IOGetPhyMem(&(dec->mem_desc));
    if (ret) {
        err_msg("Unable to obtain physical mem\n");
        goto err1;
    }

    if (IOGetVirtMem(&(dec->mem_desc)) == -1) {
        err_msg("Unable to obtain virtual mem\n");
        ret = -1;
        goto err1;
    }

    if (dec->format == STD_AVC) {
        dec->ps_mem_desc.size = PS_SAVE_SIZE;
        ret = IOGetPhyMem(&(dec->ps_mem_desc));
        if (ret) {
            err_msg("Unable to obtain physical ps save mem\n");
            goto err1;
        }
    }

    *phande = dec;
    return 0;
err1:
    if (dec->format == STD_AVC)
        IOFreePhyMem(&(dec->ps_mem_desc));
    IOFreeVirtMem(&(dec->mem_desc));
    IOFreePhyMem(&(dec->mem_desc));
err:
    if(dec)
        free(dec);

    *phande = NULL;
    return ret;
}

int decoder_open(vpu_decode_t *dec)
{
    RetCode ret;
    DecHandle handle = {0};
    DecOpenParam oparam = {0};
    int eos = 0, fill_end_bs = 0, fillsize = 0;

    oparam.bitstreamFormat = dec->format;
    oparam.bitstreamBuffer = dec->mem_desc.phy_addr;
    oparam.bitstreamBufferSize = STREAM_BUF_SIZE;
    oparam.pBitStream = (Uint8 *)(dec->mem_desc.virt_uaddr);
    oparam.reorderEnable = 1;
    oparam.mp4DeblkEnable = dec->deblock_en;
    oparam.chromaInterleave = dec->chromaInterleave;
    oparam.mp4Class = 0;
    if (cpu_is_mx6x())
        oparam.avcExtension = 0;
    oparam.mjpg_thumbNailDecEnable = 0;
    oparam.mapType = dec->mapType;
    oparam.tiled2LinearEnable = dec->tiled2LinearEnable;
    oparam.bitstreamMode = 1;
    oparam.jpgLineBufferMode = 0;

    /*
     * mp4 deblocking filtering is optional out-loop filtering for image
     * quality. In other words, mpeg4 deblocking is post processing.
     * So, host application need to allocate new frame buffer.
     * - On MX27, VPU doesn't support mpeg4 out loop deblocking filtering.
     * - On MX5X, VPU control the buffer internally and return one
     *   more buffer after vpu_DecGetInitialInfo().
     * - On MX32, host application need to set frame buffer externally via
     *   the command DEC_SET_DEBLOCK_OUTPUT.
     */
    if (oparam.mp4DeblkEnable == 1) {
        dec->deblock_en = 0;
    }

    oparam.psSaveBuffer = dec->ps_mem_desc.phy_addr;
    oparam.psSaveBufferSize = PS_SAVE_SIZE;

    ret = vpu_DecOpen(&handle, &oparam);
    if (ret != RETCODE_SUCCESS) {
        err_msg("vpu_DecOpen failed, ret:%d\n", ret);
        return -1;
    }

    dec->handle = handle;

    if(dec->src_scheme == PATH_NET)
        fillsize = 1024;
    
    ret = dec_fill_bsbuffer(dec, fillsize, &eos, &fill_end_bs);
    
    if (fill_end_bs)
        err_msg("Update 0 before seqinit, fill_end_bs=%d\n", fill_end_bs);

    if (ret < 0) {
        err_msg("dec_fill_bsbuffer failed\n");
        decoder_close(dec);
        return -1;
    }

    return 0;
}

void decoder_close(vpu_decode_t *dec)
{
    RetCode ret;

    ret = vpu_DecClose(dec->handle);
    if (ret == RETCODE_FRAME_NOT_COMPLETE) {
        vpu_SWReset(dec->handle, 0);
        ret = vpu_DecClose(dec->handle);
        if (ret != RETCODE_SUCCESS)
            err_msg("vpu_DecClose failed\n");
    }
}

int decoder_parse(vpu_decode_t *dec)
{
    DecInitialInfo initinfo = {0};
    DecHandle handle = dec->handle;
    int align, profile, level, extended_fbcount;
    RetCode ret;
    int origPicWidth, origPicHeight;

    /* Parse bitstream and get width/height/framerate etc */
    vpu_DecSetEscSeqInit(handle, 1);
    ret = vpu_DecGetInitialInfo(handle, &initinfo);
    vpu_DecSetEscSeqInit(handle, 0);
    if (ret != RETCODE_SUCCESS) {
        err_msg("vpu_DecGetInitialInfo failed, ret:%d, errorcode:%ld\n",
                ret, initinfo.errorcode);
        return -1;
    }

    if (initinfo.streamInfoObtained) {
        switch (dec->format) {
        case STD_AVC:
            info_msg("H.264 Profile: %d Level: %d Interlace: %d\n",
                     initinfo.profile, initinfo.level, initinfo.interlace);

            if (initinfo.aspectRateInfo) {
                int aspect_ratio_idc;
                int sar_width, sar_height;

                if ((initinfo.aspectRateInfo >> 16) == 0) {
                    aspect_ratio_idc = (initinfo.aspectRateInfo & 0xFF);
                    info_msg("aspect_ratio_idc: %d\n", aspect_ratio_idc);
                } else {
                    sar_width = (initinfo.aspectRateInfo >> 16) & 0xFFFF;
                    sar_height = (initinfo.aspectRateInfo & 0xFFFF);
                    info_msg("sar_width: %d, sar_height: %d\n",
                             sar_width, sar_height);
                }
            } else {
                info_msg("Aspect Ratio is not present.\n");
            }
            break;
        case STD_MPEG4:
            if (initinfo.level & 0x80) { /* VOS Header */
                initinfo.level &= 0x7F;
                if (initinfo.level == 8 && initinfo.profile == 0) {
                    level = 0; profile = 0;	 /* Simple, Level_L0 */
                } else {
                    switch (initinfo.profile) {
                    case 0xB:
                        profile = 1; /* advanced coding efficiency object */
                        break;
                    case 0xF:
                        if ((initinfo.level & 8) == 0)
                            profile = 2; /* advanced simple object */
                        else
                            profile = 5; /* reserved */
                        break;
                    case 0x0:
                        profile = 0;
                        break;    /* Simple Profile */
                    default:
                        profile = 5;
                        break;
                    }
                    level = initinfo.level;
                }
            } else { /* VOL Header only */
                level = 7;  /* reserved */
                switch (initinfo.profile) {
                case 0x1:
                    profile = 0;  /* simple object */
                    break;
                case 0xC:
                    profile = 1;  /* advanced coding efficiency object */
                    break;
                case 0x11:
                    profile = 2;  /* advanced simple object */
                    break;
                default:
                    profile = 5;  /* reserved */
                    break;
                }
            }
            dec->interlaced = initinfo.interlace;

            info_msg("Mpeg4 Profile: %d Level: %d Interlaced: %d\n",
                     profile, level, initinfo.interlace);
            /*
             * Profile: 8'b00000000: SP, 8'b00010001: ASP
             * Level: 4'b0000: L0, 4'b0001: L1, 4'b0010: L2, 4'b0011: L3, ...
             * SP: 1/2/3/4a/5/6, ASP: 0/1/2/3/4/5
             */
            if (initinfo.aspectRateInfo)
                info_msg("Aspect Ratio Table index: %d\n", initinfo.aspectRateInfo);
            else
                info_msg("Aspect Ratio is not present.\n");

            break;
        default:
            break;
        }
    }

    dec->lastPicWidth = initinfo.picWidth;
    dec->lastPicHeight = initinfo.picHeight;

    if (cpu_is_mx6x()){
        info_msg("Decoder: width = %d, height = %d, frameRateRes = %lu, frameRateDiv = %lu, count = %u\n",
                 initinfo.picWidth, initinfo.picHeight,
                 initinfo.frameRateRes, initinfo.frameRateDiv,
                 initinfo.minFrameBufferCount);
    }
    else{
        info_msg("Decoder: width = %d, height = %d, fps = %lu, count = %u\n",
                 initinfo.picWidth, initinfo.picHeight,
                 initinfo.frameRateInfo,
                 initinfo.minFrameBufferCount);
    }

    dec->minfbcount = initinfo.minFrameBufferCount;
    extended_fbcount = 2;


    if (initinfo.interlace)
        dec->regfbcount = dec->minfbcount + extended_fbcount + 2;
    else
        dec->regfbcount = dec->minfbcount + extended_fbcount;
    dprintf(4, "minfb %d, extfb %d\n", dec->minfbcount, extended_fbcount);

    origPicWidth = initinfo.picWidth;
    origPicHeight = initinfo.picHeight;


    dec->picwidth = ((origPicWidth + 15) & ~15);

    align = 16;
    if ((dec->format == STD_MPEG2 ||
         dec->format == STD_VC1 ||
         dec->format == STD_AVC ||
         dec->format == STD_VP8) && initinfo.interlace == 1)
        align = 32;

    dec->picheight = ((origPicHeight + align - 1) & ~(align - 1));

    if ((dec->picwidth == 0) || (dec->picheight == 0))
        return -1;

    /* Add non-h264 crop support, assume left=top=0 */
    if ((dec->picwidth > origPicWidth ||
         dec->picheight > origPicHeight) &&
            (!initinfo.picCropRect.left &&
             !initinfo.picCropRect.top &&
             !initinfo.picCropRect.right &&
             !initinfo.picCropRect.bottom)) {
        initinfo.picCropRect.left = 0;
        initinfo.picCropRect.top = 0;
        initinfo.picCropRect.right = origPicWidth;
        initinfo.picCropRect.bottom = origPicHeight;
    }

    info_msg("CROP left/top/right/bottom %lu %lu %lu %lu\n",
             initinfo.picCropRect.left,
             initinfo.picCropRect.top,
             initinfo.picCropRect.right,
             initinfo.picCropRect.bottom);

    memcpy(&(dec->picCropRect), &(initinfo.picCropRect),
           sizeof(initinfo.picCropRect));

    /* worstSliceSize is in kilo-byte unit */
    dec->slice_mem_desc.size = initinfo.worstSliceSize * 1024;
    dec->stride = dec->picwidth;

    if (dec->format == STD_AVC) {
        //dec->slice_mem_desc.size = dec->phy_slicebuf_size;
        ret = IOGetPhyMem(&(dec->slice_mem_desc));
        if (ret) {
            err_msg("Unable to obtain physical slice save mem\n");
            return -1;
        }
    }

    return 0;
}

int decoder_allocate_framebuffer(vpu_decode_t *dec)
{
    DecBufInfo bufinfo;
    int i, regfbcount = dec->regfbcount/*5*/, totalfb, img_size;
    int dst_scheme = dec->dst_scheme;

    RetCode ret;
    DecHandle handle = dec->handle;
    FrameBuffer *fb;
    struct FramePool *pfbpool;
    struct v4l_buf *buf;
    int stride;
    int delay = -1;


    totalfb = regfbcount;
    //             5     +         0;
    dprintf(4, "regfb %d\n", regfbcount);

    fb = calloc(totalfb, sizeof(FrameBuffer));
    if (fb == NULL) {
        err_msg("Failed to allocate fb\n");
        return -1;
    }

    pfbpool = dec->m_pPool = calloc(totalfb, sizeof(struct FramePool));
    if (pfbpool == NULL) {
        err_msg("Failed to allocate pfbpool\n");
        free(fb);
        return -1;
    }

    if (dst_scheme == PATH_V4L2){
        if(v4l_display_open(dec) < 0){
            err_msg("v4l2 display open failed.");
            goto err;
        }
        buf = dec->v4ldisp->v4l_addr;
    }else {
        buf = NULL;
        goto err;
    }
//    else if (dst_scheme == PATH_IPU)
//        disp = ipu_display_open(dec, totalfb, /*rotation*/0, dec->picCropRect);

//    if (disp == NULL) {
//        goto err;
//    }

    dec->fps = 25;


    info_msg("Display fps will be %d\n", dec->fps);

    img_size = dec->stride * dec->picheight;

    for (i = 0; i < totalfb; i++) {
        fb[i].myIndex = i;

#ifdef __TEST_VPU_DEC__
        memset(&(pfbpool[i].desc), 0, sizeof(vpu_mem_desc));
        pfbpool[i].desc.size = img_size >> 2;
        ret = IOGetPhyMem(&(pfbpool[i].desc));
        if (ret) {
            err_msg("buf alloc failed\n");
            goto err1;
        }
        fb[i].bufY = pfbpool[i].desc.phy_addr;
#else
        if (dst_scheme == PATH_V4L2){
            fb[i].bufY = buf[i].offset;
            info_msg("bufY=offset: %#x\n", (unsigned int)(buf[i].offset));
        }
        else if (dst_scheme == PATH_IPU){
//            fb[i].bufY = disp->ipu_bufs[i].ipu_paddr;
            err_msg("dst_scheme is invalid.\n");
            goto err1;
        }else{
            err_msg("dst_scheme is invalid.\n");
            goto err1;
        }
#endif  //__TEST_VPU_DEC__

        fb[i].bufCb = fb[i].bufY + img_size;
        fb[i].bufCr = fb[i].bufCb + (img_size >> 2);
        /* allocate MvCol buffer here */
        //if (!cpu_is_mx27())
        memset(&(pfbpool[i].mvdesc), 0, sizeof(vpu_mem_desc));
        pfbpool[i].mvdesc.size = img_size >> 2;
        ret = IOGetPhyMem(&(pfbpool[i].mvdesc));
        if (ret) {
            err_msg("buf alloc failed\n");
            goto err1;
        }
        //it's used to B-frame decodec in MPEG-2, AVC MP/HP, MPEG-4 ASP, VC-1 MP/AP, and so on.
        fb[i].bufMvCol = pfbpool[i].mvdesc.phy_addr;
        pfbpool[i].fb = fb + i;
    }



    stride = ((dec->stride + 15) & ~15);

    if (dec->format == STD_AVC) {
        bufinfo.avcSliceBufInfo.bufferBase = dec->slice_mem_desc.phy_addr;
        bufinfo.avcSliceBufInfo.bufferSize = dec->slice_mem_desc.size;
    }

    /* User needs to fill max suported macro block value of frame as following*/
    bufinfo.maxDecFrmInfo.maxMbX = dec->stride / 16;
    bufinfo.maxDecFrmInfo.maxMbY = dec->picheight / 16;
    bufinfo.maxDecFrmInfo.maxMbNum = dec->stride * dec->picheight / 256;

    /* For H.264, we can overwrite initial delay calculated from syntax.
         * delay can be 0,1,... (in unit of frames)
         * Set to -1 or do not call this command if you don't want to overwrite it.
         * Take care not to set initial delay lower than reorder depth of the clip,
         * otherwise, display will be out of order. */
    vpu_DecGiveCommand(handle, DEC_SET_FRAME_DELAY, &delay);

    ret = vpu_DecRegisterFrameBuffer(handle, fb, dec->regfbcount, stride, &bufinfo);
    if (ret != RETCODE_SUCCESS) {
        err_msg("Register frame buffer failed, ret=%d\n", ret);
        goto err1;
    }

    return 0;
err1:
    if (dst_scheme == PATH_V4L2) {
        v4l_display_stop(dec->v4ldisp);
        dec->v4ldisp = NULL;
    } else if (dst_scheme == PATH_IPU) {
//        ipu_display_close(disp);
//        dec->disp = NULL;
    }
    for (i = 0; i < totalfb; i++) {
        IOFreePhyMem(&(pfbpool[i].mvdesc));
    }
err:
    free(fb);
    free(pfbpool);
    dec->m_pPool = NULL;
    return -1;
}

int decoder_start(vpu_decode_t *dec)
{
    DecHandle handle = dec->handle;
    DecOutputInfo outinfo = {0};
    DecParam decparam = {0};
    int err = 0, eos = 0, fill_end_bs = 0, decodefinish = 0;
    struct timeval tdec_begin,tdec_end, total_start, total_end;
    RetCode ret;
    int sec, usec, loop_id;
    double frame_id = 0, total_time=0;
    int decIndex = 0;
    int rotid = 0;
    int totalNumofErrMbs = 0;
    int disp_clr_index = -1, actual_display_index = -1, field = V4L2_FIELD_NONE;
    int is_waited_int = 0;
    int index = -1;


    decparam.dispReorderBuf = 0;

    decparam.skipframeMode = 0;
    decparam.skipframeNum = 0;
    /*
     * once iframeSearchEnable is enabled, prescanEnable, prescanMode
     * and skipframeMode options are ignored.
     */
    decparam.iframeSearchEnable = 0;

    gettimeofday(&total_start, NULL);

    if(dec->dst_scheme == PATH_V4L2){
        if(v4l_display_start(dec->v4ldisp) < 0){
            err_msg("Can't start v4l2 display");
            goto err;
        }
    }

    while (1) {
        gettimeofday(&tdec_begin, NULL);
        ret = vpu_DecStartOneFrame(handle, &decparam);
        if (ret != RETCODE_SUCCESS) {
            err_msg("DecStartOneFrame failed, ret=%d\n", ret);
            goto err;
        }

        is_waited_int = 0;
        loop_id = 0;
        while (vpu_IsBusy()) {
            /*
             * Suppose vpu is hang if one frame cannot be decoded in 5s,
             * then do vpu software reset.
             * Please take care of this for network case since vpu
             * interrupt also cannot be received if no enough data.
             */
            err = dec_fill_bsbuffer(dec, STREAM_FILL_SIZE,
                                    &eos, &fill_end_bs);
            if (err < 0) {
                err_msg("dec_fill_bsbuffer failed\n");
                return -1;
            }
            if (loop_id == 50) {
                err = vpu_SWReset(handle, 0);
                goto err;
            }

            if (vpu_WaitForInt(100) == 0)
                is_waited_int = 1;
            loop_id ++;
        }

        if (!is_waited_int)
            vpu_WaitForInt(100);

        gettimeofday(&tdec_end, NULL);
        sec = tdec_end.tv_sec - tdec_begin.tv_sec;
        usec = tdec_end.tv_usec - tdec_begin.tv_usec;

        if (usec < 0) {
            sec--;
            usec = usec + 1000000;
        }

        dec->tdec_time += (sec * 1000000) + usec;

        ret = vpu_DecGetOutputInfo(handle, &outinfo);

        dprintf(3, "frame_id %d, decidx %d, disidx %d, rotid %d, decodingSuccess 0x%x\n",
                (int)frame_id, outinfo.indexFrameDecoded, outinfo.indexFrameDisplay,
                rotid, outinfo.decodingSuccess);
        if (ret != RETCODE_SUCCESS) {
            err_msg("vpu_DecGetOutputInfo failed Err code is %d\n"
                    "\tframe_id = %d\n", ret, (int)frame_id);
            goto err;
        }

        if (outinfo.decodingSuccess == 0) {
            warn_msg("Incomplete finish of decoding process.\n"
                     "\tframe_id = %d\n", (int)frame_id);

            continue;
        }

        if (cpu_is_mx6x() && (outinfo.decodingSuccess & 0x10)) {
            warn_msg("vpu needs more bitstream in rollback mode\n"
                     "\tframe_id = %d\n", (int)frame_id);

            err = dec_fill_bsbuffer(dec, 0, &eos, &fill_end_bs);
            if (err < 0) {
                err_msg("dec_fill_bsbuffer failed\n");
                goto err;
            }
            continue;
        }

        if (cpu_is_mx6x() && (outinfo.decodingSuccess & 0x100000))
            warn_msg("sequence parameters have been changed\n");

        if (outinfo.notSufficientPsBuffer) {
            err_msg("PS Buffer overflow\n");
            goto err;
        }

        if (outinfo.notSufficientSliceBuffer) {
            err_msg("Slice Buffer overflow\n");
            goto err;
        }


        if (outinfo.indexFrameDisplay == -1){
            decodefinish = 1;
        }

        if(decodefinish == 1)
            break;

        if(outinfo.indexFrameDecoded >= 0) {
            /* We MUST be careful of sequence param change (resolution change, etc)
             * Different frame buffer number or resolution may require vpu_DecClose
             * and vpu_DecOpen again to reallocate sufficient resources.
             * If you already allocate enough frame buffers of max resolution
             * in the beginning, you may not need vpu_DecClose, etc. But sequence
             * headers must be ahead of their pictures to signal param change.
             */
            if ((outinfo.decPicWidth != dec->lastPicWidth)
                    ||(outinfo.decPicHeight != dec->lastPicHeight)) {
                warn_msg("resolution changed from %dx%d to %dx%d\n",
                         dec->lastPicWidth, dec->lastPicHeight,
                         outinfo.decPicWidth, outinfo.decPicHeight);
                dec->lastPicWidth = outinfo.decPicWidth;
                dec->lastPicHeight = outinfo.decPicHeight;
            }

            if (outinfo.numOfErrMBs) {
                totalNumofErrMbs += outinfo.numOfErrMBs;
                info_msg("Num of Error Mbs : %d, in Frame : %d \n",
                         outinfo.numOfErrMBs, decIndex);
            }
            decIndex++;
        }

        /* BIT don't have picture to be displayed */
        if ((outinfo.indexFrameDisplay == -3) ||
                (outinfo.indexFrameDisplay == -2)) {
            dprintf(3, "VPU doesn't have picture to be displayed.\n"
                       "\toutinfo.indexFrameDisplay = %d\n",
                    outinfo.indexFrameDisplay);

            if (dec->dst_scheme != PATH_IPU) {
                if (disp_clr_index >= 0) {
                    err = vpu_DecClrDispFlag(handle, disp_clr_index);
                    if (err)
                        err_msg("vpu_DecClrDispFlag failed Error code"
                                " %d\n", err);
                }
                disp_clr_index = outinfo.indexFrameDisplay;
            }
            continue;
        }

        actual_display_index = outinfo.indexFrameDisplay;

        if ((dec->dst_scheme == PATH_V4L2) || (dec->dst_scheme == PATH_IPU))
        {
            if (dec->dst_scheme == PATH_V4L2){
                err = v4l_put_data(dec, actual_display_index, field);
            }
            else if (dec->dst_scheme == PATH_IPU){
//                err = ipu_put_data(disp, actual_display_index, dec->decoded_field[actual_display_index], dec->cmdl->fps);
            }
            if (err)
                return -1;

            if (dec->dst_scheme == PATH_V4L2)
            {
                if (dec->dst_scheme == PATH_V4L2)
                {
//                    index = v4l_rsd->buf.index;
                    index = actual_display_index;
                }


                if (disp_clr_index >= 0) {
                    err = vpu_DecClrDispFlag(handle, disp_clr_index);
                    if (err)
                        err_msg("vpu_DecClrDispFlag failed Error code"
                                " %d\n", err);
                }

                disp_clr_index = index;
            }
        }

        frame_id++;

        if(dec->src_scheme == PATH_NET){
            err = dec_fill_bsbuffer(dec, STREAM_FILL_SIZE,
                                    &eos, &fill_end_bs);
            if (err < 0) {
                err_msg("dec_fill_bsbuffer failed\n");
                return -1;
            }
        }
    }

    if (totalNumofErrMbs) {
        info_msg("Total Num of Error MBs : %d\n", totalNumofErrMbs);
    }

    gettimeofday(&total_end, NULL);
    sec = total_end.tv_sec - total_start.tv_sec;
    usec = total_end.tv_usec - total_start.tv_usec;
    if (usec < 0) {
        sec--;
        usec = usec + 1000000;
    }
    total_time = (sec * 1000000) + usec;

    info_msg("%d frames took %d microseconds\n", (int)frame_id,
             (int)total_time);
    info_msg("dec fps = %.2f\n", (frame_id / (dec->tdec_time / 1000000)));
    info_msg("total fps= %.2f \n",(frame_id / (total_time / 1000000)));
    return 0;
err:
    if(dec->dst_scheme == PATH_V4L2)
        v4l_display_stop(dec->v4ldisp);
    return -1;
}

void free_handle(vpu_decode_t *dec)
{
    int i;

    if (dec->format == STD_AVC)
        IOFreePhyMem(&(dec->ps_mem_desc));
    IOFreeVirtMem(&(dec->mem_desc));

    IOFreePhyMem(&(dec->mem_desc));

    for(i = 0; i < dec->regfbcount; ++i){
        IOFreePhyMem(&(dec->m_pPool[i].mvdesc));
#ifdef __TEST_VPU_DEC__
        IOFreePhyMem(&(dec->m_pPool[i].desc));
#endif  //__TEST_VPU_DEC__
    }

    free(dec->m_pPool[0].fb);

    free(dec->m_pPool);

    free(dec);
}
