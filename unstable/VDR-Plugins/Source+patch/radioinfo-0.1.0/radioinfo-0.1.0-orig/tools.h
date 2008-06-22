#ifndef __RADIOINFOTOOLS_H
#define __RADIOINFOTOOLS_H

#include <stdlib.h>  // free
#include <vdr/osd.h> // eOsdError


///////////////////////////////////////////////////////////////////////////////

// Remove the following line to disable debug messages
#define RI_DEBUG


///////////////////////////////////////////////////////////////////////////////

#ifdef RI_DEBUG

void DEBUG_MSG(const char* msg, ...); 

#else // Debug messages disabled

#define DEBUG_MSG(msg, args...)

#endif


const char* OsdErrorToString(eOsdError err);


///////////////////////////////////////////////////////////////////////////////


#endif //__RADIOINFOTOOLS_H
