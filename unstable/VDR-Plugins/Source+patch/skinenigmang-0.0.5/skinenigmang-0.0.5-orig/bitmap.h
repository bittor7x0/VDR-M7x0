/*
 * bitmap.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifdef HAVE_IMAGEMAGICK

#ifndef _OSDIMAGE_BITMAP_H_
#define _OSDIMAGE_BITMAP_H_

#include "common.h"
#include <vdr/osd.h>

class cOSDImageBitmap {
public:
  cOSDImageBitmap();
  ~cOSDImageBitmap();
  bool DrawImage(const char *fileNameP, int x, int y, int w, int h, int colors, cBitmap *bmp);

private:
  bool DrawMagick(const char *Filename, int x, int y, int height, int width, int colors, cBitmap *bmp);
};

#endif
#endif //HAVE_IMAGEMAGICK

// vim:et:sw=2:ts=2:
