/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: puzzle.cpp 16 2005-10-31 21:12:41Z tom $
 */

#include "puzzle.h"
#include "generator.h"
#include <assert.h>

using namespace Sudoku;


//--- class Sudoku::Numbers ----------------------------------------------------

/** Constructor */
Numbers::Numbers()
{
  reset();
}

/** Destructor */
Numbers::~Numbers()
{
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
  assert (pos <= Pos::last());
  assert (0 <= number && number <= DIM);
  content[pos] = number;
}

/** Get the number from this cell. */
unsigned int Numbers::get(Pos pos) const
{
  assert (pos <= Pos::last());
  assert (0 <= content[pos] && content[pos] <= DIM);
  return content[pos];
}


//--- class Sudoku::Puzzle -----------------------------------------------------

/** Constructor */
Puzzle::Puzzle(unsigned int givens_count, bool symmetric)
{
  if (givens_count == 0)
    clear_givens();
  else
    generate(givens_count, symmetric);
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
    for (i = 0; i < SDIM; ++i)
      marks[i] = false;
}

/** Set the number into this cell. */
void Puzzle::set(Pos pos, unsigned int number)
{
  assert (pos <= Pos::last());
  assert (0 <= number && number <= DIM);

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
  unsigned int i;

  for (i = 0; i < SDIM; ++i)
    if (get(i) != givens.get(i))
      return false;

  return true;
}

/** Is the number in this cell given? */
bool Puzzle::given(Pos pos) const
{
  assert (pos <= Pos::last());
  return givens.get(pos) != 0;
}

/** Is there an error on this position? */
bool Puzzle::error(Pos pos) const
{
  assert (pos <= Pos::last());
  return !correct(pos);
}

/** Is the number in this cell ambiguous? */
bool Puzzle::ambiguous(Pos pos) const
{
  assert (pos <= Pos::last());
  return get(pos) != 0 && count[pos] > 1;
}

/** All cells set and no errors? */
bool Puzzle::solved() const
{
  unsigned int i;

  for (i = 0; i < SDIM; ++i)
    if (get(i) == 0 || !correct(i))
      return false;

  return true;
}

/** Is this cell marked? */
bool Puzzle::marked(Pos pos) const
{
  assert (pos <= Pos::last());
  return marks[pos];
}

/** Toggle the mark for this cell. */
void Puzzle::toggle_mark(Pos pos)
{
  assert (pos < Pos::last());
  marks[pos] = !marks[pos];
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
  assert (pos <= Pos::last());
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
  assert (pos <= Pos::last());
  return count[pos];
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
