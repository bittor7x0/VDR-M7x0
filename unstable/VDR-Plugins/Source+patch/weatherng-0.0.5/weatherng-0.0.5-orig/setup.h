
#ifndef __SETUP_H
#define __SETUP_H


#include <ctype.h>
#include <climits>
#include <vector>
#include <string>
#include <vdr/plugin.h>

enum eWetterModes
{
 eWetterModeMaxNumber
}; 

struct cWetterSetup
{
public:
 cWetterSetup(void);
 int w_update;
 int w_left;
 int w_top;
 int w_width;
 int w_height;
#ifdef HAVE_4MB
 int w_hicolor;
#endif
 int w_fontsize;
 int w_theme;
 int w_osdoffset_x;
 int w_osdoffset_y;
 int w_inverted;
 int w_corner;
 char st_data1[15];
 char st_data2[15];
 char st_data3[15];
 char *st_pic0;
 char *st_pic1;
 char *st_pic2;
 char *st_pic3;
 char *st_pic4;
};

extern cWetterSetup wetterSetup;

enum eWetterThemes
{
 eWetterThemeClassic,
 eWetterThemeenElchi,
 eWetterThemeMoronimo,
 eWetterThemeDeepBlue,
 eWetterThemeEnigma,
 eWetterThemeSilverGreen,
 eWetterThemeMoronimoMKII,
 eWetterThemeMaxNumber
 };

struct cWetterTheme
{
 int clrBackground;
 int clrBgBorder;
 int clrFgBorder;
 int clrFgText;
 int clrFgHiTemp;
};

extern const cWetterTheme wetterTheme[eWetterThemeMaxNumber];

#endif //__SETUP_H 
