/*
 * proxy.h: The proxy plugin for VDR
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: proxy.h 649 2006-04-21 00:08:41Z udo $
 */

#ifndef __PROXY_PROXY_H
#define __PROXY_PROXY_H

#include <dlfcn.h>
#include <getopt.h>
#include <vdr/i18n.h>
#include <vdr/menuitems.h>
#include <vdr/osdbase.h>
#include <vdr/tools.h>

// Open, Sesame! ;>
#define private public
#define protected public
#include <vdr/plugin.h>
// Shut, Sesame!
#undef private
#undef protected

class cProxies;

class cPluginProxy : public cPlugin, public cListObject {
private:
    void *handle;
    cPlugin *plugin; 

    char *GroupName;
    char *MenuName;
    bool GroupLeader;
    bool ProxyLeader;
    bool Valid; // track parameter errors

    struct sProxyLoadLate {
        bool AsProxyLeader;
        bool AsGroupLeader;
        char *GroupName;
    };
    static sProxyLoadLate *NextLoad;
    
    int ArgC;
    char **ArgV;
    char **ArgVOrig;
    
    cOsdObject *PassThroughAction;
    
    bool HideMainMenu;

    bool StartSuspended;
    bool SuspendOnError;
    bool HideOnSuspend;
    bool DontSuspend;
    
    char *MainMenuEntryBuffer;
    char *DescriptionBuffer;


/*
    Call order for plugins:
    - VDRPluginCreator()
    - plugin constructor
    - SetName()
    - ProcessArgs() (optional)
    - SetupParse() (optional)
    - Initialize()
    - Start()
    
    
    - Stop()
    - Unregister I18N and other stuff
    - destructor
*/

    bool ProxyArgs(int argc, char *argv[]);

    static bool LateAddPlugin(cPluginProxy *before, sProxyLoadLate *as);
    
    bool CheckGroupPlugin();

    bool LoadPlugin();
    bool SetupPlugin();
    bool UnloadPlugin(bool force=false);
    
    bool ResumePlugin();
    bool SuspendPlugin();

    const char *ProxyMainMenuEntry(void);

    static cProxies Proxies;

public:
    
    cPluginProxy(void);
    virtual ~cPluginProxy();
    virtual const char *Version(void);
    virtual const char *Description(void);
    virtual const char *CommandLineHelp(void);
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Housekeeping(void);
    virtual const char *MainMenuEntry(void);
    virtual cOsdObject *MainMenuAction(void);
    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);
    virtual bool Service(const char *Id, void *Data = NULL);
    virtual const char **SVDRPHelpPages(void);
#if VDRVERSNUM >= 10331 // cString class not present before 1.3.18, but required from 1.3.31 on
    virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
    virtual cString Active(void);
#endif
    virtual time_t WakeupTime(void);
    virtual void MainThreadHook(void);
    
    friend class cMenuGroup;
    friend class cProxySetupPage;
    friend class cProxySetupMenuItem;
    friend class cProxies;
};

class cProxies : public cList<cPluginProxy> { 
public:
    cPluginProxy *ProxyLeader;

    cPluginProxy* PluginByName(const char *Name);
};

#endif

