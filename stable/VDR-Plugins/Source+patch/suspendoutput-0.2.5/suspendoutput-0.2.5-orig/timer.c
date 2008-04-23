/*
 * timer.c: Threaded timer class
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <sys/time.h>

#include <vdr/config.h>
#include <vdr/tools.h>
#include <vdr/thread.h>

#include "timer.h"

//#define XINELIBOUTPUT_DEBUG
//#define XINELIBOUTPUT_DEBUG_STDOUT
#ifdef XINELIBOUTPUT_DEBUG
#  include "logdefs.h"
#else
#  define TRACE(x)
#  define TRACEF(x)
#endif

#if VDRVERSNUM>10317
  #define time_ms() cTimeMs::Now()
#endif

// ---------------------------- cTimerThreadEvent ----------------------------

class cTimerThreadEvent : public cListObject {
  public:
    cTimerThreadEvent(cTimerCallback *Handler, unsigned int TimeoutMs, 
		      bool DeleteOnCancel = false) :
       m_Handler(Handler), 
       m_DeleteOnCancel(DeleteOnCancel), 
       m_TimeoutMs(TimeoutMs)
    {
      m_NextEventTime = time_ms();
      UpdateEventTime();
    }

    ~cTimerThreadEvent()
    {
      if(m_DeleteOnCancel && m_Handler)
        delete m_Handler;
    }

    void UpdateEventTime()
    {
      m_NextEventTime += m_TimeoutMs;
    }

    int TimeToNextEvent(void)
    {
      return m_NextEventTime - time_ms();
    }

    virtual bool operator< (const cListObject &ListObject)
    {
      const cTimerThreadEvent *o = (cTimerThreadEvent *)&ListObject;
      return m_NextEventTime<o->m_NextEventTime;
    }

    virtual int Compare(const cListObject &ListObject) const
    {
      const cTimerThreadEvent *o = (cTimerThreadEvent *)&ListObject;
      if(m_NextEventTime<o->m_NextEventTime)
	return -1;
      else if(m_NextEventTime>o->m_NextEventTime)
	return 1;
      return 0;
    }

    cTimerCallback *m_Handler;

  protected:
    bool          m_DeleteOnCancel;
    unsigned int  m_TimeoutMs;
    int64_t       m_NextEventTime;
};

// ------------------------------- cTimerThread ------------------------------

class cTimerThread : public cThread {
  private:
    cTimerThread(cTimerThread&); // copy not allowed

    static cMutex        m_InstanceLock;
    static cTimerThread *m_Instance;  // singleton
  
    cMutex                m_Lock;
    cCondVar              m_Signal;
    cList<cTimerThreadEvent> m_Events;
    cTimerThreadEvent     *m_RunningEvent;
    bool                  m_Finished;
    bool                  m_HandlerRunning;

    cTimerThread() : 
      m_RunningEvent(NULL), 
      m_Finished(false), 
      m_HandlerRunning(false) 
    {
    }

    virtual ~cTimerThread() 
    {
      m_Lock.Lock();
      cTimerThreadEvent *ev;
      while(NULL != (ev = m_Events.First())) {
        m_Events.Del(ev,true);
      }
      m_Lock.Unlock();
      m_Signal.Broadcast();
      Cancel(1);
    }

  protected:

    virtual void Action() 
    {
      TRACEF("cTimerThread::Action");
      m_Lock.Lock();
      while(m_Events.First()) {
        m_Signal.TimedWait(m_Lock, 
			   max(1, m_Events.First()->TimeToNextEvent()));
        TRACE("cTimerThread::Action waked up");
        while(NULL != (m_RunningEvent = m_Events.First()) && 
	      m_RunningEvent->TimeToNextEvent() <= 0) {
          TRACE("cTimerThread::Action calling handler");
          m_HandlerRunning=true;
//        m_Lock.Unlock();   
//        - can't unlock or running timer handler may be deleted while
//          executing (or thread may be killed by Delete)
          bool result = m_RunningEvent->m_Handler->TimerEvent();
//        m_Lock.Lock();
          m_HandlerRunning=false;
          if(!result) {
            if(m_RunningEvent) { // check if event was cancelled in handler...
              TRACE("cTimerThread::Action handler cancelled timer");
              m_Events.Del(m_RunningEvent, true);
            }
          } else {
            if(m_RunningEvent) {
              TRACE("cTimerThread::Action timer re-scheduled");
              m_RunningEvent->UpdateEventTime();
              m_Events.Sort();
            }
          }
          m_RunningEvent = NULL;
        }
      }
      m_Finished = true;
      m_Lock.Unlock();
    }

    void Add(cTimerThreadEvent *Event)
    {
      TRACEF("cTimerThread::Add");
      //m_Events.Del(Event, false);
      Event->Unlink();
      Del(Event->m_Handler);
      m_Events.Add(Event);
      m_Events.Sort();
    }

    bool Del(cTimerCallback *Handler, void *TargetId=NULL, 
	     bool inDestructor=false)
    {
      TRACEF("cTimerThread::Del");
      cTimerThreadEvent *ev = m_Events.First();
      while(ev) {
        if(ev->m_Handler == Handler || 
	   (TargetId && ev->m_Handler->TargetId() == TargetId) ||
           (Handler && ev->m_Handler->is(Handler,Handler->size()))) {
          cTimerThreadEvent *nev = m_Events.Next(ev);
          if(inDestructor) ev->m_Handler=NULL;
          m_Events.Del(ev, true);
          ev = nev;
        } else
          ev = m_Events.Next(ev);
      }
      if(m_RunningEvent && 
	 (m_RunningEvent->m_Handler == Handler || 
	  m_RunningEvent->m_Handler->TargetId() == TargetId))
        m_RunningEvent = NULL;
      return !m_HandlerRunning && !m_RunningEvent && !m_Events.First();
    }

  public:

    static void AddEvent(cTimerCallback *Handler, unsigned int TimeoutMs, 
			 bool DeleteOnCancel=false)
    {
      TRACEF("cTimerThread::AddEvent");
      m_InstanceLock.Lock();
      if(m_Instance && m_Instance->m_Finished) {
          delete m_Instance;
          m_Instance = NULL;
      }
      if(!m_Instance) {
        m_Instance = new cTimerThread;
        m_Instance->m_Lock.Lock();
        m_Instance->Start();
      } else {
        m_Instance->m_Lock.Lock();
        m_Instance->m_Signal.Broadcast();
      }
      m_Instance->Add(new cTimerThreadEvent(Handler, max(1U,TimeoutMs), 
					    DeleteOnCancel));
      m_Instance->m_Lock.Unlock();
      m_InstanceLock.Unlock();
    }

    static void CancelEvent(cTimerCallback *Handler, void *TargetId = NULL, 
			    bool inDestructor=false) 
    {    
      TRACEF("cTimerThread::CancelEvent");
      m_InstanceLock.Lock();
      if(m_Instance && !m_Instance->m_Finished) {
        m_Instance->m_Lock.Lock();
        if(m_Instance->Del(Handler, TargetId, inDestructor) && !inDestructor) {
          m_Instance->m_Lock.Unlock();
          delete m_Instance;
          m_Instance = NULL;
        } else
          m_Instance->m_Lock.Unlock();
      }
      m_InstanceLock.Unlock();
    }

};

cMutex        cTimerThread::m_InstanceLock;
cTimerThread *cTimerThread::m_Instance = NULL;

// ------------------------------ cTimerCallback -----------------------------

cTimerCallback::~cTimerCallback()
{
  TRACEF("cTimerCallback::~cTimerCallback");
  cTimerThread::CancelEvent(this, NULL, true);
}

void cTimerCallback::Set(cTimerCallback *handler, unsigned int TimeoutMs)
{
  TRACEF("cTimerCallback::Set");
  cTimerThread::AddEvent(handler, TimeoutMs);
}

void cTimerCallback::Cancel(cTimerCallback *handler)
{
  TRACEF("cTimerCallback::Cancel");
  cTimerThread::CancelEvent(handler);
}

// ------------------------------- cTimerEvent -------------------------------

//cTimerEvent::cTimerEvent(unsigned int TimeoutMs)
//{
//  TRACEF("cTimerEvent::cTimerEvent");
////  cTimerThread::AddEvent(this, TimeoutMs, true);
//}

void cTimerEvent::AddEvent(unsigned int TimeoutMs)
{
  TRACEF("cTimerEvent::AddEvent");
  cTimerThread::AddEvent(this, TimeoutMs, true);
}

void cTimerEvent::Cancel(cTimerEvent *&event)
{
  TRACEF("cTimerEvent::Cancel");
  cTimerThread::CancelEvent(event);
  event = NULL;
}

void cTimerEvent::CancelAll(void *Target)
{
  TRACEF("cTimerEvent::CancelAll");
  cTimerThread::CancelEvent(NULL, Target);
}







