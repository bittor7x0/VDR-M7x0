/*
 * Spider-Arachnid: A plug-in for the Video Disk Recorder
 *
 * Copyright (C) 2005-2008, Thomas Günther <tom@toms-cafe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id: spider.cpp 168 2008-11-30 00:10:05Z tom $
 */

#include "spider.h"
namespace Spider { class Deck; class Tableau; }
#include "game.h"
#include "setup.h"
#include "i18n.h"
#include <vdr/plugin.h>
#include <vdr/config.h>


/** 'Spider Arachnid' is a VDR plugin implementation of a patience game. */
namespace SpiderPlugin
{

  /** Version number of the plugin */
  static const char* VERSION = "0.2.2";

  /** Short description of the plugin's purpose */
  static const char* DESCRIPTION =
    trNOOP("Spider Arachnid - the best patience game");

  /** Name of the entry in VDR's main menu */
  static const char* MAINMENUENTRY = trNOOP("Spider Arachnid");


  //--- class SpiderPlugin::Plugin ---------------------------------------------

  /** Main class of the VDR plugin 'Spider' */
  class Plugin : public cPlugin
  {
    SetupData setup;
    Spider::Deck* deck;
    Spider::Tableau* tableau;

  public:

    /** Version number of the plugin */
    virtual const char* Version() { return VERSION; }

    /** Localized short description of the plugin's purpose */
    virtual const char* Description() { return tr(DESCRIPTION); }

    /** Perform the startup actions of the plugin. */
    virtual bool Start();

    /** Localized name of the entry in VDR's main menu */
    virtual const char* MainMenuEntry() { return tr(MAINMENUENTRY); }

    /** OSD object that shows the plugin's main menu */
    virtual cOsdObject* MainMenuAction();

    /** Setup menu page to adjust the setup parameters of the plugin */
    virtual cMenuSetupPage* SetupMenu();

    /** Parse the setup parameters of the plugin. */
    virtual bool SetupParse(const char* name, const char* value);
  };

} // namespace SpiderPlugin


using namespace SpiderPlugin;


//--- class SpiderPlugin::Plugin -----------------------------------------------

/** Perform the startup actions of the plugin.
 *
 * This method is called once at VDR's startup.
 */
bool Plugin::Start()
{
#if VDRVERSNUM < 10507
  RegisterI18n(Phrases);
#endif
  deck = NULL;
  tableau = NULL;
  return true;
}

/** OSD object that shows the plugin's main menu
 *
 * This method is called every time the plugin's main menu entry is selected.
 */
cOsdObject* Plugin::MainMenuAction()
{
  return new Game(setup, ConfigDirectory(Name()), deck, tableau);
}

/** Setup menu page to adjust the setup parameters of the plugin
 *
 * This method is called every time the plugin's setup menu entry is selected.
 */
cMenuSetupPage* Plugin::SetupMenu()
{
  return new SetupPage(setup);
}

/** Parse the setup parameters of the plugin.
 *
 * This method is called for each setup parameter the plugin has previously
 * stored in the global setup data.
 */
bool Plugin::SetupParse(const char* name, const char* value)
{
  return setup.parse(name, value);
}

/** "Magic" hook that allows VDR to load the plugin into its memory */
VDRPLUGINCREATOR(Plugin); // Don't touch this!
