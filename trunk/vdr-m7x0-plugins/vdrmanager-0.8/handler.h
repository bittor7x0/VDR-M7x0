/*
 * message and event handling
 */

#ifndef _VDRMON_HANDLER
#define _VDRMON_HANDLER


class cVdrmanagerSocket;
class cSelect;
class cHandler
{
public:
  bool HandleNewClient(cVdrmanagerClientSocket * sock);
  bool HandleVdrEvent(cVdrmanagerClientSocket * sock, string& msg);
  bool HandleClientRequest(cVdrmanagerClientSocket * sock);
  bool HandleNewClient(cSelect * select, cVdrmanagerClientSocket * sock);
};

#endif
