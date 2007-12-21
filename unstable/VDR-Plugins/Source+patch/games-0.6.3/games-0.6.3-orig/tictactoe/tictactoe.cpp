/*****************************************************************************\
*                                                                             *
* Programm:    TicTacToe                                                      *
* License:     GPL (General Puplic License)                                   *
* Last Change: 2003-04-21                                                     *
* Authors:     Aaron Rogers <webmaster@aaronrogers.com>                       *
*              Clemens Kirchgatterer <clemens@thf.ath.cx>                     *
*                                                                             *
\*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "../bitmap.h"
#include "../display.h"
#include "../defines.h"

#define SIZE    7

#define EMPTY   0
#define PLAYER1 1
#define PLAYER2 2

typedef enum Direction  { LEFT, RIGHT, UP, DOWN } Direction;
typedef enum PlayerType { HUMAN, COMPUTER       } PlayerType;

static unsigned char field[3][3];
static int score[3];

static bool running, stopped;

static int key;

static int posx, posy, skill, size, moves, plr;
static Bitmap *bmpx, *bmpo;

static void
init_score(void) {
	plr = PLAYER1 + RAND(1);
	score[0] = 0;
	score[PLAYER1] = 0;
	score[PLAYER2] = 0;
}

static void
init_field(void) {
	int x, y;

	for (x=0; x<3; x++) {
		for (y=0; y<3; y++) {
			field[y][x] = EMPTY;
		}
	}
}

static void
init_x(void) {
	int x1, y1, ln;

	bmpx = new Bitmap(size*SIZE, size*SIZE);
	x1 = 2*size;
	y1 = 2*size;
	ln = size*SIZE-4*size;
	for (int i=0; i<size/2; i++) {
		for (int j=0; j<ln+1; j++) {
			bmpx->pixel(x1+j+i,   y1+j-i,    COL_WHT);
			bmpx->pixel(x1+j+i+1, y1+j-i,    COL_WHT);
			bmpx->pixel(x1+j-i,   y1+j+i,    COL_WHT);
			bmpx->pixel(x1+j-i+1, y1+j+i,    COL_WHT);
			bmpx->pixel(x1+j-i,   y1-j-i+ln, COL_WHT);
			bmpx->pixel(x1+j-i+1, y1-j-i+ln, COL_WHT);
			bmpx->pixel(x1+j+i,   y1-j+i+ln, COL_WHT);
			bmpx->pixel(x1+j+i+1, y1-j+i+ln, COL_WHT);
		}
	}
}

static void
init_o(void) {
	int x1, y1, r1, r2, r3, dx, dy, dr;

	bmpo = new Bitmap(size*SIZE, size*SIZE);
	x1 = size*SIZE/2;
	y1 = size*SIZE/2;
	r1 = size*SIZE/2-size-1;
	r2 = r1*r1;
	for (dr=0; dr<size*2; dr++) {
		r3 = r2-dr*r1;
		for (dy=0; dy<r1*0.707; dy++) {
			dx = (int)(sqrt(r3-dy*dy)+0.5);
			bmpo->pixel(x1+dx, y1+dy, COL_WHT);
			bmpo->pixel(x1-dx, y1+dy, COL_WHT);
			bmpo->pixel(x1+dx, y1-dy, COL_WHT);
			bmpo->pixel(x1-dx, y1-dy, COL_WHT);
			bmpo->pixel(x1+dy, y1+dx, COL_WHT);
			bmpo->pixel(x1-dy, y1+dx, COL_WHT);
			bmpo->pixel(x1+dy, y1-dx, COL_WHT);
			bmpo->pixel(x1-dy, y1-dx, COL_WHT);
		}
	}
}

static void
clear_cell(int x, int y) {
	int x1, y1, x2, y2;

	x1 = x*(size*SIZE+size);
	y1 = y*(size*SIZE+size);
	x2 = x1+size*SIZE;
	y2 = y1+size*SIZE;
	Dpy::rect_fill(x1, y1, x2, y2, COL_BGR);
}

static void
clear_field(void) {
	for (int i=0; i<3; i++) {
		clear_cell(i, 0);
		Dpy::update();
		usleep(10000);
	}
	clear_cell(2, 1);
	Dpy::update();
	usleep(10000);
	for (int i=0; i<3; i++) {
		clear_cell(2-i, 2);
		Dpy::update();
		usleep(10000);
	}
	clear_cell(0, 1);
	Dpy::update();
	usleep(10000);
	clear_cell(1, 1);
	Dpy::update();
	usleep(10000);
}

static void
draw_field(void) {
	int i, j, k, l, m;

	i = size*SIZE;
	j = size;
	k = 3*i+2*j;
	l = 2*i;
	m = 2*j;
	Dpy::rect_fill(  0,   0,   k,   k, COL_BGR);
	Dpy::rect_fill(  i,   0, i+j,   k, COL_WHT);
	Dpy::rect_fill(l+j,   0, l+m,   k, COL_WHT);
	Dpy::rect_fill(  0,   i,   k, i+j, COL_WHT);
	Dpy::rect_fill(  0, l+j,   k, l+m, COL_WHT);
	Dpy::status_center("TicTacToe", COL_WHT);
}

static void
draw_cursor(int x, int y, int col) {
	int x1, y1, x2, y2;

	x1 = x*(size*SIZE+size);
	y1 = y*(size*SIZE+size);
	x2 = x1+size;
	y2 = y1+size*SIZE;
	Dpy::rect_fill(x1, y1, x2, y2, col);
	x2 = x1+size*SIZE;
	y2 = y1+size;
	Dpy::rect_fill(x1, y1, x2, y2, col);
	x1 = x1+size*SIZE-size;
	x2 = x1+size;
	y2 = y1+size*SIZE;
	Dpy::rect_fill(x1, y1, x2, y2, col);
	x1 = x*(size*SIZE+size);
	y1 = y1+size*SIZE-size;
	x2 = x1+size*SIZE;
	y2 = y1+size;
	Dpy::rect_fill(x1, y1, x2, y2, col);
}

static void
move_cursor(Direction dir) {
	draw_cursor(posx, posy, COL_BGR);
	switch (dir) {
		case LEFT:  if (posx>0) posx--; break;
		case RIGHT: if (posx<2) posx++; break;
		case UP:    if (posy>0) posy--; break;
		case DOWN:  if (posy<2) posy++; break;
	}
	draw_cursor(posx, posy, COL_BLU);
	Dpy::update();
}

static void
draw_piece(Bitmap *bmp, int x, int y) {
	int x1, y1;

	x1 = x*size*SIZE + x*size;
	y1 = y*size*SIZE + y*size;
	Dpy::bitmap_blit(bmp, x1, y1);
}

static void
flash_column(int x) {
	Bitmap *bmp;

	for (int i=0; i<3; i++) {
		for (int y=0; y<3; y++) clear_cell(x, y);
		Dpy::update();
		usleep(10000);
		for (int y=0; y<3; y++) {
			(field[y][x] == PLAYER1) ? bmp = bmpo : bmp = bmpx;
			draw_piece(bmp, x, y);
		}
		Dpy::update();
		usleep(10000);
	}
}

static void
flash_row(int y) {
	Bitmap *bmp;

	for (int i=0; i<3; i++) {
		for (int x=0; x<3; x++) clear_cell(x, y);
		Dpy::update();
		usleep(10000);
		for (int x=0; x<3; x++) {
			(field[y][x] == PLAYER1) ? bmp = bmpo : bmp = bmpx;
			draw_piece(bmp, x, y);
		}
		Dpy::update();
		usleep(10000);
	}
}

static void
flash_diagonal(int d) {
	Bitmap *bmp;

	for (int i=0; i<3; i++) {
		for (int j=0; j<3; j++) {
			(d) ? clear_cell(2-j, j) : clear_cell(j, j);
		}
		Dpy::update();
		usleep(10000);
		for (int j=0; j<3; j++) {
			if (d) {
				(field[2-j][j] == PLAYER1) ? bmp = bmpo : bmp = bmpx;
				draw_piece(bmp, 2-j, j);
			} else {
				(field[j][j] == PLAYER1) ? bmp = bmpo : bmp = bmpx;
				draw_piece(bmp, j, j);
			}
		}
		Dpy::update();
		usleep(10000);
	}
}

static bool
check_win(int p) {
	bool won;

	for (int x=0; x<3; x++) {
		won = true;
		for (int y=0; y<3; y++) {
			if (field[y][x] != p) won = false;
		}
		if (won) { flash_column(x); return (true); }
	}
	for (int y=0; y<3; y++) {
		won = true;
		for (int x=0; x<3; x++) {
			if (field[y][x] != p) won = false;
		}
		if (won) { flash_row(y); return (true); }
	}
	won = true;
	for (int i=0; i<3; i++) {
		if (field[i][i] != p) won = false;
	}
	if (won) { flash_diagonal(0); return (true); }
	won = true;
	for (int i=0; i<3; i++) {
		if (field[2-i][i] != p) won = false;
	}
	if (won) { flash_diagonal(1); return (true); }
   return (false);
}

static void
reset_game(void) {
	moves = 0;
	posx = posy = 1;
	init_score();
	init_field();
	draw_field();
	draw_cursor(posx, posy, COL_BLU);
	Dpy::update();
}

static void
place_mark(int x, int y) {
	int winner;

	if (field[y][x] == EMPTY) {
		moves++;
		if (plr == PLAYER1) {
			draw_piece(bmpo, x, y);
			field[y][x] = PLAYER1;
			winner = PLAYER1;
			plr = PLAYER2;
		} else {
			draw_piece(bmpx, x, y);
			field[y][x] = PLAYER2;
			winner = PLAYER2;
			plr = PLAYER1;
		}
		Dpy::update();
		if (check_win(winner)) {
			score[winner]++;
			reset_game();
		} else {
			if (moves >= 9) {
				score[0]++;
				clear_field();
				reset_game();
			}
		}
	}
}

static int
who(int i) {
	return (field[i/3][i%3]);
}

static int
other_player(void) {
	if (plr == PLAYER1) return (PLAYER2);
	return (PLAYER1);
}

static bool
win_move(int x, int pl) {
	if ((x==0) && ((who(1)==pl) && (who(2)==pl))) return (true);
	if ((x==0) && ((who(3)==pl) && (who(6)==pl))) return (true);
	if ((x==0) && ((who(4)==pl) && (who(8)==pl))) return (true);
	if ((x==1) && ((who(0)==pl) && (who(2)==pl))) return (true);
	if ((x==1) && ((who(4)==pl) && (who(7)==pl))) return (true);
	if ((x==2) && ((who(0)==pl) && (who(1)==pl))) return (true);
	if ((x==2) && ((who(4)==pl) && (who(6)==pl))) return (true);
	if ((x==2) && ((who(5)==pl) && (who(8)==pl))) return (true);
	if ((x==3) && ((who(0)==pl) && (who(6)==pl))) return (true);
	if ((x==3) && ((who(4)==pl) && (who(5)==pl))) return (true);
	if ((x==4) && ((who(0)==pl) && (who(8)==pl))) return (true);
	if ((x==4) && ((who(1)==pl) && (who(7)==pl))) return (true);
	if ((x==4) && ((who(2)==pl) && (who(6)==pl))) return (true);
	if ((x==4) && ((who(3)==pl) && (who(5)==pl))) return (true);
	if ((x==5) && ((who(2)==pl) && (who(8)==pl))) return (true);
	if ((x==5) && ((who(3)==pl) && (who(4)==pl))) return (true);
	if ((x==6) && ((who(0)==pl) && (who(3)==pl))) return (true);
	if ((x==6) && ((who(2)==pl) && (who(4)==pl))) return (true);
	if ((x==6) && ((who(7)==pl) && (who(8)==pl))) return (true);
	if ((x==7) && ((who(1)==pl) && (who(4)==pl))) return (true);
	if ((x==7) && ((who(6)==pl) && (who(8)==pl))) return (true);
	if ((x==8) && ((who(0)==pl) && (who(4)==pl))) return (true);
	if ((x==8) && ((who(2)==pl) && (who(5)==pl))) return (true);
	if ((x==8) && ((who(6)==pl) && (who(7)==pl))) return (true);
	return (false);
}

static int
move_to_win(int pl) {	
	for (int i=0; i<9; i++) {
		if ((who(i)==EMPTY) && (win_move(i, pl))) return (i);
	}
	return (-1);
}

static int
move_to_block(int pl) {
	pl = other_player();
	for (int i=0; i<9; i++) {
		if ((who(i)==EMPTY) && (win_move(i, pl))) return (i);
	}
	return (-1);
}

static int
move_to_good(int pl) {
	char tmp[4];
	int j = 0;

	pl = other_player();
	if (moves==1) {
		if (who(4)==EMPTY) return (4);
	}
	if (moves == 3) {
		if ((who(0)==pl) && (who(8)==pl) || (who(2)==pl) && (who(6)==pl)) {
			if (who(1)==EMPTY) tmp[j++] = 1;
			if (who(3)==EMPTY) tmp[j++] = 3;
			if (who(5)==EMPTY) tmp[j++] = 5;
			if (who(7)==EMPTY) tmp[j++] = 7;
			if (j) return (tmp[RAND(j-1)]);
		}
		if (((who(1)==pl) && (who(3)==pl)) || 
			 ((who(1)==pl) && (who(6)==pl)) || 
			 ((who(2)==pl) && (who(3)==pl))) {
			 if (who(0)==EMPTY) return (0);
		}
		if (((who(1)==pl) && (who(5)==pl)) || 
			 ((who(1)==pl) && (who(8)==pl)) ||
			 ((who(0)==pl) && (who(5)==pl))) {
			 if (who(2)==EMPTY) return (2);
		}
		if (((who(0)==pl) && (who(7)==pl)) || 
			 ((who(3)==pl) && (who(7)==pl)) || 
			 ((who(3)==pl) && (who(8)==pl))) {
			 if (who(6)==EMPTY) return (6);
		}
		if (((who(2)==pl) && (who(7)==pl)) || 
			 ((who(5)==pl) && (who(6)==pl)) ||
			 ((who(5)==pl) && (who(7)==pl))) {
			 if (who(8)==EMPTY) return (8);
		}
	}
	return (-1);
}

static int
move_to_corner(int pl) {
	char tmp[4];
	int j = 0;

	if (who(0)==EMPTY) tmp[j++] = 0;
	if (who(2)==EMPTY) tmp[j++] = 2;
	if (who(6)==EMPTY) tmp[j++] = 6;
	if (who(8)==EMPTY) tmp[j++] = 8;
	if (j) return (tmp[RAND(j-1)]);
	return (-1);
}

static int
move_to_center(int pl) {
	if (who(4)==EMPTY) return (4);
	return (-1);
}

static int
move_to_free(int pl) {
	char tmp[9];
	int j = 0;

	for (int i=0; i<9; i++) {
		if (who(i)==EMPTY) tmp[j++] = i;
	}
	if (j) return (tmp[RAND(j-1)]);
	return (-1);
}

static int
computer_move(int &x, int &y) {
	int spot = -1;

	switch (skill) {
		case 1:
			if (RAND(1) && (spot = move_to_win(plr))   != -1) break;
			if (RAND(1) && (spot = move_to_block(plr)) != -1) break;
		break;
		case 2:
			if ((spot = move_to_win(plr))    != -1) break;
			if ((spot = move_to_block(plr))  != -1) break;
		break;
		case 3:
			if ((spot = move_to_win(plr))    != -1) break;
			if ((spot = move_to_block(plr))  != -1) break;
			if ((spot = move_to_good(plr))   != -1) break;
			if ((spot = move_to_corner(plr)) != -1) break;
			if ((spot = move_to_center(plr)) != -1) break;
		break;
	}
	if (spot == -1) spot = move_to_free(plr);
	x = spot%3;
	y = spot/3;
	return (-1);
}

int
tictactoe_start(int x, int y, int s, int c) {
	int X, Y, W, H;

	SRND();
	size  = 4*s;
	skill = c;
	W = 3*size*SIZE+2*size;
	H = 3*size*SIZE+2*size;
	X = DISPLAY_MINX+(int)((DISPLAY_MAXX-W-DISPLAY_MINX)*((x+9.0)/18.0));
	Y = DISPLAY_MINY+(int)((DISPLAY_MAXY-H-DISPLAY_MINY)*((y+9.0)/18.0));
	Dpy::open(X, Y, W, H);
	init_x();
	init_o();
	reset_game();
	running = true;
	stopped = false;
	key = KEY_NONE;
	while (running) {
		if (plr == PLAYER1) {
			computer_move(x, y);
			// XXX move cursor to x, y
			place_mark(x, y);
		}
		switch (key) {
			case KEY_OK:
				place_mark(posx, posy);
				usleep(250000);
			break;
			case KEY_LEFT:  move_cursor(LEFT);  break;
			case KEY_RIGHT: move_cursor(RIGHT); break;
			case KEY_UP:    move_cursor(UP);    break;
			case KEY_DOWN:  move_cursor(DOWN);  break;
			case KEY_BACK:  running = false;    break;
			case KEY_NONE:                      break;
		}
		key = KEY_NONE;
		usleep(10000);
	}
	delete (bmpx);
	delete (bmpo);
	Dpy::close();
	stopped = true;
	return (0);
}

int
tictactoe_stop(void) {
	running = false;
	while (!stopped) usleep(10000);
	return (0);
}

int
tictactoe_key(int k) {
	key = k; // XXX maybe mutex lock or cond signal?
	if (key == KEY_BACK) {
		tictactoe_stop();
		return (1);
	}
	return (0);
}
