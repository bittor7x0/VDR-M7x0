/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_setup.c 13 2007-03-06 07:09:16Z andreas $
 *
 */

#include "cridplayer_setup.h"
#include <string.h>
#include <stdlib.h>
#define CRID_DEFAULT_PATH_BASE "/var/media"
// --- cCridplayerSetup ------------------------------------------------------
cCridplayerSetup::cCridplayerSetup(void)
{
  ShowHidden = false;
  RecordingPreGap = 5 * 60;
  RecordingPostGap = 5 * 60;
  BasePath = strdup(CRID_DEFAULT_PATH_BASE);
  LastPath = strdup(CRID_DEFAULT_PATH_BASE);
  strcpy(DirMarkerPre,"[");
  strcpy(DirMarkerPost,"]");
  SortOrder = CRIDPLAYER_SORTSERIESIN;
}
cCridplayerSetup::~cCridplayerSetup(void)
{
  free(BasePath);
  free(LastPath);
}

cCridplayerSetup CridplayerSetup;
