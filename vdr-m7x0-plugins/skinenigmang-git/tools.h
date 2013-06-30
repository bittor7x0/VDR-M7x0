/*
 * tools.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINENIGMA_TOOLS_H
#define __SKINENIGMA_TOOLS_H

#include "common.h"

#define EVENT_CATEGORY                 "Category: "
#define EVENT_GENRE                    "Genre: "

std::string parseaux(const char *str);
bool ischaracters(const char *aux, const char *mask);
std::string ExtractAttribute(const char* evDescription, const char* name);

#endif // __SKINENIGMA_TOOLS_H
// vim:et:sw=2:ts=2:
