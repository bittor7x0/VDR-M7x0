#ifndef __RADIOINFOTOOLS_H
#define __RADIOINFOTOOLS_H

#include "config.h"


#define RI_DEBUG


inline void DEBUG_MSG(const char* msg)
{
  #ifdef RI_DEBUG
    fprintf(stderr, "RadioInfo: %s\n", msg);
  #endif
}





#endif //__RADIOINFOTOOLS_H
