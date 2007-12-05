#ifndef VDR_CONFLICTCHECK_THREAD_H
#define VDR_CONFLICTCHECK_THREAD_H

#include <vdr/thread.h>
#include "conflictcheck.h"
#include "epgsearch.h"

class cConflictCheckThread: public cThread {
 private:
    bool m_Active;
    time_t m_lastUpdate;
    cPluginEpgsearch* m_plugin;
    static bool m_runOnce; 
    static bool m_forceUpdate;
 protected:
    virtual void Action(void);
    void Stop(void);
 public:
    static cConflictCheckThread *m_Instance;
    static time_t m_cacheNextConflict;
    static int m_cacheRelevantConflicts;
    static int m_cacheTotalConflicts;
    cConflictCheckThread(cPluginEpgsearch* thePlugin);
    virtual ~cConflictCheckThread();
    static void Init(cPluginEpgsearch* thePlugin, bool runOnce = false);
    static void Exit(void);
};

#endif 
