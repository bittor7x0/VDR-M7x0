#ifndef VDR_TIMER_THREAD_H
#define VDR_TIMER_THREAD_H

#include <vdr/thread.h>
#include "epgsearchext.h"
#include "log.h"

extern int gl_TimerProgged;

typedef enum
{
    TimerThreadReady,
    TimerThreadWorking,
    TimerThreadError,
    TimerThreadDone
} TimerThreadStatus;

class cTimerThread: public cThread {
private:
        static cTimerThread *m_Instance;
	char* m_cmd;
	static TimerThreadStatus m_Status;
protected:
        virtual void Action(void);
        void Stop(void);
public:
        bool m_Active;
	TimerThreadStatus GetStatus() { return cTimerThread::m_Status; }
	void SetStatus(TimerThreadStatus Status) { LogFile.eSysLog("%d", int(Status)); cTimerThread::m_Status = Status; }
        cTimerThread();
        virtual ~cTimerThread();
        void Init(char*);
        void Exit(void);
};

#endif 
