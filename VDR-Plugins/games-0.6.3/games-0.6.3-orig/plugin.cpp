#include <plugin.h>
#include <osd.h>

extern int launcher_menu(char ***m);
extern int launcher_start(int g, int x, int y, int s, int c);
extern int launcher_stop(void);
extern int launcher_key(int key);

class GameMenu;
class GameStart;
class GameSetup;
class GamePlugin;

static const char   *DESCRIPTION = "OSD Games Collection";
static const char *MAINMENUENTRY = "Games";

static int  x = -8;    // x position
static int  y = -8;    // y position
static int  s =  2;    // osd size
static int  c =  2;    // computer skill
static int  g = -1;    // selected game

static bool m = true;  // open menu
static bool r = false; // repeat keys

class GameMenu : public cOsdMenu {
public:
	GameMenu(void);
	eOSState ProcessKey(eKeys k);
};

class GameStart : public cOsdObject {
public:
	~GameStart(void);
	void Show(void);
	eOSState ProcessKey(eKeys k);
};

class GameSetup : public cMenuSetupPage {
public:
	GameSetup(void);
protected:
	void Store(void);
private:
	int new_x, new_y, new_s, new_r, new_c;
};

class GamePlugin : public cPlugin {
public:
	bool SetupParse(const char *Name, const char *Value);
	const char     *Version(void)        { return VERSION;       }
	const char     *Description(void)    { return DESCRIPTION;   }
	const char     *MainMenuEntry(void)  { return MAINMENUENTRY; }
	cMenuSetupPage *SetupMenu(void)      { return new GameSetup; }
	cOsdObject     *MainMenuAction(void);
};

GameMenu::GameMenu(void) : cOsdMenu("Games") {
	char **menu;
	int games;

	SetHasHotkeys();
	games = launcher_menu(&menu);
	for (int i=0; i<games; i++) {
		Add(new cOsdItem(hk(menu[i])));
	}
	Display();
	m = true;
}

eOSState
GameMenu::ProcessKey(eKeys k) {
	eOSState state = cOsdMenu::ProcessKey(k);

	switch (state) {
		case osUnknown:
			switch (k) {
				case kRight:
				case kOk:
					m = false;
					g = Current();
					CloseSubMenu();
					// calls MainMenuAction() implicitly
					return (osPlugin);
				break;
				default: break;
			}
		break;
		case osBack:
			// close menu
			return (osBack);
		case osEnd:
			// close menu
			return (osEnd);
		break;
		default: break;
	}
	return (state);
}

void
GameStart::Show(void) {
	launcher_start(g, x, y, s, c);
	m = true;
}

GameStart::~GameStart(void) {
	launcher_stop();
	g = -1;
}

eOSState
GameStart::ProcessKey(eKeys k) {
	int key = (int)k;

	if (r) key &= 0x7fff;
	if (launcher_key(key)) return (osEnd);
	return (osContinue);
}

GameSetup::GameSetup(void) {
	new_s = s; Add(new cMenuEditIntItem(      "OSD Size", &new_s,     1,    3));
	new_x = x; Add(new cMenuEditIntItem(    "X Position", &new_x,    -9,    9));
	new_y = y; Add(new cMenuEditIntItem(    "Y Position", &new_y,    -9,    9));
	new_c = c; Add(new cMenuEditIntItem("Computer Skill", &new_c,     1,    3));
	new_r = r; Add(new cMenuEditBoolItem(   "Key Repeat", &new_r, "Off", "On"));
}

void
GameSetup::Store(void) {
	SetupStore("OsdSize",   s = new_s);
	SetupStore("XPosition", x = new_x);
	SetupStore("YPosition", y = new_y);
	SetupStore("Skill",     c = new_c);
	SetupStore("KeyRepeat", r = new_r);
}

cOsdObject *
GamePlugin::MainMenuAction(void) {
	if (m) return new GameMenu;
	return new GameStart;
}

bool
GamePlugin::SetupParse(const char *Name, const char *Value) {
	if      (!strcasecmp(Name, "OsdSize"))   s = atoi(Value);
	else if (!strcasecmp(Name, "XPosition")) x = atoi(Value);
	else if (!strcasecmp(Name, "YPosition")) y = atoi(Value);
	else if (!strcasecmp(Name, "Skill"))     c = atoi(Value);
	else if (!strcasecmp(Name, "KeyRepeat")) r = strcasecmp(Value, "Off");
	else return (false);
	return (true);
}

VDRPLUGINCREATOR(GamePlugin);
