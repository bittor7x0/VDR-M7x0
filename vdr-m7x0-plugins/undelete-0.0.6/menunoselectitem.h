/*
 * autotimeredit: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menunoselectitem.h 0.5 2005/11/17 21:05:05 hflor Exp $
 */

#ifndef __MENUNOSELECTITEM_H
#define __MENUNOSELECTITEM_H

#include <vdr/osd.h>
#include <vdr/config.h>
#ifdef HAVE_ELCHI
  #include <vdr/theme.h>
#endif

// --- cMenuOsdNoSelectItem -----------------------------------------------------

class cMenuOsdNoSelectItem : public cOsdItem {
public:
#if VDRVERSNUM >= 10307
  cMenuOsdNoSelectItem(const char *Text):cOsdItem(Text) { SetSelectable(false); }
#else
  #ifdef HAVE_ELCHI
  cMenuOsdNoSelectItem(const char *Text):cOsdItem(Text) { SetColor(Setup.Theme == themeVanilla ? clrCyan : clrScrolLine, clrBackground); }
  #else
  cMenuOsdNoSelectItem(const char *Text):cOsdItem(Text) { SetColor(clrCyan, clrBackground); }
  #endif
#endif
  };

#endif //__MENUNOSELECTITEM_H
