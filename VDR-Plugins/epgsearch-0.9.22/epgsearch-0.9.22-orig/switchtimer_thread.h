#ifndef VDR_SWITCHTIMER_THREAD_H
#define VDR_SWITCHTIMER_THREAD_H

#include "switchtimer.h"
#include <vdr/thread.h>

class cSwitchTimerThread: public cThread {
 private:
    bool m_Active;
    time_t m_lastUpdate;
 protected:
    virtual void Action(void);
    void Stop(void);
 public:
    static cSwitchTimerThread *m_Instance;

    cSwitchTimerThread(void);
    virtual ~cSwitchTimerThread();
    static void Init(void);
    static void Exit(void);
};

#endif 
