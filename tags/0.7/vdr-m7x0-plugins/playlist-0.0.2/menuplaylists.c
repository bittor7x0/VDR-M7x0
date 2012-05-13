/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menuplaylists.c 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#include "menuplaylists.h"
#include "dataplaylist.h"
#include "playlist.h"
#include "menuplaylist.h"
#include "i18n.h"
#include "vdrtools.h"
#include <vdr/interface.h>

// --- cMenuPlaylistEdit -----------------------------------------------------------

cMenuPlaylistEdit::cMenuPlaylistEdit(cPlaylist *Playlist, bool New)
:cOsdMenu(New ? tr("Edit$Create Playlist") : tr("Edit$Edit Playlist"), 33)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylistEdit::cMenuPlaylistEdit Playlist=%s new=%s", plugin_name, Playlist ? Playlist->Name() : "(null)", New ? "true" : "false");
#endif
  playlist = Playlist;
  addIfConfirmed = New;
  DisplayStatus[0] = 0;
  if (playlist)
  {
    data = *playlist;
    Set();
  }
  if (New)
    AddSubMenu(new cMenuPlaylist(playlist));
}

cMenuPlaylistEdit::~cMenuPlaylistEdit()
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylistEdit::~cMenuPlaylistEdit Playlist=%s addIfConfirmed=%s", plugin_name, playlist ? playlist->Name() : "(null)", addIfConfirmed ? "true" : "false");
#endif
  data.lastrecord = NULL;
  data.recordcol = NULL;
  if (playlist && addIfConfirmed)
    delete playlist; // apparently it wasn't confirmed
}

void cMenuPlaylistEdit::Set(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylistEdit::Set Current=%d", plugin_name, Current());
#endif
  for (int i = 0; i < MAXOptionLines; i++)
    displayoption[i] = Option_max;
  int current = Current();
  Clear();
  Add(new cMenuEditStrItem(tr("OptionPL$Name"),                                          data.name, sizeof(data.name), tr(FileNameChars)));
  displayoption[Count()] = Option_playoption1;
  Add(new cMenuEditStraItem(tr("OptionPL$In playlistview starts play at"),               &data.options[Option_playoption1], PlayOptions_max, PlayOptions));
  displayoption[Count()] = Option_playoption2;
  Add(new cMenuEditStraItem(tr("OptionPL$In detailview starts play at"),                 &data.options[Option_playoption2], PlayOptions_max, PlayOptions));
  displayoption[Count()] = Option_searchnewrecord;
  Add(new cMenuEditStraItem(tr("OptionPL$Search new records on folders"),                &data.options[Option_searchnewrecord], 3, NoYesDefault));
  displayoption[Count()] = Option_searchrecordsub;
  Add(new cMenuEditStraItem(tr("OptionPL$Search records on subfolders"),                 &data.options[Option_searchrecordsub], 3, NoYesDefault));
  displayoption[Count()] = Option_playonlynew;
  Add(new cMenuEditStraItem(tr("OptionPL$Play only new Records"),                        &data.options[Option_playonlynew], 3, NoYesDefault));
  displayoption[Count()] = Option_jumpmark;
  Add(new cMenuEditStraItem(tr("OptionPL$Jump to first segment of movie"),               &data.options[Option_jumpmark], 3, NoYesDefault));
  displayoption[Count()] = Option_confirmstartnewrecord;
  Add(new cMenuEditStraItem(tr("OptionPL$Confirm start new record"),                     &data.options[Option_confirmstartnewrecord], 3, NoYesDefault));
  displayoption[Count()] = Option_deletefromlist;
  Add(new cMenuEditStraItem(tr("OptionPL$Remove records after play from list"),          &data.options[Option_deletefromlist], 3, NoYesDefault));
  displayoption[Count()] = Option_deleterecord;
  Add(new cMenuEditStraItem(tr("OptionPL$Delete records after play"),                    &data.options[Option_deleterecord], 3, NoYesDefault));
  if (data.options[Option_deleterecord] != NoYesDefault_no)
  {
    displayoption[Count()] = Option_confirmdeleterecord;
    Add(new cMenuEditStraItem(tr("OptionPL$  Confirm delete records"),                   &data.options[Option_confirmdeleterecord], 3, NoYesDefault));
  }
  SetCurrent(Get(current));
}

eOSState cMenuPlaylistEdit::ProcessKey(eKeys Key)
{
#ifdef PL_Debug3
  bool noneKey = Key == kNone;
  if (!noneKey)
    dsyslog("%s: cMenuPlaylistEdit::ProcessKey Key=%s", plugin_name, KeyName(Key));
#endif
  int m_searchnewrecord = data.options[Option_searchnewrecord];
  int m_jumpmark = data.options[Option_jumpmark];
  int m_deleterecord = data.options[Option_deleterecord];
  int m_playoption = data.options[Option_playoption1];
  bool hSubMenu = HasSubMenu();

  eOSState state = cOsdMenu::ProcessKey(Key);
#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
  {
    dsyslog("%s: cMenuPlaylistEdit::ProcessKey OSState=%s", plugin_name, OSStateName(state));
    noneKey = false;
  }
#endif

  if (m_searchnewrecord != data.options[Option_searchnewrecord] || m_jumpmark != data.options[Option_jumpmark] || m_deleterecord != data.options[Option_deleterecord])
  {
    Set();
    Display();
  } else if (m_playoption != data.options[Option_playoption1] && data.options[Option_playoption1] == PlayOptions_selectpos)
  {
    data.options[Option_playoption1] = m_playoption < PlayOptions_selectpos ? PlayOptions_question : PlayOptions_lastplay;
    RefreshCurrent();
  }

  char *buffer = NULL;
  eOptions display = (Current() >= 0) && (Current() < MAXOptionLines) ? displayoption[Current()] : Option_max;
  if (display < Option_max && data.OptionInherited(display))
    asprintf(&buffer, "%s %s", tr("OptionPL$Effective setting is:"), (display == Option_playoption1 || display == Option_playoption2) ? PlayOptions[data.Playoption(display)] : NoYesDefault[data.Option(display)]);
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
  }
  if (hSubMenu && !HasSubMenu())
  {
    if (playlist && playlist->RecordCol2() && !playlist->RecordCol2()->Count())
      state = osBack; // empty new list
//TODO    name 
    RefreshCurrent(); // name
  }

  cPlaylist *tempplaylist = NULL;
  bool namefound = false;
  switch (state)
  {
    case osUnknown:  switch (Key)
                     {
                       case kOk:     strn0cpy(data.name, skipspace(stripspace(data.name)), sizeof(data.name));
                                     Display();
                                     if (!strlen(data.name))
                                     {
                                       ERROR(tr("Error$Missing playlist name!"));
                                       break;
                                     }
                                     tempplaylist = PlaylistCol.First();
                                     while (tempplaylist)
                                     {
                                       if (tempplaylist != playlist && !strcasecmp(tempplaylist->name, data.name))
                                       {
                                         namefound = true;
                                         break;
                                       }
                                       tempplaylist = PlaylistCol.Next(tempplaylist);
                                     }
                                     if (namefound)
                                     {
                                       ERROR(tr("Error$same name of playlist exists!"));
                                       break;
                                     }
                                     if (playlist)
                                     {
                                       if (data != *playlist)
                                       {
                                         if (strcmp(data.name, playlist->name))
                                           playlist->SetChange();      
                                         *playlist = data;
                                         if (addIfConfirmed)
                                           PlaylistCol.Add(playlist);
                                         PlaylistCol.Save();
                                         if (verbose.u)
                                           isyslog("%s: playlist %d %s (%s)", plugin_name, playlist->Index() + 1, addIfConfirmed ? "added" : "modified", playlist->name);
                                         addIfConfirmed = false;
                                       }
                                       state = osBack;
                                     }
                                     break;
                       case kRed:
                       case kGreen:
                       case kYellow:
                       case kBlue:   state = osContinue;
                                     break;
                       default: break;
                     }
                     break;
    case osBack:     if (playlist && !strlen(playlist->name) && playlist->RecordCol2() && playlist->RecordCol2()->Count() && playlist->RecordCol2()->GetChange() && !Interface->Confirm(tr("Discard all changes?")))
                       state = osContinue;
    default:         break;
  }

#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
    dsyslog("%s: cMenuPlaylistEdit::ProcessKey returned OSState=%s", plugin_name, OSStateName(state));
#endif
  return state;
}

// --- cMenuPlaylistsItem --------------------------------------------------------

cMenuPlaylistsItem::cMenuPlaylistsItem(cPlaylist *Playlist)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylistsItem::cMenuPlaylistsItem Playlist=%s", plugin_name, Playlist ? Playlist->Name() : "(null)");
#endif
  playlist = Playlist;
  Set();
}

cMenuPlaylistsItem::~cMenuPlaylistsItem()
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylistsItem::~cMenuPlaylistsItem Playlist=%s", plugin_name, playlist ? playlist->Name() : "(null)");
#endif
}

bool cMenuPlaylistsItem::operator< (const cListObject &ListObject)
{
  return strcmp(playlist->Name(), ((cMenuPlaylistsItem *)&ListObject)->playlist->Name()) < 0;
}

void cMenuPlaylistsItem::Set(void)
{
  char *buffer = NULL;
  asprintf(&buffer, "%d\t%d\t%s", playlist->RecordCount(), playlist->RecordNew(), playlist->Name());
  SetText(buffer, false);
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylistsItem::Set SetText=%s", plugin_name, buffer);
#endif
}

// --- cMenuPlaylists -----------------------------------------------------------

cMenuPlaylists::cMenuPlaylists(void)
:cOsdMenu(tr("Display$Display Playlists"), 4, 4)
{
#ifdef PL_Debug1
  dsyslog("%s: cMenuPlaylists::cMenuPlaylists", plugin_name);
#endif
  helpkeys = 1;

#if VDRVERSNUM >= 10311
  if (Recordings.NeedsUpdate())
  {
    STATUS(tr("scanning recordings..."));
#else
  if (time(NULL) - LoadTime_Recordings > 3600)
  {
    LoadTime_Recordings = time(NULL);
#ifdef HAVE_ELCHI
    Interface->Status(tr("scanning recordings..."), Setup.Theme == themeVanilla ? clrBlack : (eDvbColor)fginfofont, Setup.Theme == themeVanilla ? clrCyan : clrInfoLine);
#else
    Interface->Status(tr("scanning recordings..."));
#endif
    Interface->Flush();
#endif
    if (verbose.u)
      isyslog("%s: loading recordings", plugin_name);
    Recordings.Load();
    if (verbose.u)
      isyslog("%s: %d recordings loaded", plugin_name, Recordings.Count());
    PlaylistSort.Clear();
    PlaylistMark.Clear();
  } else
  {
#if VDRVERSNUM < 10311
#ifdef HAVE_ELCHI
    Interface->Status(tr("Display$prepare playlists display..."), Setup.Theme == themeVanilla ? clrBlack : (eDvbColor)fginfofont, Setup.Theme == themeVanilla ? clrCyan : clrInfoLine);
#else
    Interface->Status(tr("Display$prepare playlists display..."));
#endif
    Interface->Flush();
#else
    STATUS(tr("Display$prepare playlists display..."));
#endif
  }
  if (verbose.u)
    isyslog("%s: loading playlists", plugin_name);
  SelectPLR = NULL;
  cMenuPlaylistsItem *selitem = NULL;
  PlaylistCol.Load(PluginPlaylist->ExpandPath("playlists.conf"), false);
  for (cPlaylist *playlist = PlaylistCol.First(); playlist; playlist = PlaylistCol.Next(playlist))
    if (playlist->RecordCol2())
    {
      char *tempname = NULL;
      asprintf(&tempname, "playlists.%s.conf", playlist->Name());
      playlist->RecordCol2()->Load(PluginPlaylist->ExpandPath(tempname), false);
      free(tempname);
      cPlaylistRecord *plr_n = NULL;
      for (cPlaylistRecord *plr = playlist->First_PLR(); plr; plr = plr_n)
      {
        plr->Playlist(playlist);
        plr_n = playlist->Next_PLR(plr);
        if (!plr->IsDir() && plr->Start() == 0)
          playlist->RecordCol2()->Del(plr);
      }
      if (playlist->SearchRecords())
        playlist->RecordCol2()->SaveWithFilename();
      if (verbose.u)
        isyslog("%s playlist '%s' contain %d records, %d new", plugin_name, playlist->Name(), playlist->RecordCount(), playlist->RecordNew());
      cMenuPlaylistsItem *item = new cMenuPlaylistsItem(playlist);
      Add(item);
      if (strcasecmp(playlist->Name(), lastplaylist.u) == 0)
      {
        selitem = item;
        SelectPLR = playlist->First_PLR();
      }
    }
  Sort();
  SetStatus(NULL);
  if (selitem)
    SetCurrent(selitem);
  SetHelpKeys();
  Display();
}

cMenuPlaylists::~cMenuPlaylists(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylists::~cMenuPlaylists", plugin_name);
#endif
}

void cMenuPlaylists::SetHelpKeys(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylists::SetHelpKeys", plugin_name);
#endif
  if (Count())
    switch (helpkeys)
    {
      case 1: SetHelp(okstartplay.u ? tr("Edit") : tr("Play"), tr("New"), tr("Delete"), "-->"); break;
      case 2: SetHelp("<--", okstartplay.u ? NULL : tr("Edit"), tr("Option"), NULL); break;
    }
  else
  {
    helpkeys = 1;
    SetHelp(NULL, tr("New"), NULL, NULL);
  }
}

eOSState cMenuPlaylists::Play(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylists::Play", plugin_name);
#endif
  cPlaylist *playlist = CurrentPlaylist();
  if (HasSubMenu() || !playlist)
    return osContinue;
  if (verbose.u)
    isyslog("%s: start play Playlist %d Option=%d", plugin_name, playlist->Index() + 1, playlist->Playoption(Option_playoption1));
  playlist->SearchRecords();
  cPlaylistRecord *plr_firstnomal = playlist->Firstnormal_PLR();
  cPlaylistRecord *plr_firstnew = playlist->Firstnew_PLR();
  cPlaylistRecord *plr_lastplayed = playlist->Lastplayed_PLR();
  switch (playlist->Playoption(Option_playoption1))
  {
    case PlayOptions_selectpos: // select not allow
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
    case PlayOptions_question:  if (plr_lastplayed && Interface->Confirm(tr("Question$Play last played recording?")))
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

eOSState cMenuPlaylists::New(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylists::New", plugin_name);
#endif
  if (HasSubMenu())
    return osContinue;
  FREENULL(LastSelectedRecord);
  return AddSubMenu(new cMenuPlaylistEdit(new cPlaylist, true));
}

eOSState cMenuPlaylists::Delete(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylists::Delete", plugin_name);
#endif
  cPlaylist *playlist = CurrentPlaylist();
  if (PlayedPLR && PlayedPLR->Playlist() == playlist)
  {
    ERROR(tr("Error$Playlist is currently used by played recording"));
    return osContinue;
  }
  if (playlist && playlist->RecordCol2())
  {
    if (Interface->Confirm(tr("Edit$Delete Playlist?")))
    {
      if (verbose.u)
        isyslog("%s: Playlist %d deleted", plugin_name, playlist->Index() + 1);
      playlist->RecordCol2()->DeleteFile();
      PlaylistCol.Del(playlist);
      PlaylistCol.Save();
      cOsdMenu::Del(Current());
      Display();
    }
  }
  return osContinue;
}

eOSState cMenuPlaylists::EditPlaylist(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylists::EditPlaylist", plugin_name);
#endif
  if (HasSubMenu() || !CurrentPlaylist())
    return osContinue;
  isyslog("%s: editing Playlist %d", plugin_name, CurrentPlaylist()->Index() + 1);
  cPlaylist *playlist = CurrentPlaylist();
  if (playlist)
    playlist->SearchRecords();
  return AddSubMenu(new cMenuPlaylist(playlist));
}

eOSState cMenuPlaylists::EditOption(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylists::EditOption", plugin_name);
#endif
  if (HasSubMenu() || !CurrentPlaylist())
    return osContinue;
  isyslog("%s: editing option Playlist %d", plugin_name, CurrentPlaylist()->Index() + 1);
  return AddSubMenu(new cMenuPlaylistEdit(CurrentPlaylist()));
}

cPlaylist *cMenuPlaylists::CurrentPlaylist(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuPlaylists::CurrentPlaylist", plugin_name);
#endif
  cMenuPlaylistsItem *item = (cMenuPlaylistsItem *)Get(Current());
  return item ? item->Playlist() : NULL;
}

eOSState cMenuPlaylists::ProcessKey(eKeys Key)
{
#ifdef PL_Debug3
  bool noneKey = Key == kNone;
  if (!noneKey)
    dsyslog("%s: cMenuPlaylists::ProcessKey Key=%s", plugin_name, KeyName(Key));
#endif
  cOsdItem *Item;
  int n;
  int nPlaylists = PlaylistCol.Count();
  bool hSubMenu = HasSubMenu();

  eOSState state = cOsdMenu::ProcessKey(Key);
#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
  {
    dsyslog("%s: cMenuPlaylists::ProcessKey OSState=%s", plugin_name, OSStateName(state));
    noneKey = false;
  }
#endif

  if (!HasSubMenu() && hSubMenu)
  {
    if (nPlaylists == PlaylistCol.Count())
    {
      Item = Get(Current());
      RefreshCurrent();
    } else
      Add((Item = new cMenuPlaylistsItem(PlaylistCol.Get(nPlaylists))));
    Sort();
    SetCurrent(Item);
    cPlaylist *playlist = CurrentPlaylist();
    if (playlist)
    {
      playlist->SearchRecords();
      RefreshCurrent();
      Display();
      playlist->SaveRecordCol();
    } else
    {
      RefreshCurrent();
      Display();
    }
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
                       case kOk:     state = okstartplay.u ? Play() : EditPlaylist();
                                     break;
                       case kRed:    switch (helpkeys)
                                     {
                                       case 1: okstartplay.u ? EditPlaylist() : state = Play(); break;
                                       case 2: helpkeys = 1; break;
                                     }
                                     break;
                       case kGreen:  switch (helpkeys)
                                     {
                                       case 1: state = New(); break;
                                       case 2: state = okstartplay.u ? osUnknown : EditPlaylist(); break;
                                     }
                                     break;
                       case kYellow: switch (helpkeys)
                                     {
                                       case 1: state = Delete(); break;
                                       case 2: state = EditOption(); break;
                                     }
                                     break;
                       case kBlue:   switch (helpkeys)
                                     {
                                       case 1: helpkeys = 2; break;
                                       case 2: break;
                                     }
                                     break;
                       case kUser1:  state = EditPlaylist();
                                     break;
                       case kUser2:  state = Play();
                                     break;
                       case kUser3:  state = New();
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
    dsyslog("%s: cMenuPlaylists::ProcessKey returned OSState=%s", plugin_name, OSStateName(state));
#endif
  return state;
}
