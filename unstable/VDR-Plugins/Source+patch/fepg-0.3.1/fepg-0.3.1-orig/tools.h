#ifndef ___FEPGTOOLS_H
#define ___FEPGTOOLS_H

#include <vdr/osd.h>
#include <vdr/font.h>
#include <vdr/tools.h>

#include <stdlib.h>
#include <time.h>

#define PAL  0
#define NTSC 1

//////////////////////////////////////////////////////////////////////////////

// Converts time a time string
void TimeToString(char* buf, const time_t t);
void TimeToString(char* buf, const struct tm* t);

// Convert number of seconds into H:M
char* DurationToString(time_t t);

// Converts ARGB values into color
tColor toColor(int r, int g, int b, int t);

const char* OsdErrorToString(eOsdError err);

#define GET_MINUTES(t) ( (t - (t % 60)) / 60 ) % 60
 
// Returns the max of a & b
//#define max(x, y) (x < y) ? y : x  
 
 

//////////////////////////////////////////////////////////////////////////////
  
#endif //___FEPGTOOLS_H
