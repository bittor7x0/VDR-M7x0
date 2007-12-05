/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: sudoku.cpp 70 2007-02-23 20:45:21Z tom $
 */

#include "sudoku.h"
#include "puzzle.h"
#include "menu.h"
#include "setup.h"
#include "i18n.h"
#include <vdr/plugin.h>


/** 'Sudoku' is a VDR plugin to generate and solve Number Place puzzles. */
namespace SudokuPlugin
{

  /** Version number of the plugin */
  static const char* VERSION = "0.1.3";

  /** Short description of the plugin's purpose */
  static const char* DESCRIPTION =
    "Sudoku - generate and solve Number Place puzzles";

  /** Name of the entry in VDR's main menu */
  static const char* MAINMENUENTRY = "Sudoku";


  //--- class SudokuPlugin::Plugin ---------------------------------------------

  /** Main class of the VDR plugin 'Sudoku' */
  class Plugin : public cPlugin
  {
    SetupData setup;
    Sudoku::Puzzle puzzle;
    Sudoku::Pos curr;

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
  RegisterI18n(Phrases);
  puzzle.generate(setup.givens_count, setup.symmetric);
  curr = curr.center();
  return true;
}

/** OSD object that shows the plugin's main menu
 *
 * This method is called every time the plugin's main menu entry is selected.
 */
cOsdObject* Plugin::MainMenuAction()
{
  return new Menu(setup, puzzle, curr);
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
