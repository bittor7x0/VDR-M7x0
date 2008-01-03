/*
 * launcher.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include <vdr/status.h>
#include <vdr/menu.h>
#include "i18n.h"

static const char *VERSION        = "0.0.3";
static const char *DESCRIPTION    = trNOOP("A plugin to launch other plugins");
static const char *MAINMENUENTRY  = trNOOP("Plugins");
static int PluginIndex = -1;    // selected plugin

static char** excludelist = NULL;

// --- cLauncherOsdItem -------------------------------------------------------

class cLauncherOsdItem : public cOsdItem {
private:
  int pluginIndex;
public:
  cLauncherOsdItem(const char *Name, int Index);
  int PluginIndex(void) { return pluginIndex; }
  eOSState ProcessKey(eKeys Key);
  };

cLauncherOsdItem::cLauncherOsdItem(const char *Name, int Index)
:cOsdItem(Name, osPlugin)
{
  pluginIndex = Index;
}

eOSState cLauncherOsdItem::ProcessKey(eKeys Key)
{
    return osUnknown;
}

// --- cLauncherOsd -------------------------------------------------------
class cLauncherOsd : public cOsdMenu{

protected:
  virtual void Action(void);

public:
  cLauncherOsd();
  ~cLauncherOsd();

  virtual eOSState ProcessKey(eKeys Key);
  virtual void SetHelp();
  char* GetPluginInfo(const char* szPluginName);
};

cLauncherOsd::cLauncherOsd(void)
:cOsdMenu(tr("Plugins"))
{
  SetHelp();
  SetHasHotkeys();
  //printf("cLauncherOsd::cLauncherOsd()\n");
  for (int i = 0; ; i++) {
      cPlugin *p = cPluginManager::GetPlugin(i);
      if (p) {
	 if (strcasecmp(p->Name(), "launcher") == 0)
		continue;
	 bool bIgnore = false;
	 if (excludelist)
	 {
	     int i=0;
	     while(excludelist[i])
	     {
		 if (strcasecmp(excludelist[i], p->Name()) == 0)
		 {
		     bIgnore = true;
		     break;
		 }
		 i++;
	     }
	 }
	 if (bIgnore)
	     continue;
         char *buffer = NULL;
         asprintf(&buffer, "%s - %s", p->Name(), p->Description());
         Add(new cLauncherOsdItem(hk(buffer), i));
         free(buffer);
         }
      else
         break;
      }
}

cLauncherOsd::~cLauncherOsd(void)
{
}


void cLauncherOsd::Action(void)
{
  //printf("cLauncherOsd::Action()\n");
#if (VDRVERSNUM < 10300)
	isyslog("Launcher plugin: thread started (pid = %d)", getpid());
#endif
#if (VDRVERSNUM < 10300)
	isyslog("Launcher plugin: thread stopped (pid = %d)", getpid());
#endif
}

char* cLauncherOsd::GetPluginInfo(const char* szPluginName)
{
    char* szInfo = NULL;    
    char* szFilename = new char[strlen(szPluginName) + 6];
    memset(szFilename, 0, strlen(szPluginName)+6);
    strcpy(szFilename, szPluginName);
    strcat(szFilename, ".info");
    FILE *f=fopen(AddDirectory( cPlugin::ConfigDirectory(), szFilename),"r");
    delete [] szFilename;
    if(f) 
    {
  	fseek (f , 0 , SEEK_END);
        long lSize = ftell(f);
        rewind (f);
        szInfo = (char*) malloc (lSize);
        if (szInfo == NULL) return NULL;
        fread (szInfo,1,lSize,f);
    	fclose(f);
    }
    return szInfo;
}

eOSState cLauncherOsd::ProcessKey(eKeys Key)
{ 
    cLauncherOsdItem *item = (cLauncherOsdItem *)Get(Current());
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (HasSubMenu())  
	return osContinue;
  if (state == osUnknown)
  {
      if (Key == kOk || Key == kRed) 
      {
	  if (item) 
	  {
		PluginIndex = item->PluginIndex();
		CloseSubMenu();
		return osPlugin;
	  }
      }
      if (Key == kGreen)
      {
	  if (HasSubMenu() || Count() == 0)
	      return osContinue;
	  if (item) 
	  {
	      cPlugin *p = cPluginManager::GetPlugin(item->PluginIndex());
	      if (p)
	      {
		  char *buffer = NULL;
		  asprintf(&buffer, "%s - %s", p->Name(), p->Description());
		  char* szInfo = GetPluginInfo(p->Name());
		  if (szInfo)
		  	return AddSubMenu(new cMenuText(buffer, szInfo));
		  else
		      Skins.Message(mtInfo, tr("This plugin has no info text!")); 
	      }
	  }
      }
      if (Key == kBlue)
      {	
	  if (item) 
	  {
	      cPlugin *p = cPluginManager::GetPlugin(item->PluginIndex());
	      if (p)
	      {
		  cMenuSetupPage *menu = p->SetupMenu();
		  if (menu) {
		      menu->SetPlugin(p);
		      return AddSubMenu(menu);
		  }
		  Skins.Message(mtInfo, trVDR("This plugin has no setup parameters!")); 
	      }
	  }
      }
  }
  return state;
}

void cLauncherOsd::SetHelp() {
  
  cOsdMenu::SetHelp(trVDR("Button$Open"),trVDR("Button$Info"),NULL,trVDR("Setup"));
  
}


class cPluginLauncher : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginLauncher(void);
  virtual ~cPluginLauncher();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginLauncher::cPluginLauncher(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginLauncher::~cPluginLauncher()
{
  // Clean up after yourself!
    if (excludelist)
	delete [] excludelist;
}

const char *cPluginLauncher::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginLauncher::ProcessArgs(int argc, char *argv[])
{
  static struct option long_options[] = {
    { "excludelist",          required_argument, NULL, 'x' },
    { NULL }
  };

  int c=0,i=0;
  
  if (excludelist)
      delete [] excludelist;
  excludelist = new char*[argc];
  while ((c = getopt_long(argc, argv, "x:", long_options, NULL)) != -1) {
        switch (c) {
          case 'x': excludelist[i++] = optarg;
                    break;
          default:  return false;
          }
        }
  // end of the list
  excludelist[i]=NULL;
  return true;
}

bool cPluginLauncher::Initialize(void)
{
#if APIVERSNUM < 10507
  RegisterI18n(Phrases);
#endif
  return true;
}

bool cPluginLauncher::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginLauncher::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginLauncher::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  if (PluginIndex != -1)
  {
      cPlugin *p = cPluginManager::GetPlugin(PluginIndex);
      PluginIndex = -1;
      if (p)
		return p->MainMenuAction();
  }
  return new cLauncherOsd();;
}

cMenuSetupPage *cPluginLauncher::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginLauncher::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

VDRPLUGINCREATOR(cPluginLauncher); // Don't touch this!
