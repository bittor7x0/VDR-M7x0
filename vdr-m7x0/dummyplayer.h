#ifndef __VDR_DUMMY_PLAYER_H
#define __VDR_DUMMY_PLAYER_H

#include "player.h"


/**
 * This player does NOTHING
 * Just exits when kPower is got
 */

class cDummyPlayer : public cPlayer {

public:
    cDummyPlayer();
    virtual ~cDummyPlayer();
};

class cDummyPlayerControl : public cControl {
  private:
    cDummyPlayer *player;
    
  public:
    cDummyPlayerControl();
    virtual ~cDummyPlayerControl();
    virtual void Hide();

    virtual eOSState ProcessKey(eKeys Key);
    ///< handles kPower = end player, else does nothing
};


#endif

