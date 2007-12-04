/*
 * autotimeredit: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menueditkeys.h 0.4 2005/11/16 18:39:18 hflor Exp $
 */

#ifndef __MENUEDITKEYS_H
#define __MENUEDITKEYS_H

#include <vdr/menuitems.h>

// --- cMenuEditKeysItem -----------------------------------------------------

class cMenuEditKeysItem : public cMenuEditStraItem {
public:
  cMenuEditKeysItem(const char *Name, int *Value);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__MENUEDITKEYS_H
