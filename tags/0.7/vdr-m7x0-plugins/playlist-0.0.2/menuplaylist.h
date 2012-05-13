/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menuplaylist.h 0.1 2004/09/23 23:42:32 hflor Exp $
 */

#ifndef __MENUPLAYLIST_H
#define __MENUPLAYLIST_H

#include "dataplaylist.h"
#include <vdr/menu.h>
#include <vdr/menuitems.h>

// --- cMenuRecordingSelectItem --------------------------------------------------------

class cMenuRecordingSelectItem : public cOsdItem {
private:
  char *filename;
  char *name;
  time_t start;
  bool isdir;
  bool ismark;
  int totalEntries;
  int newEntries;
public:
  cMenuRecordingSelectItem(cRecording *Recording, int Level);
  ~cMenuRecordingSelectItem();
  virtual bool operator< (const cListObject &ListObject);
  void IncrementCounter(bool New);
  const char *FileName(void) { return filename; }
  const char *Name(void) { return name; }
  bool IsDirectory(void) { return isdir; }
  bool IsMark(void) { return ismark; }
  void SetMark(bool Mark) { ismark = Mark; }
  };

// --- cMenuRecordingSelect --------------------------------------------------------

class cMenuRecordingSelect : public cOsdMenu {
private:
  char *base;
  int level;
  bool singleselect;
  void SetHelpKeys(void);
  cRecording *GetRecording(cMenuRecordingSelectItem *Item);
  eOSState Open(bool OpenSubMenus = false);
  eOSState AddPlaylist(void);
  eOSState MarkSel(void);
  eOSState Delete(void);
  eOSState Summary(void);
public:
  cMenuRecordingSelect(const char *Base = NULL, int Level = 0, bool OpenSubMenus = false, bool SingleSelect = false);
  ~cMenuRecordingSelect();
  virtual eOSState ProcessKey(eKeys Key);
  };

// --- cMenuRecordingEdit --------------------------------------------------------

class cMenuRecordingEdit : public cOsdMenu {
private:
  cPlaylistRecord *playlistrecord;
  cPlaylistRecord data;
  bool confirmoptions;
  eOptions displayoption[MAXOptionLines];
#if VDRVERSNUM >= 10307
  int editwidth;
#else
  int firstline;
#endif
  void SetHelpKeys(void);
  eOSState Summary(void);
public:
  cMenuRecordingEdit(cPlaylistRecord *PlaylistRecord, bool ConfirmOptions = false);
  virtual ~cMenuRecordingEdit();
  virtual void Set(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

// --- cMenuPlaylistItem --------------------------------------------------------

class cMenuPlaylistItem : public cOsdItem {
private:
  cPlaylistRecord *playlistrecord;
public:
  cMenuPlaylistItem(cPlaylistRecord *PlaylistRecord);
  virtual ~cMenuPlaylistItem();
  virtual void Set(void);
  cPlaylistRecord *PlaylistRecord(void) { return playlistrecord; }
  };

// --- cMenuPlaylist -----------------------------------------------------------

class cMenuPlaylist : public cOsdMenu {
private:
  cPlaylist *playlist;
  bool addrecords;
  void SetHelpKeys(void);
  bool AddPlaylistentrys(void);
  eOSState Edit(void);
  eOSState Play(void);
  eOSState New(void);
  eOSState Delete(void);
  cPlaylistRecord *CurrentPlaylistRecord(void);
public:
  cMenuPlaylist(cPlaylist *Playlist);
  virtual ~cMenuPlaylist();
  virtual void Move(int From, int To);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__MENUPLAYLIST_H
