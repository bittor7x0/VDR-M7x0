/*
 * freecell.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include "game.h"
#include "i18n.h"

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = "The well-known card game";
static const char *MAINMENUENTRY  = "Freecell";

class cPluginFreecell : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginFreecell(void);
  virtual ~cPluginFreecell();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return tr(MAINMENUENTRY); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginFreecell::cPluginFreecell(void)
{
}

cPluginFreecell::~cPluginFreecell()
{
}

bool cPluginFreecell::Initialize(void)
{
  return true;
}

bool cPluginFreecell::Start(void)
{
	RegisterI18n(Phrases);
  return true;
}

void cPluginFreecell::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginFreecell::MainMenuAction(void)
{
  return new cFreecellGame;
}

cMenuSetupPage *cPluginFreecell::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginFreecell::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

VDRPLUGINCREATOR(cPluginFreecell); // Don't touch this!
