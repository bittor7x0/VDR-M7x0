/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: sudoku.h 11 2005-10-28 01:00:01Z tom $
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
