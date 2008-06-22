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
 * $Id: sudoku.h 106 2007-12-03 23:28:24Z tom $
 */

#ifndef VDR_SUDOKU_H
#define VDR_SUDOKU_H


#ifdef USE_RAND
#include <stdlib.h>
#include <time.h>
/** Random number 0 .. max-1 */
static unsigned int rand(unsigned int max)
{
  static unsigned int seed = time(NULL);
  return (unsigned int)((double)max * rand_r(&seed) / (RAND_MAX + 1.0));
}
#endif // USE_RAND

#endif // VDR_SUDOKU_H
