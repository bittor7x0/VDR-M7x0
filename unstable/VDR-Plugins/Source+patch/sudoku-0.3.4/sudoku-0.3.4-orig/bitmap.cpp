/*
 * Sudoku: A plug-in for the Video Disk Recorder
 *
 * Copyright (C) 2005-2008, Thomas Günther <tom@toms-cafe.de>
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
 * $Id: bitmap.cpp 122 2008-03-22 00:38:13Z tom $
 */

#include "bitmap.h"
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>
#include <ctype.h>

using namespace SudokuPlugin;


//--- class SudokuPlugin::Bitmap -----------------------------------------------

/** Constructor */
Bitmap::Bitmap(int width, int height) :
  cBitmap(width, height, 4)
{
}

/** Constructor for a bitmap with frame */
Bitmap::Bitmap(int width, int height, tColor frameColor,
                                      tColor backgroundColor) :
  cBitmap(width, height, 4)
{
  DrawRectangle(0, 0, width - 1, height - 1, backgroundColor);
  frame(0, 0, width - 1, height - 1, frameColor);
}

/** Write a text into the bitmap. */
void Bitmap::text(const char* text, bool centered)
{
  DrawRectangle(0, 0, Width() - 1, Height() - 1, clrWhite);
  frame(0, 0, Width() - 1, Height() - 1, clrRed);
  const cFont* font = cFont::GetFont(fontOsd);
  cTextWrapper wrapper(text, font, Width() - 10);
  int y = max((Height() - wrapper.Lines() * font->Height()) / 2, 0);
  for (int l = 0; l < wrapper.Lines(); ++l, y += font->Height())
  {
    int x = 0;
    if (centered)
      x = max((Width() - font->Width(wrapper.GetLine(l))) / 2, 0);
    DrawText(x, y, wrapper.GetLine(l), clrBlack, clrWhite, font);
  }
}

/** Draw a frame into the bitmap. */
void Bitmap::frame(int x1, int y1, int x2, int y2, tColor frameColor)
{
  DrawRectangle(x1,     y1,     x2,     y1 + 1, frameColor);
  DrawRectangle(x1,     y1,     x1 + 1, y2,     frameColor);
  DrawRectangle(x1,     y2 - 1, x2,     y2,     frameColor);
  DrawRectangle(x2 - 1, y1,     x2,     y2,     frameColor);
}
