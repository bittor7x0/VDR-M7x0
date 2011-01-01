#include "aide.h"
#include "menuaide.h"
#include "aidei18n.h"

cPluginAide::cPluginAide(void)
{
}

cPluginAide::~cPluginAide(void)
{
}

bool cPluginAide::Initialize(void)
{
	return true;
}

const char *cPluginAide::CommandLineHelp(void)
{
	return NULL;
}

bool cPluginAide::ProcessArgs(int argc, char *argv[])
{
	return true;
}

bool cPluginAide::Start(void)
{
	RegisterI18n(aidePhrases);
	return true;
}

cOsdObject *cPluginAide::MainMenuAction(void)
{
	return new cMenuAide;
}

/*
cMenuSetupPage *cPluginAide::SetupMenu(void)
{
}


bool cPluginAide::SetupParse(const char *Name, const char *Value)
{
	return true;
}

void cPluginAide::Store(void)
{
}
*/


VDRPLUGINCREATOR(cPluginAide);
