/*
 * dummyplayer.h: Player that does nothing (useful when tuners need to be free)
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __DUMMY_PLAYER_H
#define __DUMMY_PLAYER_H

#include "player.h"

class cDummyPlayer : public cPlayer {

public:
    cDummyPlayer(ePlayMode PlayMode = pmAudioVideo);
    virtual ~cDummyPlayer();
};

class cDummyPlayerControl : public cControl {
  private:
    cDummyPlayer *player;

  public:
    cDummyPlayerControl(ePlayMode PlayMode = pmAudioVideo);
    virtual ~cDummyPlayerControl();
    virtual void Hide();

    virtual eOSState ProcessKey(eKeys Key);
    ///< End player if shutdown isn't blocked with background activity,
    ///< else does nothing.
};

#endif //__DUMMY_PLAYER_H
