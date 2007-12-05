/*
 * Spider-Arachnid: A plugin for the Video Disk Recorder
 *
 * Copyright (C) 2005-2007, Thomas Günther <tom@toms-cafe.de>
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
 * $Id: spider.c 88 2007-06-23 00:15:39Z tom $
 */

#include "spider.h"
#include "game.h"
#include "setup.h"
#include "i18n.h"
#include <vdr/plugin.h>


static const char* VERSION       = "0.1.4";
static const char* DESCRIPTION   = "Spider Arachnid - the best patience game";
static const char* MAINMENUENTRY = "Spider Arachnid";


/** --- class SpiderPlugin ------------------------------------------------- **/

class SpiderPlugin : public cPlugin
{
private:
  SpiderSetup setup;
public:
  virtual const char* Version() { return VERSION; }
  virtual const char* Description() { return tr(DESCRIPTION); }
  virtual bool Start();
  virtual const char* MainMenuEntry() { return tr(MAINMENUENTRY); }
  virtual cOsdObject* MainMenuAction();
  virtual cMenuSetupPage* SetupMenu();
  virtual bool SetupParse(const char* name, const char* value);
};

bool SpiderPlugin::Start()
{
  RegisterI18n(Phrases);
  return true;
}

cOsdObject* SpiderPlugin::MainMenuAction()
{
  return new SpiderGame(setup, ConfigDirectory(Name()));
}

cMenuSetupPage* SpiderPlugin::SetupMenu()
{
  return new SpiderSetupMenu(setup);
}

bool SpiderPlugin::SetupParse(const char* name, const char* value)
{
  return setup.parse(name, value);
}

VDRPLUGINCREATOR(SpiderPlugin); // Don't touch this!
