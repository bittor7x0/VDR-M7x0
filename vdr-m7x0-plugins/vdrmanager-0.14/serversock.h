/*
 * extendes sockets
 */

#ifndef _VDRMON_SERVERSOCK
#define _VDRMON_SERVERSOCK

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

#include "clientsock.h"

#if VDRMANAGER_USE_SSL
#define SSL_NO_RETRY       0
#define SSL_RETRY_READ     1
#define SSL_RETRY_WRITE    2
#endif

using namespace std;

class cVdrmanagerServerSocket : public cVdrmanagerSocket
{
private:
  int port;
  SSL_CTX * sslCtx;
public:
  cVdrmanagerServerSocket();
  virtual ~cVdrmanagerServerSocket();
  bool Create(int port, const char * password, bool forceCheckSvdrp, int compressionMode, const char * certFile, const char * keyFile);
  cVdrmanagerClientSocket * Accept();
  int GetPort();
};

#endif
