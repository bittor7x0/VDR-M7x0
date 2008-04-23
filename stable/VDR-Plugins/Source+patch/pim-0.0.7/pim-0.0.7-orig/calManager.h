/*
 * $Id: calManager.h,v 1.1.1.1 2005/02/17 05:45:54 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: calManager.h,v $
 * Revision 1.1.1.1  2005/02/17 05:45:54  sundin
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


#ifndef __CALMANAGER_H_
#define __CALMANAGER_H_

#include "calendar.h"
#include "calEvent.h"
#include "calEvents.h"


class cCalManager
{
 private:
  
  // Singleton
  static cCalManager* instance;
  cCalManager();
  ~cCalManager() { };
  
  cCalEvents events;
  
 public:

  // get an instance
  static cCalManager* getInstance();

  // does this day have an event?
  bool hasEvent (int day, int month, int year, int dayOfWeek);

  tEventIds getEventsForDay(int day, int month, int year, int dayOfWeek);

  tEventIds getEventsForDays(time_t t);

  tEventIds getAllEvents();

  tEventIds getAllOldEvents();

  char* getTextById(int id, bool detail=false);

  cCalEvent* getEventById(int id);

  void add(cCalEvent* ev);

  void del(cCalEvent* ev);

  void sort(void);

  void save() { events.Save(); }

};

#endif
