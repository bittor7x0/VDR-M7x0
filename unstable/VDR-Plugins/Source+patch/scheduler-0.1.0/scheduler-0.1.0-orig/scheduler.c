/*
Copyright (C) 2007 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/taskman
*/

/*
 * taskman.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/menu.h>
#include <getopt.h>
#include <string>

#include "scheduler.h"
#include "menu_tasks.h"
#include "taskmanager_thread.h"
#include "log.h"
#include "i18n.h"
#include "menu_setup.h"
#include "task.h"

cSchedulerConfig SchedulerConfig;

static const char *VERSION        = "0.1.0";
static const char *DESCRIPTION    = trNOOP("Job scheduler and manager");
static const char *MAINMENUENTRY  = trNOOP("Job scheduler");
std::string cTaskLog::logDirectory = "";
char *ConfigDir = NULL;


// LogFile declaration and statics
cLogFile LogFile;
char *cLogFile::LogFileName = NULL;
int cLogFile::loglevellimit = 0;


cPluginScheduler::cPluginScheduler(void)
{
}

cPluginScheduler::~cPluginScheduler()
{
   cTaskManagerThread::Exit();
   if (ConfigDir) free(ConfigDir);
}

const char* cPluginScheduler::Version(void) 
{ 
   return VERSION; 
}

const char* cPluginScheduler::Description(void) 
{ 
   return tr(DESCRIPTION); 
}

const char *cPluginScheduler::MainMenuEntry(void)
{
  if (SchedulerConfig.hidemenu)
    return NULL;
  else
    return tr(MAINMENUENTRY); 
}

const char *cPluginScheduler::CommandLineHelp(void)
{
   return "  -l file,  --logfile=file to specify a specific logfile for scheduler\n"
     "  -v n,  --verbose=n       verbose level for logfile\n"
     "  -d direcoty, --logdirectory=directory save task logfile in the given directory";
}

bool cPluginScheduler::ProcessArgs(int argc, char *argv[])
{
  static const struct option long_options[] = {
    { "logfile",        required_argument, NULL, 'l' },
    { "verbose",        required_argument, NULL, 'v' },
    { "logdirectory",   required_argument, NULL, 'd' },
    { NULL }
  };
  
  int c=0,i=0;
  
  while ((c = getopt_long(argc, argv, "l:v:d:", long_options, &i)) != -1) {
    switch (c) {
    case 'l': 
      cLogFile::LogFileName = optarg;
      break;
    case 'v': 
      cLogFile::loglevellimit = atoi(optarg);
      break;
    case 'd': 
      cTaskLog::logDirectory = optarg;
      break;
    }
  }
  
  return true;
}

bool cPluginScheduler::Initialize(void)
{
#if APIVERSNUM < 10507
   RegisterI18n(Phrases);
#endif
  return true;
}

bool cPluginScheduler::Start(void)
{
  if (!ConfigDir)
    ConfigDir = strdup(cPlugin::ConfigDirectory("scheduler"));

  if (cLogFile::LogFileName)
    LogFile.Open(cLogFile::LogFileName, Version());
  else
    LogFile.Open(AddDirectory(CONFIGDIR, "scheduler.log"), Version());

  if (cTaskLog::logDirectory.empty())
    cTaskLog::logDirectory = ConfigDir;

  Tasks.Load(AddDirectory(CONFIGDIR, "tasks.conf"), true);

  cTaskManagerThread::Init();  

  return true;
}

void cPluginScheduler::Stop(void)
{
}

void cPluginScheduler::Housekeeping(void)
{
}

void cPluginScheduler::MainThreadHook(void)
{
}

cString cPluginScheduler::Active(void)
{
  return Tasks.Active();
}

time_t cPluginScheduler::WakeupTime(void)
{
  return Tasks.NextWakeupTime();
}

cOsdObject *cPluginScheduler::MainMenuAction(void)
{
  return new cMenuTasks();
}

cMenuSetupPage *cPluginScheduler::SetupMenu(void)
{
  return new cMenuTaskmanSetup;
}

bool cPluginScheduler::SetupParse(const char *Name, const char *Value)
{
  if (!strcasecmp(Name, "HideMenu")) SchedulerConfig.hidemenu = atoi(Value);
  return true;
}

bool cPluginScheduler::Service(const char *Id, void *Data)
{
  return false;
}

cSchedulerConfig::cSchedulerConfig(void)
{
  hidemenu = 0;
}


VDRPLUGINCREATOR(cPluginScheduler); // Don't touch this!
