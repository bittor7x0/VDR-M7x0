/*
 * osd.c: Abstract On Screen Display layer
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: osd.c 319 2008-09-28 19:46:15Z andreas $
 */

#include "osd.h"
#include <math.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "tools.h"

// --- cPalette --------------------------------------------------------------

cPalette::cPalette(int Bpp)
{
  SetBpp(Bpp);
}

void cPalette::Reset(void)
{
  numColors = 0;
  modified = false;
}

int cPalette::Index(tColor Color)
{
  for (int i = 0; i < numColors; i++) {
      if (color[i] == Color)
         return i;
      }
  if (numColors < maxColors) {
     color[numColors++] = Color;
     modified = true;
     return numColors - 1;
     }
  dsyslog("too many different colors used in palette");
  //TODO: return the index of the "closest" color?
  return 0;
}

void cPalette::SetBpp(int Bpp)
{
  bpp = Bpp;
  maxColors = 1 << bpp;
//M7X0 BEGIN AK
//M7X0 TODO: Find a cleaner way, for this workaround in case of using 8bpp
// framebuffer.
// We need at least one index which is fully transparent.
// This may be only a problem if the area uses 8bpp palette.
// Allow in this case only 255 colors (last gets reversed for transparency)
// Can we assume background color (index 0) is always transparent,
// if we allow only one area??
  if (bpp == 8)
     maxColors--;

  Reset();
}

void cPalette::SetColor(int Index, tColor Color)
{
  if (Index < maxColors) {
     if (numColors <= Index)
        numColors = Index + 1;
     modified = true;
     color[Index] = Color;
     }
}
//M7X0 END AK
const tColor *cPalette::Colors(int &NumColors)
{
  NumColors = numColors;
  return numColors ? color : NULL;
}

//M7X0 BEGIN AK
void cPalette::Take(const cPalette &Palette, tIndexes *Indexes, tColor ColorFg, tColor ColorBg)
{
  int i = 0;
  if (ColorFg | ColorBg) {
     if (0 < Palette.numColors) {
        int n = Index(ColorBg);
        if (Indexes)
           (*Indexes)[0] = n;
        if (1 < Palette.numColors) {
           n = Index(ColorFg);
           if (Indexes)
              (*Indexes)[1] = n;
           }
        }
     i = 2;
     }
  for (; i < Palette.numColors; i++) {
      tColor Color = Palette.color[i];
      int n = Index(Color);
      if (Indexes)
         (*Indexes)[i] = n;
      }
}

void cPalette::Replace(const cPalette &Palette)
{
  memcpy(color, Palette.color, sizeof(color[0]) *  Palette.numColors);
  modified = true;
  numColors = min(Palette.numColors, maxColors);
}
//M7X0 END AK

// --- cBitmap ---------------------------------------------------------------

cBitmap::cBitmap(int Width, int Height, int Bpp, int X0, int Y0)
:cPalette(Bpp)
{
  bitmap = NULL;
  x0 = X0;
  y0 = Y0;
  SetSize(Width, Height);
}

cBitmap::cBitmap(const char *FileName)
{
  bitmap = NULL;
  x0 = 0;
  y0 = 0;
  LoadXpm(FileName);
}

cBitmap::cBitmap(const char *const Xpm[])
{
  bitmap = NULL;
  x0 = 0;
  y0 = 0;
  SetXpm(Xpm);
}

cBitmap::~cBitmap()
{
  free(bitmap);
}

void cBitmap::SetSize(int Width, int Height)
{
  if (bitmap && Width == width && Height == height)
     return;
  width = Width;
  height = Height;
  free(bitmap);
  bitmap = NULL;
  dirtyX1 = 0;
  dirtyY1 = 0;
  dirtyX2 = width - 1;
  dirtyY2 = height - 1;
  if (width > 0 && height > 0) {
     bitmap = MALLOC(tIndex, width * height);
     if (bitmap)
        memset(bitmap, 0x00, width * height);
     else
        esyslog("ERROR: can't allocate bitmap!");
     }
  else
     esyslog("ERROR: invalid bitmap parameters (%d, %d)!", width, height);
}
//M7X0 BEGIN AK
bool cBitmap::Contains(int x, int y) const
{
  x -= x0;
  y -= y0;
  return (0 <= x) & (x < width) & (0 <= y) & (y < height);
}

bool cBitmap::Covers(int x1, int y1, int x2, int y2) const
{
  x1 -= x0;
  y1 -= y0;
  x2 -= x0;
  y2 -= y0;
  return (x1 <= 0) & (y1 <= 0) & (x2 >= width - 1) & (y2 >= height - 1);
}

bool cBitmap::Intersects(int x1, int y1, int x2, int y2) const
{
  x1 -= x0;
  y1 -= y0;
  x2 -= x0;
  y2 -= y0;
  return !((x2 < 0) | (x1 >= width) | (y2 < 0) | (y1 >= height));
}
//M7X0 END AK
bool cBitmap::Dirty(int &x1, int &y1, int &x2, int &y2)
{
  if (dirtyX2 >= 0) {
     x1 = dirtyX1;
     y1 = dirtyY1;
     x2 = dirtyX2;
     y2 = dirtyY2;
     return true;
     }
  return false;
}

void cBitmap::Clean(void)
{
  dirtyX1 = width;
  dirtyY1 = height;
  dirtyX2 = -1;
  dirtyY2 = -1;
}

//M7X0 BEGIN AK
bool cBitmap::LoadXpm(const char *FileName)
{
  bool Result = true;
  FILE *f = fopen(FileName, "r");

  if (!f) {
     esyslog("ERROR: can't open XPM file '%s'", FileName);
     return false;
     }

  char **Xpm = NULL;
  int lines = 0;
  int index = 0;
  char *s;

  cReadLine ReadLine;

  if ((s = ReadLine.Read(f)) == NULL) {
     esyslog("ERROR: cannot read header in XPM file '%s'", FileName);
     return false;
     }
  s = skipspace(s);
  if (strcmp(s, "/* XPM */") != 0) {
     esyslog("ERROR: invalid header in XPM file '%s'", FileName);
     return false;
     }

  while ((lines == 0) && (s = ReadLine.Read(f)) != NULL) {
        s = skipspace(s);
        if (*s++ != '"')
           continue;
        int w, h, n, c;
        if (4 != sscanf(s, "%d %d %d %d", &w, &h, &n, &c)) {
           esyslog("ERROR: faulty 'values' line in XPM file '%s'", FileName);
           return false;
           }
        char *q = strchr(s, '"');
        if (!q) {
           esyslog("ERROR: missing quotes in XPM file '%s'", FileName);
           return false;
           }
        *q = 0;

        lines = h + n + 1;
        Xpm = MALLOC(char *, lines);
        memset(Xpm, 0, lines * sizeof(char*));
        Xpm[0] = strdup(s);
        index++;
        }

  if (lines == 0)
     return false;

  while ((s = ReadLine.Read(f)) != NULL) {
        s = skipspace(s);
        if (*s++ != '"')
           continue;
        char *q = strchr(s, '"');
        if (!q) {
           esyslog("ERROR: missing quotes in XPM file '%s'", FileName);
           break;
           }
        *q = 0;
        if (index < lines)
           Xpm[index++] = strdup(s);
        else {
           esyslog("ERROR: too many lines in XPM file '%s'", FileName);
           Result = false;
           break;
           }
        }

  if (Result) {
     if (index == lines)
        Result = SetXpm(Xpm);
     else
        esyslog("ERROR: too few lines in XPM file '%s'", FileName);
     }

  if (Xpm) {
     for (int i = 0; i < index; i++)
         free(Xpm[i]);
     }
  free(Xpm);
  fclose(f);
  return Result;
}

bool cBitmap::SetXpm(const char *const Xpm[], bool IgnoreNone)
{
  const char *const *p = Xpm;
  int w, h, n, c;
  if (4 != sscanf(*p, "%d %d %d %d", &w, &h, &n, &c)) {
     esyslog("ERROR: faulty 'values' line in XPM: '%s'", *p);
     return false;
     }
  if (n > MAXNUMCOLORS) {
     esyslog("ERROR: too many colors in XPM: %d", n);
     return false;
     }

  SetBpp(8);
  SetSize(w, h);

  int NoneColorIndex = MAXNUMCOLORS;

  if (c == 1) {
     int colorIndex[256];
     memset(colorIndex, 0xff, sizeof(int) * 256);

     for (int i = 0; i < n; i ++) {
         const char *s = *++p;
         const int ci = (uint8_t) *s;
         if (!ci) {
            esyslog("ERROR: faulty 'colors' line in XPM: '%s'", s);
            return false;
            }
         s = skipspace(s + 1);
         if (*s != 'c') {
             esyslog("ERROR: unknown color key in XPM: '%c'", *s);
             return false;
             }
         s = skipspace(s + 1);
         tColor color = 0;
         if (NoneColorIndex == MAXNUMCOLORS && strcasecmp(s, "none") == 0) {
            NoneColorIndex = i;
            if (IgnoreNone)
               continue;
            if (i) {
               esyslog("WARNING: XPM none color Index %d != 0", i);
               }
            color = clrTransparent;
            }
         else if (*s != '#') {
            esyslog("ERROR: unknown color code in XPM: '%c'", *s);
            return false;
            }
         else
            color = strtoul(++s, NULL, 16) | 0xFF000000;
         int ind = i;
         if ((IgnoreNone == true) & (i > NoneColorIndex))
            ind = i - 1;
         colorIndex[ci] = ind;
         SetColor(ind, color);
         }

     for (int y = 0; y < h; y++) {
         const char *s = *++p;
         int x;
         for (x = 0; (x < w) & (*s != 0); x++, s++) {
             const int i = colorIndex[(uint8_t) *s];
             if (i == -1) {
                esyslog("ERROR: undefined pixel color in XPM: %d %d '%s'", x, y, s);
                return false;
                }
             if (i == NoneColorIndex)
                NoneColorIndex = MAXNUMCOLORS;
             SetIndexRaw(x, y, i);
             }
         if (x < w) {
            esyslog("ERROR: faulty 'colors' line in XPM: '%s'", *p);
            return false;
            }
         }
     if (NoneColorIndex < MAXNUMCOLORS && !IgnoreNone)
        return SetXpm(Xpm, true);
     return true;
     }

  for (int i = 0; i < n; i++) {
      const char *s = *++p;
      if (int(strlen(s)) < c) {
         esyslog("ERROR: faulty 'colors' line in XPM: '%s'", s);
         return false;
         }
      s = skipspace(s + c);
      if (*s != 'c') {
         esyslog("ERROR: unknown color key in XPM: '%c'", *s);
         return false;
         }
      s = skipspace(s + 1);
      tColor color = 0;
      if (NoneColorIndex == MAXNUMCOLORS && strcasecmp(s, "none") == 0) {
         // s = "#00000000";
         NoneColorIndex = i;
         if (IgnoreNone)
            continue;
         if (i) {
            esyslog("WARNING: XPM none color Index %d != 0", i);
            }
         color = clrTransparent;
         }
      else if (*s != '#') {
         esyslog("ERROR: unknown color code in XPM: '%c'", *s);
         return false;
         }
      else
         color = strtoul(++s, NULL, 16) | 0xFF000000;
      int ind = i;
      if ((IgnoreNone == true) & (i > NoneColorIndex))
         ind = i - 1;
      SetColor(ind, color);
      }
  const int len = w * c;
  for (int y = 0; y < h; y++) {
      const char *s = *++p;
      if (int(strlen(s)) != len) {
         esyslog("ERROR: faulty pixel line in XPM: %d '%s'", y, s);
         return false;
         }

      for (int x = 0; x < w; x++) {
          int i;
          for (i = 0; i < n; i++) {
              if (strncmp(Xpm[i + 1], s, c) == 0) {
                 if (i == NoneColorIndex)
                    NoneColorIndex = MAXNUMCOLORS;
                 int ind = i;
                 if ((IgnoreNone == true) & (i > NoneColorIndex))
                    ind = i - 1;
                 SetIndexRaw(x, y, ind);
                 break;
                 }
              }
          if (i == n) {
             esyslog("ERROR: undefined pixel color in XPM: %d %d '%s'", x, y, s);
             return false;
             }
          s += c;
          }
//M7X0 END AK
      }
  if (NoneColorIndex < MAXNUMCOLORS && !IgnoreNone)
     return SetXpm(Xpm, true);
  return true;
}

inline void cBitmap::SetIndexRaw(int x, int y, tIndex Index)
{
  bitmap[y*width+x]=Index;
}

void cBitmap::SetIndex(int x, int y, tIndex Index)
{
  if (bitmap) {
     if (0 <= x && x < width && 0 <= y && y < height) {
        if (bitmap[width * y + x] != Index) {
           bitmap[width * y + x] = Index;
           if (dirtyX1 > x)  dirtyX1 = x;
           if (dirtyY1 > y)  dirtyY1 = y;
           if (dirtyX2 < x)  dirtyX2 = x;
           if (dirtyY2 < y)  dirtyY2 = y;
           }
        }
     }
}
//M7X0 BEGIN AK
void cBitmap::AddDirty(int xmin, int ymin, int xmax, int ymax)
{
  if (dirtyX1 > xmin) dirtyX1=xmin;
  if (dirtyY1 > ymin) dirtyY1=ymin;
  if (dirtyX2 < xmax) dirtyX2=xmax;
  if (dirtyY2 < ymax) dirtyY2=ymax;
}

void cBitmap::DrawPixel(int x, int y, tColor Color)
{
  x -= x0;
  y -= y0;
  SetIndex(x, y, Index(Color));
}


void cBitmap::DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg, tColor ColorBg, bool ReplacePalette, bool Overlay)
{
  if (bitmap && Bitmap.bitmap && Intersects(x, y, x + Bitmap.Width() - 1, y + Bitmap.Height() - 1)) {
     x -= x0; y -= y0;
     int w = Bitmap.width; int h = Bitmap.height;
     int xb = 0; int yb = 0;
     if (x + w > width)
        w = width - x;
     if (y + h > height)
        h = height - y;
     if (x < 0)
        xb = -x;
     if (y < 0)
        yb = -y;
     x += xb; y += yb; w -= xb; h -= yb;
     AddDirty(x, y, x + w - 1, y + h -1);
     if (Covers(x + x0, y + y0, x + x0 + Bitmap.Width() - 1, y + y0 + Bitmap.Height() - 1)) {
        Replace(Bitmap);
        if ((!ReplacePalette) & ((ColorFg != 0) | (ColorBg != 0))) {
           SetColor(0, ColorBg);
           SetColor(1, ColorFg);
           }
        if (!Overlay) {
           tIndex *bmDest = bitmap + y * width + x;
           tIndex *bmSrc = Bitmap.bitmap + yb * Bitmap.width + xb;
           for (int iy = 0; iy < h; iy++) {
               memcpy(bmDest, bmSrc, w);
               bmDest += width; bmSrc += Bitmap.width;
               }
           }
        else {
           int srcOffset = Bitmap.width * yb + xb;
           for (int iy = 0; iy < h; iy++) {
               for (int ix = 0; ix < w; ix++) {
                   if (Bitmap.bitmap[srcOffset + ix] != 0)
                      SetIndexRaw(x + ix, y + iy, Bitmap.bitmap[srcOffset + ix]);
                   }
               srcOffset += Bitmap.width;
               }
           }
        }
     else {
        tIndexes Indexes;
        Take(Bitmap, &Indexes, ColorFg, ColorBg);
        int srcOffset = Bitmap.width * yb + xb;
        for (int iy = 0; iy < h; iy++) {
            for (int ix = 0; ix < w; ix++) {
                if ((!Overlay) | (Bitmap.bitmap[srcOffset + ix] != 0))
                   SetIndexRaw(x + ix, y + iy, Indexes[int(Bitmap.bitmap[srcOffset + ix])]);
                }
            srcOffset += Bitmap.width;
            }
        }
     }
}

void cBitmap::DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width, int Height, int Alignment)
{
  if (bitmap) {
     int w = Font->Width(s);
     int h = Font->Height();
     int x_orig = x;
     int y_orig = y;

     if (Width) {
        if (w >= Width)
           w = Width;
        else if ((Alignment & taLeft) == 0) {
           if ((Alignment & taRight) != 0)
              x += Width - w;
           else // taCentered
              x += (Width - w) / 2;
           }
        }
     else
        Width = w;

     if (Height) {
        if (h >= Height)
           h = Height;
        else if ((Alignment & taTop) == 0) {
           if ((Alignment & taBottom) != 0)
              y += Height - h;
           else  // taCentered
              y += (Height - h) / 2;
           }
        }
     else
        Height = h;

     if (ColorBg != clrTransparent)
        DrawRectangle(x_orig, y_orig, x_orig + Width - 1, y_orig + Height - 1, ColorBg);

     x -= x0;
     y -= y0;
     int limit = x + w;
     int x_end = limit;
     if (x_end > width)
        x_end = width;
     int y_end = y + h;
     if (y_end > height)
        y_end = height;
     if ((y_end <= 0) | (x_end <= 0) | (x >= x_end) | (y >= y_end))
        return;
     int row_s = 0;
     if (y < 0)
        row_s = -y;
     AddDirty(x >= 0 ? x : 0, y + row_s , x_end - 1, y_end - 1);

     tIndex fg = Index(ColorFg);
     while ((s && *s) & (x < x_end)) {
           const cFont::tCharData *CharData = Font->CharData(*s++);
           if (int(x + CharData->width) > limit)
              break; // we don't draw partial characters
           if (int(x + CharData->width) > 0) {
              for (int row = row_s; y + row < y_end; row++) {
                  cFont::tPixelData PixelData = CharData->lines[row];
                  for (int col = CharData->width - 1;(x + col >= 0) & (PixelData != 0); col--) {
                      if ((PixelData & 1) & (x + col < x_end))
                         SetIndexRaw(x + col, y + row, fg);
                      PixelData >>= 1;
                      }
                  }
              }
           x += CharData->width;
           }
     }
}

void cBitmap::DrawRectangle(int x1, int y1, int x2, int y2, tColor Color)
{
  if (bitmap && Intersects(x1, y1, x2, y2)) {
     if (Covers(x1, y1, x2, y2))
        Reset();
     x1 -= x0;
     y1 -= y0;
     x2 -= x0;
     y2 -= y0;
     x1 = max(x1, 0);
     y1 = max(y1, 0);
     x2 = min(x2, width - 1);
     y2 = min(y2, height - 1);
     tIndex c = Index(Color);
     AddDirty(x1,y1,x2,y2);
     tIndex *bm=bitmap+y1*width+x1;
     int w = x2-x1+1;
     for (int y=y1;y<=y2;y++) {
         memset(bm, c, w);
         bm+=width;
         }
     }
}
//M7X0 END AK
void cBitmap::DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants)
{
  if (!Intersects(x1, y1, x2, y2))
     return;
  // Algorithm based on http://homepage.smc.edu/kennedy_john/BELIPSE.PDF
  int rx = x2 - x1;
  int ry = y2 - y1;
  int cx = (x1 + x2) / 2;
  int cy = (y1 + y2) / 2;
  switch (abs(Quadrants)) {
    case 0: rx /= 2; ry /= 2; break;
    case 1: cx = x1; cy = y2; break;
    case 2: cx = x2; cy = y2; break;
    case 3: cx = x2; cy = y1; break;
    case 4: cx = x1; cy = y1; break;
    case 5: cx = x1;          ry /= 2; break;
    case 6:          cy = y2; rx /= 2; break;
    case 7: cx = x2;          ry /= 2; break;
    case 8:          cy = y1; rx /= 2; break;
    }
  int TwoASquare = 2 * rx * rx;
  int TwoBSquare = 2 * ry * ry;
  int x = rx;
  int y = 0;
  int XChange = ry * ry * (1 - 2 * rx);
  int YChange = rx * rx;
  int EllipseError = 0;
  int StoppingX = TwoBSquare * rx;
  int StoppingY = 0;
  while (StoppingX >= StoppingY) {
        switch (Quadrants) {
          case  5: DrawRectangle(cx,     cy + y, cx + x, cy + y, Color); // no break
          case  1: DrawRectangle(cx,     cy - y, cx + x, cy - y, Color); break;
          case  7: DrawRectangle(cx - x, cy + y, cx,     cy + y, Color); // no break
          case  2: DrawRectangle(cx - x, cy - y, cx,     cy - y, Color); break;
          case  3: DrawRectangle(cx - x, cy + y, cx,     cy + y, Color); break;
          case  4: DrawRectangle(cx,     cy + y, cx + x, cy + y, Color); break;
          case  0:
          case  6: DrawRectangle(cx - x, cy - y, cx + x, cy - y, Color); if (Quadrants == 6) break;
          case  8: DrawRectangle(cx - x, cy + y, cx + x, cy + y, Color); break;
          case -1: DrawRectangle(cx + x, cy - y, x2,     cy - y, Color); break;
          case -2: DrawRectangle(x1,     cy - y, cx - x, cy - y, Color); break;
          case -3: DrawRectangle(x1,     cy + y, cx - x, cy + y, Color); break;
          case -4: DrawRectangle(cx + x, cy + y, x2,     cy + y, Color); break;
          }
        y++;
        StoppingY += TwoASquare;
        EllipseError += YChange;
        YChange += TwoASquare;
        if (2 * EllipseError + XChange > 0) {
           x--;
           StoppingX -= TwoBSquare;
           EllipseError += XChange;
           XChange += TwoBSquare;
           }
        }
  x = 0;
  y = ry;
  XChange = ry * ry;
  YChange = rx * rx * (1 - 2 * ry);
  EllipseError = 0;
  StoppingX = 0;
  StoppingY = TwoASquare * ry;
  while (StoppingX <= StoppingY) {
        switch (Quadrants) {
          case  5: DrawRectangle(cx,     cy + y, cx + x, cy + y, Color); // no break
          case  1: DrawRectangle(cx,     cy - y, cx + x, cy - y, Color); break;
          case  7: DrawRectangle(cx - x, cy + y, cx,     cy + y, Color); // no break
          case  2: DrawRectangle(cx - x, cy - y, cx,     cy - y, Color); break;
          case  3: DrawRectangle(cx - x, cy + y, cx,     cy + y, Color); break;
          case  4: DrawRectangle(cx,     cy + y, cx + x, cy + y, Color); break;
          case  0:
          case  6: DrawRectangle(cx - x, cy - y, cx + x, cy - y, Color); if (Quadrants == 6) break;
          case  8: DrawRectangle(cx - x, cy + y, cx + x, cy + y, Color); break;
          case -1: DrawRectangle(cx + x, cy - y, x2,     cy - y, Color); break;
          case -2: DrawRectangle(x1,     cy - y, cx - x, cy - y, Color); break;
          case -3: DrawRectangle(x1,     cy + y, cx - x, cy + y, Color); break;
          case -4: DrawRectangle(cx + x, cy + y, x2,     cy + y, Color); break;
          }
        x++;
        StoppingX += TwoBSquare;
        EllipseError += XChange;
        XChange += TwoBSquare;
        if (2 * EllipseError + YChange > 0) {
           y--;
           StoppingY -= TwoASquare;
           EllipseError += YChange;
           YChange += TwoASquare;
           }
        }
}

void cBitmap::DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type)
{
  // TODO This is just a quick and dirty implementation of a slope drawing
  // machanism. If somebody can come up with a better solution, let's have it!
  if (!Intersects(x1, y1, x2, y2))
     return;
  bool upper    = Type & 0x01;
  bool falling  = Type & 0x02;
  bool vertical = Type & 0x04;
  if (vertical) {
     for (int y = y1; y <= y2; y++) {
         double c = cos((y - y1) * M_PI / (y2 - y1 + 1));
         if (falling)
            c = -c;
         int x = int((x2 - x1 + 1) * c / 2);
         if ((upper && !falling) || (!upper && falling))
            DrawRectangle(x1, y, (x1 + x2) / 2 + x, y, Color);
         else
            DrawRectangle((x1 + x2) / 2 + x, y, x2, y, Color);
         }
     }
  else {
     for (int x = x1; x <= x2; x++) {
         double c = cos((x - x1) * M_PI / (x2 - x1 + 1));
         if (falling)
            c = -c;
         int y = int((y2 - y1 + 1) * c / 2);
         if (upper)
            DrawRectangle(x, y1, x, (y1 + y2) / 2 + y, Color);
         else
            DrawRectangle(x, (y1 + y2) / 2 + y, x, y2, Color);
         }
     }
}

const tIndex *cBitmap::Data(int x, int y)
{
  return &bitmap[y * width + x];
}

// --- cOsd ------------------------------------------------------------------

int cOsd::isOpen = 0;

cOsd::cOsd(int Left, int Top)
{
  if (isOpen)
     esyslog("ERROR: OSD opened without closing previous OSD!");
  savedRegion = NULL;
  numBitmaps = 0;
  left = Left;
  top = Top;
  width = height = 0;
  isOpen++;
}

cOsd::~cOsd()
{
  for (int i = 0; i < numBitmaps; i++)
      delete bitmaps[i];
  delete savedRegion;
  isOpen--;
}

cBitmap *cOsd::GetBitmap(int Area)
{
  return Area < numBitmaps ? bitmaps[Area] : NULL;
}

eOsdError cOsd::CanHandleAreas(const tArea *Areas, int NumAreas)
{
  if (NumAreas > MAXOSDAREAS)
     return oeTooManyAreas;
  eOsdError Result = oeOk;
  for (int i = 0; i < NumAreas; i++) {
      if (Areas[i].x1 > Areas[i].x2 || Areas[i].y1 > Areas[i].y2 || Areas[i].x1 < 0 || Areas[i].y1 < 0)
         return oeWrongAlignment;
      for (int j = i + 1; j < NumAreas; j++) {
          if (Areas[i].Intersects(Areas[j])) {
             Result = oeAreasOverlap;
             break;
             }
          }
      }
  return Result;
}

eOsdError cOsd::SetAreas(const tArea *Areas, int NumAreas)
{
  eOsdError Result = oeUnknown;
  if (numBitmaps == 0) {
     Result = CanHandleAreas(Areas, NumAreas);
     if (Result == oeOk) {
        width = height = 0;
        for (int i = 0; i < NumAreas; i++) {
            bitmaps[numBitmaps++] = new cBitmap(Areas[i].Width(), Areas[i].Height(), Areas[i].bpp, Areas[i].x1, Areas[i].y1);
            width = max(width, Areas[i].x2 + 1);
            height = max(height, Areas[i].y2 + 1);
            }
        }
     }
  if (Result != oeOk)
     esyslog("ERROR: cOsd::SetAreas returned %d", Result);
  return Result;
}

void cOsd::SaveRegion(int x1, int y1, int x2, int y2)
{
  delete savedRegion;
  savedRegion = new cBitmap(x2 - x1 + 1, y2 - y1 + 1, 8, x1, y1);
  for (int i = 0; i < numBitmaps; i++)
      savedRegion->DrawBitmap(bitmaps[i]->X0(), bitmaps[i]->Y0(), *bitmaps[i]);
}

void cOsd::RestoreRegion(void)
{
  if (savedRegion) {
     DrawBitmap(savedRegion->X0(), savedRegion->Y0(), *savedRegion);
     delete savedRegion;
     savedRegion = NULL;
     }
}

eOsdError cOsd::SetPalette(const cPalette &Palette, int Area)
{
  if (Area < numBitmaps)
     bitmaps[Area]->Take(Palette);
  return oeUnknown;
}

void cOsd::DrawPixel(int x, int y, tColor Color)
{
  for (int i = 0; i < numBitmaps; i++)
      bitmaps[i]->DrawPixel(x, y, Color);
}

void cOsd::DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg, tColor ColorBg, bool ReplacePalette, bool Overlay)
{
  for (int i = 0; i < numBitmaps; i++)
      bitmaps[i]->DrawBitmap(x, y, Bitmap, ColorFg, ColorBg, ReplacePalette, Overlay);
}

void cOsd::DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width, int Height, int Alignment)
{
  for (int i = 0; i < numBitmaps; i++)
      bitmaps[i]->DrawText(x, y, s, ColorFg, ColorBg, Font, Width, Height, Alignment);
}

void cOsd::DrawRectangle(int x1, int y1, int x2, int y2, tColor Color)
{
  for (int i = 0; i < numBitmaps; i++)
      bitmaps[i]->DrawRectangle(x1, y1, x2, y2, Color);
}

void cOsd::DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants)
{
  for (int i = 0; i < numBitmaps; i++)
      bitmaps[i]->DrawEllipse(x1, y1, x2, y2, Color, Quadrants);
}

void cOsd::DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type)
{
  for (int i = 0; i < numBitmaps; i++)
      bitmaps[i]->DrawSlope(x1, y1, x2, y2, Color, Type);
}

void cOsd::Flush(void)
{
}

// --- cOsdProvider ----------------------------------------------------------

cOsdProvider *cOsdProvider::osdProvider = NULL;

cOsdProvider::cOsdProvider(void)
{
  delete osdProvider;
  osdProvider = this;
}

cOsdProvider::~cOsdProvider()
{
  osdProvider = NULL;
}

cOsd *cOsdProvider::NewOsd(int Left, int Top)
{
  if (cOsd::IsOpen())
     esyslog("ERROR: attempt to open OSD while it is already open - using dummy OSD!");
  else if (osdProvider)
     return osdProvider->CreateOsd(Left, Top);
  else
     esyslog("ERROR: no OSD provider available - using dummy OSD!");
  return new cOsd(Left, Top); // create a dummy cOsd, so that access won't result in a segfault
}

void cOsdProvider::Shutdown(void)
{
  delete osdProvider;
  osdProvider = NULL;
}

// --- cTextScroller ---------------------------------------------------------

cTextScroller::cTextScroller(void)
{
  osd = NULL;
  left = top = width = height = 0;
  font = NULL;
  colorFg = 0;
  colorBg = 0;
  offset = 0;
  shown = 0;
}

cTextScroller::cTextScroller(cOsd *Osd, int Left, int Top, int Width, int Height, const char *Text, const cFont *Font, tColor ColorFg, tColor ColorBg)
{
  Set(Osd, Left, Top, Width, Height, Text, Font, ColorFg, ColorBg);
}

void cTextScroller::Set(cOsd *Osd, int Left, int Top, int Width, int Height, const char *Text, const cFont *Font, tColor ColorFg, tColor ColorBg)
{
  osd = Osd;
  left = Left;
  top = Top;
  width = Width;
  height = Height;
  font = Font;
  colorFg = ColorFg;
  colorBg = ColorBg;
  offset = 0;
  textWrapper.Set(Text, Font, Width);
  shown = min(Total(), height / font->Height());
  height = shown * font->Height(); // sets height to the actually used height, which may be less than Height
  DrawText();
}

void cTextScroller::Reset(void)
{
  osd = NULL; // just makes sure it won't draw anything
}

void cTextScroller::DrawText(void)
{
  if (osd) {
     for (int i = 0; i < shown; i++)
         osd->DrawText(left, top + i * font->Height(), textWrapper.GetLine(offset + i), colorFg, colorBg, font, width);
     }
}

void cTextScroller::Scroll(bool Up, bool Page)
{
  if (Up) {
     if (CanScrollUp()) {
        offset -= Page ? shown : 1;
        if (offset < 0)
           offset = 0;
        DrawText();
        }
     }
  else {
     if (CanScrollDown()) {
        offset += Page ? shown : 1;
        if (offset + shown > Total())
           offset = Total() - shown;
        DrawText();
        }
     }
}
