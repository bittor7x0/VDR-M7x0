/*
 * $Id: calendar.h,v 1.1.1.1 2005/02/17 05:45:54 sundin Exp $
 *
 * Tag: $Name:  $
 *
 * See the file COPYRIGHT for copyright information
 *
 * $Log: calendar.h,v $
 * Revision 1.1.1.1  2005/02/17 05:45:54  sundin
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


#ifndef __CALENDAR_H_
#define __CALENDAR_H_


#include <vector>
#include <string>

class cCalendar
{
  
 public:
  
  static int _rows, _cols;
  
  cCalendar();
  cCalendar(int month, int year);
  
  ~cCalendar() {};
  
  std::vector<int> & operator[](int i)
    {
      return _data[i];
    }

  const std::vector<int> & operator[] (int i) const
    {
      return _data[i];
    }
  
  void setValid()   { _isValid = true;  }
  void setInvalid() { _isValid = false; }

  bool isValid()    { return _isValid; }

  int getMonth() { return _month; }
  int getYear()  { return _year;  }

  int getTodayDay()   { return _today_d;  }
  int getTodayMonth() { return _today_m;  }
  int getTodayYear()  { return _today_y;  }

  const char* getMonthAsText();
  const char* getDayOfWeekAsText(const int day);

  void increaseMonth();
  void decreaseMonth();

  void reset();

 private:

  int _month, _year;

  int _today_d, _today_m, _today_y;

  std::vector<std::vector<int> > _data;

  bool _isValid;

  void calculateDays();

};

std::ostream& operator<< (std::ostream& os, const cCalendar& cal);

#endif
