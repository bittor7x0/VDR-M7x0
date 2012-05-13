#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "defines.h"
#include "screen.h"
#include "text.h"

#include "display.h"

namespace Dpy {

#define BORDERWIDTH   2
#define STATUSHEIGHT 32

static bool frame = false;

static int W = -1;
static int H = -1;

static void
frame_draw(void) {
	int i, x1, x2, y1, y2, y3, col = COL_RED;

	x1 = y1 = 0;
	x2 = W+BORDERWIDTH;
	y2 = BORDERWIDTH+STATUSHEIGHT;
	y3 = H+2*BORDERWIDTH+STATUSHEIGHT;
	for (int x=0; x<Scr::width(); x++) {
		for (i=0; i<BORDERWIDTH; i++) Scr::pixel(x, y1+i, col);
		for (i=0; i<BORDERWIDTH; i++) Scr::pixel(x, y2+i, col);
		for (i=0; i<BORDERWIDTH; i++) Scr::pixel(x, y3+i, col);
	}
	for (int y=0; y<Scr::height(); y++) {
		for (i=0; i<BORDERWIDTH; i++) Scr::pixel(x1+i, y, col);
		for (i=0; i<BORDERWIDTH; i++) Scr::pixel(x2+i, y, col);
	}
}

int 
open(int x, int y, int w, int h, bool fr) {
	frame = fr; W = w; H = h;
	if (frame) {
		w += 2*BORDERWIDTH;
		h += 3*BORDERWIDTH+STATUSHEIGHT;
	}
	if (!Scr::open(x, y, w, h)) {
		if (frame) frame_draw();
		return (0);
	}
	return (-1);
}

int
close(void) {
	return (Scr::close());
}

int
width(void) {
	return (W);
}

int
height(void) {
	return (H);
}

int
update(void) {
	return (Scr::update());
}

int
text_width(const char *txt) {
	return (Txt::width(txt));
}

int
text_width(char c) {
	return (Txt::width(c));
}

int
text_height(void) {
	return (Txt::height());
}

int
text_print(int x, int y, const char *txt, int fg, int bg) {
	if (frame) {
		x += BORDERWIDTH;
		y += 2*BORDERWIDTH+STATUSHEIGHT;
	}
	return (Txt::print(x, y, txt, fg, bg));
}

int
rect_fill(int x1, int y1, int x2, int y2, int col) {
	if (frame) {
		x1 += BORDERWIDTH;
		y1 += 2*BORDERWIDTH+STATUSHEIGHT;
		x2 += BORDERWIDTH;
		y2 += 2*BORDERWIDTH+STATUSHEIGHT;
	}
	for (int y=y1; y<y2; y++) {
		for (int x=x1; x<x2; x++) {
			if (Scr::pixel(x, y, col)) return (-1);
		}
	}
	return (0);
}

int
pixel_set(int x, int y, int col) {
	if (frame) {
		x += BORDERWIDTH;
		y += 2*BORDERWIDTH+STATUSHEIGHT;
	}
	return (Scr::pixel(x, y, col));
}

int
bitmap_blit(Bitmap *bmp, int x, int y) {
	if (frame) {
		x += BORDERWIDTH;
		y += 2*BORDERWIDTH+STATUSHEIGHT;
	}
	return (bmp->blit(x, y));
}

int
line_draw(int x1, int y1, int x2, int y2, int col) {
	float xdiff, ydiff, x, y;

	if (frame) {
		x1 += BORDERWIDTH;
		y1 += 2*BORDERWIDTH+STATUSHEIGHT;
		x2 += BORDERWIDTH;
		y2 += 2*BORDERWIDTH+STATUSHEIGHT;
	}
	xdiff = x2 - x1;
	ydiff = y2 - y1;
	if (xdiff>ydiff) {
		y = y1;
		for (x=x1; x<=x2; x++) {
			y += ydiff/xdiff;
			Scr::pixel((int)x, (int)y, col);
		}
	} else {
		x = x1;
		for (y=y1; y<=y2; y++) {
			x += xdiff/ydiff;
			Scr::pixel((int)x, (int)y, col);
		} 
	}
	return (0);
} 

int
status_left(const char *txt, int col) {
	static int x = 0, y = 0, w = 0, h = 0;

	for (int j=y; j<=y+h; j++) {
		for (int i=x; i<=x+w; i++) {
			Scr::pixel(i, j, 0);
		}
	}
	w=text_width(txt);
	h=text_height();
	if (frame) {
		x = 2*BORDERWIDTH;
		y = BORDERWIDTH+((STATUSHEIGHT-h)/2);
	} else {
		x = 2;
		y = 2;
	}
	if (w<W-2*BORDERWIDTH) {
		Txt::print(x, y, txt, col);
	}
	return (0);
}

int
status_center(const char *txt, int col) {
	static int x = 0, y = 0, w = 0, h = 0;

	for (int j=y; j<=y+h; j++) {
		for (int i=x; i<=x+w; i++) {
			Scr::pixel(i, j, 0);
		}
	}
	w=text_width(txt);
	h=text_height();
	if (frame) {
		x = (W-w)/2;
		y = BORDERWIDTH+((STATUSHEIGHT-h)/2);
	} else {
		x = (W-w)/2;
		y = 2;
	}
	if (w<W-2*BORDERWIDTH) {
		Txt::print(x, y, txt, col);
	}
	return (0);
}

int
status_right(const char *txt, int col) {
	static int x = 0, y = 0, w = 0, h = 0;

	for (int j=y; j<=y+h; j++) {
		for (int i=x; i<=x+w; i++) {
			Scr::pixel(i, j, 0);
		}
	}
	w=text_width(txt);
	h=text_height();
	if (frame) {
		x = W-w;
		y = BORDERWIDTH+((STATUSHEIGHT-h)/2);
	} else {
		x = W-w;
		y = 2;
	}
	if (w<W-2*BORDERWIDTH) {
		Txt::print(x, y, txt, col);
	}
	return (0);
}

int
status_left(int i, int col) {
	char txt[16];

	snprintf(txt, sizeof (txt), "%i", i);
	return (status_left(txt, col));
}

int
status_center(int i, int col) {
	char txt[16];

	snprintf(txt, sizeof (txt), "%i", i);
	return (status_center(txt, col));
}

int
status_right(int i, int col) {
	char txt[16];

	snprintf(txt, sizeof (txt), "%i", i);
	return (status_right(txt, col));
}

int
message_show(const char *txt1, const char *txt2, int fg, int bg) {
	int i, t, x, y, w = 0, h = 0;

	if (!txt1 && !txt2) return (-1);
	w = text_height();
	h = text_height()/2;
	if (txt1) h += text_height();
	if (txt2) h += text_height();
	if (txt1 && txt2) {
		if (text_width(txt1) > text_width(txt2)) {
			w += text_width(txt1);
		} else {
			w += text_width(txt2);
		}
	} else {
		if (txt1) w += text_width(txt1);
		if (txt2) w += text_width(txt2);
	}
	if ((x=(W-w)/2)<0) { x=0; w=W; }
	if ((y=(H-h)/3)<0) { y=0; h=H; }
	rect_fill(x, y, x+w, y+h, bg);
	for (i=0; i<2; i++) line_draw(x+i,   y,     x+i,   y+h,   fg);
	for (i=0; i<2; i++) line_draw(x,     y+h-i, x+w,   y+h-i, fg);
	for (i=0; i<2; i++) line_draw(x+w-i, y,     x+w-i, y+h,   fg);
	for (i=0; i<2; i++) line_draw(x,     y+i,   x+w,   y+i,   fg);
	y += text_height()/4;
	if (txt1) {
		t = x+(w-text_width(txt1))/2;
		text_print(t, y, txt1, fg, bg);
		y += text_height();
	}
	if (txt2) {
		t = x+(w-text_width(txt2))/2;
		text_print(t, y, txt2, fg, bg);
	}
	return (0);
}

} // namespace
