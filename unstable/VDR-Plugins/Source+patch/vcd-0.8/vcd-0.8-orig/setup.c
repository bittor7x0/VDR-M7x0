/*
 * setup.c: Setup the plugin
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

#include <vdr/menuitems.h>
#include "setup.h"
#include "i18n.h"

cVcdSetupData VcdSetupData;

// --- cVcdSetupData ---------------------------------------------------------

cVcdSetupData::cVcdSetupData(void)
{
  DriveSpeed = 2;
  BrokenMode = 0;
  HideMainMenuEntry = 0;
  PlayTracksContinuously = 1;
  AutostartReplay = 0;
  PlaySequenceReplay = 0;
}


// --- cVcdSetupMenu ---------------------------------------------------------

cVcdSetupMenu::cVcdSetupMenu(cVcd *Vcd)
{
  vcd = Vcd;
  setupData = VcdSetupData;
  SetSection(tr("VCD"));
  Add(new cMenuEditIntItem(tr("Setup.VCD$Drive speed"), &setupData.DriveSpeed, 1, 50));
  Add(new cMenuEditBoolItem(tr("Setup.VCD$Broken mode"), &setupData.BrokenMode));
  Add(new cMenuEditBoolItem(tr("Setup.VCD$Hide main menu entry"), &setupData.HideMainMenuEntry));
  Add(new cMenuEditBoolItem(tr("Setup.VCD$Play tracks continuously"), &setupData.PlayTracksContinuously));
  Add(new cMenuEditBoolItem(tr("Setup.VCD$Autostart replay"), &setupData.AutostartReplay));
  Add(new cMenuEditBoolItem(tr("Setup.VCD$Play sequence replay"), &setupData.PlaySequenceReplay));
}

void cVcdSetupMenu::Store(void)
{
  VcdSetupData = setupData;
  SetupStore("DriveSpeed", VcdSetupData.DriveSpeed);
  vcd->SetDriveSpeed(VcdSetupData.DriveSpeed);
  SetupStore("BrokenMode", VcdSetupData.BrokenMode);
  SetupStore("HideMainMenuEntry", VcdSetupData.HideMainMenuEntry);
  SetupStore("PlayTracksContinuously", VcdSetupData.PlayTracksContinuously);
  SetupStore("AutostartReplay", VcdSetupData.AutostartReplay);
  SetupStore("PlaySequenceReplay", VcdSetupData.PlaySequenceReplay);
}
