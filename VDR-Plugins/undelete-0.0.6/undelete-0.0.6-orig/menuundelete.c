/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menuundelete.c 0.6 2006/03/15 22:12:06 hflor Exp $
 */

#include "menuundelete.h"
#if VDRVERSNUM < 10307
  #include "menutext.h"
#endif
#include "undelete.h"
#include "menudispkey.h"
#include "i18n.h"
#include "vdrtools.h"
#include <vdr/interface.h>
#include <vdr/videodir.h>
#include <vdr/remote.h>
#ifdef HAVE_ICONPATCH
  #include <vdr/iconpatch.h>
#endif
#include <typeinfo>

// --- cMenuRecordingSelectItem --------------------------------------------------------

cMenuRecordingSelectItem::cMenuRecordingSelectItem(cRecording *Recording, int Level)
{
#ifdef UND_Debug
  objID = ++cMenuRecordingSelectItem_nextID;
#endif
  filename = strdup(Recording->FileName());
  totalEntries = newEntries = 0;
  start = Recording->start;
  SetText(Recording->Title('\t', true, Level));
  if ((isdir = (*Text() == '\t')))
    name = strdup(Text() + 2);
  else
    name = strdup(Recording->Name());
  level = Level;
  d2syslog("cMenuRecordingSelectItem::cMenuRecordingSelectItem", "FileName=%s Level=%d Name=%s", filename, level, name);
}

cMenuRecordingSelectItem::~cMenuRecordingSelectItem()
{
  d2syslog("cMenuRecordingSelectItem::~cMenuRecordingSelectItem", "FileName=%s Name=%s", filename, name);
  freenull(filename);
  freenull(name);
}

#if VDRVERSNUM >= 10315
int cMenuRecordingSelectItem::compare(const cListObject &ListObject) const
{
  cMenuRecordingSelectItem *temp = (cMenuRecordingSelectItem *)&ListObject;
  if (isdir != temp->isdir)
    return isdir ? -1 : 1;
  if (isdir)
    return strcasecmp(name, temp->name);
  return start == temp->start ? 0 : start < temp->start ? -1 : 1;
}
#else
bool cMenuRecordingSelectItem::operator< (const cListObject &ListObject)
{
  cMenuRecordingSelectItem *temp = (cMenuRecordingSelectItem *)&ListObject;
  if (isdir != temp->isdir)
    return isdir;
  if (isdir)
    return strcasecmp(name, temp->name) < 0;
  return start < temp->start;
}
#endif

void cMenuRecordingSelectItem::IncrementCounter(bool New)
{
  totalEntries++;
  if (New)
    newEntries++;
  char *buffer = NULL;
  asprintf(&buffer, "%d\t%d\t%s", totalEntries, newEntries, name);
  SetText(buffer, false);
  d2syslog("cMenuRecordingSelectItem::IncrementCounter", "FileName=%s New=%s Buffer=%s", filename, New ? "true" : "false", buffer);
}

void cMenuRecordingSelectItem::RefreshCounter(void)
{
  d2syslog("cMenuRecordingSelectItem::RefeshCounter", "FileName=%s", filename);
  if (isdir)
  {
    char *dirname = DirName();
    if (dirname)
    {
      totalEntries = 0;
      newEntries = 0;
      for (cRecording *recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording))
        if (!strncmp(recording->FileName(), dirname, strlen(dirname)))
          if (strchr(recording->FileName() + strlen(dirname), '/'))
            IncrementCounter(recording->IsNew());
      free(dirname);
    }
  }
}

char *cMenuRecordingSelectItem::DirName(bool Parent)
{
  char *temp = NULL;
  if (isdir || Parent)
  {
    temp = strdup(filename);
    char *last = temp + strlen(VideoDirectory) + 1;
    for (int i = level; *last && i >= Parent ? -1 : 0; i--)
    {
      last = strchr(last, '/');
      if (!last)
        break;
      last++;
    }
    if (last)
      *last = 0;
  }
  d2syslog("cMenuRecordingSelectItem::DirName", "Level=%d Parent=%s DirName=%s", level, Parent ? "yes" : "no", temp ? temp : "(NULL)");
  return temp;
}

// --- cMenuRecordingSelect --------------------------------------------------------

cMenuRecordingSelect::cMenuRecordingSelect(const char *Base, int Level)
#if VDRVERSNUM >= 10307
:cOsdMenu(Base ? Base : tr("Deleted Recordings"), 9, 6, 6)
#else
:cOsdMenu(Base ? Base : tr("Deleted Recordings"), 6, 6, 6)
#endif
{
#ifdef UND_Debug
  objID = ++cMenuRecordingSelect_nextID;
#endif
  d2syslog("cMenuRecordingSelect::cMenuRecordingSelect", "Base=%s Level=%d", Base, Level);
  STATUS(tr("Display$prepare recording display..."));
  base = Base ? strdup(Base) : NULL;
  level = Level;
  top_line[0] = top_line[1] = bottom_line[0] = bottom_line[1] = false;

  Display();
  cMenuRecordingSelectItem *LastItem = NULL;
  char *LastItemText = NULL;
  for (cRecording *recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording))
  {
    if (!Base || (strstr(recording->Name(), Base) == recording->Name() && recording->Name()[strlen(Base)] == '~'))
    {
      cMenuRecordingSelectItem *Item = new cMenuRecordingSelectItem(recording, level);
      if (*Item->Text() && (!LastItem || strcmp(Item->Text(), LastItemText)))
      {
        Add(Item);
        LastItem = Item;
        freenull(LastItemText);
        LastItemText = strdup(LastItem->Text());
      } else
        delete Item;
      if (LastItem)
      {
        if (LastItem->IsDirectory())
          LastItem->IncrementCounter(recording->IsNew());
      }
    }
  }
  freenull(LastItemText);
  Sort();
  AddFunctionLines();
  d1listlog("cMenuRecordingSelect::cMenuRecordingSelect");
  SetCurrent(First());
  SetHelpKeys();
}

cMenuRecordingSelect::~cMenuRecordingSelect()
{
  d2syslog("cMenuRecordingSelect::~cMenuRecordingSelect", "Base=%s Level=%d", base, level);
  freenull(base);
}

bool cMenuRecordingSelect::IsMenuRecordingSelectItem(cOsdItem *Item)
{
  if (Item)
  {
    d1syslog("cMenuRecordingSelect::IsMenuRecordingSelectItem", "Type(Item)=%s Text(Item)=%s", typeid(*Item).name(), Item->Text());
    return typeid(*Item) == typeid(cMenuRecordingSelectItem);
  }
  d1syslog("cMenuRecordingSelect::IsMenuRecordingSelectItem", "Item=%s", "(NULL)");
  return false;
}

bool cMenuRecordingSelect::IsDirectory(cOsdItem *Item)
{
  if (IsMenuRecordingSelectItem(Item))
    return ((cMenuRecordingSelectItem *)Item)->IsDirectory();
  return false;
}

bool cMenuRecordingSelect::IsRecording(cOsdItem *Item)
{
  if (IsMenuRecordingSelectItem(Item))
    return !((cMenuRecordingSelectItem *)Item)->IsDirectory();
  return false;
}

bool cMenuRecordingSelect::IsReady(void)
{
  if (WorkFilename)
     esyslog("%s: Error previous jobs not completed (%s)", plugin_name, WorkFilename);
  return !WorkFilename;
}

cRecording *cMenuRecordingSelect::GetRecording(cOsdItem *Item)
{
  if (IsRecording(Item))
  {
    cRecording *recording = DeletedRecordings.GetByName(((cMenuRecordingSelectItem *)Item)->FileName());
    if (!recording)
       ERROR(tr("Error while accessing recording!"));
    return recording;
  } else
    return NULL;
}

void cMenuRecordingSelect::AddFunctionLines(void)
{
  d1syslog("cMenuRecordingSelect::AddFunctionLines", "Count=%d purge=%s lines=%d salvage=%s lines=%d", Count(), FunctionLine[functionline_b_e[0].u], functionline_num[0].u, FunctionLine[functionline_b_e[1].u], functionline_num[1].u);
  for (int i = 0; i < 2; i++)
  {
    if (functionline_b_e[i].u != 0x0 && Count())
    {
      if (functionline_b_e[i].u & 0x1)
        top_line[i] = true;
      if ((functionline_b_e[i].u & 0x2) && (!top_line[i] || Count() >= functionline_num[i].u))
        bottom_line[i] = true;
    }
  }
  if (firstsalvage.u)
  {
    if (top_line[0])
      Ins(new cOsdItem(tr("Display$Purge all records ..."), osUser2));
    if (top_line[1])
      Ins(new cOsdItem(tr("Display$Salavge all records ..."), osUser3));
    if (bottom_line[0])
      Add(new cOsdItem(tr("Display$Purge all records ..."), osUser2));
    if (bottom_line[1])
      Add(new cOsdItem(tr("Display$Salavge all records ..."), osUser3));
  } else
  {
    if (top_line[1])
      Ins(new cOsdItem(tr("Display$Salavge all records ..."), osUser3));
    if (top_line[0])
      Ins(new cOsdItem(tr("Display$Purge all records ..."), osUser2));
    if (bottom_line[1])
      Add(new cOsdItem(tr("Display$Salavge all records ..."), osUser3));
    if (bottom_line[0])
      Add(new cOsdItem(tr("Display$Purge all records ..."), osUser2));
  }
  d1syslog("cMenuRecordingSelect::AddFunctionLines", "new top(purge)=%s top(salavge)=%s bottom(purge)=%s bottom(salvage)=%s", top_line[0] ? "yes" : "no", top_line[1] ? "yes" : "no", bottom_line[0] ? "yes" : "no", bottom_line[1] ? "yes" : "no");
}

void cMenuRecordingSelect::DelFunctionLines(void)
{
  if (top_line[0] || top_line[1])
    Del(0);
  if (top_line[0] && top_line[1])
    Del(0);
  if (bottom_line[0] || bottom_line[1])
    Del(Count() - 1);
  if (bottom_line[0] && bottom_line[1])
    Del(Count() - 1);
  top_line[0] = top_line[1] = bottom_line[0] = bottom_line[1] = false;
}

void cMenuRecordingSelect::SetHelpKeys(void)
{
  const char *helptext[4] = { NULL, NULL, NULL, NULL };
  cOsdItem *item = Get(Current());
  
  for (int i = 2; i < MaxFunctionName; i++)
  {
    bool functionavailable = false;
    switch (i)
    {
      case 2:
      case 3:   functionavailable = IsMenuRecordingSelectItem(item);
                break;
      case 4:
      case 5:   functionavailable = Count();
                break;
      case 6:   functionavailable = IsDirectory(item);
                break;
      case 7:   if (IsRecording(item))
                {
                  cRecording *recording = GetRecording(item);
#if VDRVERSNUM >= 10325
                  functionavailable = (recording && recording->Info() && recording->Info()->Description() && *recording->Info()->Description());
#else 
                  functionavailable = (recording && recording->Summary() && *recording->Summary());
#endif
                }
                break;
      case 8:   functionavailable = KeyState != 1 || functionkey[8].u != functionkey[9].u;
                break;
      case 9:   functionavailable = KeyState != 2 || functionkey[8].u != functionkey[9].u;
                break;
      case 10:  functionavailable = true;
                break;
    }
    if (functionavailable)
      switch (functionkey[i].u)
      {
        case 1:
        case 3:
        case 5:
        case 7: if (KeyState == 1)
                  helptext[functionkey[i].u >> 1] = FunctionHotKey[i];
                break;  
        case 2:
        case 4:
        case 6:
        case 8: if (KeyState == 2)
                  helptext[(functionkey[i].u - 1) >> 1] = FunctionHotKey[i];
                break;
      }
  }
  SetHelp(helptext[0], helptext[1], helptext[2], helptext[3]);
  d2syslog("cMenuRecordingSelect::SetHelpKeys", "KeyState=%d Item=%s red=%s green=%s yellow=%s blue=%s", KeyState, item ? item->Text() : "(NULL)", helptext[0] ? helptext[0] : "(NULL)", helptext[1] ? helptext[1] : "(NULL)", helptext[2] ? helptext[2] : "(NULL)", helptext[3] ? helptext[3] : "(NULL)");
}

eOSState cMenuRecordingSelect::FunctionCall(int FunctionNumber)
{
  eOSState state = osUnknown;
  cOsdItem *item = Get(Current());
  d2syslog("cMenuRecordingSelect::FunctionCall", "FunctionNumber=%d HasSubMenu=%s Item=%s", FunctionNumber, HasSubMenu() ? "yes" : "no", item ? item->Text() : "(NULL)");
  cMenuRecordingSelectItem *ri = NULL;
  cRecording *recording = NULL;
  if (!item || HasSubMenu())
    return osContinue;
  switch (FunctionNumber)
  {
    // open/summary
    case 1:   if (IsDirectory(item))
                state = FunctionCall(6);
              else if (IsRecording(item))
                state = FunctionCall(7);
              break;

    // purge
    case 2:   state = osContinue;
              if (IsMenuRecordingSelectItem(item) && IsReady())
              {
                ri = (cMenuRecordingSelectItem *)item;
                if ((!ri->IsDirectory() && (!confirmpurge.u || Interface->Confirm(tr("Question$Purge recording?")))) || (ri->IsDirectory() && (!confirmpurgedir.u || Interface->Confirm(tr("Question$Purge recordings in diretory?")))))
                {
                  cRemote::Put((eKeys)kPurgeRec);
                  if (ri->IsDirectory())
                    WorkFilename = ri->DirName(); // WorkFilename is NULL see IsReady()
                  else
                    WorkFilename = strdup(ri->FileName());
                }
              }
              break;

    // salvage
    case 3:   state = osContinue;
              if (IsMenuRecordingSelectItem(item) && IsReady())
              {
                ri = (cMenuRecordingSelectItem *)item;
                if ((!ri->IsDirectory() && (!confirmsalvage.u || Interface->Confirm(tr("Question$Salvage recording?")))) || (ri->IsDirectory() && (!confirmsalvagedir.u || Interface->Confirm(tr("Question$Salvage recordings in directory?")))))
                {
                  cRemote::Put((eKeys)kSalvageRec);
                  if (ri->IsDirectory())
                    WorkFilename = ri->DirName(); // WorkFilename is NULL see IsReady()
                  else
                    WorkFilename = strdup(ri->FileName());
                }
              }
              break;

    // purge all
    case 4:   state = osContinue;
              if (IsReady() && (!confirmpurgeall.u || Interface->Confirm(tr("Question$Purge all recordings?"))))
              {
                cRemote::Put((eKeys)kPurgeRec);
                for (item = First(); !WorkFilename && item; item = Next(item))
                  if (IsMenuRecordingSelectItem(item))
                    WorkFilename = ((cMenuRecordingSelectItem *)item)->DirName(true);
                Clear();
                top_line[0] = top_line[1] = bottom_line[0] = bottom_line[1] = false;
              }
              break;

    // salvage all
    case 5:   state = osContinue;
              if (IsReady() && (!confirmsalvageall.u || Interface->Confirm(tr("Question$Salvage all recordings?"))))
              {
                cRemote::Put((eKeys)kSalvageRec);
                for (item = First(); !WorkFilename && item; item = Next(item))
                  if (IsMenuRecordingSelectItem(item))
                    WorkFilename = ((cMenuRecordingSelectItem *)item)->DirName(true);
                Clear();
                top_line[0] = top_line[1] = bottom_line[0] = bottom_line[1] = false;
              }
              break;

    // open
    case 6:   if (IsDirectory(item))
              {
                ri = (cMenuRecordingSelectItem *)item;
                const char *t = ri->Name();
                char *buffer = NULL;
                if (base)
                {
                  asprintf(&buffer, "%s~%s", base, t);
                  t = buffer;
                }
                state = AddSubMenu(new cMenuRecordingSelect(t, level + 1));
                freenull(buffer);
              }
              break;

    // summary
    case 7:   recording = GetRecording(item);
#if VDRVERSNUM >= 10325
              if (recording && recording->Info() && recording->Info()->Description() && *recording->Info()->Description())
                state = AddSubMenu(new cMenuText(tr("Summary"), recording->Info()->Description()));
#else 
              if (recording && recording->Summary() && *recording->Summary())
                state = AddSubMenu(new cMenuText(tr("Summary"), recording->Summary()));
#endif
              break;
   
    // switch hotkeys to 1
    case 8:   if (functionkey[8].u == functionkey[9].u && KeyState == 1)
                KeyState = 2;
              else
                KeyState = 1;
              cRemote::Put((eKeys)kHelpKeys);
              state = osContinue;
              break;

    // switch hotkeys to 2
    case 9:   KeyState = 2;
              cRemote::Put((eKeys)kHelpKeys);
              state = osContinue;
              break;

    // Choise$Display keys
    case 10:  state = AddSubMenu(new cMenuDispKey());
              break;
  }
  d2syslog("cMenuRecordingSelect::FunctionCall", "FunctionNumber=%d returned OSState=%s WorkFilename=%s KeyState=%d", FunctionNumber, OSStateName(state), WorkFilename ? WorkFilename : "(NULL)", KeyState);
  return state;
}

eOSState cMenuRecordingSelect::ProcessKey(eKeys Key)
{
  bool hSubMenu = HasSubMenu();
  cOsdItem *item = NULL;
  int keynumber;

  d3ProcessKey1("cMenuRecordingSelect::ProcessKey");
  eOSState state = cOsdMenu::ProcessKey(Key);
  d3ProcessKey2("cMenuRecordingSelect::ProcessKey");

  if (hSubMenu && !HasSubMenu())
  {
    if (state == osContinue)
    {
      for (item = First(); item; item = Next(item))
        if (IsDirectory(item))
          ((cMenuRecordingSelectItem *)item)->RefreshCounter();
      Display();
    }
  }

  switch (state)
  {
    case osUnknown: keynumber = KeyState - 2; // KeyState is 1 or 2
                    switch (Key)
                    {
                      case kBlue:           keynumber += 2;  // no break!
                      case kYellow:         keynumber += 2;  // no break!
                      case kGreen:          keynumber += 2;  // no break!
                      case kRed:            keynumber += 2;  // keynumber red=1/2 green=3/4 yellow=5/6 blue=7/8
                                            break;
                      case k0...k9:         keynumber = Key - k0 + 9;
                                            break;
                      case kUser1...kUser9: keynumber = Key - kUser1 + 19;
                                            break;
                      case kOk:             state = FunctionCall(okkey.u);
                                            break;
                      case kDelLine:        
                                            if (Count())
                                              Del(Current());
                                            item = Get(Current());
                                            if (item && !IsMenuRecordingSelectItem(item))
                                            {
                                              item = Prev(item);
                                              if (!IsMenuRecordingSelectItem(item))
                                                item = NULL;
                                            }
                                            DelFunctionLines();
                                            AddFunctionLines();
                                            if (item)
                                              SetCurrent(item);
                                            if (Count())
                                            {
                                              Display();
                                              state = osContinue;
                                              cRemote::Put((eKeys)kHelpKeys);
                                            } else
                                            {
                                              state = osBack;
                                              cRemote::Put((eKeys)kDelLine);
                                            }
                                            break;
                      default:              break;
                    }
                    if (keynumber > 0)
                      for (int i = 0; i < MaxFunctionName && state == osUnknown; i++)
                        if (functionkey[i].u == keynumber)
                          state = FunctionCall(i);
                    if (state == osUnknown && Key >= k0 && Key <= k9)
                    {
                      item = Get((Key == k0 ? 10 : Key - k0) + (top_line[0] ? 1 : 0) + (top_line[1] ? 1 : 0) - 1);
                      if (IsMenuRecordingSelectItem(item))
                      {
                        SetCurrent(item);
                        Display();
                        state = FunctionCall(numkey.u);
                      }
                    }
                    break;
    case osUser2:   state = FunctionCall(4);
                    break;
    case osUser3:   state = FunctionCall(5);
                    break;
    default:        break;
  }
  if (!HasSubMenu() && Key != kNone && !WorkFilename)
  {
    SetHelpKeys();
    item = Get(Current());
    if (IsDirectory(item))
    {
      ((cMenuRecordingSelectItem *)item)->RefreshCounter();
      RefreshCurrent();
    }
  }

#ifdef UND_Debug1
  if (state != osUnknown && !HasSubMenu())
  {
    d1syslog("cMenuRecordingSelect::ProcessKey", "base=%s", base ? base : "(NULL)");
    d1listlog("cMenuRecordingSelect::ProcessKey");
  }
#endif
  d3ProcessKey3("cMenuRecordingSelect::ProcessKey");
  return state;
}

// --- cMenuUndelete --------------------------------------------------------

cMenuUndelete::cMenuUndelete(void)
:cOsdMenu(tr("Deleted Recordings"), 6, 6)
{
#ifdef UND_Debug
  objID = ++cMenuUndelete_nextID;
#endif
  d2syslog("cMenuUndelete::cMenuUndelete", "%s", "");
  MenuIsOpen = true;
  PurgeRecording = false;
  SalvageRecording = false;
  FillMenu();
}

cMenuUndelete::~cMenuUndelete()
{
  d2syslog("cMenuUndelete::~cMenuUndelete", "%s", "");
  if (PurgeRecording && !oRemoveThread.ActiveWithCancel())
    oRemoveThread.Start();
  DeletedRecordings.Clear();
#if VDRVERSNUM >= 10311
  if (SalvageRecording)
#if VDRVERSNUM >= 10333
    Recordings.Update();
#else
    Recordings.TriggerUpdate();
#endif
#endif
  MenuIsOpen = false;
}

void cMenuUndelete::FillMenu(void)
{
  STATUS(tr("Display$prepare recording display..."));
  DeletedRecordings.Clear();
#if VDRVERSNUM >= 10311
  DeletedRecordings.Load();
#else
  DeletedRecordings.Load(true);
#endif
  DeletedRecordings.Sort();
  KeyState = 1;
  AddSubMenu(new cMenuRecordingSelect);
}

eOSState cMenuUndelete::ProcessKey(eKeys Key)
{
  d3ProcessKey1("cMenuUndelete::ProcessKey");
  eOSState state = cOsdMenu::ProcessKey(Key);
  d3ProcessKey2("cMenuUndelete::ProcessKey");
#ifdef UND_Debug3
  if (!noneKey || state != osUnknown)
    d3syslog("cMenuUndelete::ProcessKey", "WorkFilename=%s", WorkFilename ? WorkFilename : "(NULL)");
#endif

  if (state == osUnknown)
  {
    cRecording *recording = NULL;
    bool processerror = false;
    bool processrecording = false;
    if (WorkFilename && (Key == kPurgeRec || Key == kSalvageRec))
      STATUS(tr("Display$please wait ..."));
    switch (Key)
    {
      case kPurgeRec:   state = osContinue;
                        if (WorkFilename)
                        {
                          for (recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording))
                            if (!strncmp(recording->FileName(), WorkFilename, strlen(WorkFilename)) && (*(WorkFilename + strlen(WorkFilename) - 1) != '/' || strchr(recording->FileName() + strlen(WorkFilename), '/')))
                            {
                              if (verbose.u)
                                isyslog("%s: purge deleted recording (%s)", plugin_name, recording->FileName());
                              if (!RemoveVideoFile(recording->FileName()))
                              {
                                esyslog("%s: Error while remove deleted recording (%s)", plugin_name, recording->FileName());
                                processerror = true;
                              }
                              PurgeRecording = true;
                              cRemote::Put(Key);
                              DeletedRecordings.Del(recording);
                              processrecording = true;
                              break;
                            }
                        }
                        break;
      case kSalvageRec: state = osContinue;
                        if (WorkFilename)
                        {
                          for (recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording))
                            if (!strncmp(recording->FileName(), WorkFilename, strlen(WorkFilename)) && (*(WorkFilename + strlen(WorkFilename) - 1) != '/' || strchr(recording->FileName() + strlen(WorkFilename), '/')))
                            {
                              if (verbose.u)
                                isyslog("%s: undelete recording (%s)", plugin_name, recording->FileName());
                              char *NewName = strdup(recording->FileName());
                              char *ext = strrchr(NewName, '.');
                              if (!strcmp(ext, DELEXT))
                              {
                                strncpy(ext, RECEXT, strlen(ext));
                                if (!access(NewName, F_OK))
                                {
                                  if (verbose.u)
                                    isyslog("%s: Recording with the same name exists (%s)", plugin_name, NewName);
                                  ERROR(tr("Error$Recording with the same name exists!"));
                                  processerror = true;
                                } else
                                {
                                  if (!RenameVideoFile(recording->FileName(), NewName))
                                  {
                                    esyslog("%s: Error while rename deleted recording (%s) to (%s)", plugin_name, recording->FileName(), NewName);
                                    processerror = true;
                                  }
                                  SalvageRecording = true;
                                  cIndexFile *index = new cIndexFile(NewName, false);
                                  int LastFrame = index->Last() - 1;
                                  if (LastFrame > 0) {
                                    uchar FileNumber = 0;
                                    int FileOffset = 0;
                                    index->Get(LastFrame, &FileNumber, &FileOffset);
                                    delete index;
                                    if (FileNumber == 0) {
                                      if (verbose.u)
                                        isyslog("%s: error while read last filenumber (%s)", plugin_name, NewName);
                                      ERROR(tr("Error$error while read last filenumber"));
                                    } else {
                                      for (int i = 1; i <= FileNumber; i++) {
                                        char *temp;
                                        asprintf(&temp, "%s/%03d.vdr", (const char *)NewName, i);
                                        if (access(temp, R_OK) != 0) {
                                          i = FileNumber;                                          
                                          if (verbose.u)
                                            isyslog("%s: error accessing vdrfile %03d (%s)", plugin_name, i, NewName);
                                          free(temp);
                                          asprintf(&temp, tr("Error$error accessing vdrfile %03d"), i);
                                          ERROR(temp);
                                        }
                                        free(temp);
                                      }
                                    }
                                  } else {
                                    delete index;
                                    if (verbose.u)
                                      isyslog("%s: error accessing indexfile (%s)", plugin_name, NewName);
                                    ERROR(tr("Error$error accessing indexfile"));
                                  }
// *********************************                                  
                                }
                              }
                              free(NewName);
                              cRemote::Put(Key);
                              DeletedRecordings.Del(recording);
                              processrecording = true;
                              break;
                            }
                        }
                        break;
      case kHelpKeys:   if (!HasSubMenu())
                          state = osBack;
      default:          break;
    }
    if (WorkFilename && (!processrecording || processerror))
    {
      freenull(WorkFilename);
      if (processerror)
      {
        CloseSubMenu();
        Clear();
        FillMenu();
      } else
        cRemote::Put((eKeys)kDelLine);
      STATUS(NULL);
    }
  }
  if (!WorkFilename && !HasSubMenu())
    state = osBack;

  d3ProcessKey3("cMenuUndelete::ProcessKey");
  return state;
}
