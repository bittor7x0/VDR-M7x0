/***************************************************************************
                          commands.c  -  description
                             -------------------
    begin                : Sa Jan 15 2005
    copyright            : (C) 2005 by Holger Brunn
    email                : holger.brunn@stud.uni-karlsruhe.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <malloc.h>
#include <string.h>
#include <fnmatch.h>
#include <sys/wait.h>
#include <vdr/tools.h>
#include <vdr/thread.h>
#include <vdr/menuitems.h>
#include "commands.h"
#ifdef FILEBROWSER_PLUGIN_BUILD
#include "menu-threads.h"
#endif
#include "statebag.h"

cFilebrowserCommand::cFilebrowserCommand(cFilebrowserStatebag* Statebag)
{
  Name=   NULL;
  Pattern=NULL;
  Confirm=false;
  UseDestination=false;
  UseCurrentFile=false;
  UseSelectedFiles=false;
  UseNewDestination=false;
  Synchronous=false;
  Menu=true;
  AccessCode=NULL;
  RemoveFinished=false;
  PatternIsShellscript=false;
  State=osContinue;
  this->Statebag=Statebag;
}

cFilebrowserCommand::~cFilebrowserCommand()
{
  if(Name)    free(Name);
  if(Pattern) free(Pattern);
  if(AccessCode) free(AccessCode);
}

bool cFilebrowserCommand::Matches(const char* Filename)
{
  bool Match=true;
  
  if(Statebag && (Statebag->GetSelectedFiles()->Count() > 0))
  {
    for(cStringContainer* i=Statebag->GetSelectedFiles()->First(); i; i=Statebag->GetSelectedFiles()->Next(i))
    {
      Match&=MatchesSingleFile(i->GetObject());
      if(!Match)
      {
        break;
      }
    }
  }
  else
  {
    Match=MatchesSingleFile(Filename);
  }
  return Match;
}

bool cFilebrowserCommand::MatchesSingleFile(const char* Filename)
{
  if(!Filename)
  {
    return false;
  }
  if(PatternIsShellscript)
  {
    cCommandParser* Parser=new cCommandParser(Pattern);
    Parser->AddReplacement('f', Filename);
    cString Command=Parser->Parse();
    D(fprintf(stderr, "Trying to execute %s\n", *Command));
    int Status=SystemExec(*Command);
    D(fprintf(stderr, "Executed \"%s\": %d\n", *Command, Status));
    return Status == 0;
  }
  else
  {
    if(Pattern[0]!='/')
    {
      char* filename_tmp=NULL;
      Filename=(Filename && (filename_tmp=strrchr(Filename, '/'))) ? filename_tmp + 1 : NULL;
    }
    if(Filename==NULL)
    {
      return false;
    }
    return fnmatch(Pattern, Filename, FNM_FILE_NAME | FNM_EXTMATCH) != FNM_NOMATCH;
  }
}

char* cFilebrowserCommand::GetName()
{
  return Name;
}

bool cFilebrowserCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  return true;
}

eKeys cFilebrowserCommand::GetHotkey()
{
  return kNone;
}

#ifdef FILEBROWSER_PLUGIN_BUILD

/*
  Implementation cConfigCommand
*/

cFilebrowserConfigCommand::cFilebrowserConfigCommand(cFilebrowserStatebag* Statebag) : cFilebrowserCommand(Statebag)
{
   Command=NULL;
};

char* cFilebrowserConfigCommand::FlagHandler(const char* OrgString, const char* CurrentPos, const cCommandParser::cHandlerParameters* Params)
{
  *((bool*)Params->Data)=true;
  return NULL;
}

cFilebrowserConfigCommand* cFilebrowserConfigCommand::Parse(char* Config, cFilebrowserStatebag* Statebag)
{
  cFilebrowserConfigCommand* Command=new cFilebrowserConfigCommand(Statebag);
  
  cConfigParser Parser=cConfigParser(Config);
  for(char* Text=Parser.First(); Text; Text=Parser.Next())
  {
    switch(Parser.GetFieldNumber())
    {
      case 0: //The name of this command
        if(Text[Parser.GetCurrentLength()-1]=='?')
        {
          Text[Parser.GetCurrentLength()-1]='\0';
          Command->Confirm=true;
          D(fprintf(stderr, "I will ask "));
        }
        D(fprintf(stderr, "[doing realloc] "));
        Command->Name=(char*)realloc(Command->Name, Parser.GetCurrentLength() + 1);
        D(fprintf(stderr, "[copying %s] ", Text));
        strcpy(Command->Name, Text);
        D(fprintf(stderr, "\"%s\" ", Command->Name));
        break;
      case 1: //The pattern of this command
        Command->PatternIsShellscript=(Text[0]=='!');
        Command->Pattern=(char*)realloc(Command->Pattern, Parser.GetCurrentLength() + 1);
        strcpy(Command->Pattern, Text + (Command->PatternIsShellscript ? 1 : 0));
        D(fprintf(stderr, "\"%s\" ", Command->Pattern));
        break;
      case 2: //The command string of this command
        Command->Command=(char*)realloc(Command->Command, Parser.GetCurrentLength() + 1);
        strcpy(Command->Command, Text);
        D(fprintf(stderr, "\"%s\" ", Command->Command));
        break;
      case 3: //Flags for this command
        Command->Synchronous=strchr(Text, 's');
        Command->Menu=!strchr(Text, 'b');
        Command->RemoveFinished=strchr(Text, 'r');
        D(fprintf(stderr, " Sync:%d Menu:%d Remove:%d ", Command->Synchronous, Command->Menu, Command->RemoveFinished));
        break;
      case 4: //An access code
        Command->AccessCode=strdup(Text);
        D(fprintf(stderr, " Access code: %s ", Command->AccessCode));
        break;
    }
  }
  
  D(fprintf(stderr, "\n"));

  if(!Command->Pattern || !Command->Command)
  {
    D(fprintf(stderr, "Misconfigured commands.conf - skipping this entry [was \"%s\"]!\n", Config));
    esyslog("[filebrowser] Misconfigured commands.conf - skipping this entry [was \"%s\"]!\n", Config);
    return NULL;
  }
  
  //Set Use-Flags
  cCommandParser CommandParser(Command->Command, false, false);
  CommandParser.AddHandler('D', &cFilebrowserConfigCommand::FlagHandler, &Command->UseNewDestination);
  CommandParser.AddHandler('d', &cFilebrowserConfigCommand::FlagHandler, &Command->UseDestination);
  CommandParser.AddHandler('M', &cFilebrowserConfigCommand::FlagHandler, &Command->UseSelectedFiles);
  CommandParser.AddHandler('m', &cFilebrowserConfigCommand::FlagHandler, &Command->UseSelectedFiles);
  CommandParser.AddHandler('f', &cFilebrowserConfigCommand::FlagHandler, &Command->UseCurrentFile);
  CommandParser.Parse();
  
  D(fprintf(stderr, "NewDest %d, Dest %d, SelFiles %d, File %d\n", Command->UseNewDestination, Command->UseDestination, Command->UseSelectedFiles, Command->UseCurrentFile));
  
  return Command;
}

cFilebrowserConfigCommand::~cFilebrowserConfigCommand()
{
  if(Command) free(Command);
}

bool cFilebrowserConfigCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  D(fprintf(stderr, "got dest %s and %d marked files\n", DestinationFile, Statebag->GetSelectedFiles()->Count()));
  cConfigCommandThread* Thread=new cConfigCommandThread(Statebag, DestinationFile, CurrentFile, this);
  if(Synchronous)
  {
    //TODO: Move this to filebrowser.c, implement some kind of wait function to wait until the thread is finished
    pid_t child_pid=fork();
    if(child_pid == 0)
    {
      int MaxPossibleFileDescriptors = getdtablesize();
      for (int i = STDERR_FILENO + 1; i < MaxPossibleFileDescriptors; i++)
        close(i);
      Thread->Execute();
    }
    else if(child_pid < 0)
    {
      LOG_ERROR;
    }
    else
    {
      int status;
      waitpid(child_pid, &status, 0);
      delete Thread;
    }
  }
  else
  {
    Statebag->GetThreads()->Add(new cThreadContainer(Thread));
  }
  return true;
}

char* cFilebrowserConfigCommand::GetCommand()
{
  return Command;
}

#endif

/*
  Implementation cFilebrowserMarkCommand
*/
cFilebrowserMarkCommand::cFilebrowserMarkCommand(cFilebrowserStatebag* Statebag) : cFilebrowserCommand(Statebag)
{
  Name=strcpyrealloc(Name, tr("Mark"));
}
bool cFilebrowserMarkCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  cOsdMenuFilebrowser* FbMenu=(cOsdMenuFilebrowser*)Menu;
  cOsdItemFileEntry* Entry=(cOsdItemFileEntry*)FbMenu->Get(FbMenu->Current());
  Statebag->GetSelectedFiles()->Add(new cStringContainer(strdup(Entry->GetFilename())));
  Entry->Mark(true);
  FbMenu->DisplayCurrent(true);
  FbMenu->UpdateHelp();
  return true;
}

bool cFilebrowserMarkCommand::Matches(const char* Filename)
{
  return Filename && !Statebag->GetSelectedFiles()->Contains(Filename) && strcmp(strlen(Filename) > 2 ? Filename + strlen(Filename) - 2 : "", "..") != 0;
}

/*
  Implementation cFilebrowserMarkAllCommand
*/
cFilebrowserMarkAllCommand::cFilebrowserMarkAllCommand(cFilebrowserStatebag* Statebag) : cFilebrowserCommand(Statebag)
{
  Name=strcpyrealloc(Name, tr("Mark all"));
}
bool cFilebrowserMarkAllCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  cOsdMenuFilebrowser* FbMenu=(cOsdMenuFilebrowser*)Menu;
  for(cOsdItemFileEntry* Entry=(cOsdItemFileEntry*)FbMenu->First(); Entry; Entry=(cOsdItemFileEntry*)Entry->Next())
  {
    if(!Entry->IsMarked() && !Entry->IsParentDirectory())
    {
      Entry->Mark(true);
      Statebag->GetSelectedFiles()->Add(new cStringContainer(strdup(Entry->GetFilename())));
    }
  }
  FbMenu->UpdateHelp();
  FbMenu->Display();
  return true;
}

bool cFilebrowserMarkAllCommand::Matches(const char* Filename)
{
  return true;
}

/*
  Implementation cFilebrowserUnmarkCommand
*/
cFilebrowserUnmarkCommand::cFilebrowserUnmarkCommand(cFilebrowserStatebag* Statebag) : cFilebrowserCommand(Statebag)
{
  Name=strcpyrealloc(Name, tr("Unmark"));
}
bool cFilebrowserUnmarkCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  cOsdMenuFilebrowser* FbMenu=(cOsdMenuFilebrowser*)Menu;
  cOsdItemFileEntry* Entry=(cOsdItemFileEntry*)FbMenu->Get(FbMenu->Current());
  Statebag->GetSelectedFiles()->Remove(Entry->GetFilename());
  Entry->Mark(false);
  FbMenu->DisplayCurrent(true);
  FbMenu->UpdateHelp();
  return true;
}

bool cFilebrowserUnmarkCommand::Matches(const char* Filename)
{
  return Filename && Statebag->GetSelectedFiles()->Contains(Filename) && strcmp(strlen(Filename) > 2 ? Filename + strlen(Filename) - 2 : "", "..") != 0;
}

/*
  Implementation cFilebrowserUnmarkAllCommand
*/
cFilebrowserUnmarkAllCommand::cFilebrowserUnmarkAllCommand(cFilebrowserStatebag* Statebag) : cFilebrowserCommand(Statebag)
{
  Name=strcpyrealloc(Name, tr("Unm. all"));
}
bool cFilebrowserUnmarkAllCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  cOsdMenuFilebrowser* FbMenu=(cOsdMenuFilebrowser*)Menu;
  Statebag->GetSelectedFiles()->Clear();
  for(cOsdItemFileEntry* Entry=(cOsdItemFileEntry*)FbMenu->First(); Entry; Entry=(cOsdItemFileEntry*)Entry->Next())
  {
    Entry->Mark(false);
  }
  FbMenu->UpdateHelp();
  FbMenu->Display();
  return true;
}

bool cFilebrowserUnmarkAllCommand::Matches(const char* Filename)
{
  return Statebag->GetSelectedFiles()->Count() > 0;
}

/*
  Implementation of cFilebrowserDestinationContainerCommand
*/
cFilebrowserDestinationContainerCommand::cFilebrowserDestinationContainerCommand(cFilebrowserCommand* Command, char* CurrentFile, cFilebrowserStatebag* Statebag)
  : cFilebrowserCommand(Statebag)
{
  this->Command=Command;
  UseSelectedFiles=Command->UsesSelectedFiles();
  UseNewDestination=Command->UsesNewDestination();
  UseCurrentFile=Command->UsesCurrentFile();
  Menu=Command->ShowMenu();
  Synchronous=Command->IsSynchronous();
  RemoveFinished=Command->RemoveWhenFinished();
  Confirm=Command->NeedsConfirmation();
  this->CurrentFile=strdup(CurrentFile);
  DestinationFile=NULL;
  Name=strcpyrealloc(Name, tr("OK"));
}
cFilebrowserDestinationContainerCommand::~cFilebrowserDestinationContainerCommand()
{
  if(CurrentFile) free(CurrentFile);
  if(DestinationFile) free(DestinationFile);
}
bool cFilebrowserDestinationContainerCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  if(this->DestinationFile) free(this->DestinationFile);
  this->DestinationFile=strdup(CurrentFile);
  
  for(cOsdItem* i=Menu->First(); i!=NULL; i=Menu->cOsdMenu::Next(i))
  {
    if(dynamic_cast<cOsdItemNewFileEntry*>(i))
    {
      ((cOsdMenuFilebrowser*)Menu)->Del(i->Index());
    }
  }
  return Command->Execute(Menu, this->DestinationFile, this->CurrentFile);
}
bool cFilebrowserDestinationContainerCommand::Matches(const char* Filename)
{
  return Command ? Command->MatchesDestination(Filename) : false;
}

char* cFilebrowserDestinationContainerCommand::GetAccessCode()
{
  return Command ? Command->GetAccessCode() : NULL;
}

/*
  Implementation of cFilebrowserDestinationAbortCommand
*/
cFilebrowserDestinationAbortCommand::cFilebrowserDestinationAbortCommand(cFilebrowserStatebag* Statebag)
  : cFilebrowserCommand(Statebag)
{
  Name=strcpyrealloc(Name, tr("Abort"));
}
bool cFilebrowserDestinationAbortCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  ((cOsdMenuFilebrowser*)Menu)->Task=taskBrowse;
  for(cOsdItem* i=Menu->First(); i!=NULL; i=Menu->cOsdMenu::Next(i))
  {
    if(dynamic_cast<cOsdItemNewFileEntry*>(i))
    {
      ((cOsdMenuFilebrowser*)Menu)->Del(i->Index());
    }
  }
  Menu->Display();
  return true;
}
bool cFilebrowserDestinationAbortCommand::Matches(const char* Filename)
{
  return true;
}

/*
  Implementation of cFilebrowserDestinationNewCommand
*/
cFilebrowserDestinationNewCommand::cFilebrowserDestinationNewCommand(cFilebrowserStatebag* Statebag)
  : cFilebrowserCommand(Statebag)
{
  Name=strcpyrealloc(Name, tr("New"));
}
bool cFilebrowserDestinationNewCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  char* val=(char*)malloc(255);
  sprintf(val, tr("New file"));
  cOsdItemNewFileEntry* NewItem=new cOsdItemNewFileEntry(val, *(Statebag->CurrentDirectory));
  Menu->Add(NewItem, true);
  ((cOsdMenuFilebrowser*)Menu)->DisplayCurrent(true);
  NewItem->ProcessKey(kNone);
  Menu->Display();
  return true;
}
bool cFilebrowserDestinationNewCommand::Matches(const char* Filename)
{
  return true;
}

#ifdef FILEBROWSER_PLUGIN_BUILD

/*
  Implementation of cFilebrowserCommandThreadList
*/
cFilebrowserCommandThreadList::cFilebrowserCommandThreadList(cFilebrowserStatebag* Statebag) : cFilebrowserCommand(Statebag)
{
  Name=strcpyrealloc(Name, tr("Threads"));
}

bool cFilebrowserCommandThreadList::Execute(cOsdMenu* Menu, char* DestinationFile, char* SelectedFile)
{
  cOsdMenuFilebrowser* FbMenu=(cOsdMenuFilebrowser*)Menu;
  FbMenu->AddSubMenu(new cOsdMenuThreadList(Statebag));
  return true;
}
bool cFilebrowserCommandThreadList::Matches(const char* Filename)
{
  return true;
}

#endif
