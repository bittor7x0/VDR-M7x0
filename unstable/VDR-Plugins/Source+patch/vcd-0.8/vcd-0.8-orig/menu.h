/*
 * menu.h: OSD menus
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


#ifndef __VCD_MENU_H
#define __VCD_MENU_H


#include <vdr/osd.h>
#include "functions.h"


class cMenu : public cOsdMenu {
protected:
  cVcd *vcd;
  bool validLabel;
  char vcdTitle[17];
  eOSState Eject(void);
  void SetHelp(const char *Red, const char *Green, const char *Blue);
public:
  cMenu(cVcd *Vcd);
  ~cMenu();
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMenuSpi : public cMenu {
private:
  bool validItems;
  bool ListItems(void);
  eOSState Play(void);
public:
  cMenuSpi(cVcd *Vcd);
  ~cMenuSpi();
  virtual eOSState ProcessKey(eKeys Key);
  };

class cMenuVcd : public cMenu {
private:
  bool validTracks;
  bool spi;
  bool ListTracks(void);
  eOSState Play(void);
public:
  cMenuVcd(cVcd *Vcd);
  ~cMenuVcd();
  virtual eOSState ProcessKey(eKeys Key);
  };


#endif //__VCD_MENU_H
