/*
 * videodir.h: Functions to maintain a distributed video directory
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __VIDEODIR_H
#define __VIDEODIR_H

#include <stdlib.h>
#include "tools.h"

extern const char *VideoDirectory;

cUnbufferedFile *OpenVideoFile(const char *FileName, int Flags);
int CloseVideoFile(cUnbufferedFile *File);
bool RenameVideoFile(const char *OldName, const char *NewName);
bool RemoveVideoFile(const char *FileName);
bool VideoFileSpaceAvailable(int SizeMB);
int VideoDiskSpace(int *FreeMB = NULL, int *UsedMB = NULL); // returns the used disk space in percent
cString PrefixVideoFileName(const char *FileName, char Prefix);
void RemoveEmptyVideoDirectories(void);

#endif //__VIDEODIR_H
