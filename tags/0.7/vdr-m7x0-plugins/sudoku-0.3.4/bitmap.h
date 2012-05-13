/*
 * Sudoku: A plug-in for the Video Disk Recorder
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
 * $Id: bitmap.h 106 2007-12-03 23:28:24Z tom $
 */

#ifndef VDR_SUDOKU_BITMAP_H
#define VDR_SUDOKU_BITMAP_H

#include "sudoku.h"
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>


namespace SudokuPlugin
{

  //--- class SudokuPlugin::Bitmap ---------------------------------------------

  /** Plugin-specific version of the bitmap class */
  class Bitmap : public cBitmap
  {
  public:

    /** Constructor */
    Bitmap(int width, int height);

    /** Constructor for a bitmap with frame */
    Bitmap(int width, int height, tColor frameColor, tColor backgroundColor);

    /** Write a text into the bitmap. */
    void text(const char* text, bool centered = true);

    /** Draw a frame into the bitmap. */
    void frame(int x1, int y1, int x2, int y2, tColor frameColor);
  };

} // namespace SudokuPlugin

#endif // VDR_SUDOKU_BITMAP_H
