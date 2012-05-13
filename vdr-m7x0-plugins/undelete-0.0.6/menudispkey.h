/*
 * autotimeredit: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menudispkey.h 0.5 2005/11/17 21:05:05 hflor Exp $
 */

#ifndef __MENUDISPKEY_H
#define __MENUDISPKEY_H

#include "vdrtools.h"
#include <vdr/menuitems.h>

// --- cMenuDispKeyItem -----------------------------------------------------

class cMenuDispKeyItem : public cOsdItem {
private:
  int function;
public:
  cMenuDispKeyItem(const char *Key, const char *FunctionName, int Function);
  int Function(void) { return function; }
  };

class cMenuDispKey : public cOsdMenu {
private:
  void SetHelpKeys(void);
  void Set(void);
public:
#ifdef UND_Debug
  int objID;  
#endif
  cMenuDispKey(void);
  ~cMenuDispKey(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__MENUDISPKEY_H
