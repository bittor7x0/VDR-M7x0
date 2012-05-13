/*
 * filebrowser.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: filebrowser.c 35 2007-04-12 14:22:05Z gambler $
 */

#include <vdr/plugin.h>
#include <vdr/i18n.h>
#include "menu-filebrowser.h"
#include "commands.h"
#include "statebag.h"
#include "i18n.h"
#include "menu-setup.h"
#include "command-sources.h"
#include "command-other.h"
#include "commands-plugins.h"

static const char *VERSION        = "0.0.6b";
static const char *DESCRIPTION    = "Plugin to browse through files and execute actions on them";
static const char *MAINMENUENTRY  = "Filebrowser";

class cPluginFilebrowser : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cFilebrowserStatebag* Statebag;
public:
  cPluginFilebrowser(void);
  virtual ~cPluginFilebrowser();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
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

cPluginFilebrowser::cPluginFilebrowser(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  Statebag=new cFilebrowserStatebag();
}

cPluginFilebrowser::~cPluginFilebrowser()
{
  // Clean up after yourself!
  delete Statebag;
}

const char *cPluginFilebrowser::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginFilebrowser::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginFilebrowser::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginFilebrowser::Start(void)
{
  RegisterI18n(Phrases);

  Statebag->FilebrowserPlugin=this;
  
  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserMarkCommand(Statebag)));
  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserUnmarkCommand(Statebag)));
  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserMarkAllCommand(Statebag)));
  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserUnmarkAllCommand(Statebag)));

  Statebag->ConfigDirectory=ConfigDirectory(Name());
  
  cConfigReader CommandsReader=cConfigReader(Statebag, "commands.conf");
  char* Line=NULL;
  while((Line=CommandsReader.Read()))
  {
    cFilebrowserConfigCommand* Command=cFilebrowserConfigCommand::Parse(Line, Statebag);
    if(Command)
    {
      Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(Command));
    }
  }

  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserCommandImagePlugin()));
  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserCommandMPlayerPlugin()));  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserCommandMP3Plugin()));
  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserCommandThreadList(Statebag)));
  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserCommandSourcesList(Statebag)));
  Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cFilebrowserCommandOtherCommands(Statebag)));

  /*
  Do ordering: Mentioned commands first, then unmentioned
  */

  cConfigReader OrderReader=cConfigReader(Statebag, "order.conf");
  cFilebrowserCommandContainer* Current=Statebag->GetCommands()->First();
  while((Line=OrderReader.Read()))
  {
    cCommandParser Parser=cCommandParser(Line, false, false);
    cString ParsedLine=Parser.Parse();
    const char* Name=(Line[0]=='-' ? *ParsedLine + 1 : *ParsedLine);
    D(fprintf(stderr, "Searching %s in Commands\n", Name));
    for(cFilebrowserCommandContainer* Command=Current; Command; Command=Command->Next())
    {
      if(strcmp(Command->GetObject()->GetName(), Name)==0)
      {
        if(Line[0]=='-')
        {
          D(fprintf(stderr, "Deleting %s from commands\n", Name));
          Current=Command->Next();
          Command->Unlink();
          delete Command;
          break;
        }
        if(Command!=Current)
        {
          //TODO: Move behaves strange when you do Move(foo, foo) - is this a bug or by design?
          //Then foo->Next()=foo, foo->Prev()=foo...
          Statebag->GetCommands()->Move(Command, Current);
        }
        Current=Command->Next();
        D(fprintf(stderr, "Found %s in Commands - Command is %s, Current is %s\n", Name, Command->GetObject()->GetName(), Current->GetObject()->GetName()));
      }
    }
  }

  D(fprintf(stderr, "Now Commands are Ordered:\n"));
  for(cFilebrowserCommandContainer* Command=Statebag->GetCommands()->First(); Command; Command=Command->Next())
  {
    D(fprintf(stderr, "%s\n", Command->GetObject()->GetName()));
  }

  return true;
}

void cPluginFilebrowser::Housekeeping(void)
{
  D(fprintf(stderr, "Housekeeping started\n"));
  cThreadContainer* i=Statebag->GetThreads()->First();
  while(i)
  {
    if(i->GetObject()->IsRemoveRequested())
    {
      cThreadContainer* tmp=i;
      i=Statebag->GetThreads()->Next(i);
      Statebag->GetThreads()->Del(tmp);
      continue;
    }
    else
    {
      i=Statebag->GetThreads()->Next(i);
    }
  }
  D(fprintf(stderr, "Housekeeping ended\n"));
}

cOsdObject *cPluginFilebrowser::MainMenuAction(void)
{

  D(fprintf(stderr, "starting menu\n"));
  return new cOsdMenuFilebrowser(Statebag->BaseDir, Statebag);
/*
quick hack to show sources menu first when starting filebrowser

  cOsdMenu* Menu=new cOsdMenuFilebrowser(Statebag->BaseDir, Statebag);
  cFilebrowserCommandSourcesList* SourcesCommand=new cFilebrowserCommandSourcesList(Statebag);
  SourcesCommand->Execute(Menu, NULL, NULL);
  return Menu;
*/
}

cMenuSetupPage *cPluginFilebrowser::SetupMenu(void)
{
  return new cFilebrowserSetupPage(Statebag);
}

bool cPluginFilebrowser::SetupParse(const char *Name, const char *Value)
{
  if(strcmp(Name, SETUP_PARAM_BASEDIR)==0)
  {
    strcpy(Statebag->BaseDir, Value);
  }
  if(strcmp(Name, SETUP_PARAM_FILTER)==0)
  {
    Statebag->Filter=cString(Value);
  }
  else if(strcmp(Name, SETUP_PARAM_SHOW_HIDDEN_FILES)==0)
  {
    Statebag->ShowHiddenFiles=atoi(Value);
  }
  else if(strcmp(Name, SETUP_PARAM_SHOW_FULL_PATH)==0)
  {
    Statebag->ShowFullPath=atoi(Value);
  }
  else
  {
    return false;
  }
  return true;
}

VDRPLUGINCREATOR(cPluginFilebrowser); // Don't touch this!
