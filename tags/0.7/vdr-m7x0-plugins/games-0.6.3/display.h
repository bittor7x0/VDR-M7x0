#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "bitmap.h"

namespace Dpy {

int open(int x, int y, int w, int h, bool frame=true);
int close(void);
int width(void);
int height(void);
int update(void);

int text_width(const char *txt);
int text_width(char c);
int text_height(void);
int text_print(int x, int y, const char *txt, int fg, int bg=0);
int pixel_set(int x, int y, int col=1);
int bitmap_blit(Bitmap *bmp, int x, int y);
int rect_fill(int x1, int y1, int x2, int y2, int col=0);
int line_draw(int x1, int y1, int x2, int y2, int col=1);
int status_left(const char *txt, int col=1);
int status_left(int i, int col=1);
int status_center(const char *txt, int col=1);
int status_center(int i, int col=1);
int status_right(const char *txt, int col=1);
int status_right(int i, int col=1);
int message_show(const char *txt1, const char *txt2, int fg, int bg=15);

} // namespace

#endif // _DISPLAY_H_
