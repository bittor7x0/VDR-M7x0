/***************************************************************************
                          menu-threads.c  -  description
                             -------------------
    begin                : Do Jan 27 2005
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

#include <vdr/interface.h>
#include "menu-filebrowser.h"
#include "menu-threads.h"
#include "tools.h"
#include "threads.h"

cOsdMenuThreadList::cOsdMenuThreadList(cFilebrowserStatebag* Statebag) : cOsdMenu(tr("Running threads"))
{
  this->Statebag=Statebag;
  cThreadContainer* i=Statebag->GetThreads()->First();
  while(i)
  {
    D(fprintf(stderr, "Adding Thread %s (Remove %s)\n", i->GetObject()->GetDescription(), i->GetObject()->IsRemoveRequested() ? "true" : "false"));
    if(i->GetObject()->IsRemoveRequested())
    {
      cThreadContainer* tmp=i;
      i=Statebag->GetThreads()->Next(i);
      Statebag->GetThreads()->Del(tmp);
      continue;
    }
    cOsdItem* Item=new cOsdItem("");
    UpdateMenuItem(Item, i->GetObject());
    Add(Item);
    i=Statebag->GetThreads()->Next(i);
  }      
}

eOSState cOsdMenuThreadList::ProcessKey(eKeys Key)
{
  eOSState retval=cOsdMenu::ProcessKey(Key);

  if(retval==(eOSState)osFilebrowserCloseRefresh)
  {
    CloseSubMenu();
    retval=osContinue;
  }
  if(retval!=osUnknown)
  {
    return retval;
  }
  
  switch(Key)
  {
    case kNone:
      cOsdItem* Item;
      Item=First();
      for(cThreadContainer* i=Statebag->GetThreads()->First(); i && Item; i=Statebag->GetThreads()->Next(i))
      {
        UpdateMenuItem(Item, i->GetObject());
        Item=(cOsdItem*)Item->Next();
      }
      Display();
    break;
    case kOk:
      if(Current()<0)
      {
        return osBack;
      }
      AddSubMenu(Statebag->GetThreads()->Get(Current())->GetObject()->GetMenu());
      retval=osContinue;
    break;
    case kBack:
      return (eOSState)osFilebrowserCloseRefresh;
    break;
    case kYellow:
      if(Current()>=0)
      {
        cCommandThread* Thread=Statebag->GetThreads()->Get(Current())->GetObject();
        switch(Thread->GetState())
        {
          case tsPaused:
            Thread->Resume();
            break;
          case tsRunning:
            Thread->Pause();
            break;
          default:
            //do nothing
            break;
        }
      }
      retval=osContinue;
    break;
    case kBlue:
      if(Current()>=0)
      {
        cCommandThread* Thread=Statebag->GetThreads()->Get(Current())->GetObject();
        if(Thread->Active())
        {
          if(Interface->Confirm(tr("Kill?")))
          {
            Thread->Cancel();
            UpdateMenuItem(Get(Current()), Thread);
          }
        }
        else
        {
          Statebag->GetThreads()->Del(Statebag->GetThreads()->Get(Current()));
          Del(Current());
        }
        UpdateHelp();
        Display();
      }
      retval=osContinue;
    default:
    break;
  }
  return retval;  
}

void cOsdMenuThreadList::OsdCurrentItem(const char *Text)
{
  if(HasSubMenu())
  {
    return;
  }
  UpdateHelp();
}

void cOsdMenuThreadList::UpdateHelp()
{
  if(Current()>=0 && Count() > 0)
  {
    cCommandThread* Thread=Statebag->GetThreads()->Get(Current())->GetObject();
    SetHelp("", "", Thread->GetState()==tsPaused ? tr("Resume") : (Thread->GetState()==tsRunning ? tr("Pause") : ""),
            Thread->Active() ? tr("Kill") : tr("Remove"));
  }
  else
  {
    SetHelp("", "", "", "");
  }
}

void cOsdMenuThreadList::UpdateMenuItem(cOsdItem* Item, cCommandThread* Thread)
{
  char* Text=(char*)malloc(strlen(Thread->GetDescription()) + strlen(tr("Active")) + strlen(tr("Finished")) + strlen(tr("Paused")) + 4);
  switch(Thread->GetState())
  {
    case tsRunning:
      sprintf(Text, "%s [%s]", Thread->GetDescription(), tr("Running"));
      break;
    case tsPaused:
      sprintf(Text, "%s [%s]", Thread->GetDescription(), tr("Paused"));
      break;
    default:
      sprintf(Text, "%s [%s]", Thread->GetDescription(), tr("Finished"));
      break;
  }
  Item->SetText(Text);
  free(Text);
}
