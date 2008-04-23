/*
 * $Id: calendar.c,v 1.1.1.1 2005/02/17 05:45:51 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: calendar.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:51  sundin
 * initial import ver0.0.5
 *
 * Revision 1.2  2005/01/17 18:30:50  ksltuf
 * support for vdr 1.3.18 (readline)
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.4  2004/12/13 07:59:04  ksltuf
 * bugfix setup parsing; new option firstIsMonday
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


#include <iostream>
#include <iomanip>
#include <string>

#include "calendar.h"
#include "pimSetup.h"

#include <vdr/config.h>
#include <vdr/tools.h>
#include <vdr/i18n.h>

int cCalendar::_rows = 6;  // rows: max weeks per month
int cCalendar::_cols = 7;  // cols: days per week

cCalendar::cCalendar()
{
  // calculate today's date
  
  time_t t = time(NULL);
  struct tm *now = localtime(&t);

  // default: display calendar for actual month
  
  _month = now->tm_mon + 1;
  _year  = now->tm_year + 1900;

  calculateDays();
}


cCalendar::cCalendar(int month, int year) : _month(month), _year(year)
{
  calculateDays();
}


void cCalendar::increaseMonth()
{
  _month++;
  if (_month == 13)
    {
      _month = 1;
      _year++;
    }

  calculateDays();
}


void cCalendar::decreaseMonth()
{
  _month--;
  if (_month == 0)
    {
      _month = 12;
      _year--;
    }

  calculateDays();
}


void cCalendar::reset()
{
  time_t t = time(NULL);
  struct tm *now = localtime(&t);

  _month = now->tm_mon + 1;
  _year  = now->tm_year + 1900;

  calculateDays();
}


const char* cCalendar::getDayOfWeekAsText(const int day)
{
  switch (day)
  {
    case 1:
            return (globalSetup.firstIsMonday==1) ? tr("Mo") : tr("Su");
    case 2:
            return (globalSetup.firstIsMonday==1) ? tr("Tu") : tr("Mo");
    case 3:
            return (globalSetup.firstIsMonday==1) ? tr("We") : tr("Tu");
    case 4:
            return (globalSetup.firstIsMonday==1) ? tr("Th") : tr("We");
    case 5:
            return (globalSetup.firstIsMonday==1) ? tr("Fr") : tr("Th");
    case 6:
            return (globalSetup.firstIsMonday==1) ? tr("Sa") : tr("Fr");
    case 7:
            return (globalSetup.firstIsMonday==1) ? tr("Su") : tr("Sa");
    default:
	    return "Error";

  }
}

const char* cCalendar::getMonthAsText()
{
  switch (_month)
  {
    case 1:
	    return tr("January");
    case 2:
	    return tr("February");
    case 3:
	    return tr("March");
    case 4:
	    return tr("April");
    case 5:
	    return tr("May");
    case 6:
	    return tr("June");
    case 7:
	    return tr("July");
    case 8:
	    return tr("August");
    case 9:
	    return tr("September");
    case 10:
	    return tr("October");
    case 11:
	    return tr("November");
    case 12:
	    return tr("December");
    default:
	    return "Error";

  }
}


// calculate days for given month/year and fill in data[][]
void cCalendar::calculateDays()
{
  time_t t = time(NULL);
  struct tm *now = localtime(&t);

  _today_d = now->tm_mday;
  _today_m = now->tm_mon + 1;
  _today_y = now->tm_year + 1900;

  setInvalid();

  if (_month < 1 || _month > 12 || _year < 1900 || _year > 2100)
    return;

  // use system calls instead of the cal tool to obtain the calendar
  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  tm.tm_hour = 12;
  tm.tm_mday = 1;
  tm.tm_mon = _month - 1;
  tm.tm_year = _year - 1900;
  mktime(&tm);
  int first = tm.tm_wday;
  int month = tm.tm_mon;

  if (globalSetup.firstIsMonday) {
    if (first == 0)
      first = 6;
    else
      first--;
  }

  // correct start day to the first displayable day (from month before)
  tm.tm_mday -= first;
  mktime(&tm);

  bool monthEnd = false;
  for (int i = 0; i < _rows; i++)
  {
    _data.push_back(std::vector<int>(_cols));
    if (i >= (_rows - 2) && (month != tm.tm_mon)) monthEnd = true;
    for (int k = 0; k < _cols; k++) {
      if (!monthEnd) {
	// mark days from last/next month by minus sign
	_data[i][k] = (month == tm.tm_mon) ? tm.tm_mday :
			(globalSetup.previewLastNextW ? -tm.tm_mday : 0);
	tm.tm_mday++;
	mktime(&tm);
      }
      else
	_data[i][k] = 0;	// no day in complete week, so display nothing
    }
  }

  setValid();
}

