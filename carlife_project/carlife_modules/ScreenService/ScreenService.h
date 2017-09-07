#ifndef SCREENSERVICE_H
#define SCREENSERVICE_H

#include <CarlifeCommonType.h>

typedef enum
{
    SCREEN_STATE_NULL,
    SCREEN_STATE_INIT,
    SCREEN_STATE_READY,
    SCREEN_STATE_FOREGROUND,
    SCREEN_STATE_BACKGROUND,
    SCREEN_STATE_ERROR,
    SCREEN_STATE_MAX
}SCREEN_STATE;

extern STATE_CHANGE_RESULT update_screen_state(SCREEN_STATE next_state);

#endif
