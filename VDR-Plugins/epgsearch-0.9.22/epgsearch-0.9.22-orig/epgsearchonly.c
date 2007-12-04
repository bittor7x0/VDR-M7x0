/*
 * epgsearchonly.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
#include <string>
#include <list>
#include <vdr/plugin.h>
#include <vdr/i18n.h>
#include "services.h"
#include "mainmenushortcut.h"

static const char VERSION[]        = "0.0.1";
static const char DESCRIPTION[]    = "Direct access to epgsearch's search menu";
static const char MAINMENUENTRY[]  = "Search";
static const char SETUPTEXT[]      = "EpgSearch-Search in main menu";

const tI18nPhrase Phrases[] =
{
    {
        "Direct access to epgsearch's search menu",
        "Direkter Zugriff auf epgsearch's Suchenmenu",// Deutsch
        "",// TODO 
        "",// TODO
        "",// TODO
        "",// TODO 
        "",// TODO
        "",// TODO 
        "Suoratoiminto EPGSearch-laajennoksen haulle",
        "",// TODO Polski
        "",// TODO EspaÒol
        "",// TODO ≈ÎÎÁÌÈÍ‹ (Greek)
        "",// TODO Svenska
        "",// TODO Romaneste
        "",// TODO Magyar
        "",// TODO Catal‡
        "",// TODO ¿„··⁄ÿŸ (Russian)
        "",// TODO Hrvatski (Croatian)
        "", // Eesti
        "", // Dansk
        "", // Czech
    },
    {
        "Search",
        "Suche",
        "",// TODO
        "Ricerca",
        "Zoek",
        "",// TODO
        "Recherche",
        "",// TODO
        "Etsi ohjelmaoppaasta",
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
        "EpgSearch-Search in main menu",
        "EpgSearch-Suche im Hauptmen¸",// Deutsch
        "",// TODO 
        "",// TODO
        "",// TODO
        "",// TODO 
        "",// TODO
        "",// TODO 
        "",// TODO Finnish
        "",// TODO Polski
        "",// TODO EspaÒol
        "",// TODO ≈ÎÎÁÌÈÍ‹ (Greek)
        "",// TODO Svenska
        "",// TODO Romaneste
        "",// TODO Magyar
        "",// TODO Catal‡
        "",// TODO ¿„··⁄ÿŸ (Russian)
        "",// TODO Hrvatski (Croatian)
        "", // Eesti
        "", // Dansk
        "", // Czech
    },
    { NULL }
};
  
class cPluginEpgsearchonly: public cMainMenuShortcut
{
    public:
        virtual const char* Version() { return VERSION; }
        virtual const char* Description() { return tr(DESCRIPTION); }
        virtual bool Initialize();
        virtual cOsdObject *MainMenuAction() { return GetEpgSearchMenu("Epgsearch-searchmenu-v1.0"); };

    protected:
        virtual const char* SetupText() { return tr(SETUPTEXT); }
        virtual const char* MainMenuText() { return tr(MAINMENUENTRY); };
};

bool cPluginEpgsearchonly::Initialize()
{
    RegisterI18n(Phrases);
    return cMainMenuShortcut::Initialize();
}

VDRPLUGINCREATOR(cPluginEpgsearchonly); // Don't touch this!
