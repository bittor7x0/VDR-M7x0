/***************************************************************************
 *   Copyright (C) 2005 by Reel Multimedia                                 *
 *                                                                         *
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
 *   channelscan: A plugin for VDR 
 *
 ***************************************************************************/
 /* kate: set expandtab tabstop=4 shiftwidth=4: */

#include "i18n.h"
#include "service.h"
#include "csmenu.h"
#include "channelscan.h"

int oldChannelNumbers = 0; //XXX

eAutoScanStat cPluginChannelscan::AutoScanStat = AssNone;

int cMenuChannelscanSetup::SetupServiceType = 0;

// --- cMenuChannelscanSetup  ------------------------------------------------------

cMenuChannelscanSetup::cMenuChannelscanSetup()
{
  serviceType = cMenuChannelscanSetup::SetupServiceType;
  serviceTypeTexts[0]=tr("TV only");
  serviceTypeTexts[1]=tr("Radio only");
  serviceTypeTexts[2]=tr("Radio + TV");
  Setup();
}

void cMenuChannelscanSetup::Setup()
{
  int current = Current();
  Clear();
  serviceType = SetupServiceType;
  Add(new cMenuEditStraItem(tr("Service Type"), &serviceType, 3, serviceTypeTexts));

  SetCurrent(Get(current));
  Display();
}

void cMenuChannelscanSetup::Store(void)
{
  SetupStore("ServiceType", cMenuChannelscanSetup::SetupServiceType = serviceType);
}


// --- cPluginChannelscan  ---------------------------------------------------------

cPluginChannelscan::cPluginChannelscan(void) {
   // Initialize any member variables here.
   // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
   // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginChannelscan::~cPluginChannelscan() {
   // Clean up after yourself!
}

const char *cPluginChannelscan::CommandLineHelp(void) {
   // Return a string that describes all known command line options.
   return NULL;
}

bool cPluginChannelscan::ProcessArgs(int argc, char *argv[]) {
   // Implement command line argument processing here if applicable.
   return true;
}

bool cPluginChannelscan::Initialize(void) {
   // Initialize any background activities the plugin shall perform.
   return true;
}

bool cPluginChannelscan::Start(void) {
   // Start any background activities the plugin shall perform.
   RegisterI18n(Phrases);
   return true;
}

void cPluginChannelscan::Housekeeping(void) {
   // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginChannelscan::MainMenuAction(void) {

   // Perform the action when selected from the main VDR menu.
   oldChannelNumbers = Channels.MaxNumber();
    int currentChannel = cDevice::CurrentChannel();
   return new cMenuChannelscan(currentChannel);
}

cMenuSetupPage *cPluginChannelscan::SetupMenu(void) {
   // Return a setup menu in case the plugin supports one.
   return new cMenuChannelscanSetup;
}

bool cPluginChannelscan::Service(const char *Id, void *Data)
{

  if (Id && strcmp(Id,"AutoScan")==0)
  {
     printf( " [channelscan] Id Flag set: %s  DATA %s \n", Id, (char*)Data);
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
        esyslog ("channelscan: wrong Service Data: \"%s\" ", static_cast<const char *>(Data)); 
        AutoScanStat = AssNone;
     }
     return true;
  }
  return false;
}

bool cPluginChannelscan::SetupParse(const char *Name, const char *Value) {
   // Parse your own setup parameters and store their values.
   if (!strcasecmp(Name, "ServiceType")) 
   {
     cMenuChannelscanSetup::SetupServiceType = atoi(Value);
   }
   else
   {
     return false;
   }
   return true;
}

VDRPLUGINCREATOR(cPluginChannelscan); // Don't touch this!

