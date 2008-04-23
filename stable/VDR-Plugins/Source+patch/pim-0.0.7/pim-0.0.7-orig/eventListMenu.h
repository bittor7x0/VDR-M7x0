/*
 * $Id: eventListMenu.h,v 1.1.1.1 2005/02/17 05:45:54 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: eventListMenu.h,v $
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


#ifndef __EVENTLISTMENU_H_
#define __EVENTLISTMENU_H_

#include "calEvent.h"

#include <vdr/osdbase.h>
#include <vdr/osd.h>


class cEventListMenu : public cOsdMenu
{

 friend class cPimOsdMenu;

 public:

   // new menu of events for one day
   
   cEventListMenu(int day, int month, int year, int dayOfWeek);

   // new menu of all events (allows to edit them)

   cEventListMenu();

   ~cEventListMenu() { };

   virtual eOSState ProcessKey(eKeys key);

   cCalEvent* getCurrentEvent();


 private:

   eOSState Edit(void);
   eOSState New(void);
   eOSState Delete(void);

   bool _refresh;

   int _day;
   int _month;
   int _year;
   int _dayOfWeek;

   int _selectedItem;
};

#endif
