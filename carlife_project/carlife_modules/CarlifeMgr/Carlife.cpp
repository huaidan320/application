#include <CCarLifeLibWrapper.h>
#include <CarlifeDebug.h>

#include <CarlifeCommonType.h>
#include <SessionMgr.h>
#include <ScreenService.h>
#include <MediaService.h>
#include <NaviService.h>
#include <VrService.h>
#include <TelService.h>
#include <InputService.h>

#include "Carlife.h"

using namespace CCarLifeLibH;

RESULT_TYPE CarlifeInit(sCarlifeInfo *info, sCarlifeDelegate Delegate)
{
	CL_UNUSED(info);
	CL_UNUSED(Delegate);
	return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeStart(CompletionFunc callback)
{
	CL_UNUSED(callback);
	
	STATE_CHANGE_RESULT ret;
	
	ret = updateSessionState(SESSION_STATE_CONNECTING);
	if(ret != STATE_CHANGE_SUCCESS)
	{
		log_err("CarlifeStart failed");
		return RESULT_TYPE_FAILED;
	}

	return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeStop()
{
	return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeDstory()
{
	return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeLaunchNavi()
{
	return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeLaunchMeida()
{
	return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE carlifeLaunchTel()
{
	return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE CarlifeTouch(int x, int y, int action)
{
    /*
    S_TOUCH_ACTION_DOWN action_down;
    S_TOUCH_ACTION_UP action_up;

    if(action == 0)
    {
        action_down.x = x/1600 * 768;
        action_down.y = y;

        if(CCarLifeLib::getInstance()->ctrlTouchActionDown(&action_down) != 0)
        {
            log_err("ctrlTouchActionDown failed");
            return RESULT_TYPE_FAILED;
        }
    }
    else if(action == 1)
    {
        action_up.x = x/1600 * 768;
        action_up.y = y;

        if(CCarLifeLib::getInstance()->ctrlTouchActionUp(&action_up) != 0)
        {
            log_err("ctrlTouchActionUp failed");
            return RESULT_TYPE_FAILED;
        }
    }
    */
    S_TOUCH_SIGNAL_CLICK click;

    if(action == 0)
    {
    click.x = x * 768 / 1600;
    click.y = y;

    if(CCarLifeLib::getInstance()->ctrlTouchSigleClick(&click) != 0)
    {
        log_err("ctrlTouchActionUp failed");
        return RESULT_TYPE_FAILED;
    }
    }

	return RESULT_TYPE_SUCCESS;
}

#if 0
RESULT_TYPE CarlifeInputHk(KeyCode key)
{

}
#endif

RESULT_TYPE UpdateVideoFocus(SCREEN_FOCUS_TYPE type)
{
	CL_UNUSED(type);
	return RESULT_TYPE_SUCCESS;
}

RESULT_TYPE UpdateAudioFocus(AUDIO_TYPE audiotype, AUDIO_FOCUS_TYPE focustype)
{
	CL_UNUSED(audiotype);
	CL_UNUSED(focustype);
	return RESULT_TYPE_SUCCESS;
}
