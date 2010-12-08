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
 * $Id: setup.h 143 2008-07-26 18:38:59Z tom $
 */

#ifndef VDR_SUDOKU_SETUP_H
#define VDR_SUDOKU_SETUP_H

#include "sudoku.h"
#include <vdr/config.h>
#include <vdr/menuitems.h>
#include <vdr/font.h>


namespace SudokuPlugin
{

  //--- class SudokuPlugin::SetupData ------------------------------------------

  /** Setup parameters of the plugin */
  class SetupData
  {
  public:
    int givens_count;
    int symmetric;
    int mark_errors;
    int mark_ambiguous;
    int show_possibles_pattern;
    int show_possibles_digits;
    int clear_marks;
    int key_red;
    int key_green;
    int key_yellow;
#if VDRVERSNUM >= 10504
    char maxi_font[MAXFONTNAME];
    int maxi_font_height;
    int maxi_font_width;
    char mini_font[MAXFONTNAME];
    int mini_font_height;
    int mini_font_width;
#endif
    int transparency;

    /** Constructor */
    SetupData();

    /** Parse the setup parameters of the plugin. */
    bool parse(const char* name, const char* value);
  };


  //--- class SudokuPlugin::SetupPage ------------------------------------------

  /** Setup menu page to adjust the setup parameters of the plugin */
  class SetupPage : public cMenuSetupPage
  {
    SetupData& setup;
    SetupData data;
#if VDRVERSNUM >= 10504
    cStringList maxi_font_names, mini_font_names;
    int maxi_font_index, mini_font_index;
#endif

  public:

    /** Constructor */
    SetupPage(SetupData& setup);

  protected:

    /** Store the setup parameters of the plugin. */
    virtual void Store();
  };

} // namespace SudokuPlugin

#endif // VDR_SUDOKU_SETUP_H
