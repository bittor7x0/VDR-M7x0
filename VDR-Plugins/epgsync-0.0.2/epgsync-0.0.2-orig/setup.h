/*
 * setup.h: Settings
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _EPGSYNC_SETUP__H
#define _EPGSYNC_SETUP__H

#include <vdr/config.h>

#define MAX_IP_LENGTH 16

struct cEpgSyncSetup {
	int hideMainMenuEntry;
	char serverIp[MAX_IP_LENGTH];
	int serverPort;
	int connectAttempts;
	int nowNext;
	int channelByChannel;
	int syncOnStart;

	bool Parse(const char *Name, const char *Value);
	cEpgSyncSetup& operator=(const cEpgSyncSetup &Setup);
	cEpgSyncSetup();
};

extern cEpgSyncSetup EpgSyncSetup;

class cEpgSyncMenuSetup: public cMenuSetupPage {
	private:
		cEpgSyncSetup setupTmp;
	protected:
		virtual void Store(void);
	public:
		cEpgSyncMenuSetup();
		virtual ~cEpgSyncMenuSetup();
};

#endif //_EPGSYNC_SETUP__H
