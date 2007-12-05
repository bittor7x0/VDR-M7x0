/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menutext.c 0.4 2005/11/16 18:39:18 hflor Exp $
 */

#include "menutext.h"
#include <vdr/menuitems.h>

// --- cMenuText -------------------------------------------------------------

cMenuText::cMenuText(const char *Title, const char *Text, eDvbFont Font)
:cOsdMenu(Title)
{
  Add(new cMenuTextItem(Text, 1, 2, Setup.OSDwidth - 2, MAXOSDITEMS, clrWhite, clrBackground, Font));
}

eOSState cMenuText::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  switch (state)
  {
    case osUnknown:  switch (NORMALKEY(Key))
                     {
                       case kOk:     state = osBack;
                                     break;
                       default: break;
                     }
                     break;
    default:         break;
  }
  return state;
}
