/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menu.h 27 2006-04-25 00:08:15Z tom $
 */

#ifndef VDR_SUDOKU_MENU_H
#define VDR_SUDOKU_MENU_H

#include "sudoku.h"
namespace SudokuPlugin { class SetupData; class Bitmap; }
namespace Sudoku { class Puzzle; class Pos; }
#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/osd.h>


namespace SudokuPlugin
{

  //--- class SudokuPlugin::Menu -----------------------------------------------

  /** Main menu of the plugin */
  class Menu : public cOsdObject
  {
    const SetupData& setup;
    Sudoku::Puzzle& puzzle;
    Sudoku::Pos& curr;
    int xPos, yPos;
    cOsd* osd;
    Bitmap* info;
    const char* infoText;
    bool new_puzzle_request;

  public:

    /** Constructor */
    Menu(const SetupData& setup, Sudoku::Puzzle& puzzle, Sudoku::Pos& curr);

    /** Destructor */
    virtual ~Menu();

    /** Display the menu on the OSD. */
    virtual void Show();

    /** Process user events. */
    virtual eOSState ProcessKey(eKeys key);

  private:

    /** Paint all pieces of the menu. */
    void paint();
  };

} // namespace SudokuPlugin

#endif // VDR_SUDOKU_MENU_H
