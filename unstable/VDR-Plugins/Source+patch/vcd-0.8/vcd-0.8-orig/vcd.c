/*
 * vcd.c: main
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

#include <getopt.h>
#include <stdlib.h>
#include <vdr/interface.h>
#include <vdr/plugin.h>
#include "functions.h"
#include "i18n.h"
#include "menu.h"
#include "menucontrol.h"
#include "psdcontrol.h"
#include "psd.h"
#include "setup.h"

static const char *VERSION        = "0.8";
static const char *DESCRIPTION    = "VideoCD Player";
static const char *MAINMENUENTRY  = "VideoCD";

class cPluginVcd : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cVcd *vcd;
  const char *option_vcd;
public:
  cPluginVcd(void);
  virtual ~cPluginVcd();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void);
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdMenu *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginVcd::cPluginVcd(void)
{
  // Initialize any member varaiables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  vcd = NULL;
  option_vcd = NULL;
}

cPluginVcd::~cPluginVcd()
{
  // Clean up after yourself!
  delete vcd;
}

const char *cPluginVcd::Description(void)
{
  if (option_vcd) {
     char *buf = NULL;
     asprintf(&buf, "%s (%s)", DESCRIPTION, option_vcd);
     return buf;
     }
  else
     return DESCRIPTION;
}

const char *cPluginVcd::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return "  -v DEV,   --vcd=DEV      use DEV as the CD-ROM device (default is /dev/cdrom)\n";
}

bool cPluginVcd::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  static struct option long_options[] = {
       { "vcd", required_argument, NULL, 'v' },
       { NULL }
     };

  int c;
  while ((c = getopt_long(argc, argv, "v:", long_options, NULL)) != -1) {
     switch (c) {
        case 'v': option_vcd = optarg; break;
        default:  return false;
        }
     }
  return true;
}

bool cPluginVcd::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  if (option_vcd)
     vcd = new cVcd(option_vcd);
  else
     vcd = new cVcd("/dev/cdrom");
  return true;
}

void cPluginVcd::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

const char *cPluginVcd::MainMenuEntry(void)
{
  if (VcdSetupData.HideMainMenuEntry)
     return NULL;
  if (vcd->DriveExists()) {
     if (option_vcd) {
        char *buf = NULL;
        asprintf(&buf, "%s (%s)", tr(MAINMENUENTRY), option_vcd);
        return buf;
        }
     else
        return tr(MAINMENUENTRY);
     }
  else
     return NULL;
}

cOsdMenu *cPluginVcd::MainMenuAction(void)
{
  if (vcd->getVCD() && vcd->DiscOk()) {
     int psd_size = 0;
     if (vcd->Open()) {
        vcd->readTOC(CDROM_LBA);
        vcd->readSectorXA21(INFO_VCD_LBA, &(vcd->vcdInfo));
        vcd->readSectorXA21(ENTRIES_VCD_LBA, &(vcd->vcdEntries));
        for (int i=0; i<LOT_VCD_SIZE; i++)
           vcd->readSectorXA21(LOT_VCD_LBA+i, &(vcd->vcdLot.list_id_offset[i*CD_XA21_DATASIZE/2]));
        psd_size = max((int)U32_TO_LE(vcd->vcdInfo.psd_size), PSD_VCD_MAX_SIZE*CD_XA21_DATASIZE);
        for (int i=0; i<psd_size/CD_XA21_DATASIZE+1; i++)
           vcd->readSectorXA21(PSD_VCD_LBA+i, &(vcd->vcdPsd[i*CD_XA21_DATASIZE]));
        }
     else
        psd_size = max((int)U32_TO_LE(vcd->vcdInfo.psd_size), PSD_VCD_MAX_SIZE*CD_XA21_DATASIZE);
     if (VcdSetupData.AutostartReplay) {
        if (vcd->isLabel()  && vcd->getTracks()>0) {
           if (VcdSetupData.PlaySequenceReplay && psd_size) {
              int offs = vcd->vcdInfo.offset_multiplier * U16_TO_LE(vcd->vcdLot.list_id_offset[0]);
              union psd_vcd *psdVcd = (union psd_vcd*)(&(vcd->vcdPsd[offs]));
              if (psdVcd->header==PLAY_LIST_HEADER) {
                 __u16 item = U16_TO_LE(psdVcd->play.play_item[0]);
                 if (PLAY_ITEM_TYP(item)==piTrack)
                    cPsdVcdControl::SetTrack(PLAY_ITEM_NO(item), vcd, psdVcd);
                 else if (PLAY_ITEM_TYP(item)==piEntry)
                    cPsdVcdControl::SetEntry(PLAY_ITEM_NO(item), vcd, psdVcd);
                 else if (PLAY_ITEM_TYP(item)==piSpi)
                    cPsdSpiControl::SetItem(PLAY_ITEM_NO(item), vcd, psdVcd);
                 }
              else if (psdVcd->header==SELECTION_LIST_HEADER) {
                 __u16 item = U16_TO_LE(psdVcd->selection.play_item);
                 if (PLAY_ITEM_TYP(item)==piTrack)
                    cPsdVcdControl::SetTrack(PLAY_ITEM_NO(item), vcd, psdVcd);
                 else if (PLAY_ITEM_TYP(item)==piEntry)
                    cPsdVcdControl::SetEntry(PLAY_ITEM_NO(item), vcd, psdVcd);
                 else if (PLAY_ITEM_TYP(item)==piSpi)
                    cPsdSpiControl::SetItem(PLAY_ITEM_NO(item), vcd, psdVcd);
                 }
              dsyslog("VCD: Autoplay, PSD");
              }
           else {
              cMenuVcdControl::SetTrack(1, vcd);
              dsyslog("VCD: Autoplay, no PSD");
              }
           }
        else
           Skins.Message(mtInfo, tr("No VideoCD detected"));
        }
     else {
        if (VcdSetupData.PlaySequenceReplay && psd_size) {
           return new cVcdPsd(vcd);
           }
        else
           return new cMenuVcd(vcd);
        }
     }
  else
     Skins.Message(mtInfo, tr("No disc inserted"));

  return NULL;
}

cMenuSetupPage *cPluginVcd::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cVcdSetupMenu(vcd);
}

bool cPluginVcd::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if (!strcasecmp(Name, "DriveSpeed"))
     VcdSetupData.DriveSpeed = atoi(Value);
  else if (!strcasecmp(Name, "BrokenMode"))
     VcdSetupData.BrokenMode = atoi(Value);
  else if (!strcasecmp(Name, "HideMainMenuEntry"))
     VcdSetupData.HideMainMenuEntry = atoi(Value);
  else if (!strcasecmp(Name, "PlayTracksContinuously"))
     VcdSetupData.PlayTracksContinuously = atoi(Value);
  else if (!strcasecmp(Name, "AutostartReplay"))
     VcdSetupData.AutostartReplay = atoi(Value);
  else if (!strcasecmp(Name, "PlaySequenceReplay"))
     VcdSetupData.PlaySequenceReplay = atoi(Value);
  else
     return false;
  return true;
}

VDRPLUGINCREATOR(cPluginVcd); // Don't touch this!
