/*************************************************************************
    > File Name: vpuplayer_config.h
    > Author: CM
    > Mail:
    > Created Time: 2017年07月31日 星期一 17时57分51秒
    > Note:
 ************************************************************************/

#ifndef VPUPLAYER_CONFIG_H
#define VPUPLAYER_CONFIG_H
#include <stdio.h>
#ifdef __cplusplus
extern "C"{
#endif
#include "vpu_io.h"
#include "vpu_lib.h"
#include "linux/mxcfb.h"
#include "linux/input.h"
#include "linux/videodev2.h"


extern int vpu_player_dbg_level;

#define FMT_SENSE(condition, operation, fmt, arg...) \
    if(condition){  \
        dprintf(0, fmt, ##arg); \
        operation \
    }

#define dprintf(level, fmt, arg...)     if (vpu_player_dbg_level >= level) \
        printf("[DEBUG]\t%s:%d " fmt, __FILE__, __LINE__, ## arg)

#define err_msg(fmt, arg...) do { if (vpu_player_dbg_level >= 1)		\
    printf("[ERR]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
    printf("[ERR]\t" fmt, ## arg);	\
    } while (0)
#define info_msg(fmt, arg...) do { if (vpu_player_dbg_level >= 1)		\
    printf("[INFO]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
    printf("[INFO]\t" fmt, ## arg);	\
    } while (0)
#define warn_msg(fmt, arg...) do { if (vpu_player_dbg_level >= 1)		\
    printf("[WARN]\t%s:%d " fmt,  __FILE__, __LINE__, ## arg); else \
    printf("[WARN]\t" fmt, ## arg);	\
    } while (0)

#define DEBUG dprintf(3, "< %s >\n\n", __func__);


//#define __TEST_VPU_DEC__


struct FramePool
{
   FrameBuffer *fb;
   vpu_mem_desc mvdesc;
#ifdef __TEST_VPU_DEC__
   vpu_mem_desc desc;
#endif  //__TEST_VPU_DEC__
};

struct v4l_buf {
    void    *start;
    off_t   offset;
    size_t  length;
};

struct ipu_buf {
    int     ipu_paddr;
    void    * ipu_vaddr;
    int     field;
};

typedef struct{
    int fd;
    int nframes;    // = dec->regfbcount;
    int ncount;
    int istart;

    suseconds_t usec;
    time_t sec;
    struct v4l2_buffer buf;
    int frame_size;

    struct v4l_buf *v4l_addr;//array length = dec->regfbcount
    struct ipu_buf *ipu_bufs;
}vpu_display_t;

typedef struct decoder{
    DecHandle handle;
    vpu_mem_desc mem_desc;
//    PhysicalAddress phy_bsbuf_addr;
//    unsigned int virt_bsbuf_addr;
    vpu_mem_desc ps_mem_desc;
//    PhysicalAddress phy_ps_buf;
    vpu_mem_desc slice_mem_desc;
//    PhysicalAddress phy_slice_buf;

    int chromaInterleave;
    int deblock_en;
    int mapType;
    int tiled2LinearEnable;
    int regfbcount;
    int minfbcount;
    int picwidth;
    int picheight;
    int lastPicWidth;
    int lastPicHeight;
    Rect picCropRect;
    int stride;
    int fd;
    int dst_scheme;
    int src_scheme;
    int post_processing;
    int fps;
    int interlaced;
    CodStd format;
    struct FramePool *m_pPool;
    vpu_display_t *v4ldisp;
    vpu_display_t *ipudisp;

    double tdec_time;   //decodec elapse time
}vpu_decode_t;









#ifdef __cplusplus
}
#endif
#endif // VPUPLAYER_CONFIG_H

