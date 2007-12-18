/*
 * dummy_player.h: Player that does nothing (saves CPU time)
 *
 * See the main source file '.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __DUMMY_PLAYER_H
#define __DUMMY_PLAYER_H

#include <vdr/player.h>

class cDummyPlayer;

class cDummyPlayerControl : public cControl {
private:
  static cDummyPlayer *m_Player;
  static cMutex m_Lock;

  static cDummyPlayer *OpenPlayer(void);

public:
  cDummyPlayerControl(void);
  virtual ~cDummyPlayerControl();

  virtual void Show(void) {}
  virtual void Hide(void) {}
  virtual eOSState ProcessKey(eKeys Key);

  static void Close(void);
  static bool IsOpen(void) { return m_Player != NULL; }

  static bool UseBlankImage;
};

#endif //__DUMMY_PLAYER_H

