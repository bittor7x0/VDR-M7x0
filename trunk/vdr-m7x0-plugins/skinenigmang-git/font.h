/*
 * font.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Taken from GraphTFT 
 */

#ifndef VDR__GRAPHTFTFONT_H
#define VDR__GRAPHTFTFONT_H

#include "common.h"

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <vdr/font.h>
#include <vdr/config.h>

#ifndef MINFONTSIZE
#define MINFONTSIZE 10
#endif

using std::map;
using std::string;


class cGraphtftFont
{
private:
	typedef map<string,cFont*> cache_map;
	
public:
	cGraphtftFont();
	~cGraphtftFont();

  const cFont* GetFont(const char *Filename, int Size, int Width = 0);
	void Clear();

private:
	cache_map 		_cache;

};
#endif // VDR__GRAPHTFTFONT_H
