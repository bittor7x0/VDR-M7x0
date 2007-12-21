/*
 * gateways.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _CTRLGATEWAYS_H_
#define _CTRLGATEWAYS_H_

#include <vdr/svdrp.h>  // we need the socket class
#include <vdr/thread.h>


extern int glParPort;


class cCtrlGateways
:     public cThread
{
private:
  cSocket*      _socket;
  volatile bool _active;


private:
  virtual void Action();


public:
  cCtrlGateways();
  ~cCtrlGateways();

};


#endif // _CTRLGATEWAYS_H_

