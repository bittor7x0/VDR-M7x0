#ifndef __TXTFONT_H
#define __TXTFONT_H

#include "txtrender.h"

unsigned int* GetFontChar(cTeletextChar c, unsigned int *buffer);
// Get a character bitmap for character/charset
// Also handle double width/height partial characters
// buffer must be an unsigned int[10] buffer, that *may* be used
// to store the character - a different pointer may be returned too.
// returns NULL if undefined character


#endif
