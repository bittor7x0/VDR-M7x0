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
 *   dvbmon.h: plugin framework
 *
 ***************************************************************************/

#ifndef __DVBMON_H
#define __DVBMON_H

#include <vdr/plugin.h>
#include "menusetup.h"

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Monitor signal of DVB Channels";
static const char *MAINMENUENTRY  = "DVB Monitor";

enum eAutoScanStat { 
   AssNone,
   AssDvbS,
   AssDvbC,
   AssDvbT
};

// --- cPluginDVBmon ---------------------------------------------------------

class cPluginDVBmon : public cPlugin {
private:
   // Add any member variables or functions you may need here.
   eAutoScanStat AutoScanStat;
public:
   struct SetupData setupData;
   cPluginDVBmon();
   virtual ~cPluginDVBmon();
   virtual const char *Version(void) { return VERSION; }
   virtual const char *Description(void) { return tr(DESCRIPTION); }
   virtual const char *CommandLineHelp(void);
   virtual bool ProcessArgs(int argc, char *argv[]);
   virtual bool Initialize(void);
   virtual bool Start(void);
   virtual void Housekeeping(void);
   virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
   virtual cOsdObject *MainMenuAction(void);
   virtual cMenuSetupPage *SetupMenu(void);
   virtual bool SetupParse(const char *Name, const char *Value);
   virtual bool Service(const char *Id,void *Data = NULL);
   virtual bool HasSetupOptions(void) { return true; }
};

#endif //  __DVBMON_H
