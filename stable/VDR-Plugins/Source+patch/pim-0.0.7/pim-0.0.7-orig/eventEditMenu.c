/*
 * $Id: eventEditMenu.c,v 1.1.1.1 2005/02/17 05:45:52 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: eventEditMenu.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:52  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.3  2004/11/24 21:29:51  ksltuf
 * Changed order of include-Statements - VDR-includes come last
 * (problems with STL under gcc 2.95)
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#include "eventEditMenu.h"
#include "calManager.h"
#include "calEvent.h"
#include "i18n.h"

#include <vdr/menuitems.h>
#include <vdr/tools.h>

cEventEditMenu::cEventEditMenu(cCalEvent* ev, bool insert) :
 cOsdMenu (tr("Edit Event"), 24), _newEvent(insert), _ev(ev)
{
   if (_ev)
   {
      // deep copy
      _data = *_ev;
      if (_newEvent)
      {
	SetTitle(tr("New Event"));
      }

      _type = (int)_ev->_type;
      _last_type = _type;

      _calTypeTexts[0] = tr("Fix Day");
      _calTypeTexts[1] = tr("Weekly");
      _calTypeTexts[2] = tr("Yearly");

      _weekDayTexts[0] = tr("Monday");
      _weekDayTexts[1] = tr("Tuesday");
      _weekDayTexts[2] = tr("Wednesday");
      _weekDayTexts[3] = tr("Thursday");
      _weekDayTexts[4] = tr("Friday");
      _weekDayTexts[5] = tr("Saturday");
      _weekDayTexts[6] = tr("Sunday");

      Add(new cMenuEditStraItem(tr("Event type"), &_type, 3, _calTypeTexts));

      Add((_menu_day = new cMenuEditIntItem(tr("Day"), &_data._day, 1, 31)));
      Add((_menu_month = new cMenuEditIntItem(tr("Month"), &_data._month, 1, 12)));
      Add((_menu_year = new cMenuEditIntItem(tr("Year"), &_data._year, 1970, 2100)));

      _data._dayOfWeek--;	// saved: 1..7, in menu: 0..6
      Add((_menu_dayOfWeek = new cMenuEditStraItem(tr("Day Of Week"),
			       &_data._dayOfWeek, 7, _weekDayTexts)));

      _remind = (_data._reminderDays < 0) ? 0 : 1;
      Add(new cMenuEditBoolItem(tr("Remind me"), &_remind, tr("off"), tr("on")));
      _last_remind = _remind;

      if (!_remind) _data._reminderDays = 0;
      Add((_menu_reminderDays = new cMenuEditIntItem(tr("Remind days before"), &_data._reminderDays, 0, 30)));

      Add(new cMenuEditStrItem(tr("Description"), _data._text, MAX_EVENT_NAME, tr(FileNameChars)));

      _infoItem = NULL;
      setItemType(_type);
      setRemindType(_remind);
      Display();
   }
}

cEventEditMenu::~cEventEditMenu()
{
  if (_ev && _newEvent)
  {
     delete _ev;     // NEW was pressed, but not confirmed
  }
}

void cEventEditMenu::setRemindType(int remind)
{
  _menu_reminderDays->SetSelectable((remind == 1));
  _last_remind = remind;
}

void cEventEditMenu::setItemType(int type)
{
  _menu_day->SetSelectable((type == evWeekly) ? false : true);
  _menu_month->SetSelectable((type == evWeekly) ? false : true);
  _menu_year->SetSelectable((type == evFixedDay) ? true : false);
  _menu_dayOfWeek->SetSelectable((type == evWeekly) ? true : false);
  _last_type = type;
}

bool cEventEditMenu::updateEvent()
{
  if (_ev)
  {
    if ((memcmp(_ev, &_data, sizeof(_data)) != 0) || _type != _ev->_type)
    {
      // use event only if it is valid
      size_t len = 0;
      if (!_data.isValid((eCalEventType)_type) ||
	  ((len = strlen(_data._text)) == 0) ||
	  (strspn(_data._text, " ") == len))
      {
	dsyslog("PIM: data (type=%d) invalid (%d.%d.%d/%d/%s)", _type, _data._day, _data._month,
		_data._year, _data._dayOfWeek, _data._text);
	if (!_infoItem)
	  Add(_infoItem = new cOsdItem(tr("--- Data invalid ! ---"), osUnknown, false));
        return false;
      }

      *_ev = _data;	// deep copy
      _ev->_type = (eCalEventType)_type;
      _ev->_dayOfWeek++;	// saved: 1..7, in menu: 0..6

      if (!_remind)
	_ev->_reminderDays = -1;

      // Reminder days > 6 makes no sense for weekly events as a week
      // has only 7 days. So simply reduce to 6.
      // You got always a reminder each day.
      if ((_ev->_type == evWeekly) && (_ev->_reminderDays > 6))
	_ev->_reminderDays = 6;

      cCalManager* mgr = cCalManager::getInstance();
      if (_newEvent)
	mgr->add(_ev);

      mgr->save();
      _newEvent = false;
    }
  }
  return true;
}


eOSState cEventEditMenu::ProcessKey(eKeys key)
{
  bool do_refresh = false;
  eOSState state = cOsdMenu::ProcessKey(key);

  if (state == osUnknown)
  {
     switch (key)
     {
       case kOk:
            if ((do_refresh = !updateEvent())) {
	      state = osContinue;
	      break;
	    }
            return osBack;

       case kRed:
       case kGreen:
       case kYellow:
       case kBlue:   return osContinue;

       default:
	    break;
     }
  }
  else if ((state == osContinue) && (key != kNone)) {
    if ((_type != _last_type) || (_remind != _last_remind)) {
      if (_type != _last_type)
        setItemType(_type);
      if (_remind != _last_remind)
        setRemindType(_remind);
      do_refresh = true;
    }
    if (_infoItem) {
      Del(_infoItem->Index());
      _infoItem = NULL;
      do_refresh = true;
    }
  }

  if (do_refresh) {
    RefreshCurrent();
    Display();
  }

  return state;
}

