/*
 * proxy.c: The proxy plugin for VDR
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: proxy.c 673 2006-04-30 17:24:00Z udo $
 */

#include "proxy.h"
#include "menu.h"
#if VDRVERSNUM < 10507
#include "i18n.h"
#define trNOOP(s) (s)
#endif
#include <vdr/audio.h>
#include <vdr/remote.h>
#include <vdr/device.h>



static const char *VERSION        = "0.1.3";
static const char *DESCRIPTION    = trNOOP("Proxy is a plugin that loads a plugin");


#define LIBVDR_PREFIX  "libvdr-"
#define SO_INDICATOR   ".so."
#if VDRVERSNUM >= 10347
  #define PLUGINVERSION APIVERSION
#else
  #define PLUGINVERSION VDRVERSION
#endif



cProxies cPluginProxy::Proxies;

cPluginProxy::sProxyLoadLate *cPluginProxy::NextLoad=NULL;

cPluginProxy* cProxies::PluginByName(const char *Name) {
    for (cPluginProxy *p=First(); p; p=Next(p)) {
        if (p->GroupLeader) continue;
        if (p->ProxyLeader) continue;

        if (strcasecmp(Name, p->Name()) == 0)  return p;
    }
    return NULL;
}

cPluginProxy::cPluginProxy(void) {
    plugin=NULL;
    handle=NULL;
    GroupName=NULL;
    MenuName=NULL;
    GroupLeader=false;
    ProxyLeader=false;
    Valid=true;
    StartSuspended=false;
    HideOnSuspend=false;
    SuspendOnError=false;
    DontSuspend=false;
    HideMainMenu=false;
    PassThroughAction=NULL;
    ArgC=0;
    ArgV=NULL;
    ArgVOrig=NULL;
    MainMenuEntryBuffer=NULL;
    DescriptionBuffer=NULL;

    if (NextLoad) {
        if (NextLoad->AsGroupLeader) {
            GroupLeader=true;
            GroupName=strdup(NextLoad->GroupName);
        }
        if (NextLoad->AsProxyLeader) {
            ProxyLeader=true;
            Proxies.ProxyLeader=this;
        }
        NextLoad=NULL;
    }

    Proxies.Add(this);
}

cPluginProxy::~cPluginProxy() {
    Proxies.Del(this,false);
    
    UnloadPlugin(true);
    
    if (GroupName) free(GroupName);
    if (MenuName) free(MenuName);
    
    if (MainMenuEntryBuffer) free(MainMenuEntryBuffer);
    if (DescriptionBuffer) free(DescriptionBuffer);
    
    if (ArgVOrig) delete[] ArgVOrig;
}


bool cPluginProxy::ProxyArgs(int argc, char *argv[]) {
    
    // Read command line
    static struct option getopt_array[] = {
        {"menu-hide",0,0,'h'},
        {"menu-group",1,0,'g'},
        {"menu-name",1,0,'n'},
        {"suspended",0,0,'s'},
        {"hide-on-suspend",0,0,'i'},
        {"suspend-on-error",0,0,'e'},
        {0,0,0,0}
    };
    
    while (1) {
        int optindex;
        int c=getopt_long(argc,argv,"+hg:n:sei",getopt_array,&optindex);
        if (c==-1) break;
        switch (c) {
        case 'h': 
            HideMainMenu=true;
            break;
        case 'g':
            GroupName=strdup(optarg);
            break;
        case 'n':
            MenuName=strdup(optarg);
            break;
        case 's':
            StartSuspended=true;
            break;
        case 'e':
            SuspendOnError=true;
            break;
        case 'i':
            HideOnSuspend=true;
            break;
        }
    }
        
    if (optind >= argc) {
        // No plugin name specified on command line
        // Delay error until Initialize() to allow command line help to work
        Valid=false;
        return true;
    }

    SetName(argv[optind]);

    ArgC = argc - optind;

    // Create a copy of the remaining argc/argv
    ArgVOrig=new char*[ArgC+2];
    memcpy(ArgVOrig,argv+optind,(ArgC+2)*sizeof(char*));

    // Reset getopt
    optind = 0;

    // Check if we need a proxy leader or group leader plugin
    if (!CheckGroupPlugin()) return false;

    return true;
}


// Each group should have an generic (not assigned) proxy plugin as group leader, for the
// main menu entry. 
// Also, there should be a generic proxy leader that handles the proxy setup menu


bool cPluginProxy::LateAddPlugin(cPluginProxy *before, cPluginProxy::sProxyLoadLate *as) {
    // Add another proxy plugin without params and encapsulated plugin
    char *fileName = NULL;
    cDll *dllBefore=NULL;
    cDll *dll;
    const cDlls &dlls=cPluginManager::pluginManager->dlls;
    
    if (before) {
        for (dllBefore=dlls.First(); dllBefore; dllBefore=dlls.Next(dllBefore)) {
            if (dllBefore->Plugin()==before) break;
        }
    }
    
    asprintf(&fileName, "%s/%s%s%s%s", cPluginManager::pluginManager->directory, LIBVDR_PREFIX, "proxy", SO_INDICATOR, PLUGINVERSION);
        
    dll=new cDll(fileName,"proxy");
    
    free(fileName);
    
    cPluginManager::pluginManager->dlls.Ins(dll,dllBefore);
    
    // Tell him what we need
    NextLoad=as;
            
    // and load manually
    if (!dll->Load(true)) {
        // back to normal
        NextLoad=NULL;
        return false;
    }

    return true;    
}


bool cPluginProxy::CheckGroupPlugin() {
    // Search for a group leader if this proxy belongs to a group,  
    // search for a nameless leader if this proxy doesnt belong to a group
    sProxyLoadLate as;
    
    if (!Proxies.ProxyLeader) {
        // We need a proxy leader
        as.AsProxyLeader=true;
        as.AsGroupLeader=false;
        
        if (!LateAddPlugin(this,&as)) return false;
    }

    if (!GroupName) return true;
    
    // Wer also need a group leader for the main menu entry.
    
    for (cPluginProxy *p=Proxies.First(); p; p=Proxies.Next(p)) {
        if (p->GroupLeader) {
            // Generic proxy plugin found
            
            if (0==strcmp(GroupName,p->GroupName)) {
                // Same group. We already have a group leader.
                return true;
            }
        }   
    }
    
    // We still need a group leader.
    as.AsGroupLeader=true;
    as.AsProxyLeader=false;
    as.GroupName=GroupName;
    
    if (!LateAddPlugin(this,&as)) return false;
    
    return true;
}


bool cPluginProxy::LoadPlugin() {
    char *fileName = NULL;
    void *(*creator)(void) = NULL;
    const char *error;

    if (handle||plugin) return false;

    asprintf(&fileName, "%s/%s%s%s%s", cPluginManager::pluginManager->directory, LIBVDR_PREFIX, Name(), SO_INDICATOR, PLUGINVERSION);
    
    isyslog("proxy loading plugin: %s", fileName);
    
    handle = dlopen(fileName, RTLD_NOW);
    error = dlerror();
    free(fileName);
    
    if (!error) {
        creator = (void *(*)(void))dlsym(handle, "VDRPluginCreator");
        error = dlerror();
    }
    if (!error) {
        plugin = (cPlugin*)creator();
    }
    if (plugin) {
        plugin->SetName(Name());
        if (ArgC>0) {
            // copy argument array so plugin can mess with copy
            ArgV=new char*[ArgC+2];
            memcpy(ArgV,ArgVOrig,(ArgC+2)*sizeof(char*));
            
            if (!plugin->ProcessArgs(ArgC, ArgV)) return false;
        }
        return true;
    }
    if (error) {
        esyslog("ERROR: %s", error);
        fprintf(stderr, "vdr: %s\n", error);
    }
    return false;
}

bool cPluginProxy::SetupPlugin() {
    bool res=true;
    
    if (!plugin) return false;
    
    for (cSetupLine *l = Setup.First(); l; l = Setup.Next(l)) {
        if (l->Plugin() && 0==strcmp(Name(),l->Plugin())) {
            if (!plugin->SetupParse(l->Name(), l->Value())) {
                esyslog("ERROR: unknown config parameter: %s.%s = %s", Name(), l->Name(), l->Value());
                res=false;
            }
        }   
    }

    return true;
}

bool cPluginProxy::UnloadPlugin(bool force) {
    if (DontSuspend && !force) return false;
    
    if (plugin) {
        isyslog("proxy unloading plugin: %s",plugin->Name());
        #if VDRVERSNUM < 10507
        I18nRegister(NULL,plugin->Name());
        // disconnect string table before unload
        #endif
        delete plugin;
        plugin=NULL;
    }
    if (handle) dlclose(handle);
    handle=NULL;
    if (ArgV) delete[] ArgV;
    ArgV=NULL;
    return true;
}


bool cPluginProxy::ResumePlugin() {
    if (plugin) return false;
    
    if (!LoadPlugin()) return false;
    
    SetupPlugin();
    
    if (!Initialize()) {
        UnloadPlugin();
        return false;   
    }
    
    if (!Start()) {
        UnloadPlugin();
        return false;
    }
    
    return true;
}

bool cPluginProxy::SuspendPlugin() {
    if (DontSuspend) return false;
    
    if (plugin) plugin->Stop();
    
    return UnloadPlugin();
}

const char *cPluginProxy::ProxyMainMenuEntry(void) { 
    if (plugin) {
        if (MenuName) return MenuName;
        return plugin->MainMenuEntry();
    }
    
    // suspended plugin
    
    if (HideOnSuspend) return NULL;
    
    if (MainMenuEntryBuffer) free(MainMenuEntryBuffer);
    MainMenuEntryBuffer=NULL;
    asprintf(&MainMenuEntryBuffer,"%s: %s",tr("Suspended Plugin"),Name());
    
    return MainMenuEntryBuffer;
}

/*************************************
 * VDR plugin interface pass-through *
 *************************************/


const char *cPluginProxy::Version(void) { 
    if (plugin) return plugin->Version();
    if (GroupLeader||ProxyLeader||!Valid) return VERSION; 
    return "?";
}

const char *cPluginProxy::Description(void) { 
    if (DescriptionBuffer) free(DescriptionBuffer);
    
    if (plugin) return plugin->Description();
    if (ProxyLeader || !Valid) return tr(DESCRIPTION); 
    if (GroupLeader) {
        asprintf(&DescriptionBuffer,"%s: %s",tr("Menu entry"),GroupName);
        return DescriptionBuffer;
    }

    asprintf(&DescriptionBuffer,"%s: %s",tr("Suspended"),Name());
    return DescriptionBuffer;
}

const char *cPluginProxy::CommandLineHelp(void) {
    if (!Valid) {
        //      12345678 10 345678 20 345678 30 345678 40 345678 50 345678 60 345678 70 345678 80
        return "Load: -P\"proxy [proxy-OPTIONS] foo [foo-OPTIONS]\"  Proxy-load plugin 'foo'\n"
               "  -h,       --menu-hide         Hide main menu of plugin\n"
               "  -g bar,   --menu-group=bar    Put plugin into sub-menu foo\n"
               "  -n bar,   --menu-name=bar     Set plugin menu title to bar\n"
               "  -s,       --suspended         Start plugin suspended\n"
               "  -i,       --hide-on-suspend   Hide suspended plugin from menu\n"
               "  -e,       --suspend-on-error  Suspend plugin on load error\n";
    }
    if (plugin) return plugin->CommandLineHelp();
    return NULL;
}

bool cPluginProxy::ProcessArgs(int argc, char *argv[]) {
    // Group leaders ignore parameters
    if (GroupLeader||ProxyLeader) return true;

    if (!ProxyArgs(argc,argv)) return false;

    if (!Valid) return true;
    
    if (StartSuspended) return true;
    
    if (LoadPlugin()) return true;
    
    if (SuspendOnError) {
        UnloadPlugin();
        return true;
    }
    
    return false;
}

bool cPluginProxy::Initialize(void) {
    if (!Valid) {
        esyslog("proxy-plugin: ERROR: Need to specify plugin name");
        fprintf(stderr, "vdr: Proxy-Plugin: Need to specify plugin name\n");
        return false;
    }

    #if VDRVERSNUM < 10507
    if (ProxyLeader) RegisterI18n(ProxyPhrases);
    #endif

    if (plugin) {
        int nSkins=Skins.Count();
        int nDevices=cDevice::NumDevices();
        int nAudios=Audios.Count();
        int nRemotes=Remotes.Count();
        
        bool ok=plugin->Initialize();
        
        if (Skins.Count() != nSkins) DontSuspend=true;
        if (cDevice::NumDevices() != nDevices) DontSuspend=true;        
        if (Audios.Count() != nAudios) DontSuspend=true;
        if (Remotes.Count() != nRemotes) DontSuspend=true;
        
        if (ok) return true;
        
        if (!SuspendOnError) return false;

        UnloadPlugin(); 
    }
    return true;
}

bool cPluginProxy::Start(void) {
    if (plugin) {
        int nSkins=Skins.Count();
        int nDevices=cDevice::NumDevices();
        int nAudios=Audios.Count();
        int nRemotes=Remotes.Count();
        
        bool ok=plugin->Start();
        
        if (Skins.Count() != nSkins) DontSuspend=true;
        if (cDevice::NumDevices() != nDevices) DontSuspend=true;        
        if (Audios.Count() != nAudios) DontSuspend=true;
        if (Remotes.Count() != nRemotes) DontSuspend=true;
        
        if (ok) return true;
        
        if (!SuspendOnError) return false;

        UnloadPlugin(); 
    }
    return true;
}

void cPluginProxy::Stop(void) {
    if (plugin) plugin->Stop();
}

void cPluginProxy::Housekeeping(void) {
    if (plugin) plugin->Housekeeping();
}

const char *cPluginProxy::MainMenuEntry(void) { 
    if (GroupLeader) return GroupName;  
    
    if (ProxyLeader) return NULL;
        
    if (GroupName) return NULL;

    if (HideMainMenu) return NULL;
    
    return ProxyMainMenuEntry();
}

cOsdObject *cPluginProxy::MainMenuAction(void) {
    if (PassThroughAction) {
        cOsdObject *temp=PassThroughAction;
        PassThroughAction=NULL;
        return temp;
    }
    if (GroupLeader) {
        if (GroupName) return new cMenuGroup(GroupName,this);
        return NULL;
    }
    if (ProxyLeader) {
        return NULL;
    }

    if (!plugin) ResumePlugin();
        
    if (plugin) return plugin->MainMenuAction();
    
    return NULL;
}

cMenuSetupPage *cPluginProxy::SetupMenu(void) {
    if (plugin) return plugin->SetupMenu();
    if (ProxyLeader) return new cProxySetupPage();
    return NULL;
}

bool cPluginProxy::SetupParse(const char *Name, const char *Value) {
    if (plugin) return plugin->SetupParse(Name,Value);
    return false;
}

bool cPluginProxy::Service(const char *Id, void *Data) {
    #if VDRVERSNUM >= 10330
    if (plugin) return plugin->Service(Id,Data);
    #endif
    return false;
}
   
const char **cPluginProxy::SVDRPHelpPages(void) {
    static const char *HelpPages_Leader[] = {
        "RSUM <plugin>\n"
        "    Resume plugin.",
        "SUSP <plugin>\n"
        "    Suspend plugin.",
        "RELD <plugin>\n"
        "    Reload plugin.",
        NULL
    };

    if (ProxyLeader) return HelpPages_Leader;

    #if VDRVERSNUM >= 10331
    if (plugin) return plugin->SVDRPHelpPages();
    #endif
    return NULL;
}

#if VDRVERSNUM >= 10331 // Plugin interfaces depending on cString

cString cPluginProxy::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {

    if (ProxyLeader) {
        if (strcasecmp(Command, "RSUM") == 0) {
            cPluginProxy *proxy=cPluginProxy::Proxies.PluginByName(Option);
            if (!proxy) return cString("No plugin specified or plugin not found.");
            if (proxy->plugin) return cString::sprintf("Plugin %s is already running.", proxy->Name());
            if (proxy->ResumePlugin()) 
                return cString::sprintf("Resumed Plugin %s", proxy->Name());
            else
                return cString::sprintf("Failed to resume Plugin %s", proxy->Name());
        } else if (strcasecmp(Command, "SUSP") == 0) {
            cPluginProxy *proxy=cPluginProxy::Proxies.PluginByName(Option);
            if (!proxy) return cString("No plugin specified or plugin not found.");
            if (!proxy->plugin) return cString::sprintf("Plugin %s is not running.", proxy->Name());
            if (proxy->SuspendPlugin())
                return cString::sprintf("Suspended Plugin %s", proxy->Name());
            else
                return cString::sprintf("Failed to suspend Plugin %s", proxy->Name());
        } else if (strcasecmp(Command, "RELD") == 0) {
            cPluginProxy *proxy=cPluginProxy::Proxies.PluginByName(Option);
            if (!proxy) return cString("No plugin specified or plugin not found.");
            if (proxy->plugin) {
                if (!proxy->SuspendPlugin())
                    return cString::sprintf("Failed to suspend Plugin %s", proxy->Name());
            }
            if (proxy->ResumePlugin())
                return cString::sprintf("Reloaded Plugin %s", proxy->Name());
            else
                return cString::sprintf("Failed to reload Plugin %s", proxy->Name());
        } else {
            return cString("Unknown command.");
        }
    }
    
    if (plugin) return plugin->SVDRPCommand(Command,Option,ReplyCode);
    return NULL;
}

cString cPluginProxy::Active(void) {
    #if VDRVERSNUM >= 10347
    if (plugin) return plugin->Active();
    #endif
    return NULL;
}

#endif // VDRVERSNUM >= 10331

time_t cPluginProxy::WakeupTime(void) {
    #if VDRVERSNUM >= 10501
    if (plugin) return plugin->WakeupTime();
    #endif
    return 0;
}
 
void cPluginProxy::MainThreadHook(void) {
    #if VDRVERSNUM >= 10347
    if (plugin) plugin->MainThreadHook();
    #endif
}


VDRPLUGINCREATOR(cPluginProxy); // Don't touch this!
