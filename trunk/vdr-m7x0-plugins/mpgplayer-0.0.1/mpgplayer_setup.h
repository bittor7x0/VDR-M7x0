/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */

#ifndef __MPGPLAYER_SETUP_H
#define __MPGPLAYER_SETUP_H
#define MAXMARKERLEN 4
#define MPGPLAYER_SORTSERIESIN 0
#define MPGPLAYER_SORTSERIESFIRST 1
#define MPGPLAYER_SORTSERIESLAST 2
#include <string.h>
#include <stdlib.h>
class cMpgplayerSetup {
public:
  cMpgplayerSetup(void);
  ~cMpgplayerSetup();
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
extern cMpgplayerSetup MpgplayerSetup;
#endif
