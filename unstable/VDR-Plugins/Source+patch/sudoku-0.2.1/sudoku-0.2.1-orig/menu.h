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
 * $Id: menu.h 117 2008-03-21 17:57:50Z tom $
 */

#ifndef VDR_SUDOKU_MENU_H
#define VDR_SUDOKU_MENU_H

#include "sudoku.h"
namespace Sudoku { class Puzzle; class Pos; }
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>
#include <vdr/plugin.h>
#include <vdr/tools.h>


namespace SudokuPlugin
{
  class SetupData;
  class Bitmap;
  class CommandMenu;
  class ListMenu;
  class SetupPage;

  //--- class SudokuPlugin::Menu -----------------------------------------------

  /** Main menu of the plugin */
  class Menu : public cOsdObject
  {
    cPlugin* plugin;
    const SetupData& setup;
    Sudoku::Puzzle& puzzle;
    Sudoku::Pos& curr;
    int xPos, yPos;
    cOsd* osd;
    Bitmap* info;
    const char* infoText;
    bool new_puzzle_request;
    const cFont* maxi_font;
    const cFont* mini_font;
    CommandMenu* command_menu;
    ListMenu* list_menu;
    cMenuSetupPage* setup_menu;
    cString listfile;

  public:

    /** Constructor */
    Menu(cPlugin* plugin, const SetupData& setup,
         Sudoku::Puzzle& puzzle, Sudoku::Pos& curr);

    /** Destructor */
    virtual ~Menu();

    /** Display the menu on the OSD. */
    virtual void Show();

    /** Process user events. */
    virtual eOSState ProcessKey(eKeys key);

    /** Generate a new puzzle. */
    eOSState generate();

    /** Load a puzzle. */
    eOSState load();

    /** Save the puzzle. */
    eOSState save();

    /** Reset the puzzle. */
    eOSState reset();

    /** Open setup menu. */
    eOSState open_setup();

    /** Exit plugin menu. */
    eOSState exit();

  private:

    /** Paint all pieces of the menu. */
    void paint();
  };

} // namespace SudokuPlugin

#endif // VDR_SUDOKU_MENU_H
