#include "services.h"
#include "mainmenushortcut.h"

static const char SETUPENTRY[] = "MainMenuEntryEnabled";

const tI18nPhrase Phrases[] =
{
    {
        "This version of EPGSearch does not support this service!",
        "Diese Version von EPGSearch unterstützt diesen Dienst nicht!",
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "EPGSearch-laajennos ei tarjoa vaadittavaa palvelua!",
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "", // Eesti
        "", // Dansk
        "", // Czech
    },
    {
        "EPGSearch does not exist!",
        "EPGSearch nicht vorhanden!",
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "EPGSearch-laajennosta ei löydy!",
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "", // Eesti
        "", // Dansk
        "", // Czech
    },
    { NULL }
};

cMainMenuShortcutSetupPage::cMainMenuShortcutSetupPage(const char* setupText, const char* setupEntry,
  int* const setupValue)
:_setupEntry(setupEntry), _setupValue(setupValue)
{
    Add(new cMenuEditBoolItem(setupText, _setupValue, tr("no"), tr("yes")));
};
  
void cMainMenuShortcutSetupPage::Store()
{
    SetupStore(_setupEntry, *_setupValue);
}

cMainMenuShortcut::cMainMenuShortcut()
:_mainMenuEntryEnabled(1)
{
}

cMainMenuShortcut::~cMainMenuShortcut()
{
    I18nRegister(NULL, PLUGIN_NAME_I18N);
}

cOsdMenu* cMainMenuShortcut::GetEpgSearchMenu(const char* serviceName)
{
    cOsdMenu* menu = NULL;
    cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (epgSearchPlugin)
    {
        EpgSearchMenu_v1_0* serviceData = new EpgSearchMenu_v1_0;
 
        if (epgSearchPlugin->Service(serviceName, serviceData))
        {
            menu = serviceData->Menu;
        }
        else
        {
            Skins.Message(mtError, tr("This version of EPGSearch does not support this service!"));
        }
	
        delete serviceData;
    }
    else
    {
        Skins.Message(mtError, tr("EPGSearch does not exist!"));
    }
    return menu;
}

bool cMainMenuShortcut::Initialize()
{
    I18nRegister(Phrases, PLUGIN_NAME_I18N);
    return true;
}

bool cMainMenuShortcut::SetupParse(const char *Name, const char *Value)
{
    if (!strcasecmp(Name, SETUPENTRY))
    {
        _mainMenuEntryEnabled = atoi(Value);
    }
    return true;
}

cMenuSetupPage* cMainMenuShortcut::SetupMenu()
{
    return new cMainMenuShortcutSetupPage(SetupText(), SETUPENTRY, &_mainMenuEntryEnabled);
}

const char* cMainMenuShortcut::MainMenuEntry()
{
    if (_mainMenuEntryEnabled)
    {
        return (const char*) MainMenuText();
    }
    else
    {
        return NULL;
    }
}
