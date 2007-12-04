/*
 * dummy_player.c: Player that does nothing (saves CPU time)
 *
 * See the main source file '.c' for copyright information and
 * how to reach the author.
 *
 */

#include <vdr/status.h>

#include "dummy_player.h"
#include "timer.h"

#ifndef STILLPICTURE_INTERVAL 
# define STILLPICTURE_INTERVAL (5*1000)   // 5 sec
#endif


// --- cDummyPlayerControl --------------------------------------------------

extern const unsigned char v_mpg_vdrlogo[]; // vdrlogo_720x576.c
extern const int v_mpg_vdrlogo_length;      // vdrlogo_720x576.c
//extern const unsigned char v_mpg_nosignal[];// nosignal_720x576.c
//extern const int v_mpg_nosignal_length;     // nosignal_720x576.c
extern const unsigned char v_mpg_black[];   // black_720x576.c
extern const int v_mpg_black_length;        // black_720x576.c

class cDummyPlayer : public cPlayer {
  protected:
    virtual void Activate(bool On)
    {
      if(On) {
	TimerHandler();
        CreateTimerEvent(this, &cDummyPlayer::TimerHandler, 
			 STILLPICTURE_INTERVAL);
      } else {
        CancelTimerEvents(this);
      }
    }
    bool TimerHandler(void)
    {
      if(! cDummyPlayerControl::UseBlankImage)
	DeviceStillPicture(v_mpg_vdrlogo, v_mpg_vdrlogo_length);
      else
	DeviceStillPicture(v_mpg_black, v_mpg_black_length);
      //DeviceStillPicture(v_mpg_nosignal, v_mpg_nosignal_length);
      return true;
    }

  public:
    cDummyPlayer(void) {};
    virtual ~cDummyPlayer()
    {
      Activate(false);
      Detach();
    }
};

// --- cDummyPlayerControl --------------------------------------------------

cDummyPlayer *cDummyPlayerControl::m_Player = NULL;
cMutex cDummyPlayerControl::m_Lock;
bool cDummyPlayerControl::UseBlankImage = false;

cDummyPlayerControl::cDummyPlayerControl(void) :
  cControl(OpenPlayer())
{
#if VDRVERSNUM < 10338
  cStatus::MsgReplaying(this, "none");
#else
  cStatus::MsgReplaying(this, "none", NULL, true);
#endif
}

cDummyPlayerControl::~cDummyPlayerControl()
{
#if VDRVERSNUM < 10338
  cStatus::MsgReplaying(this, NULL);
#else
  cStatus::MsgReplaying(this, NULL, NULL, false);
#endif
  Close();
}

cDummyPlayer *cDummyPlayerControl::OpenPlayer(void)
{
  m_Lock.Lock();
  if(!m_Player)
    m_Player = new cDummyPlayer;
  m_Lock.Unlock();
  return m_Player;
}

void cDummyPlayerControl::Close(void)
{
  m_Lock.Lock();
  if(m_Player)
    delete m_Player;
  m_Player = NULL;
  m_Lock.Unlock();
}

eOSState cDummyPlayerControl::ProcessKey(eKeys Key)
{
  if(!ISMODELESSKEY(Key) || Key == kBack || Key == kStop) {
    Close();
    return osEnd;
  }
  return osContinue;
}

