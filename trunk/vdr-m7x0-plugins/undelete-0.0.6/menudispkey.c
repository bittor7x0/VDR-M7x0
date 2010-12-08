/*
 * autotimeredit: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menueditkeys.c 0.5 2005/11/17 21:05:05 hflor Exp $
 */

#include "menudispkey.h"
#include "undelete.h"
#include "menunoselectitem.h"
#include "menusetup.h"
#include "i18n.h"
#include <vdr/osd.h>
#include <vdr/osdbase.h>
#include <typeinfo>

// --- cMenuDispKeyItem -----------------------------------------------------

cMenuDispKeyItem::cMenuDispKeyItem(const char *Key, const char *FunctionName, int Function)
{
  function = Function;
  char *temp;
  asprintf(&temp, "%s\t%s", Key, FunctionName);
  SetText(temp, false);
}

// --- cMenuDispKey -----------------------------------------------------

cMenuDispKey::cMenuDispKey(void)
:cOsdMenu(tr("Menu$assigned keys"), 15)
{
#ifdef UND_Debug
  objID = ++cMenuDispKey_nextID;
#endif
  d2syslog("cMenuDispKey::cMenuDispKey", "%s", "");
  Set();
  SetHelpKeys();
}

cMenuDispKey::~cMenuDispKey()
{
  d2syslog("cMenuDispKey::~cMenuDispKey", "%s", "");
}

  
void cMenuDispKey::SetHelpKeys(void)
{
  const char *helptext[4] = { NULL, NULL, NULL, NULL };
  helptext[0] = tr("Edit");
  d2syslog("cMenuDispKey::SetHelpKeys", "red=%s green=%s yellow=%s blue=%s", helptext[0] ? helptext[0] : "(NULL)", helptext[1] ? helptext[1] : "(NULL)", helptext[2] ? helptext[2] : "(NULL)", helptext[3] ? helptext[3] : "(NULL)");
  SetHelp(helptext[0], helptext[1], helptext[2], helptext[3]);
}

void cMenuDispKey::Set(void)
{
  int current = Current();
  d2syslog("cMenuDispKey::Set", "Current=%d", current);
  Clear();

  Add(new cMenuOsdNoSelectItem(tr("Help$key\tfunction")));
  for (int i = 1; i < MaxFunctionName; i++)
  {
    if (functionkey[i].u > 0)
      Add(new cMenuDispKeyItem(KeysName[functionkey[i].u], FunctionName[i], i));
    if (okkey.u == i)
      Add(new cMenuDispKeyItem(tr("Help$OK"), FunctionName[i], i));
    if (numkey.u == i)
      Add(new cMenuDispKeyItem(tr("Help$numerickey"), FunctionName[i], i));
  }
  d1listlog("cMenuDispKey::Set");
  d2syslog("cMenuDispKey::Set", "Count=%d", Count());
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuDispKey::ProcessKey(eKeys Key)
{
  bool hSubMenu = HasSubMenu();
  cOsdItem *item = NULL;
  int function = 99; // dummy function for display change key in setupmenu

  d3ProcessKey1("cMenuDispKey::ProcessKey");
  eOSState state = cOsdMenu::ProcessKey(Key);
  d3ProcessKey2("cMenuDispKey::ProcessKey");

  if (hSubMenu && !HasSubMenu())
    Set();

  switch (state)
  {
    case osUnknown: switch (Key)
                    {
                      case kRed:            item = Get(Current());
                                            if (item && typeid(*item) == typeid(cMenuDispKeyItem))
                                              function = ((cMenuDispKeyItem *)item)->Function();
                                            {
                                              cMenuSetupPage *menu = new cMenuSetupUndelete(function);
                                              if (menu)
                                              {
                                                menu->SetPlugin(plugin);
                                                state = AddSubMenu(menu);
                                              }
                                            }
                                            break;
                      default:              break;
                    }
                    break;
    default:        break;
  }
  d3ProcessKey3("cMenuDispKey::ProcessKey");
  return state;
}
