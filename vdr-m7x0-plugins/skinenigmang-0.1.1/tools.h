/*
 * tools.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINENIGMA_TOOLS_H
#define __SKINENIGMA_TOOLS_H

#include "common.h"

std::string parseaux(const char *str);
bool ischaracters(const char *aux, const char *mask);

#if APIVERSNUM < 10505
cString WeekDayNameFull(int WeekDay);
cString WeekDayNameFull(time_t t);
#endif

#endif // __SKINENIGMA_TOOLS_H
// vim:et:sw=2:ts=2:
