/*
 * VdrmonThread
 */
#include <string.h>
#include <vdr/plugin.h>
#include <vdr/thread.h>
#include "vdrmanagerthread.h"
#include "select.h"
#include "helpers.h"

cVdrManagerThread::cVdrManagerThread(int port,
#if VDRMANAGER_USE_SSL
                                     int sslPort,
#endif
                                     const char * password, bool forceCheckSvdrp,
                                     int compressionMode
#if VDRMANAGER_USE_SSL
                                     , const char * certFile, const char * keyFile
#endif
                                     )
{
  select = NULL;
  this->port = port;
  this->password = password;
  this->forceCheckSvdrp = forceCheckSvdrp;
  this->compressionMode = compressionMode;
#if VDRMANAGER_USE_SSL
  this->sslPort = sslPort;
  this->certFile = certFile;
  this->keyFile = keyFile;
#endif
}

cVdrManagerThread::~cVdrManagerThread()
{
  Cleanup();
}

void cVdrManagerThread::Action(void)
{
  // create listener socket
  if (!Init()) {
    Cleanup();
    return;
  }

  // do processing
  select->Action();

  // cleanup
  Cleanup();
}

bool cVdrManagerThread::Init()
{
  // create select
  select = new cSelect();
  if (select == NULL)
    return false;

  // create server socket
  cVdrmanagerServerSocket * sock = new cVdrmanagerServerSocket();
  if (sock == NULL || !sock->Create(port, password, forceCheckSvdrp, compressionMode, NULL, NULL))
    return false;

  // register server sockets
  select->SetServerSockets(sock, NULL);

#if VDRMANAGER_USE_SSL
  cVdrmanagerServerSocket * sslSock;
  if (!access(certFile, R_OK) && !access(keyFile, R_OK))  {
    sslSock = new cVdrmanagerServerSocket();
    if (sslSock == NULL || !sslSock->Create(sslPort, password, forceCheckSvdrp, compressionMode, certFile, keyFile)) {
      return false;
    }
  } else {
    sslSock = NULL;
    esyslog("[vdrmanager] SSL key files %s and %s can't be read. SSL disabled.", certFile, keyFile);
  }

  // register server sockets
  select->SetServerSockets(sock, sslSock);
#endif

  return true;
}

void cVdrManagerThread::Cleanup()
{
  if (select) {
    delete select;
    select = NULL;
  }
}

void cVdrManagerThread::Shutdown()
{
}
