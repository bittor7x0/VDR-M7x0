/*
 * fussball.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include "osd.h"

static const char *VERSION        = "0.0.3b";
static const char *DESCRIPTION    = "Zeigt die 1.Bundesliga Fussball Ergebnisse an";
static const char *MAINMENUENTRY  = "Fussball";

class cPluginFussball : public cPlugin {
private:
 
public:
  cPluginFussball(void);
  virtual ~cPluginFussball();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginFussball::cPluginFussball(void)
{

}

cPluginFussball::~cPluginFussball()
{

}

const char *cPluginFussball::CommandLineHelp(void)
{

  return NULL;
}

bool cPluginFussball::ProcessArgs(int argc, char *argv[])
{

  return true;
}

bool cPluginFussball::Initialize(void)
{

  return true;
}

bool cPluginFussball::Start(void)
{
	

  return true;
}

void cPluginFussball::Stop(void)
{

}

void cPluginFussball::Housekeeping(void)
{

}

cOsdObject *cPluginFussball::MainMenuAction(void)
{

	int dow_i = system("/usr/local/bin/ergebnisse.sh");
	    dow_i = system("/usr/local/bin/tabelle.sh");
	    
	if (dow_i != 0){
		dsyslog("EROOR: can't execute the Script!");
  
   	
		return false;
		
	}else{
		dsyslog("succenful executed");	
	}
	
  return new cOSD();
}

cMenuSetupPage *cPluginFussball::SetupMenu(void)
{
  return NULL;
}

bool cPluginFussball::SetupParse(const char *Name, const char *Value)
{
   return false;
}

bool cPluginFussball::Service(const char *Id, void *Data)
{
   return false;
}

const char **cPluginFussball::SVDRPHelpPages(void)
{
   return NULL;
}

cString cPluginFussball::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  return NULL;
}

VDRPLUGINCREATOR(cPluginFussball); // Don't touch this!
