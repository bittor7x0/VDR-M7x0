/*
 * cinebars.c, a plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#if VDRVERSNUM < 10507
    #include "i18n.h"
    #define trNOOP(s) (s)
#endif

#if VDRVERSNUM >= 10307
    const tColor barColor = clrBlack;
#endif

static const char *VERSION        = "0.0.5";
static const char *DESCRIPTION    = trNOOP("Overlays \"cinebars\"");
static const char *MAINMENUENTRY  = trNOOP("Cinebars");

int barUp = true;
int barUpHeight = 78;
int barDn = true;
int barDnHeight = 78;
int barStep = 5;
int osdMode = 0;
int HideMenuEntry = false;

// --- cBars -------------------------------------------------------------------

class cBars : public cOsdObject {
public:
    cBars(void);
    ~cBars(void);
    virtual void Show(void);
    virtual eOSState ProcessKey(eKeys Key);
private:
    void Draw(void); 
#if VDRVERSNUM >= 10307
    cOsd *osd;
#else
    cOsdBase *osd;
    tWindowHandle window;
#endif
    int w, h;
    // for Gimmick :-)
    void CloseOpen(void); 
    bool barClosed;
    int uh, dh, hb;
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
    w = (osdMode == 0) ? 720 : 768;	// for PAL-Setup, must be done for NTSC
    h = 576;
#if VDRVERSNUM >= 10307
    osd = cOsdProvider::NewOsd(0, 0);
#else
    osd = cOsd::OpenRaw(0, 0);
#endif

    if (osd) {		
#if VDRVERSNUM >= 10307
        tArea Area = {0, 0, w - 1, h - 1, 1};
        osd->SetAreas(&Area, 1);
#else
	window = osd->Create(0, 0, w, h - 1 , 1, true, false);
	osd->AddColor(clrTransparent, window);
	osd->AddColor(clrBlack, window);
#endif
        Draw();
	}
    else  printf("Error creating OSD\n");
}


void cBars::Draw(void)
{
#if VDRVERSNUM >= 10307
    osd->DrawRectangle(0, 0, w - 1, barUpHeight - 1, barUp ? barColor : clrTransparent);
    osd->DrawRectangle(0, barUpHeight, w - 1, h - barDnHeight -1, clrTransparent);
    osd->DrawRectangle(0, h - barDnHeight, w -1, h - 1, barDn ? barColor : clrTransparent);
#else
    osd->Fill(0, 0, w - 1, barUpHeight - 1, barUp ? clrBlack : clrTransparent, window);
    osd->Fill(0, barUpHeight, w - 1, h - barDnHeight -1, clrTransparent, window);
    osd->Fill(0, h - barDnHeight, w -1, h - 1, barDn ? clrBlack : clrTransparent, window);
#endif
    osd->Flush();
}

void cBars::CloseOpen(void)
{
#if VDRVERSNUM >= 10307
    osd->DrawRectangle(0, 0, w - 1, uh - 1,  barColor);
    osd->DrawRectangle(0, uh, hb, h - dh - 1,  barColor);
    osd->DrawRectangle(hb, uh, w - hb - 1, h - dh -1, clrTransparent);
    osd->DrawRectangle(w - hb, uh, w - 1, h - dh - 1,  barColor);
    osd->DrawRectangle(0, h - dh, w - 1, h - 1,  barColor);
#else
    osd->Fill(0, 0, w - 1, uh - 1,  clrBlack, window);
    osd->Fill(0, uh, hb, h - dh - 1,  clrBlack, window);
    osd->Fill(hb, uh, w - hb - 1, h - dh -1, clrTransparent, window);
    osd->Fill(w - hb, uh, w - 1, h - dh - 1,  clrBlack, window);
    osd->Fill(0, h - dh, w - 1, h - 1,  clrBlack, window);
#endif
    osd->Flush();
}

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
		    if (barUpHeight > barStep) barUpHeight -= barStep;
		    else  barUpHeight = 0;
		    }
		if (barDn) {
		    if (barDnHeight > barStep) barDnHeight -= barStep;
		    else  barDnHeight = 0;
		    }
		barClosed = false;
		break;
	    case kRight:
		if (barUp) {
		    if (barUpHeight < h / 2 - (barStep-1)) barUpHeight += barStep;
		    else  barUpHeight = h / 2 + 1;
		    }
		if (barDn) {
		    if (barDnHeight < h / 2 - (barStep-1)) barDnHeight += barStep;
		    else  barDnHeight = h / 2 + 1;
		    }
		barClosed = false;
		break;
	    case k0:
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
#if VDRVERSNUM >= 10307
		    osd->DrawRectangle(0, 0, w - 1, h - 1,  barColor);
#else
		    osd->Fill(0, 0, w - 1, h - 1,  clrBlack, window);
#endif
		    osd->Flush();
		    uh = dh = h / 2 + 1;
		    barClosed = true;
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
	    case kOk:
	    case kBack:
	    case kBlue:
		return osEnd;
	    default:
		return osContinue;
	    }
	Draw();
	}

    return state;
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
    int nHideMenuEntry;
protected:
    virtual void Store(void);
public:
    cMenuSetupCinebars(void);
};

void cMenuSetupCinebars::Store(void)
{
    barUp = nbarUp;
    barUpHeight = nbarUpHeight;
    barDn = nbarDn;
    barDnHeight = nbarDnHeight;
    barStep = nbarStep;
    osdMode = nosdMode;
    HideMenuEntry = nHideMenuEntry;
    SetupStore("TopBar", barUp);
    SetupStore("TopBarHeight", barUpHeight);
    SetupStore("BottomBar", barDn);
    SetupStore("BottomBarHeight", barDnHeight);
    SetupStore("StepHeight", barStep);
    SetupStore("OsdMode", osdMode);
    SetupStore("HideMenuEntry", HideMenuEntry);
}
  
cMenuSetupCinebars::cMenuSetupCinebars(void)
{
    nbarUp = barUp;
    nbarUpHeight = barUpHeight;
    nbarDn = barDn;
    nbarDnHeight = barDnHeight;
    nbarStep = barStep;
    nosdMode = osdMode;
    nHideMenuEntry = HideMenuEntry;

    Add(new cMenuEditBoolItem(tr("Hide mainmenu entry"), &nHideMenuEntry));
    Add(new cMenuEditBoolItem(tr("Top Bar"), 		 &nbarUp));
    Add(new cMenuEditIntItem(tr("Top BarHeight"), 	 &nbarUpHeight, 1, 200));
    Add(new cMenuEditBoolItem(tr("Bottom Bar"), 	 &nbarDn));
    Add(new cMenuEditIntItem(tr("Bottom BarHeight"), 	 &nbarDnHeight, 1, 200));
    Add(new cMenuEditIntItem(tr("StepHeight"), 		 &nbarStep, 1, 20));
    char *temp;
    asprintf(&temp, "%s (0=DVB/1=Softdevice)", tr("OSD-Mode"));
    Add(new cMenuEditIntItem(temp, 			 &nosdMode, 0, 1));
    free(temp);
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

cMenuSetupPage *cPluginCinebars::SetupMenu(void)
{
    // Return a setup menu in case the plugin supports one.
    return new cMenuSetupCinebars;
}

bool cPluginCinebars::SetupParse(const char *Name, const char *Value)
{
    // Parse your own setup parameters and store their values.
    if       (!strcasecmp(Name, "TopBar"))	    barUp = atoi(Value);
    else if  (!strcasecmp(Name, "TopBarHeight"))    barUpHeight = atoi(Value);
    else if  (!strcasecmp(Name, "BottomBar")) 	    barDn = atoi(Value);
    else if  (!strcasecmp(Name, "BottomBarHeight")) barDnHeight = atoi(Value);
    else if  (!strcasecmp(Name, "StepHeight")) 	    barStep = atoi(Value);
    else if  (!strcasecmp(Name, "OsdMode")) 	    osdMode = atoi(Value);
    else if  (!strcasecmp(Name, "HideMenuEntry"))   HideMenuEntry = atoi(Value);
    else return false;
    
    return true;
}


VDRPLUGINCREATOR(cPluginCinebars); // Don't touch this!
