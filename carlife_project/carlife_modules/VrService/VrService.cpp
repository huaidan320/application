#include <CCarLifeLibWrapper.h>
#include <CarlifeDebug.h>
#include "VrService.h"
#include "VrCb.h"

using namespace CCarLifeLibH;

/* VR cmd channel callback */
void cmdMicRecordWakeupStart(void)
{
	cout<<"cmdMicRecordWakeupStart() is invoked"<<endl;
}
void cmdMicRecordEnd(void)
{
	cout<<"cmdMicRecordEnd() is invoked"<<endl;
}

void cmdMicRecordRecogStart(void)
{
	cout<<"cmdMicRecordRecogStart() is invoked"<<endl;
}

/* VR channel callback */
void vrInit(S_AUDIO_INIT_PARAMETER *initParam)
{
	cout<<"vrInit() is invoked"<<endl;
	cout<<"\tsampleRate: "<<initParam->sampleRate<<endl;
	cout<<"\tchannelConfig: "<<initParam->channelConfig<<endl;
	cout<<"\tsampleFormat: "<<initParam->sampleFormat<<endl;
}

void vrNormalData(u8 *data, u32 len)
{
	CL_UNUSED(data);
	
	cout<<"vrNormalData() is invoked"<<endl;
	cout<<"\treceive vr data: "<<len<<" bytes"<<endl;
}

void vrStop(void)
{
	cout<<"vrStop() is invoked"<<endl;
	cout<<"\tvr stop status received!"<<endl;
}
