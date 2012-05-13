/*****************************************************************************\
*                                                                             *
* Programm:    Tetris                                                         *
* License:     GPL (General Puplic License)                                   *
* Last Change: 2003-05-04                                                     *
* Authors:     Franko Kulaga <franko@kulaga.net>                              *
*              Clemens Kirchgatterer <clemens@thf.ath.cx>                     *
*                                                                             *
\*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../display.h"
#include "../defines.h"

#include "tetris.h"

#define COLORS 14
#define WIDTH  10
#define HEIGHT 20

static unsigned char field[HEIGHT][WIDTH];
static unsigned char piece[4][2];

static bool running, stopped;

static int key = KEY_NONE;

static int score  = 0;
static int level  = 0;
static int lines  = 0;
static int pieces = 0;
static int halt   = 0;
static int fallen = 0;
static int over   = 0;
static int color  = 0;
static int size   = 0;

static void
draw_block(int x, int y, int c) {
	int x1, y1, x2, y2;

	x1 = x*size;
	y1 = y*size;
	x2 = x*size+size;
	y2 = y*size+size;
	Dpy::rect_fill(x1, y1, x2, y2, c);
}

static void
draw_score(void) {
	Dpy::status_left(score, COL_WHT);
	Dpy::status_right(level, COL_WHT);
}

static void
game_over(void) {
	over = 1;
	Dpy::message_show("Game", "Over", COL_WHT);
	Dpy::update();
}

static void
init_field(void) {
	for (int y=0; y<HEIGHT; y++) {
		for (int x=0; x<WIDTH; x++) {
			field[y][x] = 0;
		}
	}
}

static void
draw_field(int line=-1) {
	int lines = HEIGHT;

	if (line>=0) lines = line;
	for (int y=0; y<lines; y++) {
		for (int x=0; x<WIDTH; x++) {
			draw_block(x, y, field[y][x]);
		}
	}
}

static void
draw_piece(void) {
	for (int i = 0; i < 4; i++) {
		draw_block(piece[i][0], piece[i][1], color);
	}
}

static void
init_piece(void) {
	int j, i = 0;

	j = RAND(6);
	color = 2+RAND(COLORS-2);
	for (int x=0; x<4; x++) {
		for (int y=0; y<4; y++) {
			if (shapes[j][x][y]) {
				piece[i][0] = x+4;
				piece[i][1] = y;
				i++;
			}
		}
	}
	pieces++;
	fallen = 0;
}

static void
delete_piece(void) {
	for (int i = 0; i < 4; i++) {
		draw_block(piece[i][0], piece[i][1], COL_BGR);
	}
}

static void
reset_game(void) {
	level  = 1; lines  = 0;
	pieces = 0; score  = 0;
	fallen = 0; over   = 0;
	halt   = 0;
	init_piece();
	init_field();
	draw_field();
	draw_piece();
	draw_score();
	Dpy::update();
}

static void
halt_game(void) {
	if (over) {
		reset_game();
		return;
	}
	if (halt) {
		draw_field();
		draw_piece();
		draw_score();
		halt = 0;
	} else {
		Dpy::message_show("Pause", NULL, COL_WHT);
		halt = 1;
	}
	Dpy::update();
}

static int
drop_piece(void) {
	if (halt) halt_game();
	for (int i=0; i<4; i++) {
		if ((field[piece[i][1]+1][piece[i][0]]) || ((piece[i][1]+1)>=HEIGHT)) {
			if (!fallen) game_over();
			return (1);
		}
	}
	delete_piece();
	for (int i=0; i<4; i++) {
		piece[i][1]++;
	}
	draw_piece();
	draw_score();
	Dpy::update();
	fallen++;
	return (0);
}

static void
move_piece(int dir) {
	if (halt) halt_game();
	for (int i=0; i<4; i++) {
		if (field[piece[i][1]][piece[i][0]+dir]) return;
		if ((piece[i][0]+dir)<0)                 return;
		if ((piece[i][0]+dir)>WIDTH-1)           return;
	}
	delete_piece();
	for (int i=0; i<4; i++) {
		piece[i][0] += dir;
	}
	draw_piece();
	draw_score();
	Dpy::update();
}

static void
rotate_piece(void) {
	char tmp[4][2];
	int x = WIDTH, y = HEIGHT;

	if (halt) halt_game();
	for (int i=0; i<4; i++) {
		if (piece[i][0] < x) x = piece[i][0];
		if (piece[i][1] < y) y = piece[i][1];
	}
	for (int i=0; i<4; i++) {
		tmp[i][0] = x+(piece[i][1]-y);
		tmp[i][1] = 1+y-(piece[i][0]-x);
	}
	for (int i=0; i<4; i++) {
		if (field[(int)tmp[i][1]][(int)tmp[i][0]]) return;
		if ((tmp[i][0]) < 0)        return;
		if ((tmp[i][0]) > WIDTH-1)  return;
		if ((tmp[i][1]) < 0)        return;
		if ((tmp[i][1]) > HEIGHT-1) return;
	}
	delete_piece();
	for (int i=0; i<4; i++) {
		piece[i][0] = tmp[i][0];
		piece[i][1] = tmp[i][1];
	}
	draw_piece();
	draw_score();
	Dpy::update();
}

static void
flash_line(int line) {
	for (int i=0; i<4; i++) {
		for (int x=0; x<WIDTH; x++) {
			draw_block(x, line, COL_BGR);
		}
		Dpy::update();
		usleep(1000);
		for (int x=0; x<WIDTH; x++) {
			draw_block(x, line, COL_WHT);
		}
		Dpy::update();
		usleep(1000);
	}
}

static void
remove_lines(void) {
	bool erase;

	for (int line=0; line<HEIGHT; line++) {
		erase = true;
		for (int x=0; x<WIDTH; x++) {
			if (!field[line][x]) erase = false;
		}
		if (erase) {
			for (int i=line; i>0; i--) {
				for (int x=0; x<WIDTH; x++) {
					field[i][x] = field[i-1][x];
				}
			}
			flash_line(line);
			lines++;
			if (!(lines%15)) level++;
			if (level>9) level = 9;
			score += 3*level;
			draw_field();
			draw_score();
			Dpy::update();
		}
	}
}

int
tetris_start(int x, int y, int s, int c) {
	int X, Y, W, H;

	SRND();
	size = 4+4*s;
	W = WIDTH*size;
	H = HEIGHT*size;
	X = DISPLAY_MINX+(int)((DISPLAY_MAXX-W-DISPLAY_MINX)*((x+9.0)/18.0));
	Y = DISPLAY_MINY+(int)((DISPLAY_MAXY-H-DISPLAY_MINY)*((y+9.0)/18.0));
	Dpy::open(X, Y, W, H);
	// Dpy::status_center("Tetris", COL_WHT); // XXX status line to short
	reset_game();
	running = true;
	stopped = false;
	key = KEY_NONE;
	while (running) {
		for (int j=0; j<5; j++) {
			switch (key) {
				case KEY_LEFT:  move_piece(-1);        break;
				case KEY_RIGHT: move_piece(1);         break;
				case KEY_UP:    rotate_piece();        break;
				case KEY_DOWN:  while (!drop_piece()); break;
				case KEY_BACK:  running = false;       break;
				case KEY_OK:    halt_game();           break;
				case KEY_NONE:                         break;
			}
			key = KEY_NONE;
			usleep(17000+8000*(10-level));
			if (!running) goto end;
		}
		if (halt || over) continue;
		if ((running) && (drop_piece())) {
			for (int i=0; i<4; i++) {
				field[piece[i][1]][piece[i][0]] = color;
			}
			remove_lines();
			score += level;
			draw_score();
			if (!over) init_piece();
		}
	}
end:
	Dpy::close();
	stopped = true;
	return (0);
}

int
tetris_stop(void) {
	running = false;
	while (!stopped) usleep(10000);
	return (0);
}

int
tetris_key(int k) {
	key = k; // XXX maybe mutex lock or cond signal?
	if (key == KEY_BACK) {
		tetris_stop();
		return (1);
	}
	return (0);
}
