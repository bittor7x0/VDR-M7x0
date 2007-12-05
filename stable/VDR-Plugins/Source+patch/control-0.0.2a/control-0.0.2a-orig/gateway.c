/*
 * gateway.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE


#include <sys/socket.h>

#include "global.h"
#include "state.h"
#include "gateway.h"
#include "formatter.h"
#include "keyboard.h"



cCtrlGatewayCol gl_CtrlGatewayCol;



cCtrlGateway::cCtrlGateway(int socket)
:
#if VDRVERSNUM >= 10300
  cThread	(0),
#endif
  _active	(false),
  _terminating	(false),
  _hPipe	(socket),
  _pTelnet	(0),
  _pFormatter   (0)
{
  gl_CtrlGatewayCol.Add(this);

//  if (! cKbdRemoteForControl::_pRemote)
//    cKbdRemoteForControl::_pRemote = new cKbdRemoteForControl();

  if (1) {

    cCtrlSocket* pSocket = new cCtrlSocket(socket);
    _stack.push(pSocket);
    cCtrlTelnet* pTelnet = new cCtrlTelnet();
    _stack.push(pTelnet);

    // Params: option, local, activate
    pTelnet->useOption(OPTION_ECHO, true,  true);
    pTelnet->useOption(OPTION_SGA,  true,  true);
    pTelnet->useOption(OPTION_SGA,  false, true);
    pTelnet->useOption(OPTION_NAWS, false, true);

    _pTelnet = pTelnet;

  } else {
    cCtrlTty* pSocket = new cCtrlTty(socket);
    _stack.push(pSocket);
  }

//  printf("term=%s\n", getenv("TERM")); fflush(stdout);
}



cCtrlGateway::~cCtrlGateway() {
  _terminating = true;
  _active = false;
  Cancel(200);
  Close();
}



#define cBREAK  3
#define cEOF   26
#define cESC   27



void cCtrlGateway::Action() {

  conDSYSLOG("gateway thread started (pid=%d)", getpid());
  do {

conLOG_GATEW("starting stack");
  _stack.start();

conLOG_GATEW("send greeting");
  static const unsigned char msg[] = "\033[?25lHello, foreign. I'm the keyboard handler of VDR\n\r";
  _stack.sendData(msg, sizeof(msg)-1);

  _pFormatter = new cCtrlFormatter(this);

  _active = true;
  while (_active) {

    if (_stack.poll(100)) {

      uint64 Command = 0;
      uint i = 0;
      int t0 = time_ms();
      while (_active && i < sizeof(Command)) {

        uchar ch;
        int r = 0;
        if (_stack.poll())
          r = _stack.receiveData(&ch, 1);

        if (_pTelnet)
          _pFormatter->SetSize(_pTelnet->width(), _pTelnet->height());

        if (r == 1) {
conLOG_GATEW("key received (%2x, %i)", ch, i);

          if (ch == 0 || ch == cBREAK || ch == cEOF) {
            Close();
            _active = false;
            break;
          }
          Command <<= 8;
          Command |= ch;
          i++;
        } else if (r == 0) {
//conLOG_GATEW("r == 0");
          // don't know why, but sometimes special keys that start with
          // 0x1B ('ESC') cause a short gap between the 0x1B and the rest
          // of their codes, so we'll need to wait some 100ms to see if
          // there is more coming up - or whether this really is the 'ESC'
          // key (if somebody knows how to clean this up, please let me know):
          if (Command == 0x1B && time_ms() - t0 < 100)
            continue;

          if (Command) {
conLOG_GATEW("trying to send the key command (len=%d)", i);
            if (! cCtrlKeyboard::Put(Command)) {
printf("term=%s\n", getenv("TERM")); fflush(stdout);
conLOG_GATEW("sending the key command failed");
              int func = cCtrlKeyboard::MapCodeToFunc(Command);
              if (func) {
conLOG_GATEW("trying to send the function key (func=%d, %d)", func, func - 256);
//                cCtrlKeyboard::Put((eKeys)KBDKEY(func));
                if (! cRemote::Put((eKeys)KBDKEY(func))) {
conLOG_GATEW("sending the function key failed");
                }
              }
            }
            break;
          } else {
//conLOG_GATEW("error: exiting");
//            LOG_ERROR;
            break;
          }
        }
      }
    }
  }
  } while (0);


//  UnprepareSocket();

  if (! _terminating)
    gl_CtrlGatewayCol.Del(this);

  conDSYSLOG("gateway thread ended (pid=%d)", getpid());
conLOG_GATEW("gateway thread ended (pid=%d)", getpid());
}



bool cCtrlGateway::SendMsg(const char* Msg) {
  return (_stack.sendData((const unsigned char*)Msg, strlen(Msg)) >= 0);
}



void cCtrlGateway::Close() {

    static const unsigned char msg[] = "\033[?25hThank you for using VDR\n\r";
    _stack.sendData(msg, sizeof(msg)-1);
    _stack.stop();

    _active = false;
}


