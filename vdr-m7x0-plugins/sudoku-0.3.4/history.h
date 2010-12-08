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
 * $Id: history.h 140 2008-06-30 22:10:38Z tom $
 */

#ifndef VDR_SUDOKU_HISTORY_H
#define VDR_SUDOKU_HISTORY_H

#include "sudoku.h"


namespace Sudoku
{
  class PuzzleGame;
  class Pos;
  class Move;
  typedef Vector<Move*> Moves;


  //--- class Sudoku::History --------------------------------------------------

  class History
  {
  private:
    Moves history;
    unsigned int executed;

  public:

    /** Constructor */
    History();

    /** Destructor */
    ~History();

    /** Reset the history. */
    void reset();

    /** Current move in the history */
    Move* current();

    /** Add a new move */
    void add(Move* move);

    /** Set previous move as current */
    void backward();

    /** Set next move as current */
    void forward();

    /** Are there executed moves in the history */
    bool movesExecuted();

    /** Are there moves to execute in the history */
    bool movesToExecute();
  };


  //--- class Sudoku::Move -----------------------------------------------------

  class Move
  {
  public:

    /** Destructor */
    virtual ~Move() {};

    /** Do the move */
    virtual void execute() = 0;

    /** Undo the move. */
    virtual void takeBack() = 0;
  };


  //--- class Sudoku::SetMove --------------------------------------------------

  class SetMove : public Move
  {
  private:
    PuzzleGame& puzzle;
    unsigned int pos;
    unsigned int old_number;
    unsigned int new_number;

  public:

    /** Constructor */
    SetMove(PuzzleGame& puzzle, unsigned int number);

    /** Do the move. */
    void execute();

    /** Undo the move. */
    void takeBack();
  };

} // namespace Sudoku

#endif // VDR_SUDOKU_HISTORY_H
