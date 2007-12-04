/*
 * conflictcheckonly.c: A plugin for the Video Disk Recorder
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
static const char DESCRIPTION[]    = "Direct access to epgsearch's conflict check menu";
static const char MAINMENUENTRY[]  = "Timer conflicts";
static const char SETUPTEXT[]      = "Conflict info in main menu";

const tI18nPhrase Phrases[] =
{
    {
        "Direct access to epgsearch's conflict check menu",
        "Direkter Zugriff auf epgsearch's Konflikt-Prüfungs-Menü",// Deutsch
        "",// TODO 
        "",// TODO
        "",// TODO
        "",// TODO 
        "",// TODO
        "",// TODO 
        "Suoratoiminto EPGSearch-laajennoksen ajastimien tarkistukselle",
        "",// TODO Polski
        "",// TODO Español
        "",// TODO ÅëëçíéêÜ (Greek)
        "",// TODO Svenska
        "",// TODO Romaneste
        "",// TODO Magyar
        "",// TODO Català
        "",// TODO ÀãááÚØÙ (Russian)
        "",// TODO Hrvatski (Croatian)
        "", // Eesti
        "", // Dansk
        "", // Czech
    },
    {
        "Timer conflicts",
        "Timer-Konflikte",
        "",// TODO
        "",// TODO
        "",
        "",// TODO
        "",
        "",// TODO
        "Ajastimien päällekkäisyydet",
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
        "next",
        "nächster",
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "seuraava",
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
        "Conflict info in main menu",
        "Konflikt-Info im Hauptmenü",
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "",// TODO
        "Näytä päällekkäisyydet päävalikossa",
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
  
cString DateTime(time_t t)
{
    char buffer[32];
    if (t == 0)
    {
        time(&t);
    }
    struct tm tm_r;
    tm *tm = localtime_r(&t, &tm_r);
    snprintf(buffer, sizeof(buffer), "%02d.%02d %02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_hour, tm->tm_min);
    return buffer;
}

class cPluginConflictcheckonly : public cMainMenuShortcut
{
    private:
        char* _menuText;

    public:
        cPluginConflictcheckonly();
        ~cPluginConflictcheckonly();
        virtual const char* Version() { return VERSION; }
        virtual const char* Description() { return tr(DESCRIPTION); }
        virtual bool Initialize();
        virtual cOsdObject* MainMenuAction() { return GetEpgSearchMenu("Epgsearch-conflictmenu-v1.0"); }

    protected:
        virtual const char* SetupText() { return tr(SETUPTEXT); }
        virtual const char* MainMenuText();
};

cPluginConflictcheckonly::cPluginConflictcheckonly()
:_menuText(NULL)
{
}

cPluginConflictcheckonly::~cPluginConflictcheckonly()
{
    free(_menuText);
}

const char* cPluginConflictcheckonly::MainMenuText(void)
{
    const char* menuText = tr(MAINMENUENTRY);
    cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (epgSearchPlugin)
    {
        Epgsearch_lastconflictinfo_v1_0* serviceData = new Epgsearch_lastconflictinfo_v1_0;
        if (epgSearchPlugin->Service("Epgsearch-lastconflictinfo-v1.0", serviceData))
        {
            if (serviceData->relevantConflicts > 0)
            {
                free(_menuText);
                asprintf(&_menuText, "%s (%d, %s: %s)", menuText, serviceData->relevantConflicts, 
                  tr("next"), *DateTime(serviceData->nextConflict));
                menuText = _menuText;
            }
        }
        delete serviceData;
    }
    return menuText;
}

bool cPluginConflictcheckonly::Initialize(void)
{
    RegisterI18n(Phrases);
    return cMainMenuShortcut::Initialize();
}

VDRPLUGINCREATOR(cPluginConflictcheckonly); // Don't touch this!
