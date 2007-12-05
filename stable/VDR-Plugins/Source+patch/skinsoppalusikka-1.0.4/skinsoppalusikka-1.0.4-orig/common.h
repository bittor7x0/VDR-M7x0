/*
 * common.h: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __SKINSOPPALUSIKKA_COMMON_H
#define __SKINSOPPALUSIKKA_COMMON_H

#include <vdr/tools.h>

#ifdef DEBUG
#define debug(x...) dsyslog("Soppalusikka: " x);
#define error(x...) esyslog("Soppalusikka: " x);
#else
#define debug(x...) ;
#define error(x...) esyslog("Soppalusikka: " x);
#endif

#endif // __SKINSOPPALUSIKKA_COMMON_H
