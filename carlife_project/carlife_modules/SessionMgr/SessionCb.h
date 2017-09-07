#ifndef __SESSION_CB_H__
#define __SESSION_CB_H__

#include <CCarLifeLibWrapper.h>

using namespace CommonUtilH;

//cmd channel callback
extern void cmdProtocolVersionMatchStatus(S_PROTOCOL_VERSION_MATCH_SATUS *);
extern void cmdMDInfro(S_MD_INFO *);
extern void cmdForeground(void);
extern void cmdBackground(void);
extern void cmdGoToDeskTop(void);
extern void cmdModuleStatus(S_MODULE_STATUS_LIST_MOBILE *);
extern void cmdRegisterConnectException(S_CONNECTION_EXCEPTION *);
extern void cmdRegisterRequestGoToForeground(void);
extern void cmdRegisterUIActionSound(void);
extern void cmdRegisterMdAuthenResponse(S_AUTHEN_RESPONSE *);
extern void cmdRegisterFeatureConfigRequest(void);
extern void cmdRegisterMdExit(void);

#endif //__SESSION_CB_H__