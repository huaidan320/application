#include <CCarLifeLibWrapper.h>
#include "TelService.h"
#include "TelCb.h"

using namespace CCarLifeLibH;

/* TEL cmd channel callback */
void cmdMDBTPairInfro(BTPairInfo* info)
{
	cout<<"cmdMDBTPairInfro() is invoked"<<endl;
	cout<<"/taddress: "<<info->address<<endl;
	cout<<"/tpassKey: "<<info->passKey<<endl;
	cout<<"/thash: "<<info->hash<<endl;
	cout<<"/trandomizer: "<<info->randomizer<<endl;
	cout<<"/tuuid: "<<info->uuid<<endl;
	cout<<"/tname: "<<info->name<<endl;
	cout<<"/tstatus: "<<info->status<<endl;
}

void cmdTelStateChangeIncoming(void)
{
	cout<<"cmdTelStateChangeIncoming() is invoked"<<endl;
}

void cmdTelStateChangeOutGoing(void)
{
	cout<<"cmdTelStateChangeOutGoing() is invoked"<<endl;
}

void cmdTelStateChangeIdle(void)
{
	cout<<"cmdTelStateChangeIdle() is invoked"<<endl;
}
void cmdTelStateChangeInCalling(void)
{
	cout<<"cmdTelStateChangeInCalling() is invoked"<<endl;
}
