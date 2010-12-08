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
 * $Id: sudoku.cpp 165 2008-11-29 00:09:41Z tom $
 */

#include "sudoku.h"
namespace Sudoku { class PuzzleGame; }
#include "menu.h"
#include "setup.h"
#include "i18n.h"
#include <vdr/plugin.h>
#include <vdr/config.h>


/** 'Sudoku' is a VDR plugin to generate and solve Number Place puzzles. */
namespace SudokuPlugin
{

  /** Version number of the plugin */
  static const char* VERSION = "0.3.4";

  /** Short description of the plugin's purpose */
  static const char* DESCRIPTION =
    trNOOP("Sudoku - generate and solve Number Place puzzles");

  /** Name of the entry in VDR's main menu */
  static const char* MAINMENUENTRY = trNOOP("Sudoku");


  //--- class SudokuPlugin::Plugin ---------------------------------------------

  /** Main class of the VDR plugin 'Sudoku' */
  class Plugin : public cPlugin
  {
    SetupData setup;
    Sudoku::PuzzleGame* puzzle;

  public:

    /** Version number of the plugin */
    virtual const char* Version() { return VERSION; }

    /** Localized short description of the plugin's purpose */
    virtual const char* Description() { return tr(DESCRIPTION); }

    /** Perform the startup actions of the plugin. */
    virtual bool Start();

    /** Localized name of the entry in VDR's main menu */
    virtual const char* MainMenuEntry() { return tr(MAINMENUENTRY); }

    /** OSD object that shows the plugin's main menu */
    virtual cOsdObject* MainMenuAction();

    /** Setup menu page to adjust the setup parameters of the plugin */
    virtual cMenuSetupPage* SetupMenu();

    /** Parse the setup parameters of the plugin. */
    virtual bool SetupParse(const char* name, const char* value);
  };

} // namespace SudokuPlugin


using namespace SudokuPlugin;


//--- class SudokuPlugin::Plugin -----------------------------------------------

/** Perform the startup actions of the plugin.
 *
 * This method is called once at VDR's startup.
 */
bool Plugin::Start()
{
#if VDRVERSNUM < 10507
  RegisterI18n(Phrases);
#endif
  puzzle = NULL;
  return true;
}

/** OSD object that shows the plugin's main menu
 *
 * This method is called every time the plugin's main menu entry is selected.
 */
cOsdObject* Plugin::MainMenuAction()
{
  return new Menu(this, setup, puzzle);
}

/** Setup menu page to adjust the setup parameters of the plugin
 *
 * This method is called every time the plugin's setup menu entry is selected.
 */
cMenuSetupPage* Plugin::SetupMenu()
{
  return new SetupPage(setup);
}

/** Parse the setup parameters of the plugin.
 *
 * This method is called for each setup parameter the plugin has previously
 * stored in the global setup data.
 */
bool Plugin::SetupParse(const char* name, const char* value)
{
  return setup.parse(name, value);
}

/** "Magic" hook that allows VDR to load the plugin into its memory */
VDRPLUGINCREATOR(Plugin); // Don't touch this!
