/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menuplaylist.c 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#include "menuplaylist.h"
#include "dataplaylist.h"
#if VDRVERSNUM < 10307
#include "menuitemtext.h"
#endif
#include "playlist.h"
#include "i18n.h"
#include "vdrtools.h"
#include <vdr/interface.h>
#include <vdr/videodir.h>
#ifdef HAVE_ICONPATCH
#include <vdr/iconpatch.h>
#endif

// --- cMenuRecordingSelectItem --------------------------------------------------------

cMenuRecordingSelectItem::cMenuRecordingSelectItem(cRecording *Recording, int Level)
{
  filename = strdup(Recording->FileName());
  totalEntries = newEntries = 0;
  start = Recording->start;
  SetText(Recording->Title('\t', true, Level));
  if ((isdir = (*Text() == '\t')))
    name = strdup(Text() + 2);
  else
    name = strdup(Recording->Name());
  ismark = false;
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingSelectItem::cMenuRecordingSelectItem FileName=%s Level=%d Name=%s", plugin_name, filename, Level, name);
#endif
}

cMenuRecordingSelectItem::~cMenuRecordingSelectItem()
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingSelectItem::~cMenuRecordingSelectItem FileName=%s Name=%s", plugin_name, filename, name);
#endif
  FREENULL(filename);
  FREENULL(name);
}

bool cMenuRecordingSelectItem::operator< (const cListObject &ListObject)
{
  cMenuRecordingSelectItem *temp = (cMenuRecordingSelectItem *)&ListObject;
  if (isdir != temp->isdir)
    return isdir;
  if (isdir)
    return strcasecmp(name, temp->name) < 0;
  return start < temp->start;
}

void cMenuRecordingSelectItem::IncrementCounter(bool New)
{
  totalEntries++;
  if (New)
    newEntries++;
  char *buffer = NULL;
  asprintf(&buffer, "%d\t%d\t%s", totalEntries, newEntries, name);
  SetText(buffer, false);
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingSelectItem::IncrementCounter FileName=%s New=%s Buffer=%s", plugin_name, filename, New ? "true" : "false", buffer);
#endif
}

// --- cMenuRecordingSelect --------------------------------------------------------

cMenuRecordingSelect::cMenuRecordingSelect(const char *Base, int Level, bool OpenSubMenus, bool SingleSelect)
:cOsdMenu(Base ? Base : tr("Recordings"), 6, 6, 6)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingSelect::cMenuRecordingSelect Base=%s Level=%d OpenSubMenus=%s", plugin_name, Base, Level, OpenSubMenus ? "true" : "false");
#endif
#if VDRVERSNUM < 10311
#ifdef HAVE_ELCHI
  Interface->Status(tr("Display$prepare recording display..."), Setup.Theme == themeVanilla ? clrBlack : (eDvbColor)fginfofont, Setup.Theme == themeVanilla ? clrCyan : clrInfoLine);
#else
  Interface->Status(tr("Display$prepare recording display..."));
#endif
  Interface->Flush();
#else
  STATUS(tr("Display$prepare recording display..."));
#endif
  base = Base ? strdup(Base) : NULL;
  level = Level;
  singleselect = SingleSelect;
  Display();
  cMenuRecordingSelectItem *LastItem = NULL;
  char *LastItemText = NULL;
  for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording))
  {
    if (!Base || (strstr(recording->Name(), Base) == recording->Name() && recording->Name()[strlen(Base)] == '~'))
    {
      cMenuRecordingSelectItem *Item = new cMenuRecordingSelectItem(recording, level);
      if (*Item->Text() && (!LastItem || strcmp(Item->Text(), LastItemText) != 0))
      {
        Add(Item);
        LastItem = Item;
        FREENULL(LastItemText);
        LastItemText = strdup(LastItem->Text());
      } else
        delete Item;
      if (LastItem)
      {
        if (LastSelectedRecord && strncmp(LastSelectedRecord, recording->FileName(), strlen(LastSelectedRecord)) == 0)
          SetCurrent(LastItem);
        if (LastItem->IsDirectory())
          LastItem->IncrementCounter(recording->IsNew());
      }
    }
  }
  free(LastItemText);
  LastItem = Current() < 0 ? NULL : (cMenuRecordingSelectItem *)Get(Current());
  Sort();
  if (LastItem)
  {
    SetCurrent(LastItem);
    if (OpenSubMenus && Open(true) == osContinue)
      return;
  } else
    SetCurrent(First());
  SetHelpKeys();
}

cMenuRecordingSelect::~cMenuRecordingSelect()
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingSelect::~cMenuRecordingSelect Base=%s Level=%d", plugin_name, base, level);
#endif
  FREENULL(base);
}

void cMenuRecordingSelect::SetHelpKeys(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingSelect::SetHelpKeys", plugin_name);
#endif

  cMenuRecordingSelectItem *ri = (cMenuRecordingSelectItem *)Get(Current());
  if (ri)
  {
    if (ri->IsDirectory())
      if (singleselect)
        SetHelp(tr("Open"));
      else
        SetHelp(tr("Add"), ri->IsMark() ? tr("UnMark") : tr("Mark"), NULL, NULL);
    else
    {
      cRecording *recording = GetRecording(ri);
      SetHelp(singleselect ? tr("Select") : tr("Add"), singleselect ? NULL : ri->IsMark() ? tr("UnMark") : tr("Mark"), deleterecords.u ? tr("Delete") : NULL, (recording && recording->Info()->Title() && *recording->Info()->Title()) ? tr("Summary") : NULL);
    }
  } else
  {
    SetHelp(NULL);
  }
}

cRecording *cMenuRecordingSelect::GetRecording(cMenuRecordingSelectItem *Item)
{
  cRecording *recording = Recordings.GetByName(Item->FileName());
  if (!recording)
     ERROR(tr("Error while accessing recording!"));
  return recording;
}

eOSState cMenuRecordingSelect::Open(bool OpenSubMenus)
{
  cMenuRecordingSelectItem *ri = (cMenuRecordingSelectItem *)Get(Current());
  if (ri)
  {
    if (ri->IsDirectory())
    {
      const char *t = ri->Name();
      char *buffer = NULL;
      if (base)
      {
        asprintf(&buffer, "%s~%s", base, t);
        t = buffer;
      }
      AddSubMenu(new cMenuRecordingSelect(t, level + 1, OpenSubMenus, singleselect));
      FREENULL(buffer);
      return osContinue;
    } else if (!OpenSubMenus)
      return AddPlaylist();
  }
  return osUnknown;
}

eOSState cMenuRecordingSelect::AddPlaylist(void)
{
  FREENULL(LastSelectedRecord);
  cMenuRecordingSelectItem *ri = (cMenuRecordingSelectItem *)Get(Current());
  if (ri)
  {
    if (ri->IsDirectory() && singleselect)
      return Open();
    LastSelectedRecord = strdup(ri->FileName());
    if (ri->IsDirectory())
    {
      char *last = LastSelectedRecord + strlen(VideoDirectory) + 1;
      for (int i = level; *last && i >= 0; i--)
      {
        last = strchr(last, '/');
        if (!last)
          break;
        last++;
      }
      if (last)
        *last = 0;
    }
    if (PlaylistMark.Count() && !ri->IsMark() && Interface->Confirm(tr("Add selected Record to Playlist?")))
    {
      cPlaylistMark *pm = new cPlaylistMark(LastSelectedRecord);
      PlaylistMark.Add(pm);
    }
    return osUser1; // Add to Playlist in cMenuPlaylist::ProcessKey
  }
  return osUnknown;
}

eOSState cMenuRecordingSelect::MarkSel(void)
{
  if (singleselect)
    return osContinue;
  INFO("kommt noch ...?");
  return osContinue;
}

eOSState cMenuRecordingSelect::Delete(void)
{
  if (!deleterecords.u || HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingSelectItem *ri = (cMenuRecordingSelectItem *)Get(Current());
  if (ri && !ri->IsDirectory() && Interface->Confirm(tr("Delete recording?")) && DeleteRecording(ri->FileName()))
  {
    cOsdMenu::Del(Current());
    Display();
    if (!Count())
      return osBack;
  }
  return osContinue;
}

eOSState cMenuRecordingSelect::Summary(void)
{
  if (HasSubMenu() || Count() == 0)
    return osContinue;
  cMenuRecordingSelectItem *ri = (cMenuRecordingSelectItem *)Get(Current());
  if (ri && !ri->IsDirectory())
  {
    cRecording *recording = GetRecording(ri);
    if (recording && recording->Info()->Title() && *recording->Info()->Title())
#if VDRVERSNUM >= 10307
      return AddSubMenu(new cMenuText(tr("Summary"), recording->Info()->Title()));
#else
      return AddSubMenu(new cMenuItemText(tr("Summary"), recording->Info()->Title()));
#endif
  }
  return osContinue;
}

eOSState cMenuRecordingSelect::ProcessKey(eKeys Key)
{
#ifdef PL_Debug3
  bool noneKey = Key == kNone;
  if (!noneKey)
    dsyslog("%s: cMenuRecordingSelect::ProcessKey Key=%s", plugin_name, KeyName(Key));
#endif

  eOSState state = cOsdMenu::ProcessKey(Key);
#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
  {
    dsyslog("%s: cMenuRecordingSelect::ProcessKey OSState=%s", plugin_name, OSStateName(state));
    noneKey = false;
  }
#endif

  switch (state)
  {
    case osUnknown:  switch (Key)
                     {
                       case kOk:     state = Open();
                                     break;
                       case kRed:    state = AddPlaylist();
                                     break;
                       case kGreen:  state = MarkSel();
                                     break;
                       case kYellow: state = Delete();
                                     break;
                       case kBlue:   state = Summary();
                                     break;
                       default: break;
                     }
                     break;
    default:         break;
  }
  if (!HasSubMenu() && Key != kNone)
    SetHelpKeys();

#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
    dsyslog("%s: cMenuRecordingSelect::ProcessKey returned OSState=%s", plugin_name, OSStateName(state));
#endif
  return state;
}

// --- cMenuRecordingEdit -----------------------------------------------------------

cMenuRecordingEdit::cMenuRecordingEdit(cPlaylistRecord *PlaylistRecord, bool ConfirmOptions)
:cOsdMenu(ConfirmOptions ? tr("Edit$Confirm Options") : tr("Edit$Edit Record"), 33)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingEdit::cMenuRecordingEdit Playlistentry=%s ConfirmOptions=%s", plugin_name, PlaylistRecord ? PlaylistRecord->Name() : "(null)", ConfirmOptions ? "true" : "false");
#endif
  playlistrecord = PlaylistRecord;
  confirmoptions = ConfirmOptions;
#if VDRVERSNUM >= 10307
  editwidth = DisplayMenu()->EditableWidth();
#endif
  DisplayStatus[0] = 0;
  if (playlistrecord)
  {
    data = *playlistrecord;
    Set();
  }
  SetHelpKeys();
}

cMenuRecordingEdit::~cMenuRecordingEdit()
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingEdit::~cMenuRecordingEdit Playlistentry=%s", plugin_name, playlistrecord ? playlistrecord->Name() : "(null)");
#endif
  data.filename = NULL;
  data.summary = NULL;
  data.title = NULL;
  data.path = NULL;
}

void cMenuRecordingEdit::SetHelpKeys(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingEdit:SetHelpKeys", plugin_name);
#endif

  char *buffer = NULL;
  eOptions display = (Current() >= 0) && (Current() < MAXOptionLines) ? displayoption[Current()] : Option_max;
  if (display < Option_max && data.OptionInherited(display))
    asprintf(&buffer, "%s %s", tr("OptionPL$is:"), NoYesDefault[data.Option(display)]);
  if (buffer)
  {
    if (strcmp(DisplayStatus, buffer))
      strn0cpy(DisplayStatus, buffer, MAXOSDTEXTWIDTH);
    free(buffer);
  } else
    if (strlen(DisplayStatus))
      DisplayStatus[0] = 0;
  const char *red = strlen(DisplayStatus) ? DisplayStatus : NULL;
  if (confirmoptions)
    SetHelp(red);
  else
    SetHelp(red, (data.isdir || data.parent) ? NULL : tr("Select"), tr("Delete"), (data.summary && *data.summary) ? tr("Summary") : NULL);
}

void cMenuRecordingEdit::Set(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuRecordingEdit::Set Current=%d", plugin_name, Current());
#endif
  for (int i = 0; i < MAXOptionLines; i++)
    displayoption[i] = Option_max;
  int current = Current();
  Clear();

  if (data.path)
  {
    char *buffer = NULL;
    asprintf(&buffer, "%s: %s",  tr("Path"), data.path);

#if VDRVERSNUM >= 10307
    cOsdItem *item;
    strreplace(buffer, ' ', '|');
    strreplace(buffer, '~', ' ');
    cTextWrapper *textwrapper = new cTextWrapper(buffer, cFont::GetFont(fontOsd), editwidth);
    for (int i = 0; i < textwrapper->Lines(); i++)
    {
      char *textbuf = strdup(textwrapper->GetLine(i));
      strreplace(buffer, ' ', '/');
      strreplace(buffer, '|', ' ');
      item = new cOsdItem(textbuf);
      free(textbuf);
      item->SetSelectable(false);
      Add(item);
    }
    delete textwrapper;
    item = new cOsdItem(" ");
    item->SetSelectable(false);
    Add(item);
#else
    strreplace(buffer, '~', '/');
    char *pos = buffer;
    char *pos_next;
    char *pos_last;
    char text[MAXOSDWIDTH + 1];
    int textlen;

    while (*pos)
    {
      pos_next = strchr(pos, '\n');
      if (!pos_next)
        pos_next = pos + strlen(pos);
      textlen = pos_next - pos;
      if (textlen < (Setup.OSDwidth - 1))
      {
        strn0cpy(text, pos, textlen + 1);
      } else
      {
        strn0cpy(text, pos, Setup.OSDwidth - 1);
        pos_last = strrchr(text, '/');
        if (!pos_last)
          pos_last = text + Setup.OSDwidth - 1;
        *pos_last = 0;
        textlen = pos_last - text;
      }
      Add(new cOsdItem(text));
      pos += textlen;
      if (*pos) pos++;
    }
    Add(new cOsdItem(" "));
#endif
    free(buffer);
  }
#if VDRVERSNUM < 10307
  firstline = Count();
#endif

  Add(new cMenuEditStrItem(tr("OptionPL$Name"),                                          data.name, sizeof(data.name), tr(FileNameChars)));
  if (current < Count())
    current = Count();
  if (data.isdir)
  {
    displayoption[Count()] = Option_searchnewrecord;
    Add(new cMenuEditStraItem(tr("OptionPL$Search new records on folders"),              &data.options[Option_searchnewrecord], 4, NoYesDefault));
    displayoption[Count()] = Option_searchrecordsub;
    Add(new cMenuEditStraItem(tr("OptionPL$Search records on subfolders"),               &data.options[Option_searchrecordsub], 4, NoYesDefault));
    displayoption[Count()] = Option_playonlynew;
    Add(new cMenuEditStraItem(tr("OptionPL$Play only new Records"),                      &data.options[Option_playonlynew], data.parent ? 5 : 4, NoYesDefault));
  }
  displayoption[Count()] = Option_jumpmark;
  Add(new cMenuEditStraItem(tr("OptionPL$Jump to first segment of movie"),               &data.options[Option_jumpmark], data.parent ? 5 : 4, NoYesDefault));
  displayoption[Count()] = Option_confirmstartnewrecord;
  Add(new cMenuEditStraItem(tr("OptionPL$Confirm start new record"),                     &data.options[Option_confirmstartnewrecord], data.parent ? 5 : 4, NoYesDefault));
  displayoption[Count()] = Option_deletefromlist;
  Add(new cMenuEditStraItem(tr("OptionPL$Remove records after play from list"),          &data.options[Option_deletefromlist], data.parent ? 5 : 4, NoYesDefault));
  displayoption[Count()] = Option_deleterecord;
  Add(new cMenuEditStraItem(tr("OptionPL$Delete records after play"),                    &data.options[Option_deleterecord], data.parent ? 5 : 4, NoYesDefault));
  if (data.options[Option_deleterecord] != NoYesDefault_no)
  {
    displayoption[Count()] = Option_confirmdeleterecord;
    Add(new cMenuEditStraItem(tr("OptionPL$  Confirm delete records"),                   &data.options[Option_confirmdeleterecord], data.parent ? 5 : 4, NoYesDefault));
  }
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuRecordingEdit::Summary(void)
{
  if (!HasSubMenu() && !data.isdir && data.summary && *data.summary)
#if VDRVERSNUM >= 10307
    return AddSubMenu(new cMenuText(tr("Summary"), data.summary));
#else
    return AddSubMenu(new cMenuItemText(tr("Summary"), data.summary));
#endif


  return osContinue;
}

eOSState cMenuRecordingEdit::ProcessKey(eKeys Key)
{
#ifdef PL_Debug3
  bool noneKey = Key == kNone;
  if (!noneKey)
    dsyslog("%s: cMenuRecordingEdit::ProcessKey Key=%s", plugin_name, KeyName(Key));
#endif
  int m_deleterecord = data.options[Option_deleterecord];

  eOSState state = cOsdMenu::ProcessKey(Key);
#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
  {
    dsyslog("%s: cMenuRecordingEdit::ProcessKey OSState=%s", plugin_name, OSStateName(state));
    noneKey = false;
  }
#endif

  if (m_deleterecord != data.options[Option_deleterecord])
    Set();

#if VDRVERSNUM < 10307
  if (Current() < firstline)
  {
    DisplayCurrent(false);
    SetCurrent(Get(firstline));
    DisplayCurrent(true);
  }
#endif

  char *buffer = NULL;
  eOptions display = (Current() >= 0) && (Current() < MAXOptionLines) ? displayoption[Current()] : Option_max;
/*  if (display < Option_max && data.OptionInherited(display))
    asprintf(&buffer, "%s %s", tr("OptionPL$Effective setting is:"), NoYesDefault[data.Option(display)]);
  if (buffer)
  {
    if (strcmp(DisplayStatus, buffer))
    {
      strn0cpy(DisplayStatus, buffer, MAXOSDTEXTWIDTH);
      SetStatus(DisplayStatus);
    }
    free(buffer);
  } else
  {
    if (strlen(DisplayStatus))
    {
      DisplayStatus[0] = 0;
      SetStatus(NULL);
    }
  }*/

  switch (state)
  {
    case osUnknown:  switch (Key)
                     {
                       case kOk:     strn0cpy(data.name, skipspace(stripspace(data.name)), sizeof(data.name));
                                     Display();
                                     if (!strlen(data.name))
                                     {
                                       ERROR(tr("Error$Missing playlistentry name!"));
                                       break;
                                     }
                                     if (playlistrecord)
                                     {
                                       data.isdel = playlistrecord->isdel;
                                       if (data != *playlistrecord)
                                       {
                                         *playlistrecord = data;
                                         if (playlistrecord->Playlist())
                                           playlistrecord->Playlist()->SetChange();
                                         if (verbose.u)
                                           isyslog("%s: playlistentry %d modified (%s)", plugin_name, playlistrecord->Index() + 1, playlistrecord->name);
                                       }
                                       data.filename = NULL;
                                       state = osBack;
                                     }
                                     break;
                       case kRed:    state = osContinue;
                                     break;
                       case kGreen:  LastSelectedRecord = playlistrecord->filename;
                                     state = AddSubMenu(new cMenuRecordingSelect(NULL, 0, true, true));
                                     break;
                       case kYellow: state = osUser2; // delete Playlistentry in cMenuPlaylist::ProcessKey
                                     break;
                       case kBlue:   state = Summary();
                                     break;
                       default:      break;
                     }
                     break;
    case osUser1:    state = CloseSubMenu();
                     if (state == osContinue && LastSelectedRecord && *LastSelectedRecord && LastSelectedRecord[strlen(LastSelectedRecord) - 1] != '/')
                     {
                       playlistrecord->SetName(LastSelectedRecord);
                       playlistrecord->CopyFromRecording();
                       data = *playlistrecord;
                       Set();
                     }
                     break;
    default:         break;
  }
  if (!HasSubMenu() && Key != kNone)
    SetHelpKeys();

#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
    dsyslog("%s: cMenuRecordingEdit::ProcessKey returned OSState=%s", plugin_name, OSStateName(state));
#endif
  return state;
}

// --- cMenuPlaylistItem --------------------------------------------------------

cMenuPlaylistItem::cMenuPlaylistItem(cPlaylistRecord *PlaylistRecord)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylistItem::cMenuPlaylistItem Playlistentry=%s", plugin_name, PlaylistRecord ? PlaylistRecord->Name() : "(null)");
#endif
  playlistrecord = PlaylistRecord;
  Set();
}

cMenuPlaylistItem::~cMenuPlaylistItem()
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylistItem::~cMenuPlaylistItem Playlistentry=%s", plugin_name, playlistrecord ? playlistrecord->Name() : "(null)");
#endif
}

void cMenuPlaylistItem::Set(void)
{
  char *buffer = NULL;
  if (playlistrecord)
  {
    if (playlistrecord->IsDel())
      asprintf(&buffer, "\t%s", tr("Error$Playlistentry or Record is deleted"));
    else if (playlistrecord->IsDir())
      asprintf(&buffer, "%c\t%s", charfolder.u, playlistrecord->Name());
    else
      asprintf(&buffer, "%c\t%s\t%s", playlistrecord->Parent() ? charentry.u : charrecord.u, playlistrecord->Title(), playlistrecord->Name());
  } else
    buffer = strdup(tr("Error$Playlistentry is not set"));
  SetText(buffer, false);
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylistItem::Set SetText=%s", plugin_name, buffer);
#endif
}

// --- cMenuPlaylist -----------------------------------------------------------

cMenuPlaylist::cMenuPlaylist(cPlaylist *Playlist)
:cOsdMenu(tr("Display$Display Playlist"), 2, 6, 7, 5)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylist::cMenuPlaylist", plugin_name);
#endif

  playlist = Playlist;
  if (playlist)
    for (cPlaylistRecord *plr = playlist->First_PLR(); plr; plr = playlist->Next_PLR(plr))
      if (!plr->IsDel())
        Add(new cMenuPlaylistItem(plr));
  addrecords = false;
  if (Count())
  {
    SetHelpKeys();
    Display();
  } else
    AddSubMenu(new cMenuRecordingSelect(NULL, 0, true));
}

cMenuPlaylist::~cMenuPlaylist(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylist::~cMenuPlaylist", plugin_name);
#endif
  playlist = NULL;
}

void cMenuPlaylist::SetHelpKeys(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylist::SetHelpKeys", plugin_name);
#endif
  if (Count())
    SetHelp((playlist && *playlist->Name()) ? tr("Play") : NULL, tr("New"), tr("Delete"), tr("Mark"));
  else
    SetHelp(NULL, tr("New"), NULL, NULL);
}

eOSState cMenuPlaylist::Play(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylist::Play", plugin_name);
#endif
  cPlaylistRecord *plr = CurrentPlaylistRecord();
  if (HasSubMenu() || !plr || !playlist || !*playlist->Name())
    return osContinue;
  if (verbose.u)
    isyslog("%s: start play PlaylistRecord %d Option=%d", plugin_name, plr->Index() + 1, playlist->Playoption(Option_playoption2));
  playlist->SearchRecords();
  cPlaylistRecord *plr_selected = plr;
  cPlaylistRecord *plr_firstnomal = playlist->Firstnormal_PLR();
  cPlaylistRecord *plr_firstnew = playlist->Firstnew_PLR();
  cPlaylistRecord *plr_lastplayed = playlist->Lastplayed_PLR();
  while (plr_selected && plr_selected->IsDirOrDel())
    plr_selected = playlist->Next_PLR(plr_selected);
  switch (playlist->Playoption(Option_playoption2))
  {
    case PlayOptions_selectpos: SelectPLR = plr_selected;
                                if (!SelectPLR)
                                  ERROR(tr("Error$At this position and after no recording found!"));
                                break;
    case PlayOptions_firstpos:  SelectPLR = plr_firstnomal;
                                break;
    case PlayOptions_firstnew:  if (plr_firstnew)
                                  SelectPLR = plr_firstnew;
                                else
                                  SelectPLR = Interface->Confirm(tr("Question$No new recording found! Play first?")) ? plr_firstnomal : NULL;
                                break;
    case PlayOptions_lastplay:  if (plr_lastplayed)
                                {
                                  SelectPLR = plr_lastplayed;
                                  break;
                                }
                                // no break!
    case PlayOptions_question:  if (plr_selected && Interface->Confirm(tr("Question$Play selected recording?")))
                                {
                                  SelectPLR = plr_selected;
                                  break;
                                }
                                if (plr_lastplayed && Interface->Confirm(tr("Question$Play last played recording?")))
                                {
                                  SelectPLR = plr_lastplayed;
                                  break;
                                }
                                if (plr_firstnew && plr_firstnew != plr_firstnomal && Interface->Confirm(tr("Question$Play first new recording?")))
                                {
                                  SelectPLR = plr_firstnew;
                                  break;
                                }
                                SelectPLR = plr_firstnomal;
                                break;
  }
  return SelectPLR ? osUser9 : osContinue; // osUSer9 starts play playlist with SelectPLR in cControlMenu::ProcessKey
}

eOSState cMenuPlaylist::Edit(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylist::Edit", plugin_name);
#endif
  if (HasSubMenu() || !CurrentPlaylistRecord())
    return osContinue;
  isyslog("%s: editing Playlistentry %d", plugin_name, CurrentPlaylistRecord()->Index() + 1);
  return AddSubMenu(new cMenuRecordingEdit(CurrentPlaylistRecord()));
}

bool cMenuPlaylist::AddPlaylistentrys()
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylist::AddPlaylistentrys Add=%s", plugin_name, LastSelectedRecord);
#endif
  static cPlaylistRecord *playlistrecord = NULL;
  bool iswithdir = LastSelectedRecord && LastSelectedRecord[strlen(LastSelectedRecord) - 1] == '/';
  if (!playlistrecord && iswithdir)
  {
    playlistrecord = new cPlaylistRecord(NULL,LastSelectedRecord);
    AddSubMenu(new cMenuRecordingEdit(playlistrecord, true));
    addrecords = true;
    return true;
  }
  if (!playlist || !playlist->RecordCol2())
    return false;
  if (iswithdir)
  {
    PlaylistSort.Clear();
    cPlaylistRecord *p_plr = new cPlaylistRecord(playlist, LastSelectedRecord);
    p_plr->CopyOptions(playlistrecord);
    playlist->RecordCol2()->Add(p_plr);
    cMenuPlaylistItem *mpli = new cMenuPlaylistItem(p_plr);
    Add(mpli);
    SetCurrent(mpli);
    cPlaylistRecord *plr = NULL;
    for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording))
    {
      if (strstr(recording->FileName(), LastSelectedRecord) == recording->FileName())
      {
        const char *next = NULL;
        if (!p_plr->Option(Option_searchrecordsub))
        {
          const char *test = recording->FileName() + strlen(LastSelectedRecord);
          const char *temp = strchr(test, '/');
          if (temp)
            next = strchr(temp + 1, '/');
        }
        if (!next)
        {
          plr = new cPlaylistRecord(playlist, recording, p_plr);
          PlaylistSort.Add(plr);
        }
      }
    }
    PlaylistSort.Sort();
    cPlaylistRecord *nplr = NULL;
    for (plr = PlaylistSort.First(); plr; plr = nplr)
    {
      nplr = PlaylistSort.Next(plr);
      PlaylistSort.Del(plr, false);
      playlist->RecordCol2()->Add(plr);
      Add(new cMenuPlaylistItem(plr));
    }
  } else
  { // one record to add
    cPlaylistRecord *plr = new cPlaylistRecord(playlist, LastSelectedRecord);
    playlist->RecordCol2()->Add(plr);
    cMenuPlaylistItem *mpli = new cMenuPlaylistItem(plr);
    Add(mpli);
    SetCurrent(mpli);
  }
  playlist->SetChange();
  addrecords = false;
  DELETENULL(playlistrecord);
  Display();
  return true;
}

eOSState cMenuPlaylist::New(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylist::New", plugin_name);
#endif
  if (HasSubMenu())
    return osContinue;
  cOsdMenu *menu = new cMenuRecordingSelect(NULL, 0, true);
  eOSState state = AddSubMenu(menu);
  if (LastSelectedRecord && LastSelectedRecord[strlen(LastSelectedRecord) - 1] == '/')
    menu->ProcessKey(kBack);
  return state;
}

eOSState cMenuPlaylist::Delete(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylist::Delete", plugin_name);
#endif
  cPlaylistRecord *plr = CurrentPlaylistRecord();
  if (plr && playlist)
  {
    if (!confirmdelplentry.u || Interface->Confirm(tr("Edit$Delete Playlistentry?")))
    {
      if (verbose.u)
        isyslog("%s: Playlistentry %d deleted", plugin_name, plr->Index() + 1);
      plr->SetDel();
      playlist->SetChange();
      cOsdMenu::Del(Current());
      Display();
    }
  }
  return osContinue;
}

void cMenuPlaylist::Move(int From, int To)
{
#ifdef PL_Debug1
  dsyslog("%s: cMenuPlaylist::Move from=%d to=%d", plugin_name, From, To);
#endif
  if (playlist && playlist->RecordCol2())
  {
    cMenuPlaylistItem *temp = (cMenuPlaylistItem *)Get(From);
    cPlaylistRecord *plr_f = temp ? temp->PlaylistRecord() : NULL;
    temp = (cMenuPlaylistItem *)Get(To);
    cPlaylistRecord *plr_t = temp ? temp->PlaylistRecord() : NULL;
    cPlaylistRecord *plr_p = NULL;
    if (To > 0)
    {
      temp = (cMenuPlaylistItem *)Get(To - 1);
      plr_p = temp ? temp->PlaylistRecord() : NULL;
    }
    // plr_f --> cPlaylistRecord (Source)
    // plr_t --> cPlaylistRecord at Targetposition
    // plr_p --> cPlaylistRecord before Targetposition
    if (!plr_f || !plr_t)
    {
      esyslog("%s: Error while get playlistentries in move-function", plugin_name);
      return;
    }
    if (plr_f->Parent() == plr_t->Parent() || (plr_p && plr_f->Parent() == plr_p->Parent()))
    {
      playlist->RecordCol2()->Move(plr_f->Index(), plr_t->Index());
      playlist->SetChange();
      cOsdMenu::Move(From, To);
      Display();
    } else
      ERROR(tr("Error$Move between folders and not folders not allowed."));
  }
}

cPlaylistRecord *cMenuPlaylist::CurrentPlaylistRecord(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylist::CurrentPlaylistRecord", plugin_name);
#endif
  cMenuPlaylistItem *item = (cMenuPlaylistItem *)Get(Current());
  return item ? item->PlaylistRecord() : NULL;
}

eOSState cMenuPlaylist::ProcessKey(eKeys Key)
{
#ifdef PL_Debug3
  bool noneKey = Key == kNone;
  if (!noneKey)
    dsyslog("%s: cMenuPlaylist::ProcessKey Key=%s", plugin_name, KeyName(Key));
#endif
  int n;
  bool hSubMenu = HasSubMenu();

  eOSState state = cOsdMenu::ProcessKey(Key);
#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
  {
    dsyslog("%s: cMenuPlaylist::ProcessKey OSState=%s", plugin_name, OSStateName(state));
    noneKey = false;
  }
#endif

  if (!HasSubMenu() && hSubMenu)
  {
    for (cOsdItem *item = First(); item; item = Next(item))
      item->Set();
    if (addrecords)
      AddPlaylistentrys(); // submenu for folder-options is closed
    else
      Display();
  }

  switch (state)
  {
    case osUnknown:  switch (Key)
                     {
                       case k0...k9: n = (Key == k0) ? 10 : (Key - k0);
                                     if (n > Count())
                                       break;
                                     SetCurrent(Get(--n));
                                     // no break!
                       case kOk:     state = Edit();
                                     break;
                       case kRed:    state = Play();
                                     break;
                       case kGreen:  state = New();
                                     break;
                       case kYellow: state = Delete();
                                     break;
                       case kBlue:   if (!HasSubMenu())
                                     {
                                       Mark();
                                       state = osContinue;
                                     }
                                     break;
                       default: break;
                     }
                     break;
    case osUser1:    state = CloseSubMenu();
                     if (state == osContinue)
                       AddPlaylistentrys();
                     break;
    case osUser2:    state = CloseSubMenu();
                     if (state == osContinue)
                       state = Delete();
                     break;
    default:         break;
  }
  if (!HasSubMenu() && Key != kNone)
    SetHelpKeys();

#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
    dsyslog("%s: cMenuPlaylist::ProcessKey returned OSState=%s", plugin_name, OSStateName(state));
#endif
  return state;
}
