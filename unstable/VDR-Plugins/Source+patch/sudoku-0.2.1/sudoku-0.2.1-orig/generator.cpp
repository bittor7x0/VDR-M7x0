/*
 * Sudoku: A plug-in for the Video Disk Recorder
 *
 * Copyright (C) 2005-2007, Thomas G�nther <tom@toms-cafe.de>
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
 * $Id: generator.cpp 106 2007-12-03 23:28:24Z tom $
 */

#define USE_RAND
#include "generator.h"
#include "solver.h"
#include "backtrack.h"
#include "puzzle.h"
#include <assert.h>

using namespace Sudoku;
using namespace BackTrack;


//--- class Sudoku::Generator --------------------------------------------------

/** Constructor */
Generator::Generator(Puzzle& puzzle, unsigned int givens_count,
                     bool symmetric, unsigned int max_iter) :
  Algorithm(*this, max_iter), puzzle(puzzle), symmetric(symmetric)
{
  assert(givens_count <= SDIM);

  // Search a random Sudoku solution.
  for (bool found = false; !found;)
  {
    sudoku.reset();
    Solver solver(sudoku, true);
    solver.find_next_solution();
    found = solver.solution_is_valid();
  }

  // If symmetric pos_list contains only the first halve of all positions.
  pos_count = SDIM;
  free_count = SDIM - givens_count;
  free_center = symmetric && pos_count % 2 != 0 && free_count % 2 != 0;
  if (symmetric)
    pos_count /= 2, free_count /= 2;

  // Fill pos_list with positions in random order.
  bool list[pos_count];
  unsigned int p, i, c;
  for (p = 0; p < pos_count; ++p)
    list[p] = true;
  for (i = 0; i < pos_count; ++i)
  {
    c = rand(pos_count - i) + 1;
    for (p = 0; p < pos_count; ++p)
      if (list[p])
        if (--c == 0)
          break;
    assert(p < pos_count);
    list[p] = false;
    pos_list[i] = p;
  }
}

/** Set the element to the first sibling. */
void Generator::set_first_at(unsigned int level)
{
  assert(level < free_count);
  free_list[level] = 0;
}

/** Set the element to the next sibling. */
void Generator::set_next_at(unsigned int level)
{
  assert(level < free_count);
  ++free_list[level];
}

/** Reset the element. */
void Generator::reset_at(unsigned int level)
{
}

/** Check if the element is set to the last sibling. */
bool Generator::is_last_at(unsigned int level) const
{
  assert(level < free_count);
  return free_list[level] >= pos_count - 1;
}

/** Check if the element is valid (following elements ignored). */
bool Generator::is_valid_at(int level) const
{
  assert(level < int(free_count));

  // free_list contains ordered indices to pos_list.
  if (level > 0 && free_list[level] <= free_list[level - 1])
    return false;
  if (level >= 0 && free_list[level] > pos_count - (free_count - level))
    return false;

  // Fill list with marks for givens.
  bool given_marks[SDIM];
  int i;
  for (i = 0; i < SDIM; ++i)
    given_marks[i] = true;
  for (i = 0; i <= level; ++i)
  {
    Pos p = pos_list[free_list[i]];
    given_marks[p] = false;
    if (symmetric)
      given_marks[p.symmetric()] = false;
  }
  if (free_center)
    given_marks[Pos::center()] = false;

  // Set givens in puzzle and check if it has only one solution.
  puzzle.set_givens(sudoku, given_marks);
  Solver solver(puzzle);
  solver.find_next_solution();
  assert(solver.solution_is_valid());
  solver.find_next_solution();
  return !solver.solution_is_valid();
}

/** Check if the level is the last possible level. */
bool Generator::is_last_level(int level) const
{
  return level >= int(free_count) - 1;
}
