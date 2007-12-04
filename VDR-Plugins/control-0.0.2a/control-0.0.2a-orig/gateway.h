/*
 * gateway.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */


#ifndef _CTRLGATEWAY_H_
#define _CTRLGATEWAY_H_

#include <termios.h>

#include <vdr/thread.h>

#include "telnet.h"

class cCtrlTelnet;
class cCtrlFormatter;



class cCtrlGateway
:     public cThread,
      public cListObject
{

private:

  virtual void Action();

public:

  cCtrlGateway(int socket);
  ~cCtrlGateway();

  bool SendMsg(const char* Msg);

  void Close();

private:

  volatile bool    _active;
  bool             _terminating;
  int              _hPipe;
  cCtrlStack       _stack;
  cCtrlTelnet*     _pTelnet;
  cCtrlFormatter*  _pFormatter;
};



class cCtrlGatewayCol : public cList<cCtrlGateway> {};

extern cCtrlGatewayCol gl_CtrlGatewayCol;


#endif // _CTRLGATEWAY_H_


