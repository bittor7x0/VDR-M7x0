/*
*  C Implementation: command-other.c
*
* Description: 
*
*
* Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2005
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include "command-other.h"
#include "menu-filebrowser.h"
#include "tools.h"

cFilebrowserCommandOtherCommands::cFilebrowserCommandOtherCommands(cFilebrowserStatebag* Statebag) : cFilebrowserCommand(Statebag)
{
  Name=strdup(tr("Other"));
  cConfigReader Reader=cConfigReader(Statebag, "othercommands.conf");
  char* Line=NULL;
  Commands=new cFilebrowserCommands();
  while((Line=Reader.Read()))
  {
    D(fprintf(stderr, "Searching %s in Commands\n", Line));
    cFilebrowserCommandContainer* Command=Statebag->GetCommands()->First();
    while(Command)
    {
      if(strcmp(Command->GetObject()->GetName(), Line)==0)
      {
        cFilebrowserCommandContainer* Dummy=Command->Next();
        Statebag->GetCommands()->Del(Command, false);
        Commands->Add(Command);
        Command=Dummy;
        D(fprintf(stderr, "Found %s in Commands\n", Line));
      }
      else
      {
        Command=Command->Next();
      }
    }
  }
}

cFilebrowserCommandOtherCommands::~cFilebrowserCommandOtherCommands()
{
  if(Commands) delete Commands;
}

bool cFilebrowserCommandOtherCommands::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  cOsdMenuFilebrowser* FilebrowserMenu=(cOsdMenuFilebrowser*)Menu;
  FilebrowserMenu->AddSubMenu(new cOsdMenuOtherCommands(Commands, CurrentFile, FilebrowserMenu->ActualCommands));
  return true;
}
    
bool cFilebrowserCommandOtherCommands::Matches(const char* Filename)
{
  return true;
}

cOsdMenuOtherCommands::cOsdMenuOtherCommands(cFilebrowserCommands* Commands, char* CurrentFile, cFilebrowserCommands* RunCommands) : cOsdMenu(tr("Other Commands"))
{
  this->Commands=new cFilebrowserCommands();
  this->RunCommands=RunCommands;
  SetHasHotkeys();
  for(cFilebrowserCommandContainer* Command=Commands->First(); Command; Command=Command->Next())
  {
    if(Command->GetObject()->Matches(CurrentFile))
    {
      Add(new cOsdItem(hk(Command->GetObject()->GetName()), (eOSState)osFilebrowserCloseExec));
      this->Commands->Add(new cFilebrowserCommandContainer(Command->GetObject(), false));
    }
  }
}

cOsdMenuOtherCommands::~cOsdMenuOtherCommands()
{}

eOSState cOsdMenuOtherCommands::ProcessKey(eKeys Key)
{
  eOSState State=cOsdMenu::ProcessKey(Key);
  if(State==(eOSState)osFilebrowserCloseExec)
  {
    RunCommands->Clear();
    RunCommands->Add(new cFilebrowserCommandContainer(Commands->Get(Current())->GetObject(), false));
  }
  switch(Key)
  {
    case kGreen:
    case kRed:
    case kBlue:
    case kYellow:
      State=osContinue;
      break;
    default:
      break;
  }
  return State;
}


