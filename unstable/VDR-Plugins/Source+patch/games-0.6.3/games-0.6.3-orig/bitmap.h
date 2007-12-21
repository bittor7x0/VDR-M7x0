#ifndef _BITMAP_H_
#define _BITMAP_H_

class Bitmap {

public:

	Bitmap(int w, int h);
	Bitmap(char *xpm[]);

	~Bitmap(void);

	int pixel(int x, int y, int idx);
	int pixel(int x, int y);
	int blit(int x, int y);

	char *data;
	int width;
	int height;

};

#endif // _BITMAP_H_
