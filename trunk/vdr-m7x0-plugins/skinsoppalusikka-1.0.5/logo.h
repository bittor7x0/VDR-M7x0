/*
 * logo.h: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __SKINSOPPALUSIKKA_LOGO_H
#define __SKINSOPPALUSIKKA_LOGO_H

#include <map>
#include <string>
#include <vdr/osd.h>

#define ChannelLogoWidth  64
#define ChannelLogoHeight 48

class cSoppalusikkaLogoCache {
private:
  unsigned int cacheSizeM;
  cBitmap *bitmapM;
  std::map<std::string, cBitmap*> cacheMapM;
  bool LoadXpm(const char *fileNameP);
public:
  cSoppalusikkaLogoCache(unsigned int cacheSizeP);
  ~cSoppalusikkaLogoCache();
  bool Resize(unsigned int cacheSizeP);
  bool Load(const char *fileNameP);
  cBitmap& Get(void);
  bool Flush(void);
};

extern cSoppalusikkaLogoCache SoppalusikkaLogoCache;

#endif // __SKINSOPPALUSIKKA_LOGO_H
