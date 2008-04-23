/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: puzzle.h 16 2005-10-31 21:12:41Z tom $
 */

#ifndef VDR_SUDOKU_PUZZLE_H
#define VDR_SUDOKU_PUZZLE_H

#include "sudoku.h"


/** Sudoku puzzle
 *
 * A Sudoku puzzle consists of 9 x 9 cells subdivided into 9 regions with 3 x 3
 * cells. The rules are simple. There have to be the numbers from 1 to 9 in
 * every row, column and region.
 */
namespace Sudoku
{

  /** Regions, rows/columns and square dimension of the puzzle */
  enum
  {
    /** Regions dimension. Width and height of a region */
    RDIM = 3,

    /** Dimension. Number of cells in a row, column or region */
    DIM = RDIM * RDIM,

    /** Square dimension. Number of cells in the whole puzzle */
    SDIM = DIM * DIM
  };


  //--- class Sudoku::Pos ------------------------------------------------------

  /** Position in a Sudoku */
  class Pos
  {
    unsigned int pos;
  public:
    Pos(unsigned int col, unsigned int row) : pos(col + row * DIM) {}
    Pos(unsigned int pos = 0) : pos(pos) {}
    operator unsigned int() const { return pos; }
    static Pos first() { return 0; }
    static Pos last() { return SDIM-1; }
    Pos next() const { return pos + 1; }
    unsigned int col() const { return pos % DIM; }
    unsigned int row() const { return pos / DIM; }
    unsigned int reg() const { return (col() / RDIM) + RDIM * (row() / RDIM); }
    static Pos center() { return SDIM / 2; }
    Pos symmetric() const { return SDIM - 1 - pos; }
    Pos prev_col() const { return col() > 0     ? pos - 1   : pos; }
    Pos next_col() const { return col() < DIM-1 ? pos + 1   : pos; }
    Pos prev_row() const { return row() > 0     ? pos - DIM : pos; }
    Pos next_row() const { return row() < DIM-1 ? pos + DIM : pos; }
  };


  //--- class Sudoku::Numbers --------------------------------------------------

  /** Numbers of a Sudoku */
  class Numbers
  {
    unsigned int content[SDIM];

  public:

    /** Constructor */
    Numbers();

    /** Destructor */
    virtual ~Numbers();

    /** Remove all numbers. */
    virtual void reset();

    /** Set numbers from contents of sudoku if marked in marks. */
    virtual void set_contents(const Numbers& sudoku, const bool marks[SDIM]);

    /** Set the number into this cell. */
    virtual void set(Pos pos, unsigned int number);

    /** Get the number from this cell. */
    virtual unsigned int get(Pos pos) const;
  };


  //--- class Sudoku::Puzzle ---------------------------------------------------

  /** Sudoku puzzle */
  class Puzzle : public Numbers
  {
    Numbers givens;
    bool marks[SDIM];
    bool numbers[SDIM][DIM+1];
    unsigned int count[SDIM];

  public:

    /** Constructor */
    Puzzle(unsigned int givens_count = 0, bool symmetric = true);

    /** Reset the puzzle (including marks). */
    virtual void reset();

    /** Reset the puzzle (either with or without marks). */
    virtual void reset(bool clear_marks);

    /** Set the number into this cell. */
    virtual void set(Pos pos, unsigned int number);

    /** Generate a new puzzle. */
    void generate(unsigned int givens_count, bool symmetric = true);

    /** Set givens from contents of sudoku if marked in given_marks. */
    void set_givens(const Numbers& sudoku, const bool given_marks[SDIM]);

    /** Remove all givens. */
    void clear_givens();

    /** No cells set? */
    bool untouched() const;

    /** Is the number in this cell given? */
    bool given(Pos pos) const;

    /** Is there an error on this position? */
    bool error(Pos pos) const;

    /** Is the number in this cell ambiguous? */
    bool ambiguous(Pos pos) const;

    /** All cells set and no errors? */
    bool solved() const;

    /** Is this cell marked? */
    bool marked(Pos pos) const;

    /** Toggle the mark for this cell. */
    void toggle_mark(Pos pos);

    /** Get the next free cell with minimal possible numbers. */
    Pos next_free(Pos pos = Pos::last()) const;

    /** Get the next possible number for this cell. */
    unsigned int next_number(Pos pos);

    /** Get the count of possible numbers for this cell. */
    unsigned int numbers_count(Pos pos);

private:

    /** Compute all possible numbers for this cell. */
    void compute_numbers(Pos pos);

    /** Is the number in this cell a possible number? */
    bool correct(Pos pos) const;
  };

} // namespace Sudoku

#endif // VDR_SUDOKU_PUZZLE_H
