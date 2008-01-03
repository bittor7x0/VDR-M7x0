/*
 * setup.h: Setup the plugin
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


#ifndef __VCD_SETUP_H
#define __VCD_SETUP_H

#include <vdr/menuitems.h>
#include "functions.h"

class cVcdSetupData {
public:
  int DriveSpeed;
  int BrokenMode;
  int HideMainMenuEntry;
  int PlayTracksContinuously;
  int AutostartReplay;
  int PlaySequenceReplay;
public:
  cVcdSetupData(void);
  };

class cVcdSetupMenu : public cMenuSetupPage {
private:
  cVcd *vcd;
  cVcdSetupData setupData;
protected:
  virtual void Store(void);
public:
  cVcdSetupMenu(cVcd *Vcd);
  };

extern cVcdSetupData VcdSetupData;

#endif //__VCD_SETUP_H
