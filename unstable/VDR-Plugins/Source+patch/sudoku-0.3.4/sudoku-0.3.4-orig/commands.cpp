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
 * $Id: commands.cpp 142 2008-07-06 15:50:02Z tom $
 */

#include "commands.h"
#include "menu.h"
#include "i18n.h"
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>
#include <assert.h>


namespace SudokuPlugin
{
  /** Command ids */
  enum
  {
    Cmd_LowerEdge = 100,
    CmdGenerate,
    CmdReset,
    CmdExit,
    CmdMark,
    CmdNextCell,
    CmdNextNumber,
    CmdLoad,
    CmdSave,
    CmdSetup,
    CmdUndo,
    CmdRedo,
    // Add new command ids only here, don't remove commands.
    Cmd_UpperEdge,
    Cmd_Count = Cmd_UpperEdge-Cmd_LowerEdge-1,
    // Default commands
    Cmd_KeyRedDefault = CmdMark,
    Cmd_KeyGreenDefault = CmdUndo,
    Cmd_KeyYellowDefault = CmdRedo
  };
} // namespace SudokuPlugin


using namespace SudokuPlugin;


//--- class SudokuPlugin::CommandList ------------------------------------------

/** List of commands - sorted for the commands menu */
const struct CommandList::List CommandList::list[] =
{
  { CmdGenerate,   trNOOP("Generate a new puzzle"), &Menu::generate    },
  { CmdLoad,       trNOOP("Load a puzzle"),         &Menu::load        },
  { CmdSave,       trNOOP("Save the puzzle"),       &Menu::save        },
  { CmdUndo,       trNOOP("Undo last action"),      &Menu::undo        },
  { CmdRedo,       trNOOP("Redo last action"),      &Menu::redo        },
  { CmdMark,       trNOOP("Mark/unmark"),           &Menu::toggle_mark },
  { CmdNextCell,   trNOOP("Next cell"),             &Menu::next_cell   },
  { CmdNextNumber, trNOOP("Next number"),           &Menu::next_number },
  { CmdReset,      trNOOP("Reset the puzzle"),      &Menu::reset       },
  { CmdSetup,      trNOOP("Open setup menu"),       &Menu::open_setup  },
  { CmdExit,       trNOOP("Exit"),                  &Menu::exit        }
};

/** Command id of a command */
int CommandList::id(int index)
{
  assert(index >= 0 && index < count());
  return list[index].id;
}

/** Translated text of a command */
const char* CommandList::text(int index)
{
  assert(index >= 0 && index < count());
  return tr(list[index].text);
}

/** Command type of a command */
CommandType CommandList::command(int index)
{
  assert(index >= 0 && index < count());
  return list[index].cmd;
}

/** Count of commands */
int CommandList::count()
{
  assert(Cmd_Count == sizeof(list)/sizeof(*list));
  return Cmd_Count;
}

/** List with translated texts of all commands */
const char* const* CommandList::texts()
{
  static const char* text_list[Cmd_Count];
  for (int idx = 0; idx < count(); ++idx)
    text_list[idx] = tr(list[idx].text);
  return text_list;
}

/** Search a command id. Return default index if not found. */
int CommandList::id_to_index(int id, int default_index)
{
  int idx = 0;
  for (; idx < count(); ++idx)
    if (list[idx].id == id)
      break;
  if (idx >= count())
    idx = default_index;
  assert(idx >= 0 && idx < count());
  return idx;
}

/** Default index for the red key */
int CommandList::key_red_default_index()
{
  return id_to_index(Cmd_KeyRedDefault);
}

/** Default index for the green key */
int CommandList::key_green_default_index()
{
  return id_to_index(Cmd_KeyGreenDefault);
}

/** Default index for the yellow key */
int CommandList::key_yellow_default_index()
{
  return id_to_index(Cmd_KeyYellowDefault);
}


//--- class SudokuPlugin::CommandMenu ------------------------------------------

/** Constructor */
CommandMenu::CommandMenu() :
  cOsdMenu(trVDR("Commands"))
{
  SetHasHotkeys();
  for (int idx = 0; idx < CommandList::count(); ++idx)
    Add(new Command(hk(CommandList::text(idx)), CommandList::command(idx)));
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
