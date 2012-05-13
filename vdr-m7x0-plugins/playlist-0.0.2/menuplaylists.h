/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menuplaylists.h 0.1 2004/09/23 23:42:32 hflor Exp $
 */

#ifndef __MENUPLAYLISTS_H
#define __MENUPLAYLISTS_H

#include "dataplaylist.h"
#include <vdr/osd.h>
#include <vdr/menu.h>

// --- cMenuPlaylistEdit --------------------------------------------------------

class cMenuPlaylistEdit : public cOsdMenu {
private:
  cPlaylist *playlist;
  cPlaylist data;
  bool addIfConfirmed;
  eOptions displayoption[MAXOptionLines];
public:
  cMenuPlaylistEdit(cPlaylist *Playlist, bool New = false);
  virtual ~cMenuPlaylistEdit();
  virtual void Set(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

// --- cMenuPlaylistsItem --------------------------------------------------------

class cMenuPlaylistsItem : public cOsdItem {
private:
  cPlaylist *playlist;
public:
  cMenuPlaylistsItem(cPlaylist *Playlist);
  virtual ~cMenuPlaylistsItem();
  virtual bool operator< (const cListObject &ListObject);
  virtual void Set(void);
  cPlaylist *Playlist(void) { return playlist; }
  };

// --- cMenuPlaylists -----------------------------------------------------------

class cMenuPlaylists : public cOsdMenu {
private:
  int helpkeys;
  void SetHelpKeys(void);
  eOSState Play(void);
  eOSState New(void);
  eOSState Delete(void);
  eOSState EditPlaylist(void);
  eOSState EditOption(void);
  cPlaylist *CurrentPlaylist(void);
public:
  cMenuPlaylists(void);
  virtual ~cMenuPlaylists();
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__MENUPLAYLISTS_H
