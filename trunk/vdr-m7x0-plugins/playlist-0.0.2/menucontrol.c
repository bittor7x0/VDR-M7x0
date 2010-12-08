/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menucontrol.c 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#include "menucontrol.h"
#include "menuplaylists.h"
#include "playlist.h"
#include "vdrtools.h"
#include <vdr/recording.h>
#include <vdr/remote.h>
#include <vdr/interface.h>

// --- cControlPlaylist -----------------------------------------------------------

cControlPlaylist::cControlPlaylist(void)
:cControl(NULL)
{
#ifdef PL_Debug2
  dsyslog("%s: cControlPlaylist::cControlPlaylist", plugin_name);
#endif

  replaycontrol = NULL;
  if (SelectPLR)
    PlayRecording(SelectPLR);
  else
    cRemote::Put(kBack);
}

cControlPlaylist::~cControlPlaylist(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cControlPlaylist::~cControlPlaylist", plugin_name);
#endif
  if (replaycontrol)
    DELETENULL(replaycontrol);
  PlayedPLR = NULL;
#ifdef PL_Debug2
  dsyslog("%s: PlayedPLR=(null)", plugin_name);
#endif
}

void cControlPlaylist::Hide(void)
{
  if (replaycontrol)
    replaycontrol->Hide();
}

eOSState cControlPlaylist::ProcessKey(eKeys Key)
{
#ifdef PL_Debug3
  bool noneKey = Key == kNone;
  if (!noneKey)
    dsyslog("%s: cControlPlaylist::ProcessKey Key=%s", plugin_name, KeyName(Key));
#endif

  eOSState state = osUnknown;
  if (replaycontrol)
    state = replaycontrol->ProcessKey(Key);
#ifdef PL_Debug3
  if (!noneKey || state != osUnknown)
  {
    dsyslog("%s: cControlPlaylist::ProcessKey OSState=%s", plugin_name, OSStateName(state));
    noneKey = false;
  }
#endif

  char *temp = NULL;
  cPlaylistRecord *plr = PlayedPLR;
  cPlaylist *playlist = plr ? plr->Playlist() : NULL;
  switch (state)
  {
    case osUnknown:     switch (NORMALKEY(Key))
                        {
                          case k1:      if (playlist)
                                        {
                                          if (plr)
                                            plr = playlist->Prev_PLR(plr);
                                          while (plr && (plr->IsDirOrDel()))
                                            plr = playlist->Prev_PLR(plr);
                                          if (!plr && Interface->Confirm(tr("Play$Currently play first Record! Jump to last?")))
                                          {
                                            plr = playlist->Last_PLR();
                                            while (plr && (plr->IsDirOrDel()))
                                              plr = playlist->Prev_PLR(plr);
                                          }
                                        }
                                        state = osContinue;
                                        break;
                          case k3:      if (playlist)
                                        {
                                          if (plr)
                                            plr = playlist->Next_PLR(plr);
                                          while (plr && (plr->IsDirOrDel()))
                                            plr = playlist->Next_PLR(plr);
                                          if (!plr && Interface->Confirm(tr("Play$Currently play last Record! Jump to first?")))
                                          {
                                            plr = playlist->First_PLR();
                                            while (plr && (plr->IsDirOrDel()))
                                              plr = playlist->Next_PLR(plr);
                                          }
                                        }
                                        state = osContinue;
                                        break;
                          case kBack:   state = osEnd;
                                        break;
                          default:      break;
                        }
                        if (state == osContinue && plr)
                        {
                          bool visible = replaycontrol ? replaycontrol->Visible() : false;
                          UpdateRecording();
                          state = PlayRecording(plr, visible);
                        }
                        break;
    case osRecordings:  // kBack while playing
                        UpdateRecording();
                        cControl::Shutdown();
                        if (ControlMenuIsOpen)
                          state = osUser8;
                        else
                        {
                          if (playlist)
                            playlist->SaveRecordCol();
                          state = osEnd;
                        }
                        break;
    case osEnd:         // end of playing
                        {
                          bool visible = replaycontrol ? replaycontrol->Visible() : false;
                          UpdateRecording();
                          if (playlist)
                          {
                            cPlaylistRecord *plr_n = playlist->Next_PLR(plr);
                            asprintf(&temp, "%s %s", tr("Play$Delete recording:"), plr->Name());
                            if (deleterecords.u && plr->Option(Option_deleterecord) && (!plr->Option(Option_confirmdeleterecord) || Interface->Confirm(temp)))
                              plr->DeleteRecord();
                            else if (plr->Option(Option_deletefromlist))
                            {
                              plr->SetDel();
                              playlist->SetChange();
                            }
                            FREENULL(temp);
                            plr = plr_n;
                            while (plr && (plr->IsDirOrDel() || (plr->Option(Option_playonlynew) && !plr->IsNew())))
                              plr = playlist->Next_PLR(plr);
                            if (plr)
                            {
                              asprintf(&temp, "%s %s", tr("Play$Start next recording:"), plr->Name());
                              if (!plr->Option(Option_confirmstartnewrecord) || Interface->Confirm(temp))
                                state = PlayRecording(plr, visible);
                              else
                                plr = NULL;
                              FREENULL(temp);
                            }
                          } else
                            plr = NULL;
                          if (!plr)
                          {
                            if (playlist)
                              playlist->SaveRecordCol();
                            cControl::Shutdown();
                          }
                        }
                        break;
    default:            break;
  }
#ifdef PL_Debug3
  if (!noneKey || state != osUnknown)
    dsyslog("%s: cControlPlaylist::ProcessKey returned OSState=%s", plugin_name, OSStateName(state));
#endif
  return state;
}

eOSState cControlPlaylist::PlayRecording(cPlaylistRecord *PlaylistRecord, bool Visible)
{
#ifdef PL_Debug1
  dsyslog("%s: cControlPlaylist::PlayRecording Name=%s FileName=%s", plugin_name, PlaylistRecord ? PlaylistRecord->Name() : NULL, PlaylistRecord ? PlaylistRecord->Filename() : NULL);
#endif

  static char title[MaxFileName];
  eOSState state = osEnd;
  DELETENULL(replaycontrol);
  if (PlaylistRecord)
  {
    if (PlaylistRecord->IsNew() && !PlaylistRecord->IsEdited() && PlaylistRecord->Option(Option_jumpmark))
    {
      cResumeFile *resume = new cResumeFile(PlaylistRecord->Filename());
      int res = resume->Read();
      delete resume;
      if (res < 0) // new file
      {
        cMarks *marks = new cMarks;
        if (marks)
        {
          marks->Load(PlaylistRecord->Filename());
          if (marks->Count())
          {
            int mark = marks->First()->position;
            if (mark < 100 && marks->Count() > 2)
            {
              mark = marks->GetNext(mark)->position;
              mark = marks->GetNext(mark)->position;
            }
            cResumeFile *resume = new cResumeFile(PlaylistRecord->Filename());
            if (mark > 0 && resume)
            {
              resume->Save(mark);
              DELETENULL(resume);
            }
          }
        }
      }
    }
    char *buffer;
    if (PlaylistRecord->Playlist())
    {
      asprintf(&buffer, "%d/%d %s", PlaylistRecord->IndexRec(), PlaylistRecord->Playlist()->RecordCount(true), PlaylistRecord->Name());
      PlaylistRecord->Playlist()->SetLastRecord(PlaylistRecord->Filename());
      PlaylistCol.Save();
    } else
      buffer = strdup(PlaylistRecord->Name());
    strn0cpy(title, buffer, MaxFileName);
    free(buffer);
    if (verbose.u)
      dsyslog("%s: start new replay Name=%s Directory=%s", plugin_name, title, PlaylistRecord->Filename());
    cReplayControl::SetRecording(NULL, NULL);
    cReplayControl::SetRecording(PlaylistRecord->Filename(), title);
    replaycontrol = new cReplayControlPlaylist;
    if (replaycontrol)
    {
      player = replaycontrol->GetPlayerPlaylist();
      if (cDevice::PrimaryDevice()->AttachPlayer(player))
        state = osContinue;
      PlayedPLR = PlaylistRecord;
#ifdef PL_Debug2
      dsyslog("%s: PlayedPLR=(null)", plugin_name, PlayedPLR ? PlayedPLR->Name() : "(null)");
#endif
      if (Visible)
        replaycontrol->Show();
      if (PlaylistRecord && PlaylistRecord->Playlist())
        strn0cpy(lastplaylist.u, PlaylistRecord->Playlist()->Name(), sizeof(lastplaylist.u));
      else
        lastplaylist.u[0] = 0;
      if (strcmp(lastplaylist.u, lastplaylist.o))
      {
        PluginPlaylist->SetupStore("lastpl", lastplaylist.u);
        strcpy(lastplaylist.o, lastplaylist.u);
      }
    }
  }
  return state;
}

void cControlPlaylist::UpdateRecording(void)
{
#ifdef PL_Debug1
  dsyslog("%s: cControlPlaylist::UpdateRecording PlayedPLR=%s", plugin_name, PlayedPLR ? PlayedPLR->Filename() : "(null)");
#endif

  DELETENULL(replaycontrol);
  cRecording *recording_n = NULL;
  if (PlayedPLR)
  {
    cRecording *recording_o = Recordings.GetByName(PlayedPLR->Filename());
    recording_n = new cRecording(PlayedPLR->Filename());
    if (recording_o && recording_n)
    {
      Recordings.Add(recording_n, recording_o);
      Recordings.Del(recording_o);
    }
  }
  for (cPlaylist *pl = PlaylistCol.First(); pl; pl = PlaylistCol.Next(pl))
    for (cPlaylistRecord *plr = pl->First_PLR(); plr; plr = pl->Next_PLR(plr))
      if (strcasecmp(plr->Filename(), PlayedPLR->Filename()) == 0)
{
  dsyslog("%s: playlist=%s isnew=%s", plugin_name, pl->Name(), recording_n ? recording_n->IsNew() ? "true" : "false" : "(null)");
        if (recording_n)
          plr->SetNew(recording_n->IsNew());
        else
          plr->SetDel();
}
}

// --- cControlMenu -----------------------------------------------------------

cControlMenu::cControlMenu(void)
:cOsdObject(true)
{
#ifdef PL_Debug2
  dsyslog("%s: cControlMenu::cControlMenu Startoption=%d", plugin_name, startoption.u);
#endif

  ControlMenuIsOpen = true;
  switch (startoption.u)
  {
    case Start_DisplayPL:   osdmenu = new cMenuPlaylists;
                            break;
    case Start_DisplayLast: osdmenu = new cMenuPlaylists;
                            if (SelectPLR)
                              osdmenu->ProcessKey(kUser1);
                            break;
    case Start_PlayLast:    osdmenu = new cMenuPlaylists;
                            if (SelectPLR)
                            {
                              if (osdmenu->ProcessKey(kUser2) == osUser9)
                              {
                                DELETENULL(osdmenu);
                                cControl::Shutdown();
                                cControl::Launch(new cControlPlaylist);
                              }
                            }
                            break;
    case Start_NewEmpty:    osdmenu = new cMenuPlaylists;
                            osdmenu->ProcessKey(kUser3);
                            break;
  }
}

cControlMenu::~cControlMenu()
{
#ifdef PL_Debug
  dsyslog("%s: cControlMenu::~cControlMenu", plugin_name);
#endif
  if (osdmenu)
    DELETENULL(osdmenu);
  ControlMenuIsOpen = false;
}

void cControlMenu::Show(void)
{
  if (osdmenu)
    osdmenu->Display();
}

eOSState cControlMenu::ProcessKey(eKeys Key)
{
#ifdef PL_Debug3
  bool noneKey = Key == kNone;
  if (!noneKey)
    dsyslog("%s: cControlMenu::ProcessKey Key=%s", plugin_name, KeyName(Key));
#endif

  eOSState state;
  cOsdObject *Interact = osdmenu ? (cOsdObject*)osdmenu : cControl::Control();
  if (Interact)
    state = Interact->ProcessKey(Key);
  else
    state = cOsdObject::ProcessKey(Key);

#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
  {
    dsyslog("%s: cControlMenu::ProcessKey OSState=%s", plugin_name, OSStateName(state));
    noneKey = false;
  }
#endif

  switch (state)
  {
    case osUnknown:  switch (NORMALKEY(Key))
                     {
                       case kBack:   state = osEnd;
                                     break;
                       default:      break;
                     }
                     break;
    case osUser8:    osdmenu = new cMenuPlaylists;
                     if (SelectPLR)
                       osdmenu->ProcessKey(kUser1);
                     break;
    case osUser9:    if (SelectPLR)
                     {
                       DELETENULL(osdmenu);
                       cControl::Shutdown();
                       cControl::Launch(new cControlPlaylist);
                     }
                     state = osContinue;
                     break;
    default:         break;
  }

#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
    dsyslog("%s: cControlMenu::ProcessKey returned OSState=%s", plugin_name, OSStateName(state));
#endif
  return state;
}
