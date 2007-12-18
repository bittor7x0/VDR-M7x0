#include <string.h>
#include <ctype.h>

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

tColor toColor(int r, int g, int b, int t)
{
  int temp = t;
  temp <<= 8;
  temp += r;
  temp <<= 8;
  temp += g;
  temp <<= 8;
  temp += b;
  return (tColor) temp;
}


//----------------------------------------------------------------------------

const char* OsdErrorToString(eOsdError err)
{
  const char* e = "Unknown";
  switch (err)
  {
    case oeOk:              e = "Ok"; break;
    case oeTooManyAreas:    e = "Too Many Areas"; break;
    case oeTooManyColors:   e = "Too Many Colors"; break;
    case oeBppNotSupported: e = "Bpp Not Supported"; break;
    case oeAreasOverlap:    e = "Areas Overlap"; break;
    case oeWrongAlignment:  e = "Wrong Alignment"; break;
    case oeOutOfMemory:     e = "Out Of Memory"; break;
    case oeWrongAreaSize:   e = "Wrong Area Size"; break;
    case oeUnknown: break;
  }
  return e;
}


//////////////////////////////////////////////////////////////////////////////
