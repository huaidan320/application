#include <pthread.h>
#include <CCarLifeLibWrapper.h>
#include <CarlifeDebug.h>
#include <SessionCb.h>
#include <ScreenCb.h>
#include <MediaCb.h>
#include <NaviCb.h>
#include <VrCb.h>
#include <TelCb.h>
#include <InputCb.h>

#include "MessageService.h"

using namespace CCarLifeLibH;

enum CHANNEL_ID
{
	CHANNEL_CMD = 1,
	CHANNEL_VIDEO,
	CHANNEL_MEDIA,
	CHANNEL_TTS,
	CHANNEL_VR
};

static S_HU_PROTOCOL_VERSION huProtocolVersion={1, 0};

static pthread_t channelThreadId[5] = {0};

//thread
static void *cmdReceiveThread(void *arg);
static void *videoReceiveThread(void *arg);
static void *mediaReceiveThread(void *arg);
static void *ttsReceiveThread(void *arg);
static void *vrReceiveThread(void *arg);

static void *cmdReceiveThread(void *arg)
{
	CL_UNUSED(arg);
	
	//register callback

	/* SessionMgr */
	CCarLifeLib::getInstance()->cmdRegisterProtocolVersionMatchStatus(cmdProtocolVersionMatchStatus);
	CCarLifeLib::getInstance()->cmdRegisterMDInfro(cmdMDInfro);
	CCarLifeLib::getInstance()->cmdRegisterForeground(cmdForeground);
	CCarLifeLib::getInstance()->cmdRegisterBackground(cmdBackground);
	CCarLifeLib::getInstance()->cmdRegisterGoToDeskTop(cmdGoToDeskTop);
	CCarLifeLib::getInstance()->cmdRegisterModuleStatus(cmdModuleStatus);
	CCarLifeLib::getInstance()->cmdRegisterConnectException(cmdRegisterConnectException);
	CCarLifeLib::getInstance()->cmdRegisterRequestGoToForeground(cmdRegisterRequestGoToForeground);
	CCarLifeLib::getInstance()->cmdRegisterUIActionSound(cmdRegisterUIActionSound);
	CCarLifeLib::getInstance()->cmdRegisterMdAuthenResponse(cmdRegisterMdAuthenResponse);
	CCarLifeLib::getInstance()->cmdRegisterFeatureConfigRequest(cmdRegisterFeatureConfigRequest);
	CCarLifeLib::getInstance()->cmdRegisterMdExit(cmdRegisterMdExit);

	/* ScreenService */
	CCarLifeLib::getInstance()->cmdRegisterVideoEncoderInitDone(cmdVideoEncoderInitDone);
	CCarLifeLib::getInstance()->cmdRegisterVideoEncoderFrameRateChangeDone(cmdVideoEncoderFrameRateChangeDone);
	CCarLifeLib::getInstance()->cmdRegisterVideoEncoderJPEGAck(cmdRegisterVideoEncoderJPEGAck);
	CCarLifeLib::getInstance()->cmdRegisterScreenOn(cmdScreenOn);
	CCarLifeLib::getInstance()->cmdRegisterScreenOff(cmdScreenOff);
	CCarLifeLib::getInstance()->cmdRegisterScreenUserPresent(cmdScreenUserPresent);

	/* MediaService */
	CCarLifeLib::getInstance()->cmdRegisterMediaInfo(cmdMediaInfo);
	CCarLifeLib::getInstance()->cmdRegisterMediaProgressBar(cmdMediaProgressBar);

	/* NaviService */
	CCarLifeLib::getInstance()->cmdRegisterNaviNextTurnInfo(cmdNaviNextTurnInfo);

	/* VrService */
	CCarLifeLib::getInstance()->cmdRegisterMicRecordWakeupStart(cmdMicRecordWakeupStart);
	CCarLifeLib::getInstance()->cmdRegisterMicRecordEnd(cmdMicRecordEnd);
	CCarLifeLib::getInstance()->cmdRegisterMicRecordRecogStart(cmdMicRecordRecogStart);

	/* TelService */
	CCarLifeLib::getInstance()->cmdRegisterMDBTPairInfro(cmdMDBTPairInfro);
	CCarLifeLib::getInstance()->cmdRegisterTelStateChangeIncoming(cmdTelStateChangeIncoming);
	CCarLifeLib::getInstance()->cmdRegisterTelStateChangeOutGoing(cmdTelStateChangeOutGoing);
	CCarLifeLib::getInstance()->cmdRegisterTelStateChangeIdle(cmdTelStateChangeIdle);
	CCarLifeLib::getInstance()->cmdRegisterTelStateChangeInCalling(cmdTelStateChangeInCalling);

	/* InputService */
	CCarLifeLib::getInstance()->cmdRegisterCarDataSubscribe(cmdCarDataSubscribe);	
	CCarLifeLib::getInstance()->cmdRegisterCarDataSubscribeStart(cmdCarDataSubscribeStart);
	CCarLifeLib::getInstance()->cmdRegisterCarDataSubscribeStop(cmdCarDataSubscribeStop);

	while(1)
	{
		//receive data from command channel
		if(-1 == CCarLifeLib::getInstance()->cmdReceiveOperation())
		{
			log_err("command channel receive thread exit!");
			break;
		}
	}

	return NULL;
}

/* video channel receive thread */
static void *videoReceiveThread(void *arg)
{
	CL_UNUSED(arg);
	
	//register callback
	CCarLifeLib::getInstance()->videoRegisterDataReceive(videoDataReceive);
	CCarLifeLib::getInstance()->videoRegisterHeartBeat(videoHeartBeat);
	
	while(1)
	{
		if(-1 == CCarLifeLib::getInstance()->videoReceiveOperation())
		{
			log_err("video channel receive thread exit!");
			break;
		}
	}

	return NULL;	
}

/* media channel receive tread */
static void *mediaReceiveThread(void *arg)
{
	CL_UNUSED(arg);

	//register callback
	CCarLifeLib::getInstance()->mediaRegisterInit(mediaInit);
	CCarLifeLib::getInstance()->mediaRegisterNormalData(mediaNormalData);
	CCarLifeLib::getInstance()->mediaRegisterStop(mediaStop);
	CCarLifeLib::getInstance()->mediaRegisterPause(mediaPause);
	CCarLifeLib::getInstance()->mediaRegisterResume(mediaResume);
	CCarLifeLib::getInstance()->mediaRegisterSeek(mediaSeek);
	
	while(1)
	{
		if(-1==CCarLifeLib::getInstance()->mediaReceiveOperation())
		{
			log_err("media channel thread exit!");
			break;
		}
	}

	return NULL;
}

/* tts channel receive thread */
static void *ttsReceiveThread(void *arg)
{
	CL_UNUSED(arg);
	
	//register callback
	CCarLifeLib::getInstance()->ttsRegisterInit(ttsInit);
	CCarLifeLib::getInstance()->ttsRegisterNormalData(ttsNormalData);
	CCarLifeLib::getInstance()->ttsRegisterStop(ttsStop);
	
	while(1)
	{
		if(-1==CCarLifeLib::getInstance()->ttsReceiveOperation())
		{
			log_err("tts channel thread exit!");
			break;
		}
	}

	return NULL;
}

/* vr channel receive thread */
static void *vrReceiveThread(void *arg)
{
	CL_UNUSED(arg);

	//register callback
	CCarLifeLib::getInstance()->vrRegisterInit(vrInit);
	CCarLifeLib::getInstance()->vrRegisterNormalData(vrNormalData);
	CCarLifeLib::getInstance()->vrRegisterStop(vrStop);
	
	while(1)
	{
		if(-1==CCarLifeLib::getInstance()->vrReceiveOperation())
		{
			log_err("vr channel thread exit!");
			break;
		}
	}

	return NULL;
}

int startCarlifeConnection(void)
{
	int ret = 0;

    pthread_attr_t channel_thread_attr;

    pthread_attr_init(&channel_thread_attr);
    pthread_attr_setdetachstate(&channel_thread_attr, PTHREAD_CREATE_DETACHED);

    //Initialize CarLife library
    CCarLifeLib::getInstance()->carLifeLibInit();

	if(0 == CCarLifeLib::getInstance()->connectionSetup("127.0.0.1"))
	{
		debug("command/video/media/tts/vr/control channel connection have been set up!");
	}
	else
	{
		log_err("connection set up failed!");
		return -1;
	}

    //Start Cmd channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_CMD], &channel_thread_attr, cmdReceiveThread, NULL);
    if(ret != 0)
	{
        log_err("cmdReceiveThread create failed!");
        return -1;
    }
	else
	{
		debug("cmdReceiveThread create!");
	}

    //Start Video channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_VIDEO], &channel_thread_attr, videoReceiveThread, NULL);
    if(ret != 0) 
	{
        log_err("videoReceivThread create failed!");
        return -1;
    } 
	else 
	{
        debug("videoReceivThread create!");
    }

    //Start Media channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_MEDIA], &channel_thread_attr, mediaReceiveThread, NULL);
    if(ret != 0)
	{
        log_err("mediaReceivThread create failed!");
        return -1;
    } 
	else
	{
        debug("mediaReceivThread create!");
    }

    //Start TTS channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_TTS], &channel_thread_attr, ttsReceiveThread, NULL);
    if(ret != 0)
	{
        log_err("ttsReceivThread create failed!");
        return -1;
    } 
	else
    {
        debug("ttsReceivThread create!");
    }

    //Start VR channel thread
    ret = pthread_create(&channelThreadId[CHANNEL_VR], &channel_thread_attr, vrReceiveThread, NULL);
    if(ret != 0)
	{
        log_err("vrReceivThread create failed!");
        return -1;
    } 
	else
    {
        debug("vrReceivThread create!");
    }

    ret = CCarLifeLib::getInstance()->cmdHUProtoclVersion(&huProtocolVersion);
	if(ret != 0)
    {
		log_err("send cmdHUProtoclVersion failed");
		return -1;
    }
    else
    {
		debug("send cmdHUProtoclVersion success");
    }

    return ret;
}

int cleanupCarlife(void) 
{ 
	//kill adapter

	//cancel thread

    //Close all sockets
    CCarLifeLib::getInstance()->disconnect();
	
	CCarLifeLib::getInstance()->carLifeLibDestory();
	
    return 0;    
}