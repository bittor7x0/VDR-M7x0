/*
 * $Id: eventEditMenu.h,v 1.1.1.1 2005/02/17 05:45:54 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: eventEditMenu.h,v $
 * Revision 1.1.1.1  2005/02/17 05:45:54  sundin
 * initial import ver0.0.5
 *
 * Revision 1.1.1.1  2005/01/17 18:06:31  ksltuf
 * PIM-Plugin 0.0.4
 *
 * Revision 1.5  2004/11/24 21:57:01  ksltuf
 * more osd-include-stuff
 *
 * Revision 1.4  2004/11/24 21:49:29  ksltuf
 * Make OSD-includes version-dependent
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


#ifndef __EVENTEDITMENU_H_
#define __EVENTEDITMENU_H_

#include "calEvent.h"

#include <vdr/osdbase.h>
#include <vdr/osd.h>
#include <vdr/menuitems.h>

class cEventEditMenu : public cOsdMenu
{
 private:
   int		_type;
   int		_last_type;
   const char*	_calTypeTexts[3];
   const char*	_weekDayTexts[7];
   bool		_newEvent;
   cCalEvent*	_ev;
   cCalEvent	_data;
   int		_remind;
   int		_last_remind;

   cMenuEditIntItem * _menu_day;
   cMenuEditIntItem * _menu_month;
   cMenuEditIntItem * _menu_year;
   cMenuEditIntItem * _menu_dayOfWeek;
   cMenuEditIntItem * _menu_reminderDays;
   cOsdItem	    * _infoItem;

   bool updateEvent(void);
   void setItemType(int type);
   void setRemindType(int remind);

 public:
   cEventEditMenu(cCalEvent* ev, bool insert=false);
   virtual ~cEventEditMenu();
   virtual eOSState ProcessKey(eKeys key);
};

#endif
