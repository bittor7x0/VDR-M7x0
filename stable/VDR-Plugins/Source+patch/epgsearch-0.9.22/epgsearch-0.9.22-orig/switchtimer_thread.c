#include "switchtimer_thread.h"
#include "epgsearchcfg.h"
#include "epgsearchtools.h"
#include <vdr/tools.h>
#include <vdr/plugin.h>

#define MSG_DELAY 5

cSwitchTimerThread *cSwitchTimerThread::m_Instance = NULL;

cSwitchTimerThread::cSwitchTimerThread(void)
: cThread("EPGSearch: switchtimer")
{
    m_Active = false;
    m_lastUpdate = time(NULL);
}

cSwitchTimerThread::~cSwitchTimerThread() {
    if (m_Active) 
	Stop();
}

void cSwitchTimerThread::Init(void) {
    if (m_Instance == NULL && SwitchTimers.Count() > 0) {
	m_Instance = new cSwitchTimerThread;
	m_Instance->Start();
    }
}

void cSwitchTimerThread::Exit(void) {
    if (m_Instance != NULL) {
	m_Instance->Stop();
	DELETENULL(m_Instance);
    }
}

void cSwitchTimerThread::Stop(void) {
    m_Active = false;
    Cancel(6);
}

void cSwitchTimerThread::Action(void) 
{
    m_Active = true;
    // let VDR do its startup
    sleepSec(20);
    time_t nextUpdate = time(NULL);
    while (m_Active) 
    {
	time_t now = time(NULL);
	if (now >= nextUpdate)
	{
	    LogFile.Log(3,"locking switch timers");
	    cMutexLock SwitchTimersLock(&SwitchTimers);
	    LogFile.Log(3,"switch timer check started");
	    cSwitchTimer* switchTimer = SwitchTimers.First();
	    while (switchTimer) 
	    {
		const cEvent* event = switchTimer->event;
		if (event && event->StartTime() - now < switchTimer->switchMinsBefore*60 + MSG_DELAY + 1)
		{
		    cChannel *channel = Channels.GetByChannelID(event->ChannelID(), true, true);
		    bool doswitch = (switchTimer->announceOnly == 0);
		    SwitchTimers.Del(switchTimer);
		    if (channel)
		    {
			// announce and switch
			if (doswitch)
			{
			    LogFile.Log(1,"switching to channel %d", channel->Number());
			    char* cmd = NULL;
			    asprintf(&cmd, "CHAN %d", channel->Number());
			    SendViaSVDRP(cmd);
			    free(cmd);
			}
			char* Message = NULL;
			asprintf(&Message, "%s: %s - %s", event->Title(), CHANNELNAME(channel), GETTIMESTRING(event));
			SendMsg(Message);
			free(Message);
			sleepSec(MSG_DELAY);			
		    }
		    SwitchTimers.Save();
		    break;
		}
		switchTimer = SwitchTimers.Next(switchTimer);
	    }
	    LogFile.Log(3,"switch timer check finished");
	    sleepSec(MSG_DELAY);
	    m_lastUpdate = time(NULL);
	    nextUpdate = long(m_lastUpdate/60)*60+ 60 - MSG_DELAY ; // check at each full minute minus 5sec
	    if (SwitchTimers.Count() == 0)
		m_Active = false;
	}
	while (m_Active && time(NULL)%MSG_DELAY != 0) // sync heart beat to MSG_DELAY
	    sleepSec(1);
	sleepSec(1);
    };
    m_Instance = NULL;
}

