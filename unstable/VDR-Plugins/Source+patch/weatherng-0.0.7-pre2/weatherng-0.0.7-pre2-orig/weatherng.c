/*
 * wetter.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: wetter.c,v 1.3 2005/01/23 00:56:49 beejay Exp $
 */

#include <getopt.h>
#include "setup.h"
#include "weatherng.h"
#include "vars.h"
#include "http.h"

#define DEFAULT_CHARS "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ-_@."

cPluginWetter::cPluginWetter(void)
{
	// Initialize any member variables here.
	// DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
	// VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginWetter::~cPluginWetter()
{
	// Clean up after yourself!
	if (wetterSetup.st_pic0!=NULL){
		free(wetterSetup.st_pic0);
	}
	if (wetterSetup.st_pic1!=NULL){
		free(wetterSetup.st_pic1);
	}
	if (wetterSetup.st_pic2!=NULL){
		free(wetterSetup.st_pic2);
	}
	if (wetterSetup.st_pic3!=NULL){
		free(wetterSetup.st_pic3);
	}
	if (wetterSetup.st_pic4!=NULL){
		free(wetterSetup.st_pic4);
	}
}

const char *cPluginWetter::CommandLineHelp(void)
{
	static char *help_str = NULL;

	free(help_str);
	asprintf(&help_str,
			"  -D DIR    --data=DIR    DIR to datafiles of plugin.  e.g. /etc/plugins/weatherng\n"
			"  -I DIR    --images=DIR  DIR to imagefolder of plugin e.g. /etc/plugins/weatherng\n"
			"  -S DIR    --script=DIR  DIR to weatherng.sh  e.g. /usr/local/bin\n");
	return help_str;
}

bool cPluginWetter::ProcessArgs(int argc, char *argv[]) {
	static struct option long_options[] = {
		{	"data", required_argument, NULL, 'D'},
		{	"images", required_argument, NULL, 'I'},
		{	"script", required_argument, NULL, 'S'},
		{	NULL}
	};

	int c, option_index = 0;
	while ((c = getopt_long(argc, argv, "D:I:S:", long_options, &option_index)) != -1) {
		switch (c) {
			case 'D': DataDir = optarg; break;
			case 'I': ImageDir = optarg; break;
			case 'S': ScriptDir = optarg; break;
			default: return false;
		}
	}
	return true;
}

bool cPluginWetter::Initialize(void)
{
	if (DataDir == NULL) {
		DataDir = strdup(ConfigDirectory(PLUGIN_NAME_I18N));
		esyslog("%s: Parameter '-D' not set.\n"
				" Set default value : %s\n", PLUGIN_NAME_I18N, DataDir);
	}
	if (ImageDir == NULL) {
		ImageDir = strdup(ConfigDirectory(PLUGIN_NAME_I18N));
		esyslog("%s: Parameter '-I' not set.\n"
				" Set default value : %s\n", PLUGIN_NAME_I18N, ImageDir);
	}
	if (ScriptDir == NULL) {
		ScriptDir = strdup(ConfigDirectory(PLUGIN_NAME_I18N));
		esyslog("%s: Parameter '-S' not set.\n"
				" Set default value : %s\n", PLUGIN_NAME_I18N, ImageDir);
	}
#ifdef HAVE_MAGICK
	dsyslog("DEBUG : %s: defined HAVE_MAGICK\n",PLUGIN_NAME_I18N);
#endif

#ifdef HAVE_IMLIB2
	dsyslog("DEBUG : %s: defined HAVE_IMLIB2\n",PLUGIN_NAME_I18N);
#endif

#ifdef HAVE_4MB
	dsyslog("DEBUG : %s: defined HAVE_4MB\n",PLUGIN_NAME_I18N);
#endif

	return true;
}

bool cPluginWetter::Start(void)
{
	if ( wetterSetup.w_update == true ) {
		char *buffer;
		char *url;

		asprintf(&buffer, "%s%s",ScriptDir,"/data1.xml");
//		asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data1,"?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494");
		asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data1,"?cc=*&unit=m&dayf=10");
		dsyslog("DEBUG : weatherng: Autoupdate %s => %s",url, buffer);
		httpGet(url,buffer);
		free(buffer);
		free(url);

		asprintf(&buffer, "%s%s",ScriptDir,"/data2.xml");
//		asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data2,"?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494");
		asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data2,"?cc=*&unit=m&dayf=10");
		dsyslog("DEBUG : weatherng: %s => %s",url, buffer);
		httpGet(url,buffer);
		free(buffer);
		free(url);

		asprintf(&buffer, "%s%s",ScriptDir,"/data3.xml");
//		asprintf(&url, "%s%s%s","http://xoap.weather.com/weather/local/",wetterSetup.st_data3,"?cc=*&unit=m&dayf=10&prod=xoap&par=1004124588&key=079f24145f208494");
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
	RegisterI18n(Phrases);
	return true;
}

void cPluginWetter::Housekeeping(void)
{
}

cOsdObject *cPluginWetter::MainMenuAction(void)
{
	return new cWetterOsd();
}

cMenuSetupPage *cPluginWetter::SetupMenu(void)
{
	return new cMenuWetterSetup;
}

bool cPluginWetter::SetupParse(const char *Name, const char *Value)
{
	if (!strcasecmp(Name, "AutoUpdate")) wetterSetup.w_update = atoi(Value);
	else if (!strcasecmp(Name, "RadarLeft")) wetterSetup.w_left = atoi(Value);
	else if (!strcasecmp(Name, "RadarTop")) wetterSetup.w_top = atoi(Value);
	else if (!strcasecmp(Name, "RadarWidth")) wetterSetup.w_width = atoi(Value);
	else if (!strcasecmp(Name, "RadarHeight")) wetterSetup.w_height = atoi(Value);
#ifdef HAVE_4MB
	else if (!strcasecmp(Name, "HiColor")) wetterSetup.w_hicolor = atoi(Value);
#endif
	else if (!strcasecmp(Name, "Fontsize")) wetterSetup.w_fontsize = atoi(Value);
	else if (!strcasecmp(Name, "RoundedCorner")) wetterSetup.w_corner = atoi(Value);
	else if (!strcasecmp(Name, "Inverted")) wetterSetup.w_inverted = atoi(Value);
	else if (!strcasecmp(Name, "OSDOffsetX")) wetterSetup.w_osdoffset_x = atoi(Value);
	else if (!strcasecmp(Name, "OSDOffsetY")) wetterSetup.w_osdoffset_y = atoi(Value);
	else if (!strcasecmp(Name, "Theme")) wetterSetup.w_theme = atoi(Value);
	else if (!strcasecmp(Name, "Data1")) strcpy(wetterSetup.st_data1,Value);
	else if (!strcasecmp(Name, "Data2")) strcpy(wetterSetup.st_data2,Value);
	else if (!strcasecmp(Name, "Data3")) strcpy(wetterSetup.st_data3,Value);
	else if (!strcasecmp(Name, "Pic0")) asprintf(&wetterSetup.st_pic0, "%s", Value);
	else if (!strcasecmp(Name, "Pic1")) asprintf(&wetterSetup.st_pic1, "%s", Value);
	else if (!strcasecmp(Name, "Pic2")) asprintf(&wetterSetup.st_pic2, "%s", Value);
	else if (!strcasecmp(Name, "Pic3")) asprintf(&wetterSetup.st_pic3, "%s", Value);
	else if (!strcasecmp(Name, "Pic4")) asprintf(&wetterSetup.st_pic4, "%s", Value);
	else
	return false;

	return true;
}

cMenuWetterSetup::cMenuWetterSetup(void)
{
	themes[eWetterThemeClassic] = tr("Classic");
	themes[eWetterThemeenElchi] = tr("enElchi");
	themes[eWetterThemeMoronimo] = tr("Moronimo");
	themes[eWetterThemeDeepBlue] = tr("DeepBlue");
	themes[eWetterThemeEnigma] = tr("Enigma");
	themes[eWetterThemeSilverGreen] = tr("SilverGreen");
	themes[eWetterThemeMoronimoMKII] = tr("MoronimoMKII");

	Add(new cMenuEditBoolItem(tr("Update data on start"), &wetterSetup.w_update));
	Add(new cMenuEditIntItem(tr("Radarbitmap left"), &wetterSetup.w_left, 1, 200));
	Add(new cMenuEditIntItem(tr("Radarbitmap top"), &wetterSetup.w_top, 1, 200));
#ifdef HAVE_4MB
	// 4 MB FF-CARD
	Add(new cMenuEditIntItem(tr("Radarbitmap width"), &wetterSetup.w_width, 100, 672));
	Add(new cMenuEditIntItem(tr("Radarbitmap height"), &wetterSetup.w_height, 100, 567));
	Add(new cMenuEditBoolItem(tr("Use 256 colors for radarmaps"), &wetterSetup.w_hicolor));
#else
	// 2 MB FF-CARD
	Add(new cMenuEditIntItem(tr("Radarbitmap width"), &wetterSetup.w_width, 100, 450));
	Add(new cMenuEditIntItem(tr("Radarbitmap height"), &wetterSetup.w_height, 100, 400));
#endif
	Add(new cMenuEditBoolItem(tr("Use small fonts"), &wetterSetup.w_fontsize));
	Add(new cMenuEditBoolItem(tr("Corner rounded"), &wetterSetup.w_corner));
	Add(new cMenuEditBoolItem(tr("Symbols inverted"), &wetterSetup.w_inverted));
	Add(new cMenuEditIntItem(tr("Offset horizontal"), &wetterSetup.w_osdoffset_x, -50, 50));
	Add(new cMenuEditIntItem(tr("Offset vertical"), &wetterSetup.w_osdoffset_y, -50, 0));
	Add(new cMenuEditStraItem(tr("Theme"), &wetterSetup.w_theme, eWetterThemeMaxNumber, themes));
	Add(new cMenuEditStrItem(tr("Weather station 1"), wetterSetup.st_data1, sizeof( wetterSetup.st_data1), DEFAULT_CHARS));
	Add(new cMenuEditStrItem(tr("Weather station 2"), wetterSetup.st_data2, sizeof( wetterSetup.st_data2), DEFAULT_CHARS));
	Add(new cMenuEditStrItem(tr("Weather station 3"), wetterSetup.st_data3, sizeof( wetterSetup.st_data3), DEFAULT_CHARS));
}

void cMenuWetterSetup::Store(void)
{
	SetupStore("AutoUpdate", wetterSetup.w_update);
	SetupStore("RadarLeft", wetterSetup.w_left);
	SetupStore("RadarTop", wetterSetup.w_top);
	SetupStore("RadarWidth", wetterSetup.w_width);
	SetupStore("RadarHeight", wetterSetup.w_height);
#ifdef HAVE_4MB
	SetupStore("HiColor", wetterSetup.w_hicolor);
#endif
	SetupStore("Fontsize", wetterSetup.w_fontsize);
	SetupStore("RoundedCorner", wetterSetup.w_corner);
	SetupStore("Inverted", wetterSetup.w_inverted);
	SetupStore("OSDOffsetX", wetterSetup.w_osdoffset_x);
	SetupStore("OSDOffsetY", wetterSetup.w_osdoffset_y);
	SetupStore("Theme", wetterSetup.w_theme);
	SetupStore("Data1", wetterSetup.st_data1);
	SetupStore("Data2", wetterSetup.st_data2);
	SetupStore("Data3", wetterSetup.st_data3);
}

VDRPLUGINCREATOR(cPluginWetter); // Don't touch this!
