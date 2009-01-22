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
 * $Id: history.cpp 140 2008-06-30 22:10:38Z tom $
 */

#include "history.h"
#include "puzzle.h"

using namespace Sudoku;


//--- class Sudoku::History ----------------------------------------------------

/** Constructor */
History::History()
{
  executed = 0;
}

/** Destructor */
History::~History()
{
  reset();
}

/** Reset the history. */
void History::reset()
{
  for (unsigned int pos = history.size(); pos > 0; --pos)
  {
    // Remove object created outside of History
    delete history.back();
    history.pop_back();
  }
  executed = 0;
}

/** Current move in the history */
Move* History::current()
{
  return history[executed - 1];
}

/** Add a new move */
void History::add(Move* move)
{
  for (unsigned int pos = history.size(); pos > executed; --pos)
  {
    // Remove object created outside of History
    delete history.back();
    history.pop_back();
  }
  history.push_back(move);
  ++executed;
}

/** Set previous move as current */
void History::backward()
{
  if (movesExecuted())
    --executed;
}

/** Set next move as current */
void History::forward()
{
  if (movesToExecute())
    ++executed;
}

/** Are there executed moves in the history */
bool History::movesExecuted()
{
  return executed > 0;
}

/** Are there moves to execute in the history */
bool History::movesToExecute()
{
  return executed < history.size();
}


//--- class Sudoku::SetMove ----------------------------------------------------

/** Constructor */
SetMove::SetMove(PuzzleGame& puzzle, unsigned int number) :
  puzzle(puzzle)
{
  pos = puzzle.get_pos();
  old_number = puzzle.get(pos);
  new_number = number;
}

/** Do the move. */
void SetMove::execute()
{
  puzzle.set(pos, new_number);
  puzzle.set_pos(pos);
}

/** Undo the move. */
void SetMove::takeBack()
{
  puzzle.set(pos, old_number);
  puzzle.set_pos(pos);
}
