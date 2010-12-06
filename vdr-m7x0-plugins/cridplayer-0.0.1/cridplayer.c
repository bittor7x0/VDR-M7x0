/*
 * cridplayer.c: A plugin for the Video Disk Recorder
 *
 * Copyright (C) 2006 Andreas Koch - the open7x0.org group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 * Some parts are taken from vdr main sources.
 *
 * $Id: cridplayer.c 42 2007-04-30 14:25:31Z andreas $
 *
 */

#include <vdr/plugin.h>
#include "cridplayer_i18n.h"
#include "cridplayer_controller.h"
#include "cridplayer_menu.h"

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "A player for Crid-Recordings";
static const char *MAINMENUENTRY  = "CridPlayer";

class cPluginCridplayer : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cCridController *controller;
  cCridRemoveDeleted *cridCleaner;
public:
  cPluginCridplayer(void);
  virtual ~cPluginCridplayer();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginCridplayer::cPluginCridplayer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  controller = NULL;
  cridCleaner = NULL;
}

cPluginCridplayer::~cPluginCridplayer()
{
  // Clean up after yourself!
  delete controller;
}

const char *cPluginCridplayer::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginCridplayer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginCridplayer::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  RegisterI18n(cridplayer_phrases);
  delete controller;
  controller = new cCridController();
  return controller;
}

bool cPluginCridplayer::Start(void)
{
  // Start any background activities the plugin shall perform.
  cridCleaner = new cCridRemoveDeleted();
  return cridCleaner;

}

void cPluginCridplayer::Stop(void)
{
   // Stop any background activities the plugin shall perform.
  SetupStore("LastPath", controller->GetCurrentPath());
  delete cridCleaner;
  cridCleaner = NULL;

}

void cPluginCridplayer::Housekeeping(void)
{
  //dsyslog("Cridplayer: Housekeeping");
  // Perform any cleanup or other regular tasks.
  if (cridCleaner)
     cridCleaner->StartRemoveOnNeed();
}

void cPluginCridplayer::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginCridplayer::Active(void)
{
  // Return a message string if shutdown should be postponed
  //dsyslog("Cridplayer: Active");
  return NULL;
}

cOsdObject *cPluginCridplayer::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.

  return new cCridMenuRecordings(controller, controller->GetSeriesSelected()
              && controller->GetRecordingSelected());
}

cMenuSetupPage *cPluginCridplayer::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cCridSetupMenu(controller);
}

bool cPluginCridplayer::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp("PathBase",Name)) {
          if (controller)
             controller->SetBasePath(Value);
          else
             CridplayerSetup.SetBasePath(Value);
          }
  else if (!strcasecmp("LastPath",Name)) {
          if (controller)
             controller->SetInitPath(Value);
          else
             CridplayerSetup.SetLastPath(Value);
          }
  else if (!strcasecmp("ShowHiddenFiles",Name)) {
          if (controller)
             controller->SetShowHidden(atoi(Value));
          else
             CridplayerSetup.ShowHidden=atoi(Value);
          }
  else if (!strcasecmp("StartMargin",Name)) CridplayerSetup.RecordingPreGap = atoi(Value);
  else if (!strcasecmp("StopMargin",Name)) CridplayerSetup.RecordingPostGap = atoi(Value);
  else if (!strcasecmp("DirectoryMarkingPrefix",Name)) {
     strncpy(CridplayerSetup.DirMarkerPre,Value,MAXMARKERLEN);
     CridplayerSetup.DirMarkerPre[MAXMARKERLEN] = 0;
     }
  else if (!strcasecmp("DirectoryMarkingSuffix",Name)) {
     strncpy(CridplayerSetup.DirMarkerPost,Value,MAXMARKERLEN);
     CridplayerSetup.DirMarkerPost[MAXMARKERLEN] = 0;
     }
  else return false;
  return true;
}

bool cPluginCridplayer::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginCridplayer::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginCridplayer::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginCridplayer); // Don't touch this!
