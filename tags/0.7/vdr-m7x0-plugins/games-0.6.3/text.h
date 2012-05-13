#ifndef _TEXT_H_
#define _TEXT_H_

namespace Txt {

int height(void);
int width(const char *txt);
int width(char c);
int print(int x, int y, const char *txt, int fg, int bg=0);

} // namespace

#endif
