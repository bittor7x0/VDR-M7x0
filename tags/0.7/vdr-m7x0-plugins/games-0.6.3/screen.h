#ifndef _SCREEN_H_
#define _SCREEN_H_

namespace Scr {

int open(int x, int y, int w, int h);
int pixel(int x, int y, int idx);
int pixel(int x, int y);
int update(void);
int height(void);
int width(void);
int close(void);

} // namespace

#endif // _SCREEN_H_
