/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */

#ifndef __MPGPLAYER_MENU_H
#define __MPGPLAYER_MENU_H
#include <vdr/osdbase.h>
#include <vdr/menuitems.h>
#include "mpgplayer_recording.h"
#include "mpgplayer_player.h"
#include "mpgplayer_setup.h"
#define MAXPATHLEN 255

class cMpgController;
class cMpgMenuRecordings : public cOsdMenu {
private:
  cMpgController *controller;
  cMpgSeriesRecording *series;
  int helpKeys;
  void Set(void);
  void SetHelpKeys(void);
  eOSState PlayOpen(void);
  eOSState Rewind(void);
  eOSState Delete(void);
  eOSState Info(void);
public:
  cMpgMenuRecordings(cMpgController *Controller, bool OpenSubMenu);
  cMpgMenuRecordings(cMpgController *Controller, cMpgSeriesRecording *Series);
  virtual ~cMpgMenuRecordings();
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMpgReplayControl : public cMpgPlayerControl {
private:
  cSkinDisplayReplay *displayReplay;
  cMpgController *controller;
  cMpgMarks *marks;
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
  cMpgReplayControl(cMpgController *Controller, const cMpgRecording *Rec);
  virtual ~cMpgReplayControl();
  virtual cOsdObject *GetInfo(void);
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Show(void);
  virtual void Hide(void);
  };

class cMpgSetupMenu : public cMenuSetupPage {
private:
  char basePath[MAXPATHLEN + 1];
  int showHidden;
  int recordingPreGap;
  int recordingPostGap;
  char dirMarkerPre[MAXMARKERLEN + 1];
  char dirMarkerPost[MAXMARKERLEN + 1];
  cMpgController *controller;
protected:
  virtual void Store(void);
public:
  cMpgSetupMenu(cMpgController *Controller);
  };
#endif
