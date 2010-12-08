#include "screen.h"

#include "font.h"
#include "text.h"

namespace Txt {

int
height(void) {
	return (FontHeight);
}

int
width(const char *txt) {
	int w = 0;

	while (txt && *txt) w += width(*txt++);
	return (w);
}

int
width(char c) {
	return ((int)FontData[c-32][0]);
}

int
print(int x, int y, const char *txt, int fg, int bg) {
	unsigned int pxl;

	while (txt && *txt) {
		if ((int)(x+width(*txt)) > Scr::width()) break;
		for (int row=0; row<height(); row++) {
			pxl = FontData[*txt-32][row+1];
			// XXX maybe inrement col befor starting?
			for (int col=width(*txt); col>0; col--) {
				Scr::pixel(x+col, y+row, (pxl&1) ? fg : bg);
				pxl >>= 1; 
			}
		}
		x += width(*txt++);
	}
	return (0);
}

} // namespace
