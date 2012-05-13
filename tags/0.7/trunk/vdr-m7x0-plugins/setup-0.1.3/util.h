/*********************************************************
 * DESCRIPTION: 
 *             Header File
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2004 by Ralf Dotzert 
 *********************************************************/

#ifndef UTIL_H
#define UTIL_H
#include <vdr/tools.h>
#include <vdr/timers.h>
#include <string>
using namespace std;




/**
@author Ralf Dotzert
*/
class Util{
public:
    Util();
    ~Util();
    enum Type { UNDEFINED,TEXT, NUMBER_TEXT, HEX, IP, BOOL, NUMBER, SELECTION, FILE, DIR};
    static char * Strdupnew(const char  *str);
    static char * Strdupnew(const char  *prefix, const char  *str);
    static char * Strdupnew( const char  *str, int size );
	static void trim(string& str,const string& drop=" \t\n\r" );
//    static char * Strtrim(char *str);
    static bool isBool(const char *string, bool &flag);
    static bool isBool(const char *string, int  &flag);
    static bool isType( const char * string, Util::Type & typ );
    static bool isNumber( const char * string, int &number );
    static const char * typeToStr(Type type);
    static const char * boolToStr( bool val );
    static void SwitchChannelList(const char  *selectedChannelList);
};

class TimerString : public cListObject
{
  private:
    cString  _timer;
  public:
    TimerString (cString timer)   {_timer=timer; }
    cString      GetTimerString() {return (_timer);}
};

class TimerList
{
  private:
    cList<TimerString>  myTimers;
  public:
    TimerList()  {};
    ~TimerList() {};
    void SaveTimer();
    void RestoreTimer();
};

#endif
