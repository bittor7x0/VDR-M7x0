/*
 * externalplayer-player.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _EXTERNALPLAYER_PLAYER_H_
#define _EXTERNALPLAYER_PLAYER_H_

#include <vdr/player.h>

#include "externalplayer-remotes.h"
#include "externalplayer-config.h"

using namespace std;

class cPlayerExternalplayer : public cPlayer {
private:
  cRemotesDisable * remotesDisable;
  sPlayerArgs * config;
  int fdReadPipe;
protected:
  void Activate(bool On);
public:
  cPlayerExternalplayer(ePlayMode playMode, sPlayerArgs * nConfig, int nFdReadPipe);
  ~cPlayerExternalplayer();
  bool isActive();
};

#endif /*_EXTERNALPLAYER_PLAYER_H_*/
