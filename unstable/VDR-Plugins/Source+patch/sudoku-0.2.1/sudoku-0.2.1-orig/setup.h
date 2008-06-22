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
 * $Id: setup.h 117 2008-03-21 17:57:50Z tom $
 */

#ifndef VDR_SUDOKU_SETUP_H
#define VDR_SUDOKU_SETUP_H

#include "sudoku.h"
#include <vdr/menuitems.h>


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

  public:

    /** Constructor */
    SetupPage(SetupData& setup);

  protected:

    /** Store the setup parameters of the plugin. */
    virtual void Store();
  };

} // namespace SudokuPlugin

#endif // VDR_SUDOKU_SETUP_H
