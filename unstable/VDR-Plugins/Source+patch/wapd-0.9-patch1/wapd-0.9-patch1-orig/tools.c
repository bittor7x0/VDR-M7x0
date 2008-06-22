/*
 * server.c: wapd server core
 *
 * WAP remote control plugin for VDR (the Video Disk Recorder)
 * Copyright (C) 2002  Thomas Heiligenmann  <thomas@heiligenmann.de>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA
 *
 */


#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tools.h"


// --- tools -----------------------------------------------------------------

char *base64decode(char *s)
{
  char *p = s;
  char *q = s;
  int i = 0;
  while (p && *p && *p != '=') {
     char c = 63;
     if (isupper(*p))
        c = *p - 'A';
     else if (islower(*p))
        c = *p - 'a' + 26;
     else if (isdigit(*p))
        c = *p - '0' + 52;
     else if (*p == '+')
        c = 62;
     i %= 4;
     if (i == 0)
        *q = c << 2;
     else if (i == 1) {
        *q++ |= (c & 0x30) >> 4;
        *q = c << 4;
     } else if (i == 2) {
        *q++ |= (c & 0x3c) >> 2;
        *q = c << 6;
     } else if (i == 3)
        *q++ |= c;
     *p++;
     i++;
  }
  *q = 0;
  return s;
}

char *unurilize(char *s)
{
  char *p = s;
  while (p && *p) {
     if (*p == '%') {
        int c;
        sscanf(p, "%%%2x", &c);
        memmove(p, p + 2, strlen(p) + 1);
        *p = (char)c;
     } else if (*p == '+')
        *p = ' ';
     p++;
  }
  return s;
}

char *wmlescape(char *s)
{
  char *p = s;
  char s1[7];
  while (p && *p) {
     if (strchr("\"&<>", *p) != NULL || (unsigned char)*p > 127) {
        sprintf(s1, "&#%03d;", (unsigned char)*p);
        int l = strlen(s);
        int of = p - s;
        s = (char*)realloc(s, l + 7);
        p = s + of;
        memmove(p + 6, p + 1, l - of);
        strncpy(p, s1, 6);
        p += 6;
     } else if (*p == '\n') {
        int l = strlen(s);
        int of = p - s;
        int so = sizeof("<br/>\n");
        s = (char*)realloc(s, l + so);
        p = s + of;
        memmove(p + so - 1, p + 1, l - of);
        strncpy(p, "<br/>\n", so - 1);
        p += so - 1;
     } else
        p++;
  }
  return s;
}

