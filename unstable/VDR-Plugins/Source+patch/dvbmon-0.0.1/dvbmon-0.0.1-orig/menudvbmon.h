/***************************************************************************
 *   Copyright (C) 2009 by Hannes Brunner                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *
 *************************************************************************** 
 *   
 *   dvbmon: plugin for VDR 
 *   menudvbmon.h: main osd 
 *
 ***************************************************************************/

#ifndef __MENUDVBMON_H
#define __MENUDVBMON_H

#include <vdr/osdbase.h>
#include <vdr/channels.h>
#include <vdr/menuitems.h>

#include <map>

#include "menusetup.h"

using std::map;

// --- cMenuDVBmon ----------------------------------------------------

struct freqinfo {
   cChannel *channel;
   char currentInfo[100];
   char previousInfo[100];
   char channels[100];
   int BER;
   int signal;
};

typedef map<int, struct freqinfo> FREQ2CHANNEL;

class cMenuDVBmon : public cOsdMenu {
private:
   FREQ2CHANNEL frequencies;
   FREQ2CHANNEL::iterator f_current;
   bool changeFrequency(bool up);
   bool tryCurrentFrequency();
   bool cycleMode;
   int cycleTime;
   bool deviceAvailable;
   uint64_t cycleTimer;
   int fd_frontend0;
   int fd_frontend1;
   struct SetupData *pSetupData;
   uint32_t getSignal(int fd_frontend);
   uint32_t getSNR(int fd_frontend);
   uint32_t getBER(int fd_frontend);
   uint16_t getStatus(int fd_frontend);
   int openFrontend(int cardnr);
   void closeFrontend(int fd_frontend);
public:
   cMenuDVBmon(struct SetupData *pSetupData);
   ~cMenuDVBmon();
   eOSState ProcessKey(eKeys Key);
   void Setup();
   virtual bool NoAutoClose(void) { return true; }
};

// --- Class cMenuTextInfo ----------------------------------------------------
class cMenuTextInfo : public cOsdItem {
private:
public:
   cMenuTextInfo(const char *text);
};

// --- Class cMenuStatusBar ----------------------------------------------------
class cMenuStatusBar : public cOsdItem {
private:
public:
   cMenuStatusBar(int total, int current, int mode);
};

#endif // __MENUDVBMON_H

