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
 * $Id: puzzle.cpp 117 2008-03-21 17:57:50Z tom $
 */

#include "puzzle.h"
#include "generator.h"
#include <string.h>
#include <assert.h>

using namespace Sudoku;


//--- class Sudoku::Numbers ----------------------------------------------------

/** Constructor */
Numbers::Numbers(const char* dump) :
  numbers_dump()
{
  reset();

  if (dump)
    for (unsigned int i = 0; *dump && *dump != ':' && i < SDIM; ++i, ++dump)
      if (*dump == '+')
        --i;
      else if (*dump > '0' && *dump - '0' <= DIM)
        content[i] = *dump - '0';

  assert(!numbers_dump);
}

/** Destructor */
Numbers::~Numbers()
{
  delete[] numbers_dump;
}

/** Get the numbers as dump */
const char* Numbers::get_dump() const
{
  if (!numbers_dump)
    numbers_dump = new char[SDIM + DIM];
  assert(numbers_dump);

  char* dump = numbers_dump;
  unsigned int n;
  for (unsigned int row = 0; row < DIM; ++row, *dump++ = '+')
    for (unsigned int col = 0; col < DIM; ++col, ++dump)
      *dump = (n = get(Pos(col, row))) ? '0' + n : '_';
  *--dump = 0;

  return numbers_dump;
}

/** Remove all numbers. */
void Numbers::reset()
{
  for (unsigned int i = 0; i < SDIM; ++i)
    content[i] = 0;
}

/** Set numbers from contents of sudoku if marked in marks. */
void Numbers::set_contents(const Numbers& sudoku, const bool marks[SDIM])
{
  for (unsigned int i = 0; i < SDIM; ++i)
    if (marks[i])
      content[i] = sudoku.get(i);
    else
      content[i] = 0;
}

/** Set the number into this cell. */
void Numbers::set(Pos pos, unsigned int number)
{
  assert(pos <= Pos::last());
  assert(0 <= number && number <= DIM);
  content[pos] = number;
}

/** Get the number from this cell. */
unsigned int Numbers::get(Pos pos) const
{
  assert(pos <= Pos::last());
  assert(0 <= content[pos] && content[pos] <= DIM);
  return content[pos];
}


//--- class Sudoku::Puzzle -----------------------------------------------------

/** Constructor */
Puzzle::Puzzle(const char* dump) :
  puzzle_dump()
{
  // Set givens from the first part of the dump (before the first colon)
  givens = Numbers(dump);
  reset();

  // Set numbers from the second part (between the first and the second colon)
  if (dump)
    dump = strchr(dump, ':');
  if (dump)
  {
    Numbers numbers(++dump);
    for (unsigned int i = 0; i < SDIM; ++i)
      if (numbers.get(i) != 0)
        set(i, numbers.get(i));
  }

  // Set marks from the third part (behind the second colon)
  if (dump)
    dump = strchr(dump, ':');
  if (dump)
    marks = Numbers(++dump);

  assert(!puzzle_dump);
}

/** Constructor with generation of a random puzzle */
Puzzle::Puzzle(unsigned int givens_count, bool symmetric) :
  puzzle_dump()
{
  if (givens_count == 0)
    clear_givens();
  else
    generate(givens_count, symmetric);

  assert(!puzzle_dump);
}

/** Destructor */
Puzzle::~Puzzle()
{
  delete[] puzzle_dump;
}

/** Get the puzzle as dump */
const char* Puzzle::get_dump() const
{
  if (!puzzle_dump)
    puzzle_dump = new char[3 * (SDIM + DIM)];
  assert(puzzle_dump);

  // Set givens as first part of the dump
  strcpy(puzzle_dump, givens.get_dump());
  if (!untouched())
  {
    // Set numbers as second part of the dump
    strcat(puzzle_dump, ":");
    strcat(puzzle_dump, Numbers::get_dump());

    // Set marks as third part of the dump
    strcat(puzzle_dump, ":");
    strcat(puzzle_dump, marks.get_dump());
  }

  return puzzle_dump;
}

/** Reset the puzzle (including marks). */
void Puzzle::reset()
{
  reset(true);
}

/** Reset the puzzle (either with or without marks). */
void Puzzle::reset(bool clear_marks)
{
  unsigned int i;

  // Fill the puzzle with the givens.
  for (i = 0; i < SDIM; ++i)
    Numbers::set(i, givens.get(i));

  // Compute possible numbers for all cells.
  for (i = 0; i < SDIM; ++i)
    compute_numbers(i);

  // Reset marked cells.
  if (clear_marks)
    marks.reset();
}

/** Set the number into this cell. */
void Puzzle::set(Pos pos, unsigned int number)
{
  assert(pos <= Pos::last());
  assert(0 <= number && number <= DIM);

  if (!given(pos) && get(pos) != number)
  {
    Numbers::set(pos, number);

    // Refresh possible numbers of all affected cells.
    for (Pos p = Pos::first(); p <= Pos::last(); p = p.next())
      if (p.col() == pos.col() || p.row() == pos.row() || p.reg() == pos.reg())
        compute_numbers(p);
  }
}

/** Generate a new puzzle. */
void Puzzle::generate(unsigned int givens_count, bool symmetric)
{
  // Search a random non-ambiguous puzzle.
  for (bool found = false; !found;)
  {
    Generator generator(*this, givens_count, symmetric, symmetric ? 500 : 2000);
    generator.find_next_solution();
    found = generator.solution_is_valid();
  }
  reset();
}

/** Set givens from contents of sudoku if marked in given_marks. */
void Puzzle::set_givens(const Numbers& sudoku, const bool given_marks[SDIM])
{
  givens.set_contents(sudoku, given_marks);
  reset();
}

/** Remove all givens. */
void Puzzle::clear_givens()
{
  givens.reset();
  reset();
}

/** No cells set? */
bool Puzzle::untouched() const
{
  for (unsigned int i = 0; i < SDIM; ++i)
    if (get(i) != givens.get(i))
      return false;

  return true;
}

/** Is the number in this cell given? */
bool Puzzle::given(Pos pos) const
{
  assert(pos <= Pos::last());
  return givens.get(pos) != 0;
}

/** Is there an error on this position? */
bool Puzzle::error(Pos pos) const
{
  assert(pos <= Pos::last());
  return !correct(pos);
}

/** Is the number in this cell ambiguous? */
bool Puzzle::ambiguous(Pos pos) const
{
  assert(pos <= Pos::last());
  return get(pos) != 0 && count[pos] > 1;
}

/** All cells set and no errors? */
bool Puzzle::solved() const
{
  for (unsigned int i = 0; i < SDIM; ++i)
    if (get(i) == 0 || !correct(i))
      return false;

  return true;
}

/** Is this cell marked? */
bool Puzzle::marked(Pos pos) const
{
  assert(pos <= Pos::last());
  return marks.get(pos) != 0;
}

/** Toggle the mark for this cell. */
void Puzzle::toggle_mark(Pos pos)
{
  assert(pos <= Pos::last());
  marks.set(pos, marks.get(pos) != 0 ? 0 : 1);
}

/** Get the next free cell with minimal possible numbers. */
Pos Puzzle::next_free(Pos pos) const
{
  unsigned int min_count = DIM+1, min_index = SDIM, i;

  for (pos = (pos+1)%SDIM, i = 0; i < SDIM; ++i, pos = (pos+1)%SDIM)
    if (get(pos) == 0 && count[pos] < min_count)
      min_count = count[pos], min_index = pos;

  return min_index;
}

/** Get the next possible number for this cell. */
unsigned int Puzzle::next_number(Pos pos)
{
  assert(pos <= Pos::last());
  unsigned int n = get(pos);
  unsigned int i;

  if (!given(pos))
    for (n = (n+1)%(DIM+1), i = 0; i < DIM+1; ++i, n = (n+1)%(DIM+1))
      if (numbers[pos][n])
        break;

  return n;
}

/** Get the count of possible numbers for this cell. */
unsigned int Puzzle::numbers_count(Pos pos)
{
  assert(pos <= Pos::last());
  return count[pos];
}

/** Is this number in this cell a possible number? */
bool Puzzle::possible_number(Pos pos, unsigned int number)
{
  assert(pos <= Pos::last());
  assert(0 <= number && number <= DIM);
  return numbers[pos][number];
}

/** Compute all possible numbers for this cell. */
void Puzzle::compute_numbers(Pos pos)
{
  assert(pos <= Pos::last());
  unsigned int i;

  // Fill list with all numbers.
  for (i = 1; i <= DIM; ++i)
    numbers[pos][i] = true;

  // Disable numbers found in row, column or region.
  for (Pos p = Pos::first(); p <= Pos::last(); p = p.next())
    if (p != pos &&
        (p.col() == pos.col() || p.row() == pos.row() || p.reg() == pos.reg()))
      numbers[pos][get(p)] = false;

  // Count the possible numbers.
  count[pos] = 0;
  for (i = 1; i <= DIM; ++i)
    if (numbers[pos][i])
      ++count[pos];

  // 0 is always possible.
  numbers[pos][0] = true;
}

/** Is the number in this cell a possible number? */
bool Puzzle::correct(Pos pos) const
{
  assert(pos <= Pos::last());
  return count[pos] != 0 && numbers[pos][get(pos)];
}
