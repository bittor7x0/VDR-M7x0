/*****************************************************************************\
*                                                                             *
* Programm:    Mines                                                          *
* License:     GPL (General Puplic License)                                   *
* Last Change: 2003-05-03                                                     *
* Authors:     Gerard Monells <Chico_GMB@hotmail.com>                         *
*              Clemens Kirchgatterer <clemens@thf.ath.cx>                     *
*                                                                             *
\*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../display.h"
#include "../defines.h"

#include "minesweeper.h"

typedef enum Direction {
	LEFT,
	RIGHT,
	UP,
	DOWN
} Direction;

typedef enum Icons {
	MINE = 9,
	EXPLODED,
	WRONG,
	HIDDEN,
	FLAG,
	ENIGMA,
	CURSOR,
	SHOWN,
	ICONS
} Icons;
	
static char field[16][12];
static char hidden[16][12];

static bool running, stopped;

static int key;

static int over, remaining, posx, posy, mines, width, height, skill;

static void
init_field(void) {
	int a, b, i, j, m = 0;
 
	for (j=0; j<height; j++) {
		for (i=0; i<width; i++) {
			field[i][j] = HIDDEN;
		}
	}
	for (j=0; j<height; j++) {
		for (i=0; i<width; i++) {
			hidden[i][j] = 0;
		}
	}
	while (m < mines) {
		i = RAND(width-1);
		j = RAND(height-1);
		if (hidden[i][j] != MINE) {
			hidden[i][j] = MINE; m++;
			for (b=-1; b<2; b++) {
				for (a=-1; a<2; a++) {
					if (i+a>=0 && i+a<width && j+b>=0 && j+b<height) {
						if (hidden[i+a][j+b]<MINE) hidden[i+a][j+b]++;
					}
				}
			}
		}
	}
}

static void
draw_icon(int x, int y) {
	int a=SIZE*x, b=SIZE*y;

	for (int j=0; j<SIZE; j++) {
		for (int i=0; i<SIZE; i++) {
			Dpy::pixel_set(a+i, b+j, icon[(int)field[x][y]][j][i]);
		}
	}
}

static void
draw_field(void) {
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			draw_icon(i, j);
		}
	}
	Dpy::status_center("Minesweeper", COL_WHT);
}

static void
draw_cursor(void) {
	int a=SIZE*posx, b=SIZE*posy, c;

	for (int j=0; j<SIZE; j++) {
		for (int i=0; i<SIZE; i++) {
			c = icon[CURSOR][j][i];
			if (c != t) {
				Dpy::pixel_set(a+i, b+j, c);
			}
		}
	}
}

static void
move_cursor(Direction dir) {
	if (over) return;
	draw_icon(posx, posy);
	switch (dir) {
		case LEFT:  if (posx>0)        posx--; break;
		case RIGHT: if (posx<width-1)  posx++; break;
		case UP:    if (posy>0)        posy--; break;
		case DOWN:  if (posy<height-1) posy++; break;
	}
	draw_cursor();
	Dpy::update();
}

void
wrong_show(void) {
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			if (field[i][j] != FLAG && hidden[i][j] == MINE) {
				field[i][j] = MINE;
			}
			if (field[i][j] == FLAG && hidden[i][j] != MINE) {
				field[i][j] = WRONG;
			}
			hidden[i][j] = SHOWN;
			draw_icon(i, j);
		}
	}
}

static int
zero_show(int x, int y) {
	int h, s = 0;

	for (int j=-1; j<2; j++) {
		for (int i=-1; i<2; i++) {
			if (x+i>=0 && x+i<width && y+j>=0 && y+j<height) {
				h = hidden[x+i][y+j];
				if (h != SHOWN) {
					hidden[x+i][y+j] = SHOWN;
					field[x+i][y+j] = h;
					draw_icon(x+i, y+j);
					s++;
					if (field[x+i][y+j] == 0) {
						s = s + zero_show(x + i, y + j);
					}
				}
			}
		}
	}
	return (s);
}

static void
flags_show(void) {
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			if (hidden[i][j] != SHOWN && hidden[i][j] == MINE) {
				field[i][j] = FLAG;
				draw_icon(i, j);
			}
		}
	}
}

static void
mines_show(void) {
	for (int j=0; j<height; j++) {
		for (int i=0; i<width; i++) {
			if (field[i][j] == FLAG) {
				field[i][j] = MINE;
				draw_icon(i, j);
			}
		}
	}
}

static void
reset_game(void) {
	over = 0;
	posx = 0;
	posy = 0;
	remaining = width*height;
	init_field();
	draw_field();
	draw_cursor();
	Dpy::update();
}

static void
game_over(int won) {
	if (won) {
		flags_show();
		mines_show();
		Dpy::message_show("Game Over", "You Won", COL_WHT);
	} else {
		wrong_show();
		Dpy::message_show("Game Over", "You Lost", COL_WHT);
	}
	over = 1;
}

static void
place_mark(int key) {
	int h;

	if (over || (hidden[posx][posy] == SHOWN)) return;
	switch (key) {
		case KEY_1:
			h = hidden[posx][posy];
			hidden[posx][posy] = SHOWN;
			field[posx][posy] = h;
			if (h != MINE) {
				if (field[posx][posy] == 0) {
					remaining -= zero_show(posx, posy);
				}
				remaining--;
				if (remaining == mines) {
					game_over(1);
				}
			} else {
				field[posx][posy] = EXPLODED;
				game_over(0);
			}
		break;
		case KEY_2:
			field[posx][posy] = ENIGMA;
		break;
		case KEY_3:
			field[posx][posy] = FLAG;
		break;
	}
	if (!over) {
		draw_icon(posx, posy);
		draw_cursor();
	}
	Dpy::update();
}

int
minesweeper_start(int x, int y, int s, int c) {
	int X, Y, W, H;

	SRND();
	switch (s) {
		case 1: width =  7; height =  5; mines = 4; break;
		case 2: width = 10; height =  7; mines = 6; break;
		case 3: width = 15; height = 10; mines = 8; break;
	}
	mines *= c;
	skill = c;
	W = width*SIZE;
	H = height*SIZE;
	X = DISPLAY_MINX+(int)((DISPLAY_MAXX-W-DISPLAY_MINX)*((x+9.0)/18.0));
	Y = DISPLAY_MINY+(int)((DISPLAY_MAXY-H-DISPLAY_MINY)*((y+9.0)/18.0));
	Dpy::open(X, Y, W, H);
	reset_game();
	running = true;
	stopped = false;
	key = KEY_NONE;
	while (running) {
		switch (key) {
			case KEY_NONE:                      break;
			case KEY_OK:    reset_game();       break;
			case KEY_LEFT:  move_cursor(LEFT);  break;
			case KEY_RIGHT: move_cursor(RIGHT); break;
			case KEY_UP:    move_cursor(UP);    break;
			case KEY_DOWN:  move_cursor(DOWN);  break;
			case KEY_BACK:  running = false;    break;
			default:        place_mark(key);    break;
		}
		key = KEY_NONE;
		usleep(10000);
	}
	Dpy::close();
	stopped = true;
	return (0);
}

int
minesweeper_stop(void) {
	running = false;
	while (!stopped) usleep(10000);
	return (0);
}

int
minesweeper_key(int k) {
	key = k; // XXX maybe mutex lock or cond signal?
	if (key == KEY_BACK) {
		minesweeper_stop();
		return (1);
	}
	return (0);
}
