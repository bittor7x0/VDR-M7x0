/*
 * logo.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINENIGMA_LOGO_H
#define __SKINENIGMA_LOGO_H

#include "common.h"

#ifdef HAVE_IMAGEMAGICK
#include "bitmap.h"
#endif

#include <map>
#include <vdr/channels.h>
#include <vdr/epg.h>
#include <vdr/recording.h>
#include <vdr/osd.h>

// size of channel logos
#define ChannelLogoWidth  80
#define ChannelLogoHeight 80
// size of icons, e.g. icons top right in the main menu...
#define IconWidth         80
#define IconHeight        80
// size of symbols, e.g. the flags
#define SymbolWidth       27
#define SymbolHeight      18

class cEnigmaLogoCache {
private:
  bool fImageRequested;
  unsigned int cacheSizeM;
  cBitmap *bitmapM;
#ifdef HAVE_IMAGEMAGICK
  cOSDImageBitmap image;
  cBitmap *bmpImage;
#endif
  std::map<std::string, cBitmap*> cacheMapM;
  bool DrawImage(const char *fileNameP, int x, int y, int w, int h, int c, cBitmap *bmp);
  bool LoadXpm(const char *fileNameP, int w, int h, bool fLogNotFound = true);
  bool Load(const char *fileNameP, int w, int h, bool fLogNotFound = true);
public:
  cEnigmaLogoCache(unsigned int cacheSizeP);
  ~cEnigmaLogoCache();
  bool Resize(unsigned int cacheSizeP);
  bool DrawEventImage(const cEvent *Event, int x, int y, int w, int h, int c, cBitmap *bmp);
  bool DrawRecordingImage(const cRecording *Recording, int x, int y, int w, int h, int c, cBitmap *bmp);
  bool LoadChannelLogo(const cChannel *Channel);
  bool LoadSymbol(const char *fileNameP);
  bool LoadIcon(const char *fileNameP);
  cBitmap& Get(void);
  bool Flush(void);
};

extern cEnigmaLogoCache EnigmaLogoCache;

#endif // __SKINENIGMA_LOGO_H

// vim:et:sw=2:ts=2:
