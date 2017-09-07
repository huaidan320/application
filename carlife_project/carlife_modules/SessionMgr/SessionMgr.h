#ifndef SESSIONMGR_H
#define SESSIONMGR_H

#include <CarlifeCommonType.h>

typedef enum
{
    SESSION_STATE_NULL,
    SESSION_STATE_CONNECTING,
    SESSION_STATE_CONNECTED
}SESSION_STATE;

extern STATE_CHANGE_RESULT updateSessionState(SESSION_STATE);

#endif
