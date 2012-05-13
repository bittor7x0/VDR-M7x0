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
 *   channelscan: plugin for VDR 
 *
 ***************************************************************************/

#ifndef __CHANNELSCAN_H
#define __CHANNELSCAN_H

#include <vdr/plugin.h>

static const char *VERSION        = "0.4.3";
static const char *DESCRIPTION    = "Search Transponders for DVB Channels";
static const char *MAINMENUENTRY  = "Channelscan";

enum eAutoScanStat { 
		AssNone,
		AssDvbS,
		AssDvbC,
		AssDvbT
};


//extern volatile eScan ScanStat;

// --- cPluginChannelscan ---------------------------------------------------------

class cPluginChannelscan : public cPlugin {
private:
	// Add any member variables or functions you may need here.
public:
	cPluginChannelscan();
	virtual ~cPluginChannelscan();
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
    static eAutoScanStat AutoScanStat;
};

// --- cSetupMenu ------------------------------------------------------
// Move to menu.c

class cMenuChannelscanSetup : public cMenuSetupPage
{
private:
	int serviceType;
	const char *serviceTypeTexts[3];
	virtual void Setup(void);
protected:
	//virtual eOSState ProcessKey(eKeys Key);
	virtual void Store(void);
public:
	cMenuChannelscanSetup(void);
    static int SetupServiceType;
};

#endif //  __CHANNELSCAN_H
