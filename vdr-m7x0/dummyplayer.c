/*
 * dummyplayer.c: Player that does nothing (useful when tuners need to be free)
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "config.h"
#include "cutter.h"
#include "dummyplayer.h"
#include "menu.h"
#include "remote.h"
#include "shutdown.h"
#include "timers.h"
#include "thread.h"

#define DUMMYPLAYER_KEY_WAIT       15 // seconds
#define DUMMYPLAYER_SHUTDOWN_RETRY (DUMMYPLAYER_KEY_WAIT * 4)

// --- cDummyPlayer ----------------------------------------------------------

cDummyPlayer::cDummyPlayer(ePlayMode PlayMode) : cPlayer(PlayMode) { }
cDummyPlayer::~cDummyPlayer() { }

// --- cDummyPlayerControl ---------------------------------------------------

cDummyPlayerControl::cDummyPlayerControl(ePlayMode PlayMode)
:cControl(player = new cDummyPlayer(PlayMode))
{
  dsyslog("starting dummyplayer");

  // set initial retry shutdown at next timer
  cTimer *timer = Timers.GetNextActiveTimer();
  time_t Now    = time(NULL);
  time_t Next   = timer ? timer->StartTime() : 0;
  time_t Delta  = Next ? Next - Now : 0;
  if (Next && Next > Now && Next > ShutdownHandler.GetRetry() && Delta <= Setup.MinEventTimeout * 60) {
      dsyslog("set retry shutdown at %s", *TimeToString(Next));
      ShutdownHandler.SetRetry(Delta);
      }
}

cDummyPlayerControl::~cDummyPlayerControl() {
  dsyslog("stopping dummyplayer");
  delete player;
  player = NULL;
}

void cDummyPlayerControl::Hide() { }

eOSState cDummyPlayerControl::ProcessKey(eKeys Key)
{
  if (!Setup.HotStandby && !cRecordControls::Active() && !cCutter::Active() && !cUsbAutomounter::Active()) {
      time_t Soon = time(NULL) + DUMMYPLAYER_SHUTDOWN_RETRY;
      if (ShutdownHandler.Retry(Soon)) {
          if (ShutdownHandler.ConfirmShutdown(false))
              return osEnd;
          else
              return osContinue; // no wait if retry shutdown is near
          }
      }

  // wait for keep the CPU load low
  time_t Timeout = time(NULL) + DUMMYPLAYER_KEY_WAIT;
  while (time(NULL) <= Timeout) {
      if (cRemote::HasKeys())
         return osContinue; // react immediately on user input
      cCondWait::SleepMs(1500);
      }

  return osContinue;
}
