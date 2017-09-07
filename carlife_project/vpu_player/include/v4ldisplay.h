/*************************************************************************
    > File Name: vpudisplay.h
    > Author: CM
    > Mail:
    > Created Time: 2017年07月31日 星期一 17时57分51秒
    > Note: Used to VPU decodec display for v4l.
 ************************************************************************/

#ifndef V4LDISPLAY_H
#define V4LDISPLAY_H

#include "vpuplayer_config.h"

#ifdef __cplusplus
extern "C"{
#endif

extern int v4l_display_open(vpu_decode_t *dec);
extern int v4l_display_start(vpu_display_t *disp);
extern void v4l_display_stop(vpu_display_t *disp);
extern int v4l_put_data(vpu_decode_t *dec, int index, int field);


#ifdef __cplusplus
}
#endif
#endif // V4LDISPLAY_H
