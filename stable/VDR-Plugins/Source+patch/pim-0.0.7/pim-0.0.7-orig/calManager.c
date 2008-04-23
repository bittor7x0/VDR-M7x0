/*
 * $Id: calManager.c,v 1.1.1.1 2005/02/17 05:45:51 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: calManager.c,v $
 * Revision 1.1.1.1  2005/02/17 05:45:51  sundin
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


#include "calManager.h"
#include "pimSetup.h"

#include <vdr/plugin.h>

#define PIMCONF	"pimEvents.conf"

// Singleton-instance

cCalManager* cCalManager::instance = 0;


cCalManager* cCalManager::getInstance()
{
  if (!instance)
    instance = new cCalManager();

  return instance;
}


// private constructor
cCalManager::cCalManager()
{
  char *conf_file;

  asprintf(&conf_file, "%s/%s", globalSetup.ConfigDir, PIMCONF);
  if (!events.Load(conf_file, true))
    isyslog ("PIM: events file [%s] does not exist", conf_file);
  else
    dsyslog ("PIM: events file [%s] loaded", conf_file);

  free(conf_file);
}


// has event?
bool cCalManager::hasEvent(int day, int month, int year, int dayOfWeek)
{
  for (cCalEvent *ev = events.First(); ev; ev = events.Next(ev))
    {
      if ( ev->validForDay (day, month, year, dayOfWeek) )
      {
         return true;
      }
    }

  return false;
}


tEventIds cCalManager::getEventsForDay(int day, int month, int year,
		                  int dayOfWeek)
{
  tEventIds ids;

  for (cCalEvent *ev = events.First(); ev; ev = events.Next(ev))
    {
      if ( ev->validForDay (day, month, year, dayOfWeek) )
        ids.push_back(ev->getId());
    }

  return ids;
}

// get events for reminding
tEventIds cCalManager::getEventsForDays(time_t t)
{
  tEventIds ids;

  for (cCalEvent *ev = events.First(); ev; ev = events.Next(ev))
    {
      if ( ev->validForDays (t) )
        ids.push_back(ev->getId());
    }

  return ids;
}

tEventIds cCalManager::getAllEvents()
{
  tEventIds ids;

  for (cCalEvent *ev = events.First(); ev; ev = events.Next(ev))
    {
      ids.push_back(ev->getId());
    }

  return ids;
}


tEventIds cCalManager::getAllOldEvents()
{
  tEventIds ids;
  time_t t = time(NULL);
  struct tm *now = localtime(&t);

  dsyslog("PIM: search for fixed events older then %d.%d.%d", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
  for (cCalEvent *ev = events.First(); ev; ev = events.Next(ev))
  {
    if ( ev->isOldEvent(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900) )
      ids.push_back(ev->getId());
  }

  return ids;
}


char* cCalManager::getTextById(int id, bool detail)
{
  for (cCalEvent *ev = events.First(); ev; ev = events.Next(ev))
    {
      if ( ev->getId() == id)
      {
         if (detail)
	   return ev->getLongText();
         else
	   return ev->getText();
      }
    }
  return NULL;
}


cCalEvent* cCalManager::getEventById(int id)
{
  for (cCalEvent *ev = events.First(); ev; ev = events.Next(ev))
    {
      if ( ev->getId() == id)
      {
	 return ev;
      }
    }
  return NULL;
}


void cCalManager::add(cCalEvent* ev)
{
  if (ev)
  {
    events.Add(ev);
  }
}


void cCalManager::del(cCalEvent* ev)
{
  if (ev)
  {
    events.Del(ev);
  }
}


void cCalManager::sort(void)
{
    events.Sort();
}

