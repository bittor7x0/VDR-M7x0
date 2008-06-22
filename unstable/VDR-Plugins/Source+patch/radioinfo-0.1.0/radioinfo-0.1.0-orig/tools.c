#include "tools.h"


///////////////////////////////////////////////////////////////////////////////

#ifdef RI_DEBUG
#include <stdarg.h>

void DEBUG_MSG(const char* msg, ...)
{
  if (!msg) return;
  
  char* output = NULL;
  va_list ap;
  va_start(ap, msg);
  vasprintf(&output, msg, ap);
  va_end(ap);
   
  fprintf(stderr, "[RadioInfo] %s\n", output);
     
  fflush(stderr);
  free(output);  
}


#endif


//-----------------------------------------------------------------------------

const char* OsdErrorToString(eOsdError err)
{
  const char* e = "Unknown";
  switch (err)
  {
    case oeOk:              e = "Ok"; break;
    case oeTooManyAreas:    e = "Too Many Areas";    break;
    case oeTooManyColors:   e = "Too Many Colors";   break;
    case oeBppNotSupported: e = "Bpp Not Supported"; break;
    case oeAreasOverlap:    e = "Areas Overlap";     break;
    case oeWrongAlignment:  e = "Wrong Alignment";   break;
    case oeOutOfMemory:     e = "Out Of Memory";     break;
    case oeWrongAreaSize:   e = "Wrong Area Size";   break;
    case oeUnknown: break;
  }
  return e;
}


///////////////////////////////////////////////////////////////////////////////

