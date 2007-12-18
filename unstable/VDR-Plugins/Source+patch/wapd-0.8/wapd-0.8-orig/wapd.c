/*
 * wapd.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include <vdr/tools.h>
#include "server.h"
#include "i18n.h"
#include "access.h"

static const char *VERSION        = "0.8";
static const char *DESCRIPTION    = "Remote control by WAP";


class cPluginWapd : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cWapServer *server;
  int tcpport;
protected:
  virtual void Stop(void);
public:
  cPluginWapd(void);
  virtual ~cPluginWapd();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginWapd::cPluginWapd(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  server = NULL;
  tcpport = 8888;
}

cPluginWapd::~cPluginWapd()
{
  // Clean up after yourself!
  delete server;
}

const char *cPluginWapd::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return "  -p PORT,  --port=PORT    use PORT for WAP (default: 8888)\n";
}

bool cPluginWapd::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  static struct option long_options[] = {
        { "port", required_argument, NULL, 'p' },
        { NULL }
     };
  int c;
  while ((c = getopt_long(argc, argv, "p:", long_options, NULL)) != -1) {
     switch (c) {
        case 'p': if (isnumber(optarg))
                     tcpport = atoi(optarg);
                  break;
        default:  return false;
        }
     }
  return true;
}

bool cPluginWapd::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  if (WapHosts.Load(AddDirectory(ConfigDirectory(), "waphosts"), true)
     && WapAccess.Load(AddDirectory(ConfigDirectory(), "wapaccess"), true)
     && (server = new cWapServer(tcpport)))
     return true;
  return false;
}

cMenuSetupPage *cPluginWapd::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginWapd::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

void cPluginWapd::Stop(void)
{
  if (server)
     server->Shutdown();
}

VDRPLUGINCREATOR(cPluginWapd); // Don't touch this!
