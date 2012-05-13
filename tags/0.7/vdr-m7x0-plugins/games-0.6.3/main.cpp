#include <stdlib.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_syswm.h>

#include <X11/keysym.h>
#include <X11/Xutil.h>

#include "defines.h"

extern int launcher_menu(char ***m);
extern int launcher_start(int g, int x, int y, int s, int c);
extern int launcher_stop(void);
extern int launcher_key(int key);

int
main(int argc, char **argv) {
	int games, events, size, game, skill;
	bool running = true, quit = false;
	Display *display = NULL;
	SDL_SysWMinfo info;
	char **menu;
	XEvent xev;
	KeySym key;

	games = launcher_menu(&menu);
	if (argc != 4) {
		printf("\nUSAGE: games <number> <size> <skill>\n\n");
		for (int i=0; i<games; i++) {
			printf("\t%i ... %s\n", i+1, menu[i]);
		}
		printf("\n");
		exit(-1);
	}
	game  = atoi(argv[1]);
	size  = atoi(argv[2]);
	skill = atoi(argv[3]);
	if (game  > games) game  = games;
	if (game  < 1)     game  = 1;
	if (size  > 3)     size  = 3;
	if (size  < 1)     size  = 1;
	if (skill > 3)     skill = 3;
	if (skill < 1)     skill = 1;
	launcher_start(game-1, 0, 0, size, skill);
	sleep(1);
	SDL_VERSION(&info.version);
	if (SDL_GetWMInfo(&info) <= 0) exit(-1);
	display = info.info.x11.display;
	while (running) {
		info.info.x11.lock_func();
		XFlush(display);
		events = XPending(display);
		while (events--) {
			XNextEvent(display, &xev);
			if (xev.type == KeyPress) {
				key = XLookupKeysym(&xev.xkey, 0);
				switch (key) {
					case XK_Left:   quit = launcher_key(KEY_LEFT);  break;
					case XK_Right:  quit = launcher_key(KEY_RIGHT); break;
					case XK_Up:     quit = launcher_key(KEY_UP);    break;
					case XK_Down:   quit = launcher_key(KEY_DOWN);  break;
					case XK_Return: quit = launcher_key(KEY_OK);    break;
					case XK_Escape: quit = launcher_key(KEY_BACK);  break;
					default:
						if ((key >= XK_0) && (key <= XK_9)) {
							quit = launcher_key(key-XK_0+KEY_0);
						}
					break;
				}
				if (quit) return (0);;
			}
		}
		info.info.x11.unlock_func();
		usleep(5000); // 5 ms
	}
	return (0);
}
