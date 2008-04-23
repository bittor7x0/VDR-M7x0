/*
 * WAP remote control plugin for VDR
 * tools.h: wapd server tools
 *
 * See the README file for copyright information and how to reach the author.
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */


#ifndef __WAPD_TOOLS_H
#define __WAPD_TOOLS_H

char *base64decode(char *s);
char *unurilize(char *s);
char *wmlescape(char *s);

#endif //__WAPD_TOOLS_H
