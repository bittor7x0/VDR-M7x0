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
 * $Id: menu.cpp 143 2008-07-26 18:38:59Z tom $
 */

#include "menu.h"
#include "puzzle.h"
#include "setup.h"
#include "bitmap.h"
#include "commands.h"
#include "list.h"
#include "i18n.h"
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>
#include <vdr/font.h>

using namespace SudokuPlugin;
using namespace Sudoku;


// Definition of the file name of the sudoku list
#define SUDOKU_LIST     "sudoku-list"

// Definitions for grid structure
#define CELL_SIZE       42
#define CELL_POS(i)     ((i) * (CELL_SIZE + 2) + (i)/RDIM * 3 + 5)
#define GRID_SIZE       (DIM * (CELL_SIZE + 2) + DIM/RDIM * 3 + 5)

// Definitions for grid colors
#define TRANS(c,t)      tColor(((c) & 0x00FFFFFF) | (0xFF * (100-(t))/100)<<24)
#define GRID_COLOR      clrWhite
#define CURSUR_COLOR    clrBlue
#define NUMBER_FG       clrWhite
#define NUMBER_BG       clrBlack
#define ERROR_FG        clrWhite
#define ERROR_BG        clrRed
#define AMBIG_FG        clrWhite
#define AMBIG_BG        clrMagenta
#define MARKED_FG       clrWhite
#define MARKED_BG       0x006400 /* darkgreen */
#define GIVEN_FG        clrBlack
#define GIVEN_BG        clrCyan
#define POSSIBLE_FG     clrBlack
#define POSSIBLE_BG(n)  (((n) % 2) ? clrYellow : 0xFF8C00 /* darkorange */)


//--- class SudokuPlugin::Menu -------------------------------------------------

/** Constructor */
Menu::Menu(cPlugin* plugin, const SetupData& setup, PuzzleGame*& puzzle) :
  cOsdObject(true), plugin(plugin), setup(setup), puzzle(puzzle)
{
  if (puzzle == NULL)
    puzzle = new PuzzleGame(setup.givens_count, setup.symmetric);
  xPos = (720 - GRID_SIZE) / 2;
  yPos = (576 - GRID_SIZE) / 2;
  osd = NULL;
  info = NULL;
  infoText = NULL;
  new_puzzle_request = false;
#if VDRVERSNUM >= 10504
  maxi_font = cFont::CreateFont(setup.maxi_font, setup.maxi_font_height,
                                setup.maxi_font_width);
  mini_font = cFont::CreateFont(setup.mini_font, setup.mini_font_height,
                                setup.mini_font_width);
#else
  maxi_font = cFont::GetFont(fontFix);
  mini_font = NULL;
#endif
  command_menu = NULL;
  list_menu = NULL;
  setup_menu = NULL;
  listfile = AddDirectory(plugin->ConfigDirectory(plugin->Name()), SUDOKU_LIST);
}

/** Destructor */
Menu::~Menu()
{
  delete setup_menu;
  delete list_menu;
  delete command_menu;
#if VDRVERSNUM >= 10504
  delete maxi_font;
  delete mini_font;
#endif
  delete info;
  delete osd;
}

/** Display the menu on the OSD. */
void Menu::Show()
{
  int x1 = xPos,                 y1 = yPos;
  int x2 = xPos + GRID_SIZE - 1, y2 = yPos + GRID_SIZE -1;

  osd = cOsdProvider::NewOsd(0, 0);
  if (osd)
  {
    tArea area = { x1, y1, x2, y2, 4 };
    osd->SetAreas(&area, 1);
    paint();
  }
}

/** Process user events. */
eOSState Menu::ProcessKey(eKeys key)
{
  if (command_menu)
  {
    eOSState state = command_menu->ProcessKey(key);
    if (state == osBack)
    {
      state = osContinue;
      CommandType command = command_menu->get_selected_command();
      DELETENULL(command_menu);
      if (command)
        state = (this->*command)();
      if (state == osContinue)
        Show();
      if (state == osUnknown)
        state = osContinue;
    }
    return state;
  }

  if (list_menu)
  {
    eOSState state = list_menu->ProcessKey(key);
    if (state == osBack)
    {
      state = osContinue;
      const char* sudoku = list_menu->get_selected_sudoku();
      if (sudoku)
        puzzle->load_from_dump(sudoku);
      DELETENULL(list_menu);
      Show();
    }
    return state;
  }

  if (setup_menu)
  {
    eOSState state = setup_menu->ProcessKey(key);
    if (state == osBack)
    {
      state = osContinue;
      if (key == kOk)
        Setup.Save();
      DELETENULL(setup_menu);
#if VDRVERSNUM >= 10504
      DELETENULL(maxi_font);
      DELETENULL(mini_font);
      maxi_font = cFont::CreateFont(setup.maxi_font, setup.maxi_font_height,
                                    setup.maxi_font_width);
      mini_font = cFont::CreateFont(setup.mini_font, setup.mini_font_height,
                                    setup.mini_font_width);
#endif
      Show();
    }
    return state;
  }

  eOSState state = cOsdObject::ProcessKey(key);
  if (state == osUnknown)
  {
    Pos curr = puzzle->get_pos();
    CommandType command = NULL;
    switch (key)
    {
      case kLeft:
      case kLeft|k_Repeat:
        puzzle->set_pos(curr.prev_col());
        break;
      case kRight:
      case kRight|k_Repeat:
        puzzle->set_pos(curr.next_col());
        break;
      case kUp:
      case kUp|k_Repeat:
        puzzle->set_pos(curr.prev_row());
        break;
      case kDown:
      case kDown|k_Repeat:
        puzzle->set_pos(curr.next_row());
        break;
      case k0:
      case k1:
      case k2:
      case k3:
      case k4:
      case k5:
      case k6:
      case k7:
      case k8:
      case k9:
        puzzle->set_with_history(key - k0);
        break;
      case kRed:
        command = CommandList::command(setup.key_red);
        break;
      case kGreen:
        command = CommandList::command(setup.key_green);
        break;
      case kYellow:
        command = CommandList::command(setup.key_yellow);
        break;
      case kBlue:
        osd->Flush();
        DELETENULL(osd);
        command_menu = new CommandMenu();
        command_menu->Display();
        return osContinue;
      case kOk:
        if (new_puzzle_request)
          generate();
        break;
      case kBack:
        return exit();
      default:
        return osContinue;
    }
    if (command)
    {
      state = (this->*command)();
      if (state == osUnknown)
        return osContinue;
      if (state == osEnd)
        return osEnd;
    }
    new_puzzle_request = !new_puzzle_request && puzzle->solved();
    if (new_puzzle_request)
      infoText = tr("Congratulations!\nPress OK to start a new puzzle");
    paint();
    state = osContinue;
  }
  return state;
}

/** Generate a new puzzle. */
eOSState Menu::generate()
{
  puzzle->generate(setup.givens_count, setup.symmetric);
  return osContinue;
}

/** Load a puzzle. */
eOSState Menu::load()
{
  if (osd)
    osd->Flush();
  DELETENULL(osd);
  list_menu = new ListMenu(listfile);
  list_menu->Display();
  return osUnknown;
}

/** Save the puzzle. */
eOSState Menu::save()
{
  if (osd)
    osd->Flush();
  DELETENULL(osd);
  list_menu = new ListMenu(listfile, puzzle->get_dump());
  list_menu->Display();
  return osUnknown;
}

/** Undo last action. */
eOSState Menu::undo()
{
  puzzle->backward();
  return osContinue;
}

/** Redo last action. */
eOSState Menu::redo()
{
  puzzle->forward();
  return osContinue;
}

/** Mark/unmark the current cell. */
eOSState Menu::toggle_mark()
{
  puzzle->toggle_mark(puzzle->get_pos());
  return osContinue;
}

/** Move the cursor to the next free cell with minimal possible numbers. */
eOSState Menu::next_cell()
{
  Pos new_pos = puzzle->next_cell(puzzle->get_pos());
  if (new_pos <= Pos::last())
    puzzle->set_pos(new_pos);
  return osContinue;
}

/** Set the next possible number for the current cell. */
eOSState Menu::next_number()
{
  puzzle->set_with_history(puzzle->next_number(puzzle->get_pos()));
  return osContinue;
}

/** Reset the puzzle. */
eOSState Menu::reset()
{
  puzzle->reset(setup.clear_marks);
  return osContinue;
}

/** Open setup menu. */
eOSState Menu::open_setup()
{
  if (osd)
    osd->Flush();
  DELETENULL(osd);
  setup_menu = plugin->SetupMenu();
  setup_menu->SetPlugin(plugin);
  setup_menu->Display();
  return osUnknown;
}

/** Exit plugin menu. */
eOSState Menu::exit()
{
  return osEnd;
}

/** Paint all pieces of the menu. */
void Menu::paint()
{
  int trans = setup.transparency;
  tColor fg, bg = TRANS(GRID_COLOR, trans);
  int x1 = xPos,                 y1 = yPos;
  int x2 = xPos + GRID_SIZE - 1, y2 = yPos + GRID_SIZE -1;

  // Save and restore palette to reduce flickering.
  cPalette savePalette(*osd->GetBitmap(0));
  osd->DrawRectangle(x1, y1, x2, y2, bg);
  osd->SetPalette(savePalette, 0);

  // Paint all cells.
  for (Pos p = Pos::first(); p <= Pos::last(); p = p.next())
  {
    fg = NUMBER_FG, bg = NUMBER_BG;
    if (puzzle->given(p))
      fg = GIVEN_FG, bg = GIVEN_BG;
    else if (puzzle->marked(p))
      fg = MARKED_FG, bg = MARKED_BG;
    else if (setup.mark_errors && puzzle->error(p))
      fg = ERROR_FG, bg = ERROR_BG;
    else if (setup.mark_ambiguous && puzzle->ambiguous(p))
      fg = AMBIG_FG, bg = AMBIG_BG;
    fg = TRANS(fg, trans);
    bg = TRANS(bg, trans);

    // Paint the background of the cell.
    x1 = xPos + CELL_POS(p.col()), y1 = yPos + CELL_POS(p.row());
    x2 = x1   + CELL_SIZE - 1,     y2 = y1   + CELL_SIZE - 1;
    osd->DrawRectangle(x1, y1, x2, y2, bg);

    // Paint the content of the cell.
    if (puzzle->get(p) != 0)
    {
      char txt[2] = { '0' + puzzle->get(p), 0 };
      osd->DrawText(x1, y1, txt, fg, bg, maxi_font,
                    CELL_SIZE, CELL_SIZE, taCenter);
    }
    else if (setup.show_possibles_pattern || setup.show_possibles_digits)
    {
      for (unsigned int n = 1; n <= DIM; ++n)
      {
        if (puzzle->possible_number(p, n))
        {
          int x3 = x1 + (((n - 1) % RDIM) * CELL_SIZE) / RDIM;
          int y3 = y1 + (((n - 1) / RDIM) * CELL_SIZE) / RDIM;
          int x4 = x1 + (((n - 1) % RDIM + 1) * CELL_SIZE) / RDIM - 1;
          int y4 = y1 + (((n - 1) / RDIM + 1) * CELL_SIZE) / RDIM - 1;

          if (setup.show_possibles_pattern)
          {
            fg = TRANS(POSSIBLE_FG, trans);
            bg = TRANS(POSSIBLE_BG(n), trans);
            osd->DrawRectangle(x3, y3, x4, y4, bg);
          }

#if VDRVERSNUM >= 10504
          if (setup.show_possibles_digits)
          {
            char txt[2] = { '0' + n, 0 };
            osd->DrawText(x3, y3, txt, fg, bg, mini_font,
                          CELL_SIZE / RDIM, CELL_SIZE / RDIM, taCenter);
          }
#endif
        }
      }
    }
  }

  // Paint the cursor.
  Pos curr = puzzle->get_pos();
  bg = TRANS(CURSUR_COLOR, trans);
  x1 = xPos + CELL_POS(curr.col()), y1 = yPos + CELL_POS(curr.row());
  x2 = x1   + CELL_SIZE - 1,        y2 = y1   + CELL_SIZE - 1;
  osd->DrawRectangle(x1 - 5, y1 - 5, x2,     y1,     bg);
  osd->DrawRectangle(x1 - 5, y1,     x1,     y2 + 5, bg);
  osd->DrawRectangle(x1,     y2,     x2 + 5, y2 + 5, bg);
  osd->DrawRectangle(x2,     y1 - 5, x2 + 5, y2,     bg);

  // Paint info window.
  if (infoText)
  {
    if (!info)
      info = new Bitmap(GRID_SIZE - 20, 90);
    info->text(infoText);
    osd->DrawBitmap(xPos + 10, yPos + 10, *info);
    infoText = NULL;
  }

  osd->Flush();
}
