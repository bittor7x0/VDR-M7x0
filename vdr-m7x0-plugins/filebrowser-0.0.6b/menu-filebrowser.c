/***************************************************************************
                          filebrowser_menu.cpp  -  description
                             -------------------
    begin                : Sa Jan 8 2005
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

#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <string.h>
#include <vdr/i18n.h>
#include <vdr/menu.h>
#include <vdr/skins.h>
#include <vdr/interface.h>
#include "menu-filebrowser.h"
#include "commands.h"
#ifdef FILEBROWSER_PLUGIN_BUILD
#include "threads.h"
#include "command-other.h"
#endif
#include "tools.h"
#include "menu-accesscode.h"

void cOsdItemFileEntry::UpdateText()
{
  char name[strlen(Name) + 5];
  struct stat64 buf;

  if(stat64(Filename, &buf) == 0 && S_ISDIR(buf.st_mode))
  {
    if(Marked)
    {
      sprintf(name, "*[%s]*", Name);
    }
    else
    {
      sprintf(name, "[%s]", Name);
    }
  }
  else
  {
    if(Marked)
    {
      sprintf(name, "*%s*", Name);
    }
    else
    {
      sprintf(name, "%s", Name);
    }
  }
  SetText(name);
}

bool cOsdItemFileEntry::IsDir()
{
  struct stat64 buf;

  return stat64(Filename, &buf) == 0 && S_ISDIR(buf.st_mode);
}
    
cOsdItemFileEntry::cOsdItemFileEntry(dirent64* DirectoryEntry, cString Directory, cFilebrowserStatebag* Statebag) : cOsdItem("")
{
  Filename=(char*)malloc(strlen(Directory) + strlen(DirectoryEntry->d_name) + 2);
  bool NeedsSlash=strlen(*Directory) > 0 && (*Directory)[strlen(*Directory)-1]!='/';
  sprintf(Filename, "%s%s%s", *Directory, NeedsSlash ? "/" : "", DirectoryEntry->d_name);
  Name=strdup(DirectoryEntry->d_name);
  IsParentDir=(strcmp(DirectoryEntry->d_name, "..") == 0);

  Marked=Statebag->GetSelectedFiles()->Contains(Filename);
  UpdateText();
}

cOsdItemFileEntry::~cOsdItemFileEntry()
{
  if(Filename) free(Filename);
  if(Name)     free(Name);
}

eOSState cOsdItemFileEntry::ProcessKey(eKeys Key)
{
  eOSState state=cOsdItem::ProcessKey(Key);
  return state;

}

void cOsdItemFileEntry::Mark(bool Marked)
{
  this->Marked=Marked;
  UpdateText();
}

/*
cOsdMenuFilebrowser
*/

cOsdMenuFilebrowser::cOsdMenuFilebrowser(char* Directory, cFilebrowserStatebag* Statebag) : cOsdMenu(tr("Filebrowser"), 10)
{
  this->Statebag=Statebag;

  DIR* BaseDir;
  if(Directory && (BaseDir=opendir(Directory)))
  {
    BaseDirectory=strdup(Directory);
    closedir(BaseDir);
  }
  else
  {
    BaseDirectory=strdup("/");
  }
  
  Task=taskBrowse;

  DestinationContainerCommand=NULL;

  

  LoadDir(*(Statebag->CurrentDirectory) ? Statebag->CurrentDirectory : cString(BaseDirectory));
  UpdateHelp();
}

cOsdMenuFilebrowser::~cOsdMenuFilebrowser()
{
  if(DestinationContainerCommand) delete DestinationContainerCommand;
  if(BaseDirectory) delete BaseDirectory;
}

int cOsdMenuFilebrowser::DirectorySort(const void* File1, const void* File2)
{
  struct dirent64* ent1=*(struct dirent64**)File1;
  struct dirent64* ent2=*(struct dirent64**)File2;

  if(strcmp(ent1->d_name, "..") == 0)
  {
    return -1;
  }

  if(strcmp(ent2->d_name, "..") == 0)
  {
    return 1;
  }

  struct stat64 buf1, buf2;
  if(stat64(ent1->d_name, &buf1)==0 && stat64(ent2->d_name, &buf2)==0)
  {
    if(((buf1.st_mode & S_IFMT) != (buf2.st_mode & S_IFMT)) && (S_ISDIR(buf1.st_mode) || S_ISDIR(buf2.st_mode)))
    {
      return S_ISDIR(buf1.st_mode) ? -1 : 1;
    }
    else
    {
      return strcoll(ent1->d_name, ent2->d_name);
    }
  }
  D(fprintf(stderr, "stat failed for %s or %s\n", ent1->d_name, ent2->d_name));
  /*
  if(ent1->d_type!=ent2->d_type && (S_ISDIR(DTTOIF(ent1->d_type)) || S_ISDIR(DTTOIF(ent2->d_type))))
  {
    return S_ISDIR(DTTOIF(ent1->d_type)) ? -1 : 1;
  }
  */
  return strcoll(ent1->d_name, ent2->d_name);
}

void cOsdMenuFilebrowser::LoadDir(cString Directory)
{
  D(fprintf(stderr, "Current dir is %s, loading %s\n", *(Statebag->CurrentDirectory), *Directory));
  
  char* CurrentFile=NULL;
  if(!*(Statebag->CurrentDirectory) || strcmp(Directory, "..") != 0)
  {
    Statebag->CurrentDirectory=Directory;
    if(Current() >= 0)
    {
      Statebag->CurrentFiles->Add(new cStringContainer(strdup(((cOsdItemFileEntry*)Get(Current()))->GetFilename())));
    }
  }
  else
  {
    if(*(Statebag->CurrentDirectory))
    {
      char* slash=strrchr(*(Statebag->CurrentDirectory), '/');
      if(slash)
      {
        *(slash + (slash - *(Statebag->CurrentDirectory) > 0 ? 0 : 1))='\0';
        CurrentFile=Statebag->CurrentFiles->Last() ? strdup(Statebag->CurrentFiles->Last()->GetObject()) : NULL;
        if(Statebag->CurrentFiles->Last())
        {
          Statebag->CurrentFiles->Del(Statebag->CurrentFiles->Last());
        }
      }
    }
  }

  D(fprintf(stderr, "Current file is %s\n", CurrentFile ? CurrentFile : NULL));
  
  Refresh(CurrentFile);
  
  if(CurrentFile) free(CurrentFile);
}

bool cOsdMenuFilebrowser::MatchesFilter(dirent64* Entry)
{
  if(!Statebag->ShowHiddenFiles && Entry->d_name[0]=='.' && strcmp(Entry->d_name, "..")!=0)
  {
    return false;
  }
  
  return Entry->d_type==DT_DIR || !*Statebag->Filter || fnmatch(*Statebag->Filter, Entry->d_name, FNM_FILE_NAME | FNM_EXTMATCH) != FNM_NOMATCH;
}

void cOsdMenuFilebrowser::Refresh(const char* CurrentFile)
{
  if(Task==taskBrowse)
  {
    char* Title=(char*)malloc(strlen(tr("Filebrowser")) + strlen(Statebag->CurrentDirectory) + 3);
    char* Title_tmp=NULL;
    sprintf(Title, "%s: %s", tr("Filebrowser"), ((Title_tmp=strrchr(Statebag->CurrentDirectory, '/')) && !(Statebag->ShowFullPath)) ? Title_tmp + 1 : *(Statebag->CurrentDirectory));
    SetTitle(Title);
    free(Title);
  }
  else if(Task==taskSelectDestination)
  {
    SetTitle(tr("Select Destination"));
  }

  Clear();

  D(fprintf(stderr, "Refreshing, current dir is %s and i got %s\n", *(Statebag->CurrentDirectory), CurrentFile));

  char* cwd=getcwd(NULL, 0);
  dirent64** entries;
  int count;
  if(chdir(Statebag->CurrentDirectory) == 0 && (count=scandir64(Statebag->CurrentDirectory, &entries, NULL, &cOsdMenuFilebrowser::DirectorySort)) > 0)
  {
    
    
    for(int i=0; i<count; i++)
    {
      if(strcmp(entries[i]->d_name, ".")==0 || (strcmp(entries[i]->d_name, "..")==0 && strcmp(Statebag->BaseDir, Statebag->CurrentDirectory)==0))
      {
        continue;
      }
      if(MatchesFilter(entries[i]))
      {
        cOsdItemFileEntry* FileEntry=new cOsdItemFileEntry(entries[i], *(Statebag->CurrentDirectory), Statebag);
        Add(FileEntry);
        if(CurrentFile && strcmp(CurrentFile, FileEntry->GetFilename()) == 0)
        {
          SetCurrent(FileEntry);
        }
      }
    }
    free(entries);
    chdir(cwd);
  }
  else
  {
    Skins.Message(mtError, tr("Failed to load directory"));
  }

  if(cwd) free(cwd);

  if(Count() == 0)
  {
    /* Update Commands */
    UpdateCommands(NULL, false, false);
    UpdateHelp();
  }
  
  Display();
}

eOSState cOsdMenuFilebrowser::ProcessKey(eKeys Key)
{
  eOSState state=cOsdMenu::ProcessKey(Key);

  cFilebrowserCommand* ExecCommand=NULL;

  if(state == (eOSState)osFilebrowserCloseRefresh)
  {
    /* A submenu of this menu was closed - refresh files */
    if(HasSubMenu())
    {
      CloseSubMenu();
    }
    char* CurrentFile=CurrentFilename() ? strdup(CurrentFilename()) : NULL;
    Refresh(CurrentFile);
    if(CurrentFile) free(CurrentFile);
  }
  else if(state == osUnknown || state == osBack)
  {
    state=osContinue;
    switch(Key)
    {
      case kRed:
        if(LeftVisibleCommand)
        {
          if(LeftVisibleCommand->Prev()) // this is the "<<"-key
          {
            if(LeftVisibleCommand->Prev()->Prev()->Prev()->Prev())
            {
              LeftVisibleCommand=LeftVisibleCommand->Prev()->Prev();
            }
            else
            {
              LeftVisibleCommand=LeftVisibleCommand->Prev()->Prev()->Prev();
            }
            UpdateHelp();
          }
          else
          {
            ExecCommand=LeftVisibleCommand->GetObject();
          }
        }
      break;
      case kGreen:
        if(LeftVisibleCommand)
        {
          cFilebrowserCommandContainer* Container=LeftVisibleCommand->Prev() ? LeftVisibleCommand : LeftVisibleCommand->Next();
          ExecCommand=Container ? Container->GetObject() : NULL;
        }
      break;
      case kYellow:
        if(LeftVisibleCommand && LeftVisibleCommand->Next())
        {
          cFilebrowserCommandContainer* Container=LeftVisibleCommand->Prev() ? LeftVisibleCommand->Next() : LeftVisibleCommand->Next()->Next();
          ExecCommand=Container ? Container->GetObject() : NULL;
        }
      break;
      case kBlue:
        if(LeftVisibleCommand && LeftVisibleCommand->Next() && LeftVisibleCommand->Next()->Next())
        {
          cFilebrowserCommandContainer* Right;
          Right=LeftVisibleCommand->Prev() ? LeftVisibleCommand->Next()->Next() : LeftVisibleCommand->Next()->Next()->Next();
          if(Right)
          {
            if(Right->Next()) //this is the ">>"-Key
            {
              LeftVisibleCommand=Right;
              UpdateHelp();
            }
            else
            {
              ExecCommand=Right->GetObject();
            }
          }
        }
      break;
      case kOk:
        cOsdItemFileEntry* CurrentEntry;
        CurrentEntry=(cOsdItemFileEntry*)this->Get(this->Current());
        if(CurrentEntry && CurrentEntry->IsDir())
        {
          if(!CurrentEntry->IsParentDirectory())
          {
            LoadDir(cString(CurrentEntry->GetFilename()));
          }
          else
          {
            LoadDir(cString(".."));
          }
        }
#ifdef FILEBROWSER_PLUGIN_BUILD
        else
        {
          for(cFilebrowserCommandContainer* i=ActualCommands->First(); i; i=i->Next())
          {
            if(dynamic_cast<cFilebrowserCommandOtherCommands*>(i->GetObject()))
            {
              ExecCommand=i->GetObject();
              break;
            }
          }
        }
#endif
      break;
      case kBack:
        if(Statebag->CurrentFiles->Count() > 0)
        {
          LoadDir(cString(".."));
        }
        else
        {
          state=osBack;
        }
      break;
      case kNone:
#ifdef FILEBROWSER_PLUGIN_BUILD
        if(Statebag->UpdateRequested)
        {
          Statebag->UpdateRequested=false;
          Refresh(*cString(CurrentFilename()));
        }
#endif
      break;
      default:
        state=osUnknown;
      break;
    }
  }
  else if(state==(eOSState)osFilebrowserCloseExec)
  {
    if(ActualCommands->Count()>0)
    {
      ExecCommand=ActualCommands->Get(0)->GetObject();
      D(fprintf(stderr, "Preparing for execution: %s\n", ExecCommand->GetName()));
    }
    CloseSubMenu();
  }
  else if(state==(eOSState)osFilebrowserAccessCodeOk)
  {
    if(ActualCommands->Count()>0)
    {
      ExecCommand=ActualCommands->Get(0)->GetObject();
      D(fprintf(stderr, "Preparing for execution: %s\n", ExecCommand->GetName()));
    }
    CloseSubMenu();
  }

  if(ExecCommand)
  {
    char* CurrentFile=CurrentFilename();
    
    if(ExecCommand->UsesSelectedFiles() && Statebag->GetSelectedFiles()->Count()==0)
    {
      Statebag->GetSelectedFiles()->Add(new cStringContainer(strdup(CurrentFile)));
    }
    
    if(ExecCommand->UsesDestination() && Task!=taskSelectDestination && Task!=taskRequireAccessCode)
    {
      Task=taskSelectDestination;
      SetTitle(tr("Select Destination"));
      DestinationContainerCommand=new cFilebrowserDestinationContainerCommand(ExecCommand, CurrentFile, Statebag);
      UpdateHelp();
      Display();
      state=ExecCommand->GetState();
    }
    else if (ExecCommand->GetAccessCode() && Task!=taskRequireAccessCode)
    {
      Task=taskRequireAccessCode;
      ActualCommands->Clear();
      ActualCommands->Add(new cFilebrowserCommandContainer(ExecCommand));
      AddSubMenu(new cOsdMenuAccessCode(ExecCommand->GetAccessCode()));
    }
    else
    {
      bool Execute=true;
      D(fprintf(stderr, "confirmation needed: %s\n", ExecCommand->NeedsConfirmation() ? "yes" : "no"));
      if(ExecCommand->NeedsConfirmation())
      {
        char* Text=(char*)malloc(strlen(ExecCommand->GetName()) + 2);
        sprintf(Text, "%s?", ExecCommand->GetName());
        Execute=Interface->Confirm(Text);
        free(Text);
      }
      if(Execute)
      {
#ifdef FILEBROWSER_PLUGIN_BUILD
        int ThreadCount=Statebag->GetThreads()->Count();
#endif
        ExecCommand->Execute(this, NULL, CurrentFile);
#ifdef FILEBROWSER_PLUGIN_BUILD
        for(; ThreadCount < Statebag->GetThreads()->Count() && !ExecCommand->IsSynchronous(); ThreadCount++)
        {
          cCommandThread* Thread=Statebag->GetThreads()->Get(ThreadCount)->GetObject();
          if(ExecCommand->ShowMenu())
          {
            AddSubMenu(Thread->GetMenu());
          }
          Thread->Start();
        }
#endif
        Task=ExecCommand->Task();
        if(ExecCommand->UsesSelectedFiles())
        {
          Statebag->GetSelectedFiles()->Clear();
          for(cOsdItem* i=First(); i!=NULL; i=cOsdMenu::Next(i))
          {
            ((cOsdItemFileEntry*)i)->Mark(false);
          }
        }
        if(!(ExecCommand->ShowMenu()) || ExecCommand->IsSynchronous())
        {
          char* CurrentFile=strdup(CurrentFilename());
          Refresh(CurrentFile);
          if(CurrentFile) free(CurrentFile);
        }

	      state=ExecCommand->GetState();
      }
    }
  }
  
  return state;
}

void cOsdMenuFilebrowser::OsdCurrentItem(const char *Text)
{
  if(HasSubMenu()) /* Do nothing if this is not top most menu */
  {
    return;
  }
  
  for(cOsdItem* i=First(); i!=NULL; i=cOsdMenu::Next(i))
  {
    if(!Text || strcmp(i->Text(), Text) == 0)
    {
      cOsdItemNewFileEntry* NewFileEntry=dynamic_cast<cOsdItemNewFileEntry*>(i);
      
      UpdateCommands(((cOsdItemFileEntry*)i)->GetFilename(), NewFileEntry, NewFileEntry ? NewFileEntry->InEditMode() : false);
      
      break;
    }    
  }
}

void cOsdMenuFilebrowser::UpdateCommands(const char* Filename, bool IsVirtualNewFile, bool NewFileInEditMode)
{
  cFilebrowserCommands* Commands;

  if(Task==taskSelectDestination)
  {
    Commands=new cFilebrowserCommands();
    Commands->Add(new cFilebrowserCommandContainer(DestinationContainerCommand));
    Commands->Add(new cFilebrowserCommandContainer(new cFilebrowserDestinationAbortCommand(Statebag)));
    if(DestinationContainerCommand->UsesNewDestination() && !IsVirtualNewFile)
    {
      Commands->Add(new cFilebrowserCommandContainer(new cFilebrowserDestinationNewCommand(Statebag)));
    }
  }
  else
  {
    Commands=Statebag->GetCommands();
  }
      
  ActualCommands=new cFilebrowserCommands();
  for(cFilebrowserCommandContainer* i=Commands->First(); i; i=Commands->Next(i))
  {
    if(i->GetObject()->Matches(Filename))
    {
      ActualCommands->Add(new cFilebrowserCommandContainer(i->GetObject(), false));
    }
  }

  LeftVisibleCommand=ActualCommands->First();

  if(!(Task==taskSelectDestination && NewFileInEditMode))
  {
    UpdateHelp();
  }
}

void cOsdMenuFilebrowser::UpdateHelp()
{
  char* Red=NULL;
  char* Green=NULL;
  char* Yellow=NULL;
  char* Blue=NULL;

  int HelpPos=0;

  for(cFilebrowserCommandContainer* i = ActualCommands ? LeftVisibleCommand : NULL; i; i=ActualCommands->Next(i))
  {
    switch(HelpPos)
    {
      case 0:
        if(i->Prev())
        {
          Red="<<";
          i=ActualCommands->Prev(i);
        }
        else
        {
          Red=i->GetObject()->GetName();
        }
      break;
      case 1:
        Green=i->GetObject()->GetName();
      break;
      case 2:
        Yellow=i->GetObject()->GetName();
      break;
      case 3:
        if(i->Next())
        {
          Blue=">>";
        }
        else
        {
          Blue=i->GetObject()->GetName();
        };
      break;
    }
    HelpPos++;
    if(HelpPos > 3)
    {
      break;
    }
  }
  SetHelp(Red, Green, Yellow, Blue);
}

char* cOsdMenuFilebrowser::CurrentFilename()
{
  if(Current() < 0)
  {
    return NULL;
  }
  
  if(dynamic_cast<cOsdItemFileEntry*>(Get(Current())))
  {
    return ((cOsdItemFileEntry*)Get(Current()))->GetFilename();
  }
  else if(dynamic_cast<cOsdItemNewFileEntry*>(Get(Current())))
  {
    return ((cOsdItemNewFileEntry*)Get(Current()))->GetFilename();
  }

  return NULL;
}

/*
cOsdItemNewFileEntry
*/

cOsdItemNewFileEntry::cOsdItemNewFileEntry(char* Value, cString Directory) : cMenuEditStrItem(tr("New"), Name=(char*)malloc(NAME_MAX), NAME_MAX, FileNameChars)
{
  Filename=NULL;

  strcpy(Name, Value);
  
  this->Directory=strdup(*Directory);

  ProcessKey(kLeft);
}

cOsdItemNewFileEntry::~cOsdItemNewFileEntry()
{
  if(Directory) free(Directory);
  if(Name)      free(Name);
  if(Filename)  free(Filename);
}

bool cOsdItemNewFileEntry::InEditMode()
{
  return cMenuEditStrItem::InEditMode();
}

char* cOsdItemNewFileEntry::GetFilename()
{
  if(Filename) free(Filename);
  Filename=(char*)malloc(strlen(Directory) + strlen(Name) + 2);
  sprintf(Filename, "%s/%s", Directory, Name);
  return Filename;
}

eOSState cOsdItemNewFileEntry::ProcessKey(eKeys Key)
{
  if(InEditMode() || Key==kLeft || Key==kRight || Key==kNone)
  {
    return cMenuEditStrItem::ProcessKey(Key);
  }
  else
  {
    if(Key==kRed || Key==kYellow || Key==kBlue || Key==kGreen)
    {
      return osUnknown;
    }
    else
    {
      return osContinue;
    }
  }
}
