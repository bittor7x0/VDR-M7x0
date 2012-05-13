/*
 * dvbosd.h: Implementation of the DVB On Screen Display
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __DVBOSD_H
#define __DVBOSD_H

#include <linux/fb.h>
#include "osd.h"
#include "tools.h"

class cDvbOsdProvider : public cOsdProvider {
private:
  int osdDev;
//M7X0 BEGIN AK
// OSD is done via framebuffer-device so mmap can be used.
// Save settings in here.
  uint8_t *fbMem;
  bool fbInterlaced;
  int fbWidth;
  int fbHeight;
  int fbBpp;
  int fbBppSaved;
public:
  cDvbOsdProvider(int OsdDev);
  virtual ~cDvbOsdProvider();
//M7X0 END AK
  virtual cOsd *CreateOsd(int Left, int Top);
  };

#endif //__DVBOSD_H
