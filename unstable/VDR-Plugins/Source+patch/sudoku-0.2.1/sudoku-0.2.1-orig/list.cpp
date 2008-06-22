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
 * $Id: list.cpp 115 2008-03-19 00:38:58Z tom $
 */

#include "list.h"
#include "i18n.h"
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>
#include <vdr/menuitems.h>
#include <assert.h>

// Compatibility to older vdr versions
#if VDRVERSNUM < 10511
  #define cMenuEditStrItem(n, v, l) cMenuEditStrItem((n), (v), (l),\
                                                     trVDR(FileNameChars))
#endif

using namespace SudokuPlugin;


//--- class SudokuPlugin::ListLine ---------------------------------------------

/** Default constructor for cConfig<ListLine> */
ListLine::ListLine()
{
}

/** Constructor */
ListLine::ListLine(const char* sudoku, const char* description) :
  sudoku(sudoku), description(description)
{
  assert(sudoku != NULL);
  assert(description != NULL);
}

/** Get the sudoku dump. */
const char* ListLine::get_sudoku() const
{
  assert((const char*)sudoku != NULL);
  return sudoku;
}

/** Get the description. */
const char* ListLine::get_description() const
{
  assert((const char*)description != NULL);
  return description;
}

/** Set the description. */
void ListLine::set_description(const char* new_description)
{
  assert(new_description != NULL);
  description = skipspace(new_description);
}

/** Parse a line of the sudoku list. */
bool ListLine::Parse(const char* line)
{
  const char *delim = strchr(line, ' ');
  if (delim)
  {
    sudoku = cString(strndup(line, delim - line), true);
    description = skipspace(delim);
  }
  else
  {
    sudoku = line;
    description = "";
  }
  assert((const char*)sudoku != NULL);
  assert((const char*)description != NULL);
  return !isempty(sudoku);
}

/** Save a line of the sudoku list. */
bool ListLine::Save(FILE* file) const
{
  return fprintf(file, "%s %s\n", *sudoku, *description) > 0;
}


//--- class SudokuPlugin::ListMenu ---------------------------------------------

/** Constructor */
ListMenu::ListMenu(const char* filename, const char* new_sudoku) :
  cOsdMenu(tr("Sudoku list")), selected(-1), save_mode(new_sudoku)
{
  list.Load(filename);
  if (save_mode)
    list.Add(new ListLine(new_sudoku));
  refresh();
  if (save_mode)
  {
    SetCurrent(Last());
    AddSubMenu(new ListEdit(*list.Get(Current())));
    ProcessKey(kRight); // enter edit mode
  }
}

/** Get the selected sudoku. */
const char* ListMenu::get_selected_sudoku() const
{
  ListLine* line = list.Get(selected);
  if (line)
    return line->get_sudoku();
  return NULL;
}

/** Process user events. */
eOSState ListMenu::ProcessKey(eKeys key)
{
  bool had_submenu = HasSubMenu();
  if (key == kBlue && !HasSubMenu())
    key = kBack;
  if (key == kGreen && !HasSubMenu() && !save_mode)
    key = kOk;
  eOSState state = cOsdMenu::ProcessKey(key);
  if (state == osUnknown)
  {
    state = osContinue;
    switch (key)
    {
      case kRed:
        if (Current() >= 0 && Current() < list.Count())
          return AddSubMenu(new ListEdit(*list.Get(Current())));
        break;
      case kYellow:
        if (Current() >= 0 && Current() < list.Count())
          list.Del(list.Get(Current()));
        refresh();
        list.Save();
        break;
      case kOk:
        if (Current() >= 0 && Current() < list.Count() && !save_mode)
          selected = Current();
        state = osBack;
        break;
      default:
        break;
    }
  }
  if (had_submenu && !HasSubMenu())
  {
    refresh();
    list.Save();
  }
  return state;
}

/** Refresh the list menu. */
void ListMenu::refresh()
{
  int current = Current();
  Clear();
  SetHasHotkeys();
  for (ListLine* line = list.First(); line; line = list.Next(line))
    Add(new cOsdItem(hk(line->get_description())));
  SetCurrent(Get(current));
  SetHelp(trVDR("Button$Edit"), save_mode ? NULL : tr("Button$Load"),
          trVDR("Button$Delete"), tr("Button$Back"));
  Display();
}


//--- class SudokuPlugin::ListEdit ---------------------------------------------

/** Constructor */
ListEdit::ListEdit(ListLine& line) :
  cOsdMenu(tr("Edit sudoku list"), 15), line(line)
{
  strn0cpy(description, line.get_description(), sizeof(description));
  Add(new cMenuEditStrItem(tr("Description"), description, sizeof(description)));
}

/** Process user events. */
eOSState ListEdit::ProcessKey(eKeys key)
{
  eOSState state = cOsdMenu::ProcessKey(key);
  if (state == osUnknown)
  {
    if (key == kOk)
    {
      line.set_description(description);
      state = osBack;
    }
  }
  return state;
}
