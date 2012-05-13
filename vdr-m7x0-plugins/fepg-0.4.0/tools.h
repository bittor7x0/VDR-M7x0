#ifndef ___FEPGTOOLS_H
#define ___FEPGTOOLS_H

#include <vdr/osd.h>
#include <vdr/font.h>
#include <vdr/tools.h>

#include <stdlib.h>
#include <time.h>


//////////////////////////////////////////////////////////////////////////////


// Converts time a time string
void TimeToString(char* buf, const time_t t);
void TimeToString(char* buf, const struct tm* t);

// Convert number of seconds into H:M
char* DurationToString(time_t t);

// Converts ARGB values into color
#define toColor(r,g,b,a) ((tColor) ((a << 24) | (r << 16) | (g << 8 ) | b))

const char* OsdErrorToString(eOsdError err);

#define GET_MINUTES(t) (( (t - (t % 60)) / 60 ) % 60)
#define MIN_TO_SEC(t)  (t*60)
#define HRS_TO_SEC(t)  (t*3600)
 

//////////////////////////////////////////////////////////////////////////////
  
#endif //___FEPGTOOLS_H
