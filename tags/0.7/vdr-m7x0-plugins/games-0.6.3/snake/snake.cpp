/*****************************************************************************\
*                                                                             *
* Programm:    Snake                                                          *
* License:     GPL (General Puplic License)                                   *
* Last Change: 2003-04-23                                                     *
* Authors:     Justin Todd <gameprogrammer@excite.com>                        *
*              Clemens Kirchgatterer <clemens@thf.ath.cx>                     *
*                                                                             *
\*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../display.h"
#include "../defines.h"

#define WIDTH  36
#define HEIGHT 27

#define EMPTY  0
#define FRUIT  1
#define SNAKE  2

static unsigned char field[HEIGHT][WIDTH];

static bool running, stopped;
static int key;

static int posx, posy, dirx, diry, length;
static int score, level, fruits, halt, over, size;

static void
move_left() {
	dirx = -1;
	diry = 0;
}

static void
move_right() {
	dirx = 1;
	diry = 0;
}

static void
move_up() {
	dirx = 0;
	diry = -1;
}

static void
move_down() {
	dirx = 0;
	diry = 1;
}

static void
reset_game() {
	int x, y;

	posx   = WIDTH/2;
	posy   = HEIGHT/2;
	length = 3;
	fruits = 0;
	halt   = 0;
	over   = 0;
	score  = 0;
	level  = 1;
	switch (RAND(3)) {
		case 0: move_up();    break;
		case 1: move_down();  break;
		case 2: move_left();  break;
		case 3: move_right(); break;
	}
	for (y=0; y<HEIGHT; y++) {
		for (x=0; x<WIDTH; x++) field[y][x] = EMPTY;
	}
	field[RAND(HEIGHT-1)][RAND(WIDTH-1)] = FRUIT;
}

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
draw_field() {
	int x, y;

	for (y=0; y<HEIGHT; y++) {
		for (x=0; x<WIDTH; x++) {
			switch (field[y][x]) {
				case EMPTY: draw_block(x, y, COL_BGR); break;
				case FRUIT: draw_block(x, y, COL_GRN); break;
				default:    draw_block(x, y, COL_WHT); break;
			}
		}
	}
}

static void
halt_game(void) {
	if (over) {
		reset_game();
		draw_field();
		return;
	}
	if (halt) {
		draw_field();
		halt = 0;
	} else {
		Dpy::message_show("Pause", NULL, COL_WHT);
		halt = 1;
	}
	Dpy::update();
}

static void
draw_score(void) {
	Dpy::status_left(score, COL_WHT);
	Dpy::status_right(level, COL_WHT);
}

static void
game_over(void) {
	over = 1;
	Dpy::message_show("Game Over", NULL, COL_WHT);
	Dpy::update();
}

static void
move_snake() {
	int x, y;

	posx += dirx;
	posy += diry;
	if (field[posy][posx] >= SNAKE) {
		game_over(); return;
	}
	if ((posx < 0) || (posx > WIDTH-1) || (posy < 0) || (posy > HEIGHT-1)) {
		game_over(); return;
	}
	if (field[posy][posx] == FRUIT) {
		x = RAND(WIDTH-1);
		y = RAND(HEIGHT-1);
		field[y][x] = FRUIT;
		draw_block(x, y, COL_GRN);
		length += level;
		score  += level;
		fruits += 1;
		if (fruits == 7) {
			if (level<9) level++;
			length = 3;
			fruits = 0;
		}
	}
	field[posy][posx] = SNAKE;
	for (y=0; y<HEIGHT; y++) {
		for (x=0; x<WIDTH; x++) {
			if (field[y][x] >= SNAKE) {
				if (field[y][x] >= length + SNAKE) {
					field[y][x] = EMPTY;
					draw_block(x, y, COL_BGR);
				} else {
					++field[y][x];
					draw_block(x, y, COL_WHT);
				}
			}
		}
	}
}

int
snake_start(int x, int y, int s, int c) {
	int X, Y, W, H;

	SRND();
	size = 4*s;
	W = WIDTH*size;
	H = HEIGHT*size;
	X = DISPLAY_MINX+(int)((DISPLAY_MAXX-W-DISPLAY_MINX)*((x+9.0)/18.0));
	Y = DISPLAY_MINY+(int)((DISPLAY_MAXY-H-DISPLAY_MINY)*((y+9.0)/18.0));
	Dpy::open(X, Y, W, H);
	Dpy::status_center("Snake", COL_WHT);
	reset_game();
	draw_field();
	draw_score();
	running = true;
	stopped = false;
	key = KEY_NONE;
	while (running) {
		switch (key) {
			case KEY_LEFT:  move_left();     break;
			case KEY_RIGHT: move_right();    break;
			case KEY_UP:    move_up();       break;
			case KEY_DOWN:  move_down();     break;
			case KEY_BACK:  running = false; break;
			case KEY_OK:    halt_game();     break;
			case KEY_NONE:                   break;
		}
		key = KEY_NONE;
		usleep(10000+10000*(10-level));
		if (halt || over) continue;
		if (running) {
			move_snake();
			if (!over) {
				draw_score();
				Dpy::update();
			}
		}
	}
	Dpy::close();
	stopped = true;
	return (0);
}

int
snake_stop(void) {
	running = false;
	while (!stopped) usleep(10000);
	return (0);
}

int
snake_key(int k) {
	key = k; // XXX maybe mutex lock or cond signal?
	if (key == KEY_BACK) {
		snake_stop();
		return (1);
	}
	return (0);
}
