/*
 * externalplayer-control.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _EXTERNALPLAYER_CONTROL_H_
#define _EXTERNALPLAYER_CONTROL_H_

#include <vdr/player.h>
#include <vdr/status.h>

#include "externalplayer-player.h"

using namespace std;

class cStatusExternalplayer : public cStatus {
private:
  sPlayerArgs * config;
public:
  cStatusExternalplayer(sPlayerArgs * nConfig);
  ~cStatusExternalplayer() {}
  void OsdTitle(const char * title);
};

class cControlExternalplayer : public cControl {
private:
  sPlayerArgs * config;
  cStatusExternalplayer * status;
  int fdWritePipe;
public:
  cControlExternalplayer(sPlayerArgs * nConfig, int fdsPipe[2]);
  ~cControlExternalplayer();
  void Hide() {}
  eOSState ProcessKey(eKeys key);
};

#endif /*_EXTERNALPLAYER_CONTROL_H_*/
