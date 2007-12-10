/*
 * remoteosd.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/menuitems.h>
#include <vdr/skins.h>
#include "i18n.h"
#include "setup.h"

cRemoteOsdSetup RemoteOsdSetup;

cRemoteOsdSetup::cRemoteOsdSetup() {
	hideMainMenuEntry = 0;
	replaceSchedule = 0;
	replaceTimers = 0;
	serverIp[0] = 0;
	serverPort = 2001;
	tuneServer = 0;
	maxItems = 0;
	remoteTheme[0] = 0;
}

cRemoteOsdSetup& cRemoteOsdSetup::operator=(const cRemoteOsdSetup &Setup) {
	hideMainMenuEntry = Setup.hideMainMenuEntry;
	replaceSchedule = Setup.replaceSchedule;
	replaceTimers = Setup.replaceTimers;
	strn0cpy(serverIp, Setup.serverIp, sizeof(serverIp));
	serverPort = Setup.serverPort;
	tuneServer = Setup.tuneServer;
	maxItems = Setup.maxItems;
	strn0cpy(remoteTheme, Setup.remoteTheme, sizeof(remoteTheme));
	return *this;
}

void cRemoteOsdMenuSetup::Store() {
	setupTmp.remoteTheme[0] = 0;
	if (themeIndex > 0)
		strn0cpy(setupTmp.remoteTheme, themes.Name(themeIndex - 1), sizeof(setupTmp.remoteTheme));

	SetupStore("HideMainMenuEntry", setupTmp.hideMainMenuEntry);
	SetupStore("ReplaceSchedule", setupTmp.replaceSchedule);
	SetupStore("ReplaceTimers", setupTmp.replaceTimers);
	SetupStore("ServerIp", setupTmp.serverIp);
	SetupStore("ServerPort", setupTmp.serverPort);
	SetupStore("TuneServer", setupTmp.tuneServer);
	SetupStore("MaxItems", setupTmp.maxItems);
	SetupStore("RemoteTheme", setupTmp.remoteTheme);
	RemoteOsdSetup = setupTmp;
}

cRemoteOsdMenuSetup::cRemoteOsdMenuSetup() {
	setupTmp = RemoteOsdSetup;

	themes.Load(Skins.Current()->Name());
	int numThemes = themes.NumThemes();
	const char* const* descriptions = themes.Descriptions();
	themeList = MALLOC(const char*, numThemes + 1);
	themeIndex = 0;
	themeList[0] = strdup("");
	for (int i = 0; i < themes.NumThemes(); i++) {
		themeList[i + 1] = descriptions[i];
		if (strcmp(setupTmp.remoteTheme, themes.Name(i)) == 0)
			themeIndex = i + 1;
	}

	Add(new cMenuEditBoolItem(tr("Hide mainmenu entry"), &setupTmp.hideMainMenuEntry));
#ifdef MAINMENUHOOKSVERSNUM
	Add(new cMenuEditBoolItem(tr("Replace mainmenu \"Schedule\""), &setupTmp.replaceSchedule));
	Add(new cMenuEditBoolItem(tr("Replace mainmenu \"Timers\""), &setupTmp.replaceTimers));
#endif
	Add(new cMenuEditStrItem(tr("Server IP"), setupTmp.serverIp, 15, ".1234567890"));
	Add(new cMenuEditIntItem(tr("Server port"), &setupTmp.serverPort, 0, 65535));
	Add(new cMenuEditBoolItem(tr("Tune server channel"), &setupTmp.tuneServer));
	Add(new cMenuEditIntItem(tr("Number of lines per page"), &setupTmp.maxItems, 0));
	Add(new cMenuEditStraItem(tr("Remote menu theme"), &themeIndex, numThemes + 1, themeList));
}

cRemoteOsdMenuSetup::~cRemoteOsdMenuSetup() {
	free((void *) themeList[0]);
	free(themeList);
}
