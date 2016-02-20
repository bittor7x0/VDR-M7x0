/*
 * cinebars.c, a plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include "i18n.h"

static const char *VERSION        = "0.1.0";
static const char *DESCRIPTION    = trNOOP("Overlays \"cinebars\"");
static const char *MAINMENUENTRY  = trNOOP("Cinebars");

int HideMenuEntry = false;
int barUp = true;
int barUpHeight = 78;
int barDn = true;
int barDnHeight = 78;
int barStep = 5;
int osdMode = 1;
int osdWidth = 768;
int osdHeight = 576;
int osdColor = 0;

// --- cBars -------------------------------------------------------------------

class cBars : public cOsdObject {
public:
  cBars(void);
  ~cBars(void);
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
private:
  void Draw(void); 
  cOsd *osd;
  int w, h;
  // for Gimmick :-)
  void CloseOpen(void); 
  bool barClosed;
  int uh, dh, hb;
  void Color(void);
  tColor barColor;
};
	

cBars::cBars(void)
{
  barClosed = false;
}

cBars::~cBars(void)
{
  delete osd;
}

void cBars::Show(void)
{
#if VDRVERSNUM >= 10706
  switch (osdMode) {
     case 0:
        w = Setup.OSDWidth;
        h = Setup.OSDHeight;
        break;
     case 1:
        w = osdWidth;
        h = osdHeight;
        break;
     }
#else
        w = osdWidth;
        h = osdHeight;
#endif

  switch (osdColor) {
     case 0:
        barColor = clrBlack;
        break;
     case 1:
        barColor = clrWhite;
        break;
     case 2:
        barColor = clrRed;
        break;
     case 3:
        barColor = clrGreen;
        break;
     case 4:
        barColor = clrYellow;
        break;
     case 5:
        barColor = clrBlue;
        break;
     }

  osd = cOsdProvider::NewOsd(0, 0);

  if (osd) {		
     tArea Area = {0, 0, w - 1, h - 1, 1};
     osd->SetAreas(&Area, 1);
     Draw();
	 }
  else
     printf("Error creating OSD\n");
}

void cBars::Color(void)
{
  switch (osdColor) {
     case 0:
        barColor = clrBlack;
        break;
     case 1:
        barColor = clrWhite;
        break;
     case 2:
        barColor = clrRed;
        break;
     case 3:
        barColor = clrGreen;
        break;
     case 4:
        barColor = clrYellow;
        break;
     case 5:
        barColor = clrBlue;
        break;
     }

  if (osd) delete osd;
  
  osd = cOsdProvider::NewOsd(0, 0);

  if (osd) {		
     tArea Area = {0, 0, w - 1, h - 1, 1};
     osd->SetAreas(&Area, 1);
     Draw();
	 }
  else
     printf("Error creating OSD\n");
}

void cBars::Draw(void)
{
  osd->DrawRectangle(0, 0, w - 1, barUpHeight - 1, barUp ? barColor : clrTransparent);
  osd->DrawRectangle(0, barUpHeight, w - 1, h - barDnHeight -1, clrTransparent);
  osd->DrawRectangle(0, h - barDnHeight, w -1, h - 1, barDn ? barColor : clrTransparent);
  osd->Flush();
}

void cBars::CloseOpen(void)
{
  osd->DrawRectangle(0, 0, w - 1, uh - 1,  barColor);
  osd->DrawRectangle(0, uh, hb, h - dh - 1,  barColor);
  osd->DrawRectangle(hb, uh, w - hb - 1, h - dh -1, clrTransparent);
  osd->DrawRectangle(w - hb, uh, w - 1, h - dh - 1,  barColor);
  osd->DrawRectangle(0, h - dh, w - 1, h - 1,  barColor);
  osd->Flush();
}


// --- cPluginCinebars ---------------------------------------------------------

class cPluginCinebars : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginCinebars(void);
  virtual ~cPluginCinebars();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return HideMenuEntry ? NULL : tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
};

eOSState cBars::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key & ~k_Repeat) {
        case kUp:
           barUp = !barUp;
           barClosed = false;
           break;
        case kDown:
           barDn = !barDn;
           barClosed = false;
           break;
        case kLeft:
           if (barUp) {
              if (barUpHeight > barStep)
                 barUpHeight -= barStep;
              else
                 barUpHeight = 0;
              }
           if (barDn) {
              if (barDnHeight > barStep)
                 barDnHeight -= barStep;
              else
                 barDnHeight = 0;
              }
           barClosed = false;
           break;
        case kRight:
           if (barUp) {
              if (barUpHeight < h / 2 - (barStep-1))
                 barUpHeight += barStep;
              else
                 barUpHeight = h / 2 + 1;
              }
           if (barDn) {
              if (barDnHeight < h / 2 - (barStep-1))
                 barDnHeight += barStep;
              else
                 barDnHeight = h / 2 + 1;
              }
           barClosed = false;
           break;
        case kRed:
        case kOk:
           // Gimmick, open/close Screen 
           if (!barClosed) {
              uh = barUpHeight;
              dh = barDnHeight;
              uh > dh ? hb = w / h * uh - 1 : hb = w / h * dh - 1;
              while ((uh < h / 2 - 4) && (dh < h / 2 - 4)) {
                 CloseOpen();
                 uh += 5;
                 dh += 5;
                 uh > dh ? hb = w / h *  uh - 1 : hb = w / h * dh - 1;
                 }
              osd->DrawRectangle(0, 0, w - 1, h - 1,  barColor);
              osd->Flush();
              uh = dh = h / 2 + 1;
              barClosed = true;
              if (Key == kOk) return osEnd;
              return state;
              }
           else {
              while ((uh > barUpHeight) && (dh > barDnHeight)) {
                 CloseOpen();
                 uh -= 5;
                 dh -= 5;
                 hb = w / h * uh - 1;
                 }
              barClosed = false;
              break;		
              }
        case kGreen:
           osdColor -= 1;
           if (osdColor < 0) osdColor = 5;
           barClosed = false;		   
		   Color();
           return state;
        case kYellow:
           osdColor += 1;
           if (osdColor > 5) osdColor = 0;
           barClosed = false;		   
		   Color();
           return state;		   
        case kBlue:
        case kBack:
           return osEnd;
        case kPlay:
        case kPause:
        case kStop:
        case kRecord:
        case kFastFwd:
        case kFastRew:
        case kNext:
        case kPrev:
           return osUnknown;
        default:
           return osContinue;
        }
	 Draw();
     }
  return state;
}

cPluginCinebars::cPluginCinebars(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginCinebars::~cPluginCinebars()
{
  // Clean up after yourself!
}

const char *cPluginCinebars::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginCinebars::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginCinebars::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginCinebars::Start(void)
{
  // Start any background activities the plugin shall perform.
#if VDRVERSNUM < 10507
    RegisterI18n(Phrases);
#endif
  return true;
}

void cPluginCinebars::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginCinebars::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cBars();
}

// --- cMenuSetupCinebars --------------------

class cMenuSetupCinebars : public cMenuSetupPage {
private:
  int nbarUp;
  int nbarUpHeight;
  int nbarDn;
  int nbarDnHeight;
  int nbarStep;
  int nosdMode;
  int nosdWidth;
  int nosdHeight;
  int nosdColor;  
  int nHideMenuEntry;
#if VDRVERSNUM >= 10706
  const char *osdModeEntry[2];
#endif
  const char *osdColorEntry[6];
  void Setup(void);
protected:
  virtual void Store(void);
public:
  cMenuSetupCinebars(void);
  virtual eOSState ProcessKey(eKeys Key);
};

cMenuSetupPage *cPluginCinebars::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupCinebars;
}

cMenuSetupCinebars::cMenuSetupCinebars(void)
{
#if VDRVERSNUM >= 10706
  osdModeEntry[0] = trVDR("auto");
  osdModeEntry[1] = tr("manual");
#endif
  
  osdColorEntry[0] = tr("black");
  osdColorEntry[1] = tr("white");
  osdColorEntry[2] = tr("red");
  osdColorEntry[3] = tr("green");
  osdColorEntry[4] = tr("yellow");
  osdColorEntry[5] = tr("blue");
  
  nbarUp = barUp;
  nbarUpHeight = barUpHeight;
  nbarDn = barDn;
  nbarDnHeight = barDnHeight;
  nbarStep = barStep;
  nosdMode = osdMode;
  nosdWidth = osdWidth;
  nosdHeight = osdHeight;
  nosdColor = osdColor; 
  nHideMenuEntry = HideMenuEntry;

  Setup();
}

void cMenuSetupCinebars::Setup(void)
{
  int current = Current();
  Clear();

  Add(new cMenuEditBoolItem(tr("Hide mainmenu entry"), &nHideMenuEntry));
  Add(new cMenuEditStraItem(tr("Bar color"),           &nosdColor, 6, osdColorEntry));   
  Add(new cMenuEditBoolItem(tr("Top bar"),             &nbarUp));
  if (nbarUp)
     Add(new cMenuEditIntItem(tr("Top bar height"),    &nbarUpHeight, 1, 200));
  Add(new cMenuEditBoolItem(tr("Bottom bar"),          &nbarDn));
  if (nbarDn)
     Add(new cMenuEditIntItem(tr("Bottom bar height"), &nbarDnHeight, 1, 200));
  Add(new cMenuEditIntItem(tr("Step size"),            &nbarStep, 1, 20));
#if VDRVERSNUM >= 10706
  Add(new cMenuEditStraItem(tr("OSD size"),	           &nosdMode, 2, osdModeEntry));
  if (nosdMode > 0) {
     Add(new cMenuEditIntItem(tr("OSD width"),         &nosdWidth,  720, 768));
     Add(new cMenuEditIntItem(tr("OSD height"),        &nosdHeight, 480, 576));
     }
#endif

  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupCinebars::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key) {
        case kOk:
           state = osBack;
        default:
           break;
        }
     }
  if ((Key == kLeft) || (Key == kRight))
     Setup();
  if (state == osBack && Key == kOk)
     Store();
  return state;
}

bool cPluginCinebars::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if       (!strcasecmp(Name, "TopBar"))	      barUp = atoi(Value);
  else if  (!strcasecmp(Name, "TopBarHeight"))    barUpHeight = atoi(Value);
  else if  (!strcasecmp(Name, "BottomBar")) 	  barDn = atoi(Value);
  else if  (!strcasecmp(Name, "BottomBarHeight")) barDnHeight = atoi(Value);
  else if  (!strcasecmp(Name, "StepHeight")) 	  barStep = atoi(Value);
  else if  (!strcasecmp(Name, "OsdMode")) 	      osdMode = atoi(Value);
  else if  (!strcasecmp(Name, "OsdWidth")) {	  osdWidth = atoi(Value);  osdWidth &= ~0x07; } // OSD width must be a multiple of 8
  else if  (!strcasecmp(Name, "OsdHeight"))	      osdHeight = atoi(Value);
  else if  (!strcasecmp(Name, "OsdColor"))	      osdColor = atoi(Value);  
  else if  (!strcasecmp(Name, "HideMenuEntry"))   HideMenuEntry = atoi(Value);
  else return false;
    
  return true;
}

void cMenuSetupCinebars::Store(void)
{
  barUp = nbarUp;
  barUpHeight = nbarUpHeight;
  barDn = nbarDn;
  barDnHeight = nbarDnHeight;
  barStep = nbarStep;
  osdMode = nosdMode;
  osdWidth = nosdWidth;
  osdHeight = nosdHeight;
  osdColor = nosdColor;
  HideMenuEntry = nHideMenuEntry;
  
  SetupStore("TopBar", barUp);
  SetupStore("TopBarHeight", barUpHeight);
  SetupStore("BottomBar", barDn);
  SetupStore("BottomBarHeight", barDnHeight);
  SetupStore("StepHeight", barStep);
  SetupStore("OsdMode", osdMode);
  SetupStore("OsdWidth", osdWidth);
  SetupStore("OsdHeight", osdHeight);
  SetupStore("OsdColor", osdColor);  
  SetupStore("HideMenuEntry", HideMenuEntry);
}

VDRPLUGINCREATOR(cPluginCinebars); // Don't touch this!
