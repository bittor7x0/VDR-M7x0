/*
 * dvbosd.c: Implementation of the DVB On Screen Display
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: dvbosd.c 289 2008-09-09 15:07:06Z andreas $
 */

#include "dvbosd.h"
//M7X0 BEGIN AK
// Nearly the complete File is changed.
// OSD is done via Linux Framebuffer-Device
// At the moment runs 720x576@8-Bit Indexed Interlaced


#include <signal.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <sys/mman.h>

#include "tools.h"
#include <errno.h>

// --- cDvbOsd ---------------------------------------------------------------
class cDvbOsd : public cOsd {
private:
  int fb_fd;
  uint8_t *fbMem;
  bool shown;
  void SetFbPalette(const tColor *Colors, int NumColors);
public:
  cDvbOsd(int Left, int Top, int Fb_fd, uint8_t *FbMem);
  virtual ~cDvbOsd();
  virtual eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
  virtual void Flush(void);
  };

cDvbOsd::cDvbOsd(int Left, int Top, int Fb_fd, uint8_t *FbMem)
 : cOsd(Left, Top)
{
  shown = false;
  fb_fd = Fb_fd;
  fbMem = FbMem;
}

cDvbOsd::~cDvbOsd()
{
  if (shown) {
     uint8_t *f1;
     uint8_t *f2;
     f1 = f2 = fbMem;
     f2 += (576 / 2) * 720;
     const uint8_t *const f1end = f2;
     while (f1 < f1end) {
         memset(f1, 0xFF, 720);
         memset(f2, 0xFF, 720);
         f1 += 720; f2 +=720;
         }
     msync(fbMem,720 * 576, MS_ASYNC);
     }
 //dsyslog("OSD DELETED!");
}

eOsdError cDvbOsd::CanHandleAreas(const tArea *Areas, int NumAreas)
{
  return ( (NumAreas <= 1) ? cOsd::CanHandleAreas(Areas,NumAreas) : oeTooManyAreas );
}

void cDvbOsd::SetFbPalette(const tColor *Colors, int NumColors)
{
  // This is wired ,driver seems to expect little endian format
  // for palette. Only 8 bits are used.
  uint16_t red[256];
  uint16_t green[256];
  uint16_t blue[256];
  uint16_t trans[256];

  for (int i = 0; i < NumColors; i++) {
      blue[i] = (Colors[i] & 0xFF) << 8;
      green[i] = (Colors[i] & 0xFF00);
      red[i] = (Colors[i] >> 8) & 0xFF00;
      trans[i] = (0xFF - (Colors[i] >> 24)) << 8;
      }
  blue[255] = green[255] = red[255] = 0;
  trans[255] = 0xFF00;

  // Works only correct if we set the full palette
  // otherwise you get a pink osd (don't know why may be the box likes pink ;))
  struct fb_cmap palette;
  palette.start = 0;
  palette.len = 256;
  palette.red = red;
  palette.green = green;
  palette.blue = blue;
  palette.transp = trans;
  CHECK(ioctl(fb_fd, FBIOPUTCMAP, &palette));
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

  uint8_t *y1Field1;
  uint8_t *y1Field2;
  y1Field2 = y1Field1 = fbMem + (y1 >> 1) * 720;
  if (!(y1 & 1)) {
     // Bitmaps First Dirty Line is in the first Field
     y1Field2 += (576 / 2) * 720;
     }
  else {
     // Bitmaps First Dirty Line is in the second Field
     y1Field1 += (576 / 2) * 720;
     y1Field2 += 720;
     }

  y1Field1 += x1;
  y1Field2 += x1;


  x2BitmapDirty -= x1BitmapDirty -1;
  y2BitmapDirty -= y1BitmapDirty;
  const tIndex *const bitmapDataLimit = bitmapData + y2BitmapDirty * bitmapWidth;

  while (bitmapData < bitmapDataLimit) {
        memcpy(y1Field1, bitmapData, x2BitmapDirty);
        bitmapData += bitmapWidth;
        memcpy(y1Field2, bitmapData, x2BitmapDirty);
        bitmapData += bitmapWidth;

        y1Field1 += 720;
        y1Field2 += 720;
        }

  // Copy last dirty line
  if (bitmapData == bitmapDataLimit)
     memcpy(y1Field1, bitmapData, x2BitmapDirty);

  msync(fbMem, 720 * 576, MS_ASYNC);
  //dsyslog("OSD FLUSHED!");
}
// --- cDvbOsdProvider -------------------------------------------------------

cDvbOsdProvider::cDvbOsdProvider(int OsdDev)
{
  osdDev = OsdDev;
  fbMem=NULL;
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
  if (fbBppSaved != 8) {
     scrInf.bits_per_pixel=8;

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
  fbInterlaced=(scrInf.vmode&FB_VMODE_INTERLACED) == FB_VMODE_INTERLACED;
  fbMem= (uint8_t *) mmap(0, fbWidth * fbHeight,
                                       PROT_READ | PROT_WRITE,
                                       MAP_SHARED, osdDev, 0);

  if (fbMem == MAP_FAILED) {
     esyslog("ERROR: cannot mmap Framebuffer (%d)!",errno);
     fbMem=NULL;
     return;
     }

  if ((fbWidth != 720) | (fbHeight != 576) | (fbBpp != 8) | !fbInterlaced) {
     esyslog("ERROR: illegal or unsupported framebuffer: %u x %u x %u !",
                                                   fbWidth,fbHeight,fbBpp);
     munmap(fbMem, fbWidth * fbHeight * (fbBpp>>3));
     return;
     }

  uint16_t zeros[256];
  uint16_t trans[256];

  memset(zeros,0,256 * sizeof(uint16_t));
  memset(trans,0xFF,256 * sizeof(uint16_t));

  struct fb_cmap palette;
  palette.start = 0;
  palette.len = 256;
  palette.red = zeros;
  palette.green = zeros;
  palette.blue = zeros;
  palette.transp = trans;
  if (ioctl(osdDev, FBIOPUTCMAP, &palette) < 0) {
     esyslog("ERROR: cannot set framebuffer palette");
     }
  memset(fbMem, 0xFF, fbWidth * fbHeight);
  msync(fbMem, fbWidth * fbHeight, MS_ASYNC);
}

cDvbOsdProvider::~cDvbOsdProvider(){

  if (fbMem){
     munmap(fbMem, fbWidth * fbHeight);
     }

  if (fbBpp != fbBppSaved) {
     struct fb_var_screeninfo scrInf;
     if (ioctl(osdDev,FBIOGET_VSCREENINFO,&scrInf) < 0) {
        esyslog("ERROR: cannot get Framebuffer-Settings !");
        return;
        }
     scrInf.bits_per_pixel = fbBppSaved;
     if (ioctl(osdDev,FBIOPUT_VSCREENINFO,&scrInf) < 0) {
        esyslog("ERROR: cannot get Framebuffer-Settings !");
        return;
        }
     }

  if (fbBppSaved == 8) {
     uint16_t zeros[256];
     uint16_t trans[256];

     memset(zeros,0,256 * sizeof(uint16_t));
     memset(trans,0xFF,256 * sizeof(uint16_t));

     struct fb_cmap palette;
     palette.start = 0;
     palette.len = 256;
     palette.red = zeros;
     palette.green = zeros;
     palette.blue = zeros;
     palette.transp = trans;

     if (ioctl(osdDev, FBIOPUTCMAP, &palette) < 0) {
        esyslog("ERROR: cannot set framebuffer palette");
        }
     }

  const int startSize = fbWidth * fbHeight * (fbBppSaved>>3);
  uint8_t buf[startSize];
  memset(buf, 0, startSize);
  write(osdDev, buf, startSize);
}

cOsd *cDvbOsdProvider::CreateOsd(int Left, int Top)
{
  if ((fbMem != NULL) & (fbWidth == 720) & (fbHeight == 576) & (fbBpp == 8) & fbInterlaced) {
     return new cDvbOsd(Left, Top, osdDev, fbMem);
     }

  esyslog("ERROR: illegal or unsupported framebuffer: %u x %u x %u !",
                                               fbWidth,fbHeight,fbBpp);
  return new cOsd(Left, Top);
}

//M7X0 END AK
