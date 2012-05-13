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
 * $Id: backtrack.cpp 106 2007-12-03 23:28:24Z tom $
 */

#include "backtrack.h"

using namespace BackTrack;


//--- class BackTrack::Algorithm -----------------------------------------------

/** Constructor
 *
 * Constructs an backtracking algorithm to solve a problem. The problem is
 * implemented in 'solution' which represents a path through the decision
 * tree from the root to one leaf.
 */
Algorithm::Algorithm (Solution& solution, unsigned int max_iter) :
  solution(solution), max_iter(max_iter)
{
  first = true;
  valid = false;
  level = -1;
  iter = 0;
}

/** Find the next valid solution to the problem.
 *
 * Repeated calls will find all solutions to a problem if multiple solutions
 * exist.
 */
void Algorithm::find_next_solution()
{
  valid = find_solution();
}

/** Is the current solution a valid solution? */
bool Algorithm::solution_is_valid()
{
  return valid;
}

/** Reset the decision tree, i.e. the next call to 'find_solution' finds
 * the first valid solution.
 */
void Algorithm::reset()
{
  while (level >= 0)
  {
    solution.reset_at(level);
    --level;
  }
  first = true;
}

/** Create the next leaf on the end of the solution. */
void Algorithm::create_left_leaf()
{
  ++level;
  solution.set_first_at(level);
}

/** Backtrack through the decision tree until a node was found that hasn't
 * been visited, return true if an unvisited node was found.
 */
bool Algorithm::visit_new_node()
{
  // If the current node is the rightmost child we must backtrack
  // one level because there are no more children at this level.
  // So we back up until we find a non-rightmost child, then
  // generate the child to the right. If we back up to the top
  // without finding an unvisted child, then all nodes have been
  // generated.
  while (level >= 0 && solution.is_last_at(level))
  {
    solution.reset_at(level);
    --level;
  }
  if (level < 0)
    return false;
  solution.set_next_at(level);
  return true;
}

/** Find the next valid sibling of the last leaf, return true if a valid
 * sibling was found.
 */
bool Algorithm::find_valid_sibling()
{
  // If the current node is not valid pass through all siblings until either
  // a valid sibling is found or the last sibling is reached.
  for (;;)
  {
    ++iter;
    if (max_iter != 0 && iter > max_iter)
      return false;
    if (solution.is_valid_at(level))
      return true;
    if (solution.is_last_at(level))
      return false;
    solution.set_next_at(level);
  }
}

/** Find the next valid solution to the problem, return true if a solution
 * was found.
 */
bool Algorithm::find_solution()
{
  // If first time, need to create a root.
  if (first)
  {
    first = false;
    level = -1;
    if (solution.is_last_level(level))
      return solution.is_valid_at(level);
    create_left_leaf();
  }
  // Otherwise visit new node since solution contains the last solution.
  else if (!visit_new_node())
    return false;

  for (;;)
  {
    if (find_valid_sibling())
    {
      if (solution.is_last_level(level))
        return true;
      create_left_leaf();
    }
    else if (max_iter != 0 && iter > max_iter)
      return false;
    else if (!visit_new_node())
      return false; // The tree has been exhausted, so no solution exists.
  }
}
