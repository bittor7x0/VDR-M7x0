/*
 * setup.c: Svdrpservice setup
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/menuitems.h>
#include "i18n.h"
#include "setup.h"

cSvdrpServiceSetup SvdrpServiceSetup;

cSvdrpServiceSetup::cSvdrpServiceSetup() {
	serverIp[0] = 0;
	serverPort = 2001;
}

cSvdrpServiceSetup& cSvdrpServiceSetup::operator=(const cSvdrpServiceSetup &Setup) {
	strn0cpy(serverIp, Setup.serverIp, sizeof(serverIp));
	serverPort = Setup.serverPort;
	return *this;
}

bool cSvdrpServiceSetup::Parse(const char *Name, const char *Value) {
	if (!strcasecmp(Name, "ServerIp"))
		strn0cpy(serverIp, Value, sizeof(serverIp));
	else if (!strcasecmp(Name, "ServerPort"))
		serverPort = atoi(Value);
	else
		return false;
	return true;
}

void cSvdrpServiceMenuSetup::Store() {
	SetupStore("ServerIp", setupTmp.serverIp);
	SetupStore("ServerPort", setupTmp.serverPort);
	SvdrpServiceSetup = setupTmp;
}

cSvdrpServiceMenuSetup::cSvdrpServiceMenuSetup() {
	setupTmp = SvdrpServiceSetup;
	Add(new cMenuEditStrItem(tr("Default server IP"), setupTmp.serverIp, 15, ".1234567890"));
	Add(new cMenuEditIntItem(tr("Default server port"), &setupTmp.serverPort, 1, 65535));
}

cSvdrpServiceMenuSetup::~cSvdrpServiceMenuSetup() {
}
