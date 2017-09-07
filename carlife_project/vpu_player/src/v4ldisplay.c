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

#include "v4ldisplay.h"

static void v4l_free_bufs(int n, vpu_display_t *disp)
{
    int i;
    struct v4l_buf *buf = disp->v4l_addr;

    for (i = 0; i < n; i++) {
        if ((buf + i) != NULL) {
            if (buf[i].start != NULL)
                munmap(buf[i].start, buf[i].length);
        }
    }
    free(buf);
}

int v4l_display_open(vpu_decode_t *dec)
{
    int width = dec->picwidth;
    int height = dec->picheight;
    int left = dec->picCropRect.left;
    int top = dec->picCropRect.top;
    int right = dec->picCropRect.right;
    int bottom = dec->picCropRect.bottom;
    int disp_width = 1600;
    int disp_height = 480;
    int disp_left =  0;
    int disp_top =  0;
    int fd = -1, err = 0, i = 0;
    char v4l_device[80];
    struct v4l2_cropcap cropcap = {0};
    struct v4l2_crop crop = {0};
    struct v4l2_format fmt = {0};
    struct v4l2_requestbuffers reqbuf = {0};
    struct v4l2_rect icrop = {0};
    vpu_display_t *disp;
    struct v4l_buf *v4l_rsd;
    int fd_fb;
    struct mxcfb_gbl_alpha alpha;
    struct v4l2_buffer *buffer;

    fd_fb = open("/dev/fb0", O_RDWR, 0);

    if (fd_fb < 0) {
        err_msg("unable to open fb0\n");
        return -1;
    }
    alpha.alpha = 0;
    alpha.enable = 1;
    if (ioctl(fd_fb, MXCFB_SET_GBL_ALPHA, &alpha) < 0) {
        err_msg("set alpha blending failed\n");
        return -1;
    }
    close(fd_fb);


    disp = (vpu_display_t *)calloc(1, sizeof(vpu_display_t));
    if (disp == NULL) {
        err_msg("falied to allocate vpu_display\n");
        return -1;
    }
    memset(disp, 0, sizeof(vpu_display_t));

    strcpy(v4l_device, "/dev/video17");

    fd = open(v4l_device, O_RDWR, 0);
    if (fd < 0) {
        err_msg("unable to open %s\n", v4l_device);
        free(disp);
        return -1;
    }

    info_msg("v4l output to %s\n", v4l_device);


    cropcap.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    err = ioctl(fd, VIDIOC_CROPCAP, &cropcap);
    if (err < 0) {
        err_msg("VIDIOC_CROPCAP failed\n");
        goto err;
    }
    dprintf(1, "cropcap.bounds.width = %d\n\tcropcap.bound.height = %d\n\t" \
        "cropcap.defrect.width = %d\n\tcropcap.defrect.height = %d\n",
        cropcap.bounds.width, cropcap.bounds.height,
        cropcap.defrect.width, cropcap.defrect.height);


    crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    crop.c.top = disp_top;
    crop.c.left = disp_left;
    crop.c.width = width;
    crop.c.height = height;

    if ((disp_width != 0) && (disp_height!= 0 )) {
        crop.c.width = disp_width;
        crop.c.height = disp_height;
    } else if (!cpu_is_mx27()) {
        crop.c.width = cropcap.bounds.width;
        crop.c.height = cropcap.bounds.height;
    }

    info_msg("Display to %d %d, top offset %d, left offset %d\n",
            crop.c.width, crop.c.height, disp_top, disp_left);

    dprintf(4, "crop.c.width/height: %d/%d\n", crop.c.width, crop.c.height);

    err = ioctl(fd, VIDIOC_S_CROP, &crop);
    if (err < 0) {
        err_msg("VIDIOC_S_CROP failed\n");
        goto err;
    }

    if (cpu_is_mx6x()) {
        /* Set rotation via new V4L2 interface on 2.6.38 kernel */
        struct v4l2_control ctrl;

        ctrl.id = V4L2_CID_ROTATE;
        ctrl.value = 0;
        err = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
        if (err < 0) {
            err_msg("VIDIOC_S_CTRL failed\n");
            goto err;
        }
    } else {
        struct v4l2_control ctrl;
        ctrl.id = V4L2_CID_PRIVATE_BASE;
        ctrl.value = 0;
        err = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
        if (err < 0) {
            err_msg("VIDIOC_S_CTRL failed\n");
            goto err;
        }
    }

    dprintf(4, "top:%d,left:%d,right:%d,bottom:%d, nframes: %d\n",
            top,left,right,bottom,dec->regfbcount);
    fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    if (right && bottom) {
        /* This is aligned with new V4L interface on 2.6.38 kernel */
        fmt.fmt.pix.width = width;
        fmt.fmt.pix.height = height;
        fmt.fmt.pix.bytesperline = width;
        icrop.left = left;
        icrop.top = top;
        icrop.width = right - left;
        icrop.height = bottom - top;
        fmt.fmt.pix.priv =  (unsigned long)&icrop;
    } else {
        fmt.fmt.pix.width = width;
        fmt.fmt.pix.height = height;
        fmt.fmt.pix.bytesperline = width;
    }
    dprintf(1, "fmt.fmt.pix.width = %d\n\tfmt.fmt.pix.height = %d\n",
                fmt.fmt.pix.width, fmt.fmt.pix.height);

    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (dec->mapType == LINEAR_FRAME_MAP) {
        if (dec->chromaInterleave == 0) {
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;
        } else {
            info_msg("Display: NV12\n");
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
        }
    }
    else {
        err_msg("Display cannot support mapType = %d\n", dec->mapType);
        goto err;
    }
    err = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if (err < 0) {
        err_msg("VIDIOC_S_FMT failed\n");
        goto err;
    }

    err = ioctl(fd, VIDIOC_G_FMT, &fmt);
    if (err < 0) {
        err_msg("VIDIOC_G_FMT failed\n");
        goto err;
    }

    reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    reqbuf.count = dec->regfbcount;

    err = ioctl(fd, VIDIOC_REQBUFS, &reqbuf);
    if (err < 0) {
        err_msg("VIDIOC_REQBUFS failed\n");
        goto err;
    }

    if (reqbuf.count < (__u32)dec->regfbcount) {
        err_msg("VIDIOC_REQBUFS: not enough buffers\n");
        goto err;
    }

    v4l_rsd = calloc(dec->regfbcount, sizeof(struct v4l_buf));
    if (v4l_rsd == NULL) {
        err_msg("calloc: not enough buffers\n");
        goto err;
    }
    disp->v4l_addr = v4l_rsd;
    buffer = &(disp->buf);
    for (i = 0; i < dec->regfbcount; i++) {
        struct v4l_buf *buf = v4l_rsd + i;

        buffer->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        buffer->memory = V4L2_MEMORY_MMAP;
        buffer->index = i;

        err = ioctl(fd, VIDIOC_QUERYBUF, buffer);
        if (err < 0) {
            err_msg("VIDIOC_QUERYBUF: not enough buffers\n");
            v4l_free_bufs(i, disp);
            goto err;
        }
        buf->start = mmap(NULL, buffer->length, PROT_READ | PROT_WRITE,
                MAP_SHARED, fd, buffer->m.offset);

        err = ioctl(fd, VIDIOC_QUERYBUF, buffer);
        if (err < 0) {
            err_msg("VIDIOC_QUERYBUF: not enough buffers\n");
            v4l_free_bufs(i, disp);
            goto err;
        }

        buf->offset = buffer->m.offset;
        buf->length = buffer->length;
        info_msg("V4L2buf phy addr: %08x, size = %d\n",
                    (unsigned int)buf->offset, buf->length);

        if (buf->start == MAP_FAILED) {
            err_msg("mmap failed\n");
            v4l_free_bufs(i, disp);
            goto err;
        }
    }

    disp->fd = fd;
    disp->nframes = dec->regfbcount;
    dec->v4ldisp = disp;

    return 0;
err:
    close(fd);
    free(disp);
    return -1;
}

void v4l_display_stop(vpu_display_t *disp)
{
    int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    if (disp)
    {
        if(disp->istart)
            ioctl(disp->fd, VIDIOC_STREAMOFF, &type);
        v4l_free_bufs(disp->nframes, disp);
        close(disp->fd);
        free(disp);
    }
}

int v4l_put_data(vpu_decode_t *dec, int index, int field)
{
    int err;
    struct timeval tv;
    struct v4l2_buffer *buffer;
    vpu_display_t *disp;

    disp = dec->v4ldisp;
    buffer = &(disp->buf);

    buffer->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    buffer->memory = V4L2_MEMORY_MMAP;

    /* query buffer info */
    buffer->index = index;
    err = ioctl(disp->fd, VIDIOC_QUERYBUF, buffer);
    if (err < 0) {
        err_msg("VIDIOC_QUERYBUF failed\n");
        goto err;
    }

    if (disp->ncount == 0) {
        gettimeofday(&tv, 0);
        buffer->timestamp.tv_sec = tv.tv_sec;
        buffer->timestamp.tv_usec = tv.tv_usec;

        disp->sec = tv.tv_sec;
        disp->usec = tv.tv_usec;
    }

    if (disp->ncount > 0) {
        if (dec->fps != 0) {
            disp->usec += (1000000 / dec->fps);
            if (disp->usec >= 1000000) {
                disp->sec += 1;
                disp->usec -= 1000000;
            }

            buffer->timestamp.tv_sec = disp->sec;
            buffer->timestamp.tv_usec = disp->usec;
        } else {
            gettimeofday(&tv, 0);
            buffer->timestamp.tv_sec = tv.tv_sec;
            buffer->timestamp.tv_usec = tv.tv_usec;
        }
    }

    buffer->index = index;
    buffer->field = field;

    err = ioctl(disp->fd, VIDIOC_QBUF, buffer);
    if (err < 0) {
        err_msg("VIDIOC_QBUF failed\n");
        goto err;
    }

    disp->ncount++;

    buffer->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    buffer->memory = V4L2_MEMORY_MMAP;
    buffer->index = index;
    err = ioctl(disp->fd, VIDIOC_DQBUF, buffer);
    if (err < 0) {
        err_msg("VIDIOC_DQBUF failed\n");
        goto err;
    }

    return 0;
err:
    return -1;
}

int v4l_display_start(vpu_display_t *disp)
{
//    int i;
    int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
//    struct v4l2_buffer buffer = {0};

//    buffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
//    buffer.memory = V4L2_MEMORY_MMAP;
//    for(i = 0; i < disp->nframes; ++i){
//        buffer.index  = i;
//        if(-1 == ioctl(disp->fd, VIDIOC_QBUF, &buffer)){
//            err_msg("VIDIOC_QBUF failed\n");
//            goto err;
//        }
//    }

    if(-1 == ioctl(disp->fd, VIDIOC_STREAMON, &type)){
        err_msg("VIDIOC_STREAMON failed\n");
        goto err;
    }
    disp->istart = 1;
    return 0;
err:
    return -1;
}
