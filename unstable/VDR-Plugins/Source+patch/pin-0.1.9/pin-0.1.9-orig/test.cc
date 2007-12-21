#include "def.h"


int main()
{
   char ct[sizeHHMM+TB];
   long theTime = DT::lNow();

   printf("lNow = '%s'; daylight  %d; timezone %ld hours\n", DT::int2Hhmm(theTime, ct), daylight, timezone/tfSecondsPerHour);

   return 0;
}

