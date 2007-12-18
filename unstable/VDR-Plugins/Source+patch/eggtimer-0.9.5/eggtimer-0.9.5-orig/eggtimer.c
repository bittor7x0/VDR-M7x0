/*
 * eggtimer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: eggtimer.c,v 1.7 2006/03/25 13:19:54 peter Exp $
 */

#include <vdr/plugin.h>

#include "eggtimer.h"
#include "set_eggtimer.h"

static const char *VERSION        = "0.9.5";
static const char *DESCRIPTION    = "Eggtimer for VDR";
static const char *MAINMENUENTRY  = "Eggtimer";
static const char *MAINMENUENTRY_STOP  = "Stop eggtimer";

cEggtimerTemplates templates;

class cPluginEggtimer : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cEggtimerThread *bgThread;
  
public:
  cPluginEggtimer(void);
  virtual ~cPluginEggtimer();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
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
  };

cPluginEggtimer::cPluginEggtimer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  bgThread = new cEggtimerThread();
}

cPluginEggtimer::~cPluginEggtimer()
{
  // Clean up after yourself!
  delete bgThread;
}

const char *cPluginEggtimer::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

const char *cPluginEggtimer::MainMenuEntry() {
  if (bgThread->Active())
    return tr(MAINMENUENTRY_STOP);
  else
    return tr(MAINMENUENTRY);
} 

bool cPluginEggtimer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginEggtimer::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginEggtimer::Start(void)
{
  RegisterI18n(Phrases);
  
  // Start any background activities the plugin shall perform.
  if (!templates.Load( AddDirectory( ConfigDirectory(), "eggtimer.conf" ))) {
    fprintf( stderr, "vdr: eggtimer.conf could not be parsed" );
    return false;  
  } 
  
  if (templates.Count() < 1) {
    fprintf( stderr, "vdr: you must have defined at least one entry in eggtimer.conf" );
    return false;
  }
    
  return true;
}

void cPluginEggtimer::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginEggtimer::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginEggtimer::MainMenuAction(void)
{
  if ( !bgThread->Active() ) {    
    return new cSetEggTimerMenu(bgThread);
  } else {
    bgThread->Stop();
    return NULL;
  }
}

cMenuSetupPage *cPluginEggtimer::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginEggtimer::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

bool cPluginEggtimer::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginEggtimer::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginEggtimer); // Don't touch this!
