/*
 * extendes sockets
 */

#ifndef _VDRMON_SOCK
#define _VDRMON_SOCK

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

#if VDRMANAGER_USE_SSL
#define SSL_NO_RETRY       0
#define SSL_RETRY_READ     1
#define SSL_RETRY_WRITE    2
#endif

using namespace std;

class cVdrmanagerSocket
{
protected:
  int sock;
  const char * password;
  bool forceCheckSvdrp;
  int compressionMode;
protected:
  cVdrmanagerSocket();
  bool IsPasswordSet();
public:
  virtual ~cVdrmanagerSocket();
  void Close();
  int GetSocket();
  bool MakeDontBlock();
  const char * GetPassword();
};

#endif
