#include <CCarLifeLibWrapper.h>
#include <CarlifeDebug.h>
#include <CarlifeCommonType.h>
#include <ScreenService.h>
#include <MessageService.h>
#include "SessionMgr.h"
#include "SessionCb.h"

using namespace CCarLifeLibH;

static S_HU_INFO huInfo = {
	"Linux",//os
	"board",//board
	"xxx",//bootloader
	"xxx",//brand
	"xxx",//cpu_abi
	"xxx",//cpu_abi2
	"xxx",//device
	"xxx",//dispaly
	"xxx",//fingerprint
	"xxx",//hardware
	"xxx",//host
	"xxx",//cid
	"xxx",//manufacturer
	"xxx",//model
	"xxx",//product
	"xxx",//serial
	"xxx",//codename
	"xxx",//incremental
	"xxx",//release
	"xxx",//sdk
	2,//sdk_int
	"",//token
	""
};

static S_STATISTICS_INFO statisticsInfo = {
	"carlife",//cuid
	"RADIO",//versionName
	1,//versionCode
	"20012100",//channel ID
	3,//connectCount
	3,//connectSuccessCount
	0,//ConnectTime
	"crash"//carshLog
};

static SESSION_STATE session_state = SESSION_STATE_NULL;
	
//cmd channel
void cmdProtocolVersionMatchStatus(S_PROTOCOL_VERSION_MATCH_SATUS *status)
{
	debug("cmdProtocolVersionMatchStatus() is invoked");
	debug("\tprotocol version match status: %d", status->matchStatus);

	if(-1 == CCarLifeLib::getInstance()->cmdStatisticInfo(&statisticsInfo))
	{
		log_err("cmdStatisticInfo send fail");
	}
	else
	{
		debug("cmdStatisticInfo send success");
	}

	if(-1 == CCarLifeLib::getInstance()->cmdHUInfro(&huInfo))
	{
		log_err("cmdHUInfo send fail");
	}
	else
	{
		debug("cmdHUInfo send success");
	}

	if(updateSessionState(SESSION_STATE_CONNECTED) != STATE_CHANGE_SUCCESS)
	{
		log_err("updateSessionState failed");	
	}
}

void cmdMDInfro(S_MD_INFO * mdInfro)
{
	cout<<"cmdMDInfro() is invoked"<<endl;
	cout<<"\tos: "<<mdInfro->os<<endl;
	cout<<"\tboard: "<<mdInfro->board<<endl;
	cout<<"\tbootloader: "<<mdInfro->bootloader<<endl;
	cout<<"\tbrand: "<<mdInfro->brand<<endl;
	cout<<"\tcpu_abi: "<<mdInfro->cpu_abi<<endl;
	cout<<"\tcpu_abi2: "<<mdInfro->cpu_abi2<<endl;
	cout<<"\tdevice: "<<mdInfro->device<<endl;
	cout<<"\tdisplay: "<<mdInfro->display<<endl;
	cout<<"\tfingerprint: "<<mdInfro->fingerprint<<endl;
	cout<<"\thardware: "<<mdInfro->hardware<<endl;
	cout<<"\thost: "<<mdInfro->host<<endl;
	cout<<"\tcid: "<<mdInfro->cid<<endl;
	cout<<"\tmanufacturer: "<<mdInfro->manufacturer<<endl;
	cout<<"\tmodel: "<<mdInfro->model<<endl;
	cout<<"\tproduct: "<<mdInfro->product<<endl;
	cout<<"\tserial: "<<mdInfro->serial<<endl;
	cout<<"\tcodename: "<<mdInfro->codename<<endl;
	cout<<"\tincremental: "<<mdInfro->incremental<<endl;
	cout<<"\trelease: "<<mdInfro->release<<endl;
	cout<<"\tsdk: "<<mdInfro->sdk<<endl;
	cout<<"\tsdk_int: "<<mdInfro->sdk_int<<endl;
}

void cmdForeground(void)
{
	cout<<"cmdForeground() is invoked"<<endl;
}

void cmdBackground(void)
{
	cout<<"cmdBackground() is invoked"<<endl;
}

void cmdGoToDeskTop(void)
{
	cout<<"cmdGoToDeskTop() is invoked"<<endl;
}

void cmdModuleStatus(S_MODULE_STATUS_LIST_MOBILE *)
{
	debug("cmdModuleStatus is invoked");
}

void cmdRegisterConnectException(S_CONNECTION_EXCEPTION *)
{
	cout<<"cmdRegisterConnectException is invoked"<<endl;	
}

void cmdRegisterRequestGoToForeground(void)
{
	cout<<"cmdRegisterRequestGoToForeground is invoked"<<endl;
}

void cmdRegisterUIActionSound(void)
{
	cout<<"cmdRegisterUIActionSound is invoked"<<endl;
}

void cmdRegisterMdAuthenResponse(S_AUTHEN_RESPONSE *response)
{
	cout<<"cmdRegisterMdAuthenResponse is invoked"<<endl;
	cout<<"encryptValue= "<<response->encryptValue<<endl;
}

void cmdRegisterFeatureConfigRequest(void)
{
	cout<<"cmdRegisterFeatureConfigRequest is invoked"<<endl;
}

void cmdRegisterMdExit(void)
{
	cout<<"cmdRegisterMdExit is invoked"<<endl;
}

STATE_CHANGE_RESULT updateSessionState(SESSION_STATE next_state)
{
    STATE_CHANGE_RESULT state_ret = STATE_CHANGE_SUCCESS;

    switch(next_state)
	{
        case SESSION_STATE_NULL:
            //Update the state variable so that subsequent read are all updated.
            //Since you are processing a state transition to NULL
            session_state = next_state;

			//TODO Reset all variables
			
            //Cleanup sockets and WDT
            cleanupCarlife();
            break;

        case SESSION_STATE_CONNECTING:
            if(session_state == SESSION_STATE_NULL)
			{
                session_state = next_state;
                //TODO Start timer for checking connection timeout
				//TODO start adapter.
				if(0 != startCarlifeConnection())
				{
					log_err("Carlife Connection start failed");
				}
                //TODO Start timer for checking md_adapter status
            }
			else
			{
                state_ret = STATE_CHANGE_FAILURE;
            }
            break;

        case SESSION_STATE_CONNECTED:
            if(session_state == SESSION_STATE_CONNECTING)
			{
                session_state = next_state;

				if(update_screen_state(SCREEN_STATE_INIT) != STATE_CHANGE_SUCCESS)
				{
					//TODO stop carlife
					log_err("update_screen_state failed");
				}

                //TODO call startcallback();

                //TODO Start timer for sending video heartbeat to MD

                //TODO stop Start timer

            } 
			else
			{
                state_ret = STATE_CHANGE_FAILURE;
			}
			
            break;
        default:
            state_ret = STATE_CHANGE_INVALID;
            break;
    }

    //TODO UNLOCK();
    return state_ret;
}