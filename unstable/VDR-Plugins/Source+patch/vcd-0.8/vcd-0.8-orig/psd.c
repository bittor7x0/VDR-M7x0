/*
 * psd.c: PSD controlled replay
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


#include "i18n.h"
#include "psd.h"
#include "psdcontrol.h"


// --- cVcdPsdItem -----------------------------------------------------------

class cVcdPsdItem : public cOsdItem {
private:
  int lot;
  union psd_vcd *psdVcd;
public:
  cVcdPsdItem(int Lot, union psd_vcd* PsdVcd);
  int Lot(void) { return lot; }
  union psd_vcd* PsdVcd(void) { return psdVcd; }
  };

cVcdPsdItem::cVcdPsdItem(int Lot, union psd_vcd* PsdVcd)
{
  lot = Lot;
  psdVcd = PsdVcd;
  char *buffer = NULL;
  asprintf(&buffer, "%s %d  %s %d",
     PsdVcd->header==PLAY_LIST_HEADER
     ?  "Play List"
     :  PsdVcd->header==SELECTION_LIST_HEADER
     ?  "Selection List"
     :  "Unknown",
     PsdVcd->header==PLAY_LIST_HEADER
     ?  U16_TO_LE(PsdVcd->play.list_id)
     :  PsdVcd->header==SELECTION_LIST_HEADER
     ?  U16_TO_LE(PsdVcd->selection.list_id)
     :  0,
     PsdVcd->header==PLAY_LIST_HEADER
     ?  PLAY_ITEM_STR(U16_TO_LE(PsdVcd->play.play_item[0]))
     :  PsdVcd->header==SELECTION_LIST_HEADER
     ?  PLAY_ITEM_STR(U16_TO_LE(PsdVcd->selection.play_item))
     :  "?",
     PsdVcd->header==PLAY_LIST_HEADER
     ?  PLAY_ITEM_NO(U16_TO_LE(PsdVcd->play.play_item[0]))
     :  PsdVcd->header==SELECTION_LIST_HEADER
     ?  PLAY_ITEM_NO(U16_TO_LE(PsdVcd->selection.play_item))
     :  0
     );
  SetText(buffer, false);
}


// --- cVcdPsd ---------------------------------------------------------------

cVcdPsd::cVcdPsd(cVcd *Vcd)
: cOsdMenu(tr("VideoCD"))
{
  vcd = Vcd;
  validList = false;

  if (vcd->isLabel()) {
     for (int i=1; i<=U16_TO_LE(vcd->vcdInfo.max_list_id); i++) {
        int offs = vcd->vcdInfo.offset_multiplier * U16_TO_LE(vcd->vcdLot.list_id_offset[i]);
        union psd_vcd *psdVcd = (union psd_vcd*)(&(vcd->vcdPsd[offs]));
        if (psdVcd->header==END_LIST_HEADER)
           break;
        else {
           Add(new cVcdPsdItem(i, psdVcd));
           validList = true;
           }
        }
     }
  else
     Add(new cOsdItem(tr("No VideoCD detected")));

  SetHelp(
     validList ? tr("Play") : NULL,
     NULL,
     vcd->DiscOk() ? tr("Eject") : NULL,
     NULL
     );
  Display();
}

cVcdPsd::~cVcdPsd()
{
}

eOSState cVcdPsd::Eject(void)
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

eOSState cVcdPsd::Play(void)
{
  if (validList) {
     cVcdPsdItem *vpi = (cVcdPsdItem*)Get(Current());
     if (vpi) {
        if (vpi->PsdVcd()->header==PLAY_LIST_HEADER) {
           __u16 item = U16_TO_LE(vpi->PsdVcd()->play.play_item[0]);
           if (PLAY_ITEM_TYP(item)==piTrack)
              cPsdVcdControl::SetTrack(PLAY_ITEM_NO(item), vcd, vpi->PsdVcd());
           else if (PLAY_ITEM_TYP(item)==piEntry)
              cPsdVcdControl::SetEntry(PLAY_ITEM_NO(item), vcd, vpi->PsdVcd());
           else if (PLAY_ITEM_TYP(item)==piSpi)
              cPsdSpiControl::SetItem(PLAY_ITEM_NO(item), vcd, vpi->PsdVcd());
           dsyslog("VCD: Lot %d  Play list", vpi->Lot());
           }
        else if (vpi->PsdVcd()->header==SELECTION_LIST_HEADER) {
           __u16 item = U16_TO_LE(vpi->PsdVcd()->selection.play_item);
           if (PLAY_ITEM_TYP(item)==piTrack)
              cPsdVcdControl::SetTrack(PLAY_ITEM_NO(item) , vcd, vpi->PsdVcd());
           else if (PLAY_ITEM_TYP(item)==piEntry)
              cPsdVcdControl::SetEntry(PLAY_ITEM_NO(item), vcd, vpi->PsdVcd());
           else if (PLAY_ITEM_TYP(item)==piSpi)
              cPsdSpiControl::SetItem(PLAY_ITEM_NO(item), vcd, vpi->PsdVcd());
           dsyslog("VCD: Lot %d  Selection list", vpi->Lot());
           }
        else
           dsyslog("VCD: Lot %d  Unknown", vpi->Lot());
        return osEnd;
        }
     }
  return osContinue;
}

eOSState cVcdPsd::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
        case kOk:
        case kRed:     return Play();
        case kYellow:  return Eject();
        default:       break;
        }
     }
  return state;
}

