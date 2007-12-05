#ifndef ___FEPGTOOLS_H
#define ___FEPGTOOLS_H

#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <time.h>

using namespace std;


//////////////////////////////////////////////////////////////////////////////

// Converts long to a time string of type X:XX
string toTimeString(long);
// Converts struct tm* to a time string of type X:XX
string toTimeString(struct tm*);

// Coverts 24hr time to 12hr
string to12time(string);

// Returns valid channel length away from sid in the given direction
int jumpTo(int sid, int length, char direction);

// Converts ARGB values into color
tColor toColor(int r, int g, int b, int t);


template<class type>
inline string to_string(const type& value)
{
  ostringstream streamOut;
  streamOut << value;
  return streamOut.str();
}
  
  
// Returns the max of a & b
int max(int a, int b);
   
//////////////////////////////////////////////////////////////////////////////


#endif //___FEPGTOOLS_H
