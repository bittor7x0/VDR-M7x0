/*
Copyright (C) 2004-2008 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#include <string>
#include <list>
#include <memory>
#include <set>
#include <vdr/plugin.h>
#include "i18n.h"
#include "services.h"
#include "mainmenushortcut.h"

static const char SETUPENTRY[] = "MainMenuEntryEnabled";

#if APIVERSNUM < 10507
const tI18nPhrase Phrases[] = {
  {
   "This version of EPGSearch does not support this service!",
   "Diese Version von EPGSearch unterst�tzt diesen Dienst nicht!",
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "EPGSearch-laajennos ei tarjoa vaadittavaa palvelua!",
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// Eesti
   "",				// Dansk
   "",				// Czech
   },
  {
   "EPGSearch does not exist!",
   "EPGSearch nicht vorhanden!",
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "EPGSearch-laajennosta ei l�ydy!",
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// TODO
   "",				// Eesti
   "",				// Dansk
   "",				// Czech
   },
  {NULL}
};
#endif

cMainMenuShortcutSetupPage::cMainMenuShortcutSetupPage(const char *setupText,
						       const char *setupEntry,
						       int *const setupValue)
:_setupEntry(setupEntry), _setupValue(setupValue)
{
  Add(new cMenuEditBoolItem(setupText, _setupValue, trVDR("no"), trVDR("yes")));
}

void cMainMenuShortcutSetupPage::Store()
{
  SetupStore(_setupEntry, *_setupValue);
}

cMainMenuShortcut::cMainMenuShortcut()
:  _mainMenuEntryEnabled(1)
{
}

cMainMenuShortcut::~cMainMenuShortcut()
{
#if APIVERSNUM < 10507
  I18nRegister(NULL, PLUGIN_NAME_I18N);
#endif
}

cOsdMenu *cMainMenuShortcut::GetEpgSearchMenu(const char *serviceName)
{
  cOsdMenu *menu = NULL;
  cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
  if (epgSearchPlugin) {
    EpgSearchMenu_v1_0 *serviceData = new EpgSearchMenu_v1_0;

    if (epgSearchPlugin->Service(serviceName, serviceData)) {
      menu = serviceData->Menu;
    } else {
      Skins.Message(mtError, tr("This version of EPGSearch does not support this service!"));
    }

    delete serviceData;
  } else {
    Skins.Message(mtError, tr("EPGSearch does not exist!"));
  }
  return menu;
}

bool cMainMenuShortcut::Initialize()
{
#if APIVERSNUM < 10507
  I18nRegister(Phrases, PLUGIN_NAME_I18N);
#endif
  return true;
}

bool cMainMenuShortcut::SetupParse(const char *Name, const char *Value)
{
  if (!strcasecmp(Name, SETUPENTRY)) {
    _mainMenuEntryEnabled = atoi(Value);
  }
  return true;
}

cMenuSetupPage *cMainMenuShortcut::SetupMenu()
{
  return new cMainMenuShortcutSetupPage(SetupText(), SETUPENTRY, &_mainMenuEntryEnabled);
}

const char *cMainMenuShortcut::MainMenuEntry()
{
  if (_mainMenuEntryEnabled) {
    return (const char *) MainMenuText();
  } else {
    return NULL;
  }
}
