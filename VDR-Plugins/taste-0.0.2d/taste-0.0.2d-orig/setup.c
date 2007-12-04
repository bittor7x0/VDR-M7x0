/*
 * $Id: setup.c,v 1.1.1.1 2004/11/19 16:45:31 lordjaxom Exp $
 */

#include "setup.h"

#include <vdr/interface.h>

cMenuSetupTaste::cMenuSetupTaste():
    cMenuSetupPage()
{
  mEventsData = EventsTaste; // need to use copy constructor
  mSetupData  = SetupTaste;
  Set();
}

void cMenuSetupTaste::Set(void) {
  int current = Current();
  cOsdItem *item;

  Clear();

  Add(new cMenuEditBoolItem(tr("Hide Mainmenu Entry"), &mSetupData.HideMenuEntry));
  Add(new cMenuEditIntItem(tr("Message Timeout [s]"),  &mSetupData.MessageTimeout, 0, 10));

  item = new cOsdItem("");
  item->SetSelectable(false);
  Add(item);

  item = new cOsdItem(tr("--- Keywords -------------------------------------------------------------------"));
  item->SetSelectable(false);
  Add(item);

#define NONKEYWORDITEMS 4

  int index = 0;
  cEventTaste *event = mEventsData.First();
  while (event != NULL) {
    Add(new cOsdItem(event->Pattern()));
    event = mEventsData.Next(event);
    ++index;
  }

  SetCurrent(Get(current));
  SetHelpKeys();
  Display();
}

void cMenuSetupTaste::SetHelpKeys(void)
{
  const char *red    = NULL;
  const char *yellow = NULL;

  printf("sethelpkeys, current = %d\n", Current());

  if (Current() >= NONKEYWORDITEMS) {
	red    = tr("Edit");
	yellow = tr("Delete");
  }
  SetHelp(red, tr("New"), yellow, NULL);
}

void cMenuSetupTaste::Store(void) 
{
  EventsTaste = mEventsData;
  EventsTaste.Save();

  SetupTaste  = mSetupData;
  SetupStore("HideMenuEntry",  SetupTaste.HideMenuEntry);
  SetupStore("MessageTimeout", SetupTaste.MessageTimeout);
}

eOSState cMenuSetupTaste::Edit(void)
{
  if (HasSubMenu() || Current() < NONKEYWORDITEMS)
    return osContinue;

  cEventTaste *event = mEventsData.Get(Current() - NONKEYWORDITEMS);
  if (event != NULL)
    return AddSubMenu(new cMenuSetupEditTaste(event));
  return osContinue;
}

eOSState cMenuSetupTaste::New(void)
{
  if (HasSubMenu())
    return osContinue;

  mEventsData.Add(new cEventTaste());
  Set();
  return osContinue;
}

eOSState cMenuSetupTaste::Delete(void)
{
  if (HasSubMenu() || Current() < NONKEYWORDITEMS)
    return osContinue;
        
  cEventTaste *event = mEventsData.Get(Current() - NONKEYWORDITEMS);
  if (event != NULL) {
	if (Interface->Confirm(tr("Delete keyword?")))
	  mEventsData.Del(event);
  }
  Set();
  return osContinue;
}

eOSState cMenuSetupTaste::ProcessKey(eKeys Key) {
  bool hadSubMenu = HasSubMenu();
  eOSState state = cMenuSetupPage::ProcessKey(Key);

  if (hadSubMenu && !HasSubMenu()) {
		Set();
		return state;
  }

  switch (state) {
  case osUnknown: // normal key handling
    switch (Key) {
    case kRed:    return Edit();
    case kGreen:  return New();
    case kYellow: return Delete();

    default:
      break;
    }
    break;

  default:
    break;
  }
	
	if (!HasSubMenu())
		Set();

  return state;
}

static const char *ALLOWEDCHARS = "$ abcdefghijklmnopqrstuvwxyz0123456789-_.#~/[]{}()+*^$";

cMenuSetupEditTaste::cMenuSetupEditTaste(cEventTaste *Event):
    cOsdMenu("", 33),
    mEvent(Event),
    mData(*Event)
{
  char buf[80];
  snprintf(buf, sizeof(buf), "%s - %s '%s'", tr("Setup"), tr("Plugin"), "taste");
  SetTitle(buf);
  Add(new cMenuEditStrItem(tr("Pattern"), mData.mPattern, sizeof(mData.mPattern), tr(ALLOWEDCHARS)));
	Add(new cMenuEditBoolItem(tr("Regular Expression"), &mData.mRegularExp));
	Add(new cMenuEditBoolItem(tr("Ignore Case"), &mData.mIgnoreCase));
}

eOSState cMenuSetupEditTaste::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
    case kOk:
			if (!mData.Compile()) {
				Skins.Message(mtError, tr("Malformed regular expression!"));
				state = osContinue;
			} else {
				*mEvent = mData;
				state = osBack;
			}
      break;

    default:
      break;
    }
  }
  return state;
}
