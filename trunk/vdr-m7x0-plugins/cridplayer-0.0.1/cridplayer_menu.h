/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_menu.h 13 2007-03-06 07:09:16Z andreas $
 *
 */

#ifndef __CRIDPLAYER_MENU_H
#define __CRIDPLAYER_MENU_H
#include <vdr/osdbase.h>
#include <vdr/menuitems.h>
#include "cridplayer_recording.h"
#include "cridplayer_player.h"
#include "cridplayer_setup.h"
#define MAXPATHLEN 255

class cCridController;
class cCridMenuRecordings : public cOsdMenu {
private:
  cCridController *controller;
  cCridSeriesRecording *series;
  int helpKeys;
  void Set(void);
  void SetHelpKeys(void);
  eOSState PlayOpen(void);
  eOSState Rewind(void);
  eOSState Delete(void);
  eOSState Info(void);
public:
  cCridMenuRecordings(cCridController *Controller, bool OpenSubMenu);
  cCridMenuRecordings(cCridController *Controller, cCridSeriesRecording *Series);
  virtual ~cCridMenuRecordings();
  virtual eOSState ProcessKey(eKeys Key);
  };

class cCridReplayControl : public cCridPlayerControl {
private:
  cSkinDisplayReplay *displayReplay;
  cCridController *controller;
  cCridMarks *marks;
  bool visible;
  bool modeOnly;
  bool shown;
  int lastCurrent, lastTotal;
  int32_t lastCurrentTime, lastTotalTime;
  bool lastPlay, lastForward;
  int lastSpeed;
  time_t timeoutShow;
  bool timeSearchActive, timeSearchHide;
  int timeSearchTime, timeSearchPos;
  void TimeSearchDisplay(void);
  void TimeSearchProcess(eKeys Key);
  void TimeSearch(void);
  void ShowTimed(int Seconds = 0);
  void ShowMode(void);
  bool ShowProgress(bool Initial);
  void MarkToggle(void);
  void MarkJump(bool Forward);
  void MarkMove(bool Forward);
  void EditCut(void);
  void EditTest(void);
public:
  cCridReplayControl(cCridController *Controller, const cCridRecording *Rec);
  virtual ~cCridReplayControl();
  virtual cOsdObject *GetInfo(void);
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Show(void);
  virtual void Hide(void);
  };

class cCridSetupMenu : public cMenuSetupPage {
private:
  char basePath[MAXPATHLEN + 1];
  int showHidden;
  int recordingPreGap;
  int recordingPostGap;
  char dirMarkerPre[MAXMARKERLEN + 1];
  char dirMarkerPost[MAXMARKERLEN + 1];
  cCridController *controller;
protected:
  virtual void Store(void);
public:
  cCridSetupMenu(cCridController *Controller);
  };
#endif
