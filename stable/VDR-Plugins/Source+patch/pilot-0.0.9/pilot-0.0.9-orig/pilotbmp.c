/*
 * pilotbmp.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
 
#include "pilotbmp.h"
#include <vdr/font.h>
#include <vdr/config.h>

cBmp::cBmp(int Width, int Height):cBitmap(Width, Height, 4, true) {
  font = cFont::GetFont(eDvbFont(0));
}

cBmp::~cBmp() {
}

void cBmp::SetPalette(int palette[16])
{
   for (int i = 0; i < 15; i++)
   {
     SetColor(i, palette[i]);
   }
}
void cBmp::DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font)
{
    // adapted from VDR source in osdbase.c
     char fg = Index(ColorFg);
     int h = font->Height(s);
     while (s && *s) {
           const cFont::tCharData *CharData = font->CharData(*s++);
/*           if (int(x + CharData->width) > width)
              break;*/
           for (int row = 0; row < h; row++) {
               cFont::tPixelData PixelData = CharData->lines[row];
               for (int col = CharData->width; col-- > 0; ) {
           if (PixelData & 1)
                   SetIndex(x + col, y + row, fg);
                   PixelData >>= 1;
                   }
               }
           x += CharData->width;
           }
}


  void cBmp::DrawSymbol(int x, int y, char *SymbolName[], tColor color)
{
  cBitmap bm(SymbolName);
  osd->DrawBitmap(x, y, bm, color );
}


