/*
 * encapsulated poll
 */

#ifndef _VDRMON_SELECT
#define _VDRMON_SELECT

#include <string>
#include <sys/select.h>
#include "sock.h"
#include "handler.h"

struct node;
class cSelect
{
private:
  node * clientsockets;
  int clientsocketcount;
  cVdrmanagerServerSocket * serversocket;
  cHandler * handler;
  struct pollfd * pollfds;
  bool stopped;
  time_t nexttimer;
  time_t shutdown;
public:
  cSelect();
  virtual ~cSelect();
  void DispatchVdrEvent(string event);
  void SetServerSocket(cVdrmanagerServerSocket * sock);
  void AddClientSocket(cVdrmanagerClientSocket * sock);
  void RemoveClientSocket(cVdrmanagerClientSocket * sock);
  bool Action();
  bool Stop();
private:
  void CreatePollfds();
  cVdrmanagerClientSocket * GetClientSocket(int fd);
  bool Poll();
  void NotifyClients(string event);
};

#endif
