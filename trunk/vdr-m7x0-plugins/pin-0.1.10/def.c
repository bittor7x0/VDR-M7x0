/*
 * pin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Date: 11.04.05 - 12.02.07, horchi
 */

//***************************************************************************
// Include
//***************************************************************************

#include <stdarg.h>
#include <syslog.h>

#include "def.h"

int logLevel = eloOff;

//***************************************************************************
// Tell
//***************************************************************************

int tell(int eloquence, const char* format, ...)
{
   char tmp[1024];
   va_list ap;

   va_start(ap, format);

   if (logLevel >= eloquence)
   {
      vsnprintf(tmp + 5, sizeof tmp - 5, format, ap);
      memcpy(tmp, "[pin] ", 5);
      syslog(LOG_INFO, "%s", tmp);
   }

   va_end(ap);

   return success;
}
