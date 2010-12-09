/*
 * eit.h: EIT section filter
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: eit.h 170 2007-03-06 07:49:43Z andreas $
 */

#ifndef __EIT_H
#define __EIT_H

#include "filter.h"

class cEitFilter : public cFilter {
private:
  static time_t disableUntil;
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char *Data, int Length);
public:
  cEitFilter(void);
  static void SetDisableUntil(time_t Time);
  };

#endif //__EIT_H
