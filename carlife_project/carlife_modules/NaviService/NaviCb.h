#ifndef __NAVI_CB_H__
#define __NAVI_CB_H__

#include <CCarLifeLibWrapper.h>

using namespace CommonUtilH;

//Navi TTS cmd channel
extern void cmdNaviNextTurnInfo(S_NAVI_NEXT_TURN_INFO *); //0x00010030

//Navi TTS channel
extern void ttsInit(S_AUDIO_INIT_PARAMETER *);
extern void ttsNormalData(u8 *, u32);
extern void ttsStop(void);

#endif //__NAVI_CB_H__