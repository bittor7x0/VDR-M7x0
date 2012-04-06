/*
 * osdserver.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __OSDSERVER_OSDSERVER_H
#define __OSDSERVER_OSDSERVER_H


#include <vdr/plugin.h>
#include "server.h"
#include "tools.h"

class cPluginOsdServer : public cPlugin {
private:
    // Add any member variables or functions you may need here.
    cServer *Server;
    cOpenOsd OpenOsd;
    static cPluginOsdServer *plugin;
    int Port;

public:
    cPluginOsdServer();
    virtual ~cPluginOsdServer();
    virtual const char *Version();
    virtual const char *Description();
    virtual const char *CommandLineHelp();
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize();
    virtual bool Start();
    virtual void Stop();
    virtual void Housekeeping();
    virtual void MainThreadHook();
    virtual cString Active();
    virtual const char *MainMenuEntry() { return NULL; }
    virtual cOsdObject *MainMenuAction();
    virtual cMenuSetupPage *SetupMenu();
    virtual bool SetupParse(const char *Name, const char *Value);
    virtual bool Service(const char *Id, void *Data = NULL);
    virtual const char **SVDRPHelpPages();
    virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);

    static cPluginOsdServer* Plugin() { return plugin; }
    bool CallMainMenuAction(cOpenOsd::cCallback *Callback, int timeout=-1) { return OpenOsd.CallMainMenuAction(Callback,timeout); }
};

#endif
