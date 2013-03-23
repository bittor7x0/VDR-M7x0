/*
 *      vdr-plugin-xvdr - XVDR server plugin for VDR
 *
 *      Copyright (C) 2010 Alwin Esch (Team XBMC)
 *      Copyright (C) 2010, 2011 Alexander Pipelka
 *
 *      https://github.com/pipelka/vdr-plugin-xvdr
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include "xvdr.h"

cPluginXVDRServer::cPluginXVDRServer(void)
{
  Server = NULL;
}

cPluginXVDRServer::~cPluginXVDRServer()
{
  // Clean up after yourself!
}

const char *cPluginXVDRServer::CommandLineHelp(void)
{
    return "  -t n, --timeout=n      stream data timeout in seconds (default: 10)\n";
}

bool cPluginXVDRServer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  static struct option long_options[] = {
       { "timeout",  required_argument, NULL, 't' },
       { NULL,       no_argument,       NULL,  0  }
     };

  int c;

  while ((c = getopt_long(argc, argv, "t:", long_options, NULL)) != -1) {
        switch (c) {
          case 't': if(optarg != NULL) XVDRServerConfig.stream_timeout = atoi(optarg);
                    break;
          default:  return false;
          }
        }
  return true;
}

bool cPluginXVDRServer::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  XVDRServerConfig.ConfigDirectory = ConfigDirectory(PLUGIN_NAME_I18N);
  XVDRServerConfig.Load();
  return true;
}

bool cPluginXVDRServer::Start(void)
{
  Server = new cXVDRServer(XVDRServerConfig.listen_port);

  return true;
}

void cPluginXVDRServer::Stop(void)
{
  delete Server;
  Server = NULL;
}

void cPluginXVDRServer::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginXVDRServer::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginXVDRServer::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginXVDRServer::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cMenuSetupPage *cPluginXVDRServer::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginXVDRServer::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

bool cPluginXVDRServer::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginXVDRServer::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginXVDRServer::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginXVDRServer); // Don't touch this!
