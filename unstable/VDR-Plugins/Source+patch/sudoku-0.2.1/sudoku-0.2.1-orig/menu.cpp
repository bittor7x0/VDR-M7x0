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
 * $Id: menu.cpp 124 2008-03-23 13:32:18Z tom $
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
Menu::Menu(cPlugin* plugin, const SetupData& setup, Puzzle& puzzle, Pos& curr) :
  cOsdObject(true), plugin(plugin), setup(setup), puzzle(puzzle), curr(curr)
{
  xPos = (720 - GRID_SIZE) / 2;
  yPos = (576 - GRID_SIZE) / 2;
  osd = NULL;
  info = NULL;
  infoText = NULL;
  new_puzzle_request = false;
#if VDRVERSNUM >= 10504
  maxi_font = cFont::CreateFont(Setup.FontOsd, 3 * CELL_SIZE / 4, CELL_SIZE);
  mini_font = cFont::CreateFont(Setup.FontOsd, 3 * CELL_SIZE / RDIM / 4,
                                CELL_SIZE / RDIM);
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
        puzzle = Puzzle(sudoku);
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
      Show();
    }
    return state;
  }

  eOSState state = cOsdObject::ProcessKey(key);
  if (state == osUnknown)
  {
    if (key == kBack)
      return exit();
    if (key == kBlue)
    {
      osd->Flush();
      DELETENULL(osd);
      command_menu = new CommandMenu();
      command_menu->Display();
      return osContinue;
    }
    if (new_puzzle_request)
    {
      if (key == kOk)
        generate();
    }
    else
    {
      switch (key)
      {
        case kLeft:
        case kLeft|k_Repeat:
          curr = curr.prev_col();
          break;
        case kRight:
        case kRight|k_Repeat:
          curr = curr.next_col();
          break;
        case kUp:
        case kUp|k_Repeat:
          curr = curr.prev_row();
          break;
        case kDown:
        case kDown|k_Repeat:
          curr = curr.next_row();
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
          puzzle.set(curr, key - k0);
          break;
        case kRed:
          puzzle.set(curr, puzzle.next_number(curr));
          break;
        case kGreen:
          puzzle.toggle_mark(curr);
          break;
        case kYellow:
          if (puzzle.next_free(curr) <= Pos::last())
            curr = puzzle.next_free(curr);
          break;
        default:
          return osContinue;
      }
    }
    if (puzzle.solved())
    {
      new_puzzle_request = true;
      infoText = tr("Congratulations!\nPress OK to start a new puzzle");
    }
    paint();
    state = osContinue;
  }
  return state;
}

/** Generate a new puzzle. */
eOSState Menu::generate()
{
  puzzle.generate(setup.givens_count, setup.symmetric);
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
  list_menu = new ListMenu(listfile, puzzle.get_dump());
  list_menu->Display();
  return osUnknown;
}

/** Reset the puzzle. */
eOSState Menu::reset()
{
  puzzle.reset(setup.clear_marks);
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
    if (puzzle.given(p))
      fg = GIVEN_FG, bg = GIVEN_BG;
    else if (puzzle.marked(p))
      fg = MARKED_FG, bg = MARKED_BG;
    else if (setup.mark_errors && puzzle.error(p))
      fg = ERROR_FG, bg = ERROR_BG;
    else if (setup.mark_ambiguous && puzzle.ambiguous(p))
      fg = AMBIG_FG, bg = AMBIG_BG;
    fg = TRANS(fg, trans);
    bg = TRANS(bg, trans);

    // Paint the background of the cell.
    x1 = xPos + CELL_POS(p.col()), y1 = yPos + CELL_POS(p.row());
    x2 = x1   + CELL_SIZE - 1,     y2 = y1   + CELL_SIZE - 1;
    osd->DrawRectangle(x1, y1, x2, y2, bg);

    // Paint the content of the cell.
    if (puzzle.get(p) != 0)
    {
      char txt[2] = { '0' + puzzle.get(p), 0 };
      osd->DrawText(x1, y1, txt, fg, bg, maxi_font,
                    CELL_SIZE, CELL_SIZE, taCenter);
    }
    else if (setup.show_possibles_pattern || setup.show_possibles_digits)
    {
      for (unsigned int n = 1; n <= DIM; ++n)
      {
        if (puzzle.possible_number(p, n))
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
  else
    new_puzzle_request = false;

  osd->Flush();
}
