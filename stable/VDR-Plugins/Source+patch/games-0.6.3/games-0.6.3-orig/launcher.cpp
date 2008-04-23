#include <stdlib.h>
#include <pthread.h>

#include "defines.h"

typedef struct Game {
	char *name;
	int  (*start)(int, int, int, int);
	int  (*stop)(void);
	int  (*key)(int);
} Game;

typedef struct ThreadParam {
	int i, x, y, s, c;
} ThreadParam;

// function prototype factory
GAME(tetris);
GAME(tron);
GAME(snake);
GAME(tictactoe);
GAME(minesweeper);

// launcher menu factory
MENUSTART(Game, menu)
MENUENTRY("Tetris",      tetris)
MENUENTRY("Tron",        tron)
MENUENTRY("Snake",       snake)
MENUENTRY("TicTacToe",   tictactoe)
MENUENTRY("Minesweeper", minesweeper)
MENUEND()

static int   game = -1;
static char *games[32];

static void *
launcher_thread(void *p) {
	ThreadParam *param;
	int x, y, s, c;

	param = (ThreadParam *)p;
	x = param->x;
	y = param->y;
	s = param->s;
	c = param->c;
	free(param);
	// call the games start function
	menu[game].start(x, y, s, c);
	return (NULL);
}

int
launcher_menu(char ***m) {
	int i = 0;

	while (menu[i].name) {
		games[i] = menu[i].name;
		i++;
	}
	games[i] = NULL;
	*m = games;
	return (i);
}

int
launcher_start(int g, int x, int y, int s, int c) {
	ThreadParam *param;
	pthread_t thr_id;

	game = g;
	param = (ThreadParam *)malloc(sizeof (ThreadParam));
	param->x = x; param->y = y; param->s = s; param->c = c;
	pthread_create(&thr_id, NULL, &launcher_thread, (void *)param);
	pthread_detach(thr_id);
	return (0);
}

int
launcher_stop(void) {
	if (game >= 0) {
		// call the games stop function
		return (menu[game].stop());
		game = -1;
	}
	return (0);
}

int
launcher_key(int key) {
	if (game >= 0) {
		// call the games event handler
		return (menu[game].key(key));
	}
	return (0);
}
