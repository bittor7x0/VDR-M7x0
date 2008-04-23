/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */

#include <vdr/plugin.h>
#include "mpgplayer_i18n.h"
#include "mpgplayer_controller.h"
#include "mpgplayer_menu.h" 

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "A player for MPG PS files";
static const char *MAINMENUENTRY  = "MpgPlayer";

class cPluginMpgplayer : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cMpgController *controller;
  cMpgRemoveDeleted *mpgCleaner;
public:
  cPluginMpgplayer(void);
  virtual ~cPluginMpgplayer();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginMpgplayer::cPluginMpgplayer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  controller = NULL;
  mpgCleaner = NULL;
}

cPluginMpgplayer::~cPluginMpgplayer()
{
  // Clean up after yourself!
  delete controller;
}

const char *cPluginMpgplayer::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginMpgplayer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginMpgplayer::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  RegisterI18n(mpgplayer_phrases);
  delete controller;
  controller = new cMpgController();
  return controller;
}

bool cPluginMpgplayer::Start(void)
{
  // Start any background activities the plugin shall perform.
  //TODO:  mpgCleaner = new cMpgRemoveDeleted();  
  // return mpgCleaner;
  return true;
}

void cPluginMpgplayer::Stop(void)
{
  // Stop any background activities the plugin shall perform.
  SetupStore("LastPath", controller->GetCurrentPath());
  //TODO: delete mpgCleaner;
  mpgCleaner = NULL;
}

void cPluginMpgplayer::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
  if (mpgCleaner)
     mpgCleaner->StartRemoveOnNeed();
}

void cPluginMpgplayer::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginMpgplayer::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

cOsdObject *cPluginMpgplayer::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cMpgMenuRecordings(controller, controller->GetSeriesSelected()
              && controller->GetRecordingSelected());
}

cMenuSetupPage *cPluginMpgplayer::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
    return new cMpgSetupMenu(controller);
}

bool cPluginMpgplayer::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
    // Parse your own setup parameters and store their values.
  if      (!strcasecmp("PathBase",Name)) {
          if (controller)
             controller->SetBasePath(Value);
          else
             MpgplayerSetup.SetBasePath(Value);
          }
  else if (!strcasecmp("LastPath",Name)) {
          if (controller)
             controller->SetInitPath(Value);
          else
             MpgplayerSetup.SetLastPath(Value);
          }
  else if (!strcasecmp("ShowHiddenFiles",Name)) {
          if (controller)
             controller->SetShowHidden(atoi(Value));
          else
             MpgplayerSetup.ShowHidden=atoi(Value);
          }
  else if (!strcasecmp("StartMargin",Name)) MpgplayerSetup.RecordingPreGap = atoi(Value);
  else if (!strcasecmp("StopMargin",Name)) MpgplayerSetup.RecordingPostGap = atoi(Value);
  else if (!strcasecmp("DirectoryMarkingPrefix",Name)) {
     strncpy(MpgplayerSetup.DirMarkerPre,Value,MAXMARKERLEN);
     MpgplayerSetup.DirMarkerPre[MAXMARKERLEN] = 0;
     }
  else if (!strcasecmp("DirectoryMarkingSuffix",Name)) {
     strncpy(MpgplayerSetup.DirMarkerPost,Value,MAXMARKERLEN);
     MpgplayerSetup.DirMarkerPost[MAXMARKERLEN] = 0;
     }
  else return false;
  return true;

}

bool cPluginMpgplayer::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginMpgplayer::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginMpgplayer::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginMpgplayer); // Don't touch this!
