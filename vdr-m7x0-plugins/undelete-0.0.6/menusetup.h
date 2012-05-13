/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menusetup.h 0.5 2005/11/17 21:05:05 hflor Exp $
 */

#ifndef __MENUSETUP_H
#define __MENUSETUP_H

#include "vdrtools.h"
#include <vdr/menu.h>
#include <vdr/menuitems.h>

// --- cMenuSetupUndelete -------------------------------------------------------

class cMenuSetupUndelete : public cMenuSetupPage {
private:
  int   show_setupfunction;
  int   store;
protected:
  virtual void Store(void);
public:
#ifdef UND_Debug
  int objID;  
#endif
  cMenuSetupUndelete(int FunctionToChange = 0);
  virtual ~cMenuSetupUndelete(void);
  virtual void Set(int FunctionToSelect = 0);
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif //__MENUSETUP_H
