/*
 * common.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_COMMON_H
#define __RSSREADER_COMMON_H

#ifdef DEBUG
#define debug(x...) dsyslog("RSSReader: " x);
#define error(x...) esyslog("RSSReader: " x);
#else
#define debug(x...) ;
#define error(x...) esyslog("RSSReader: " x);
#endif

// User agent string for servers
#define RSSREADER_USERAGENT "libcurl-agent/1.0"

#endif // __RSSREADER_COMMON_H

