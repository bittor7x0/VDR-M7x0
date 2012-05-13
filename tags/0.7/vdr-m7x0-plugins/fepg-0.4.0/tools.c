#include <string.h>
#include <ctype.h>

#include <vdr/i18n.h>

#include "tools.h"
#include "config.h"


//////////////////////////////////////////////////////////////////////////////


void TimeToString(char* buf, const time_t t)
{
  struct tm tm_r;
  TimeToString(buf, localtime_r(&t, &tm_r));
}


//----------------------------------------------------------------------------

void TimeToString(char* buf, const struct tm* t)
{
  if (config.timeFormat24h)
    strftime(buf, 8, "%H:%M", t);
  else
    strftime(buf, 9, "%I:%M %p", t);
    
  if (buf[0] == '0')
    for (size_t i=0; i<strlen(buf); i++)
      buf[i] = buf[i+1]; 
}


//----------------------------------------------------------------------------

char* DurationToString(time_t t)
{
  int tmin = (t - (t % 60)) / 60;
  int min = tmin % 60;
  int hrs = (tmin - min) / 60;
   
  char* time;
  asprintf(&time, "%d:%02d", hrs, min);
  return time;
}


//----------------------------------------------------------------------------

const char* OsdErrorToString(eOsdError err)
{
  const char* e = tr("Unknown");
  switch (err)
  {
    case oeOk:              e = tr("Ok");                break;
    case oeTooManyAreas:    e = tr("Too Many Areas");    break;
    case oeTooManyColors:   e = tr("Too Many Colors");   break;
    case oeBppNotSupported: e = tr("Bpp Not Supported"); break;
    case oeAreasOverlap:    e = tr("Areas Overlap");     break;
    case oeWrongAlignment:  e = tr("Wrong Alignment");   break;
    case oeOutOfMemory:     e = tr("Out Of Memory");     break;
    case oeWrongAreaSize:   e = tr("Wrong Area Size");   break;
    case oeUnknown: break;
  }
  return e;
}


//////////////////////////////////////////////////////////////////////////////
