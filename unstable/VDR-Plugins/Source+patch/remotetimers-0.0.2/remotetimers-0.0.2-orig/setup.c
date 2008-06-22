/*
 * remotetimers.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/menuitems.h>
#include <vdr/config.h>
#include "setup.h"
#include "i18n.h"

cRemoteTimersSetup RemoteTimersSetup;

cRemoteTimersSetup::cRemoteTimersSetup() {
	hideMainMenuEntry = 0;
	replaceSchedule = 0;
	replaceTimers = 0;
	serverIp[0] = 0;
	serverPort = 2001;
	userFilter = 0;
	defaultUser = 0;
	addToRemote = 1;
}

cRemoteTimersSetup& cRemoteTimersSetup::operator=(const cRemoteTimersSetup &Setup) {
	hideMainMenuEntry = Setup.hideMainMenuEntry;
	replaceSchedule = Setup.replaceSchedule;
	replaceTimers = Setup.replaceTimers;
	strn0cpy(serverIp, Setup.serverIp, sizeof(serverIp));
	serverPort = Setup.serverPort;
	userFilter = Setup.userFilter;
	defaultUser = Setup.defaultUser;
	addToRemote = Setup.addToRemote;
	return *this;
}

bool cRemoteTimersSetup::Parse(const char *Name, const char *Value) {
	if (!strcasecmp(Name, "HideMainMenuEntry"))
		hideMainMenuEntry = atoi(Value);
	else if (!strcasecmp(Name, "ReplaceSchedule"))
		replaceSchedule = atoi(Value);
	else if (!strcasecmp(Name, "ReplaceTimers"))
		replaceTimers = atoi(Value);
	else if (!strcasecmp(Name, "ServerIp"))
		strn0cpy(serverIp, Value, sizeof(serverIp));
	else if (!strcasecmp(Name, "ServerPort"))
		serverPort = atoi(Value);
	else if (!strcasecmp(Name, "UserFilter"))
		userFilter = atoi(Value);
	else if (!strcasecmp(Name, "DefaultUser"))
		defaultUser = atoi(Value);
	else if (!strcasecmp(Name, "AddToRemote"))
		addToRemote = atoi(Value);
	else
		return false;
	return true;
}

void cRemoteTimersMenuSetup::Store() {
	SetupStore("HideMainMenuEntry", setupTmp.hideMainMenuEntry);
	SetupStore("ReplaceSchedule", setupTmp.replaceSchedule);
	SetupStore("ReplaceTimers", setupTmp.replaceTimers);
	SetupStore("ServerIp", setupTmp.serverIp);
	SetupStore("ServerPort", setupTmp.serverPort);
	SetupStore("UserFilter", setupTmp.userFilter);
	SetupStore("DefaultUser", setupTmp.defaultUser);
	SetupStore("AddToRemote", setupTmp.addToRemote);
	RemoteTimersSetup = setupTmp;
}

cRemoteTimersMenuSetup::cRemoteTimersMenuSetup() {
	setupTmp = RemoteTimersSetup;
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Hide mainmenu entry"), &setupTmp.hideMainMenuEntry));
#ifdef MAINMENUHOOKSVERSNUM
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Replace mainmenu \"Schedule\""), &setupTmp.replaceSchedule));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Replace mainmenu \"Timers\""), &setupTmp.replaceTimers));
#endif
	Add(new cMenuEditStrItem(trREMOTETIMERS("Server IP"), setupTmp.serverIp, 15, ".1234567890"));
	Add(new cMenuEditIntItem(trREMOTETIMERS("Server port"), &setupTmp.serverPort, 0, 65535));
	Add(new cOsdItem(trREMOTETIMERS("Defaults for new timers"), osUnknown, false));
	Add(new cMenuEditBoolItem(trREMOTETIMERS("Location"), &setupTmp.addToRemote, trREMOTETIMERS("local"), trREMOTETIMERS("remote")));
	Add(new cMenuEditIntItem(trREMOTETIMERS("User ID"), &setupTmp.defaultUser, 0, 9));
	Add(new cOsdItem(trREMOTETIMERS("Settings for \"Timers\" menu"), osUnknown, false));
	Add(new cMenuEditIntItem(trREMOTETIMERS("Filter for user ID"), &setupTmp.userFilter, 0, 9));
}

cRemoteTimersMenuSetup::~cRemoteTimersMenuSetup() {
}
