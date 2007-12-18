
/*
 * config.c: 'Soppalusikka' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <stdlib.h>
#include "common.h"
#include "config.h"

cSoppalusikkaConfig SoppalusikkaConfig;

cSoppalusikkaConfig::cSoppalusikkaConfig()
: usesinglearea(0), showauxinfo(1), showlogo(0), showvps(1),
  showsymbols(0), showprogressbar(1), cachesize(50), usechannelid(0)
{
  memset(logodir, 0, sizeof(logodir));
}

cSoppalusikkaConfig::~cSoppalusikkaConfig()
{
}

void cSoppalusikkaConfig::SetLogoDir(const char *logodirP)
{
  debug("cSoppalusikkaConfig::SetLogoDir(%s)", logodirP);
  strncpy(logodir, logodirP, sizeof(logodir));
}
