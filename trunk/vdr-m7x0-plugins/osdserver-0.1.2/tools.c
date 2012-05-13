

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdarg.h>

#include "server.h"


// --- cSocket ---------------------------------------------------------------
// based on cSocket from VDR svdrp.c

cOsdServerSocket::cOsdServerSocket(int Port, int Queue) {
    port = Port;
    sock = -1;
    queue = Queue;
}

cOsdServerSocket::~cOsdServerSocket() {
    Close();
}

void cOsdServerSocket::Close(void) {
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
}

bool cOsdServerSocket::Open(void) {
    if (sock < 0) {
        // create socket:
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            LOG_ERROR;
            port = 0;
            return false;
        }
        // allow it to always reuse the same port:
        int ReUseAddr = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ReUseAddr, sizeof(ReUseAddr));
        //
        struct sockaddr_in name;
        name.sin_family = AF_INET;
        name.sin_port = htons(port);
        name.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
            LOG_ERROR;
            Close();
            return false;
        }
        // make it non-blocking:
        int oldflags = fcntl(sock, F_GETFL, 0);
        if (oldflags < 0) {
            LOG_ERROR;
            return false;
        }
        oldflags |= O_NONBLOCK;
        if (fcntl(sock, F_SETFL, oldflags) < 0) {
            LOG_ERROR;
            return false;
        }
        // listen to the socket:
        if (listen(sock, queue) < 0) {
            LOG_ERROR;
            return false;
        }
    }
    return true;
}

bool cOsdServerSocket::Accepted(in_addr_t Address)
{
    return false;
    // return SVDRPhosts.Acceptable(Address);
}

int cOsdServerSocket::Accept(void) {
    if (Open()) {
        struct sockaddr_in clientname;
        uint size = sizeof(clientname);
        int newsock = accept(sock, (struct sockaddr *)&clientname, &size);
        if (newsock > 0) {
            bool accepted = Accepted(clientname.sin_addr.s_addr);
            if (!accepted) {
                const char *s = "400 Access denied!\r\n202 Good Bye.\r\n";
                if (write(newsock, s, strlen(s)) < 0)
                    LOG_ERROR;
                close(newsock);
                newsock = -1;
            }
            isyslog("connect from %s, port %hu - %s", inet_ntoa(clientname.sin_addr), ntohs(clientname.sin_port), accepted ? "accepted" : "DENIED");
        } else if (errno != EINTR && errno != EAGAIN) {
            LOG_ERROR;
        }
        return newsock;
    }
    return -1;
}



int cMainThreadLock::Waiters=0;
bool cMainThreadLock::WakeupSent=false;
cMutex cMainThreadLock::mutex;
cCondVar cMainThreadLock::MainThreadLocked;
cCondVar cMainThreadLock::MainThreadContinue;
cMainThreadLock::cCallbackList cMainThreadLock::Callbacks;


#define DEBUG_LOCKING

#ifdef DEBUG_LOCKING

tThreadId MainThreadId=cThread::ThreadId();
int LockCount=1;

inline bool ThreadHasLock() {
    return (MainThreadId==cThread::ThreadId());
}

inline void CheckThreads(char *file, unsigned int line) {
    if (!ThreadHasLock())
        printf("ERROR Thread not main (%s,%d)\n", file, line);

    if (LockCount<=0)
        printf("ERROR Thread lock %i (%s,%d)\n",LockCount, file, line);
}

inline void LockMain(char *file, unsigned int line) {
    if (MainThreadId==0) MainThreadId=cThread::ThreadId();
    LockCount++;
    CheckThreads(file,line);
}

inline void UnlockMain(char *file, unsigned int line) {
    CheckThreads(file,line);
    LockCount--;
    if (!LockCount) MainThreadId=0;
}

#define CHECK_THREADS CheckThreads(__FILE__,__LINE__)
#define THREAD_HAS_LOCK ThreadHasLock()
#define LOCK_MAIN LockMain(__FILE__,__LINE__)
#define UNLOCK_MAIN UnlockMain(__FILE__,__LINE__)

#else // ifdef DEBUG_LOCKING

#define CHECK_THREADS
#define THREAD_HAS_LOCK true
#define LOCK_MAIN
#define UNLOCK_MAIN

#endif



cMainThreadLock::cMainThreadLock() {
    HasLock=false;
}

cMainThreadLock::~cMainThreadLock() {
    UnlockMainThread();
}

cMainThreadLock::cCallbackList::~cCallbackList() {
    while (cCallback *c=First()) Del(c,false);
}


int cMainThreadLock::CallCallback(cCallback *Callback) {
    // Request calling the callback from main thread
    if (cThread::IsMainThread()) {
        // Identity crisis...
        // Just short-cut.
        return Callback->Callback();
    }

    // Get lock for static vars
    mutex.Lock();

    // Add to list of scheduled callbacks
    Callbacks.Add(Callback);

    // Let VDR main loop spin
    SendWakeup();
    // Maybe we're in the callback handler, so send a wakeup
    MainThreadContinue.Broadcast();
    // ... wait for the main thread to process the callback
    // (releases the lock)
    if (THREAD_HAS_LOCK) {
        UNLOCK_MAIN;
        Callback->cond.Wait(mutex);
        LOCK_MAIN;
    } else {
        Callback->cond.Wait(mutex);
    }
    // We're done
    mutex.Unlock();
    return Callback->result;
}

void cMainThreadLock::MainThreadHook() {
    // Callback function for the VDR main loop
    // Get lock for static vars
    CHECK_THREADS;
    mutex.Lock();
    CHECK_THREADS;
    // Wait() will temporarily release the lock.

    for (;;) {
        // Process listed callback requests
        while (cCallback *c = Callbacks.First()) {
            Callbacks.Del(c,false);
            // Call this callback object

            CHECK_THREADS;
            c->result=c->Callback();
            CHECK_THREADS;
            // and wake up the initiating thread
            c->cond.Broadcast();
        }

        // All waiting threads done?
        if (Waiters<=0) break;

        // Awake sleeping processes
        CHECK_THREADS;
        MainThreadLocked.Broadcast();

        // and wait for the last thread to send the wakeup signal
        // or for a waiting thread that requests a callback
        UNLOCK_MAIN;
        MainThreadContinue.Wait(mutex);
        LOCK_MAIN;

        // Either new callbacks have been added, or all threads are done.
    }
    // exit with break only, so Waiters<=0 - and it will stay that way, since we have lock.
    WakeupSent=false;
    CHECK_THREADS;
    mutex.Unlock();
    CHECK_THREADS;
}

void cMainThreadLock::LockMainThread() {
    // Lock the main thread sleeping
    if (cThread::IsMainThread()) {
        // Identity crisis...
        // Just short-cut.
        return;
    }

    // Only lock once
    if (HasLock) return;

    // Get lock for static vars
    mutex.Lock();

    // Add to the list of waiting threads
    Waiters++;

    // Let VDR main loop spin
    SendWakeup();

    // Wait for the notify of the main thread.
    MainThreadLocked.Wait(mutex);
    LOCK_MAIN;

    // Now we have main thread like exclusivity.

    HasLock=true;
}

void cMainThreadLock::UnlockMainThread() {
    // Release the lock of the main thread

    if (!HasLock) return;
    HasLock=false;

    // This thread does not wait any more
    Waiters--;
    if (Waiters==0) {
        // If we're the last, wake up main thread
        CHECK_THREADS;
        MainThreadContinue.Broadcast();
        CHECK_THREADS;
        // Note: In worst case, another thread is appended before
        // the main thread can continue. The main thread handles this.
    } else {
        // Other threads are waiting.
        // Most probably have received a broadcast, and wait for a lock.
        // But some may be added lately, and are still waiting, so send a wakeup.
        CHECK_THREADS;
        MainThreadLocked.Broadcast();
        CHECK_THREADS;
    }
    // We're done. Next please.
    UNLOCK_MAIN;
    mutex.Unlock();
}





cOpenOsd::cOpenOsd(cPlugin *plugin) {
    Plugin=plugin;
    Callback=NULL;
    CallTime=0;
}

bool cOpenOsd::CallMainMenuAction(cCallback *callback, int timeout) {
    cMutexLock Lock(this);

    if (Callback) return false;
    if (!cRemote::CallPlugin(Plugin->Name())) return false;

    Callback=callback;
    CallTime=time(NULL);

    if (timeout>0) {
        if (!Signal.TimedWait(*this,timeout)) {
            Callback=NULL;
            return false;
        }
    } else if (timeout==0) {
        Signal.Wait(*this);
    }

    return true;
}

cOsdObject* cOpenOsd::MainMenuAction() {
    cMutexLock Lock(this);

    if (!Callback) return NULL;

    cOsdObject *obj=Callback->OpenOsd();
    Callback=NULL;
    Signal.Broadcast();
    return obj;
}

void cOpenOsd::MainThreadHook() {
    if (!Callback) return; // quick exit
    cMutexLock Lock(this);
    if (!Callback) return;
    if (time(NULL)-CallTime <= 3) return;
    if (cRemote::CallPlugin(Plugin->Name())) {
        esyslog("Timeout on calling MainMenuAction");
        CallTime=time(NULL);
    }
}
