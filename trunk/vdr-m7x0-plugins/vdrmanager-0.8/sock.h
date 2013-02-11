/*
 * extendes sockets
 */

#ifndef _VDRMON_SOCK
#define _VDRMON_SOCK

#include <sys/types.h>
#include <sys/socket.h>
#include <string>

using namespace std;

class cVdrmanagerSocket
{
protected:
  int sock;
  const char * password;
  bool forceCheckSvdrp;
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

class cVdrmanagerClientSocket : public cVdrmanagerSocket
{
private:
  string readbuf;
  string writebuf;
  bool disconnected;
  int client;
  bool login;
public:
  cVdrmanagerClientSocket(const char * password);
  virtual ~cVdrmanagerClientSocket();
  bool Attach(int fd);
  bool IsLineComplete();
  bool GetLine(string& line);
  bool PutLine(string line);
  bool Read();
  bool Disconnected();
  void Disconnect();
  bool Flush();
  int GetClientId();
  bool WritePending();
  bool IsLoggedIn();
  void SetLoggedIn();
};

class cVdrmanagerServerSocket : public cVdrmanagerSocket
{
public:
  cVdrmanagerServerSocket();
  virtual ~cVdrmanagerServerSocket();
  bool Create(int port, const char * password, bool forceCheckSvdrp);
  cVdrmanagerClientSocket * Accept();
};

#endif
