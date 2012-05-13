/*
 * Sudoku: A plug-in for the Video Disk Recorder
 *
 * Copyright (C) 2008, Thomas Günther <tom@toms-cafe.de>
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
 * $Id: list.h 114 2008-03-16 22:20:33Z tom $
 */

#ifndef VDR_SUDOKU_LIST_H
#define VDR_SUDOKU_LIST_H

#include "sudoku.h"
#include <vdr/osdbase.h>
#include <vdr/tools.h>


namespace SudokuPlugin
{

  //--- class SudokuPlugin::ListLine -------------------------------------------

  /** Line in sudoku list */
  class ListLine : public cListObject
  {
    cString sudoku;
    cString description;

    /** Default constructor for cConfig<ListLine> */
    ListLine();
    friend class cConfig<ListLine>;

  public:

    /** Constructor */
    ListLine(const char* sudoku, const char* description = "");

    /** Get the sudoku dump. */
    const char* get_sudoku() const;

    /** Get the description. */
    const char* get_description() const;

    /** Set the description. */
    void set_description(const char* new_description);

    /** Parse a line of the sudoku list. */
    bool Parse(const char* line);

    /** Save a line of the sudoku list. */
    bool Save(FILE* file) const;
  };


  //--- class SudokuPlugin::ListMenu -------------------------------------------

  /** Sudoku list menu */
  class ListMenu : public cOsdMenu
  {
    cConfig<ListLine> list;
    int selected;
    bool save_mode;

  public:

    /** Constructor */
    ListMenu(const char* filename, const char* new_sudoku = NULL);

    /** Get the selected sudoku. */
    const char* get_selected_sudoku() const;

    /** Process user events. */
    virtual eOSState ProcessKey(eKeys key);

  private:

    /** Refresh the list menu. */
    void refresh();
  };


  //--- class SudokuPlugin::ListEdit -------------------------------------------

  /** Sudoku list edit menu */
  class ListEdit : public cOsdMenu
  {
    ListLine& line;
    char description[100];

  public:

    /** Constructor */
    ListEdit(ListLine& line);

    /** Process user events. */
    virtual eOSState ProcessKey(eKeys key);
  };

} // namespace SudokuPlugin

#endif // VDR_SUDOKU_LIST_H
