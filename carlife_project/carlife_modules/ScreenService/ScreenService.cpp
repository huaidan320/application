#include <CCarLifeLibWrapper.h>
#include <CarlifeDebug.h>
#include <CarlifeCommonType.h>
#include "ScreenService.h"
#include "ScreenCb.h"

#include "vpudec_sdk.h"
#include "circle_buffer.h"

using namespace CCarLifeLibH;

static SCREEN_STATE screen_state = SCREEN_STATE_NULL;

static S_VIDEO_ENCODER_INIT initVideoParam = {768, 480, 30};

//video channel callback functions
void videoDataReceive(u8 *data, u32 len)
{	
	write_circle_buffer((char *)data, (int)len);
	
//	cout<<"videoDataReceive() is invoked"<<endl;
//	cout<<"\treceive video data: "<<len<<" bytes"<<endl;
}

void videoHeartBeat(void)
{
	cout<<"videoHeartBeat() is invoked"<<endl;
	cout<<"\tvideo heart beat received!";
}

void cmdVideoEncoderInitDone(S_VIDEO_ENCODER_INIT_DONE *videoEncoderInitDone)
{
	cout<<"cmdVideoEncoderInitDone() is invoked"<<endl;
	cout<<"\twidth: "<<videoEncoderInitDone->width<<endl;
	cout<<"\theight: "<<videoEncoderInitDone->height<<endl;
	cout<<"\tframeRate: "<<videoEncoderInitDone->frameRate<<endl;

    //TODO callbck size

	if(update_screen_state(SCREEN_STATE_READY) != STATE_CHANGE_SUCCESS)
	{
		log_err("update_screen_state failed");
	}
}

void cmdVideoEncoderFrameRateChangeDone(S_VIDEO_ENCODER_FRAME_RATE_CHANGE_DONE *
	videoEncoderFrameRateChangeDone)
{
	cout<<"cmdVideoEncoderFrameRateChangeDone() is invoked"<<endl;
	cout<<"\tframeRate: "<<videoEncoderFrameRateChangeDone->frameRate<<endl;
}

void cmdRegisterVideoEncoderJPEGAck(void)
{
	cout<<"cmdRegisterVideoEncoderJPEGAck is invoked"<<endl;
}

void cmdScreenOn(void)
{
	cout<<"cmdScreenOn() is invoked"<<endl;
}

void cmdScreenOff(void)
{
	cout<<"cmdScreenOff() is invoked"<<endl;
}

void cmdScreenUserPresent(void)
{
	cout<<"cmdScreenUserPresent() is invoked"<<endl;
}

static int screenServiceInit(void)
{
	int ret;
	
	ret = CCarLifeLib::getInstance()->cmdVideoEncoderInit(&initVideoParam);
	if(ret == -1)
	{
		log_err("cmdVideoEncoderInit Send failed");
		return ret;
	}
	else
	{
		debug("cmdVideoEncoderInit Send success");
	}

	return ret;
}

//user funcation
int ScreenServiceStart(void)
{
	int ret;
	
	ret = CCarLifeLib::getInstance()->cmdVideoEncoderStart();
	if(ret == -1)
	{
		log_err("cmdVideoEncoderStart Send failed");
		return ret;
	}
	else
	{
		debug("cmdVideoEncoderStart Send success");
	}
	
	//TODO set update_video_state(VIDEO_STATE_INIT)

	return ret;
}

STATE_CHANGE_RESULT update_screen_state(SCREEN_STATE next_state)
{
    STATE_CHANGE_RESULT state_ret = STATE_CHANGE_SUCCESS;

    switch (next_state)
	{
        case SCREEN_STATE_NULL:

            break;

        case SCREEN_STATE_INIT:
			if(screen_state == SCREEN_STATE_NULL)
			{
				screen_state = next_state;

				if(screenServiceInit() != 0)
				{
					//TODO: stop carlife
					log_err("screenServiceInit failed");
				}
			}
            break;

        case SCREEN_STATE_READY:
			if(screen_state == SCREEN_STATE_INIT)
			{
				screen_state = next_state;

				if(ScreenServiceStart() != 0)
				{
					//TODO: stop carlife
					log_err("ScreenServiceStart failed");
				}

				//vpu decoder
                start_vpu_decoder();
			}
            break;

        case SCREEN_STATE_FOREGROUND:

            break;

        case SCREEN_STATE_BACKGROUND:

            break;

		case SCREEN_STATE_ERROR:

			break;

        default:
            state_ret = STATE_CHANGE_INVALID;
            break;
    }

    //TODO UNLOCK();

    return state_ret;
}


