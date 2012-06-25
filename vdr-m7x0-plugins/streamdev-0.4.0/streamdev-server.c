/*
 * streamdev.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: streamdev-server.c,v 1.7.2.3 2009/06/29 06:25:27 schmirl Exp $
 */

#include <getopt.h>
#include <vdr/tools.h>
#include "streamdev-server.h"
#include "server/setup.h"
#include "server/server.h"
#include "server/suspend.h"
#include "i18n.h"

#if VDRVERSNUM < 10400
#error "VDR-1.4.0 or greater is required"
#endif

const char *cPluginStreamdevServer::DESCRIPTION = "VDR Streaming Server";

cPluginStreamdevServer::cPluginStreamdevServer(void) 
{
}

cPluginStreamdevServer::~cPluginStreamdevServer() 
{
	free(opt_auth);
	free(opt_remux);
}

const char *cPluginStreamdevServer::Description(void) 
{
	return tr(DESCRIPTION);
}

const char *cPluginStreamdevServer::CommandLineHelp(void)
{
	// return a string that describes all known command line options.
	return
		"  -a <LOGIN:PASSWORD>, --auth=<LOGIN:PASSWORD>  Credentials for HTTP authentication.\n"
		"  -r <CMD>, --remux=<CMD>  Define an external command for remuxing.\n"
		;
}

bool cPluginStreamdevServer::ProcessArgs(int argc, char *argv[])
{
	// implement command line argument processing here if applicable.
	static const struct option long_options[] = {
		{ "auth", required_argument, NULL, 'a' },
		{ "remux", required_argument, NULL, 'r' },
		{ NULL, 0, NULL, 0 }
	};

	int c;
	while((c = getopt_long(argc, argv, "a:r:", long_options, NULL)) != -1) {
		switch (c) {
			case 'a':
				{
					if (opt_auth)
						free(opt_auth);
					int l = strlen(optarg);
					cBase64Encoder Base64((uchar*) optarg, l,  l * 4 / 3 + 3);
					const char *s = Base64.NextLine();
					if (s)
						opt_auth = strdup(s);
				}
				break;
			case 'r':
				if (opt_remux)
				    free(opt_remux);
				opt_remux = strdup(optarg);
				break;
			default:
				return false;
		}
	}
	return true;
}

bool cPluginStreamdevServer::Start(void) 
{
	i18n_name = Name();
	RegisterI18n(Phrases);

	if (!StreamdevHosts.Load(STREAMDEVHOSTSPATH, true, true)) {
		esyslog("streamdev-server: error while loading %s", STREAMDEVHOSTSPATH);
		fprintf(stderr, "streamdev-server: error while loading %s\n", STREAMDEVHOSTSPATH);
		if (access(STREAMDEVHOSTSPATH, F_OK) != 0) {
			fprintf(stderr, "  Please install streamdevhosts.conf into the path "
			                "printed above. Without it\n" 
			                "  no client will be able to access your streaming-"
			                "server. An example can be\n"
			                "  found together with this plugin's sources.\n");
		}
		return false;
	}
	if (!opt_remux)
		opt_remux = strdup(DEFAULT_EXTERNREMUX);

	cStreamdevServer::Initialize();

	return true;
}

void cPluginStreamdevServer::Stop(void) 
{
	cStreamdevServer::Destruct();
}

cString cPluginStreamdevServer::Active(void) 
{
	if (cStreamdevServer::Active())
	{
		static const char *Message = NULL;
		if (!Message) Message = tr("Streaming active");
		return Message;
	}
	return NULL;
}

const char *cPluginStreamdevServer::MainMenuEntry(void) 
{
	if (StreamdevServerSetup.SuspendMode == smOffer && !cSuspendCtl::IsActive())
		return tr("Suspend Live TV");
	return NULL;
}

cOsdObject *cPluginStreamdevServer::MainMenuAction(void) 
{
	cControl::Launch(new cSuspendCtl);
	return NULL;
}

cMenuSetupPage *cPluginStreamdevServer::SetupMenu(void) 
{
	return new cStreamdevServerMenuSetupPage;
}

bool cPluginStreamdevServer::SetupParse(const char *Name, const char *Value) 
{
	return StreamdevServerSetup.SetupParse(Name, Value);
}

VDRPLUGINCREATOR(cPluginStreamdevServer); // Don't touch this!
