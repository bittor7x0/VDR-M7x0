/*
 * newsticker.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: newsticker.c 0.0.1 2003/11/22 ew $
 */

#include <vdr/plugin.h>
#include <vdr/osd.h>

#include <getopt.h>

#include "defines.h"
#include "news.h"


#define scrollstart

static const char *VERSION        = "0.0.4";
static const char *DESCRIPTION    = "Newsticker for VDR";
static const char *MAINMENUENTRY  = "Newsticker";

const char *plugin_Name;
char *option_savePath;

int speed = 5;
int lineRow = 520;
int scrollsteps = 2;
int colortext = 9;
int colorbg   = 1;

char url_1[100]="http://www.tagesschau.de/newsticker.rdf";
char url_2[100]="http://www.heise.de/newsticker/heise.rdf";
char url_3[100]="http://www.n24.de/rss/?rubrik=home";
char url_4[100]="http://www.heise.de/tp/news.rdf";
char url_5[100]="http://www.golem.de/golem_backend.rdf";
char url_6[100]="http://www.netphoenix.at/rss/shortnews.php";
char url_7[100]="http://slashdot.org/slashdot.rdf";
char url_8[100]="http://www.n24.de/rss/?rubrik=sport";
char url_9[100]="http://www.n24.de/rss/?rubrik=home";

struct tMyColor
{
	char*  Name;
	tColor Color;
};

static const tMyColor MyColors[] = {{"Transparent", clrTransparent},
																		{"Gray",        clrGray50     },
																		{"Black",       clrBlack      },
																		{"Red",         clrRed        },
																		{"Green",       clrGreen      },
																		{"Yellow",      clrYellow     },
																		{"Magenta",     clrMagenta    },
																		{"Blue",        clrBlue       },
																		{"Cyan",        clrCyan       },
																		{"White",       clrWhite      }};
// --- cLineGame -------------------------------------------------------------

class cOSDNewsticker : public cThread, public cOsdObject {
private:
  cOsd *osd;
  eKeys LastKey;
  bool running, shutdown, downloading;
  int x;
  int y;
  tColor colorFg;
  tColor colorBg;
  
  char* theMessage;
  cNews* news;
  
  int lineHeight;
  int lineWidth;  
  
  int messagePosition;
  
  char* url;
  
  bool restart;
  
  int getSpeed();
  void setSpeed(int value);
  int getMessagePosition();
  void setMessagePosition(int value);
  int getRow();
  void setRow(int value);
  char* getURL();
  void setURL(char* value);
  bool getRestart();
  void setRestart(bool value);
protected:
  virtual void Action(void);
public:
  cOSDNewsticker(void);
  ~cOSDNewsticker();
  void Show(void) {Start(); }
  eOSState ProcessKey(eKeys Key);
  int scrollMessage(char* message, int speed, int position, cOsd *osd, tColor ColorFg, tColor ColorBg, eDvbFont font);
  cOsd* cOSDNewsticker::createOSDLine(int row, eDvbFont font);
  };

cOSDNewsticker::cOSDNewsticker(void)
{
	url = new char[100];
	news = NULL;
	osd = NULL;
	theMessage = NULL;
	
	x = y = 50;
	colorFg = MyColors[colortext].Color;
	colorBg = MyColors[colorbg].Color;

	lineHeight = 300;
	lineWidth = 700;
	  
	running = true;
	shutdown = false;
	downloading = false;
	setRestart(false);//watch
	setURL(url_1);
	
}

cOSDNewsticker::~cOSDNewsticker()
{
	running = false;
	while(!shutdown)
	{
		usleep(100000);
      		running = false;
	}
	
	if (osd) {
		delete (osd);
		osd = NULL;
	}
	
	if(url)
	{
		delete url;
	}
	
	if(theMessage)
	{
		free(theMessage);
	}
}

int cOSDNewsticker::getSpeed()
{
	return speed;
}

void cOSDNewsticker::setSpeed(int value)
{
	speed = value;
}

int cOSDNewsticker::getMessagePosition()
{
	return messagePosition;
}

void cOSDNewsticker::setMessagePosition(int value)
{
	messagePosition = value;
}

int cOSDNewsticker::getRow()
{
	return lineRow;
}

void cOSDNewsticker::setRow(int value)
{
	lineRow = value;
}

char* cOSDNewsticker::getURL()
{
	return url;
}

void cOSDNewsticker::setURL(char* value)
{
	strcpy(url ,value);
}

bool cOSDNewsticker::getRestart()
{
	return restart;
}

void cOSDNewsticker::setRestart(bool value)
{
	restart = value;
}

void cOSDNewsticker::Action(void)
{
	int pos;
	//scrollMessage("Please wait ...", getSpeed(), getMessagePosition(), osd, colorFg, colorBg, fontOsd);
	
	if(!downloading)
	{
		downloading = true;
		news = new cNews();
		
		//show Please wait
		//scrollMessage("Please wait ...", 0, 0, osd, (eDvbColor)bgbackground, (eDvbColor)bgbackground, fontOsd);
		//scrollMessage("Please wait ...", getSpeed(), getMessagePosition(), osd, colorFg, colorBg, fontOsd);
		if(theMessage)
		{
			free(theMessage);
		}
		
		if(!news->downloadRDF(getURL(), plugin_Name, option_savePath))
		{		
			char* error_message = "Error downloading the news! +++ Error downloading the news!+++ Error downloading the news! +++ Error downloading the news!+++ Error downloading the news! +++ Error downloading the news!+++ ";
			theMessage = (char *) malloc(strlen(error_message) + 1);
			memset(theMessage, 0, strlen(error_message) + 1);
			strcpy(theMessage, error_message);
			}
		else
		{			
			theMessage = (char *) malloc(strlen(news->getScrolltext()) + 1);
			memset(theMessage, 0, strlen(news->getScrolltext()) + 1);
			strcpy(theMessage, news->getScrolltext());
		}
		
		delete news;
		
		pos = 0;//lineWidth / 2;
	        setMessagePosition(pos);	        
	        osd = createOSDLine(getRow(), fontOsd);
	        
	        downloading = false;
	}
        
        while (running)
	{		
		pos = scrollMessage(theMessage, getSpeed(), getMessagePosition(), osd, colorFg, colorBg, fontOsd);
		if (pos == 999 || getRestart() == true)
			running = false;		
	}
	
	if(getRestart())
	{
		//show Please wait
		scrollMessage("Please wait ...", 0, 0, osd, colorFg, colorBg, fontOsd);
	
		setRestart(false);
		running = true;
		Action();
	}
	
	shutdown = true;
}

cOsd* cOSDNewsticker::createOSDLine(int row, eDvbFont font)
{
	int oldspeed = getSpeed();
	setSpeed(-1);
	
	if(!osd)
		osd = cOsdProvider::NewOsd(0, row);
	else
	{
		delete osd;
		osd = cOsdProvider::NewOsd(0, row);
	}
	const cFont *theFont = cFont::GetFont(font);
	int theHeight = theFont->Height();
	
	tArea win = { 0, 0, lineWidth-1, theHeight-1, 4 };
	//tArea win = { 0, 0, lineWidth-1, theHeight-1, 4 };
	
	//lets see if it works
	if (osd->CanHandleAreas(&win, 1) == oeOk)
		osd->SetAreas(&win, 1);
	else
		fprintf(stderr, "Cannot set tArea\n");
	
	setSpeed(oldspeed);
	
	return osd;
}

int cOSDNewsticker::scrollMessage(char* message, int speedvalue, int position, cOsd *osd, tColor ColorFg, tColor ColorBg, eDvbFont font)
{
	if (getSpeed() == -1)
		return position;
		
	if(!osd)
		return 999;
	const cFont *theFont = cFont::GetFont(font);
	int theHeight = theFont->Height();
	
	cBitmap *bitmap = new cBitmap(lineWidth, theHeight, 4);
	
	int theWidth = theFont->Width(message);
	
	position -= scrollsteps;
	setMessagePosition(position);
	
	if (position < (-theWidth+lineWidth))
		return 999;	
	
	bitmap->DrawText(position, 0, message, ColorFg, ColorBg, theFont);
	osd->DrawBitmap(0, 0, *bitmap);
 	osd->Flush();
	
	delete bitmap;
	usleep(1000001 - (speedvalue * 100000));
	
	return position;
	
}
eOSState cOSDNewsticker::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key & ~k_Repeat) {
       case kUp:     if (getRow() > 0)   setRow(getRow() - 10); createOSDLine(getRow(), fontOsd); break;
       case kDown:   if (getRow() < 520) setRow(getRow() + 10); createOSDLine(getRow(), fontOsd); break;
       case kLeft:   if (getSpeed() > 1)   setSpeed(getSpeed() - 1); break;
       case kRight:  if (getSpeed() < 10)   setSpeed(getSpeed() + 1); break;
       case KEY_1:    setURL(url_1); setRestart(true); break;
       case KEY_2:    setURL(url_2); setRestart(true); break;
       case KEY_3:    setURL(url_3); setRestart(true); break;
       case KEY_4:    setURL(url_4); setRestart(true); break;
       case KEY_5:    setURL(url_5); setRestart(true); break;
       case KEY_6:    setURL(url_6); setRestart(true); break;
       case KEY_7:    setURL(url_7); setRestart(true); break;
       case KEY_8:    setURL(url_8); setRestart(true); break;
       case KEY_9:    setURL(url_9); setRestart(true); break;
       /*case kGreen:  color = clrGreen; break;
       case kYellow: color = clrYellow; break;
       case kBlue:   color = clrBlue; break;*/
       case kOk:     return osEnd;
       default: return state;
       }
       
       state = osContinue;
      }
  return state;
}

// --- cPluginNewsticker --------------------------------------------------------

class cPluginNewsticker : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginNewsticker(void);
  virtual ~cPluginNewsticker();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginNewsticker::cPluginNewsticker(void)
{
  // Initialize any member variables here.
  
  
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginNewsticker::~cPluginNewsticker()
{
  // Clean up after yourself!
}

const char *cPluginNewsticker::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginNewsticker::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  static struct option long_options[] = {
       { "output",     required_argument, NULL, 'o' },
       { NULL }
     };

  int c;
  while ((c = getopt_long(argc, argv, "o", long_options, NULL)) != -1) {
        switch (c) {
          case 'o': option_savePath = optarg;
                    break;
          }
        }
  return true;
}

bool cPluginNewsticker::Start(void)
{
	plugin_Name = Name();
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginNewsticker::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginNewsticker::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cOSDNewsticker;
}

//the setup part
class cMenuSetupNewsticker : public cMenuSetupPage {
 public:
  cMenuSetupNewsticker();
 protected:
  virtual void Store(void);
 private:
 	int new_speed;
 	int new_scrollsteps;
	int new_lineRow;
	int new_colortext;
	int new_colorbg;
	char* new_url_1;
	char* new_url_2;
	char* new_url_3;
	char* new_url_4;
	char* new_url_5;
	char* new_url_6;
	char* new_url_7;
	char* new_url_8;
	char* new_url_9;
 	
 	const char* myColors2[10];// = {"Transparent", "Gray", "Black", "Red", "Green", "Yellow", "Magenta", "Blue", "Cyan", "White"};
 	int  colorsCount;
};

cMenuSetupNewsticker::cMenuSetupNewsticker()
{
	colorsCount = 10;
	for(int i = 0; i < colorsCount; i++)
		myColors2[i] = tr(MyColors[i].Name);

	new_speed = speed;
	new_scrollsteps = scrollsteps;
	new_lineRow = lineRow;
	new_colortext = colortext;
	new_colorbg = colorbg;
	new_url_1 = url_1;
	new_url_2 = url_2;
	new_url_3 = url_3;
	new_url_4 = url_4;
	new_url_5 = url_5;
	new_url_6 = url_6;
	new_url_7 = url_7;
	new_url_8 = url_8;
	new_url_9 = url_9;
    
	Add(new cMenuEditIntItem(tr("Scroll speed"), &speed, 1, 10));
	Add(new cMenuEditIntItem(tr("Scrollsteps"), &scrollsteps, 1, 20));
	Add(new cMenuEditIntItem(tr("Row"), &lineRow, 1, 520));	
	Add(new cMenuEditStraItem(tr("Text Color"), &colortext, colorsCount, myColors2));
	Add(new cMenuEditStraItem(tr("Background Color"), &colorbg, colorsCount , myColors2));
	Add(new cMenuEditStrItem(tr("URL 1"), url_1, sizeof(url_1) ,tr(FileNameChars)));
	Add(new cMenuEditStrItem(tr("URL 2"), url_2, sizeof(url_2) ,tr(FileNameChars)));
	Add(new cMenuEditStrItem(tr("URL 3"), url_3, sizeof(url_3) ,tr(FileNameChars)));
	Add(new cMenuEditStrItem(tr("URL 4"), url_4, sizeof(url_4) ,tr(FileNameChars)));
	Add(new cMenuEditStrItem(tr("URL 5"), url_5, sizeof(url_5) ,tr(FileNameChars)));
	Add(new cMenuEditStrItem(tr("URL 6"), url_6, sizeof(url_6) ,tr(FileNameChars)));
	Add(new cMenuEditStrItem(tr("URL 7"), url_7, sizeof(url_7) ,tr(FileNameChars)));
	Add(new cMenuEditStrItem(tr("URL 8"), url_8, sizeof(url_8) ,tr(FileNameChars)));
	Add(new cMenuEditStrItem(tr("URL 9"), url_9, sizeof(url_9) ,tr(FileNameChars)));
}

void cMenuSetupNewsticker::Store(void)
{
	SetupStore("Scroll speed", new_speed = speed);
	SetupStore("Scrollsteps", new_scrollsteps = scrollsteps);
	SetupStore("Row", new_lineRow = lineRow);
	SetupStore("Text Color", new_colortext = colortext);
	SetupStore("Background Color", new_colorbg = colorbg);
	SetupStore("URL 1", new_url_1 = url_1);
	SetupStore("URL 2", new_url_2 = url_2);
	SetupStore("URL 3", new_url_3 = url_3);
	SetupStore("URL 4", new_url_4 = url_4);
	SetupStore("URL 5", new_url_5 = url_5);
	SetupStore("URL 6", new_url_6 = url_6);
	SetupStore("URL 7", new_url_7 = url_7);
	SetupStore("URL 8", new_url_8 = url_8);
	SetupStore("URL 9", new_url_9 = url_9);
}

cMenuSetupPage *cPluginNewsticker::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupNewsticker();
}

bool cPluginNewsticker::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "Scroll speed")) speed = atoi(Value);
  else if (!strcasecmp(Name, "Scrollsteps")) scrollsteps = atoi(Value);
  else if (!strcasecmp(Name, "Row")) lineRow = atoi(Value);
  else if (!strcasecmp(Name, "Text Color")) colortext = atoi(Value);
  else if (!strcasecmp(Name, "Background Color")) colorbg = atoi(Value);
  else if (!strcasecmp(Name, "URL 1")) strcpy(url_1,Value);
  else if (!strcasecmp(Name, "URL 2")) strcpy(url_2,Value);
  else if (!strcasecmp(Name, "URL 3")) strcpy(url_3,Value);
  else if (!strcasecmp(Name, "URL 4")) strcpy(url_4,Value);
  else if (!strcasecmp(Name, "URL 5")) strcpy(url_5,Value);
  else if (!strcasecmp(Name, "URL 6")) strcpy(url_6,Value);
  else if (!strcasecmp(Name, "URL 7")) strcpy(url_7,Value);
  else if (!strcasecmp(Name, "URL 8")) strcpy(url_8,Value);
  else if (!strcasecmp(Name, "URL 9")) strcpy(url_9,Value);
  else
     return false;
  return true;
}
VDRPLUGINCREATOR(cPluginNewsticker); // Don't touch this!
