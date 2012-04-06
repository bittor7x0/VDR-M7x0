

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <ctype.h>

#include <vdr/config.h>

#include "server.h"



cTCPConnection::cTCPConnection() {
    fd=0;
    Server=NULL;
    buffer=NULL;
    bufferLength=0;
    bufferFill=0;
    parsed=0;
    HasLine=false;
}

cTCPConnection::~cTCPConnection() {
    Close();
    if (buffer) free(buffer);
}


bool cTCPConnection::Open(int filedesc, cServer *server) {
    if (IsConnected()) return false;
    fd=filedesc;
    Server=server;

    // make fd non-blocking:
    int oldflags = fcntl(fd, F_GETFL, 0);
    if (oldflags < 0) {
        LOG_ERROR;
        Close();
        return false;
    }
    oldflags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, oldflags) < 0) {
        LOG_ERROR;
        Close();
        return false;
    }

    buffer=(char*)malloc(InitialBufferLength);
    if (!buffer) {
        Close();
        return false;
    }
    bufferLength=InitialBufferLength;

    return true;
}

void cTCPConnection::Close() {
    if (fd>0) close(fd);
    fd=0;
}

bool cTCPConnection::Send(const char *s, int length) {
    if (!IsConnected()) return false;
    if (length < 0)
        length = strlen(s);
    if (safe_write(fd, s, length) < 0) {
        LOG_ERROR;
        return false;
    }
    return true;
}

/*

Reply-Codes:

  1xx - Debugging
  2xx - End of reply
  3xx - Status messages
  4xx - Error codes
  5xx - Data return, quoted
  6xx - Data return, multi-line

*/




char* cTCPConnection::ReadLine() {
    if (HasLine) return buffer;

    while (parsed<bufferFill) {
        if (buffer[parsed]=='\n') {
            buffer[parsed]=0;
            HasLine=true;
            return buffer;
        }
        parsed++;
    }

    // need more data
    if (bufferLength==bufferFill) {
        bufferLength+=256;
        buffer=(char*)realloc(buffer,bufferLength);
    }
    int len=read(fd,buffer+bufferFill,bufferLength-bufferFill);
    if (len==0) {
        Close();
        return NULL;
    }
    if (len<0 && errno!=EAGAIN && errno!=EINTR) {
        LOG_ERROR;
        Close();
        return NULL;
    }
    if (len>0) bufferFill+=len;

    while (parsed<bufferFill) {
        if (buffer[parsed]=='\n') {
            buffer[parsed]=0;
            HasLine=true;
            return buffer;
        }
        parsed++;
    }

    // Still need more data
    return NULL;
}

void cTCPConnection::AddSelect(cSelect &Selector) {
    Selector.SetRead(fd);
}


void cTCPConnection::DelLine() {
    if (!HasLine) return;

    memmove(buffer,buffer+parsed+1,bufferFill-parsed-1);
    bufferFill=bufferFill-parsed-1;
    parsed=0;
    HasLine=false;
}



cServer::cServer(int Port, const char *Hosts)
    : cThread("osdserver"),
      socket(this, Port,3) {

    HasOSDServerHosts = (Hosts != NULL);
    
    if (HasOSDServerHosts) {
        OSDServerHosts.Load(Hosts, true, true);
    }
}

bool cServer::cServerSocket::Accepted(in_addr_t Address)
{
    if (!server) return false;
    if (server->HasOSDServerHosts)
        return server->OSDServerHosts.Acceptable(Address);
    else   
        return SVDRPhosts.Acceptable(Address);
}

void cServer::Action() {
    if (!socket.Open()) return;

    while (Running()) {
        cSelect Selector;

        Selector.SetTimeoutMs(1000);
        Selector.SetRead(socket);

        int fd=socket.Accept();
        if (fd>0) {
            cTCPConnection *c=new cTCPConnection();
            if (c && !c->Open(fd,this)) {
                delete c;
                c=NULL;
            }
            if (c) {
                cCommandProcessor *p=new cCommandProcessor(c);
                if (p) {
                    connections.Add(p);
                } else {
                    delete c;
                }
            }
        }

        for (cCommandProcessor *p = connections.First(); p; ) {
            cCommandProcessor *next=connections.Next(p);

            if (!p->Action(Selector)) connections.Del(p);

            p=next;
        }

        int cnt=Selector.Select();
        if (cnt < 0) {
            if (errno!=EINTR) LOG_ERROR;
        }
    }

    socket.Close();
}

cServer::~cServer() {
}
