/*
 * osdadjust.c: An EPG plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include "config.h"
#include "i18n.h"
#include "menu.h"
#include "screenmenu.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = trNOOP("Adjust OSD Size and Position");
static const char *MAINMENUENTRY  = trNOOP("OSD adjust");


class cPluginOsdadjust : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginOsdadjust(void);
  virtual ~cPluginOsdadjust();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return config.HideMenuEntry ? NULL : tr(config.MainMenuEntry); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
};

cPluginOsdadjust::cPluginOsdadjust(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!

  // Default configuration values
  config.AspectRatio = 0; // default: 4:3
  config.VideoFormat = 0; // default: auto
  config.Mute = false;
  config.HideMenuEntry = false;
  config.HideHelpText  = false;
  config.ShowGrid = false;
  config.BgColor = 10;  // default: gray
  config.FgColor = 9;   // default: white
  config.LineTn  = 3;
  config.MoveInc = 1;

  config.MainMenuEntry = MAINMENUENTRY;
}

cPluginOsdadjust::~cPluginOsdadjust()
{
  // Clean up after yourself!
}

const char *cPluginOsdadjust::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginOsdadjust::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}


bool cPluginOsdadjust::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
#if VDRVERSNUM < 10507
  RegisterI18n(Phrases);
#endif
  return true;
}

bool cPluginOsdadjust::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginOsdadjust::Stop(void) {
  // Stop any background activities the plugin is performing.
  }

void cPluginOsdadjust::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginOsdadjust::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cScreenMenu();
}

cMenuSetupPage *cPluginOsdadjust::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupOsdadjust();
}

bool cPluginOsdadjust::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "Mute"))             config.Mute            = atoi(Value);
  else if (!strcasecmp(Name, "HideMenuEntry"))    config.HideMenuEntry   = atoi(Value);
  else if (!strcasecmp(Name, "HideHelpText"))     config.HideHelpText    = atoi(Value);
  else if (!strcasecmp(Name, "AspectRatio"))      config.AspectRatio     = atoi(Value);
  else if (!strcasecmp(Name, "VideoFormat"))      config.VideoFormat     = atoi(Value);
  else if (!strcasecmp(Name, "BgColor"))          config.BgColor         = atoi(Value);
  else if (!strcasecmp(Name, "FgColor"))          config.FgColor         = atoi(Value);
  else if (!strcasecmp(Name, "LineTn"))           config.LineTn          = atoi(Value);
  else if (!strcasecmp(Name, "MoveInc"))          config.MoveInc         = atoi(Value);
  else if (!strcasecmp(Name, "ShowGrid"))         config.ShowGrid        = atoi(Value);
  else return false;

  return true;
}

sOsdadjustConfig config;

VDRPLUGINCREATOR(cPluginOsdadjust); // Don't touch this!

