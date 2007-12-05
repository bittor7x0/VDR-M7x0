/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __SHOWRECINFOCFG_H
#define __SHOWRECINFOCFG_H

#include <stdio.h>
#include <string.h>


struct cShowRecInfoConfig
{
public:
cShowRecInfoConfig(void);
    int hidemenu;
};

extern cShowRecInfoConfig ShowRecInfoConfig;

#endif // __SHOWRECINFOCFG_H
