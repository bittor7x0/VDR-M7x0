/*
 * font.c: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * Taken from GraphTFT 
 */

#include "common.h"

#if VDRVERSNUM != 10503
#include "font.h"
#include <iconv.h>
#include <stdio.h>

cGraphtftFont::cGraphtftFont()
{
#if VDRVERSNUM < 10503
	_library = 0;
	_face = 0;

	// init freetype2 lib
	int error = FT_Init_FreeType(&_library);
	if (error)
	{
		error("ERROR: Could not init freetype library");	
	}
#endif
}

cGraphtftFont::~cGraphtftFont()
{
	Clear();

#if VDRVERSNUM < 10503
	if (_face)
	{
		FT_Done_Face(_face);
	}

	if (_library)
	{
		FT_Done_FreeType(_library);
	}
#endif
}

bool cGraphtftFont::Load(string Filename, string CacheName, int Size, int Language, int Width, int format)
{
	if ( _cache.find(CacheName) != _cache.end() )
		return true;	
	
#if VDRVERSNUM >= 10504
	//TODO: "Width" not yet in VDR
# ifdef VDRVERSNUM >= 10505
	cFont *newFont = cFont::CreateFont(Filename.c_str(), Size, Width > 0 ? (Size * Width / 100) : 0);
# else
	cFont *newFont = cFont::CreateFont(Filename.c_str(), Size);
# endif
	if (newFont) {
		_cache[CacheName] = newFont;
		return true;
	}
#elif VDRVERSNUM < 10503
	int error = FT_New_Face(_library, Filename.c_str(), format, &_face);

	// every thing ok?
	if (error == FT_Err_Unknown_File_Format)
	{
		error("ERROR: Font file (%s) could be opened and read, but it appears that its font format is unsupported", Filename.c_str());
		return false;
	}
	else if (error)
	{
		error("ERROR: Font file (%s) could be opened or read, or simply it is broken", Filename.c_str());
		return false;
	}

	// set slot
	_slot = _face->glyph;

	if (Width > 0)
		Width = Size * Width / 100;

	// set Size
	FT_Set_Char_Size
	(
		_face,          // handle to face object
		Width*64,       // char_width in 1/64th of points
		Size*64,	// char_height in 1/64th of points
		0,              // horizontal device resolution (dpi)
		0               // vertical device resolution (dpi)
	);

	iconv_t cd;
	char from_code[255];
	wchar_t	 	utf_buff[256];

	// XXX: Get this values from i18n
	switch(Language) {
	case 11:
		strcpy(from_code,"ISO8859-7");
		break;
	case 13:
	case 17:
		strcpy(from_code,"ISO8859-2");
		break;
	case 16:
		strcpy(from_code,"ISO8859-5");
		break;
	default :
		strcpy(from_code,"ISO8859-15");
		break;
	}

	if ((cd = iconv_open("WCHAR_T",from_code)) == (iconv_t)-1) {
			esyslog("ERROR: Iconv encoding not supported: %m");
	        return false; //encoding no supportet
	}

	for (int c = 0; c < 256; c++) {
	
		char char_buff = c;
		wchar_t wchar_buff;

		char *in_buff,*out_buff;
		size_t in_len, out_len, count;

		in_len=1; out_len=4;
		in_buff=(char*)&char_buff;
		out_buff=(char *)&wchar_buff;
		count = iconv(cd,&in_buff,&in_len,&out_buff,&out_len);
		if ( (size_t)-1 == count ){
			//printf("ERROR - PREPARING TABLE CHAR %d \n", c);
			utf_buff[c] = 0;	
		}
		utf_buff[c] = wchar_buff;
	}
	iconv_close(cd);

	cFont::tPixelData value;
	int num_rows_global = (_face->size->metrics.height / 64 )+1;
	int num_rows = num_rows_global + 2;
	
	cFont::tPixelData * font_data = new cFont::tPixelData[225 * num_rows];
	
	for (int i = 0; i < 225; i++)
		for (int j = 0; j < num_rows; j++)
			font_data[(i*num_rows)+j]=0x0000000000000000;
	
	// Time to put char 32..255 in font_data
	FT_UInt glyph_index;	
	for ( int num_char = 32, num_char_array = 0; num_char < 256; num_char++, num_char_array++ )
	{

		 //Get FT char index 
		 glyph_index = FT_Get_Char_Index( _face, utf_buff[num_char] );
		
		 //Load the char
		 error = FT_Load_Glyph( _face, glyph_index, FT_LOAD_DEFAULT ); 
		 if ( error ) continue; /* ignore errors */  
		
		// now, convert to vdr font data
		int width = (_slot->metrics.horiAdvance / 64) + 1;
		int bearingX = (_slot->metrics.horiBearingX / 64) +1;
		width = (width > (int)sizeof(cFont::tPixelData) * 8) ? (((int)sizeof(cFont::tPixelData) * 8)-2) :width ;

		font_data[(num_char_array*num_rows)+0]=width;
		font_data[(num_char_array*num_rows)+1]=num_rows_global;

		// convert to a mono bitmap
		error = FT_Render_Glyph( _face->glyph, ft_render_mode_mono );
		if ( error ) continue;

		int  top = _slot->bitmap_top;
		int  y_off = Size - top;

		unsigned char *bmp = _slot->bitmap.buffer;
		
      		for (int y = 0; y < _slot->bitmap.rows; ++y , y_off++) {
	        	int pos = 0;
			int bit = 0x80;
			value = 0x00;

			for (int x = 0; x < _slot->bitmap.width; ++x) {
				if (bmp[pos] & bit && x < width + 1 )
					value = value | (cFont::tPixelData)1 << width - bearingX - x ;
				
				bit >>= 1;
				if (bit == 0) {
					bit = 0x80;
					++pos;
				}
			}
			bmp += _slot->bitmap.pitch;
			font_data[(num_char_array*num_rows)+y_off+2] = value;
		}
		
	}
	// If all was ok, put the vdr font into the cache and return true
	cFont* newFont = NULL;
	newFont = new cFont(font_data);
	if (newFont) {
		_cache[CacheName] = newFont;
		_del[CacheName] = font_data;
		return true;
	}
	delete(font_data);
#endif //VDRVERSNUM >= 10504

	// Something went wrong!
	return false;
}

const cFont* cGraphtftFont::GetFont(string CacheName){
	if (CacheName == "Sml") return cFont::GetFont(fontSml);
	else if (CacheName == "Fix") return cFont::GetFont(fontFix);
	else if ( _cache.find(CacheName) != _cache.end() ){
		return _cache[CacheName];
	}
	return cFont::GetFont(fontOsd);
}

void cGraphtftFont::Clear(string CacheName)
{
	Clear();	
}

void cGraphtftFont::Clear()
{
	cache_map::iterator it = _cache.begin();
	for (; it != _cache.end(); ++it)
		delete((*it).second);
	_cache.clear();

#if VDRVERSNUM < 10503
	del_map::iterator del_it = _del.begin();
	for (; del_it != _del.end(); ++del_it)
		delete[]((*del_it).second);
	_del.clear();
#endif
}
#endif //VDRVERSNUM < 10503
