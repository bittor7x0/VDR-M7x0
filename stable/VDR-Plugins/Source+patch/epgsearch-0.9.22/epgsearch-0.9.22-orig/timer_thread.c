#include <string>
#include <list>
#include "timer_thread.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"
#include "services.h"
#include "svdrpclient.h"

#include <vdr/tools.h>
#include <vdr/plugin.h>

cTimerThread *cTimerThread::m_Instance = NULL;
TimerThreadStatus cTimerThread::m_Status = TimerThreadReady;
int gl_TimerProgged=0; // Flag that indicates, when programming is finished

cTimerThread::cTimerThread()
: cThread("EPGSearch: timer")
{
    m_Active = false;
    m_cmd = NULL;
}

cTimerThread::~cTimerThread() {
    if (m_Active) 
	Stop();
    if (m_cmd)
	free(m_cmd);
    cTimerThread::m_Instance = NULL;
}

void cTimerThread::Init(char* cmd) {
    if (m_Instance == NULL) {
	m_Instance = new cTimerThread;
	m_Instance->m_cmd = strdup(cmd);
	m_Instance->Start();
    }
}

void cTimerThread::Exit(void) {
    if (m_Instance != NULL) {
	m_Instance->Stop();
	DELETENULL(m_Instance);
    }

}

void cTimerThread::Stop(void) {
    m_Active = false;
    Cancel(3);
}

void cTimerThread::Action(void) 
{
    m_Active = true;
    if (EPGSearchConfig.useExternalSVDRP && !cSVDRPClient::SVDRPSendCmd)
    {
	LogFile.eSysLog("ERROR - SVDRPSend script not specified or does not exist (use -f option)");
	m_Active = false;
	return;
    }
    while (m_Active) 
    {
	if (Timers.BeingEdited()) 		
	{
	    sleepSec(1);
	    continue;
	}
	bool bSuccess = SendViaSVDRP(m_cmd);
	if (!bSuccess)
	{
	    Epgsearch_osdmessage_v1_0* service_data = new Epgsearch_osdmessage_v1_0;
	    service_data->message = strdup(tr("Programming timer failed!"));
	    service_data->type = mtError;
	    cPluginManager::CallFirstService("Epgsearch-osdmessage-v1.0", service_data);
	    delete service_data;
	}
	else
	    gl_TimerProgged = 1;
	m_Active = false;
    };
}


