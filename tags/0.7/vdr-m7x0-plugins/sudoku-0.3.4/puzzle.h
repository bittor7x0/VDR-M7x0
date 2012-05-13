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
 * $Id: puzzle.h 142 2008-07-06 15:50:02Z tom $
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
  class History;


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
    mutable char* numbers_dump;

    // prevent use of copy constructor and copy operator
    Numbers(const Numbers& other);
    Numbers& operator=(const Numbers& other);

  public:

    /** Constructor */
    Numbers(const char* dump = 0);

    /** Destructor */
    virtual ~Numbers();

    /** Get the numbers as dump */
    virtual const char* get_dump() const;

    /** Remove all numbers. */
    virtual void reset();

    /** Set numbers from contents of sudoku if marked in marks. */
    virtual void set_contents(const Numbers& sudoku, const bool marks[SDIM]);

    /** Set the number into this cell. */
    virtual void set(Pos pos, unsigned int number);

    /** Get the number from this cell. */
    virtual unsigned int get(Pos pos) const;

    /** Load numbers from a dump. */
    virtual void load_from_dump(const char* dump);
  };


  //--- class Sudoku::Puzzle ---------------------------------------------------

  /** Sudoku puzzle */
  class Puzzle : public Numbers
  {
    Numbers givens;
    Numbers marks;
    bool numbers[SDIM][DIM+1];
    unsigned int count[SDIM];
    mutable char* puzzle_dump;

    // prevent use of copy constructor and copy operator
    Puzzle(const Puzzle& other);
    Puzzle& operator=(const Puzzle& other);

  public:

    /** Constructor */
    Puzzle(const char* dump = 0);

    /** Constructor with generation of a random puzzle */
    Puzzle(unsigned int givens_count, bool symmetric = true);

    /** Destructor */
    virtual ~Puzzle();

    /** Get the puzzle as dump */
    virtual const char* get_dump() const;

    /** Reset the puzzle (including marks). */
    virtual void reset();

    /** Reset the puzzle (either with or without marks). */
    virtual void reset(bool clear_marks);

    /** Set the number into this cell. */
    virtual void set(Pos pos, unsigned int number);

    /** Load a puzzle from a dump. */
    virtual void load_from_dump(const char* dump);

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
    Pos next_cell(Pos pos = Pos::last()) const;

    /** Get the next possible number for this cell. */
    unsigned int next_number(Pos pos) const;

    /** Get the count of possible numbers for this cell. */
    unsigned int numbers_count(Pos pos) const;

    /** Is this number in this cell a possible number? */
    bool possible_number(Pos pos, unsigned int number) const;

private:

    /** Compute all possible numbers for this cell. */
    void compute_numbers(Pos pos);

    /** Is the number in this cell a possible number? */
    bool correct(Pos pos) const;
  };


  //--- class Sudoku::PuzzleGame -----------------------------------------------

  /** Sudoku puzzle game */
  class PuzzleGame : public Puzzle
  {
    Pos pos;
    History* history;

    // prevent use of copy constructor and copy operator
    PuzzleGame(const PuzzleGame& other);
    PuzzleGame& operator=(const PuzzleGame& other);

  public:

    /** Constructor */
    PuzzleGame(const char* dump = 0);

    /** Constructor with generation of a random puzzle */
    PuzzleGame(unsigned int givens_count, bool symmetric = true);

    /** Destructor */
    virtual ~PuzzleGame();

    /** Reset the puzzle (including marks). */
    virtual void reset();

    /** Reset the puzzle (either with or without marks). */
    virtual void reset(bool clear_marks);

    /** Set the number into the current cell, write action into history. */
    virtual void set_with_history(unsigned int number);

    /** Get the position of the current cell. */
    Pos get_pos() const;

    /** Set the position of the current cell. */
    void set_pos(Pos new_pos);

    /** Go one step backward in the history. */
    void backward();

    /** Go one step forward in the history. */
    void forward();
  };

} // namespace Sudoku

#endif // VDR_SUDOKU_PUZZLE_H
