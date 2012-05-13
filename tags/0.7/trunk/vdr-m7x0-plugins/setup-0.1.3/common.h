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
#include <vdr/plugin.h>

#define STR_YES "YES"
#define STR_NO "NO"



#ifdef DEBUG
#define debug(x...) dsyslog("setup: " x);
#define info(x...) isyslog("setup: " x);
#define error(x...) esyslog("setup: " x);
#else
#define debug(x...) dsyslog("setup: " x);
#define info(x...) isyslog("setup: " x);
#define error(x...) esyslog("setup: " x);
#endif



#endif // __SKINSOPPALUSIKKA_COMMON_H
