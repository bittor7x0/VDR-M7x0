/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menutext.h 0.4 2005/11/16 18:39:18 hflor Exp $
 */

#ifndef __MENUTEXT_H
#define __MENUTEXT_H

#include <vdr/osd.h>
#include <vdr/osdbase.h>

// --- cMenuText -------------------------------------------------------------

class cMenuText : public cOsdMenu {
public:
  cMenuText(const char *Title, const char *Text, eDvbFont Font = fontOsd);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif // __MENUTEXT_H
