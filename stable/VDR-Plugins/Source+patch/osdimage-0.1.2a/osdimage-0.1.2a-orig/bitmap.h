// bitmap.h, brougs78

#ifndef _OSDIMAGE_BITMAP_H_
#define _OSDIMAGE_BITMAP_H_

#define X_DISPLAY_MISSING

//#include "osdimage.h"
#include "definitions.h"
#include "osdimage.h"
#include <vdr/osd.h>
#include <Magick++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>


using namespace Magick;


class cOSDImageBitmap
{
public:
   cOSDImageBitmap();
	~cOSDImageBitmap();
	
	
	// Library-abstraction routines
	
	bool Load(const char *file);

	void RotateCW(void);
	
	void RotateCCW(void);
	
	void Rotate0(void);
	
	void Render(cBitmap &bmp, int wWindow, int hWindow, int colors, int quantizer, bool dither, sColorSet &colorSet, int alpha);

	inline int Width() { return width; }
	
	inline int Height() { return height; }
	

private:	
	// ImageMagick-routines

	bool LoadImageMagick(Image &imgkLoad, const char *file);
	
	void RotateImageMagick(Image &imgkRotate, int dir);
	// dir: 0 = reset, 1 = clockwise, -1 = counter-clockwise

	void QuantizeImageMagick(Image &imgkQuant, int colors, bool dither);
	
	void SaveImageMagick(Image &imgkSave, const char *file);
	
	
	// Netpbm-routines 
	
	void QuantizeNetpbm(Image &imgkQuant, int colors, bool dither);
	
	
	// tools
	
	void CalcNewPicSize(int *wNew, int *hNew, int wPic, int hPic, int wWindow, int hWindow, int colors);
	
	void ConvertImgk2Bmp(cBitmap &bmp, Image &imgkConv, int alpha);
	
	
	// variables
	
	Image imgkImage;
	int orientation;
	bool loadingFailed;
	int width, height;
};

#endif
