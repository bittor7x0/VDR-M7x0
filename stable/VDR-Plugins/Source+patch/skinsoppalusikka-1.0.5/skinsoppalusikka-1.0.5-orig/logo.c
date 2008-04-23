/*
 * logo.c: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "common.h"
#include "config.h"
#include "logo.h"
#include <vdr/tools.h>
#include <vdr/plugin.h>

cSoppalusikkaLogoCache SoppalusikkaLogoCache(0);

cSoppalusikkaLogoCache::cSoppalusikkaLogoCache(unsigned int cacheSizeP)
: cacheSizeM(cacheSizeP), bitmapM(NULL)
{
}

cSoppalusikkaLogoCache::~cSoppalusikkaLogoCache()
{
  // let's flush the cache
  Flush();
}

bool cSoppalusikkaLogoCache::Resize(unsigned int cacheSizeP)
{
  debug("cPluginSkinSoppalusikka::Resize(%d)", cacheSizeP);
  // flush cache only if it's smaller than before
  if (cacheSizeP < cacheSizeM) {
     Flush();
     }
  // resize current cache
  cacheSizeM = cacheSizeP;
  return true;
}

bool cSoppalusikkaLogoCache::Load(const char *fileNameP)
{
  debug("cPluginSkinSoppalusikka::Load(%s)", fileNameP);
  // does the logo exist already in map
  std::map<std::string, cBitmap*>::iterator i = cacheMapM.find(fileNameP);
  if (i != cacheMapM.end()) {
     // yes - cache hit!
     debug("cPluginSkinSoppalusikka::Load() CACHE HIT!");
     // check if logo really exist
     if (i->second == NULL) {
        debug("cPluginSkinSoppalusikka::Load() EMPTY");
        // empty logo in cache
        return false;
        }
     bitmapM = i->second;
     }
  else {
     // no - cache miss!
     debug("cPluginSkinSoppalusikka::Load() CACHE MISS!");
     // try to load xpm logo
     LoadXpm(fileNameP);
     // check if cache is active
     if (cacheSizeM) {
        // update map
        if (cacheMapM.size() >= cacheSizeM) {
           // cache full - remove first
           debug("cPluginSkinSoppalusikka::Load() DELETE");
           if (cacheMapM.begin()->second != NULL) {
              // logo exists - delete it
              cBitmap *bmp = cacheMapM.begin()->second;
              DELETENULL(bmp);
              }
           // erase item
           cacheMapM.erase(cacheMapM.begin());
           }
        // insert logo into map
        debug("cPluginSkinSoppalusikka::Load() INSERT(%s)", fileNameP);
        cacheMapM.insert(std::make_pair(fileNameP, bitmapM));
        }
     // check if logo really exist
     if (bitmapM == NULL) {
        debug("cPluginSkinSoppalusikka::Load() EMPTY");
        // empty logo in cache
        return false;
        }
     }
  return true;
}

cBitmap& cSoppalusikkaLogoCache::Get(void)
{
  return *bitmapM;
}

bool cSoppalusikkaLogoCache::LoadXpm(const char *fileNameP)
{
  struct stat stbuf;
  char *filename;
  cBitmap *bmp = new cBitmap(1, 1, 1);

  // create absolute filename
  asprintf(&filename, "%s/%s.xpm", SoppalusikkaConfig.GetLogoDir(), fileNameP);
  debug("cPluginSkinSoppalusikka::LoadXpm(%s)", filename);
  // check validity
  if ((stat(filename, &stbuf) == 0) && bmp->LoadXpm(filename) && (bmp->Width() == ChannelLogoWidth) && (bmp->Height() == ChannelLogoHeight)) {
     debug("cPluginSkinSoppalusikka::LoadXpm() LOGO FOUND");
     // assign bitmap
     bitmapM = bmp;
     free(filename);
     return true;
     }
  // no xpm logo found - delete bitmap
  debug("cPluginSkinSoppalusikka::LoadXpm() LOGO NOT FOUND");
  delete bmp;
  bitmapM = NULL;
  free(filename);
  return false;
}

bool cSoppalusikkaLogoCache::Flush(void)
{
  debug("cPluginSkinSoppalusikka::Flush()");
  // check if map is empty
  if (!cacheMapM.empty()) {
     debug("cPluginSkinSoppalusikka::Flush() NON-EMPTY");
     // delete bitmaps and clear map
     for (std::map<std::string, cBitmap*>::iterator i = cacheMapM.begin(); i != cacheMapM.end(); ++i) {
         cBitmap *bmp = i->second;
         if (bmp)
            DELETENULL(bmp);
         cacheMapM.erase(i);
         }
     // nullify bitmap pointer
     bitmapM = NULL;
     }
  return true;
}

