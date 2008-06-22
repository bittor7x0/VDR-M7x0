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
 * $Id: commands.cpp 116 2008-03-21 16:41:20Z tom $
 */

#include "commands.h"
#include "menu.h"
#include "i18n.h"
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>
#include <assert.h>

using namespace SudokuPlugin;


//--- class SudokuPlugin::CommandMenu ------------------------------------------

/** Constructor */
CommandMenu::CommandMenu() :
  cOsdMenu(trVDR("Commands"))
{
  SetHasHotkeys();
  Add(new Command(hk(tr("Generate a new puzzle")), &Menu::generate));
  Add(new Command(hk(tr("Load a puzzle")),         &Menu::load));
  Add(new Command(hk(tr("Save the puzzle")),       &Menu::save));
  Add(new Command(hk(tr("Reset the puzzle")),      &Menu::reset));
  Add(new Command(hk(tr("Open setup menu")),       &Menu::open_setup));
  Add(new Command(hk(tr("Exit")),                  &Menu::exit));
  command = NULL;
}

/** Get the selected menu command. */
CommandType CommandMenu::get_selected_command() const
{
  return command;
}

/** Process user events. */
eOSState CommandMenu::ProcessKey(eKeys key)
{
  eOSState state = cOsdMenu::ProcessKey(key);
  if (state == osUnknown)
  {
    state = osContinue;
    if (key == kOk)
    {
      Command* item = (Command*)Get(Current());
      if (item)
        command = item->get_command();
      state = osBack;
    }
  }
  return state;
}


//--- class SudokuPlugin::Command ----------------------------------------------

/** Constructor */
Command::Command(const char* text, CommandType command) :
  cOsdItem(text), command(command)
{
  assert(text != NULL);
  assert(command != NULL);
}

/** Get the menu command. */
CommandType Command::get_command() const
{
  return command;
}
