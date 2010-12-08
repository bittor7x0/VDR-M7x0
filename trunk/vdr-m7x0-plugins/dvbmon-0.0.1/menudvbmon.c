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
 *   menudvbmon.c: main osd 
 *
 ***************************************************************************/

#include <iostream>

#include "m7x0_dvb/frontend.h" 
#include <vdr/interface.h>
#include <vdr/diseqc.h> 
#include <vdr/menu.h> 
#include <sys/ioctl.h>

#include "debug.h"
#include "menudvbmon.h"

#define DVBFRONTEND "/dev/ost/frontend%d"

// --- cMenuDVBmon -------------------------------------------------------
#ifndef DBG  
#define DBG "DEBUG [cMenuDVBmon]: "
#endif

cMenuDVBmon::cMenuDVBmon(struct SetupData *pSetupData):cOsdMenu(tr("DVB Monitor"))
{
   struct freqinfo fi;
   
   if (pSetupData != NULL) {
      cycleTime = 1000*pSetupData->cycleTime;
   } else {
      cycleTime = 1000*5;
   }
   
   DLOG(DBG " cMenuDVBmon()");

   fd_frontend0 = openFrontend(0);
   fd_frontend1 = openFrontend(1);

   // collect frequencies of currently available channels
   for (int i = 1; i <= Channels.MaxNumber(); i++) {
      cChannel *channel = Channels.GetByNumber(i);
      if (channel) {
         FREQ2CHANNEL::iterator search = frequencies.find(channel->Frequency());
         if (search == frequencies.end()) {
            // Not yet in map
            fi.channel = channel;
            fi.currentInfo[0] = 0;
            fi.previousInfo[0] = 0;
            fi.BER = 0;
            fi.signal = 0;
            snprintf(fi.channels, sizeof(fi.channels), "%s", channel->ShortName(true));
            frequencies.insert(FREQ2CHANNEL::value_type(channel->Frequency(), fi) );
         } else {
            // already in map, append channel name
            char buf[100];
            snprintf(buf, sizeof(buf), "%s %s", search->second.channels, channel->ShortName(true));
            strncpy(search->second.channels, buf, sizeof(search->second.channels));
         }
      }
   }
   f_current = frequencies.begin();
   deviceAvailable = tryCurrentFrequency();
   
   cycleMode = false;
   cycleTimer = cTimeMs::Now() + cycleTime;
   
   Setup();
}

bool cMenuDVBmon::changeFrequency(bool up) {
   if (frequencies.begin() == frequencies.end()) {
      return false;
   } else {
      if (up) {
         f_current++;
         if (f_current == frequencies.end())
            f_current = frequencies.begin();
      } else {
         if (f_current == frequencies.begin())
            f_current = frequencies.end();
         f_current--;
      }
      return true;
   }
}

bool cMenuDVBmon::tryCurrentFrequency() {
   cDevice *dev = cDevice::GetDevice(1);
   if (dev != NULL && f_current != frequencies.end()) {
      bool NeedsDetachReceivers;
      NeedsDetachReceivers = true;
      if (dev->ProvidesChannel(f_current->second.channel, 0, &NeedsDetachReceivers, false)) {
         if (!NeedsDetachReceivers) {
            dev->SwitchChannel(f_current->second.channel, false);
            return true;
         }
      }
   }
   return false;
}

cMenuDVBmon::~cMenuDVBmon()
{
   closeFrontend(fd_frontend0);
   fd_frontend0 = 0;
   closeFrontend(fd_frontend1);
   fd_frontend1 = 0;
   frequencies.clear();
}

void cMenuDVBmon::Setup()
{
   char buf[100];
   cDevice *dev;
   int freq;
   static int old_freq = 0;
   int curBER, curSignal;
   FREQ2CHANNEL::iterator search;
   
   // DLOG(DBG " Setup()");
   
   Clear();

   freq = 0;
   dev = cDevice::GetDevice(0);
   if (dev != NULL) {
      for (int i = 1; i <= Channels.MaxNumber(); i++) {
         cChannel *channel = Channels.GetByNumber(i);
         if (dev->IsTunedToTransponder(channel)) {
            freq = channel->Frequency();
            break;
         }
      }
   }
   
   search = frequencies.find(freq);   
   snprintf(buf, sizeof(buf), "Tuner 1: %6.1fMHz  %s", freq*1e-6, (search == frequencies.end() ? "empty channel list" : search->second.channels) );
   Add(new cMenuTextInfo(buf));
   Add(new cMenuStatusBar(150, getSignal(fd_frontend0), 1));
	Add(new cMenuStatusBar(25 , getBER(fd_frontend0),    2));

   Add(new cMenuTextInfo("---------------------------------------------------------------------------------------------------------------"));
   
   freq = 0;
   dev = cDevice::GetDevice(1);
   if (dev != NULL) {
      for (int i = 1; i <= Channels.MaxNumber(); i++) {
         cChannel *channel = Channels.GetByNumber(i);
         if (dev->IsTunedToTransponder(channel)) {
            freq = channel->Frequency();
            break;
         }
      }
   }   
   
   curSignal = getSignal(fd_frontend1);
   curBER    = getBER(fd_frontend1);
   
   if (!deviceAvailable) {
      Add(new cMenuTextInfo("Tuner 2 not available!"));
      SetHelp(tr("Button$Done"), NULL, NULL, NULL);
   } else if (frequencies.begin() == frequencies.end()) {
      Add(new cMenuTextInfo("Empty channel list ?!?"));
      SetHelp(tr("Button$Done"), NULL, NULL, NULL);
   } else {   
      if (f_current != frequencies.end() && f_current->first == freq) {
         if (old_freq != freq) {
            old_freq = freq;
            snprintf(buf, sizeof(buf), "%s%s", f_current->second.currentInfo, f_current->second.previousInfo);
            snprintf(f_current->second.previousInfo, sizeof(f_current->second.previousInfo), buf);
         }
         f_current->second.BER = curBER;
         f_current->second.signal = curSignal;
         
         char s[20];
         s[0] = '|';
         for (int i = 1; i < 11; i++)
            s[i] = (curBER > i*2 ? (i == 6 ? '.' : '|') : ' ');
         s[11] = ' ';
         s[12] = 0;
         
         snprintf(f_current->second.currentInfo, sizeof(f_current->second.currentInfo), "%s", s);
      }
      
      Add(new cMenuTextInfo("Frequency  Q curr.    Q prev."));
      for (FREQ2CHANNEL::iterator f_iter = frequencies.begin( ); f_iter != frequencies.end( ); f_iter++ ) {
         snprintf(buf, sizeof(buf), "%6.1fMHz  %s   %s", f_iter->first*1e-6, f_iter->second.currentInfo, f_iter->second.previousInfo);
         Add(new cMenuTextInfo(buf));
      }
      
      if (cycleMode)
         SetHelp(tr("Button$Done"), NULL, NULL, tr("Button$Manually"));
      else
         SetHelp(tr("Button$Done"), tr("Button$Freq+"), tr("Button$Freq-"), tr("Button$Auto"));
   }
   
   search = frequencies.find(freq);   
   snprintf(buf, sizeof(buf), "Tuner 2: %6.1fMHz  %s", freq*1e-6, (search == frequencies.end() ? "empty channel list" : search->second.channels) );
   Add(new cMenuTextInfo(buf));
   Add(new cMenuStatusBar(150, curSignal, 1));
	Add(new cMenuStatusBar(25 , curBER,    2));

   Display();
   
   if (cycleMode && cTimeMs::Now() > cycleTimer) {
      cycleTimer = cTimeMs::Now() + cycleTime;
      changeFrequency(true);
      tryCurrentFrequency();
   }   
}

eOSState cMenuDVBmon::ProcessKey(eKeys Key)
{
   eOSState state = cOsdMenu::ProcessKey(Key);

   if (state == osUnknown) {
      switch (Key) 
      {
         case kRed:
         case kBack:
            // shut down scanning 
            state = osBack;
            break;
         case kGreen:
            if (!cycleMode) {
               changeFrequency(true);
               deviceAvailable = tryCurrentFrequency();
            }
            state = osContinue;
            break;
         case kYellow:
            if (!cycleMode) {
               changeFrequency(false);
               deviceAvailable = tryCurrentFrequency();
            }
            state = osContinue;
            break;
         case kBlue:
            cycleMode = !cycleMode;
            state = osContinue;
            break;
         default:
            state = osContinue;
            break;
      }
   Setup();
   }    
   return state;
}

int cMenuDVBmon::openFrontend(int cardnr)
{
   char buffer[100];
   int fd_frontend = 0;
   
   snprintf(buffer, sizeof(buffer), DVBFRONTEND, cardnr);

   fd_frontend = open(buffer,O_RDONLY | O_NONBLOCK);
   if (fd_frontend<=0)
   {
      DLOG(DBG "cant open device: %s ", buffer);
   }
   return fd_frontend;
}

void cMenuDVBmon::closeFrontend(int fd_frontend)
{
   if (fd_frontend > 0)
      close(fd_frontend);   
}

//-------------------------------------------------------------------------
uint32_t cMenuDVBmon::getSignal(int fd_frontend)
{
   uint32_t value = 0;
   if (fd_frontend > 0)
      CHECK(ioctl(fd_frontend, FE_READ_SIGNAL_STRENGTH, &value));
   return value;
}

uint32_t cMenuDVBmon::getSNR(int fd_frontend)
{
   uint32_t value = 0;
   if (fd_frontend > 0)
      CHECK(ioctl(fd_frontend, FE_READ_SNR, &value));
   return value;
}

uint32_t cMenuDVBmon::getBER(int fd_frontend)
{
   uint32_t value = 0;
   if (fd_frontend > 0)
      CHECK(ioctl(fd_frontend, FE_READ_BER, &value));
   return value;
}

uint16_t cMenuDVBmon::getStatus(int fd_frontend)
{
   fe_status_t value;
   if (fd_frontend > 0)
      CHECK(ioctl(fd_frontend, FE_READ_STATUS, &value));
   return value;
}

//-------------------------------------------------------------------------

// --- cMenuTextInfo ---------------------------------------------

cMenuTextInfo::cMenuTextInfo(const char *text)
{
   SetText(text);
   SetSelectable(false);
}

// --- cMenuStatusBar ---------------------------------------------

cMenuStatusBar::cMenuStatusBar(int Total, int Current, int Mode)
{
   int barWidth = 50;
   int percent;
   Current +=1;

   if (Current > Total)
   {
      Current = Total;
   }
   if (Total < 1)
   {
      Total = 1;
   }

   // GetReal EditableWidth
   percent = static_cast<int>(((Current) * barWidth/(Total)));

   char buffer[barWidth+1];
   int i;

   buffer[0] = '[';
   for (i = 1;i < barWidth; i++)
      i < percent ?  buffer[i] = '|' : buffer[i] = ' ';

   buffer[i++] = ']';
   buffer[i] = '\0';

   char *tmp;
   int l = 0;
   
   if(Mode == 1)
      l = asprintf(&tmp,"%s   %d / %d  (Signal)", buffer, Current, Total);
   if(Mode == 2)
      l = asprintf(&tmp,"%s   %d / %d  (BER)", buffer, Current, Total);
   if(Mode == 3)
      l = asprintf(&tmp,"%s   %d / %d  (SNR)", buffer, Current, Total);

   SetText(strndup(tmp,l), false);
   SetSelectable(false);
   free(tmp);
}
