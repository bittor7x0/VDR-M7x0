/*
 * psdcontrol.c: PSD control of VCD replay
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
#include <time.h>
#include <typeinfo>

#include <vdr/interface.h>
#include <vdr/status.h>
#include "i18n.h"
#include "functions.h"
#include "setup.h"
#include "psdcontrol.h"

#define GOTO_SELF         (psdVcd->header==PLAY_LIST_HEADER \
                          ?  U16_TO_LE(vcd->vcdLot.list_id_offset[U16_TO_LE(psdVcd->play.list_id)]) \
                          :  psdVcd->header==SELECTION_LIST_HEADER \
                          ?  U16_TO_LE(vcd->vcdLot.list_id_offset[U16_TO_LE(psdVcd->selection.list_id)]) \
                          :  0xFFFF)
#define GOTO_NEXT         (psdVcd->header==PLAY_LIST_HEADER \
                          ?  U16_TO_LE(psdVcd->play.next_list_offset) \
                          :  psdVcd->header==SELECTION_LIST_HEADER \
                          ?  U16_TO_LE(psdVcd->selection.next_list_offset) \
                          :  0xFFFF)
#define GOTO_PREVIOUS     (psdVcd->header==PLAY_LIST_HEADER \
                          ?  U16_TO_LE(psdVcd->play.previous_list_offset) \
                          :  psdVcd->header==SELECTION_LIST_HEADER \
                          ?  U16_TO_LE(psdVcd->selection.previous_list_offset) \
                          :  0xFFFF)
#define GOTO_RETURN       (psdVcd->header==PLAY_LIST_HEADER \
                          ?  U16_TO_LE(psdVcd->play.return_list_offset) \
                          :  psdVcd->header==SELECTION_LIST_HEADER \
                          ?  U16_TO_LE(psdVcd->selection.return_list_offset) \
                          :  0xFFFF)
#define GOTO_DEFAULT      (psdVcd->header==SELECTION_LIST_HEADER \
                          ?  U16_TO_LE(psdVcd->selection.default_list_offset) \
                          :  0xFFFF)
#define GOTO_TIMEOUT      (psdVcd->header==PLAY_LIST_HEADER \
                          ?  U16_TO_LE(psdVcd->play.next_list_offset) \
                          :  psdVcd->header==SELECTION_LIST_HEADER \
                          ?  U16_TO_LE(psdVcd->selection.timeout_list_offset) \
                          :  0xFFFF)
#define GOTO_NUMERIC(b)   (psdVcd->header==SELECTION_LIST_HEADER \
                          ?  psdVcd->selection.nos>(b-1) \
                             ?  U16_TO_LE(psdVcd->selection.selection_offset[(b-1)]) \
                             :  0xFFFF \
                          :  0xFFFF)
#define LIST_LOOP         (psdVcd->header==SELECTION_LIST_HEADER \
                          ?  (PsdVcd->selection.loop_count & 0x7F) \
                          :  -1)
#define AUTO_WAIT         (psdVcd->header==PLAY_LIST_HEADER \
                          ?  PLAY_ITEM_WAIT(psdVcd->play.auto_pause_wait_time) \
                          :  -1)


// --- cPsdSpiControl -----------------------------------------------------------

int cPsdSpiControl::spi = 0;
cVcd *cPsdSpiControl::vcd = 0;
union psd_vcd *cPsdSpiControl::psdVcd = 0;
int cPsdSpiControl::playInit = -1;
int cPsdSpiControl::playTime = -1;
int cPsdSpiControl::waitInit = -1;
int cPsdSpiControl::waitTime = -1;

cPsdSpiControl::cPsdSpiControl(void)
: cVcdViewerControl(spi, vcd)
{
}
 
cPsdSpiControl::cPsdSpiControl(int Loop)
: cVcdViewerControl(spi, vcd)
{
}
 
cPsdSpiControl::~cPsdSpiControl()
{
  cStatus::MsgReplaying(this, NULL, NULL, true);
}

void cPsdSpiControl::SetItem(int Spi, cVcd *Vcd, union psd_vcd *PsdVcd)
{
  cControl *control = cControl::Control();
  if (control && typeid(*control)==typeid(cPsdVcdControl))
     static_cast<cPsdVcdControl*>(control)->Stop();

  psdVcd = PsdVcd;
  playInit = time(NULL);
  playTime = (psdVcd->header==PLAY_LIST_HEADER
     ?  U16_TO_LE(psdVcd->play.playing_time) / 15
     :  -1);
  waitInit = -1;
  waitTime = (psdVcd->header==PLAY_LIST_HEADER
     ?  PLAY_ITEM_WAIT(psdVcd->play.play_item_wait_time)
     :  psdVcd->header==SELECTION_LIST_HEADER
     ?  PLAY_ITEM_WAIT(psdVcd->selection.timeout_wait_time)
     :  -1);

  if (control && typeid(*control)==typeid(cPsdSpiControl) &&
        strcmp(Vcd->DeviceName(),static_cast<cPsdSpiControl*>(control)->DeviceName())==0) {
     if (static_cast<cPsdSpiControl*>(control)->Active()) {
        static_cast<cPsdSpiControl*>(control)->GotoItem(Spi);
        return;
        }
     delete static_cast<cPsdSpiControl*>(control);
     }

  spi = Spi;
  vcd = Vcd;
  cControl::Launch(new cPsdSpiControl(LIST_LOOP));
}

bool cPsdSpiControl::GotoListOffset(__u16 Offs)
{
  if (Offs==0xFFFF)
     return false;

  int offs = vcd->vcdInfo.offset_multiplier * Offs;
  union psd_vcd *PsdVcd = (union psd_vcd*)(&(vcd->vcdPsd[offs]));
  __u16 item;
  __u16 list_id;

  if (PsdVcd->header==PLAY_LIST_HEADER) {
     item = U16_TO_LE(PsdVcd->play.play_item[0]);
     list_id = U16_TO_LE(PsdVcd->play.list_id);
     }
  else if (PsdVcd->header==SELECTION_LIST_HEADER) {
     item = U16_TO_LE(PsdVcd->selection.play_item);
     list_id = U16_TO_LE(PsdVcd->selection.list_id);
     }
  else
     return false;

  dsyslog("VCD: PSD list_id %d", list_id);

  if (PLAY_ITEM_TYP(item)==piTrack)
     cPsdVcdControl::SetTrack(PLAY_ITEM_NO(item), vcd, PsdVcd);
  else if (PLAY_ITEM_TYP(item)==piEntry)
     cPsdVcdControl::SetEntry(PLAY_ITEM_NO(item), vcd, PsdVcd);
  else if (PLAY_ITEM_TYP(item)==piSpi)
     SetItem(PLAY_ITEM_NO(item), vcd, PsdVcd);

  return true;
}

eOSState cPsdSpiControl::ProcessKey(eKeys Key)
{
  int playTick = time(NULL) - playInit;

  if (!Active() || (playTick==playTime)) {
     if (waitTime>0) {
        if (waitInit==-1) {
           waitInit = time(NULL);
           }
        else {
           int waitTick = time(NULL) - waitInit;
           if (waitTick>=waitTime) {
              if (!GotoListOffset(GOTO_TIMEOUT))
                 return osEnd;
              }
           }
        }
     else if (waitTime==0) {
        if (!GotoListOffset(GOTO_TIMEOUT))
           return osEnd;
        }
     else if (waitTime==-1)
        ;
     else if (!GotoListOffset(GOTO_NEXT))
        return osEnd;
     }

  if (Still()) {
     if (waitTime>0) {
        int waitTick = time(NULL) - playInit;
        if (waitTick>=waitTime) {
           if (!GotoListOffset(GOTO_TIMEOUT))
              return osEnd;
           }
        }
     else if (waitTime==0) {
        if (!GotoListOffset(GOTO_TIMEOUT))
           return osEnd;
        }
     }

  switch (Key) {
     case k1:       GotoListOffset(GOTO_NUMERIC(1));
                    break;
     case k2:       GotoListOffset(GOTO_NUMERIC(2));
                    break;
     case k3:       GotoListOffset(GOTO_NUMERIC(3));
                    break;
     case k4:       GotoListOffset(GOTO_NUMERIC(4));
                    break;
     case k5:       GotoListOffset(GOTO_NUMERIC(5));
                    break;
     case k6:       GotoListOffset(GOTO_NUMERIC(6));
                    break;
     case k7:       GotoListOffset(GOTO_NUMERIC(7));
                    break;
     case k8:       GotoListOffset(GOTO_NUMERIC(8));
                    break;
     case k9:       GotoListOffset(GOTO_NUMERIC(9));
                    break;
     case kOk:      GotoListOffset(GOTO_DEFAULT);
                    break;
     case kLeft:    GotoListOffset(GOTO_PREVIOUS);
                    break;
     case kRight:   GotoListOffset(GOTO_NEXT);
                    break;
     case kBack:    GotoListOffset(GOTO_RETURN);
                    break;
     case kBlue:    Hide();
                    Stop();
                    vcd->Close();
                    return osEnd;
     default:       return osUnknown;
     }
  return osContinue;
}


// --- cPsdVcdControl --------------------------------------------------------

int cPsdVcdControl::track = 0;
cVcd *cPsdVcdControl::vcd = 0;
union psd_vcd *cPsdVcdControl::psdVcd = 0;
int cPsdVcdControl::playInit = -1;
int cPsdVcdControl::playTime = -1;
int cPsdVcdControl::waitInit = -1;
int cPsdVcdControl::waitTime = -1;

cPsdVcdControl::cPsdVcdControl(void)
: cVcdPlayerControl(track, vcd)
{
}
 
cPsdVcdControl::cPsdVcdControl(int Lba, int Loop, int AutoWait)
: cVcdPlayerControl(track, vcd, Lba, Loop, AutoWait)
{
}

cPsdVcdControl::~cPsdVcdControl()
{
  cStatus::MsgReplaying(this, NULL, NULL, true);
}

void cPsdVcdControl::SetTrack(int Track, cVcd *Vcd, union psd_vcd *PsdVcd)
{
  cControl *control = cControl::Control();
  if (control && typeid(*control)==typeid(cPsdSpiControl))
     static_cast<cPsdSpiControl*>(control)->Stop();

  psdVcd = PsdVcd;
  playInit = time(NULL);
  playTime = (psdVcd->header==PLAY_LIST_HEADER
     ?  U16_TO_LE(psdVcd->play.playing_time) / 15
     :  -1);
  waitInit = -1;
  waitTime = (psdVcd->header==PLAY_LIST_HEADER
     ?  PLAY_ITEM_WAIT(psdVcd->play.play_item_wait_time)
     :  psdVcd->header==SELECTION_LIST_HEADER
     ?  PLAY_ITEM_WAIT(psdVcd->selection.timeout_wait_time)
     :  -1);

  if (control && typeid(*control)==typeid(cPsdVcdControl) &&
        strcmp(Vcd->DeviceName(),static_cast<cPsdVcdControl*>(control)->DeviceName())==0) {
     if (static_cast<cPsdVcdControl*>(control)->Active()) {
        static_cast<cPsdVcdControl*>(control)->GotoTrack(Track);
        return;
        }
     delete static_cast<cPsdVcdControl*>(control);
     }

  track = Track;
  vcd = Vcd;
  cControl::Launch(new cPsdVcdControl(
     vcd->vcdEntry[Track].cdte_addr.lba,
     LIST_LOOP,
     AUTO_WAIT
     ));
}

void cPsdVcdControl::SetEntry(int Entry, cVcd *Vcd, union psd_vcd *PsdVcd)
{
  cControl *control = cControl::Control();
  if (control && typeid(*control)==typeid(cPsdSpiControl))
     static_cast<cPsdSpiControl*>(control)->Stop();

  psdVcd = PsdVcd;
  playInit = time(NULL);
  playTime = (psdVcd->header==PLAY_LIST_HEADER
     ?  U16_TO_LE(psdVcd->play.playing_time) / 15
     :  -1);
  waitInit = -1;
  waitTime = (psdVcd->header==PLAY_LIST_HEADER
     ?  PLAY_ITEM_WAIT(psdVcd->play.play_item_wait_time)
     :  PsdVcd->header==SELECTION_LIST_HEADER
     ?  PLAY_ITEM_WAIT(psdVcd->selection.timeout_wait_time)
     :  -1);

  if (control && typeid(*control)==typeid(cPsdVcdControl) &&
        strcmp(Vcd->DeviceName(),static_cast<cPsdVcdControl*>(control)->DeviceName())==0) {
     if (static_cast<cPsdVcdControl*>(control)->Active()) {
        static_cast<cPsdVcdControl*>(control)->GotoEntry(Entry);
        return;
        }
     delete static_cast<cPsdVcdControl*>(control);
     }

  track = bcd_to_bin(Vcd->vcdEntries.entry[Entry].track_no) - 1;
  vcd = Vcd;
  cControl::Launch(new cPsdVcdControl(
     bcdmsf_to_lba(vcd->vcdEntries.entry[Entry].entry_msf),
     LIST_LOOP,
     AUTO_WAIT
     ));
}

bool cPsdVcdControl::GotoListOffset(__u16 Offs)
{
  if (Offs==0xFFFF)
     return false;

  int offs = vcd->vcdInfo.offset_multiplier * Offs;
  union psd_vcd *PsdVcd = (union psd_vcd*)(&(vcd->vcdPsd[offs]));
  __u16 item;
  __u16 list_id;

  if (PsdVcd->header==PLAY_LIST_HEADER) {
     item = U16_TO_LE(PsdVcd->play.play_item[0]);
     list_id = U16_TO_LE(PsdVcd->play.list_id);
     }
  else if (PsdVcd->header==SELECTION_LIST_HEADER) {
     item = U16_TO_LE(PsdVcd->selection.play_item);
     list_id = U16_TO_LE(PsdVcd->selection.list_id);
     }
  else
     return false;

  dsyslog("VCD: PSD item %d", list_id);

  if (PLAY_ITEM_TYP(item)==piTrack)
     SetTrack(PLAY_ITEM_NO(item), vcd, PsdVcd);
  else if (PLAY_ITEM_TYP(item)==piEntry)
     SetEntry(PLAY_ITEM_NO(item), vcd, PsdVcd);
  else if (PLAY_ITEM_TYP(item)==piSpi)
     cPsdSpiControl::SetItem(PLAY_ITEM_NO(item), vcd, PsdVcd);

  return true;
}

eOSState cPsdVcdControl::ProcessKey(eKeys Key)
{
  int playTick = time(NULL) - playInit;

  if (!Active() || (playTick==playTime)) {
     if (waitTime>0) {
        if (waitInit==-1) {
           waitInit = time(NULL);
           }
        else {
           int waitTick = time(NULL) - waitInit;
           if (waitTick>=waitTime) {
              if (!GotoListOffset(GOTO_TIMEOUT))
                 return osEnd;
              }
           }
        }
     else if (waitTime==0) {
        if (!GotoListOffset(GOTO_TIMEOUT))
           return osEnd;
        }
     else if (waitTime==-1)
        ;
     else if (!GotoListOffset(GOTO_NEXT))
        return osEnd;
     }

  switch (Key) {
     case k1:       GotoListOffset(GOTO_NUMERIC(1));
                    break;
     case k2:       GotoListOffset(GOTO_NUMERIC(2));
                    break;
     case k3:       GotoListOffset(GOTO_NUMERIC(3));
                    break;
     case k4:       GotoListOffset(GOTO_NUMERIC(4));
                    break;
     case k5:       GotoListOffset(GOTO_NUMERIC(5));
                    break;
     case k6:       GotoListOffset(GOTO_NUMERIC(6));
                    break;
     case k7:       GotoListOffset(GOTO_NUMERIC(7));
                    break;
     case k8:       GotoListOffset(GOTO_NUMERIC(8));
                    break;
     case k9:       GotoListOffset(GOTO_NUMERIC(9));
                    break;
     case kOk:      GotoListOffset(GOTO_DEFAULT);
                    break;
     case kLeft:    GotoListOffset(GOTO_PREVIOUS);
                    break;
     case kRight:   GotoListOffset(GOTO_NEXT);
                    break;
     case kBack:    GotoListOffset(GOTO_RETURN);
                    break;
     case kPause:
     case kDown:    Pause(); break;
     case kGreen|k_Repeat:
     case kGreen:   SkipSeconds(-60); break;
     case kYellow|k_Repeat:
     case kYellow:  SkipSeconds(60); break;

     case kBlue:    Hide();
                    Stop();
                    vcd->Close();
                    return osEnd;
     default:       return osUnknown;
     }
  return osContinue;
}
