#include <CCarLifeLibWrapper.h>
#include <CarlifeDebug.h>
#include "InputService.h"
#include "InputCb.h"

using namespace CCarLifeLibH;

typedef struct 
{
   float xScale;
   float yScale;
}touchScaleTransform;

touchScaleTransform scaleFactor = {1.0, 1.0};

void cmdCarDataSubscribe(S_VEHICLE_INFO_LIST *)
{
	cout<<"cmdCarDataSubscribe is invoked"<<endl;
}

void cmdCarDataSubscribeStart(S_VEHICLE_INFO_LIST *)
{	
	cout<<"cmdCarDataSubscribeStart is invoked"<<endl;
}

void cmdCarDataSubscribeStop(S_VEHICLE_INFO_LIST *)
{
	cout<<"cmdCarDataSubscribeStop is invoked"<<endl;
}

