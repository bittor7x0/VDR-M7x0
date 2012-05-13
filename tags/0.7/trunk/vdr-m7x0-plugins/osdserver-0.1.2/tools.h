#ifndef __OSDSERVER_TOOLS_H
#define __OSDSERVER_TOOLS_H

#include <ctype.h>

#include <vdr/tools.h>
#include <vdr/remote.h>
#include <vdr/plugin.h>

class cOsdServerSocket {
    // based on cSocket from VDR svdrp.c
private:
    int port;
    int sock;
    int queue;
public:
    cOsdServerSocket(int Port, int Queue = 1);
    virtual ~cOsdServerSocket();
    bool Open(void);
    int Accept(void);
    virtual bool Accepted(in_addr_t Address);
    void Close(void);
    operator int() { return sock; }
};

class cSelect {
    int fdmax;
    fd_set readfds,writefds,exceptfds;
    timeval timeout;
public:
    void Reset() {
        fdmax=-1;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        timeout.tv_sec=LONG_MAX;
        timeout.tv_usec=0;
    }

    void SetRead(int fd) {
        if (fd >= 0 && fd < FD_SETSIZE) {
            FD_SET(fd,&readfds);
            fdmax=max(fd,fdmax);
        }
    }
    void SetWrite(int fd) {
        if (fd >= 0 && fd < FD_SETSIZE) {
            FD_SET(fd,&writefds);
            fdmax=max(fd,fdmax);
        }
    }
    void SetExcept(int fd) {
        if (fd >= 0 && fd < FD_SETSIZE) {
            FD_SET(fd,&exceptfds);
            fdmax=max(fd,fdmax);
        }
    }
    void ClearRead(int fd) {
        if (fd >= 0 && fd < FD_SETSIZE) FD_CLR(fd,&readfds);
    }
    void ClearWrite(int fd) {
        if (fd >= 0 && fd < FD_SETSIZE) FD_CLR(fd,&writefds);
    }
    void ClearExcept(int fd) {
        if (fd >= 0 && fd < FD_SETSIZE) FD_CLR(fd,&exceptfds);
    }
    bool GetRead(int fd) {
        return (fd >= 0 && fd < FD_SETSIZE) ? FD_ISSET(fd,&readfds) : false;
    }
    bool GetWrite(int fd) {
        return (fd >= 0 && fd < FD_SETSIZE) ? FD_ISSET(fd,&writefds) : false;
    }
    bool GetExcept(int fd) {
        return (fd >= 0 && fd < FD_SETSIZE) ? FD_ISSET(fd,&exceptfds) : false;
    }

    void SetTimeout(long s, long us) {
        timeout.tv_sec=s;
        timeout.tv_usec=us;
    }
    void SetMinTimeout(long s, long us) {
        if (s < timeout.tv_sec || (s == timeout.tv_sec && us < timeout.tv_usec)) {
            timeout.tv_sec=s;
            timeout.tv_usec=us;
        }
    }
    void SetTimeoutMs(long ms) { SetTimeout(ms/1000,(ms%1000)*1000); }
    void SetMinTimeoutMs(int ms) { SetMinTimeout(ms/1000,(ms%1000)*1000); }

    int Select() {
        return select(fdmax+1,&readfds,&writefds,&exceptfds,(timeout.tv_sec!=LONG_MAX)?(&timeout):(NULL));
    }

    cSelect() { Reset(); }
};

class cMainThreadLock {
public:
    class cCallback : public cListObject {
        cCondVar cond;
        int result;
    protected:
        virtual int Callback()=0;
    public:
        int CallCallback() { return cMainThreadLock::CallCallback(this); }

        friend class cMainThreadLock;
    };

private:
    static int Waiters;
    // # of waiting threads

    static bool WakeupSent;
    // Have we sent a keypress to the VDR main loop?
    static void SendWakeup() {
        if (WakeupSent) return;
        cRemote::Put(eKeys(kNone | k_Release));
        WakeupSent=true;
    }

    static cMutex mutex;
    // Master mutex for locking all static members

    // Signals
    static cCondVar MainThreadLocked;
    // This signal wakes sleeping threads while the main loop
    // is locked safely. Thread will be exclusive until it releases the lock.
    // Main thread won't continue until all threads are done.

    static cCondVar MainThreadContinue;
    // This signal wakes the main thread. The main thread will then process
    // callbacks (under lock) and will also check for new waiting threads

    // List of callbacks to-do
    class cCallbackList : public cList<cCallback> {
        public: virtual ~cCallbackList();
    };
    static cCallbackList Callbacks;


    // Non-static:

    bool HasLock;
    // Local thread has the main lock

    static int CallCallback(cCallback *Callback);
    // implements cCallback::CallCallback(), runs in thread.

public:
    cMainThreadLock();
    ~cMainThreadLock();

    static void MainThreadHook();

    void LockMainThread();
    void UnlockMainThread();

    friend class cCallback;
};




class cOpenOsd : protected cMutex {
public:
    class cCallback {
    protected:
        virtual cOsdObject* OpenOsd()=0;
    public:
        virtual ~cCallback() {}

        friend class cOpenOsd;
    };
private:
    cPlugin *Plugin;
    cCallback *Callback;
    time_t CallTime;
    cCondVar Signal;
public:
    cOpenOsd(cPlugin *Plugin);

    bool CallMainMenuAction(cCallback *Callback, int timeout=-1);
    // timeout<0: Call asynchronously
    // timeout=0: Wait indefinitely
    // timeout>0: Wait timeout ms.

    cOsdObject* MainMenuAction();
    void MainThreadHook();
};




static inline char* skipspace(char *p) {
    if (!p) return NULL;
    while (*p && isspace(*p)) p++;
    return p;
}

static inline bool isword(char c, bool first=false) {
    if (('a'<=c && c<='z') || ('A'<=c && c<='Z') || c=='_') return true;
    if ('0'<=c && c<='9') return !first;
    return false;
}

static inline char* skipword(char *p) {
    if (!p || *p==0 || !isword(*p,true)) return NULL;
    p++;
    while (*p && isword(*p)) p++;
    return p;
}

static inline void despace(char *p) {
    while (*p && !isspace(*p)) p++;
    char *q=p;
    while (*p!=0) {
        if (!isspace(*p)) *q++=*p;
        p++;
    }
    *q=0;
}




#endif
