/*
 *
 * See the README file for copyright information and how to reach the author.
 *
 * 
 */

#include "mpgplayer_setup.h"
#include <string.h>
#include <stdlib.h>
#define MPG_DEFAULT_PATH_BASE "/var/media"
// --- cMpgplayerSetup ------------------------------------------------------
cMpgplayerSetup::cMpgplayerSetup(void)
{
  ShowHidden = false;
  RecordingPreGap = 5 * 60;
  RecordingPostGap = 5 * 60;
  BasePath = strdup(MPG_DEFAULT_PATH_BASE);
  LastPath = strdup(MPG_DEFAULT_PATH_BASE);
  strcpy(DirMarkerPre,"[");
  strcpy(DirMarkerPost,"]");
  SortOrder = MPGPLAYER_SORTSERIESIN;
}
cMpgplayerSetup::~cMpgplayerSetup(void)
{
  free(BasePath);
  free(LastPath);
}

cMpgplayerSetup MpgplayerSetup;
