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
 * $Id: solver.cpp 117 2008-03-21 17:57:50Z tom $
 */

#define USE_RAND
#include "solver.h"
#include "backtrack.h"
#include "puzzle.h"
#include <assert.h>

using namespace Sudoku;
using namespace BackTrack;


//--- class Sudoku::Solver -----------------------------------------------------

/** Constructor */
Solver::Solver(Puzzle& puzzle, bool random_init, unsigned int max_iter) :
  Algorithm(*this, max_iter), puzzle(puzzle), random_init(random_init)
{
  free_count = 0;
  for (Pos p = Pos::first(); p <= Pos::last(); p = p.next())
    if (!puzzle.given(p))
      ++free_count;
}

/** Set the element to the first sibling. */
void Solver::set_first_at(unsigned int level)
{
  assert(level < free_count);
  const Pos p = puzzle.next_free();
  assert(p <= Pos::last());

  free_list[level] = p;
  puzzle.set(p, 0);

  if (level == 0)
    random = random_init;

  unsigned int i, count = puzzle.numbers_count(p);
  if (count != 0)
  {
    puzzle.set(p, puzzle.next_number(p));

    if (random)
      for (count = rand(count), i = 0; i < count; ++i)
        puzzle.set(p, puzzle.next_number(p));
  }
  else
    puzzle.set(p, 1);
}

/** Set the element to the next sibling. */
void Solver::set_next_at(unsigned int level)
{
  assert(level < free_count);
  Pos p = free_list[level];
  unsigned int n = puzzle.next_number(p);
  if (n != 0)
    puzzle.set(p, n);
}

/** Reset the element. */
void Solver::reset_at(unsigned int level)
{
  assert(level < free_count);
  puzzle.set(free_list[level], 0);
  random = false;
}

/** Check if the element is set to the last sibling. */
bool Solver::is_last_at(unsigned int level) const
{
  assert(level < free_count);
  return puzzle.next_number(free_list[level]) == 0;
}

/** Check if the element is valid (following elements ignored). */
bool Solver::is_valid_at(int level) const
{
  assert(level < int(free_count));
  if (level < 0)
    return puzzle.solved();
  return !puzzle.error(free_list[level]);
}

/** Check if the level is the last possible level. */
bool Solver::is_last_level(int level) const
{
  return level >= int(free_count) - 1;
}
