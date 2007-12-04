/*
 * dvbosd.h: Implementation of the DVB On Screen Display
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
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
#ifdef USE_32BPP_FRAMEBUFFER
  uint32_t *fbMem;
#else
  uint8_t *fbMem;
  uint16_t red[256];
  uint16_t green[256];
  uint16_t blue[256];
  uint16_t trans[256];
  struct fb_cmap palette;
  int paletteUses;
#endif
  bool fbInterlaced;
  int fbWidth;
  int fbHeight;
  int fbBpp;
  int fbBppSaved;
  bool clearOnStart;
  bool clearOnStop;
  bool overlay;
public:
  cDvbOsdProvider(int OsdDev, bool ClearOnStart = true, bool ClearOnStop = true, bool Overlay = false);
  virtual ~cDvbOsdProvider();
//M7X0 END AK
  virtual cOsd *CreateOsd(int Left, int Top);
  };

#endif //__DVBOSD_H
