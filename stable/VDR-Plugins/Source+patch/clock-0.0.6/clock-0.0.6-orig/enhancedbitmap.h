/*
 * enhancendbitmap.h: an extended bitmap class
 *
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


