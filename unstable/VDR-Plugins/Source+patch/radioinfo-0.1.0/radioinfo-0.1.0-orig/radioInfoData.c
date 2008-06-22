#include <string.h>

#include "radioInfoData.h"


///////////////////////////////////////////////////////////////////////////////


void cRadioInfoData::Reset(void)
{
  Lock();

    strcpy(title, "No Info");
    artist[0] = '\0';
    extra1[0] = '\0';
    extra2[0] = '\0';
    extra3[0] = '\0';
  
  Unlock(); 
}


///////////////////////////////////////////////////////////////////////////////

