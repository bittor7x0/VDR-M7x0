/*
 * tools.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_TOOLS_H
#define __RSSREADER_TOOLS_H

#include <vdr/tools.h>
#include <string>

#define DEFAULT_ENCONDING "ISO-8859-1"

using namespace std;

void striphtml(string &str);


#endif // __RSSREADER_TOOLS_H

