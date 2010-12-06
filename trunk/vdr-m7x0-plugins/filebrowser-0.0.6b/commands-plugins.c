/*
*  C Implementation: commands-plugins
*
* Description: 
*
*
* Author: Holger Brunn <holger.brunn@stud.uni-karlsruhe.de>, (C) 2006
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include <vdr/plugin.h>
#include <fnmatch.h>
#include "commands-plugins.h"

/*
 * cFilebroserCommandAnyPlugin
 */

cFilebrowserCommandAnyPlugin::cFilebrowserCommandAnyPlugin(const char* Name)
 : cFilebrowserCommand(NULL)
{
  this->Name=strdup(Name);
}

bool cFilebrowserCommandAnyPlugin::Matches(const char* Filename)
{
  return ServiceAvailable && Filename && (fnmatch(*Extensions, basename(Filename), FNM_FILE_NAME | FNM_EXTMATCH | FNM_CASEFOLD) != FNM_NOMATCH);
}

bool cFilebrowserCommandAnyPlugin::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  if(ServiceAvailable && cPluginManager::CallFirstService(ServiceID, CurrentFile))
  {
    State=osEnd;
    return true;
  }
  return false;
}

/*
 * cFilebrowserCommandImagePlugin
 */
cFilebrowserCommandImagePlugin::cFilebrowserCommandImagePlugin() : cFilebrowserCommandAnyPlugin(tr("View"))
{
  Extensions="+(*.jpg|*.jpeg|*.jif|*.jiff|*.tif|*.tiff|*.gif|*.bmp|*.png|*.pnm|*.mps)";
  ServiceID="Image-ShowImage-v1.0";
  ServiceAvailable=cPluginManager::CallFirstService(*ServiceID, NULL);
}

/*
 * cFilebroserCommandMPlayerPlugin
 */

cFilebrowserCommandMPlayerPlugin::cFilebrowserCommandMPlayerPlugin() : cFilebrowserCommandAnyPlugin(tr("Play"))
{
  Extensions="+(*.avi|*.bin|*.mpg|*.mpeg|*.ogm)";
  ServiceID="MPlayer-Play-v1";
  ServiceAvailable=cPluginManager::CallFirstService(*ServiceID, NULL);
}

bool cFilebrowserCommandMPlayerPlugin::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  if(!ServiceAvailable)
  {
    return false;
  }
  MPlayerServiceData ServiceData;
  ServiceData.data.filename=CurrentFile;
  if(cPluginManager::CallFirstService(ServiceID, &ServiceData))
  {
    if(ServiceData.result)
    {
      State=osEnd;
      return true;
    }
  }
  return false;
}

bool cFilebrowserCommandMPlayerPlugin::Matches(const char* Filename)
{
  if(!cFilebrowserCommandAnyPlugin::Matches(Filename))
  {
    return false;
  }
  MPlayerServiceData ServiceData;
  ServiceData.data.filename=Filename;
  if(cPluginManager::CallFirstService("MPlayer-Test-v1", &ServiceData))
  {
    return ServiceData.result;
  }
  else
  {
    return false;
  }
}

/*
 * cFilebroserCommandMP3Plugin
 */

cFilebrowserCommandMP3Plugin::cFilebrowserCommandMP3Plugin() : cFilebrowserCommandAnyPlugin(tr("Play"))
{
  Extensions="+(*.mp3|*.ogg|*.wav|*.flac)";
  ServiceID="MP3-Play-v1";
  ServiceAvailable=cPluginManager::CallFirstService(*ServiceID, NULL);
}

bool cFilebrowserCommandMP3Plugin::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
  if(!ServiceAvailable)
  {
    return false;
  }
  MP3ServiceData ServiceData;
  ServiceData.data.filename=CurrentFile;
  if(cPluginManager::CallFirstService(ServiceID, &ServiceData))
  {
    if(ServiceData.result)
    {
      State=osEnd;
      return true;
    }
  }
  return false;
}

bool cFilebrowserCommandMP3Plugin::Matches(const char* Filename)
{
  if(!cFilebrowserCommandAnyPlugin::Matches(Filename))
  {
    return false;
  }
  MP3ServiceData ServiceData;
  ServiceData.data.filename=Filename;
  if(cPluginManager::CallFirstService("MP3-Test-v1", &ServiceData))
  {
    return ServiceData.result;
  }
  else
  {
    return false;
  }
}
