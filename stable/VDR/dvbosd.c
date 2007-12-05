/*
 * dvbosd.c: Implementation of the DVB On Screen Display
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "dvbosd.h"
//M7X0 BEGIN AK
// Nearly the complete File is changed.
// OSD is done via Linux Framebuffer-Device
// At the moment runs 720x576@32-Bit or 720x576@8-Bit Indexed Interlaced


#include <signal.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <sys/mman.h>

#include "tools.h"
#include <errno.h>

// --- cDvbOsd ---------------------------------------------------------------
#ifdef USE_32BPP_FRAMEBUFFER
class cDvbOsd : public cOsd {
private:
	uint32_t *fbMem;
	int fbWidth;
	int fbHeight;
	int fbBpp;
	bool fbInterlaced;
	bool shown;

public:
	cDvbOsd(int Left, int Top, uint32_t *FbMem ,
			int FbWidth, int FbHeight, int FbBpp,bool FbInterlaced);
	virtual ~cDvbOsd();
	virtual void Flush(void);
  };

cDvbOsd::cDvbOsd(int Left, int Top, uint32_t *FbMem,int FbWidth,
	int FbHeight,int FbBpp, bool FbInterlaced)
:cOsd(Left, Top)
{
	shown = false;

	fbMem = FbMem;
	fbWidth=FbWidth;
	fbHeight=FbHeight;
	fbBpp=FbBpp;
	fbInterlaced=FbInterlaced;

}

cDvbOsd::~cDvbOsd()
{
	if (shown) {
		for (int i=0, i2= (fbHeight>>1) * fbWidth;
				i2 < fbWidth*fbHeight ; i+=fbWidth, i2+=fbWidth){
			memset(fbMem+i,0,fbWidth*(fbBpp>>3));
			memset(fbMem+i2,0,fbWidth*(fbBpp>>3));
		}
		msync(fbMem,fbWidth*fbHeight*(fbBpp>>3),MS_ASYNC);
	}
}

void cDvbOsd::Flush(void)
{
	cBitmap *Bitmap;
	int x1, y1, y1Field1, y1Field2;
	// Used for the Dirty-Rectangle in the Bitmap.
	int x1BitmapDirty, x2BitmapDirty, y1BitmapDirty, y2BitmapDirty;
	int i,j,k;
	int NumColors;
	int bitmapWidth;
	const tColor *Colors;
	const tIndex *bitmapData;

	shown = true;

	for (i=0; (Bitmap = GetBitmap(i)) != NULL; i++) {
		if(!Bitmap->Dirty(x1BitmapDirty, y1BitmapDirty,
				x2BitmapDirty , y2BitmapDirty))
			continue;

		// Clear Dirty-Area
		Bitmap->Clean();

		Colors = Bitmap->Colors(NumColors);
		bitmapData = Bitmap->Data(0,0);

		if(!Colors || !bitmapData)
			continue;

		bitmapWidth= Bitmap->Width();

		x1 = Bitmap->X0() + Left();
		y1 = y1BitmapDirty + Bitmap->Y0() + Top();

		if((y1&1)==0) {
			// Bitmaps First Dirty Line is in the first Field
			y1Field1= (y1 >> 1)*fbWidth;
			y1Field2= ((fbHeight>>1)+(y1>>1))*fbWidth;
		} else {
			// Bitmaps First Dirty Line is in the second Field
			y1Field1= ((fbHeight>>1)+(y1>>1))*fbWidth;
			y1Field2= ((y1 >> 1)+1)*fbWidth;
		}

		y1Field1+=x1;
		y1Field2+=x1;

    	for (j = y1BitmapDirty*bitmapWidth ;
				y1BitmapDirty < y2BitmapDirty;
				y1BitmapDirty+=2, j += bitmapWidth<<1,
				y1Field1 += fbWidth , y1Field2 += fbWidth) {
			// Maybe this loop is faster if splited up in two (for each field)
			// This can happen due to caching of the CPU
			for (k=x1BitmapDirty ; k <= x2BitmapDirty; k++){
				fbMem[y1Field1+k]=Colors[bitmapData[j+k]];
				fbMem[y1Field2+k]=Colors[bitmapData[j+k+bitmapWidth]];
			}
		}

		if ( y1BitmapDirty == y2BitmapDirty ){
			// Copy last Dirty-Line of Bitmap
			for (k=x1BitmapDirty ; k <= x2BitmapDirty; k++){
				fbMem[y1Field1+k]=Colors[bitmapData[j+k]];
			}
		}


	}

	msync(fbMem,fbWidth*fbHeight*(fbBpp>>3),MS_ASYNC);

}
#else

class cDvbOsd : public cOsd {
private:
  int fb_fd;
  uint8_t *fbMem;
  bool shown;
  struct fb_cmap *palette;
  int *paletteUses;
  bool clearOnStop;
  bool overlay;
  void SetFbPalette(const tColor *Colors, int NumColors);
public:
  cDvbOsd(int Left, int Top, int Fb_fd, uint8_t *FbMem,
          struct fb_cmap *Palette, int *PaletteUses, bool ClearOnStop, bool Overlay);
  virtual ~cDvbOsd();
  virtual eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
  virtual eOsdError SetAreas(const tArea *Areas, int NumAreas);
  virtual void Flush(void);
  };

cDvbOsd::cDvbOsd(int Left, int Top, int Fb_fd, uint8_t *FbMem,
 struct fb_cmap *Palette, int *PaletteUses, bool ClearOnStop, bool Overlay)
 : cOsd(Left, Top)
{
  shown = false;
  fb_fd = Fb_fd;
  fbMem = FbMem;
  palette = Palette;
  paletteUses = PaletteUses;
  clearOnStop = ClearOnStop;
  overlay = Overlay;
}

cDvbOsd::~cDvbOsd()
{
  if (shown && clearOnStop) {
     for (int i=0; i < (720 * 576) / 2; i += 720) {
         memset(fbMem + i, 0xFF, 720);
         memset(fbMem + i + ((720 * 576) / 2), 0xFF, 720);
         }
     msync(fbMem,720 * 576, MS_ASYNC);
     }
 //dsyslog("OSD DELETED!");
}

eOsdError cDvbOsd::CanHandleAreas(const tArea *Areas, int NumAreas)
{
  return ( (NumAreas <= 1) ? cOsd::CanHandleAreas(Areas,NumAreas) : oeTooManyAreas );
}

eOsdError cDvbOsd::SetAreas(const tArea *Areas, int NumAreas)
{
  eOsdError r = cOsd::SetAreas(Areas, NumAreas);
  if (r != oeOk || !*paletteUses || NumAreas < 1)
     return r;

  cBitmap *Bitmap = GetBitmap(0); // Only one area allowed.
  for (int i = 0; i < *paletteUses; i++) {
      const tColor color = ((palette->blue[i]  & 0xFF00) >> 8) |
                           ((palette->green[i] & 0xFF00)     ) |
                           ((palette->red[i]   & 0xFF00) << 8) |
            ((0xFF - ((palette->transp[i] >> 8) & 0xFF)) << 24);
      Bitmap->SetColor(i, color);
      }
  Bitmap->PaletteModified();
  if (overlay) {
     tIndex *bitmapBuf = Bitmap->GetBuffer();
     const int bitmapWidth = Bitmap->Width();
     const int bitmapHeight = Bitmap->Height();
     const int x = Left() + Bitmap->X0();
     const int y = Top() + Bitmap->Y0();

     const tIndex *const end = bitmapBuf + bitmapHeight * bitmapWidth;

     uint8_t *fbp = fbMem + ((y + 1) >> 1) * 720 + x;
     tIndex *start = bitmapBuf + (y & 1) * bitmapWidth;
     while (start < end) {
           memcpy(start, fbp, bitmapWidth);
           fbp += 720;
           start += (bitmapWidth << 1);
           }

     fbp = fbMem + 720 * (576 / 2) + (y >> 1) * 720 + x;
     start = bitmapBuf + ((y + 1) & 1) * bitmapWidth;
     while (start < end) {
           memcpy(start, fbp, bitmapWidth);
           fbp += 720;
           start += (bitmapWidth << 1);
           }

     }
  return r;
}

void cDvbOsd::SetFbPalette(const tColor *Colors, int NumColors)
{
  // This is wired ,driver seems to expect little endian format
  // for palette. Only 8 bits are used.
  for (int i = 0; i < NumColors; i++) {
      palette->blue[i] = (Colors[i] & 0xFF) << 8;
      palette->green[i] = (Colors[i] & 0xFF00);
      palette->red[i] = (Colors[i] >> 8) & 0xFF00;
      palette->transp[i] = (0xFF - (Colors[i] >> 24)) << 8;
      }
  *paletteUses = NumColors;

  // Works only correct if we set the full palette
  // otherwise you get a pink osd (don't know why may be the box likes pink ;))
  CHECK(ioctl(fb_fd, FBIOPUTCMAP, palette));
}

void cDvbOsd::Flush(void)
{
  cBitmap *Bitmap = GetBitmap(0); // Only one area allowed.

  if (!Bitmap)
     return;

  int x1BitmapDirty, x2BitmapDirty, y1BitmapDirty, y2BitmapDirty;
  if (!Bitmap->Dirty(x1BitmapDirty, y1BitmapDirty, x2BitmapDirty , y2BitmapDirty))
     return;

  Bitmap->Clean();



  if (Bitmap->PaletteModified()) {
     int NumColors;
     const tColor *Colors = Bitmap->Colors(NumColors);
     if (!Colors || NumColors <= 0)
        return;
     SetFbPalette(Colors,NumColors);
     }


  const tIndex *bitmapData = Bitmap->Data(x1BitmapDirty, y1BitmapDirty);
  if (!bitmapData)
     return;

  shown = true;
  int bitmapWidth= Bitmap->Width();

  const int x1 = x1BitmapDirty + Bitmap->X0() + Left();
  const int y1 = y1BitmapDirty + Bitmap->Y0() + Top();

  int y1Field1, y1Field2;
  if ((y1 & 1) == 0) {
     // Bitmaps First Dirty Line is in the first Field
     y1Field1 = (y1 >> 1) * 720;
     y1Field2 = ((y1 >> 1) + (576 / 2)) * 720;
     }
  else {
     // Bitmaps First Dirty Line is in the second Field
     y1Field1 = ((y1 >> 1) + (576 / 2)) * 720;
     y1Field2 = ((y1 >> 1) + 1) * 720;
     }

  y1Field1 += x1;
  y1Field2 += x1;


  x2BitmapDirty -= x1BitmapDirty -1;
  y2BitmapDirty -= y1BitmapDirty;
  const tIndex *const bitmapDataLimit = bitmapData + y2BitmapDirty * bitmapWidth;

  for (;bitmapData < bitmapDataLimit; y1Field1 += 720, y1Field2 += 720,
                                             bitmapData += bitmapWidth) {
      memcpy(fbMem + y1Field1, bitmapData, x2BitmapDirty);
      bitmapData += bitmapWidth;
      memcpy(fbMem + y1Field2, bitmapData, x2BitmapDirty);

      }

  // Copy last dirty line
  if (bitmapData == bitmapDataLimit)
     memcpy(fbMem + y1Field1, bitmapData, x2BitmapDirty);

  msync(fbMem, 720 * 576, MS_ASYNC);
  //dsyslog("OSD FLUSHED!");
}
#endif
// --- cDvbOsdProvider -------------------------------------------------------

cDvbOsdProvider::cDvbOsdProvider(int OsdDev, bool ClearOnStart,
                                bool ClearOnStop, bool Overlay)
{
  osdDev = OsdDev;
  fbMem=NULL;
  clearOnStart = ClearOnStart;
  clearOnStop = ClearOnStop;
  overlay = Overlay && !ClearOnStart;
  if (osdDev < 0) {
     esyslog("ERROR: illegal OSD device handle (%d)!", osdDev);
     return;
     }

  struct fb_var_screeninfo scrInf;
  if (ioctl(osdDev,FBIOGET_VSCREENINFO,&scrInf) < 0) {
     esyslog("ERROR: cannot get Framebuffer-Settings !");
     return;
     }

  fbBppSaved=scrInf.bits_per_pixel;
  if (clearOnStart ||
#ifdef USE_32BPP_FRAMEBUFFER
      fbBppSaved != 32) {
     scrInf.bits_per_pixel=32;
#else
      fbBppSaved != 8) {
     scrInf.bits_per_pixel=8;
#endif
     clearOnStart = true;
     if (ioctl(osdDev,FBIOPUT_VSCREENINFO,&scrInf) < 0) {
        esyslog("ERROR: cannot get Framebuffer-Settings !");
        return;
        }

     //Reget it. values may have been corrected.
     if (ioctl(osdDev,FBIOGET_VSCREENINFO,&scrInf) < 0) {
        esyslog("ERROR: cannot get Framebuffer-Settings !");
        return;
        }
     }

  fbWidth=scrInf.xres;
  fbHeight=scrInf.yres;
  fbBpp=scrInf.bits_per_pixel;
  fbInterlaced=scrInf.vmode&FB_VMODE_INTERLACED;
#ifdef USE_32BPP_FRAMEBUFFER
  fbMem= (uint32_t *) mmap(0, fbWidth * fbHeight * (fbBpp>>3),
                                       PROT_READ | PROT_WRITE,
                                       MAP_SHARED, osdDev, 0);
#else
  fbMem= (uint8_t *) mmap(0, fbWidth * fbHeight * (fbBpp>>3),
                                       PROT_READ | PROT_WRITE,
                                       MAP_SHARED, osdDev, 0);
#endif

  if (fbMem == MAP_FAILED) {
      esyslog("ERROR: cannot mmap Framebuffer (%d)!",errno);
      fbMem=NULL;
      }

#ifdef USE_32BPP_FRAMEBUFFER
  if (!fbMem || fbWidth != 720 || fbHeight != 576 || fbBpp != 32 || !fbInterlaced) {
#else
  if (!fbMem || fbWidth != 720 || fbHeight != 576 || fbBpp != 8 || !fbInterlaced) {
#endif
     esyslog("ERROR: illegal or unsupported framebuffer: %u x %u x %u !",
                                                   fbWidth,fbHeight,fbBpp);
     if (fbMem) {
        munmap(fbMem, fbWidth * fbHeight * (fbBpp>>3));
        }
     return;
     }

#ifdef USE_32BPP_FRAMEBUFFER
  if (clearOnStart) {
     memset(fbMem, 0, fbWidth * fbHeight * 4);
     msync(fbMem, fbWidth * fbHeight * 4, MS_ASYNC);
     }
#else
  palette.start = 0;
  palette.len = 256;
  palette.red = red;
  palette.green = green;
  palette.blue = blue;
  palette.transp = trans;
  if (clearOnStart) {
     memset(fbMem, 0xFF, fbWidth * fbHeight);
     memset(red,0,256 * sizeof(uint16_t));
     memset(green,0,256 * sizeof(uint16_t));
     memset(blue,0,256 * sizeof(uint16_t));
     memset(trans,0xFF,256 * sizeof(uint16_t));
     paletteUses = 0;
     if (ioctl(osdDev, FBIOPUTCMAP, &palette) < 0) {
          esyslog("ERROR: cannot set framebuffer palette");
        }
     msync(fbMem, fbWidth * fbHeight, MS_ASYNC);
     }
  else {
     if (ioctl(osdDev, FBIOGETCMAP, &palette) < 0) {
          esyslog("ERROR: cannot get framebuffer palette");
        }
     // Driver has differnet format for getting and setting palettes
     // Yet another bug
     paletteUses = 0;
     for (int i = 0; i < 256; i++) {
         trans[i] = 0xFF - (trans[i] >> 8);
         if (trans[i] < 0xFF)
            paletteUses = i + 1;
         trans[i] = trans[i] << 8;
         }
     trans[255] = 255 << 8;
     red[255] = green[255] = blue[255] = 0;
     }
#endif

}

cDvbOsdProvider::~cDvbOsdProvider(){
  struct fb_var_screeninfo scrInf;
  if (ioctl(osdDev,FBIOGET_VSCREENINFO,&scrInf) < 0) {
     esyslog("ERROR: cannot get Framebuffer-Settings !");
     return;
     }

  if (fbMem){
     munmap(fbMem, fbWidth * fbHeight * (fbBpp>>3));
     }

  if (clearOnStop) {
     const int startSize = fbWidth * fbHeight * (fbBppSaved>>3);
     uint8_t buf[startSize];
     memset(buf, 0, startSize);
     write(osdDev, buf, startSize);
     scrInf.bits_per_pixel = fbBppSaved;
     if (ioctl(osdDev,FBIOPUT_VSCREENINFO,&scrInf) < 0) {
        esyslog("ERROR: cannot get Framebuffer-Settings !");
        return;
        }
#ifndef USE_32BPP_FRAMEBUFFER
     if (fbBppSaved == 8) {
        memset(red,0,256 * sizeof(uint16_t));
        memset(green,0,256 * sizeof(uint16_t));
        memset(blue,0,256 * sizeof(uint16_t));
        memset(trans,0xFF,256 * sizeof(uint16_t));

        if (ioctl(osdDev, FBIOPUTCMAP, &palette) < 0) {
           esyslog("ERROR: cannot set framebuffer palette");
           }
        }
#endif

     // memset(fbMem, 0, fbWidth * fbHeight * (fbBpp>>3));
     }


}

cOsd *cDvbOsdProvider::CreateOsd(int Left, int Top)
{
#ifdef USE_32BPP_FRAMEBUFFER
  return new cDvbOsd(Left, Top, fbMem, fbWidth, fbHeight, fbBpp, fbInterlaced);
#else
  if (fbMem && fbWidth == 720 && fbHeight == 576 && fbBpp == 8 && fbInterlaced) {
     if (clearOnStart)
        paletteUses = 0;
     return new cDvbOsd(Left, Top, osdDev, fbMem, &palette, &paletteUses, clearOnStop, overlay);
     }

  esyslog("ERROR: illegal or unsupported framebuffer: %u x %u x %u !",
                                               fbWidth,fbHeight,fbBpp);
  return new cOsd(Left, Top);

#endif

}

//M7X0 END AK
