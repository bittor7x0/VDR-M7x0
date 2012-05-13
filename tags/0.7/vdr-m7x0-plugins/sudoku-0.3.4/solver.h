/*
 * Sudoku: A plug-in for the Video Disk Recorder
 *
 * Copyright (C) 2005-2007, Thomas Günther <tom@toms-cafe.de>
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
 * $Id: solver.h 106 2007-12-03 23:28:24Z tom $
 */

#ifndef VDR_SUDOKU_SOLVER_H
#define VDR_SUDOKU_SOLVER_H

#include "sudoku.h"
#include "backtrack.h"
#include "puzzle.h"


namespace Sudoku
{

  //--- class Sudoku::Solver ---------------------------------------------------

  /** Implementation of a backtracking algorithm to solve Sudoku puzzles
   *
   * The algorithm tries to set numbers in all free cells of the puzzle
   * sticking to the rules of Sudoku puzzles.
   *
   * There are three ways to use this class:
   *
   * 1. Solve a Sudoku puzzle with some numbers given:
   *
   *    \code
   *    Puzzle puzzle(36); // Generate a random puzzle with 36 givens.
   *    Solver solver(puzzle);
   *    solver.find_next_solution();
   *    if (solver.solution_is_valid())
   *    {
   *      // Do something with the puzzle.
   *      ...
   *    }
   *    solver.find_next_solution();
   *    bool more_than_one_solution = solver.solution_is_valid();
   *    \endcode
   *
   * 2. Search a random Sudoku solution:
   *
   *    \code
   *    Puzzle puzzle; // Generate an empty puzzle without givens.
   *    Solver solver(puzzle, true);
   *    solver.find_next_solution();
   *    if (solver.solution_is_valid())
   *    {
   *      // Do something with the puzzle.
   *      ...
   *    }
   *    \endcode
   *
   * 3. Search all Sudoku solutions:
   *
   *    \code
   *    Puzzle puzzle; // Generate an empty puzzle without givens.
   *    Solver solver(puzzle);
   *    solver.find_next_solution();
   *    while (solver.solution_is_valid())
   *    {
   *      // Do something with the puzzle.
   *      ...
   *      solver.find_next_solution();
   *    }
   *    \endcode
   */
  class Solver : public BackTrack::Algorithm, public BackTrack::Solution
  {
    Puzzle& puzzle;
    Pos free_list[SDIM];
    unsigned int free_count;
    bool random_init, random;

  public:

    /** Constructor */
    Solver(Puzzle& puzzle, bool random_init = false, unsigned int max_iter = 0);

    /** Set the element to the first sibling. */
    virtual void set_first_at(unsigned int level);

    /** Set the element to the next sibling. */
    virtual void set_next_at(unsigned int level);

    /** Reset the element. */
    virtual void reset_at(unsigned int level);

    /** Check if the element is set to the last sibling. */
    virtual bool is_last_at(unsigned int level) const;

    /** Check if the element is valid (following elements ignored). */
    virtual bool is_valid_at(int level) const;

    /** Check if the level is the last possible level. */
    virtual bool is_last_level(int level) const;
  };

} // namespace Sudoku

#endif // VDR_SUDOKU_SOLVER_H
