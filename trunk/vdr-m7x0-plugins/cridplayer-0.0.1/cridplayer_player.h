/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_player.h 13 2007-03-06 07:09:16Z andreas $
 *
 */

#ifndef __CRIDPLAYER_PLAYER_H
#define __CRIDPLAYER_PLAYER_H

#include <vdr/player.h>
#include <vdr/thread.h>
#include "cridplayer_recording.h"
class cCridPlayer;

class cCridPlayerControl : public cControl {
private:
  cCridPlayer *player;
protected:
  cCridRecording *Recording(void);
  cCridMarks *GetMarks(void);
public:
  cCridPlayerControl(const cCridRecording *Rec);
       // Sets up a player for the given recording.
  virtual ~cCridPlayerControl();
  bool Active(void);
  void Stop(void);
       // Stops the current replay session (if any).
  void Pause(void);
       // Pauses the current replay session, or resumes a paused session.
  void Play(void);
       // Resumes normal replay mode.
  void Forward(void);
       // Runs the current replay session forward at a higher speed.
  void Backward(void);
       // Runs the current replay session backwards at a higher speed.
  void SkipSeconds(int Seconds);
       // Skips the given number of seconds in the current replay session.
       // The sign of 'Seconds' determines the direction in which to skip.
       // Use a very large negative value to go all the way back to the
       // beginning of the recording.
  bool GetIndex(int &CurrentIndex, int &TotalIndex, int &CurrentTime, int &TotalTime, bool SnapToIFrame = false);
       // Returns the current and total frame index, optionally snapped to the
       // nearest I-frame.
  bool GetReplayMode(bool &Play, bool &Forward, int &Speed);
       // Returns the current replay mode (if applicable).
       // 'Play' tells whether we are playing or pausing, 'Forward' tells whether
       // we are going forward or backward and 'Speed' is -1 if this is normal
       // play/pause mode, 0 if it is single speed fast/slow forward/back mode
       // and >0 if this is multi speed mode.
  void Goto(int Index, bool Still = false);
  void GotoTime(int32_t Pos, bool Still = false);
       // Positions to the given index and displays that frame as a still picture
       // if Still is true.
  };
#endif
