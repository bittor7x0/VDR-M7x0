/*
 * font.c: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Taken from GraphTFT 
 */

#include "common.h"

#include "font.h"
#include <iconv.h>
#include <stdio.h>

cGraphtftFont::cGraphtftFont()
{
}

cGraphtftFont::~cGraphtftFont()
{
	Clear();
}

const cFont* cGraphtftFont::GetFont(const char *Filename, int Size, int Width)
{
  if (Filename == NULL)
    return NULL;

	if (Size < MINFONTSIZE)
		Size = MINFONTSIZE;
	if (Width == 0)
		Width = 100;

  char *cachename = NULL;
  if (-1 != asprintf(&cachename, "%s_%d_%d", Filename, Size, Width)) {
    string CacheName = cachename;
    free(cachename);
    cachename = NULL;

    if (_cache.find(CacheName) != _cache.end())
      return _cache[CacheName];

    debug("cGraphtftFont::Load() CREATING FONT %s size=%d width=%d", Filename, Size, Width);
    cFont *newFont = cFont::CreateFont(Filename, Size, Width > 0 ? (Size * Width / 100) : 0);
    if (newFont) {
      _cache[CacheName] = newFont;
      return newFont;
    }
  }

  error("ERROR: EnigmaNG: Couldn't load font %s:%d", Filename, Size);

  return NULL;
}

void cGraphtftFont::Clear()
{
	cache_map::iterator it = _cache.begin();
	for (; it != _cache.end(); ++it)
		delete((*it).second);
	_cache.clear();
}
