/*
 * Spider-Arachnid: A plugin for the Video Disk Recorder
 *
 * Copyright (C) 2005-2007, Thomas Günther <tom@toms-cafe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id: bitmap.h 87 2007-06-22 22:37:36Z tom $
 */

#ifndef VDR_SPIDER_BITMAP_H
#define VDR_SPIDER_BITMAP_H

#include "spider.h"
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>


/** --- class Bitmap ------------------------------------------------------- **/

class Bitmap : public cBitmap
{
public:

  /** Constructor */
  Bitmap(int width, int height);

  /** Constructor for a bitmap with frame */
  Bitmap(int width, int height, tColor frameColor, tColor backgroundColor);

  /** Constructor for a bitmap read from an xpm file */
  Bitmap(int width, int height, const char* dir, const char* name);

  /** Constructor for a card bitmap read from an xpm file */
  Bitmap(int width, int height, const char* dir,
         const char* suit, const char* rank);

  /** Write a text into the bitmap */
  void text(const char* text, bool centered = true);

  /** Draw a frame into the bitmap */
  void frame(int x1, int y1, int x2, int y2, tColor frameColor);

  /** Load a bitmap from an xpm file - taken from ElchiAIO4d patch */
  bool loadXpm(const char* FileName, tColor NoneColor = clrTransparent);
};

#endif // VDR_SPIDER_BITMAP_H
