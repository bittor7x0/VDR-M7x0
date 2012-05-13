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
 *   dvbmon.c: plugin framework
 *
 ***************************************************************************/

 
#include "i18n.h"
#include "dvbmon.h"

#include "menudvbmon.h"

// --- cPluginDVBmon  ---------------------------------------------------------

cPluginDVBmon::cPluginDVBmon(void) {
   // Initialize any member variables here.
   // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
   // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
   AutoScanStat = AssNone;
   setupData.cycleTime = 5;
}

cPluginDVBmon::~cPluginDVBmon() {
   // Clean up after yourself!
}

const char *cPluginDVBmon::CommandLineHelp(void) {
   // Return a string that describes all known command line options.
   return NULL;
}

bool cPluginDVBmon::ProcessArgs(int argc, char *argv[]) {
   // Implement command line argument processing here if applicable.
   return true;
}

bool cPluginDVBmon::Initialize(void) {
   // Initialize any background activities the plugin shall perform.
   return true;
}

bool cPluginDVBmon::Start(void) {
   // Start any background activities the plugin shall perform.
   RegisterI18n(Phrases);
   return true;
}

void cPluginDVBmon::Housekeeping(void) {
   // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginDVBmon::MainMenuAction(void) {

   // Perform the action when selected from the main VDR menu.
   return new cMenuDVBmon(&setupData);
}

cMenuSetupPage *cPluginDVBmon::SetupMenu(void) {
   // Return a setup menu in case the plugin supports one.
   return new cMenuSetupDVBmon(&setupData);
}

bool cPluginDVBmon::Service(const char *Id, void *Data)
{
   if (Id && strcmp(Id,"AutoScan")==0)
   {
      printf( " [dvbmon] Id Flag set: %s  DATA %s \n", Id, (char*)Data);
      if (Data && strcmp(static_cast<const char *>(Data),"DVB-S")==0)
      {
         AutoScanStat = AssDvbS;
      }
      else if (Data && strcmp(static_cast<const char *>(Data),"DVB-C")==0)
      {
         AutoScanStat = AssDvbC;
      }
      else if (Data && strcmp(static_cast<const char *>(Data),"DVB-T")==0)
      {
         AutoScanStat = AssDvbT;
      }
      else 
      {
         esyslog ("dvbmon: wrong Service Data: \"%s\" ", static_cast<const char *>(Data)); 
         AutoScanStat = AssNone;
      }
      return true;
   }
   return false;
}

bool cPluginDVBmon::SetupParse(const char *Name, const char *Value) {
   // Parse your own setup parameters and store their values.
   if (!strcasecmp(Name, "CycleTime")) {
      setupData.cycleTime = atoi(Value);
   } else {
      return false;
   }   
   return true;
}

VDRPLUGINCREATOR(cPluginDVBmon); // Don't touch this!
