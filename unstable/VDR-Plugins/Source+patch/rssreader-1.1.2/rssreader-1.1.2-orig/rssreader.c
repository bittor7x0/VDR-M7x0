/*
 * rssreader.c: A plugin for the Video Disk Recorder
 *
 * $Id$
 */

#include <getopt.h>
#include <vdr/config.h>
#include <vdr/plugin.h>

#include "menu.h"
#include "config.h"
#include "common.h"

#if defined(APIVERSNUM) && APIVERSNUM < 10508
#error "VDR-1.5.8 API version or greater is required!"
#endif

static const char VERSION[]       = "1.1.2";
static const char DESCRIPTION[]   = trNOOP("RSS Reader for OSD");
static const char MAINMENUENTRY[] = trNOOP("RSS Reader");

class cPluginRssReader : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginRssReader(void);
  virtual ~cPluginRssReader();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void) {}
  virtual cString Active(void) { return NULL; }
  virtual const char *MainMenuEntry(void) { return (RssConfig.hidemenu ? NULL : tr(MAINMENUENTRY)); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

class cPluginRssReaderSetup : public cMenuSetupPage
{
private:
  cRssReaderConfig data;
  virtual void Setup(void);
protected:
  virtual eOSState ProcessKey(eKeys Key);
  virtual void Store(void);
public:
  cPluginRssReaderSetup(void);
};

cPluginRssReader::cPluginRssReader(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginRssReader::~cPluginRssReader()
{
  // Clean up after yourself!
}

const char *cPluginRssReader::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginRssReader::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginRssReader::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginRssReader::Start(void)
{
  // Start any background activities the plugin shall perform.
  if (!RssItems.Load(AddDirectory(ConfigDirectory(), "rssreader.conf")))
     error("configuration file 'rssreader.conf' not found!");
  return true;
}

void cPluginRssReader::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginRssReader::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginRssReader::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cRssStreamsMenu();
}

cMenuSetupPage *cPluginRssReader::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cPluginRssReaderSetup();
}

bool cPluginRssReader::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "HideMenu"))  RssConfig.hidemenu = atoi(Value);
  else if (!strcasecmp(Name, "HideElem"))  RssConfig.hideelem = atoi(Value);
  else if (!strcasecmp(Name, "UseProxy"))  RssConfig.useproxy = atoi(Value);
  else if (!strcasecmp(Name, "HttpProxy")) strn0cpy(RssConfig.httpproxy, Value, sizeof(RssConfig.httpproxy));
  else return false;

  return true;
}

bool cPluginRssReader::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginRssReader::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginRssReader::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

cPluginRssReaderSetup::cPluginRssReaderSetup(void)
{
  data = RssConfig;
  Setup();
}

void cPluginRssReaderSetup::Setup(void)
{
  int current = Current();

  Clear();

  Add(new cMenuEditBoolItem(tr("Hide main menu entry"),       &data.hidemenu));
  Add(new cMenuEditBoolItem(tr("Hide non-existent elements"), &data.hideelem));
  Add(new cMenuEditBoolItem(tr("Use HTTP proxy server"),      &data.useproxy));
  if (data.useproxy)
     Add(new cMenuEditStrItem( tr("HTTP proxy server"),       data.httpproxy, sizeof(data.httpproxy), tr(FileNameChars)));

  SetCurrent(Get(current));
  Display();
}

eOSState cPluginRssReaderSetup::ProcessKey(eKeys Key)
{
  int olduseproxy = data.useproxy;
  eOSState state = cMenuSetupPage::ProcessKey(Key);
  if (Key != kNone && (data.useproxy != olduseproxy)) {
     Setup();
     }
  return state;
}

void cPluginRssReaderSetup::Store(void)
{
  RssConfig = data;
  SetupStore("HideMenu",  RssConfig.hidemenu);
  SetupStore("HideElem",  RssConfig.hideelem);
  SetupStore("UseProxy",  RssConfig.useproxy);
  SetupStore("HttpProxy", RssConfig.httpproxy);
}

VDRPLUGINCREATOR(cPluginRssReader); // Don't touch this!
