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
 * $Id: generator.h 106 2007-12-03 23:28:24Z tom $
 */

#ifndef VDR_SUDOKU_GENERATOR_H
#define VDR_SUDOKU_GENERATOR_H

#include "sudoku.h"
#include "backtrack.h"
#include "puzzle.h"


namespace Sudoku
{

  //--- class Sudoku::Generator ------------------------------------------------

  /** Implementation of a backtracking algorithm to generate Sudoku puzzles
   *
   * To generate Sudoku puzzles two nested backtracking algorithms are used.
   * First a random Sudoku solution is searched. Then the algorithm tries to
   * remove some numbers so that only the requested number of givens remains.
   * Each puzzle is checked with the nested solver algorithm if there is only
   * one solution.
   *
   * Example:
   *
   * \code
   * Puzzle puzzle;
   * Generator generator(puzzle, 36);
   * generator.find_next_solution();
   * bool found = generator.solution_is_valid();
   * \endcode
   */
  class Generator : public BackTrack::Algorithm, public BackTrack::Solution
  {
    Puzzle& puzzle;
    Puzzle sudoku;
    unsigned int free_list[SDIM];
    unsigned int free_count;
    Pos pos_list[SDIM];
    unsigned int pos_count;
    bool symmetric;
    bool free_center;

  public:

    /** Constructor */
    Generator(Puzzle& puzzle, unsigned int givens_count,
              bool symmetric = true, unsigned int max_iter = 0);

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

#endif // VDR_SUDOKU_GENERATOR_H
