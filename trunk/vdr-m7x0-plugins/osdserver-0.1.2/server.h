#ifndef __OSDSERVER_SERVER_H
#define __OSDSERVER_SERVER_H

#include <vdr/tools.h>
#include <vdr/thread.h>

#include "tools.h"
#include "interpreter.h"

class cServer;

class cTCPConnection : public cConnection {
    int fd;
    cServer *Server;

    static const int InitialBufferLength=256;
    char *buffer;
    int bufferLength;
    int bufferFill;
    int parsed;
    bool HasLine;


protected:
    virtual bool Send(const char *s, int length=-1);
    virtual char* ReadLine();
    virtual void DelLine();
    virtual void AddSelect(cSelect &Selector);


public:
    cTCPConnection();
    virtual ~cTCPConnection();

    bool Open(int filedesc, cServer *Server);
    void Close();

    virtual bool IsConnected() { return fd>0; }
};

class cServer : public cThread {
    class cServerSocket : public cOsdServerSocket {
        cServer *server;
    public:
        cServerSocket(cServer *Server, int Port, int Queue = 1) : cOsdServerSocket(Port, Queue), server(Server) { }
        virtual bool Accepted(in_addr_t Address);
    };
    friend class cServerSocket;
    
    cServerSocket socket;
    cList<cCommandProcessor> connections;
    
    bool HasOSDServerHosts;
    cSVDRPhosts OSDServerHosts;

protected:
    virtual void Action(void);

public:
    cServer(int Port, const char *Hosts);
    void Cancel(int WaitSeconds = 0) { cThread::Cancel(WaitSeconds); }

    virtual ~cServer();
};



#endif
