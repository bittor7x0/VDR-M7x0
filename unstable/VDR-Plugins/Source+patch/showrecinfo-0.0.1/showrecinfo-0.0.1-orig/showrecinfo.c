/*
 * showrecinfo.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/recording.h>
#include <vdr/menu.h>
#include "i18n.h"
#include "menu_main.h"
#include "showrecinfocfg.h"
#include "rcfile.h"

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Shows detailed information about recordings";
static const char *MAINMENUENTRY  = "Show recording information";

class cPluginShowrecinfo : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  virtual cRecording *GetRecording(char* rcFilename);
public:
  cPluginShowrecinfo(void);
  virtual ~cPluginShowrecinfo();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) {  return (ShowRecInfoConfig.hidemenu ? NULL : tr(MAINMENUENTRY)); } 
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginShowrecinfo::cPluginShowrecinfo(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginShowrecinfo::~cPluginShowrecinfo()
{
  // Clean up after yourself!
}

const char *cPluginShowrecinfo::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginShowrecinfo::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginShowrecinfo::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  return true;
}

bool cPluginShowrecinfo::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginShowrecinfo::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginShowrecinfo::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cRecording *cPluginShowrecinfo::GetRecording(char* rcFilename)
{
    cRCFile rcFile;
    if (rcFile.Load(rcFilename)) {
#if VDRVERSNUM <= 10310
      cRecordings Recordings;
      bool recordings = Recordings.Load();
      if (!recordings)
          return NULL;
#endif
      cRecording *recording = Recordings.GetByName(rcFile.recordingfile);
      if (!recording)
         esyslog("cPluginShowrecinfo: recording not found! %s", rcFile.recordingfile);
      return recording;
   }
   else
      return NULL;
}

cOsdObject *cPluginShowrecinfo::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  cOsdObject* pMenu = NULL;
  char* rcFilename = strdup(*AddDirectory( cPlugin::ConfigDirectory(), ".showrecinfo"));
  if (access(rcFilename, F_OK) == 0)
      pMenu = new cMenuShowInfoMain(GetRecording(rcFilename));
  else
      pMenu = new cMenuShowInfoMain();

  free(rcFilename);
  return pMenu;
}

cMenuSetupPage *cPluginShowrecinfo::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginShowrecinfo::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if (!strcasecmp(Name, "HideMenu"))  ShowRecInfoConfig.hidemenu         = atoi(Value);
  else return false;
  return true;
}

VDRPLUGINCREATOR(cPluginShowrecinfo); // Don't touch this!
