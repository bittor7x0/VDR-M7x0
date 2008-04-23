/*
 * control.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE


#include <getopt.h>

#include <vdr/plugin.h>

#include "global.h"
#include "gateways.h"
#include "gateway.h"
#include "state.h"


static const char *VERSION        = "0.0.2a";
static const char *DESCRIPTION    = "Control VDR over terminal or telnet";
static const char *MAINMENUENTRY  = NULL; //"Control";


const char* glParTerminal = "";


class cPluginControl : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cCtrlGateways* _pReceivers;
  cCtrlGateway*  _pTerminal;

public:
  cPluginControl(void);
  virtual ~cPluginControl();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };



cPluginControl::cPluginControl(void)
: _pReceivers( 0 ),
  _pTerminal ( 0 )
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}



cPluginControl::~cPluginControl()
{
  // Clean up after yourself!
  delete(_pReceivers);
  delete(_pTerminal);
}



const char *cPluginControl::CommandLineHelp(void) {

  // Return a string that describes all known command line options.

  //                                            for easier orientation, this is column 80|
  return "  -t TTY,   --terminal=TTY tty to control vdr per virtual terminal\n"
         "                           (default: none)\n"
         "  -p PORT,  --port=PORT    port to receive remote connections\n"
         "                           (default: 2002)\n";
}



bool cPluginControl::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.

  static struct option long_options[] = {
      { "terminal", required_argument, NULL, 't' },
      { "port",     required_argument, NULL, 'p' },
      { NULL }
    };

  int c, option_index = 0;
  while ((c = getopt_long(argc, argv, "t:p:", long_options, &option_index)) != -1) {
    switch (c) {
      case 't': glParTerminal = optarg; break;
      case 'p': glParPort     = atoi(optarg); break;
      default:  return false;
      }
    }
  return true;
}



bool cPluginControl::Initialize(void) {
  // Initialize any background activities the plugin shall perform.
  return true;
}



bool cPluginControl::Start(void) {
  // Start any background activities the plugin shall perform.

  if (glParPort || glParTerminal && *glParTerminal)
    new cCtrlState();

  if (glParPort) {
    _pReceivers = new cCtrlGateways();
    if (_pReceivers)
      _pReceivers->Start();
  }


  if (glParTerminal && *glParTerminal) {

    int terminal = open(glParTerminal, O_RDWR);
    if (terminal >= 0) {

      _pTerminal = new cCtrlGateway(terminal);
      if (_pTerminal) {
        _pTerminal->Start();
        conDSYSLOG( "terminal opened %s (%i)", glParTerminal, terminal );
      }

    } else
      conESYSLOG( "Error opening terminal %s", glParTerminal );
  }

  return true;
}



void cPluginControl::Housekeeping(void) {
  // Perform any cleanup or other regular tasks.
}



cOsdObject *cPluginControl::MainMenuAction(void) {
  // Perform the action when selected from the main VDR menu.
  return NULL;
}



cMenuSetupPage *cPluginControl::SetupMenu(void) {
  // Return a setup menu in case the plugin supports one.
  return NULL;
}



bool cPluginControl::SetupParse(const char *Name, const char *Value) {
  // Parse your own setup parameters and store their values.
  return false;
}



VDRPLUGINCREATOR(cPluginControl); // Don't touch this!


