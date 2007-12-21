/*
 * osdserver.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include <vdr/remote.h>
#include "osdserver.h"

static const char *VERSION        = "0.1.1";
static const char *DESCRIPTION    = "Server for remote OSD clients";


cPluginOsdServer::cPluginOsdServer()
    : OpenOsd(this) {
    // Initialize any member variables here.
    // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
    // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
    Server=NULL;
    plugin=this;
    Port = 2010;
}

cPluginOsdServer::~cPluginOsdServer() {
    // Clean up after yourself!
    if (Server) delete Server;
}

const char *cPluginOsdServer::Version() {
    return VERSION;
}

const char *cPluginOsdServer::Description() {
    return DESCRIPTION;
}

const char *cPluginOsdServer::CommandLineHelp() {
    // Return a string that describes all known command line options.
    return "  -p PORT, --port=PORT      Listen on TCP/IP port number\n";
}    

bool cPluginOsdServer::ProcessArgs(int argc, char *argv[]) {
    
    // Read command line
    static struct option getopt_array[] = {
        {"port",1,0,'p'},
        {0,0,0,0}
    };
    
    while (1) {
        int optindex;
        int c=getopt_long(argc,argv,"p:",getopt_array,&optindex);
        if (c==-1) break;
        switch (c) {
        case 'p':
            Port = atoi(optarg);
            break;
        }
    }
        
    if (optind < argc) {
        fprintf(stderr, "osdserver: unknown parameter %s\n", argv[optind]);
        return false;
    }

    return true;
}

bool cPluginOsdServer::Initialize() {
    // Initialize any background activities the plugin shall perform.
    cString OsdServerHosts = AddDirectory(cPlugin::ConfigDirectory(), "osdserverhosts.conf");
    if (access(OsdServerHosts, F_OK) == 0)
        Server = new cServer(Port, OsdServerHosts);
    else
        Server = new cServer(Port, NULL);

    return true;
}

bool cPluginOsdServer::Start() {
    // Start any background activities the plugin shall perform.
    Server->Start();
    return true;
}

void cPluginOsdServer::Stop() {
    Server->Cancel(2);
    // Stop any background activities the plugin shall perform.
}

void cPluginOsdServer::Housekeeping() {
    // Perform any cleanup or other regular tasks.
}

void cPluginOsdServer::MainThreadHook() {
    // Perform actions in the context of the main program thread.
    // WARNING: Use with great care - see PLUGINS.html!
    OpenOsd.MainThreadHook();
    cMainThreadLock::MainThreadHook();
}

cString cPluginOsdServer::Active() {
    // Return a message string if shutdown should be postponed
    return NULL;
}

cOsdObject *cPluginOsdServer::MainMenuAction() {
    // Perform the action when selected from the main VDR menu.

    cOsdObject *obj=OpenOsd.MainMenuAction();
    if (obj) return obj;

    esyslog("Strange MainMenuAction call");

    return NULL;
}

cMenuSetupPage *cPluginOsdServer::SetupMenu() {
    // Return a setup menu in case the plugin supports one.
    return NULL;
}

bool cPluginOsdServer::SetupParse(const char *Name, const char *Value) {
    // Parse your own setup parameters and store their values.
    return false;
}

bool cPluginOsdServer::Service(const char *Id, void *Data) {
    // Handle custom service requests from other plugins
    return false;
}

const char **cPluginOsdServer::SVDRPHelpPages() {
    // Return help text for SVDRP commands this plugin implements
    return NULL;
}

cString cPluginOsdServer::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
    // Process SVDRP commands this plugin implements
    return NULL;
}


cPluginOsdServer *cPluginOsdServer::plugin=NULL;

VDRPLUGINCREATOR(cPluginOsdServer); // Don't touch this!
