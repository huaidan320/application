#ifndef __MEDIA_CB_H__
#define __MEDIA_CB_H__

#include <CCarLifeLibWrapper.h>

using namespace CommonUtilH;

// Media channel callback
extern void mediaInit(S_AUDIO_INIT_PARAMETER *);
extern void mediaNormalData(u8 *, u32);
extern void mediaStop(void);
extern void mediaPause(void);
extern void mediaResume(void);
extern void mediaSeek(void);
extern void cmdMediaInfo(S_MEDIA_INFO *); //0x00010035
extern void cmdMediaProgressBar(S_MEDIA_PROGRESS_BAR *); //0x00010036

#endif //__MEDIA_CB_H__