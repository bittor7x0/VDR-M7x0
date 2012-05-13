/*
 * tools.h: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __SKINSOPPALUSIKKA_TOOLS_H
#define __SKINSOPPALUSIKKA_TOOLS_H

#include <vdr/tools.h>

char *strcatrealloc(char *dest, const char *src);
char *strncatrealloc(char *dest, const char *src, size_t len);
char *parseaux(char *str);
bool ischaracters(const char *str, const char *mask);
bool ischaracter(const char ch, const char *mask);

#endif // __SKINSOPPALUSIKKA_TOOLS_H
