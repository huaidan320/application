#include <CCarLifeLibWrapper.h>
#include <CarlifeDebug.h>
#include "MediaService.h"
#include "MediaCb.h"

using namespace CCarLifeLibH;

//media channel callback functions
void mediaInit(S_AUDIO_INIT_PARAMETER *initParam)
{
	cout<<"mediaInit() is invoked"<<endl;
	cout<<"\tsampleRate: "<<initParam->sampleRate<<endl;
	cout<<"\tchannelConfig: "<<initParam->channelConfig<<endl;
	cout<<"\tsampleFormat: "<<initParam->sampleFormat<<endl;
}

void mediaNormalData(u8 *data, u32 len)
{
	CL_UNUSED(data);
	
	cout<<"mediaNormalData() is invoked"<<endl;
	cout<<"\treceive media data: "<<len<<" bytes"<<endl;
}

void mediaStop(void)
{
	cout<<"mediaStop() is invoked"<<endl;
	cout<<"\tmedia stop status received!"<<endl;
}

void mediaPause(void)
{
	cout<<"mediaPause() is invoked"<<endl;
	cout<<" \tmedia pause status received!"<<endl;
}

void mediaResume(void)
{
	cout<<"mediaResume() is invoked"<<endl;
	cout<<"\tmedia resume status received!"<<endl;
}

void mediaSeek(void)
{
	cout<<"mediaSeek() is invoked"<<endl;
	cout<<"\tmedia seek status received!"<<endl;
}

//0x00010035
void cmdMediaInfo(S_MEDIA_INFO* info)
{
	cout<<"cmdMediaInfo is invoked"<<endl;
	cout<<"source= "<<info->source<<endl;
	cout<<"song= "<<info->song<<endl;
	cout<<"artist= "<<info->artist<<endl;
	cout<<"album= "<<info->album<<endl;
	cout<<"duration= "<<info->duration<<endl;
	cout<<"playlistNum= "<<info->playlistNum<<endl;
	cout<<"songId= "<<info->songId<<endl;
	cout<<"mode= "<<info->mode<<endl;
}

//0x00010036
void cmdMediaProgressBar(S_MEDIA_PROGRESS_BAR *)
{
	cout<<"cmdMediaProgressBar is invoked"<<endl;
}
