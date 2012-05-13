/*****************************************************************************\
*                                                                             *
* Programm:    Tron                                                           *
* License:     GPL (General Puplic License)                                   *
* Last Change: 2003-04-21                                                     *
* Authors:     Rhett D. Jacobs <rhett@hotel.canberra.edu.au>                  *
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

#define EMPTY   0
#define PLAYER1 1
#define PLAYER2 2

#define _A_ (index[0] < WIDTH  && index[0] >= 0)
#define _B_ (index[1] < HEIGHT && index[1] >= 0)
#define _C_ (field[index[1]][index[0]] == 0)

typedef enum Direction  { LEFT, RIGHT, UP, DOWN } Direction;
typedef enum PlayerType { HUMAN, COMPUTER       } PlayerType;

typedef struct Player {
	int posx;
	int posy;
	int score;
	int alive;
	Direction  dir;
	PlayerType type;
} Player;

static char field[HEIGHT][WIDTH];
static Player player[2];

static bool running, stopped;

static int key;

static int level, over, halt, next, skill, size;

static void
init_player(void) {
	for (int i=0; i<2; i++) {
		player[i].alive = true;
		player[i].score = 0;
		player[i].posy  = HEIGHT/2;
	}
	player[0].dir  = LEFT;
	player[0].type = HUMAN;
	player[0].posx = (WIDTH/2)-3;
	player[1].dir  = RIGHT;
	player[1].type = COMPUTER;
	player[1].posx = (WIDTH/2)+3;
}

static void
init_field(void) {
	int x, y;

	for (x=0; x<WIDTH; x++) {
		for (y=0; y<HEIGHT; y++) {
			field[y][x] = EMPTY;
		}
	}
	field[player[0].posy][player[0].posx] = PLAYER1;
	field[player[1].posy][player[1].posx] = PLAYER2;
}

static void
draw_score(void) {
	Dpy::status_left(player[0].score, COL_YEL);
	Dpy::status_right(player[1].score, COL_GRN);
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

	for (x=0; x<WIDTH; x++) {
		for (y=0; y<HEIGHT; y++) {
			switch (field[y][x]) {
				case PLAYER1: draw_block(x, y, COL_YEL); break;
				case PLAYER2: draw_block(x, y, COL_GRN); break;
				default:      draw_block(x, y, COL_BGR); break;
			}
		}
	}
}

static void
restart_game(void) {
	int sc1, sc2;

	halt = 0;
	over = 0;
	next = 0;
	sc1  = player[0].score;
	sc2  = player[1].score;
	init_player();
	player[0].score = sc1;
	player[1].score = sc2;
	init_field();
	draw_field();
	draw_score();
	Dpy::update();
}

static void
reset_game(void) {
	halt  = 0;
	over  = 0;
	next  = 0;
	level = 1;
	init_player();
	init_field();
	draw_field();
	draw_score();
	Dpy::update();
}

static void
halt_game(void) {
	if (over) {
		reset_game();
		return;
	}
	if (next) {
		restart_game();
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
game_over(void) {
	over = 1;
	if (player[0].score > player[1].score) {
		Dpy::message_show("Game Over", "Yellow Wins", COL_WHT);
	} else {
		Dpy::message_show("Game Over", "Green Wins", COL_WHT);
	}
	Dpy::update();
}

static void
player_turn(Direction dir) {
	if (player[0].type == HUMAN) {
		switch (player[0].dir) {
			case LEFT:  if (dir != RIGHT) player[0].dir = dir;
			case RIGHT: if (dir != LEFT)  player[0].dir = dir;
			case UP:    if (dir != DOWN)  player[0].dir = dir;
			case DOWN:  if (dir != UP)    player[0].dir = dir;
		}
	}
}

static bool
will_crash(int p, int x, int y) {
	if (field[player[p].posy+y][player[p].posx+x]) {
		return (true);
	}
	if (y && ((player[p].posy+y) >= HEIGHT || (player[p].posy+y) < 0)) {
		return (true);
	}
	if (x && ((player[p].posx+x) >= WIDTH  || (player[p].posx+x) < 0)) {
		return (true);
	}
	return (false);
}

static void
player_think(int p) {
	int index[2], flags[6] = { 0, 0, 0, 0, 0, 0 };
	int dis_forward, dis_left, dis_right;
	Direction sides[2];

	dis_forward = dis_left = dis_right = 1;
	switch (player[p].dir) {
		case LEFT:
			flags[0] = -1;
			flags[1] = 0;
			flags[2] = 0;
			flags[3] = 1;
			flags[4] = 0;
			flags[5] = -1;
			sides[0] = DOWN;
			sides[1] = UP;
		break;
		case RIGHT:
			flags[0] = 1;
			flags[1] = 0;
			flags[2] = 0;
			flags[3] = -1;
			flags[4] = 0;
			flags[5] = 1;
			sides[0] = UP;
			sides[1] = DOWN;
		break;
		case UP:
			flags[0] = 0;
			flags[1] = -1;
			flags[2] = -1;
			flags[3] = 0;
			flags[4] = 1;
			flags[5] = 0;
			sides[0] = LEFT;
			sides[1] = RIGHT;
		break;
		case DOWN:
			flags[0] = 0;
			flags[1] = 1;
			flags[2] = 1;
			flags[3] = 0;
			flags[4] = -1;
			flags[5] = 0;
			sides[0] = RIGHT;
			sides[1] = LEFT;
		break;
	}
	index[0] = player[p].posx+flags[0];
	index[1] = player[p].posy+flags[1];
	while (_A_ && _B_ && _C_) {
		dis_forward++;
		index[0] += flags[0];
		index[1] += flags[1];
	}
	if (dis_forward < skill) {
		dis_forward = 100 - 100/dis_forward;
		index[0] = player[p].posx+flags[2];
		index[1] = player[p].posy+flags[3];
		while (_A_ && _B_ && _C_) {
			dis_left++;
			index[0] += flags[2];
			index[1] += flags[3];
		} 
		index[0] = player[p].posx+flags[4];
		index[1] = player[p].posy+flags[5];
		while (_A_ && _B_ && _C_) {
			dis_right++;
			index[0] += flags[4];
			index[1] += flags[5];
		} 
		if (!(dis_left == 1 && dis_right == 1))
		if (RAND(100) >= dis_forward || dis_forward == 0) {
			if (RAND(100) <= (100*dis_left)/(dis_left+dis_right)) {
				if (dis_left != 1) {
					player[p].dir = sides[0];
				} else {
					player[p].dir = sides[1];
				}
			} else {
				if (dis_right != 1) {
					player[p].dir = sides[1];
				} else {
					player[p].dir = sides[0];
				}
			}
		}
	}
}

static bool
update_game(void) {
	int i;

	for (i=0; i<2; i++) {
		if (player[i].type == COMPUTER) player_think(i);
		switch (player[i].dir) {
			case LEFT:
				if (will_crash(i, -1, 0)) {
					player[i].alive = false;
				} else {
					player[i].posx--;
				}
			break;
			case RIGHT:
				if (will_crash(i, 1, 0)) {
					player[i].alive = false;
				} else {
					player[i].posx++;
				}
			break;
			case UP:
				if (will_crash(i, 0, -1)) {
					player[i].alive = false;
				} else {
					player[i].posy--;
				}
			break;
			case DOWN:
				if (will_crash(i, 0, 1)) {
					player[i].alive = false;
				} else {
					player[i].posy++;
				}
			break;
		}
		field[player[i].posy][player[i].posx] = PLAYER1+i;
	}
	draw_block(player[0].posx, player[0].posy, COL_YEL);
	draw_block(player[1].posx, player[1].posy, COL_GRN);
	if (!player[0].alive) player[1].score++;
	if (!player[1].alive) player[0].score++;
	draw_score();
	Dpy::update();
	for (i=0; i<2; i++) if (!player[i].alive) return (true);
	return (false);
}

int
tron_start(int x, int y, int s, int c) {
	int X, Y, W, H;

	SRND();
	skill = 3*c;
	size  = 4*s;
	W = WIDTH*size;
	H = HEIGHT*size;
	X = DISPLAY_MINX+(int)((DISPLAY_MAXX-W-DISPLAY_MINX)*((x+9.0)/18.0));
	Y = DISPLAY_MINY+(int)((DISPLAY_MAXY-H-DISPLAY_MINY)*((y+9.0)/18.0));
	Dpy::open(X, Y, W, H);
	Dpy::status_center("Tron", COL_WHT);
	reset_game();
	running = true;
	stopped = false;
	key = KEY_NONE;
	while (running) {
		switch (key) {
			case KEY_LEFT:  player_turn(LEFT);  break;
			case KEY_RIGHT: player_turn(RIGHT); break;
			case KEY_UP:    player_turn(UP);    break;
			case KEY_DOWN:  player_turn(DOWN);  break;
			case KEY_BACK:  running = false;    break;
			case KEY_OK:    halt_game();        break;
			case KEY_NONE:                      break;
		}
		key = KEY_NONE;
		usleep(10000+6000*(10-level));
		if (halt || over || next) continue;
		if (running && update_game()) {
			next = true;
			if ((player[0].score >= 9) || (player[1].score >= 9)) {
				game_over();
			}
			level = player[0].score + 1;
		}
	}
	Dpy::close();
	stopped = true;
	return (0);
}

int
tron_stop(void) {
	running = false;
	while (!stopped) usleep(10000);
	return (0);
}

int
tron_key(int k) {
	key = k; // XXX maybe mutex lock or cond signal?
	if (key == KEY_BACK) {
		tron_stop();
		return (1);
	}
	return (0);
}
