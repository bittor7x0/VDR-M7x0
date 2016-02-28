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
#include <vdr/menuitems.h>
#include "commands.h"

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
  RemoveFinished=false;
  State=osContinue;
  this->Statebag=Statebag;
}

cFilebrowserCommand::~cFilebrowserCommand()
{
  if(Name)    free(Name);
  if(Pattern) free(Pattern);
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
