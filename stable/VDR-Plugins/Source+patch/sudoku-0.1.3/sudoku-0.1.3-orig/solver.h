/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: solver.h 18 2005-11-03 21:48:08Z tom $
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
