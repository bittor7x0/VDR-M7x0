/*
 * vdrmon.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/thread.h>
#include <vdr/status.h>
#include <vdr/device.h>
#include <vdr/player.h>
#include "vdrmanagerthread.h"

#define ANDROVDR_PORT		6420

const char *VERSION = "0.8";
static const char *DESCRIPTION = "VDR-Manager support plugin";

class cPluginAndroVdr: public cPlugin {
private:
	// Add any member variables or functions you may need here.
	cAndroVdrThread * Thread;
	int port;
	const char * password;
	bool forceCheckSvdrp;
protected:
public:
	cPluginAndroVdr(void);
	virtual ~cPluginAndroVdr();
	virtual const char *Version(void) {
		return VERSION;
	}
	virtual const char *Description(void) {
		return DESCRIPTION;
	}
	virtual const char *CommandLineHelp(void);
	virtual bool Initialize(void);
	virtual bool Start(void);
	virtual void Stop(void);
	virtual void Housekeeping(void);
	virtual const char *MainMenuEntry(void) {
		return NULL;
	}
	virtual cOsdObject *MainMenuAction(void);
	virtual cMenuSetupPage *SetupMenu(void);
	virtual bool ProcessArgs(int argc, char *argv[]);
};

cPluginAndroVdr::cPluginAndroVdr(void) {
	// Initialize any member variables here.
	// DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
	// VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
	Thread = NULL;
	port = ANDROVDR_PORT;
	password = "";
	forceCheckSvdrp = false;
}

cPluginAndroVdr::~cPluginAndroVdr() {
	// Clean up after yourself!
}

cOsdObject * cPluginAndroVdr::MainMenuAction(void) {
	return NULL;
}

cMenuSetupPage * cPluginAndroVdr::SetupMenu(void) {
	return NULL;
}

const char * cPluginAndroVdr::CommandLineHelp(void) {
	return "  -p port          port number to listen to\n"
		   "  -P password      password (none if not given). No password forces check against svdrphosts.conf.\n"
		   "  -s               force check against svdrphosts.conf, even if a password was given\n";
}

bool cPluginAndroVdr::ProcessArgs(int argc, char *argv[]) {
	int c;
	while ((c = getopt(argc, argv, "p:P:s")) != -1)
		switch (c) {
		case 'p':
			port = atoi(optarg);
			break;
		case 'P':
			password = optarg;
			break;
		case 's':
			forceCheckSvdrp = true;
			break;
		case '?':
			if (optopt == 'c') {
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			} else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
			return false;
		default:
			return false;
		}

// default port
	if (port <= 0)
		port = ANDROVDR_PORT;

	return true;
}

bool cPluginAndroVdr::Initialize(void) {
// Initialize any background activities the plugin shall perform.

// Start any background activities the plugin shall perform.
	Thread = new cAndroVdrThread(port, password, forceCheckSvdrp);

	return Thread != NULL;
}

bool cPluginAndroVdr::Start(void) {
	Thread->Start();

	return true;
}

void cPluginAndroVdr::Stop(void) {
// Stop any background activities the plugin shall perform.
	Thread->Shutdown();
}

void cPluginAndroVdr::Housekeeping(void) {
// Perform any cleanup or other regular tasks.
}

VDRPLUGINCREATOR(cPluginAndroVdr);
// Don't touch this!
