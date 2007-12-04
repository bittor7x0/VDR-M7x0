// setup.c, brougs78


#include "setup.h"


cOSDImageSetup OSDImageSetup;


// --- cOSDImageSetup ----------------------------------------------------------

cOSDImageSetup::cOSDImageSetup(void)
{
	picAlpha = 0xFF;
	backgroundAlpha = 0xD2;
	textAlpha = 0xD2;
	slideShowDelay = 5;
	quantizer = 0; // use ImageMagick as default
	dither = true;
	have256colors = false;
}


bool cOSDImageSetup::SetupParse(const char *Name, const char *Value)
{
	if      (strcmp(Name, "picAlpha")        == 0) picAlpha        = atoi(Value);
	else if (strcmp(Name, "backgroundAlpha") == 0) backgroundAlpha = atoi(Value);
	else if (strcmp(Name, "textAlpha")       == 0) textAlpha       = atoi(Value);
	else if (strcmp(Name, "slideShowDelay")  == 0) slideShowDelay  = atoi(Value);
	else if (strcmp(Name, "quantizer")       == 0) quantizer       = atoi(Value);
	else if (strcmp(Name, "dither")          == 0) dither          = atoi(Value);
	else if (strcmp(Name, "have256colors")   == 0) have256colors   = atoi(Value);
	else return false;

#ifndef HAVE_NETPBM
	// reset quantizer to 'ImageMagick'
	quantizer = 0;
#endif
	
	return true;
}


// --- cOSDImageSetupPage ------------------------------------------------------

cOSDImageSetupPage::cOSDImageSetupPage(void)
{
	myOSDImageSetup = OSDImageSetup;
	
#ifdef HAVE_NETPBM
	Add(new cMenuEditBoolItem(tr("Quantizer"), &myOSDImageSetup.quantizer, "ImageMagick", "Netpbm" ) );
#endif
	Add(new cMenuEditBoolItem(tr("Activate Dithering"), &myOSDImageSetup.dither, tr("no"), tr("yes") ) );
	Add(new cMenuEditIntItem(tr("Delay for the Slideshow [s]"), &myOSDImageSetup.slideShowDelay, 1, 15) );
	Add(new cMenuEditIntItem(tr("Alpha-value picture"), &myOSDImageSetup.picAlpha, 0, 0xFF));
	
	if (OSDImageSetup.have256colors)
	{
		Add(new cMenuEditIntItem(tr("Alpha-value background"), &myOSDImageSetup.backgroundAlpha, 0, 0xFF) );
	}
	
	Add(new cMenuEditIntItem(tr("Alpha-value text-area"), &myOSDImageSetup.textAlpha, 0, 0xFF) );
}


cOSDImageSetupPage::~cOSDImageSetupPage()
{
}


void cOSDImageSetupPage::Store(void)
{
	OSDImageSetup = myOSDImageSetup;

	SetupStore("picAlpha", OSDImageSetup.picAlpha);
	SetupStore("backgroundAlpha", OSDImageSetup.backgroundAlpha);
	SetupStore("textAlpha", OSDImageSetup.textAlpha);
	SetupStore("slideShowDelay", OSDImageSetup.slideShowDelay);
	SetupStore("quantizer", OSDImageSetup.quantizer);
	SetupStore("dither", OSDImageSetup.dither);
	SetupStore("have256colors", OSDImageSetup.have256colors);
}

