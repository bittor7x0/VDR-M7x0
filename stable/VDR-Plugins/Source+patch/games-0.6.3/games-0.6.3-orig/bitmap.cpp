#include <stdlib.h> // malloc(), free()
#include <stdio.h>  // sscanf()

#include "screen.h"
#include "bitmap.h"

Bitmap::Bitmap(int w, int h) {
	if (w>0 && h>0) {
		width = w;
		height = h;
		data = (char *)calloc(width*height, sizeof (char));
	}
}

Bitmap::Bitmap(char *xpm[]) {
	int colors, pix;

	sscanf(xpm[0], "%d %d %d %d", &height, &width, &colors, &pix);
	data = (char *)calloc(width*height, sizeof (char));
	if (data) {
		for (int y=colors+1; y<colors+1+height; y++) {
			for (int x=0; x<width; x++) {
				for (int i=0; i<colors; i++) {
					if (xpm[i+1][0] == xpm[y][x]) {
						data[width * y + x] = i;
						break;
					}
				}
			}
		}
	}
}

Bitmap::~Bitmap(void) {
	free(data);
}

int
Bitmap::pixel(int x, int y, int idx) {
	if (data && 0<=x && x<width && 0<=y && y<height) {
		data[width * y + x] = idx;
		return (0);
	}
	return (-1);
}

int
Bitmap::pixel(int x, int y) {
	if (data && x>=0 && x<width && y>=0 && y<height) {
		return (data[width * y + x]);
	}
	return (-1);
}

int
Bitmap::blit(int x, int y) {
	if (data && x>=0 && x<Scr::width() && y>=0 && y<Scr::height()) {
		for (int ix=0; ix<width; ix++) {
			for (int iy=0; iy<height; iy++) {
				Scr::pixel(x + ix, y + iy, data[width * iy + ix]);
			}
		}
		return (0);
	}
	return (-1);
}
