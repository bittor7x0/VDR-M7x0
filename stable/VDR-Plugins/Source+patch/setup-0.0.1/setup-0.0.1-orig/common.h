/*
 * common.h: The 'setup' plugin
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef SETUP_COMMON_H
#define SETUP_COMMON_H

#include <vdr/tools.h>

#ifdef DEBUG
#define debug(x...) dsyslog("open 7x0 Setup: " x);
#define error(x...) esyslog("open 7x0 Setup: " x);
#else
#define debug(x...) ;
#define error(x...) esyslog("open 7x0 Setup: " x);
#endif

#endif // __SKINSOPPALUSIKKA_COMMON_H
