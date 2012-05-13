/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menuitemtext.h 0.1 2004/09/23 23:42:32 hflor Exp $
 */

#ifndef __MENUITEMTEXT_H
#define __MENUITEMTEXT_H

#include <vdr/osd.h>

// --- cMenuText -------------------------------------------------------------

class cMenuItemText : public cOsdMenu {
public:
  cMenuItemText(const char *Title, const char *Text, eDvbFont Font = fontOsd);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif // __MENUITEMTEXT_H
