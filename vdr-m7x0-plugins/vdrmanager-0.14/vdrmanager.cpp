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
#include "compressor.h"

#define VDRMANAGER_PORT		    6420

#if VDRMANAGER_USE_SSL
#define VDRMANAGER_SSL_PORT   6421
#define VDRMANAGER_CRT_FILE   "/etc/vdr/plugins/vdrmanager/vdrmanager.pem"
#define VDRMANAGER_KEY_FILE   "/etc/vdr/plugins/vdrmanager/vdrmanager.pem"
#endif

#define VDRMANAGER_ARGS_COMMON "p:P:s"
#if VDRMANAGER_USE_SSL
#define VDRMANAGER_ARGS_SSL "k:"
#else
#define VDRMANAGER_ARGS_SSL
#endif
#if VDRMNAGER_USE_GZIP || VDRMANAGER_USE_ZLIB
#define VDRMANAGER_ARGS_COMPRESS "c::"
#else
#define VDRMANAGER_ARGS_COMPRESS
#endif
#define VDRMANAGER_ARGS VDRMANAGER_ARGS_COMMON VDRMANAGER_ARGS_SSL VDRMANAGER_ARGS_COMPRESS

static const char *VERSION = "0.14";
static const char *DESCRIPTION = "VDR-Manager plugin";

class cVdrManager: public cPlugin {
private:
	// Add any member variables or functions you may need here.
	cVdrManagerThread * Thread;
	int port;
	const char * password;
	bool forceCheckSvdrp;
	int compressionMode;
#if VDRMANAGER_USE_SSL
  int sslport;
	const char * certFile;
	const char * keyFile;
#endif
protected:
public:
	cVdrManager(void);
	virtual ~cVdrManager();
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

cVdrManager::cVdrManager(void) {
	// Initialize any member variables here.
	// DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
	// VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
	Thread = NULL;
	port = VDRMANAGER_PORT;
	password = "";
	forceCheckSvdrp = false;
	compressionMode = COMPRESSION_NONE;
#if VDRMANAGER_USE_SSL
  sslport = VDRMANAGER_SSL_PORT;
	certFile = VDRMANAGER_CRT_FILE;
	keyFile = VDRMANAGER_KEY_FILE;
#endif
}

cVdrManager::~cVdrManager() {
	// Clean up after yourself!
}

cOsdObject * cVdrManager::MainMenuAction(void) {
	return NULL;
}

cMenuSetupPage * cVdrManager::SetupMenu(void) {
	return NULL;
}

const char * cVdrManager::CommandLineHelp(void) {
	return  "  -p port                port number to listen to\n"
			"  -P password            password (none if not given). No password forces check against svdrphosts.conf.\n"
			"  -s                     force check against svdrphosts.conf, even if a password was given\n"
			"  -c compression         selects the compression mode to use ('z' for zlib or 'g' for gzip and 'n' for none).\n"
			"                         Zlib compression is enabled as default or is default compression if youf specify -c without arguments";
#if VDRMANAGER_USE_SSL
			"  -k certfile[,keyfile]  cert and key file for SSL (or one file for both)"
#endif
	;
}

bool cVdrManager::ProcessArgs(int argc, char *argv[]) {
	int c;
	while ((c = getopt(argc, argv, VDRMANAGER_ARGS)) != -1)
		switch (c) {
		case 'p':
			port = atoi(optarg);
#if VDRMANAGER_USE_SSL
			{
				const char * sep = strchr(optarg, ',');
				if (sep)
					sslport = atoi(sep + 1);
				else
					sslport = port + 1;
			}
#endif
			break;
		case 'P':
			password = optarg;
			break;
		case 's':
			forceCheckSvdrp = true;
			break;
#if VDRMANAGER_USE_GZIP || VDRMANAGER_USE_ZLIB
		case 'c':
			if (!optarg) {
				compressionMode = COMPRESSION_ZLIB;
			} else if (optarg[0] == 'g') {
				compressionMode = COMPRESSION_GZIP;
			} else if (optarg[0] == 'z') {
				compressionMode = COMPRESSION_ZLIB;
			} else if (optarg[0] == 'n') {
				compressionMode = COMPRESSION_NONE;
			} else {
				return false;
			}
			break;
#endif
#if VDRMANAGER_USE_SSL
		case 'k':
		  {
		    const char * sep = strchr(optarg, ',');
		    if (sep == NULL) {
		      certFile = keyFile = optarg;
		    } else {
		      certFile = strndup(optarg, sep - optarg);
		      keyFile = sep;
		    }
		  }
			break;
#endif
		case '?':
			return false;
		default:
			return false;
		}

	// default port
	if (port <= 0)
		port = VDRMANAGER_PORT;

#if VDRMANAGER_USE_SSL
	if (sslport <= 0)
		sslport = port + 1;
#endif

	return true;
}

bool cVdrManager::Initialize(void) {
// Initialize any background activities the plugin shall perform.

// Start any background activities the plugin shall perform.
	Thread = new cVdrManagerThread(port,
#if VDRMANAGER_USE_SSL
	    sslport,
#endif
	    password, forceCheckSvdrp
			, compressionMode
#if VDRMANAGER_USE_SSL
			, certFile, keyFile
#endif
			);

	return Thread != NULL;
}

bool cVdrManager::Start(void) {
	Thread->Start();

	return true;
}

void cVdrManager::Stop(void) {
// Stop any background activities the plugin shall perform.
	Thread->Shutdown();
}

void cVdrManager::Housekeeping(void) {
// Perform any cleanup or other regular tasks.
}

VDRPLUGINCREATOR(cVdrManager);
// Don't touch this!
