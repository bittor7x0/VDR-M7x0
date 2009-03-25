/*
 * $Id: bitmap.h,v 1.1.1.1 2005/01/17 20:03:24 beejay Exp $
 */

#ifndef VDR_WEATHERNG_BITMAP_H
#define VDR_WEATHERNG_BITMAP_H

#include <vdr/osd.h>
#include <vdr/tools.h>

#ifdef HAVE_MAGICK
	cBitmap*  LoadMagick(const char *Filename,int height, int width, int colors, bool Quiet);
#else
#ifdef HAVE_IMLIB2
	cBitmap*  LoadImlib(const char *Filename,int height, int width, int colors, bool Quiet);
#else
	cBitmap*  LoadXpm(const char *Filename);
#endif
#endif

#endif // VDR_WEATHERNG_BITMAP_H
