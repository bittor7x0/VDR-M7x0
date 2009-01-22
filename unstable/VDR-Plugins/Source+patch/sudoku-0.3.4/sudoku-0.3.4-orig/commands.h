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
 * $Id: commands.h 142 2008-07-06 15:50:02Z tom $
 */

#ifndef VDR_SUDOKU_COMMANDS_H
#define VDR_SUDOKU_COMMANDS_H

#include "sudoku.h"
#include <vdr/osdbase.h>


namespace SudokuPlugin
{
  class Menu;

  /** Type of menu commands: pointer to member function of class Menu */
  typedef eOSState (Menu::*CommandType)();


  //--- class SudokuPlugin::CommandList ----------------------------------------

  class CommandList
  {
    /** List of commands - sorted for the commands menu */
    struct List { int id; const char* text; CommandType cmd; };
    static const struct List list[];

  public:

    /** Command id of a command */
    static int id(int index);

    /** Translated text of a command */
    static const char* text(int index);

    /** Command type of a command */
    static CommandType command(int index);

    /** Count of commands */
    static int count();

    /** List with translated texts of all commands */
    static const char* const* texts();

    /** Search a command id. Return default index if not found. */
    static int id_to_index(int id, int default_index = -1);

    /** Default index for the red key */
    static int key_red_default_index();

    /** Default index for the green key */
    static int key_green_default_index();

    /** Default index for the yellow key */
    static int key_yellow_default_index();
  };


  //--- class SudokuPlugin::CommandMenu ----------------------------------------

  /** Commands menu of the plugin */
  class CommandMenu : public cOsdMenu
  {
    CommandType command;

  public:

    /** Constructor */
    CommandMenu();

    /** Get the selected menu command. */
    CommandType get_selected_command() const;

    /** Process user events. */
    virtual eOSState ProcessKey(eKeys key);
  };


  //--- class SudokuPlugin::Command --------------------------------------------

  /** Item in commands menu */
  class Command : public cOsdItem
  {
    CommandType command;

  public:

    /** Constructor */
    Command(const char* text, CommandType command);

    /** Get the menu command. */
    CommandType get_command() const;
  };

} // namespace SudokuPlugin

#endif // VDR_SUDOKU_COMMANDS_H
