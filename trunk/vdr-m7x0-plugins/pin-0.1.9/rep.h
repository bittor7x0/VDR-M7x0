/*
 * pin.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Date: 28.01.2006, horchi
 */

//***************************************************************************

#ifndef __REP_H__
#define __REP_H__

//***************************************************************************

enum Option
{
  repUseRegularExpression = 1,
  repIgnoreCase = 2
};

int rep(const char* string, const char* expression, Option options = repUseRegularExpression);

int rep(const char* string, const char* expression, 
        const char*& s_location, Option options = repUseRegularExpression);

int rep(const char* string, const char* expression, const char*& s_location, 
        const char*& e_location, Option options = repUseRegularExpression);

//***************************************************************************
#endif
