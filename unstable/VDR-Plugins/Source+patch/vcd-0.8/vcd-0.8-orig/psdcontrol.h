/*
 * psdcontrol.h: PSD control of VCD replay
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


#include "functions.h"
#include "viewer.h"
#include "player.h"


// --- cPsdSpiControl -----------------------------------------------------------

class cPsdSpiControl : public cVcdViewerControl {
private:
  static int spi;
  static cVcd *vcd;
  static union psd_vcd *psdVcd;
  static int playInit;
  static int playTime;
  static int waitInit;
  static int waitTime;
  bool GotoListOffset(__u16 Offs);
public:
  cPsdSpiControl(void);
  cPsdSpiControl(int Loop);
  virtual ~cPsdSpiControl();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Hide(void) {}
  static void SetItem(int Spi, cVcd *Vcd, union psd_vcd *PsdVcd);
  };


// --- cPsdVcdControl --------------------------------------------------------

class cPsdVcdControl : public cVcdPlayerControl {
private:
  static int track;
  static cVcd *vcd;
  static union psd_vcd *psdVcd;
  static int playInit;
  static int playTime;
  static int waitInit;
  static int waitTime;
  bool GotoListOffset(__u16 Offs);
public:
  cPsdVcdControl(void);
  cPsdVcdControl(int Lba, int Loop, int AutoWait);
  virtual ~cPsdVcdControl();
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Hide(void) {}
  static void SetTrack(int Track, cVcd *Vcd, union psd_vcd *PsdVcd);
  static void SetEntry(int Entry, cVcd *Vcd, union psd_vcd *PsdVcd);
  };


