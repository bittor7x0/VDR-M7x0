/*
 * $Id: eventListMenu.c,v 1.1.1.1 2005/02/17 05:45:52 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: eventListMenu.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:52  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.4  2004/11/24 21:34:35  ksltuf
 * Added new translation for "Delete Event?"
 *
 * Revision 1.3  2004/11/24 21:29:51  ksltuf
 * Changed order of include-Statements - VDR-includes come last
 * (problems with STL under gcc 2.95)
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 */


#include "eventListMenu.h"
#include "eventEditMenu.h"
#include "calEvent.h"
#include "calEventMenuItem.h"
#include "calManager.h"
#include "i18n.h"

#include <vdr/interface.h>
#include <vdr/menuitems.h>
#include <vdr/osdbase.h>


cEventListMenu::cEventListMenu(int day, int month, int year, int dayOfWeek)
  : cOsdMenu(tr("Events"), 13, 12)
{
  char* dbuf;

  asprintf (&dbuf, "%s %d-%02d-%02d", tr("Events for"), year, month, day);
  SetTitle(dbuf);
  free(dbuf);

  cCalManager* mgr = cCalManager::getInstance();
  mgr->sort();

  tEventIds ids = mgr->getEventsForDay(day, month, year, dayOfWeek);
  tEventIds::const_iterator it = ids.begin();

  char buf[256];
  while (it != ids.end())
  {
      cCalEvent *ev = mgr->getEventById(*it);
      strcpy(buf, ev->getLongText());
      cOsdMenu::Add (new cCalEventMenuItem(buf, *it));
      ++it;
  }

  _refresh = false;
  _day = day;
  _month = month;
  _year = year;
  _dayOfWeek = dayOfWeek;
  _selectedItem = 0;

  SetHelp(tr("Edit"),tr("New"),tr("Delete"), NULL);
  Display();
}


cEventListMenu::cEventListMenu() : cOsdMenu(tr("Events"), 13, 12)
{
  cCalManager* mgr = cCalManager::getInstance();
  mgr->sort();

  tEventIds ids = mgr->getAllEvents();
  tEventIds::const_iterator it = ids.begin();

  char buf[256];
  while (it != ids.end())
  {
      cCalEvent *ev = mgr->getEventById(*it);
      strcpy(buf, ev->getLongText());
      cOsdMenu::Add (new cCalEventMenuItem(buf, *it));
      ++it;
  }

  _refresh = false;
  _day = _month = _year = _dayOfWeek = 0;

  SetHelp(tr("Edit"),tr("New"),tr("Delete"), NULL);
  Display();
}


cCalEvent* cEventListMenu::getCurrentEvent()
{
  cCalEventMenuItem* item = (cCalEventMenuItem*)Get(Current());

  return item ? cCalManager::getInstance()->getEventById(item->getId()) : NULL;
}


eOSState cEventListMenu::Edit(void)
{
  cCalEvent* ev = getCurrentEvent();

  if (HasSubMenu() || !ev)
    return osContinue;

  return AddSubMenu(new cEventEditMenu(ev));
}


eOSState cEventListMenu::New(void)
{
  if (HasSubMenu())
    return osContinue;

  return AddSubMenu(new cEventEditMenu(new cCalEvent, true));
}


eOSState cEventListMenu::Delete(void)
{
  if (HasSubMenu())
    return osContinue;

  cCalEvent* ev = getCurrentEvent();
  if (ev)
  {
    if (Interface->Confirm(tr("Delete Event?")))
    {
       cCalManager::getInstance()->del(ev);
       cCalManager::getInstance()->save();
       cOsdMenu::Del(Current());
       Display();
    }
  }

  return osContinue;
}


eOSState cEventListMenu::ProcessKey(eKeys key)
{
  eOSState state = cOsdMenu::ProcessKey(key);

  if (state == osUnknown) {
    switch (key)
    {
       case kOk:     state = Edit();
       case kRed:    state = Edit();
		     _selectedItem = Current();
		     break;

       case kGreen:  state = New();
		     _selectedItem = Current();
		     break;

       case kYellow: state = Delete();
		     _selectedItem = Current();
		     break;

       default:      break;
    }
  }

  // refresh menu entries after return to list menu
  if (!HasSubMenu())
  {
    tEventIds ids;
    if (!_refresh) return state;
    while(Count()>0) Del(0);

    cCalManager* mgr = cCalManager::getInstance();

    mgr->sort();	// sort list
    if (_day == 0)
      ids = mgr->getAllEvents();
    else
      ids = mgr->getEventsForDay(_day, _month, _year, _dayOfWeek);

    tEventIds::const_iterator it = ids.begin();

    int pos = 0;
    char buf[256];
    while (it != ids.end())
    {
      cCalEvent *ev = mgr->getEventById(*it);
      strcpy(buf, ev->getLongText());
      cCalEventMenuItem *item = new cCalEventMenuItem(buf, *it);
      Add(item);

      // set cursor to last list position before opening of submenu
      if (pos == _selectedItem) SetCurrent(item);
      ++it;
      ++pos;
    }
    Display();
    _refresh = false;
  }
  else
    _refresh = true;

  return state;
}

