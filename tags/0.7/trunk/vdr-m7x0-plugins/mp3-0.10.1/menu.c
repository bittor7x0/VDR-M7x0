/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2007 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "menu.h"
#include "data.h"
#include "data-src.h"
#include "i18n.h"

// -----------------------------------------------------------------------------

void Status(const char *text)
{
#if APIVERSNUM >= 10307
  Skins.Message(mtStatus,text);
#else
  if(text) {
    Interface->Status(text);
    Interface->Flush();
    }
#endif
}

void Error(const char *text)
{
#if APIVERSNUM >= 10307
  Skins.Message(mtError,text);
#else
  Interface->Error(text);
#endif
}

void Info(const char *text)
{
#if APIVERSNUM >= 10307
  Skins.Message(mtInfo,text);
#else
  Interface->Info(text);
#endif
}

// --- cMenuBrowseItem ---------------------------------------------------------

class cMenuBrowseItem : public cOsdItem {
  private:
  cFileObj *item;
  virtual void Set(void);
public:
  cMenuBrowseItem(cFileObj *Item);
  cFileObj *Item(void) { return item; }
  };

cMenuBrowseItem::cMenuBrowseItem(cFileObj *Item)
{
  item = Item;
  Set();
}

void cMenuBrowseItem::Set(void)
{
  char *buffer=0;
  asprintf(&buffer,item->Type()==otFile?"%s":"[%s]",item->Name());
  SetText(buffer,false);
}

// --- cMenuBrowse ------------------------------------------------------

cFileObj *cMenuBrowse::lastselect=0;

cMenuBrowse::cMenuBrowse(cFileSource *Source, bool Dirselect, bool WithID3, const char *title, const char * const *Excl)
:cOsdMenu(title)
{
  currentdir=parent=0;
  delete lastselect; lastselect=0;
  list=new cDirList;

  dirselectable=Dirselect;
  withID3=WithID3;
  excl=Excl;

  SetSource(Source); 
  NewDir(currentdir);
}

cMenuBrowse::~cMenuBrowse()
{
  free(parent);
  free(currentdir);
  delete list;
}

cFileObj *cMenuBrowse::CurrentItem(void)
{
  cMenuBrowseItem *item = (cMenuBrowseItem *)Get(Current());
  return item ? item->Item():0;
}

void cMenuBrowse::SetButtons(void)
{
  SetHelp(tr("Select"), currentdir?tr("Parent"):0, 0, withID3?tr("ID3 info"):0);
  Display();
}

void cMenuBrowse::SetSource(cFileSource *Source)
{
  source=Source;
  free(currentdir); currentdir=0;
  free(parent); parent=0;
  source->GetRemember(currentdir,parent);
}

bool cMenuBrowse::LoadDir(const char *dir)
{
  Clear();
  Status(tr("Scanning directory..."));
  bool res=list->Load(source,dir,excl);
  if(res) {
    cFileObj *item=list->First();
    while(item) {
      Add(new cMenuBrowseItem(item),(parent && !strcmp(item->Name(),parent)));
      item=list->Next(item);
      }
    }
  Status(0);
  return res;
}

bool cMenuBrowse::NewDir(const char *dir)
{
  char *ncur=dir ? strdup(dir):0;
  bool r=LoadDir(ncur);
  if(!r && ncur) {
    free(ncur); ncur=0;
    r=LoadDir(ncur);
    }
  if(r) {
    free(currentdir); currentdir=ncur;

    cFileObj *item=CurrentItem();
    source->SetRemember(currentdir,item?item->Name():0);

    SetButtons();
    return true;
    }
  free(ncur);
  Error(tr("Error scanning directory!"));
  return false;
}

eOSState cMenuBrowse::Parent(void)
{
  eOSState res=osContinue;

  if(currentdir) {
    free(parent);
    char *ss=strrchr(currentdir,'/');
    if(ss) {
      *ss++=0;
      parent=strdup(ss);
      ss=currentdir;
      }
    else parent=strdup(currentdir);

    if(!NewDir(ss)) res=osEnd;
    }
  return res;
}

eOSState cMenuBrowse::Select(bool isred)
{
  eOSState res=osContinue;
  cFileObj *item;

  if((item=CurrentItem())) {
    switch(item->Type()) {
      case otParent:
        if(!isred || !dirselectable)
          res=Parent();
        break;
      case otDir:
        if(!isred || !dirselectable) {
          if(!NewDir(item->Path())) res=osEnd;
          break;
          }
        // fall through to otFile
      case otFile:
        lastselect=new cFileObj(item);
        res=osBack;
        break;
      default:
        break;
      }
    } 
  return res;
}

eOSState cMenuBrowse::ID3Info(void)
{
  return osContinue;
}

eOSState cMenuBrowse::ProcessStdKey(eKeys Key, eOSState state)
{
  if(state==osUnknown) {
    switch (Key) {
      case kOk:     state=Select(false); break;
      case kRed:    state=Select(true); break;
      case kGreen:  state=Parent(); break;
      case kBlue:   if(withID3) state=ID3Info();
                    break;
      //case kMenu:   state=osEnd; break;
      default: break;
      }
    }
  if(state==osEnd || state==osBack) {
    cFileObj *item=CurrentItem();
    if(item) source->SetRemember(currentdir,item->Name());
    }
  return state;
}

// --- cMenuSourceItem ----------------------------------------------------------

class cMenuSourceItem : public cOsdItem {
  private:
  cFileSource *source;
  virtual void Set(void);
public:
  cMenuSourceItem(cFileSource *Source);
  cFileSource *Source(void) { return source; }
  };

cMenuSourceItem::cMenuSourceItem(cFileSource *Source)
{
  source=Source;
  Set();
}

void cMenuSourceItem::Set(void)
{
  char *buffer=0;
  asprintf(&buffer, "%s\t%s\t%s", source->NeedsMount()?(source->Status()?"*":">"):"", source->Description(), source->BaseDir());
  SetText(buffer,false);
}

// --- cMenuSource --------------------------------------------------

cFileSource *cMenuSource::selected=0;

cMenuSource::cMenuSource(cFileSources *Sources, const char *title)
:cOsdMenu(title,2,20)
{
  selected=0;
  current=Sources->GetSource();
  cFileSource *source=Sources->First();
  while(source) {
    cOsdMenu::Add(new cMenuSourceItem(source),source==current);
    source=Sources->Next(source);
    }

  SetHelp(tr("Select"), tr("Mount"), tr("Unmount"), tr("Eject"));
  Display();
}

bool cMenuSource::DoMount(cFileSource *src)
{
  bool res=src->Mount();
  RefreshCurrent();
  DisplayCurrent(true);
  return res;
}

bool cMenuSource::CheckMount(void)
{
  cFileSource *src=selected ? selected:current;
  if(src->NeedsMount() && !src->Status()) {
    Error(tr("Selected source is not mounted!"));
    return false;
    }
  return true;
}

eOSState cMenuSource::Select(void)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  cFileSource *src = ((cMenuSourceItem *)Get(Current()))->Source();
  if(src->NeedsMount() && !src->Status()) {
    if(!DoMount(src)) Error(tr("Mount failed!"));
    }
  if(!src->NeedsMount() || src->Status()) {
    selected=src;
    return osBack;
    }
  return osContinue;
}

eOSState cMenuSource::Mount(void)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  cFileSource *src = ((cMenuSourceItem *)Get(Current()))->Source();
  if(src->NeedsMount() && !src->Status()) {
    if(DoMount(src)) Info(tr("Mount succeeded"));
    else Error(tr("Mount failed!"));
    }
  return osContinue;
}

eOSState cMenuSource::Unmount(void)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  cFileSource *src = ((cMenuSourceItem *)Get(Current()))->Source();
  if(src->NeedsMount() && src->Status()) {
    bool res=src->Unmount();
    RefreshCurrent();
    DisplayCurrent(true);
    if(res) Info(tr("Unmount succeeded"));
    else Error(tr("Unmount failed!"));
    }
  return osContinue;
}

eOSState cMenuSource::Eject(void)
{
  if(HasSubMenu() || Count() == 0) return osContinue;

  cFileSource *src = ((cMenuSourceItem *)Get(Current()))->Source();
  if(src->NeedsMount()) {
    bool res=src->Eject();
    RefreshCurrent();
    DisplayCurrent(true);
    if(!res) Error(tr("Eject failed!"));
    }
  return osContinue;
}

eOSState cMenuSource::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if(state==osBack && !CheckMount()) state=osContinue;
  if(state==osUnknown) {
     switch(Key) {
       case kOk:     
       case kRed:    return Select();
       case kGreen:  return Mount();
       case kYellow: return Unmount();
       case kBlue:   return Eject();
       case kMenu:   CheckMount(); return osEnd;
       default: break;
       }
     }
  return state;
}

// --- cProgressBar ------------------------------------------------------------

cProgressBar::cProgressBar(int Width, int Height, int Current, int Total)
:cBitmap(Width, Height, 2)
{
  if(Total > 0) {
    int p = Current * Width / Total;;
#if APIVERSNUM >= 10307
    DrawRectangle(0, 0, p, Height - 1, clrGreen);
    DrawRectangle(p + 1, 0, Width - 1, Height - 1, clrWhite);
#else
    Fill(0, 0, p, Height - 1, clrGreen);
    Fill(p + 1, 0, Width - 1, Height - 1, clrWhite);
#endif
    }
}

