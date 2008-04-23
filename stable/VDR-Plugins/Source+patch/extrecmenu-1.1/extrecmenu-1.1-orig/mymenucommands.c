/*
 * See the README file for copyright information and how to reach the author.
 *
 * This code is directly taken from VDR with some changes by me to work with this plugin
 */

#include <vdr/menu.h>
#include <vdr/config.h>
#include <vdr/interface.h>
#include "mymenucommands.h"

myMenuCommands::myMenuCommands(const char *Title,cCommands *Commands,const char *Parameters):cOsdMenu(Title)
{
 SetHasHotkeys();
 commands=Commands;
 parameters=Parameters?strdup(Parameters):NULL;
 for(cCommand *command=commands->First();command;command=commands->Next(command))
  Add(new cOsdItem(hk(command->Title())));
}

myMenuCommands::~myMenuCommands()
{
 free(parameters);
}

eOSState myMenuCommands::Execute()
{
 cCommand *command=commands->Get(Current());
 if(command)
 {
  char *buffer=NULL;
  bool confirmed=true;
#ifdef CMDSUBMENUVERSNUM
     if (command->hasChilds()) {
        AddSubMenu(new myMenuCommands(command->Title(), command->getChilds(), parameters));
        return osContinue;
        }
#endif
  if(command->Confirm()) {
   asprintf(&buffer,"%s?",command->Title());
   confirmed=Interface->Confirm(buffer);
   free(buffer);
  }
  if(confirmed)
  {
   asprintf(&buffer, "%s...",command->Title());
   Skins.Message(mtStatus,buffer);
   free(buffer);
   const char *Result=command->Execute(parameters);
   Skins.Message(mtStatus, NULL);
   if(Result)
    return AddSubMenu(new cMenuText(command->Title(),Result,fontFix));
   return osEnd;
  }
 }
 return osContinue;
}

eOSState myMenuCommands::ProcessKey(eKeys Key)
{
 eOSState state=cOsdMenu::ProcessKey(Key);

 if(state==osUnknown)
 {
  switch(Key)
  {
   case kRed:
   case kGreen:
   case kYellow:
   case kBlue: return osContinue;
   case kOk: return Execute();
   default: break;
  }
 }
 return state;
}
