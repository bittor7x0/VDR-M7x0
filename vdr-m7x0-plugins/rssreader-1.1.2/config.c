/*
 * config.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "tools.h"
#include "config.h"
#include "common.h"

cRssReaderConfig RssConfig;

cRssReaderConfig::cRssReaderConfig(void)
: hidemenu(0), hideelem(0), useproxy(0)
{
  strn0cpy(httpproxy, "127.0.0.1:8000", sizeof(httpproxy));
}
