/*
 * $Id: calEvent.c,v 1.1.1.1 2005/02/17 05:45:51 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: calEvent.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:51  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.2  2004/07/05 19:00:26  ksltuf
 * Added COPYRIGHT and cvs information
 *
 *
 */


#include <string>
#include <iostream>

#include "i18n.h"
#include "calEvent.h"

// static member

char* cCalEvent::_buffer = NULL;

int cCalEvent::uniqueId = 0;


cCalEvent::cCalEvent()
{
  init();
}


cCalEvent::~cCalEvent()
{
  if (_text)
    free(_text);
}


cCalEvent& cCalEvent::operator= (const cCalEvent& ev)
{
  memcpy(this, &ev, sizeof(*this));
  if (ev._text)
  {
    _text = strdup(ev._text);
  }
  return *this;
}


void cCalEvent::init()
{
  _day = 1;
  _month = 1;
  _year = 2007;
  _dayOfWeek = 1;

  _type = evFixedDay;
  _reminderDays = 1;	// remind one day before

  asprintf(&_text, "%*s", MAX_EVENT_NAME, " ");

  _id = uniqueId++;
}


bool cCalEvent::isValid(eCalEventType type) const
{
  eCalEventType my_type = (type == evUnknown) ? _type : type;
  return(my_type == evYearly && isValidDayMonth()
         ||
	 my_type == evWeekly && isValidDayOfWeek()
	 ||
	 my_type == evFixedDay && isValidDayMonthYear()
	);
}


bool cCalEvent::isValidDayMonth() const
{
  int end = 0;

  if ( _month < 1 && _month > 12 )
    return false;

  switch (_month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      end = 31;
      break;

    case 2:
      end = 29;
      break;

    case 4:
    case 6:
    case 9:
    case 11:
      end = 30;
      break;

    default:
      break;
  }

  return ( _day >= 1 && _day <= end );
}


bool cCalEvent::isValidDayMonthYear() const
{
  struct tm tm = { 0, 0, 12, _day, _month - 1, _year - 1900, 0, 0, -1 };
  mktime(&tm);
  return ((_day == tm.tm_mday) && ((_month - 1) == tm.tm_mon) && ((_year - 1900) == tm.tm_year));
}


bool cCalEvent::isValidDayOfWeek() const
{
  return ( _dayOfWeek >= 1 && _dayOfWeek <= 7 );
}


bool cCalEvent::validForDay (int day, int month, int year, int dayOfWeek)
{
  return (_type==evFixedDay && _day == day && _month == month && _year == year
	  ||
	  _type==evWeekly && _dayOfWeek == dayOfWeek
	  ||
	  _type==evYearly && _day == day && _month == month);
}


bool cCalEvent::validForDays (time_t t)
{
  struct tm *today, tm;

  // check for next events
  if (_reminderDays < 0)
    return false;	// no reminder configured for this event

  // t includes time for end of today
  today = localtime(&t);

  // dsyslog("Search for %d (%d days), have [%d] %d.%d.%d (%d)", (int)t, _reminderDays, _type, _day, _month, _year, _dayOfWeek);
  if (_type == evWeekly) {
    if (_reminderDays >= 6)
      return true;	// remind 6 days before makes no sence for weekly dates = remind always

    int dayOfWeek = today->tm_wday;
    if (dayOfWeek == _dayOfWeek)	// today
      return true;

    int diff = _reminderDays;
    while (diff) {
      if ((_dayOfWeek % 7) == ((dayOfWeek + diff) % 7))
        return true;
      diff--;
    }

    return false;
  }
  else {
    tm = *today;
    tm.tm_mday = _day;
    tm.tm_mon = _month - 1;
    if (_type == evFixedDay) tm.tm_year = _year - 1900;
    time_t t_ev = mktime(&tm);
    if ((t_ev >= t) && ((t + (86400 * _reminderDays)) >= t_ev))
      return true;
  }
  return false;
}


bool cCalEvent::isOldEvent (int day, int month, int year)
{
  // check for events older the the given date
  if (_type != evFixedDay || _year > year) return false;
  if (_year < year || _month < month) return true;
  if (_month > month) return false;
  if (_day < day) return true;
  return false;
}


char* cCalEvent::getText() const
{
  return _text;
}


char* cCalEvent::getLongText() const
{
  if (_buffer) free(_buffer);
  char *s[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
  
  switch (_type)
  {
    case evFixedDay:
      asprintf(&_buffer,"%s\t%02d.%02d.%04d\t%s", tr("Fix Day"),
	       _day, _month, _year, _text);
      break;

    case evWeekly:
      asprintf(&_buffer,"%s\t%s\t%s", tr("Weekly"), tr(s[_dayOfWeek-1]), _text);
      break;

    case evYearly:
      asprintf(&_buffer,"%s\t%02d.%02d.\t%s", tr("Yearly"), _day, _month,
	       _text);
      break;

    default:
      break;
  }

  return _buffer;
}

// get human readable text back with day of the year in front, format: <yday>:<text>
char* cCalEvent::getTextAndDay(time_t t, int *day) const
{
  if (!day) return _text;

  struct tm *tmptr = localtime(&t);
  struct tm tm = *tmptr;
  int weekDay, cnt;

  switch (_type)
  {
    case evFixedDay:
      tm.tm_year = _year - 1900;
    case evYearly:
      tm.tm_mday = _day;
      tm.tm_mon = _month - 1;
      mktime(&tm);
      *day = tm.tm_yday;
      break;

    case evWeekly:
      // event is assumed to be max one week in future starting from today
      weekDay = tm.tm_wday;
      if (!weekDay) weekDay = 7;	// Sunday
      cnt = _dayOfWeek - weekDay;
      if (cnt < 0) cnt += 7;
      *day = tm.tm_yday + cnt;
      break;

    default:
      *day = -1;	// should never happen
      break;
  }

  return _text;
}


int cCalEvent::getId() const
{
  return _id;
}


void cCalEvent::print()
{
  std::cout << "Event id=" << _id
	    << " Type: " << _type
	    << " d=" << _day << " m=" <<  _month << " y=" <<  _year
	    << " text=" << _text
	    << " reminder=" << _reminderDays
	    << std::endl;
}


char* cCalEvent::toString()
{
  if (_buffer) free(_buffer);

  switch (_type)
  {
    case evFixedDay:
      asprintf(&_buffer, "F:%02d.%02d.%d:%s:%d\n", _day, _month, _year, _text, _reminderDays);
      break;

    case evWeekly:
      asprintf(&_buffer, "W:%d:%s:%d\n", _dayOfWeek, _text, _reminderDays);
      break;

    case evYearly:
      asprintf(&_buffer, "Y:%02d.%02d.:%s:%d\n", _day, _month, _text, _reminderDays);
      break;

    default:
      break;
  }

  return _buffer;
}


bool cCalEvent::Save(FILE *f)
{
  if ( isValid() )
    return fprintf (f, toString()) > 0;
  else
    return false;
}


bool cCalEvent::Parse(const char* s)
{
  bool rc = false;
  char dummy;
  int n;

  if (!s)
    return rc;

  _reminderDays = -1;	// to accept also old file format

  switch (s[0]) {
    case 'F':
      n = sscanf (s, "%c:%d.%d.%d:%a[^:]:%d", &dummy, &_day, &_month, &_year, &_text, &_reminderDays);
      if (n >= 5) {
	_type = evFixedDay;
	rc = isValid();
      }
      break;

    case 'Y':
      n = sscanf (s, "%c:%d.%d.:%a[^:]:%d", &dummy, &_day, &_month, &_text, &_reminderDays);
      if (n >= 4) {
	_type = evYearly;
	rc = isValid();
      }
      break;

    case 'W':
      n = sscanf (s, "%c:%d:%a[^:]:%d", &dummy, &_dayOfWeek, &_text, &_reminderDays);
      if (n >= 3) {
	_type = evWeekly;
	rc = isValid();
      }
      break;

    default:
      break;
  }

  if (rc == false)
    esyslog("PIM: illegal event entry [%s] found", s);

  return rc;
}

// compare 2 events to sort the event list
// Must return 0 if this object is equal to ListObject, a positive value
// if it is "greater", and a negative value if it is "smaller".
//
// we sort like this:
// 1) weekly events
// 2) fixed events
// 3) yearly events
//
// all are sorted by date
int cCalEvent::Compare(const cListObject &ListObject) const
{
  cCalEvent *ev_cmp = (cCalEvent *)&ListObject;
  int v;

  if (_type == evWeekly) {
    if (ev_cmp->_type != evWeekly) return -1;
    return (_dayOfWeek - ev_cmp->_dayOfWeek);
  }

  if (ev_cmp->_type == evWeekly) return 1;

  // now both are yearly or fixed
  if (_type == evFixedDay) {
    if (ev_cmp->_type != evFixedDay) return -1;
  }
  else
    if (ev_cmp->_type == evFixedDay) return 1;

  if ((_type == evFixedDay) && (ev_cmp->_type == evFixedDay)) {
    v = _year - ev_cmp->_year;
    if (v != 0) return v;		// years differ
  }

  // both years are the same or at least one yearly event
  v = _month - ev_cmp->_month;
  if (v != 0) return v;			// months differ

  v = _day - ev_cmp->_day;
  return v;
}

