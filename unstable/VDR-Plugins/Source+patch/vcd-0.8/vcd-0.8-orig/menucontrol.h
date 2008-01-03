/*
 * menucontrol.h: Menu control of VCD replay
 *
 * VCD Player plugin for VDR (the Video Disk Recorder)
 * Copyright (C) 2002  Thomas Heiligenmann  <thomas@heiligenmann.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 f the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA
 *
 */


#include "viewer.h"
#include "player.h"


// --- cMenuSpiControl -------------------------------------------------------

class cMenuSpiControl : public cVcdViewerControl {
private:
  cSkinDisplayReplay *displayReplay;
  bool visible, modeOnly;
  static int spi;
  static cVcd *vcd;
public:
  cMenuSpiControl(void);
  virtual ~cMenuSpiControl();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Hide(void);
  static void SetItem(int Spi, cVcd *Vcd);
  };


// --- cMenuVcdControl -------------------------------------------------------

class cMenuVcdControl : public cVcdPlayerControl {
private:
  cSkinDisplayReplay *displayReplay;
  bool visible, modeOnly, shown, displayFrames;
  time_t timeoutShow;
  bool timeSearchActive, timeSearchHide;
  int timeSearchTime, timeSearchPos;
  int lastCurrent, lastTotal;
  bool lastPlay, lastForward;
  int lastSpeed;
  static int track;
  static cVcd *vcd;
  static char *title;
  void ShowMode(void);
  bool ShowProgress(bool Initial);
  void TimeSearchDisplay(void);
  void TimeSearchProcess(eKeys Key);
  void TimeSearch(void);
  void ShowTimed(int Seconds = 0);
public:
  cMenuVcdControl(void);
  virtual ~cMenuVcdControl();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Show(void);
  virtual void Hide(void);
  bool Visible(void) { return visible; }
  static void SetTrack(int Track, cVcd *Vcd);
  static const int LastReplayed(void);
  };

