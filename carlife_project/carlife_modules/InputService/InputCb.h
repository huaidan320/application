#ifndef __INPUT_CB_H__
#define __INPUT_CB_H__

#include <CCarLifeLibWrapper.h>

using namespace CommonUtilH;

extern void cmdCarDataSubscribe(S_VEHICLE_INFO_LIST *);	
extern void cmdCarDataSubscribeStart(S_VEHICLE_INFO_LIST *);
extern void cmdCarDataSubscribeStop(S_VEHICLE_INFO_LIST *);

#endif //__INPUT_CB_H__