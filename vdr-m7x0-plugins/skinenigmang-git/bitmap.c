#ifdef HAVE_IMAGEMAGICK

#include "common.h"
#include "bitmap.h"
#include "config.h"

#ifndef X_DISPLAY_MISSING
#define X_DISPLAY_MISSING
#endif

#undef debug
#include <Magick++.h>
using namespace Magick;

#include <vector>

#include <vdr/themes.h>

void cOSDImageBitmap::Init(void)
{
  InitializeMagick(NULL);
}
 
cOSDImageBitmap::cOSDImageBitmap()
{}


cOSDImageBitmap::~cOSDImageBitmap()
{}

bool cOSDImageBitmap::DrawImage(const char *fileNameP, int x, int y, int w, int h, int colors, cBitmap *bmp)
{
  return DrawMagick(fileNameP, x, y, w, h, colors, bmp);
}

bool cOSDImageBitmap::DrawMagick(const char *Filename, int x, int y, int width, int height, int colors, cBitmap *bmp)
{
  Image image;
  try {
    Geometry geo;
    image.read(Filename);
    geo = image.size();
    int w = geo.width();
    int h = geo.height();
    if (height != h || width != w) {
      switch (EnigmaConfig.resizeImages) {
        case 0:
          image.sample(Geometry(width, height));
          break;
        case 1:
          image.scale(Geometry(width, height));
          break;
        case 2:
          image.zoom(Geometry(width, height));
          break;
        default:
          esyslog("ERROR: unknown resize mode %d", EnigmaConfig.resizeImages);
          break;
      }
    }
    geo = image.size();
    w = geo.width();
    h = geo.height();
    if (colors != 0){
      image.opacity(OpaqueOpacity);
      image.backgroundColor(Color(0, 0, 0, 0));
      image.quantizeColorSpace(RGBColorspace);
      image.quantizeColors(colors);
      image.quantize();
    }

    // center image
    x += ((width - w) / 2);
    y += ((height - h) / 2);

    const PixelPacket *pix = image.getConstPixels(0, 0, w, h);
    for (int iy = 0; iy < h; ++iy) {
      for (int ix = 0; ix < w; ++ix) {
        tColor col = (~(int)(pix->opacity * 255 / MaxRGB) << 24)
                     | ((int)(pix->red * 255 / MaxRGB) << 16)
                     | ((int)(pix->green * 255 / MaxRGB) << 8)
                     | (int)(pix->blue * 255 / MaxRGB);
        bmp->DrawPixel(x + ix, y + iy, col);
        ++pix;
      }
    }
  } catch (Exception &e) {
    esyslog("ERROR: Couldn't load %s: %s", Filename, e.what());
    return false;
  } catch (...) {
    esyslog("ERROR: Couldn't load %s: Unknown exception caught", Filename);
    return false;
  }
  return true;
}
#endif //HAVE_IMAGEMAGICK
// vim:et:sw=2:ts=2:
