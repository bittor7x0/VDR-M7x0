/*
 * menu.c: OSD menu
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


#include <stdio.h>
#include <typeinfo>

#include <vdr/interface.h>
#include <vdr/status.h>
#include "i18n.h"
#include "menu.h"
#include "functions.h"
#include "setup.h"
#include "menucontrol.h"



// --- cMenu -----------------------------------------------------------------

cMenu::cMenu(cVcd *Vcd)
: cOsdMenu(tr("VideoCD"))
{
  char *buf = 0;

  vcd = Vcd;
  memset(vcdTitle,0,17);
  validLabel = vcd->isLabel();

  if (validLabel) {
     strncpy(vcdTitle,vcd->vcdInfo.album_id,16);
     asprintf(&buf, "VCD %d/%d %s",
        U16_TO_LE(vcd->vcdInfo.album_sequence),
        U16_TO_LE(vcd->vcdInfo.album_volumes),
        vcdTitle);
     SetTitle(buf);
     dsyslog("VCD: %d/%d Title: %s Tracks: %d Entries: %d Items: %d PSD: %d",
        U16_TO_LE(vcd->vcdInfo.album_sequence),
        U16_TO_LE(vcd->vcdInfo.album_volumes),
        vcdTitle, vcd->getTracks(),
        U16_TO_LE(vcd->vcdEntries.entries),
        U16_TO_LE(vcd->vcdInfo.max_segment_number),
        U32_TO_LE(vcd->vcdInfo.psd_size));
     }
  else if (VcdSetupData.BrokenMode) {
     validLabel = true;   // enable anyway
     asprintf(&buf, "noVCD");
     SetTitle(buf);
     }
  else
     Add(new cOsdItem(tr("No VideoCD detected")));
}

cMenu::~cMenu()
{
}

eOSState cMenu::Eject(void)
{
  if (vcd->DiscOk()) {
     dsyslog("VCD: Eject");
     memset(&(vcd->vcdEntries), 0, sizeof(vcd->vcdEntries));
     vcd->Eject();
     vcd->Close();
     return osStopReplay;
     }
  return osContinue;
}

void cMenu::SetHelp(const char *Red, const char *Green, const char *Blue)
{
  cOsdMenu::SetHelp(
     Red,
     Green,
     vcd->DiscOk() ? tr("Eject") : NULL,
     Blue
     );
}

eOSState cMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
 
  if (state == osUnknown) {
     switch (Key) {
        case kYellow:  return Eject();
        default:       break;
        }
     }
  return state;
}


// --- cMenuSpiItem ----------------------------------------------------------

class cMenuSpiItem : public cOsdItem {
private:
  int spi;
public:
  cMenuSpiItem(int Spi, int Size, __u8 Contents);
  int Spi(void) { return spi; }
  };

cMenuSpiItem::cMenuSpiItem(int Spi, int Size, __u8 Contents)
{
  spi = Spi;
  char *buffer = NULL;
  asprintf(&buffer, "Item %04d/%d  %s%s%s%s%s%s%s%s%s", Spi, Size,
     SPI_AUDIO(Contents) == SPI_1CH_AUDIO   ? "Audio "         : "",
     SPI_AUDIO(Contents) == SPI_2CH_AUDIO   ? "2Ch.Audio "     : "",
     SPI_AUDIO(Contents) == SPI_MUL_AUDIO   ? "Multich.Audio " : "",
     SPI_VIDEO(Contents) == SPI_NTSC_STILL  ? "NTSC Image "    : "",
     SPI_VIDEO(Contents) == SPI_NTSC_STILL2 ? "NTSC Image+ "   : "",
     SPI_VIDEO(Contents) == SPI_NTSC_VIDEO  ? "NTSC Video "    : "",
     SPI_VIDEO(Contents) == SPI_PAL_STILL   ? "PAL Image "     : "",
     SPI_VIDEO(Contents) == SPI_PAL_STILL2  ? "PAL Image+ "    : "",
     SPI_VIDEO(Contents) == SPI_PAL_VIDEO   ? "PAL Video "     : ""
     );
  SetText(buffer, false);
}


// --- cMenuSpi --------------------------------------------------------------

cMenuSpi::cMenuSpi(cVcd *Vcd)
: cMenu(Vcd)
{
  validItems = validLabel ? ListItems() : false;
  SetHelp(
     validItems ? tr("Play") : NULL,
     NULL,
     vcd->getTracks() ? tr("Tracks") : NULL
     );
  Display();
} 

cMenuSpi::~cMenuSpi()
{
}

bool cMenuSpi::ListItems(void)
{
  if (U16_TO_LE(vcd->vcdInfo.max_segment_number)>0) {
     int size = 1;
     for (int i=0; i<U16_TO_LE(vcd->vcdInfo.max_segment_number); i++) {
        if (vcd->vcdInfo.spi_contents[i+1] & SPI_CONTINUED)
           size++;
        else
           if (size>0) {
             Add(new cMenuSpiItem(i+1-size, size, vcd->vcdInfo.spi_contents[i+1-size]));
             size = 1;
             }
        }
     return true;
     }
  return false;
}

eOSState cMenuSpi::Play(void)
{
  if (validItems) {
     cMenuSpiItem *msi = (cMenuSpiItem *)Get(Current());
     if (msi) {
        cMenuSpiControl::SetItem(msi->Spi(), vcd);
        dsyslog("VCD: Play Item %d", msi->Spi());
        return osEnd;
        }
     }
  return osContinue;
}

eOSState cMenuSpi::ProcessKey(eKeys Key)
{
  eOSState state = cMenu::ProcessKey(Key);
 
  if (state == osUnknown) {
     switch (Key) {
        case kOk:
        case kRed:     return Play();
        case kBlue:    return osBack;
        default:       break;
        }
     }
  return state;
}


// --- cMenuVcdItem ----------------------------------------------------------

class cMenuVcdItem : public cOsdItem {
private:
  int track;
public:
  cMenuVcdItem(int Track, struct cdrom_msf0 strt, struct cdrom_msf0 len, bool Pal);
  cMenuVcdItem(int Track, struct cdrom_msf0 strt, struct cdrom_msf0 len);
  int Track(void) { return track; }
  };
 
cMenuVcdItem::cMenuVcdItem(int Track, struct cdrom_msf0 strt, struct cdrom_msf0 len, bool Pal)
{
  track = Track;
  char *buffer = NULL;
  asprintf(&buffer, "Track %02d  %02d:%02d.%02d  %02d:%02d.%02d  %s", Track,
     strt.minute, strt.second, strt.frame, len.minute, len.second, len.frame,
     Pal ? "PAL" : "NTSC");
  SetText(buffer, false);
}

cMenuVcdItem::cMenuVcdItem(int Track, struct cdrom_msf0 strt, struct cdrom_msf0 len)
{
  track = Track;
  char *buffer = NULL;
  asprintf(&buffer, "Track %02d  %02d:%02d.%02d  %02d:%02d.%02d", Track+1,
     strt.minute, strt.second, strt.frame, len.minute, len.second, len.frame);
  SetText(buffer, false);
}


// --- cMenuVcd --------------------------------------------------------------
 
cMenuVcd::cMenuVcd(cVcd *Vcd)
: cMenu(Vcd)
{
  validTracks = validLabel ? ListTracks() : false;
  spi = !VcdSetupData.BrokenMode && U16_TO_LE(vcd->vcdInfo.max_segment_number) && validLabel;
  SetHelp(
     validTracks ? tr("Play") : NULL,
     NULL,
     spi ? tr("SPI") : NULL
     );
  Display();
}
 
cMenuVcd::~cMenuVcd()
{
}

bool cMenuVcd::ListTracks(void)
{
  if (VcdSetupData.BrokenMode) {
     for (int i=0; i<=vcd->getTracks(); i++)
        Add(new cMenuVcdItem(i,
                   lba_to_msf(vcd->vcdEntry[i].cdte_addr.lba),
                   lba_to_msf(vcd->vcdEntry[i+1].cdte_addr.lba -
                            vcd->vcdEntry[i].cdte_addr.lba)
           ));
     return true;
     }
  else if (vcd->getTracks()>0) {
     for (int i=1; i<=vcd->getTracks(); i++)
        Add(new cMenuVcdItem(i,
                   lba_to_msf(vcd->vcdEntry[i].cdte_addr.lba -
                              vcd->vcdEntry[1].cdte_addr.lba),
                   lba_to_msf(vcd->vcdEntry[i+1].cdte_addr.lba -
                             vcd->vcdEntry[i].cdte_addr.lba),
                   vcd->vcdInfo.video_type_map[(i-1)/8] & (1 << (i-1)%8)
           ));
     return true;
     }
  return false;
}

eOSState cMenuVcd::Play(void)
{
  if (validTracks) {
     cMenuVcdItem *mvi = (cMenuVcdItem *)Get(Current());
     if (mvi) {
        cMenuVcdControl::SetTrack(mvi->Track(), vcd);
        dsyslog("VCD: Play Track %d", mvi->Track() + (VcdSetupData.BrokenMode ? 1 : 0));
        return osEnd;
        }
     }
  return osContinue;
}

eOSState cMenuVcd::ProcessKey(eKeys Key)
{
  eOSState state = cMenu::ProcessKey(Key);
 
  if (state == osUnknown) {
     switch (Key) {
        case kOk:
        case kRed:     return Play();
        case kBlue:    if (!spi) break;
                       return AddSubMenu(new cMenuSpi(vcd));
        case kMenu:    return osEnd;
        default:       break;
        }
     }
  return state;
}
