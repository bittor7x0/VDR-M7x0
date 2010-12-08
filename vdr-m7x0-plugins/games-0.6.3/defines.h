#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <stdlib.h> // srand()
#include <time.h>   // time()

#define SRND()      srand(time(NULL))
#define RAND(MAX)   (int)((float)(MAX+1)*rand()/(RAND_MAX+1.0))

#define GAME(NAME)  int NAME ## _start(int x, int y, int s, int c); \
                    int NAME ## _stop(void);                        \
                    int NAME ## _key(int k);

#define MENUSTART(TYPE,NAME)   static TYPE NAME [] = {
#define MENUEND()              { NULL, NULL, NULL, NULL } };
#define MENUENTRY(NAME,PREFIX) { NAME ,            \
                                 PREFIX ## _start, \
                                 PREFIX ## _stop,  \
                                 PREFIX ## _key    \
                               },

#define DISPLAY_MINX     36
#define DISPLAY_MINY     16
#define DISPLAY_MAXX    685
#define DISPLAY_MAXY    543

#define KEY_UP            0
#define KEY_DOWN          1
#define KEY_OK            3
#define KEY_BACK          4
#define KEY_LEFT          5
#define KEY_RIGHT         6
#define KEY_RED           7
#define KEY_GREEN         8
#define KEY_YELLOW        9
#define KEY_BLUE         10
#define KEY_0            11
#define KEY_1            12
#define KEY_2            13
#define KEY_3            14
#define KEY_4            15
#define KEY_5            16
#define KEY_6            17
#define KEY_7            18
#define KEY_8            19
#define KEY_9            20
#define KEY_NONE     0x1000

#define COL_BGR           0
#define COL_WHT           1
#define COL_RED           2
#define COL_CYN           3
#define COL_MAG           4
#define COL_GRN           5
#define COL_BLU           6
#define COL_YEL           7
#define COL_TRP          15

#define PAL_VDR           0
#define PAL_C64           1

#endif // _DEFINES_H_
