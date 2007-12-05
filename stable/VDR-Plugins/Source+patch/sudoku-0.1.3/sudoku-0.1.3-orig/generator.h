/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: generator.h 11 2005-10-28 01:00:01Z tom $
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
