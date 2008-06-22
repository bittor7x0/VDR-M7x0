/*
 * wapd.c: A plugin for the Video Disk Recorder
 *
 * WAP remote control plugin for VDR (the Video Disk Recorder)
 * Copyright (C) 2002  Thomas Heiligenmann  <thomas@heiligenmann.de>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA
 *
 * $Id$
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include <vdr/tools.h>
#include "server.h"
#include "i18n.h"
#include "access.h"

static const char *VERSION        = "0.9";
static const char *DESCRIPTION    = "Remote control by WAP";


class cPluginWapd : public cPlugin {
private:
  cWapServer *server;
  int tcpport;
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
  server = NULL;
  tcpport = 8888;
}

cPluginWapd::~cPluginWapd()
{
  delete server;
}

const char *cPluginWapd::CommandLineHelp(void)
{
  return "  -p PORT,  --port=PORT    use PORT for WAP (default: 8888)\n";
}

bool cPluginWapd::ProcessArgs(int argc, char *argv[])
{
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
  RegisterI18n(Phrases);
  if (WapHosts.Load(AddDirectory(ConfigDirectory(), "waphosts"), true)
     && WapAccess.Load(AddDirectory(ConfigDirectory(), "wapaccess"), true)
     && (server = new cWapServer(tcpport)))
     return true;
  return false;
}

cMenuSetupPage *cPluginWapd::SetupMenu(void)
{
  return NULL;
}

bool cPluginWapd::SetupParse(const char *Name, const char *Value)
{
  return false;
}

VDRPLUGINCREATOR(cPluginWapd);
