/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_setup.h 13 2007-03-06 07:09:16Z andreas $
 *
 */

#ifndef __CRIDPLAYER_SETUP_H
#define __CRIDPLAYER_SETUP_H
#define MAXMARKERLEN 4
#define CRIDPLAYER_SORTSERIESIN 0
#define CRIDPLAYER_SORTSERIESFIRST 1
#define CRIDPLAYER_SORTSERIESLAST 2
#include <string.h>
#include <stdlib.h>
class cCridplayerSetup {
public:
  cCridplayerSetup(void);
  ~cCridplayerSetup();
  bool ShowHidden;
  int RecordingPreGap;
  int RecordingPostGap;
  char *BasePath;
  char *LastPath;
  char DirMarkerPre[MAXMARKERLEN + 1];
  char DirMarkerPost[MAXMARKERLEN + 1];
  int SortOrder;
  void SetBasePath(const char *Path) { free(BasePath); BasePath = strdup(Path); }
  void SetLastPath(const char *Path) { free(LastPath); LastPath = strdup(Path); }
  };
extern cCridplayerSetup CridplayerSetup;
#endif
