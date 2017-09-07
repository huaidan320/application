#include <CCarLifeLibWrapper.h>
#include <CarlifeDebug.h>
#include "NaviService.h"
#include "NaviCb.h"

using namespace CCarLifeLibH;

//tts cmd channel callback functions
void cmdNaviNextTurnInfo(S_NAVI_NEXT_TURN_INFO *) ////0x00010030
{
	cout<<"cmdNaviNextTurnInfo is invoked"<<endl;
}

//tts channel callback functions
void ttsInit(S_AUDIO_INIT_PARAMETER *initParam)
{
	cout<<"ttsInit() is invoked"<<endl;
	cout<<"\tsampleRate: "<<initParam->sampleRate<<endl;
	cout<<"\tchannelConfig: "<<initParam->channelConfig<<endl;
	cout<<"\tsampleFormat: "<<initParam->sampleFormat<<endl;
}

void ttsNormalData(u8 *data, u32 len)
{
	CL_UNUSED(data);
	
	cout<<"ttsNormalData() is invoked"<<endl;
	cout<<"\treceive tts data: "<<len<<" bytes"<<endl;
}

void ttsStop(void)
{
	cout<<"ttsStop() is invoked"<<endl;
	cout<<"\ttts stop status received!"<<endl;
}
