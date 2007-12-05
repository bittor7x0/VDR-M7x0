/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: bitmap.cpp 27 2006-04-25 00:08:15Z tom $
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

/** Draw a frame into the bitmap. */
void Bitmap::frame(int x1, int y1, int x2, int y2, tColor frameColor)
{
  DrawRectangle(x1,     y1,     x2,     y1 + 1, frameColor);
  DrawRectangle(x1,     y1,     x1 + 1, y2,     frameColor);
  DrawRectangle(x1,     y2 - 1, x2,     y2,     frameColor);
  DrawRectangle(x2 - 1, y1,     x2,     y2,     frameColor);
}
