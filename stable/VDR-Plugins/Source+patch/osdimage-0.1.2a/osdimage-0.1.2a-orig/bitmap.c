// bitmap.c, brougs78


#include "bitmap.h"
#include "loadingFailed16.xpm"
//#include "loadingFailed256.xpm"


using namespace Magick;


cOSDImageBitmap::cOSDImageBitmap()
{
   // Consturctor
}


cOSDImageBitmap::~cOSDImageBitmap()
{
   // Destructor
}


// --- LIBRARY-ABSTRACTION ROUTINES --------------------------------------------

bool cOSDImageBitmap::Load(const char *file)
{
	bool status;

	orientation = 0;	
	status = LoadImageMagick(imgkImage, file);
			
#ifdef DEBUG
	if (status == false) printf("Loading of '%s' failed!\n", file);
#endif
	
	return status;
}


void cOSDImageBitmap::RotateCW(void)
{	
	RotateImageMagick(imgkImage, 1);
	
#ifdef DEBUG
	printf("rotate cw\n");
#endif
}


void cOSDImageBitmap::RotateCCW(void)
{	
#ifdef DEBUG
	printf("rotate cw\n");
#endif

	RotateImageMagick(imgkImage, -1);
}


void cOSDImageBitmap::Rotate0(void)
{
#ifdef DEBUG
	printf("rotation resetted\n");
#endif

	RotateImageMagick(imgkImage, 0);
}


void cOSDImageBitmap::Render(cBitmap &bmp, int wWindow, int hWindow, int colors, int quantizer, bool dither, sColorSet &colorSet, int alpha)
{
   int w = Width();
	int h = Height();
	int wNew, hNew;
#ifdef DEBUG
	int time = time_ms(), dt;
#endif

	
	if (!loadingFailed)
	{
		// calculate the resizing-parameter
		CalcNewPicSize(&wNew, &hNew, w, h, wWindow, hWindow, colors);
		
		// resize the picture
		Image imgkRender = imgkImage;
		imgkRender.sample(Geometry(wNew, hNew, 0, 0) );
		
		width = imgkRender.columns();
		height = imgkRender.rows();

#ifdef DEBUG
		printf("\t- old geometry: %dx%d (ar=%e)\n", w, h, (double)w / (double)h);
		printf("\t- new geometry: %dx%d (ar=%e)\n", wNew, hNew, (double)wNew / (double)hNew );
#endif


		// quantize the picture
		if (quantizer == 0)
		{
			// use ImageMagick for quantization
			QuantizeImageMagick(imgkRender, colors, dither);
		}
		else
		{
			// use Netpbm for quantization
			QuantizeNetpbm(imgkRender, colors, dither);
		}

#ifdef DEBUG
	dt = time_ms() - time;
	printf("\t- Resizing & quantization (using %s): %dms\n", quantizer == 0 ? "ImageMagick" : "Netpbm", dt);
#endif

		
		// generate cBitmap
		if (quantizer == 0)
		{
			// used ImageMagick for quantization
			ConvertImgk2Bmp(bmp, imgkRender, alpha);
		}
		else
		{
			// used Netpbm for quantization
			Image imgkNetpbm;
			LoadImageMagick(imgkNetpbm, TMPIMAGEQUANTIZED);
			ConvertImgk2Bmp(bmp, imgkNetpbm, alpha);
		}		
	}
	else
	{
		// load 'loadinFailedPic' and restore transparency
		bmp.SetXpm(loadingFailed16_xpm);
		bmp.SetColor(0, colorSet.picBG);
		
		w = width = bmp.Width();
		h = height = bmp.Height();
		
		// Write Info into bmp
		if (OSDImageSetup.have256colors)
		{
			bmp.DrawText(SHADOWOFFSET, h - FONTSIZE + 1, tr("Error loading picture!"), colorSet.txtShadow, clrTransparent, FONT, w - SHADOWOFFSET, FONTSIZE, taCenter);
			bmp.DrawText(0, h - FONTSIZE + 1, tr("Error loading picture!"), colorSet.txtFG, clrTransparent, FONT, w - SHADOWOFFSET, FONTSIZE, taCenter);
		}
		else
		{
			// Write the Text with the 2. bmp-color as FG and 3. bmp-color as BG
			bmp.DrawText(0, h - FONTSIZE + 1, tr("Error loading picture!"), bmp.Color(1), bmp.Color(2), FONT, w, FONTSIZE, taCenter);
		}
	}	
}

// --- END OF LIBRARY-ABSTRACTION ROUTINES -------------------------------------



// --- IMAGEMAGICK ROUTINES ----------------------------------------------------

bool cOSDImageBitmap::LoadImageMagick(Image &imgkLoad, const char *file)
{
	try
	{
		// only read a smaller version of the picture (if possible)
		imgkLoad.read(Geometry(OSDMAXWIDTH, OSDMAXWIDTH, 0, 0), file);
		
		if (imgkLoad.fileSize() == 0)
		{
			loadingFailed = true;
			return false;
		}
		else
		{
			height = imgkLoad.rows();
			width = imgkLoad.columns();
			
			loadingFailed = false;
			return true;
		}
	}
	catch(exception &error)
   {
		loadingFailed = true;
		return false;
	}
}


void cOSDImageBitmap::RotateImageMagick(Image &imgkRotate, int dir)
{
	if (!loadingFailed)
	{
		if (dir == 0)
		{
			imgkRotate.rotate(-orientation * 90);
			orientation = 0;
		}
		else
		{
			orientation += dir;
			orientation &= 0x03; // 00000011
			
			imgkRotate.rotate(dir * 90);
		}
		
		width = imgkRotate.columns();
		height = imgkRotate.rows();
	}
}


void cOSDImageBitmap::QuantizeImageMagick(Image &imgkQuant, int colors, bool dither)
{
	//imgkQuant.quantizeColorSpace(YUVColorspace);
	imgkQuant.quantizeColors(colors);
	imgkQuant.quantizeDither(dither);
	imgkQuant.quantize();
	//imgkQuant.colorSpace(RGBColorspace);
}


void cOSDImageBitmap::SaveImageMagick(Image &imgkSave, const char *file)
{
	imgkSave.write(file);
}

// --- END OF IMAGEMAGICK ROUTINES ---------------------------------------------



// --- NETPBM ROUTINES ---------------------------------------------------------

void cOSDImageBitmap::QuantizeNetpbm(Image &imgkQuant, int colors, bool dither)
{
	char buf[256];
	
	
	SaveImageMagick(imgkQuant, TMPIMAGERESIZED);
	snprintf(buf, sizeof(buf), NETPBM_CMD, TMPIMAGERESIZED, dither ? "-fs" : "", colors, TMPIMAGEQUANTIZED);
	system (buf);
}

// --- END OF NETPBM ROUTINES --------------------------------------------------



// --- TOOLS -------------------------------------------------------------------

void cOSDImageBitmap::CalcNewPicSize(int *wNew, int *hNew, int wPic, int hPic, int wWindow, int hWindow, int colors)
{
	double arPic, arWindow;
	int wTmp, hTmp;
	
	
	// calculate the resizing-parameter
	arPic = (double)wPic / (double)hPic;
	arWindow = (double)wWindow / (double)hWindow;
	
	if (arPic>arWindow)
	{
		// width is limited
		wTmp = wWindow;
		// +0.5 is for rounding (only for positive numbers valid)
		hTmp = (int)( (double)hPic * (double)wWindow / (double)wPic + 0.5);
	}
	else
	{
		// height is limited
		hTmp = hWindow;
		// +0.5 is for rounding (only for positive numbers valid)
		wTmp = (int)( (double)wPic * (double)hWindow / (double)hPic + 0.5);
	}
	
	// We have to ensure, that the (width * BitsPerPixel) is an integer
	// (colors = 2 is left out)
	if (colors <= 4)
	{
		wTmp = (wTmp >> 2) << 2;
	}
	else if (colors <= 16)
	{
		wTmp = (wTmp >> 1) << 1;
	}
	
	*wNew = wTmp;
	*hNew = hTmp;
}


void cOSDImageBitmap::ConvertImgk2Bmp(cBitmap &bmp, Image &imgkConv, int alpha)
{
	int w = Width();
	int h = Height();
	tColor col;
	
	
	bmp.SetSize(w, h);
	bmp.SetBpp(OSDImageSetup.have256colors ? 8 : 4);

	const PixelPacket *pixels = imgkConv.getConstPixels(0, 0, w, h);

	for (int iy = 0; iy < h; iy++)
	{
		for (int ix = 0; ix < w; ix++)
		{
			col = (alpha << 24) 
				| ( (pixels->red * 255 / MaxRGB) << 16) 
				| ( (pixels->green * 255 / MaxRGB) << 8) 
				| ( (pixels->blue * 255 / MaxRGB) );
			bmp.DrawPixel(ix, iy, col);
			pixels++;
		}
	}
	
	/*
	// Code for saving resized and requantized image
	
	char buf[256];
	char buf1[256];
	
	strcpy(buf1, imgkImage.fileName().c_str() );

	snprintf(buf, sizeof(buf), "/mm/test/Sample_%c_16_IM_dith.png", *(strrchr(buf1, ' ') + 1) );
	printf("%s\n", buf);
	SaveImageMagick(imgkConv, buf);
	*/
}
	
// --- END OF TOOLS ------------------------------------------------------------

