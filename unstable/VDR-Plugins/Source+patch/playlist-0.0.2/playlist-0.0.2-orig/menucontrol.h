/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menucontrol.h 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#ifndef __MENUCONTROL_H
#define __MENUCONTROL_H

#include "dataplaylist.h"
#include <vdr/menu.h>


// --- cReplayControlPlaylist -----------------------------------------------------------

class cReplayControlPlaylist : public cReplayControl {
public:
  cPlayer *GetPlayerPlaylist() { return cControl::player; }
  };

// --- cControlPlaylist -----------------------------------------------------------

class cControlPlaylist : public cControl {
private:
//  cPlaylistRecord *playlistrecord;
  cReplayControlPlaylist *replaycontrol;
  eOSState PlayRecording(cPlaylistRecord *PlaylistRecord, bool Visible = false);
  void UpdateRecording(void);
public:
  cControlPlaylist(void);
  virtual ~cControlPlaylist(void);
  virtual void Hide(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

// --- cControlMenu -----------------------------------------------------------

class cControlMenu : public cOsdObject {
private:
  cOsdMenu *osdmenu;
public:
  cControlMenu(void);
  virtual ~cControlMenu(void);
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__MENUCONTROL_H
