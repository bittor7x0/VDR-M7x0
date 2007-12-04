#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <time.h>

#include "tools.h"

using namespace std;


//////////////////////////////////////////////////////////////////////////////


string toTimeString(long time)
{
  int mins = time / 60;
  int hrs  = mins / 60;
  mins = mins - hrs*60;
  string sMins = to_string(mins);
  if (sMins.length() == 1)
    sMins = "0" + sMins;

  return to_string(hrs) + ":" + sMins; 
}


//----------------------------------------------------------------------------

string toTimeString(struct tm* t)
{
  string mins = to_string((*t).tm_min);
  if (mins.length() == 1)
    mins = "0" + mins;
  int hrs = (*t).tm_hour;
  if (hrs > 12)
    hrs = hrs % 12;
  if (hrs == 0) hrs = 12;
  return to_string(hrs) + ":" + mins;
}


//----------------------------------------------------------------------------

string to12time(string time)
{
  string min = time.substr(time.find_first_of(":")+1);
  string hr  = time.substr(0, time.find_first_of(":"));
  int iHr = atoi(hr.c_str());

  if (iHr > 12)
     iHr = iHr - 12;
     
  if (iHr == 0)
    iHr = 12;
 
  return to_string(iHr)+":"+min;

}


//----------------------------------------------------------------------------


// Finds the channel number that is length channels away from sid
int jumpTo(int sid, int length, char direction)
{
  bool stop = false;
  for (int i=0; i<length && !stop; i++)
  {
     if (direction == 'U')
       sid--;
     else
       sid++;

     bool ok = false;
     while(!ok)
     {
       if (sid < 0)
       {
         direction = 'D';
         stop = true;
       }
       if (sid > Channels.MaxNumber()) 
       {
         direction = 'U';
         stop = true;
       }
       cChannel* check = Channels.GetByNumber(sid);
       if (check)
         ok=true;
       else
       {
         if (direction == 'U')
           sid--;
         else
           sid++;
       }    
     }  
   }
   return sid;
}

//----------------------------------------------------------------------------


int max(int a, int b)
{
  return (a > b) ? a : b;
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



//////////////////////////////////////////////////////////////////////////////
