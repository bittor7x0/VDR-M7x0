/*
 * solitaire.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
// originally from freecell by Sascha Volkenandt (sascha@akv-soft.de)

#include <vdr/plugin.h>
#include "game.h"
#include "i18n.h"

static const char *VERSION        = "0.0.3";
static const char *DESCRIPTION    = "The well-known card game";
static const char *MAINMENUENTRY  = "Solitaire";

class cPluginSolitaire : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginSolitaire(void);
  virtual ~cPluginSolitaire();
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

cPluginSolitaire::cPluginSolitaire(void)
{
}

cPluginSolitaire::~cPluginSolitaire()
{
}

bool cPluginSolitaire::Initialize(void)
{
	return true;
}

bool cPluginSolitaire::Start(void)
{
	RegisterI18n(Phrases);
	return true;
}

void cPluginSolitaire::Housekeeping(void)
{
	// Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginSolitaire::MainMenuAction(void)
{
	return new cSolitaireGame;
}

cMenuSetupPage *cPluginSolitaire::SetupMenu(void)
{
	// Return a setup menu in case the plugin supports one.
	return NULL;
}

bool cPluginSolitaire::SetupParse(const char *Name, const char *Value)
{
	// Parse your own setup parameters and store their values.
	return false;
}

VDRPLUGINCREATOR(cPluginSolitaire); // Don't touch this!
