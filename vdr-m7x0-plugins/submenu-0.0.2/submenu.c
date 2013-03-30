#include "submenu.h"
#include "submenu-setup.h"
#include "i18n.h"

cPluginSubMenu::cPluginSubMenu(void)
{
}

cPluginSubMenu::~cPluginSubMenu(void)
{
}

bool cPluginSubMenu::Initialize(void)
{
	RegisterI18n(Phrases);
	return true;
}

const char *cPluginSubMenu::CommandLineHelp(void)
{
	return NULL;
}

bool cPluginSubMenu::ProcessArgs(int argc, char *argv[])
{
	return true;
}

bool cPluginSubMenu::Start(void)
{
	return true;
}

cMenuSetupPage *cPluginSubMenu::SetupMenu(void)
{
	return new cSubMenuSetup;
}

bool cPluginSubMenu::SetupParse(const char *Name, const char *Value)
{
	return true;
}

VDRPLUGINCREATOR(cPluginSubMenu);
