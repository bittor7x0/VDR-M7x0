/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menusetup.h 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#ifndef __MENUSETUP_H
#define __MENUSETUP_H

#include <vdr/menu.h>
#include <vdr/menuitems.h>

// --- cMenuSetupPlayList -------------------------------------------------------

class cMenuSetupPlayList : public cMenuSetupPage {
private:
  int   store;
  int   fileitempos;
  int   changedefault;
  int   changechar;
protected:
  virtual void Store(void);
public:
  cMenuSetupPlayList(void);
  virtual ~cMenuSetupPlayList(void);
  virtual void Set(bool sel = false);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__MENUSETUP_H
