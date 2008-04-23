/*
 * statussvdrp.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/tools.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include "status.h"

#define SVDRPEXT_BUFSIZE KILOBYTE(4)

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Extensions to the SVDRP command set";

class cPluginSvdrpExt : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cSvdrpExtStatus * status;
public:
  cPluginSvdrpExt(void);
  virtual ~cPluginSvdrpExt();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return NULL; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginSvdrpExt::cPluginSvdrpExt(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  status = NULL;
}

cPluginSvdrpExt::~cPluginSvdrpExt()
{
  // Clean up after yourself!
  if (status)
	  DELETENULL(status);
}

const char *cPluginSvdrpExt::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginSvdrpExt::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginSvdrpExt::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  status = new cSvdrpExtStatus();
  return true;
}

bool cPluginSvdrpExt::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginSvdrpExt::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginSvdrpExt::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginSvdrpExt::MainMenuAction(void)
{
  return NULL;
}

cMenuSetupPage *cPluginSvdrpExt::SetupMenu(void)
{
  return NULL;
}

bool cPluginSvdrpExt::SetupParse(const char *Name, const char *Value)
{
  return false;
}

bool cPluginSvdrpExt::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginSvdrpExt::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  static const char *HelpPages[] = {
	"OSDT\n"
	"    Return the current OSD title\n",
	"OSDI items_per_page\n"
	"    Return up to items_per_page OSD entries. If applicable, the\n"
	"    column layout is printed first, followed by the actual entries.\n"
	"    The layout part consists of one line per column. Each line starts\n"
	"    with the prefix C: followed by the column width.\n"
	"    The prefix I: is used for all items except for the selected item\n"
	"    which is prefixed by S:.\n",
	"OSDH\n"
	"    Return the current OSD color key help titles. Each color is\n"
	"    returned on a line of its own, prefixed by R:, G:, Y: or B:,\n"
	"    respectively. For unused help buttons only the prefix is returned.\n",
	"OSDM\n"
	"    Return the current OSD status message.\n",
	"OSDX\n"
	"    Return the textblock which is currently shown on the OSD.\n",
	NULL
  };
  return status ? HelpPages : NULL;
};

cString cPluginSvdrpExt::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  if (!status)
	  return NULL;

  ReplyCode = 920;
  if (strcasecmp(Command, "OSDT") == 0) {
	  if (status->title)
		  return status->title;
  }
  else if (strcasecmp(Command, "OSDI") == 0) {
	  int maxitems = (*Option) ? strtol(Option, NULL, 10) : 0;
	  if (maxitems > 0) {
		  if (status->items.Count() > 0) {
			  int length = 0;
			  int size = SVDRPEXT_BUFSIZE;
			  char *buffer = MALLOC(char, SVDRPEXT_BUFSIZE);

			  // columns
			  for (int i = 0; i < status->MaxTabs && status->tabs[i] != 0; i++)
				  length += sprintf(buffer + length, "C:%hu\r\n", status->tabs[i]);

			  // items
			  int n;
			  int index = status->selected > 0 ?
				  	status->selected - (status->selected % maxitems) : 0;
			  int last = index + maxitems;
			  cSvdrpExtItem *item = status->items.Get(index);
			  while (item && index < last) {
				  n = snprintf(buffer + length, size - length, "%c:%s\r\n", index == status->selected ? 'S' : 'I', item->Text());
				  if (n >= size - length) {
					  // overflow: realloc and try same item again
					  size += SVDRPEXT_BUFSIZE;
					  char *tmp = (char *) realloc(buffer, sizeof(char) * size);
					  if (!tmp) {
						  free(buffer);
						  ReplyCode = 451;
						  return cString::sprintf("Realloc of %d byte failed", size);
					  }
					  buffer = tmp;
				  }
				  else {
					  // item appended; move to next
					  length += n;
					  index++;
					  item = status->items.Next(item);
				  }
			  } // while (item && index < last)
			  return cString(buffer, true);

		  } // if (status->items.Count() > 0)

	  } // if (maxitems > 0)
	  else {
		  ReplyCode = 501;
		  return "Invalid or missing argument";
	  }
  }
  else if (strcasecmp(Command, "OSDH") == 0) {
	  const char *red = status->red;
	  const char *green = status->green;
	  const char *yellow = status->yellow;
	  const char *blue = status->blue;
	  if (red || green || yellow || blue)
		  return cString::sprintf("R:%s\r\nG:%s\r\nY:%s\r\nB:%s",
				  red ? red : "", green ? green : "",
				  yellow ? yellow : "", blue ? blue : "");
  }
  else if (strcasecmp(Command, "OSDM") == 0) {
	  if (status->message)
		  return status->message;
  }
  else if (strcasecmp(Command, "OSDX") == 0) {
	  if (status->text)
		  return status->text;
  }
  else {
	  return NULL;
  }

  //Command recognized but nothing to reply
  ReplyCode = 930;
  return "Requested information not available";
}

VDRPLUGINCREATOR(cPluginSvdrpExt); // Don't touch this!
