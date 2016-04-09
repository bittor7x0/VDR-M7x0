#include "dummyplayer.h"
#include "tools.h"

//------------- cDummyPlayer ---------------------

/* player does nothing 
 * useful when tuners need to be free
 */
cDummyPlayer::cDummyPlayer(ePlayMode PlayMode) : cPlayer(PlayMode) {
  //dsyslog("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);
}

cDummyPlayer::~cDummyPlayer() {
  //dsyslog("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);
}


//--------------------cDummyPlayerControl -------------
cDummyPlayerControl::cDummyPlayerControl(ePlayMode PlayMode) 
  : cControl(player=new cDummyPlayer (PlayMode)) {
    //dsyslog("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);
}


cDummyPlayerControl::~cDummyPlayerControl() {
    delete player;
    player = NULL;

    //dsyslog("%s:%d\n", __PRETTY_FUNCTION__, __LINE__);
    //dsyslog("----------------   ~cDummyPlayerControl()   --------------------\n");
}


void cDummyPlayerControl::Hide() {
}


/* end player in Interactive mode
 **/
eOSState cDummyPlayerControl::ProcessKey(eKeys key) {
  if(getIaMode())
    return osEnd;
  return osContinue;
}
