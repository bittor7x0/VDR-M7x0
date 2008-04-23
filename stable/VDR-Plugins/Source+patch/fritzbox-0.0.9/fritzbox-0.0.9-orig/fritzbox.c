/*
 * Fritz!Box plugin for VDR
 *
 * Copyright (C) 2007 Joachim Wilke <vdr@joachim-wilke.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

#include <vector>
#include <vdr/plugin.h>
#include "calllist.h"
#include "fonbuchmanager.h"
#include "fritzlistener.h"
#include "fritzfonbuch.h"
#include "i18n.h"
#include "setup.h"
#include "menu.h"

static const char *VERSION        = "0.0.9";
static const char *DESCRIPTION    = "Fritz!Box Plugin for AVM Fritz!Box";
static const char *MAINMENUENTRY  = "Fritz!Box phone book";

class cPluginFritzbox : public cPlugin {
private:
	cFonbuch *fonbuch;
	cFritzListener *listener;
	cCallList *callList;
public:
	cPluginFritzbox(void);
	virtual ~cPluginFritzbox();
	virtual const char *Version(void) { return VERSION; }
	virtual const char *Description(void) { return DESCRIPTION; }
	virtual const char *CommandLineHelp(void);
	virtual bool ProcessArgs(int argc, char *argv[]);
	virtual bool Initialize(void);
	virtual bool Start(void);
	virtual void Stop(void);
	virtual void Housekeeping(void);
	virtual void MainThreadHook(void);
	virtual cString Active(void);
	virtual time_t WakeupTime(void);
	virtual const char *MainMenuEntry(void) { return fritzboxConfig.hideMainMenu ? NULL : tr(MAINMENUENTRY); }
	virtual cOsdObject *MainMenuAction(void);
	virtual cMenuSetupPage *SetupMenu(void);
	virtual bool SetupParse(const char *Name, const char *Value);
	virtual bool Service(const char *Id, void *Data = NULL);
	virtual const char **SVDRPHelpPages(void);
	virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

cPluginFritzbox::cPluginFritzbox(void)
{
	// Initialize any member variables here.
	// DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
	// VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginFritzbox::~cPluginFritzbox()
{
	// Clean up after yourself!
}

const char *cPluginFritzbox::CommandLineHelp(void)
{
	// Return a string that describes all known command line options.
	return NULL;
}

bool cPluginFritzbox::ProcessArgs(int argc, char *argv[])
{
	// Implement command line argument processing here if applicable.
	return true;
}

bool cPluginFritzbox::Initialize(void)
{
	// Initialize any background activities the plugin shall perform.
	return true;
}

bool cPluginFritzbox::Start(void)
{
	// Start any background activities the plugin shall perform.
#if VDRVERSNUM < 10507	
	RegisterI18n(Phrases);
#endif
	fonbuch = cFonbuchManager::GetFonbuch();
	callList = new cCallList();
	listener = new cFritzListener(fritzboxConfig.url, PORT_MONITOR, fonbuch, callList);
	listener->Start();
	return true;
}


void cPluginFritzbox::Stop(void)
{
	// Stop any background activities the plugin shall perform.
	delete listener;
}

void cPluginFritzbox::Housekeeping(void)
{
	// Perform any cleanup or other regular tasks.
}

void cPluginFritzbox::MainThreadHook(void)
{
	char *msg = listener->GetMessage();
	if (msg) {
		Skins.Message(mtInfo, msg);
	}
}

cString cPluginFritzbox::Active(void)
{
	// Return a message string if shutdown should be postponed
	return NULL;
}

time_t cPluginFritzbox::WakeupTime(void)
{
	// Return custom wakeup time for shutdown script
	return 0;
}

cOsdObject *cPluginFritzbox::MainMenuAction(void)
{
	return new cMenuFritzbox(callList);
}

cMenuSetupPage *cPluginFritzbox::SetupMenu(void)
{
	// Return a setup menu in case the plugin supports one.
	return new cMenuSetupFritzbox( );
}

bool cPluginFritzbox::SetupParse(const char *Name, const char *Value)
{
	// Parse your own setup parameters and store their values.
	return fritzboxConfig.SetupParse(Name, Value);
}

bool cPluginFritzbox::Service(const char *Id, void *Data)
{
	// Handle custom service requests from other plugins
	return false;
}

const char **cPluginFritzbox::SVDRPHelpPages(void)
{
	// Return help text for SVDRP commands this plugin implements
	return NULL;
}

cString cPluginFritzbox::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
	// Process SVDRP commands this plugin implements
	return NULL;
}

VDRPLUGINCREATOR(cPluginFritzbox); // Don't touch this!
