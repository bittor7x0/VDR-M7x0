/*
 * gateways.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#include <vdr/interface.h>

#include "global.h"
#include "gateways.h"
#include "gateway.h"



int glParPort = 2002;


cCtrlGateways::cCtrlGateways()
:
#if VDRVERSNUM >= 10300
  cThread (0),
#endif
  _socket (NULL),
  _active (false)
{
}



cCtrlGateways::~cCtrlGateways() {
  _active = false;
  Cancel(200);
  delete _socket;
}



void cCtrlGateways::Action() {

  if (! glParPort)
    return;

  conDSYSLOG("gateways thread started (pid=%d)", getpid());
  do {

  _socket = new cSocket(glParPort, 5);
  if (! _socket) {
    conESYSLOG( "out of memory" );
    break;
  }

  if (! _socket->Open()) {
    conESYSLOG("error opening socket");
    break;
  }

  conDSYSLOG("socket opened");

  _active = true;
  while (_active) {

    usleep(100000);

    int newSocket = _socket->Accept();
    if (newSocket > 0) {

      cCtrlGateway* pRec = new cCtrlGateway(newSocket);
      if (pRec)
        pRec->Start();

      Interface->LearnKeys();
    }
  }

  } while ( 0 );
  conDSYSLOG("gateways thread ended (pid=%d)", getpid());
}


