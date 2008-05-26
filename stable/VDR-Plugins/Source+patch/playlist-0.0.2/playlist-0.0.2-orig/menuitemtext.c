/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menuitemtext.c 0.1 2004/09/23 23:42:32 hflor Exp $
 */

#include "menuitemtext.h"
#include <vdr/menuitems.h>

// --- cMenuText -------------------------------------------------------------

cMenuItemText::cMenuItemText(const char *Title, const char *Text, eDvbFont Font)
:cOsdMenu(Title)
{
  Add(new cMenuTextItem(Text, 1, 2, Setup.OSDwidth - 2, MAXOSDITEMS, clrWhite, clrBackground, Font));
}

eOSState cMenuItemText::ProcessKey(eKeys Key)
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
