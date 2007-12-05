#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef SDL
	#include <SDL.h>
#else
	#include <osd.h>
#endif

#include "defines.h"
#include "screen.h"

#define DEPTH  4
#define COLORS 1<<DEPTH

#ifdef SDL
	#define COLOR(R,G,B,A) (SDL_MapRGB(screen->format, R, G, B))
	#define PALETTE(I)     (colortable[I])
#else
#if VDRVERSNUM < 10307
	#define COLOR(R,G,B,A) (R+(G<<8)+(B<<16)+(A<<24))
	#define PALETTE(I)     ((eDvbColor)colortable[I])
#else
	#define COLOR(R,G,B,A) (B+(G<<8)+(R<<16)+(A<<24))
	#define PALETTE(I)     ((tColor)colortable[I])
#endif
#endif

namespace Scr {

#ifdef SDL
static SDL_Surface *screen = NULL;
#else
#if VDRVERSNUM < 10307
static cOsdBase    *screen = NULL;
#else
static cOsd        *screen = NULL;
#endif
#endif

static int W, H, colortable[COLORS], *shadow = NULL;

static int
palette(int pal) {
	switch (pal) {
		case PAL_VDR:
			colortable[0]  = COLOR(0x00, 0x00, 0x00, 0x7F); // Background
			colortable[1]  = COLOR(0xFC, 0xFC, 0xFC, 0xFF); // White
			colortable[2]  = COLOR(0xFC, 0x14, 0x14, 0xFF); // Red
			colortable[3]  = COLOR(0x00, 0xFC, 0xFC, 0xFF); // Cyan
			colortable[4]  = COLOR(0xB0, 0x00, 0xFC, 0xFF); // Magenta
			colortable[5]  = COLOR(0x24, 0xFC, 0x24, 0xFF); // Green
			colortable[6]  = COLOR(0x00, 0x00, 0xFC, 0xFF); // Blue
			colortable[7]  = COLOR(0xFC, 0xC0, 0x24, 0xFF); // Yellow
			colortable[8]  = COLOR(0xC0, 0xA0, 0x00, 0xFF); // Orange
			colortable[9]  = COLOR(0xFF, 0xA0, 0x00, 0xFF); // Orange2
			colortable[10] = COLOR(0xF0, 0x80, 0x80, 0xFF); // Pink
			colortable[11] = COLOR(0x00, 0xFF, 0xFF, 0xFF); // Cyan2
			colortable[12] = COLOR(0xFF, 0x00, 0xFF, 0xFF); // Purple2
			colortable[13] = COLOR(0x00, 0xFF, 0x00, 0xFF); // Green2
			colortable[14] = COLOR(0x00, 0xA0, 0xFF, 0xFF); // Blue2
			colortable[15] = COLOR(0x00, 0x00, 0x00, 0xFF); // Black
		break;
		case PAL_C64:
			colortable[0]  = COLOR(0x00, 0x00, 0x00, 0x7F); // Background
			colortable[1]  = COLOR(0xFF, 0xFF, 0xFF, 0xFF); // White
			colortable[2]  = COLOR(0xF0, 0x00, 0x00, 0xFF); // Red
			colortable[3]  = COLOR(0x00, 0xF0, 0xF0, 0xFF); // Cyan
			colortable[4]  = COLOR(0x60, 0x00, 0x60, 0xFF); // Purple
			colortable[5]  = COLOR(0x00, 0xA0, 0x00, 0xFF); // Green
			colortable[6]  = COLOR(0x00, 0x00, 0xF0, 0xFF); // Blue
			colortable[7]  = COLOR(0xD0, 0xD0, 0x00, 0xFF); // Yellow
			colortable[8]  = COLOR(0xC0, 0xA0, 0x00, 0xFF); // Orange
			colortable[9]  = COLOR(0xFF, 0xA0, 0x00, 0xFF); // Orange2
			colortable[10] = COLOR(0xF0, 0x80, 0x80, 0xFF); // Pink
			colortable[11] = COLOR(0x00, 0xFF, 0xFF, 0xFF); // Cyan2
			colortable[12] = COLOR(0xFF, 0x00, 0xFF, 0xFF); // Purple2
			colortable[13] = COLOR(0x00, 0xFF, 0x00, 0xFF); // Green2
			colortable[14] = COLOR(0x00, 0xA0, 0xFF, 0xFF); // Blue2
			colortable[15] = COLOR(0x00, 0x00, 0x00, 0xFF); // Black
		break;
	}
#ifndef SDL
	for (int i=0; i<COLORS; i++) {
#if VDRVERSNUM < 10307
		if (screen) screen->AddColor(PALETTE(i));
#else
		if (screen) {
			screen->GetBitmap(0)->SetColor(i, PALETTE(i));
		}
#endif
	}
#endif
	return (0);
}

int
pixel(int x, int y, int idx) {
	if (!screen || x<0 || x>=W || y<0 || y>=H || idx<0 || idx>=COLORS) {
		return (-1);
	}
	if (shadow) shadow[W*y+x] = idx;
#ifdef SDL
	unsigned int c, bpp;
	unsigned char *p;

	if (SDL_MUSTLOCK(screen)) if (SDL_LockSurface(screen)) return (-1);
	bpp = screen->format->BytesPerPixel;
	p = (unsigned char *)screen->pixels + y * screen->pitch + x * bpp;
	c = PALETTE(idx);
	switch (bpp) {
		case 1:
			*p = c;
		break;
		case 2:
			*(unsigned short *)p = c;
		break;
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (c >>  16) & 0xff;
				p[1] = (c >>   8) & 0xff;
				p[2] = (c & 0xff);
			} else {
				p[0] = (c & 0xff);
				p[1] = (c >>   8) & 0xff;
				p[2] = (c >>  16) & 0xff;
			}
		break;
		case 4:
			*(unsigned int *)p = c;
		break;
	}
	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
#else
#if VDRVERSNUM < 10307
	screen->Fill(x, y, x, y, PALETTE(idx));
#else
	screen->DrawPixel(x, y, PALETTE(idx));
#endif
#endif
	return (0);
}

int 
open(int x, int y, int w, int h) {
	if (screen) return (-1);
	if (x<0 || x>=800 || y<0 || y>=600) return (-1);
	if (w<=0 || w>800 || h<=0 || h>600) return (-1);
	W = w; H = h;
#ifdef SDL
	unsigned int flg;

	flg = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;
	if (SDL_Init(flg)) return (-1);
	flg = SDL_SWSURFACE | SDL_ANYFORMAT;
	if (!(screen = SDL_SetVideoMode(W, H, 0, flg))) return (-1);
	SDL_ShowCursor(SDL_DISABLE);
	palette(PAL_VDR);
#else
#if VDRVERSNUM < 10307
	tWindowHandle win;

	if (!(screen = cOsd::OpenRaw(x, y))) {
		if (!(screen = cOsd::OpenRaw(x, y))) {
			return (-1);
		}
	}
	win = screen->Create(0, 0, w, h, DEPTH);
	palette(PAL_VDR);
	screen->Show(win);
	screen->Flush();
#else
	tArea win = { 0, 0, w-1, h-1, DEPTH };

	if (!(screen = cOsdProvider::NewOsd(x, y))) {
		if (!(screen = cOsdProvider::NewOsd(x, y))) {
			return(-1);
		}
	}
	screen->SetAreas(&win, 1);
	palette(PAL_VDR);
	screen->Flush();
#endif
#endif
	if (!(shadow = (int *)calloc(W*H, sizeof (int)))) {
		return (-1);
	}
	for (int y=0; y<H; y++) {
		for (int x=0; x<W; x++) pixel(x, y, 0);
	}
	return (0);
}

int
close(void) {
	if (screen) {
		free(shadow);
#ifdef SDL
		SDL_Quit();
#else
		delete (screen);
#endif
		screen = NULL;
		shadow = NULL;
		return (0);
	}
	return (-1);
}

int
update(void) {
	if (screen) {
#ifdef SDL
		SDL_Flip(screen);
#else
		screen->Flush();
#endif
		return (0);
	}
	return (-1);
}

int
pixel(int x, int y) {
	if (shadow) return (shadow[W*y+x]);
	return (-1);
}

int
width(void) {
	if (screen) return (W);
	return (-1);
}

int
height(void) {
	if (screen) return (H);
	return (-1);
}

} // namespace
