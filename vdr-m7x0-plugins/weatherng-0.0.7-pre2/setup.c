#include "setup.h"

cWetterSetup wetterSetup;

cWetterSetup::cWetterSetup(void)
{
	w_update = 0;
#ifdef HAVE_4MB
	// 4 MB FF-CARD
	////	w_left         =  50;
	////	w_top          =  28;
	////	w_width        = 624;
	////	w_height       = 480;
	////	w_hicolor      = 1;
	w_left = 150;
	w_top = 120;
	w_width = 420;
	w_height = 336;
	w_hicolor = 0;
#else
	// 2 MD FF-CARD
	w_left = 150;
	w_top = 120;
	w_width = 420;
	w_height = 336;
#endif
	w_fontsize = 1;
	w_theme = 2;
	w_osdoffset_x = 0;
	w_osdoffset_y = 0;
	w_inverted = 1;
	w_corner = 1;
	strcpy(st_data1,"GMXX0040");
	strcpy(st_data2,"GMXX0040");
	strcpy(st_data3,"GMXX0040");
	st_pic0=NULL;
	st_pic1=NULL;
	st_pic2=NULL;
	st_pic3=NULL;
	st_pic4=NULL;
}

const cWetterTheme wetterTheme[eWetterThemeMaxNumber] = { {
// Classic OK
		0x7F000000, //clrBackground
		0xFF00FCFC, //clrBgBorder
		0xFF000000, //clrFgBorder
		0xFFFCFCFC, //clrFgText
		0xFFFFFFC0, //clrHiTemp
	},
	{ // enElchi  OK
		0xC8000066, //clrBackground
		0xC833AAEE, //clrBgBorder
		0xFF000000, //clrFgBorder
		0xFFFFFFFF, //clrFgText
		0xFFCE6100, //clrHiTemp
	},
	{ // Moronimo OK
		0xDF294A6B, //clrBackground
		0xCF3E5578, //clrBgBorder
		0xFF9BBAD7, //clrFgBorder
		0xFF9BBAD7, //clrFgText
		0xFFFFFFC0, //clrHiTemp
	},
	{ // DeepBlue OK
		0xC80C0C0C, //clrBackground
		0xC832557A, //clrBgBorder
		0xFF000000, //clrFgBorder
		0xFF9A9A9A, //clrFgText
		0xFFFFFFC0, //clrHiTemp
	},
	{ // Enigma OK
		0xD0DEE5FA, //clrBackground
		0xD04158BC, //clrBgBorder
		0xFFFFFFFF, //clrFgBorder
		0xFF000000, //clrFgText
		0xFFCE6100, //clrHiTemp
	},
	{ // SilverGreen OK
		0xD9526470, //clrBackground
		0xD9293841, //clrBgBorder
		0xD9B3BDCA, //clrFgBorder
		0xD9B3BDCA, //clrFgText
		0xFFCE7B00, //clrHiTemp
	},
	{ // MoronimoMKII OK
		0xEFDDE2E7, //clrBackground
		0xEFBBC9E3, //clrBgBorder
		0xFFF7F9FD, //clrFgBorder
		0xFF707880, //clrFgText
		0xFFCE6100, //clrHiTemp
	},
};
