#ifndef __VR_CB_H__
#define __VR_CB_H__

#include <CCarLifeLibWrapper.h>

using namespace CommonUtilH;

/* VR cmd channel callback */
extern void cmdMicRecordWakeupStart(void);
extern void cmdMicRecordEnd(void);
extern void cmdMicRecordRecogStart(void);

/* VR channel callback */
extern void vrInit(S_AUDIO_INIT_PARAMETER *);
extern void vrNormalData(u8 *, u32);
extern void vrStop(void);

#endif
