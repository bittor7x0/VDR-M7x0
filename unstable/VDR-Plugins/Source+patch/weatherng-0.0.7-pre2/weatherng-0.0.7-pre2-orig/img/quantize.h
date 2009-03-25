#ifndef VDR_WEATHERNG_QUANTIZE_H
#define VDR_WEATHERNG_QUANTIZE_H

#define MAXCOLOR 256

class cQuantize
{
protected:
	unsigned int paletteOutput[MAXCOLOR];
	unsigned char imageOutput[1024*1024];
public:
	cQuantize(){};
	virtual ~cQuantize(){};
	virtual int Quantize(unsigned char * input, int size, int colors) = 0;
	unsigned int * OutputPalette() { return paletteOutput; }
	unsigned char * OutputImage() { return imageOutput; }
};

#define RED   2
#define	GREEN 1
#define BLUE  0

//#define NOINVERT        // RGB or BGR

struct box
{
	int r0;
	int r1;
	int g0;
	int g1;
	int b0;
	int b1;
	int vol;
};

// Histogram is in elements 1..HISTSIZE along each axis,
// element 0 is for base or marginal value.
// NB: these must start out 0!
#define BOX 33

class cQuantizeWu : public cQuantize
{
private:
	long wt[BOX][BOX][BOX];
	long mr[BOX][BOX][BOX];
	long mg[BOX][BOX][BOX];
	long mb[BOX][BOX][BOX];
	float m2[BOX][BOX][BOX];

	unsigned short * Qadd;

	unsigned char * imageInput;
	int imageSize;
	int palSize;

	void Hist3d(long *vwt, long *vmr, long *vmg, long *vmb, float *m_2);
	void Momt3d(long *vwt, long *vmr, long *vmg, long *vmb, float *m_2);
	int Cut(struct box * set1, struct box * set2);
	long Vol(struct box * cube, long mmt[BOX][BOX][BOX]);
	float Maximize(struct box * cube, unsigned char dir, int first, int last, int * cut, long whole_r, long whole_g, long whole_b, long whole_w);
	float Var(struct box * cube);
	long Top(struct box * cube, unsigned char dir, int pos, long mmt[BOX][BOX][BOX]);
	long Bottom(struct box * cube, unsigned char dir, long mmt[BOX][BOX][BOX]);
	void Mark(struct box * cube, int label, unsigned char * tag);
public:
	cQuantizeWu();
	virtual ~cQuantizeWu();
	virtual int Quantize(unsigned char * input, int size, int colors);
};

#endif // VDR_WEATHERNG_QUANTIZE_H
