/*
 * common.h: XVDR plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __XVDR_COMMON_H
#define __XVDR_COMMON_H

#ifdef DEBUG
#define debug(x...) dsyslog("XVDR: " x);
#define error(x...) esyslog("XVDR: " x);
#else
#define debug(x...) ;
#define error(x...) esyslog("XVDR: " x);
#endif

// User agent string for servers
#define RSSREADER_USERAGENT "libcurl-agent/1.0"

#endif // __XVDR_COMMON_H

