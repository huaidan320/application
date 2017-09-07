#ifndef __TEL_CB_H__
#define __TEL_CB_H__

#include <CCarLifeLibWrapper.h>

using namespace CommonUtilH;

/* TEL cmd channel callback */
extern void cmdMDBTPairInfro(BTPairInfo *);
extern void cmdTelStateChangeIncoming(void);
extern void cmdTelStateChangeOutGoing(void);
extern void cmdTelStateChangeIdle(void);
extern void cmdTelStateChangeInCalling(void);

#endif //__TEL_CB_H__