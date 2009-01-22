#include <vector>
#include <string>
#include <vdr/plugin.h>
#include <vdr/osd.h>
#include <vdr/config.h>
#include "OsdWeather.h"
#include "parsing.h"
#include "img/bitmap.h"
#include "vars.h"
#include "i18n.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "http.h"

#include "symbols/left1.xpm"
#include "symbols/right1.xpm"
#include "symbols/day1.xpm"
#include "symbols/night1.xpm"
#include "symbols/left.xpm"
#include "symbols/right.xpm"
#include "symbols/day.xpm"
#include "symbols/night.xpm"

#define TRUE 1
#define FALSE 0
#define BOOL int
#define OSDWIDTH 600
#define OSDHEIGHT (Setup.OSDHeight + wetterSetup.w_osdoffset_y)

cBitmap cWetterOsd::bmLeft(left1_xpm);
cBitmap cWetterOsd::bmRight(right1_xpm);
cBitmap cWetterOsd::bmDay(day1_xpm);
cBitmap cWetterOsd::bmNight(night1_xpm);
cBitmap cWetterOsd::bmLeft_inv(left_xpm);
cBitmap cWetterOsd::bmRight_inv(right_xpm);
cBitmap cWetterOsd::bmDay_inv(day_xpm);
cBitmap cWetterOsd::bmNight_inv(night_xpm);

char *Radarmap;

const int MAX_LOGO_COLORS=16;
const cFont *font;

int part=0;

int colordepth = 16;
int areadepth = 4;

bool IsSatelite, satelite;

static int if_exist_file(const char *filename) {
	FILE *f = fopen(filename, "r");
	if (f != NULL) {
		fclose(f);
		return TRUE;
	} else
		return FALSE;
}

cWetterOsd::cWetterOsd(void)
{
	osd = NULL;
	day = 1;
	IsSatelite = false;
	Radar_left = wetterSetup.w_left;
	Radar_top = wetterSetup.w_top;
	Radar_width = wetterSetup.w_width;
	Radar_height = wetterSetup.w_height;
#ifdef HAVE_4MB
	hicolor = wetterSetup.w_hicolor;
#endif
	row_y = 30;
	fontsize = wetterSetup.w_fontsize;
	inverted = wetterSetup.w_inverted;
	corner = wetterSetup.w_corner;
	data_file = "/data1.xml";
}

cWetterOsd::~cWetterOsd(void)
{
	if (osd)
	delete osd;

}

eOSState cWetterOsd::ProcessKey(eKeys Key)
{
	eOSState state = cOsdObject::ProcessKey(Key);
	if (state == osUnknown) {
		switch (Key & ~k_Repeat) {
			case kOk:
			if(IsSatelite==true)
			{
				IsSatelite = false;
				cWetterOsd::Show();
			}
			else
			{
				file = ImageDir;
#ifdef HAVE_MAGICK
				file = file + "/images/wait.png";
#else
#ifdef HAVE_IMLIB2
				file = file + "/images/wait.png";
#else
				file = file + "/images/wait.xpm";
#endif
#endif

#ifdef HAVE_4MB
				if (hicolor==false) {
					colordepth = 16;
				}
				else {
					colordepth = 256;
				}
#else
				colordepth = 16;
#endif

#ifdef HAVE_MAGICK
				cBitmap *b = LoadMagick(file.c_str(),196,196,colordepth,true);
#else
#ifdef HAVE_IMLIB2
				cBitmap *b = LoadImlib(file.c_str(),196,196,colordepth,true);
#else
				cBitmap *b = LoadXpm(file.c_str());
#endif
#endif

				if (b) {
					osd->DrawBitmap((OSDWIDTH-196) /2,((OSDHEIGHT/2)-196)/2 ,*b,clrTransparent,clrTransparent);
					delete b;}
				osd->Flush();
				cWetterOsd::GetData();
				cWetterOsd::Show();
			}
			break;
			case kLeft:
			day--;
			if (day<1) {
				day=1;
			}
			cWetterOsd::Show();
			break;
			case kUp:
			part++;
			if (part>1) {
				part=1;
			}
			cWetterOsd::Show();
			break;
			case kDown:
			part--;
			if (part<0) {
				part=0;
			}
			cWetterOsd::Show();
			break;
			case kRight:
			day++;
			if (day>10) {
				day=10;
			}
			cWetterOsd::Show();
			break;
			case kBack:
			if(IsSatelite==true) {
				IsSatelite=false;
				cWetterOsd::Show();
			}
			else {
				return osEnd;
			}
			break;
			case k1:
			if(IsSatelite==true) {
				IsSatelite = false;}
			data_file = "/data1.xml";
			cWetterOsd::Show();
			break;
			case k2:
			if(IsSatelite==true) {
				IsSatelite = false;}
			data_file = "/data2.xml";
			cWetterOsd::Show();
			break;
			case k3:
			if(IsSatelite==true) {
				IsSatelite =false;}
			data_file = "/data3.xml";
			cWetterOsd::Show();
			break;
			case k5:
			IsSatelite=true;
#ifdef HAVE_MAGICK
			Radarmap="/pic0";
#else
#ifdef HAVE_IMLIB2
			Radarmap="/pic0";
#else
			Radarmap="/pic0.xpm";
#endif
#endif
			cWetterOsd::Satelite();
			break;
			case kRed:
			IsSatelite=true;
#ifdef HAVE_MAGICK
			Radarmap="/pic1";
#else
#ifdef HAVE_IMLIB2
			Radarmap="/pic1";
#else
			Radarmap="/pic1.xpm";
#endif
#endif
			cWetterOsd::Satelite();
			break;
			case kGreen:
			IsSatelite=true;
#ifdef HAVE_MAGICK
			Radarmap="/pic2";
#else
#ifdef HAVE_IMLIB2
			Radarmap="/pic2";
#else
			Radarmap="/pic2.xpm";
#endif
#endif
			cWetterOsd::Satelite();
			break;
			case kYellow:
			IsSatelite=true;
#ifdef HAVE_MAGICK
			Radarmap="/pic3";
#else
#ifdef HAVE_IMLIB2
			Radarmap="/pic3";
#else
			Radarmap="/pic3.xpm";
#endif
#endif
			cWetterOsd::Satelite();
			break;
			case kBlue:
			IsSatelite=true;
#ifdef HAVE_MAGICK
			Radarmap="/pic4";
#else
#ifdef HAVE_IMLIB2
			Radarmap="/pic4";
#else
			Radarmap="/pic4.xpm";
#endif
#endif
			cWetterOsd::Satelite();
			break;
			default: return state;
		}
		state = osContinue;
	}
	return state;
}

void cWetterOsd::GetData(void)
{
	char *buffer;
	char *url;

	asprintf(&buffer, "%s%s",ScriptDir,"/data1.xml");
//	asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data1,"?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494");
	asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data1,"?cc=*&unit=m&dayf=10");
	dsyslog("DEBUG : weatherng: %s => %s",url, buffer);
	httpGet(url,buffer);
	free(buffer);
	free(url);

	asprintf(&buffer, "%s%s",ScriptDir,"/data2.xml");
//	asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data2,"?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494");
	asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data2,"?cc=*&unit=m&dayf=10");
	dsyslog("DEBUG : weatherng: %s => %s",url, buffer);
	httpGet(url,buffer);
	free(buffer);
	free(url);

	asprintf(&buffer, "%s%s",ScriptDir,"/data3.xml");
//	asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data3,"?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494");
	asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data3,"?cc=*&unit=m&dayf=10");
	dsyslog("DEBUG : weatherng: %s => %s",url, buffer);
	httpGet(url,buffer);
	free(buffer);
	free(url);

	if (wetterSetup.st_pic0!=NULL){
		asprintf(&buffer, "%s%s",ScriptDir,"/pic0.xpm");
		dsyslog("DEBUG : weatherng: %s => %s",wetterSetup.st_pic0, buffer);
		httpGet(wetterSetup.st_pic0,buffer);
		free(buffer);
	}

	if (wetterSetup.st_pic1!=NULL){
		asprintf(&buffer, "%s%s",ScriptDir,"/pic1.xpm");
		dsyslog("DEBUG : weatherng: %s => %s",wetterSetup.st_pic1, buffer);
		httpGet(wetterSetup.st_pic1,buffer);
		free(buffer);
	}

	if (wetterSetup.st_pic2!=NULL){
		asprintf(&buffer, "%s%s",ScriptDir,"/pic2.xpm");
		dsyslog("DEBUG : weatherng: %s => %s",wetterSetup.st_pic2, buffer);
		httpGet(wetterSetup.st_pic2,buffer);
		free(buffer);
	}

	if (wetterSetup.st_pic3!=NULL){
		asprintf(&buffer, "%s%s",ScriptDir,"/pic3.xpm");
		dsyslog("DEBUG : weatherng: %s => %s",wetterSetup.st_pic3, buffer);
		httpGet(wetterSetup.st_pic3,buffer);
		free(buffer);
	}

	if (wetterSetup.st_pic4!=NULL){
		asprintf(&buffer, "%s%s",ScriptDir,"/pic4.xpm");
		dsyslog("DEBUG : weatherng: %s => %s",wetterSetup.st_pic4, buffer);
		httpGet(wetterSetup.st_pic4,buffer);
		free(buffer);
	}


}

int cWetterOsd::Satelite(void)
{
	delete osd;
	osd = cOsdProvider::NewOsd(Radar_left ,Radar_top );
	satelite=false;
	if (osd) {

#ifdef HAVE_4MB
		if (hicolor==false) {
			areadepth = 4;
			colordepth = 16;
		}
		else {
			areadepth = 8;
			colordepth = 256;
		}
#else
		areadepth = 4;
		colordepth = 16;

#endif
		//                esyslog("left=%i top=%i witdh=%i height=%i",Radar_left,Radar_top,Radar_width,Radar_height);

		tArea Area = {1,1,Radar_width,Radar_height,areadepth};
		eOsdError result = osd->CanHandleAreas(&Area, 1);
		if (result == oeOk) {
			osd->SetAreas(&Area, 1);
		}
		else
		esyslog("ERROR : %s (satelite): OSD open failed! Can't handle areas (%d)\n",PLUGIN_NAME_I18N,result);

		std::string sat_dir;
		sat_dir = DataDir;
		sat_dir = sat_dir + Radarmap;

		BOOL ex = if_exist_file(sat_dir.c_str());

		if ( ex ) {

#ifdef HAVE_MAGICK
			cBitmap *b = LoadMagick(sat_dir.c_str(),Radar_height - 2,Radar_width - 2, colordepth, true);
#else
#ifdef HAVE_IMLIB2
			cBitmap *b = LoadImlib(sat_dir.c_str(),Radar_height - 2,Radar_width - 2, colordepth, true);
#else
			cBitmap *b = LoadXpm(sat_dir.c_str());
#endif
#endif

			if (b) {
				osd->DrawBitmap(2, 2, *b, clrTransparent, clrTransparent);
			}
			delete b;

			// debug

			dsyslog("DEBUG : weatherng: SATELITE\n");
#ifdef HAVE_MAGICK
			dsyslog("DEBUG : weatherng: USE IMAGEMAGICK\n");;
#endif
#ifdef HAVE_IMLIB2
			dsyslog("DEBUG : weatherng: USE IMLIB2\n");;
#endif
			dsyslog("DEBUG : weatherng: USE XPM\n");;
			dsyslog("DEBUG : weatherng: SCRIPTDIR  = %s\n", ScriptDir);
			dsyslog("DEBUG : weatherng: IMAGEDIR   = %s\n", ImageDir);
			dsyslog("DEBUG : weatherng: DATADIR   = %s\n", DataDir);
#ifdef HAVE_4MB
			dsyslog("DEBUG : weatherng: 256 COLORS ENABLED  = %i\n", hicolor);
#endif
			dsyslog("DEBUG : weatherng: AREADEPTH   = %i\n", areadepth);
			dsyslog("DEBUG : weatherng: COLORDEPTH = %i\n", colordepth);
			dsyslog("DEBUG : weatherng: RADAR_LEFT = %i\n", Radar_left);
			dsyslog("DEBUG : weatherng: RADAR_TOP  = %i\n", Radar_top);
			dsyslog("DEBUG : weatherng: RADAR_WIDTH  = %i\n", Radar_width);
			dsyslog("DEBUG : weatherng: RADAR_HEIGHT  = %i\n", Radar_height);

		} else {
			perror(sat_dir.c_str());

			sat_dir = ImageDir;
			sat_dir = sat_dir + "/images/nopic.png";
#ifdef HAVE_MAGICK
			sat_dir = ImageDir;
			sat_dir = sat_dir + "/images/nopic.png";
			cBitmap *b = LoadMagick(sat_dir.c_str(),Radar_height - 2,Radar_width - 2, colordepth, true);
#else
#ifdef HAVE_IMLIB2
			sat_dir = ImageDir;
			sat_dir = sat_dir + "/images/nopic.png";
			cBitmap *b = LoadImlib(sat_dir.c_str(),Radar_height - 2,Radar_width - 2, colordepth, true);
#else
			sat_dir = ImageDir;
			sat_dir = sat_dir + "/images/nopic.xpm";
			cBitmap *b = LoadXpm(sat_dir.c_str());
#endif
#endif

			if (b) {
				osd->DrawBitmap(2, 2, *b, clrTransparent, clrTransparent);
			}
			delete b;

		}
		osd->Flush();
	}
	return EXIT_SUCCESS;

}

void cWetterOsd::Show(void)
{
	IsSatelite=false;
	dsyslog("DEBUG : weatherng: Part: %i\n",part);

	delete osd;
	osd = cOsdProvider::NewOsd(((Setup.OSDWidth -OSDWIDTH) / 2) + Setup.OSDLeft + wetterSetup.w_osdoffset_x, ((Setup.OSDHeight - OSDHEIGHT) / 2) + Setup.OSDTop);
	if (osd) {

		std::string ausgabe;
		std::string sat_file;
		sat_file = DataDir;
		sat_file = sat_file + data_file;

		if (!fontsize) {
			font = cFont::GetFont(fontOsd);
			row_y = 30;
		}
		else {
			font = cFont::GetFont(fontSml);
			row_y = 25;
		}

		int CELLWIDTH = (OSDWIDTH / 4)-1;

#ifdef HAVE_4MB
		if (hicolor==false) {
		    colordepth  =  16;
		    areadepth   =   4;
		}
		else {
		    colordepth  = 256;
		    areadepth   =   8;
		}
#else
		colordepth  =  16;
		areadepth   =   4;

#endif
		tArea Area[] = {
			{	0,0,OSDWIDTH ,OSDHEIGHT,areadepth}
		};

		//   Change for open7x0
		//		tArea Area[] = {
		//				{CELLWIDTH-1, 0, 3 * CELLWIDTH, (OSDHEIGHT - (7 * row_y)) -11, areadepth},
		//				{0, OSDHEIGHT - ((7 * row_y) +10), OSDWIDTH -1, OSDHEIGHT -((6 * row_y) +10), 2},
		//				{0, OSDHEIGHT - ((6 * row_y) +9), OSDWIDTH -1, OSDHEIGHT -(row_y +1), 2},
		//				{0, OSDHEIGHT - row_y, OSDWIDTH -1, OSDHEIGHT, 2},
		//				};

		eOsdError result = osd->CanHandleAreas(Area, sizeof(Area) / sizeof(tArea));
		if (result == oeOk) {
			osd->SetAreas(Area, sizeof(Area) / sizeof(tArea));
		}
		else
		esyslog("ERROR : %s: OSD open failed! Can't handle areas (%d)\n",PLUGIN_NAME_I18N, result);

		// icon
		osd->DrawRectangle(CELLWIDTH-1, 0, 3 * CELLWIDTH, (OSDHEIGHT / 2) - 1, clrTransparent);

		// Info top
		osd->DrawRectangle(0, OSDHEIGHT - ((7 * row_y)+10), OSDWIDTH, OSDHEIGHT - ((6 * row_y)+10), wetterTheme[wetterSetup.w_theme].clrBgBorder);
		// Mainfield left
		osd->DrawRectangle(3, OSDHEIGHT - ((6 * row_y) + 9), (OSDWIDTH / 2) -4 , OSDHEIGHT - row_y, wetterTheme[wetterSetup.w_theme].clrBackground);
		// Mainfield right
		osd->DrawRectangle((OSDWIDTH / 2)+2, OSDHEIGHT - ((6 * row_y) + 9), OSDWIDTH -3, OSDHEIGHT - row_y, wetterTheme[wetterSetup.w_theme].clrBackground);

		// BORDER
		// left OK    middle OK    right OK    horiz. OK
		osd->DrawRectangle(0, (OSDHEIGHT - ((6 * row_y)+10))+1, 3, OSDHEIGHT-row_y, wetterTheme[wetterSetup.w_theme].clrBgBorder);
		osd->DrawRectangle((OSDWIDTH / 2) -3, (OSDHEIGHT - ((6 * row_y)+10))+1, (OSDWIDTH / 2)+1,OSDHEIGHT-(2 * row_y), wetterTheme[wetterSetup.w_theme].clrBgBorder);
		osd->DrawRectangle(OSDWIDTH -4, (OSDHEIGHT - ((6 * row_y)+10))+1, OSDWIDTH, OSDHEIGHT-row_y, wetterTheme[wetterSetup.w_theme].clrBgBorder);
		osd->DrawRectangle(4, (OSDHEIGHT - (2 * row_y))-1, OSDWIDTH -5, (OSDHEIGHT - (2 *row_y)) +1, wetterTheme[wetterSetup.w_theme].clrBgBorder);

		// Info bottom
		osd->DrawRectangle(0, OSDHEIGHT - row_y, OSDWIDTH, OSDHEIGHT, wetterTheme[wetterSetup.w_theme].clrBgBorder);

		BOOL ex = if_exist_file(sat_file.c_str());

		if (ex) {
			dsyslog("DEBUG : weatherng: Parse %s !\n", sat_file.c_str());

			cxmlParse parser;

			dsyslog("DEBUG : weatherng: day: %i\n",day);

			parser.xmlParse((int) day, DataDir, data_file, part);

			if( strcmp(parser.ort.c_str(), "" ) != 0 ) {

#ifdef HAVE_MAGICK
				file = ImageDir;
				file = file + "/images/";
				file = file + parser.icon.c_str();
				file = file + ".png";
				cBitmap *b = LoadMagick(file.c_str(),196,196,colordepth,true);
#else
#ifdef HAVE_IMLIB2
				file = ImageDir;
				file = file + "/images/";
				file = file + parser.icon.c_str();
				file = file + ".png";
				cBitmap *b = LoadImlib(file.c_str(),196,196,colordepth,true);
#else
				file = ImageDir;
				file = file + "/images/";
				file = file + parser.icon.c_str();
				file = file + ".xpm";
				dsyslog("VERSUCHE XPM ZU LADEN");
				cBitmap *b = LoadXpm(file.c_str());
				dsyslog("XPM GELADEN");
#endif
#endif

				if (b) {
					dsyslog("LADE XPM");
					// top
					osd->DrawRectangle(((OSDWIDTH-196)/2)-4,(((OSDHEIGHT/2)-196)/2)-4,((OSDWIDTH-196)/2)+200,(((OSDHEIGHT/2)-196)/2), wetterTheme[wetterSetup.w_theme].clrBgBorder);
					// bottom (something to fix)
					osd->DrawRectangle(((OSDWIDTH-196)/2)-4,(((OSDHEIGHT/2)-196)/2)+128,((OSDWIDTH-196)/2)+200,(((OSDHEIGHT/2)-196)/2)+200, wetterTheme[wetterSetup.w_theme].clrBgBorder);
					// left
					osd->DrawRectangle(((OSDWIDTH-196)/2)-4,(((OSDHEIGHT/2)-196)/2),((OSDWIDTH-196)/2),(((OSDHEIGHT/2)-196)/2)+196, wetterTheme[wetterSetup.w_theme].clrBgBorder);
					// right
					osd->DrawRectangle(((OSDWIDTH-196)/2)+196,(((OSDHEIGHT/2)-196)/2),((OSDWIDTH-196)/2)+200,(((OSDHEIGHT/2)-196)/2)+196, wetterTheme[wetterSetup.w_theme].clrBgBorder);
					// Picture
					osd->DrawBitmap((OSDWIDTH-196) /2,((OSDHEIGHT/2)-196)/2 ,*b,clrTransparent,clrTransparent);
				}

				dsyslog("DEBUG : weatherng: Information about: %s\n",parser.ort.c_str());

				// left Side
				ausgabe = tr("Weather for: ");
				ausgabe = ausgabe + parser.ort.c_str() + "  /  ";
				ausgabe = ausgabe + tr(parser.dayname.c_str());
				ausgabe = ausgabe + "  (" + parser.date + ")";
				osd->DrawText(16, (OSDHEIGHT - ((7 * row_y)+10)) + 1,ausgabe.c_str(), wetterTheme[wetterSetup.w_theme].clrFgBorder,clrTransparent,font,OSDWIDTH -16,20,taCenter);

				osd->DrawText(8, OSDHEIGHT - ((6 * row_y)+5), tr("High temperature: "), wetterTheme[wetterSetup.w_theme].clrFgHiTemp,clrTransparent,font);
				ausgabe = parser.hi.c_str();
				ausgabe = ausgabe + " ";
				ausgabe = ausgabe + parser.celsius.c_str();
				osd->DrawText(CELLWIDTH - 8, OSDHEIGHT - ((6 * row_y)+5), ausgabe.c_str(), wetterTheme[wetterSetup.w_theme].clrFgHiTemp,clrTransparent,font,CELLWIDTH,20,taRight);

				osd->DrawText(8, OSDHEIGHT - ((5 * row_y)+5), tr("Low temperature: "), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				ausgabe = parser.low.c_str();
				ausgabe = ausgabe + " ";
				ausgabe = ausgabe + parser.celsius.c_str();
				osd->DrawText(CELLWIDTH - 8, OSDHEIGHT - ((5 * row_y)+5), ausgabe.c_str(), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font,CELLWIDTH,20,taRight);

				osd->DrawText(8, OSDHEIGHT - ((4 * row_y)+5), tr("Sunrise: "), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				ausgabe = parser.sunrise.c_str();
				ausgabe = ausgabe;
				osd->DrawText(CELLWIDTH - 8, OSDHEIGHT - ((4 * row_y)+5), ausgabe.c_str(), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font,CELLWIDTH,20,taRight);

				osd->DrawText(8, OSDHEIGHT - ((3 * row_y)+5), tr("Sunset: "), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				ausgabe = parser.sunset.c_str();
				ausgabe = ausgabe;
				osd->DrawText(CELLWIDTH - 8, OSDHEIGHT - ((3 * row_y)+5), ausgabe.c_str(), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font,CELLWIDTH,20,taRight);

				ausgabe = parser.wetter;
				osd->DrawText(8, (OSDHEIGHT - (2 * row_y))+2,tr(ausgabe.c_str()), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font, OSDWIDTH-8, 20, taCenter);

				//right Side
				osd->DrawText((OSDWIDTH / 2)+8, OSDHEIGHT - ((6 * row_y)+5), tr("Wind comes from: "), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				ausgabe = parser.winddir.c_str();
				osd->DrawText(OSDWIDTH - CELLWIDTH -8 , OSDHEIGHT - ((6 * row_y)+5), ausgabe.c_str(), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font,CELLWIDTH,20,taRight);

				osd->DrawText((OSDWIDTH / 2)+8, OSDHEIGHT - ((5 * row_y)+5), tr("Wind speed: "), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				ausgabe = parser.windspeed.c_str();
				ausgabe = ausgabe + " ";
				ausgabe = ausgabe + parser.speed.c_str();
				osd->DrawText(OSDWIDTH - CELLWIDTH -8 , OSDHEIGHT - ((5 * row_y)+5), ausgabe.c_str(), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font,CELLWIDTH,20,taRight);

				osd->DrawText((OSDWIDTH / 2)+8, OSDHEIGHT - ((4 * row_y)+5), tr("Humitidy: "), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				ausgabe = parser.humidity.c_str();
				ausgabe = ausgabe + tr(" %");
				osd->DrawText(OSDWIDTH - CELLWIDTH -8 , OSDHEIGHT - ((4 * row_y)+5), ausgabe.c_str(), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font,CELLWIDTH,20,taRight);

				osd->DrawText((OSDWIDTH / 2)+8, OSDHEIGHT - ((3 * row_y)+5), tr("Precipitation: "), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				ausgabe = parser.raindown.c_str();
				ausgabe = ausgabe + tr(" %");
				osd->DrawText(OSDWIDTH - CELLWIDTH -8 , OSDHEIGHT - ((3 * row_y)+5), ausgabe.c_str(), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font,CELLWIDTH,20,taRight);

				//			font     =    cFont::GetFont(fontSml);

				if (part==0) {
					osd->DrawText(((OSDWIDTH-196)/2)-4, (((OSDHEIGHT/2)-196)/2)+204, tr("DAY"), wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder,font,204,row_y,taCenter);
					if (!inverted) {
						osd->DrawBitmap((OSDWIDTH / 2) - (bmNight.Width() / 2), (OSDHEIGHT - (row_y / 2)) - (bmNight.Height() / 2), bmNight, wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder);}
					else {
						osd->DrawBitmap((OSDWIDTH / 2) - (bmNight_inv.Width() / 2), (OSDHEIGHT - (row_y / 2)) - (bmNight_inv.Height() / 2), bmNight_inv, wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder);}
				}

				if (part==1) {
					osd->DrawText(((OSDWIDTH-196)/2)-4, (((OSDHEIGHT/2)-196)/2)+204, tr("NIGHT"), wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder,font,204,row_y,taCenter);
					if (!inverted) {
						osd->DrawBitmap((OSDWIDTH / 2) - (bmDay.Width() / 2), (OSDHEIGHT - (row_y / 2)) - (bmDay.Height() / 2), bmDay, wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder);}
					else {
						osd->DrawBitmap((OSDWIDTH / 2) - (bmDay_inv.Width() / 2), (OSDHEIGHT - (row_y / 2)) - (bmDay_inv.Height() / 2), bmDay_inv, wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder);}
				}

				if (day>1) {
					if (!inverted) {
						osd->DrawBitmap(22, (OSDHEIGHT - (row_y / 2)) - (bmLeft.Height() / 2), bmLeft, wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder);}
					else {
						osd->DrawBitmap(22, (OSDHEIGHT - (row_y / 2)) - (bmLeft_inv.Height() / 2), bmLeft_inv, wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder);}
				}

				if (day<10) {
					if (!inverted) {
						osd->DrawBitmap(OSDWIDTH - bmRight.Width() -22 , (OSDHEIGHT - (row_y / 2)) - (bmRight.Height() / 2), bmRight, wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder);}
					else {
						osd->DrawBitmap(OSDWIDTH - bmRight_inv.Width() -22 , (OSDHEIGHT - (row_y / 2)) - (bmRight_inv.Height() / 2), bmRight_inv, wetterTheme[wetterSetup.w_theme].clrFgBorder, wetterTheme[wetterSetup.w_theme].clrBgBorder);}
				}

				if ( corner == true ) {
					// Ellipse top left
					osd->DrawEllipse(0, OSDHEIGHT - ((7 * row_y)+10), 12, (OSDHEIGHT - ((7 * row_y)+10)) +12, clrTransparent, -2);
					// Ellipse top right
					osd->DrawEllipse(OSDWIDTH -12, OSDHEIGHT - ((7 * row_y)+10), OSDWIDTH, (OSDHEIGHT - ((7 * row_y)+10))+12, clrTransparent, -1);
					// Ellipse bottom left
					osd->DrawEllipse(0, OSDHEIGHT -12, 12, OSDHEIGHT, clrTransparent, -3);
					// Ellipse bottom right
					osd->DrawEllipse(OSDWIDTH -12, OSDHEIGHT -12, OSDWIDTH, OSDHEIGHT, clrTransparent, -4);
					// Ellipse day/night left
					osd->DrawEllipse(((OSDWIDTH-196)/2)-4, (((OSDHEIGHT/2)-196)/2)+204, ((OSDWIDTH-196)/2)-4+row_y, (((OSDHEIGHT/2)-196)/2)+204+row_y, clrTransparent, -3);
					// Ellipse day/night right
					osd->DrawEllipse(((OSDWIDTH-196)/2)+200-row_y, (((OSDHEIGHT/2)-196)/2)+204, ((OSDWIDTH-196)/2)+200, (((OSDHEIGHT/2)-196)/2)+204+row_y, clrTransparent, -4);
				}

				if (b) {
					delete b;
				}

				osd->Flush();

			} else {
				perror(sat_file.c_str());

				osd->DrawText(22, OSDHEIGHT - (7 * row_y), tr("ERROR : NO VAILD DATA"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				osd->DrawText(22, OSDHEIGHT - (6 * row_y), tr("PRESS OK to download/update data"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);

				osd->DrawText(22, OSDHEIGHT - (5 * row_y), tr("Path to weatherdata is :"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				osd->DrawText(22, OSDHEIGHT - (4 * row_y), DataDir, wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);

				osd->DrawText(22, OSDHEIGHT - (3 * row_y), tr("Do you have vaild values in setup ?"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				//osd->DrawText(22, OSDHEIGHT - (2 * row_y), tr("Path to weatherng.sh is :"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
				//osd->DrawText(22, OSDHEIGHT - row_y, ScriptDir, wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);

				esyslog("ERROR : weatherng: Nowalid city name in XML !\n");
				osd->Flush();
			}
		} else {
			perror(sat_file.c_str());

			osd->DrawText(22, OSDHEIGHT - (7 * row_y), tr("ERROR : NO DATA"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
			osd->DrawText(22, OSDHEIGHT - (6 * row_y), tr("PRESS OK to download/update data"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);

			osd->DrawText(22, OSDHEIGHT - (5 * row_y), tr("Path to weatherdata is :"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
			osd->DrawText(22, OSDHEIGHT - (4 * row_y), DataDir, wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);

			osd->DrawText(22, OSDHEIGHT - (3 * row_y), tr("Please check IP/Gate configuration"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
			//osd->DrawText(22, OSDHEIGHT - (2 * row_y), tr("Path to weatherng.sh is :"), wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);
			//osd->DrawText(22, OSDHEIGHT - row_y, ScriptDir, wetterTheme[wetterSetup.w_theme].clrFgText,clrTransparent,font);

			esyslog("ERROR : weatherng: %s don't exist !\n", sat_file.c_str());
			osd->Flush();
		}
	}

	// debug

	dsyslog("DEBUG : weatherng: SHOW\n");
#ifdef HAVE_MAGICK
	dsyslog("DEBUG : weatherng: USE IMAGEMAGICK\n");;
#endif
#ifdef HAVE_IMLIB2
	dsyslog("DEBUG : weatherng: USE IMLIB2\n");;
#endif
	dsyslog("DEBUG : weatherng: SCRIPTDIR  = %s\n", ScriptDir);
	dsyslog("DEBUG : weatherng: IMAGEDIR   = %s\n", ImageDir);
	dsyslog("DEBUG : weatherng: DATADIR   = %s\n", DataDir);
#ifdef HAVE_4MB
	dsyslog("DEBUG : weatherng: 256 COLORS ENABLED  = %i\n", hicolor);
#endif
	dsyslog("DEBUG : weatherng: AREADEPTH   = %i\n", areadepth);
	dsyslog("DEBUG : weatherng: COLORDEPTH = %i\n", colordepth);
	dsyslog("DEBUG : weatherng: RADAR_LEFT = %i\n", Radar_left);
	dsyslog("DEBUG : weatherng: RADAR_TOP  = %i\n", Radar_top);
	dsyslog("DEBUG : weatherng: RADAR_WIDTH  = %i\n", Radar_width);
	dsyslog("DEBUG : weatherng: RADAR_HEIGHT  = %i\n", Radar_height);

	//return EXIT_SUCCESS;
}
