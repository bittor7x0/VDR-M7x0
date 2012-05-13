/*
 * config.h: 'Soppalusikka' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __SKINSOPPALUSIKKA_CONFIG_H
#define __SKINSOPPALUSIKKA_CONFIG_H

struct cSoppalusikkaConfig
{
private:
  char logodir[255];
public:
  cSoppalusikkaConfig();
  ~cSoppalusikkaConfig();
  void SetLogoDir(const char *logodirP);
  char *GetLogoDir(void) { return logodir; }
  int usesinglearea;
  int showauxinfo;
  int showlogo;
  int showvps;
  int showsymbols;
  int showprogressbar;
  int cachesize;
  int usechannelid;
};

extern cSoppalusikkaConfig SoppalusikkaConfig;

#endif // __SKINSOPPALUSIKKA_CONFIG_H
