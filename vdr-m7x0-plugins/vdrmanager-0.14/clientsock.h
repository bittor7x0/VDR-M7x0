/*
 * extendes sockets
 */

#ifndef _VDRMON_CLIENTSOCK
#define _VDRMON_CLIENTSOCK

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#if VDRMANAGER_USE_SSL
#include <openssl/ssl.h>
#else
#define SSL_CTX void
#endif

#include <string>

#include "sock.h"

using namespace std;

class cVdrmanagerClientSocket : public cVdrmanagerSocket
{
private:
  string readbuf;
  string writebuf;
  char * sendbuf;
  size_t sendsize;
  size_t sendoffset;
  bool disconnected;
  bool initDisconnect;
  int client;
  bool login;
  bool compression;
  bool initCompression;
  int compressionMode;
#if VDRMANAGER_USE_SSL
  SSL * ssl;
  int sslReadWrite;
  int sslWantsSelect;
#endif
public:
  cVdrmanagerClientSocket(const char * password, int compressionMode);
  virtual ~cVdrmanagerClientSocket();
  bool Attach(int fd, SSL_CTX * sslCtx);
  bool IsLineComplete();
  bool GetLine(string& line);
  void Write(string line);
  bool Read();
  int ReadNoSSL();
  bool Flush();
  int FlushNoSSL();
#if VDRMANAGER_USE_SSL
  int ReadSSL();
  int FlushSSL();
  int GetSslReadWrite();
  int GetSslWantsSelect();
  bool IsSSL();
#endif
  bool Disconnected();
  void Disconnect();
  int GetClientId();
  bool WritePending();
  bool IsLoggedIn();
  void SetLoggedIn();
  void ActivateCompression();
#if VDRMANAGER_USE_GZIP || VDRMANAGER_USE_ZLIB
  void Compress();
#endif
};

#endif
