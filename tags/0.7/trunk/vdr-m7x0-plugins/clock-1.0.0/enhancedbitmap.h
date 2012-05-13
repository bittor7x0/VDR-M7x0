/*
 * enhancendbitmap.h: an extended bitmap class
 *
 * Clock Plugin for VDR. See http://vdr.aistleitner.info for download.
 * Copyright (C) 2007  Mario Aistleitner <vdr@aistleitner.info>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __ENHANCEDBITMAP_H
#define __ENHANCEDBITMAP_H

#include <vdr/osdbase.h>

class cEnhancedBitmap: public cBitmap{
private:
	char *bitmap;
protected:
public:
  // standard constructor (see cBitmap's contructor)
  cEnhancedBitmap(int Width, int Height, int Bpp, bool ClearWithBackground = true);

  // tells you, if the point is in the bitmap
  bool onBitmap(int x, int y);

  // draws a line between (x1,y1) and (x2,y2) in the specified color
  // if commit is true, then the dirty values of the bitmap are set.
  void DrawLine(int x1, int y1, int x2, int y2, tColor Color,bool commit = true);

  // draws a and fills a triangle (x1,y1), (x2,y2) and (x3,y3) in the specified color
  // if commit is true, then the dirty values of the bitmap are set.
  void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3,tColor Color,bool commit = true);

  // draws a triangle (x1,y1), (x2,y2) and (x3,y3) in the specified color
  // if commit is true, then the dirty values of the bitmap are set.
  void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3,tColor Color,bool commit = true);  
};

#endif


