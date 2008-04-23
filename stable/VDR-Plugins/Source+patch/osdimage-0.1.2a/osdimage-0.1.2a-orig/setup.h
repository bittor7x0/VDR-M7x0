// setup.h, brougs78

#ifndef _OSDIMAGE_SETUP_H_
#define _OSDIMAGE_SETUP_H_

#include <vdr/plugin.h>

//const int goes here

struct cOSDImageSetup {
	cOSDImageSetup(void);

	bool SetupParse(const char *Name, const char *Value);

	int picAlpha;
	int backgroundAlpha;
	int textAlpha;
	int slideShowDelay;
	int dither; // 0|false = no, 1|true = yes
	int quantizer; // 0 = ImageMagick, 1 = Netpbm
	bool have256colors;
};

extern cOSDImageSetup OSDImageSetup;


class cOSDImageSetupPage : public cMenuSetupPage {
private:
  cOSDImageSetup myOSDImageSetup;

public:
	cOSDImageSetupPage(void);
	virtual ~cOSDImageSetupPage();

	virtual void Store(void);
};

#endif

