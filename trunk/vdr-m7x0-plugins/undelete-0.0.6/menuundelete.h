/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menuundelete.h 0.4 2005/11/16 18:39:18 hflor Exp $
 */

#ifndef __MENUUNDELETE_H
#define __MENUUNDELETE_H

#include "vdrtools.h"
#include <vdr/menu.h>
#include <vdr/menuitems.h>
#include <vdr/recording.h>

#ifndef RECEXT
  #define RECEXT       ".rec"
  #define DELEXT       ".del"
#endif

// --- cMenuRecordingSelectItem --------------------------------------------------------

class cMenuRecordingSelectItem : public cOsdItem {
private:
  char *filename;
  char *name;
  int level;
  time_t start;
  bool isdir;
  int totalEntries;
  int newEntries;
public:
#ifdef UND_Debug
  int objID;  
#endif
  cMenuRecordingSelectItem(cRecording *Recording, int Level);
  ~cMenuRecordingSelectItem(void);
#if VDRVERSNUM >= 10315
  virtual int compare(const cListObject &ListObject) const;
#else
  virtual bool operator< (const cListObject &ListObject);
#endif
  void IncrementCounter(bool New);
  void RefreshCounter(void);
  const char *FileName(void) { return filename; }
  char *DirName(bool Parent = false);
  const char *Name(void) { return name; }
  bool IsDirectory(void) { return isdir; }
  };

// --- cMenuRecordingSelect --------------------------------------------------------

class cMenuRecordingSelect : public cOsdMenu {
private:
  char *base;
  int level;
  bool top_line[2];
  bool bottom_line[2];

  bool IsMenuRecordingSelectItem(cOsdItem *Item);
  bool IsDirectory(cOsdItem *Item);
  bool IsRecording(cOsdItem *Item);
  bool IsReady(void);
  cRecording *GetRecording(cOsdItem *Item);
  void AddFunctionLines(void);
  void DelFunctionLines(void);
  void SetHelpKeys(void);
  eOSState FunctionCall(int FunctionNumber);
public:
#ifdef UND_Debug
  int objID;  
#endif
  cMenuRecordingSelect(const char *Base = NULL, int Level = 0);
  ~cMenuRecordingSelect(void);
  virtual eOSState ProcessKey(eKeys Key);
  };
  
// --- cMenuUndelete --------------------------------------------------------

class cMenuUndelete : public cOsdMenu {
private:
  void FillMenu(void);
public:
#ifdef UND_Debug
  int objID;  
#endif
  cMenuUndelete(void);
  ~cMenuUndelete(void);
  virtual eOSState ProcessKey(eKeys Key);
  };


#endif //__MENUUNDELETE_H
