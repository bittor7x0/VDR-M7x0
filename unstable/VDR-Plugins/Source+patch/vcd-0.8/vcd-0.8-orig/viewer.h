/*
 * viewer.h: Viewer core
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


#ifndef __VCD_VIEWER_H
#define __VCD_VIEWER_H

#include <vdr/player.h>
#include <vdr/thread.h>
#include <vdr/remux.h>
#include <vdr/ringbuffer.h>
#include "functions.h"

#define PACK_SC     0xBA

class cVcdViewer;

class cVcdViewerControl : public cControl {
private:
  cVcdViewer *viewer;
public:
  cVcdViewerControl(int Spi, cVcd *Vcd);
  virtual ~cVcdViewerControl();
  bool Active(void);
  bool Still(void);
  int GetSpi(void);
  void Stop(void);
  bool SkipItems(int Items);
  void GotoItem(int Spi);
  void ToggleStillRes(void);
  const char *DeviceName(void);
  };

#endif //__VCD_VIEWER_H
