/*
 * quickepgsearch.c: A plugin for the Video Disk Recorder
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
static const char DESCRIPTION[]    = "Quick search for broadcasts";
static const char MAINMENUENTRY[]  = "Quick search";
static const char SETUPTEXT[]      = "Show in main menu";


const tI18nPhrase Phrases[] =
{
    {
        "Quick search for broadcasts",
        "Schnelle Suche nach Sendungen",// Deutsch
        "",// TODO 
        "",// TODO
        "",// TODO
        "",// TODO 
        "",// TODO
        "",// TODO 
        "Pikahaku ohjelmaoppaalle",// suomi
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
        "Quick search",
        "Schnellsuche",
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "Pikahaku",
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
        "Show in main menu",
        "Im Hauptmen¸ anzeigen",
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "N‰yt‰ valinta p‰‰valikossa",
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
  
class cPluginQuicksearch : public cMainMenuShortcut
{
    public:
        virtual const char* Version() { return VERSION; }
        virtual const char* Description() { return tr(DESCRIPTION); }
        virtual bool Initialize();
        virtual cOsdObject* MainMenuAction() { return GetEpgSearchMenu("Epgsearch-quicksearch-v1.0"); };

    protected:
        virtual const char* SetupText() { return tr(SETUPTEXT); }
        virtual const char* MainMenuText() { return tr(MAINMENUENTRY); };
};

bool cPluginQuicksearch::Initialize()
{
    RegisterI18n(Phrases);
    return cMainMenuShortcut::Initialize();
}

VDRPLUGINCREATOR(cPluginQuicksearch); // Don't touch this!
