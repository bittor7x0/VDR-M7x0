#ifndef __PILOTBMP_H
#define __PILOTBMP_H
#include <vdr/osd.h>
#include <vdr/config.h>

class cBmp: public cBitmap {
private:
  cOsd *osd;
  const cFont *font;
  enum { NUMSYMBOLS = 3 };
  typedef unsigned long tPixelData;
  struct tCharData {
      tPixelData width, height;
      tPixelData lines[1];
  };
  const tCharData *data[NUMSYMBOLS];
public:
  cBmp(int Width, int Height);
  ~cBmp();
  void SetPalette(int palette[16]);
  void DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font);
  void DrawSymbol(int x, int y, char *SymbolName[], tColor color);

};

#endif //__PILOTBMP_H
