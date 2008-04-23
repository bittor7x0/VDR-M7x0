/*
 * $Id: calEvent.h,v 1.1.1.1 2005/02/17 05:45:53 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: calEvent.h,v $
 * Revision 1.1.1.1  2005/02/17 05:45:53  sundin
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


#ifndef __CALEVENT_H_
#define __CALEVENT_H_

#include <vector>

#include <vdr/tools.h>

typedef std::vector<int> tEventIds;

#define MAX_EVENT_NAME 30

enum eCalEventType { evFixedDay,
	             evWeekly,
		     evYearly,
		     evUnknown
                   };

class cCalEvent : public cListObject
{

 friend class cEventEditMenu;

 private:

  static char* _buffer;


  int _id;

  int _day;
  int _month;
  int _year;

  int _dayOfWeek;

  char *_text;

  int _reminderDays;

  eCalEventType _type;

  void init();

  char* toString();

 public:
  
  static int uniqueId;

  cCalEvent();
  
  ~cCalEvent();

  // deep assignment
  cCalEvent& operator= (const cCalEvent& ev);

  bool isValid(eCalEventType type = evUnknown) const;

  bool isValidDayMonth() const;

  bool isValidDayMonthYear() const;

  bool isValidDayOfWeek() const;

  bool validForDay (int day, int month, int year, int dayOfWeek);

  bool validForDays (time_t t);
  
  bool isOldEvent (int day, int month, int year);

  char* getText() const;

  char* getLongText() const;

  char* getTextAndDay(time_t t, int *day) const;

  int getId() const;

  void print();

  // from cConfig
  bool Parse(const char* s);
  bool Save(FILE *f);
  int Compare(const cListObject &ListObject) const;

};

#endif
