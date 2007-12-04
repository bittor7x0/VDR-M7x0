/*
 * Spider-Arachnid: A plugin for the Video Disk Recorder
 *
 * Copyright (C) 2005-2007, Thomas Günther <tom@toms-cafe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id: bitmap.c 87 2007-06-22 22:37:36Z tom $
 */

#include "bitmap.h"
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>
#include <ctype.h>


/** --- class Bitmap ------------------------------------------------------- **/

/** Constructor */
Bitmap::Bitmap(int width, int height) :
  cBitmap(width, height, 4)
{
}

/** Constructor for a bitmap with frame */
Bitmap::Bitmap(int width, int height, tColor frameColor, tColor backgroundColor) :
  cBitmap(width, height, 4)
{
  DrawRectangle(0, 0, width - 1, height - 1, backgroundColor);
  frame(0, 0, width - 1, height - 1, frameColor);
}

/** Constructor for a bitmap read from an xpm file */
Bitmap::Bitmap(int width, int height, const char* dir, const char* name) :
  cBitmap(width, height, 4)
{
  char* path;
  asprintf(&path, "%s/%s.xpm", dir, name);
  loadXpm(path);
  free(path);
}

/** Constructor for a card bitmap read from an xpm file */
Bitmap::Bitmap(int width, int height, const char* dir,
               const char* suit, const char* rank) :
  cBitmap(width, height, 4)
{
  char* path;
  asprintf(&path, "%s/%s_%s.xpm", dir, suit, rank);
  loadXpm(path);
  free(path);
}

/** Write a text into the bitmap */
void Bitmap::text(const char* text, bool centered)
{
  DrawRectangle(0, 0, Width() - 1, Height() - 1, clrWhite);
  frame(0, 0, Width() - 1, Height() - 1, clrRed);
  const cFont* font = cFont::GetFont(fontOsd);
  cTextWrapper wrapper(text, font, Width());
  int y = max((Height() - wrapper.Lines() * font->Height()) / 2, 0);
  for (int l = 0; l < wrapper.Lines(); ++l, y += font->Height())
  {
    int x = 0;
    if (centered)
      x = max((Width() - font->Width(wrapper.GetLine(l))) / 2, 0);
    DrawText(x, y, wrapper.GetLine(l), clrBlack, clrWhite, font);
  }
}

/** Draw a frame into the bitmap */
void Bitmap::frame(int x1, int y1, int x2, int y2, tColor frameColor)
{
  DrawRectangle(x1,     y1,     x2,     y1 + 1, frameColor);
  DrawRectangle(x1,     y1,     x1 + 1, y2,     frameColor);
  DrawRectangle(x1,     y2 - 1, x2,     y2,     frameColor);
  DrawRectangle(x2 - 1, y1,     x2,     y2,     frameColor);
}

/** Load a bitmap from an xpm file - taken from ElchiAIO4d patch */
bool Bitmap::loadXpm(const char* FileName, tColor NoneColor)
{
  bool bRet = false;
  FILE *infile;
  infile = fopen(FileName, "r");
  if (infile) {
     bool ok = true;
     char buf[512];
     char *ptr;
     int state = 0;
     int width, height, colors, colwidth, cnt = 0;
     int temp;
     uint pal[65536];

     while (ok && fgets(buf, sizeof(buf), infile) != NULL) {
       int len = strlen(buf);
       ptr = buf;
       if (ptr[len - 1] == '\n')
          ptr[--len] = '\0';

       if (state > 0 && strncmp(ptr, "/*", 2) == 0) {
          continue;
          }

       switch (state) {
         case 0:
              if (strcmp(ptr, "/* XPM */") != 0) {
                 esyslog("ERROR: loading xpm %s failed: invalid header", FileName);
                 ok = false;
                 break;
                 }
              ++state;
              break;
         case 1:
              ++state;
              break;
         case 2:
              sscanf(ptr, "\"%d %d %d %d\",", &width, &height, &colors, &colwidth);
              if (colwidth > 2) {
                 esyslog("ERROR: wrong colorwidth in xpm %s", FileName);
                 ok = false;
                 break;
                 }
              cnt = 0;
              ++state;
              break;
         case 3:
              ++ptr;
              temp = 0;
              for (int i = 0; i < colwidth; ++i) {
                  temp <<= 8;
                  temp += (int)*ptr;
                  ++ptr;
                  }
              ++ptr;
              if (strncmp(ptr, "c ", 2) != 0) {
                 esyslog("ERROR: wrong character in xpm %s", FileName);
                 ok = false;
                 break;
                 }
              ptr += 2;
              if (*ptr == '#') {
                 int col = strtoul(++ptr, NULL, 16);
                 pal[temp] = 0xff000000 | col;
                 }
              else {
                 pal[temp] = NoneColor;
                 }
              if (++cnt == colors) {
                 cnt = 0;
                 ++state;
                 }
              break;
         case 4:
              ++ptr;
              for (int p = 0; p < width; ++p) {
                  temp = 0;
                  for (int i = 0; i < colwidth; ++i) {
                      temp <<= 8;
                      temp += (int)*ptr;
                      ++ptr;
                      }
                  DrawPixel(p, cnt, (tColor)pal[temp]);
                  }
              if (++cnt == height) {
                 ++state;
                 bRet = true;
                 }
              break;
         default:
              break;
         }
       }
     fclose(infile);
     }
  return bRet;
}
