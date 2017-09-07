#ifndef __SCREEN_CB_H__
#define __SCREEN_CB_H__

#include <CCarLifeLibWrapper.h>

using namespace CommonUtilH;

//video callback
extern void videoDataReceive(u8 *, u32);
extern void videoHeartBeat(void);

//cmd Screen channel callback
extern void cmdVideoEncoderInitDone(S_VIDEO_ENCODER_INIT_DONE *);
extern void cmdVideoEncoderFrameRateChangeDone(S_VIDEO_ENCODER_FRAME_RATE_CHANGE_DONE *);
extern void cmdRegisterVideoEncoderJPEGAck(void); //0x00010057
extern void cmdScreenOn(void);
extern void cmdScreenOff(void);
extern void cmdScreenUserPresent(void);

#endif //__SCREEN_CB_H__