/*
 * setup.h: Settings
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _REMOTEOSD_SETUP__H
#define _REMOTEOSD_SETUP__H

#include <vdr/config.h>
#include <vdr/themes.h>

#define MAX_IP_LENGTH 16

struct cRemoteOsdSetup {
	int hideMainMenuEntry;
	int replaceSchedule;
	int replaceTimers;
	char serverIp[MAX_IP_LENGTH];
	int serverPort;
	int tuneServer;
	int maxItems;
	char remoteTheme[MaxThemeName];

	cRemoteOsdSetup& operator=(const cRemoteOsdSetup &Setup);
	cRemoteOsdSetup();
};

extern cRemoteOsdSetup RemoteOsdSetup;

class cRemoteOsdMenuSetup: public cMenuSetupPage {
	private:
		cRemoteOsdSetup	setupTmp;
		cThemes		themes;
		int		themeIndex;
		const char **	themeList;
	protected:
		virtual void Store(void);
	public:
		cRemoteOsdMenuSetup();
		virtual ~cRemoteOsdMenuSetup();
};

#endif //_REMOTEOSD_SETUP__H
