#ifndef __streamplayer_debug_h__
#define __streamplayer_debug_h__

#include <vdr/tools.h>

//#define _DEBUG 1

#ifdef _DEBUG
  #define _dprintf(a...) dsyslog(a)
#else
  #define _dprintf(a...) void()
#endif

void ReportError(const char *fmt, ...);
char *GetError();

#endif
