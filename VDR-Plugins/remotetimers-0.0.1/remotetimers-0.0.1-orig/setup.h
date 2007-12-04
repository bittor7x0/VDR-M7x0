/*
 * setup.h: Settings
 *
 * See the README file for copyright information and how to reach the author.
 */

#ifndef _REMOTETIMERS_SETUP__H
#define _REMOTETIMERS_SETUP__H

#include <vdr/config.h>

#define MAX_IP_LENGTH 16

struct cRemoteTimersSetup {
	int hideMainMenuEntry;
	int replaceSchedule;
	int replaceTimers;
	char serverIp[MAX_IP_LENGTH];
	int serverPort;
	int userFilter;
	int defaultUser;
	int addToRemote;

	bool Parse(const char *Name, const char *Value);
	cRemoteTimersSetup& operator=(const cRemoteTimersSetup &Setup);
	cRemoteTimersSetup();
};

extern cRemoteTimersSetup RemoteTimersSetup;

class cRemoteTimersMenuSetup: public cMenuSetupPage {
	private:
		cRemoteTimersSetup setupTmp;
	protected:
		virtual void Store(void);
	public:
		cRemoteTimersMenuSetup();
		virtual ~cRemoteTimersMenuSetup();
};

#endif //_REMOTETIMERS_SETUP__H
