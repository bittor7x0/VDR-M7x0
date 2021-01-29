/****************************************************************************
 * DESCRIPTION:
 *             Read / Write Config Data
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2004 by Ralf Dotzert
 ****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "vdr/plugin.h"
#include "tinyxml2.h"
#include "config.h"
#include "util.h"

using namespace tinyxml2;

/*
 * Constructor
 * @param fname  XML File
 * @return
 */
Config::Config(char *fname)
{
  info("Load file: [%s]",fname);

  // Init sysconfig values
  _filename  	 = Util::Strdupnew(fname);
  _sysconfigFile = "/tmp/tmp.conf";
  _sysconfigPre  = "/etc/vdr/plugins/setup/local-conf -r -f /tmp/tmp.conf -c /etc";
  _sysconfigPost = "/etc/vdr/plugins/setup/local-conf -w -f /tmp/tmp.conf -c /etc";
  _bootLinux     = "/etc/vdr/rebootvdr";
  _libDir        = NULL;
  _libDirCMD     = "/etc/vdr/plugins/setup/local-plugins";
}

Config::~Config()
{
  delete [] _filename;
  delete [] _libDir;
}

/************************
* Read XML-File
*************************/
/**
 * Decode XML File
 * @return
 */
bool Config::LoadFile()
{
  bool ok = false;
  XMLElement *root;

  //Load XML Config file
  if(ok = (_xmlDoc.LoadFile(_filename) == XML_SUCCESS))
  {
    if((root = _xmlDoc.FirstChildElement("setup")) != NULL)
    {
        //get plugin path from env
        char *line = NULL;
		FILE *p;
		cReadLine ReadLine;
        const char *tmp = NULL;

        p = popen(_libDirCMD, "r");
        if (p)
		{//read response from command
			while ( (line = ReadLine.Read(p) ) !=  NULL)
			{
					tmp = line;//when we get more then one line we take the last
			}
			//cleanup
			pclose(p);
			_libDir=Util::Strdupnew(tmp);
			free(line);
		}else{
			error("Error while opening pipe! Execute '%s' faild! Could not find plugin path", tmp);
		}
		info("We found plugin path: %s", _libDir);

		//read sysconfig
		//excecute pre sysconfig command
  		ok = system(_sysconfigPre);
  		if(ok == 0) {//Load sysconfig
  			if ((ok=_sysconfig.LoadFile(_sysconfigFile)) != true){
  					error("Could not load sysconfig: [%s]",_sysconfigFile);
  			}
  	  } else{
          error("Could not execute sysconfigPre: [%s], errno=%d", _sysconfigPre, ok);
  		}

        root=root->FirstChildElement();
        if(root!= NULL &&
            strcmp(root->Value(), "plugins")==0)
        {
          _activePlugins.SetProtectedList(root->Attribute("protected"));
          const char* nameSysconfig = root->Attribute("sysconfig");

          if(nameSysconfig != NULL)
          {
           _activePlugins.SetSysconfigName(nameSysconfig);
           debug("SysPlugin [%s]",nameSysconfig);
           if(ok=loadPlugins(root->FirstChildElement())) //Load Plugins
           {
              ok = _menus.LoadXml(root->NextSiblingElement("menus"));//Load Menus
           }
          }
          else
            ok= false;
        }
    }
  }

  if(!ok)
   dumpXMLError("Error while Loading XML-FILE");

  // XML read, now load Plugins
  if(ok)
  {
  	ok=readVdrLib();
    info("Loaded config file: [%s]",_filename);
  }

  return(ok);
}

/**
 * Save the the ConfigFile, overwrite opened file
 * @return true if no error occured
 */
bool Config::SaveFile( )
{
  return(SaveFile(_filename));
}

/**
 * Save the the ConfigFile,
 * @param fname name of config file to write to
 * @return true if no error occured
 */
bool Config::SaveFile(char * fname )
{
  if(_filename != fname)
  {
    delete _filename;
    _filename=Util::Strdupnew(fname);
  }

  // Save sysconfig
  prepareSysConfig();
  bool ok = _sysconfig.SaveFile();
  if(ok) { //excecute pre sysconfig command
    debug("sysconfigPost: %s",_sysconfigPost);
    ok = system(_sysconfigPost);
  } else {
    error("Could not execute sysconfigPost: [%s], errno=%d", _sysconfigPost, ok);
  }

  return(ok);
}


//-------------------------------------------------------
// Private Methods
//-------------------------------------------------------
/**
 * Load XML Plugin Information
 * @param elem
 * @return
 */
bool Config::loadPlugins(XMLNode *node)
{
  bool ok = true;

  const char *loaded_plugins =  _sysconfig.GetVariable(_activePlugins.GetSysconfigName());

  info("Loaded plugins: [%s]",loaded_plugins);

  if ( loaded_plugins != NULL && strlen(loaded_plugins)>0) {
    char *str_plugins =  Util::Strdupnew(loaded_plugins);
    char *plugin = strtok(str_plugins," \"");

    while ( plugin != NULL ) {
      _activePlugins.AddPlugin(plugin+2, NULL,  plugin+2, true, _activePlugins.checkProtected(plugin+2),false);
      plugin = strtok(NULL," \"");
    }
    delete []str_plugins;
  }
  else {
    error ("Not found variable [%s]",_activePlugins.GetSysconfigName());
    ok = false;
  }

  return(ok);
}

/***************************
 * Dump XML-Error
 ***************************/
void Config::dumpXMLError(const char* myErrStr)
{
  if(_xmlDoc.Error())
  {
    XMLError    errID   = _xmlDoc.ErrorID();
    const char *errName = _xmlDoc.ErrorName();
    const char *errStr  = _xmlDoc.ErrorStr();
    error("%s: XMLDocument error in %s id=%d '%s' str=%s", myErrStr, _filename, static_cast<int>( errID ), errName, errStr);
  }
}

/**
 * Returns a reference to the Plugins Object
 * @return plugins object
 */
Plugins *Config::GetPlugins( )
{
  return(&_activePlugins);
}

/**
 * returns a reference to the Menus Object
 * @return reference to Menus Object
 */
Menus *Config::GetMenus()
{
  return(&_menus);
}

/**
 * Write internal representation to Sysconf Values
 */
void Config::prepareSysConfig( )
{
  // Plugins
  const char *pluginList = _activePlugins.GetActivePlugins();
  _sysconfig.SetVariable(_activePlugins.GetSysconfigName(), pluginList);
  for(int i=0; i<_menus.GetNr(); i++)
  {
    _menus.GetMenuNode(i)->SetSysConfig(&_sysconfig);
  }
}

/**
 * get all installed plugins in the specified lib directory
 * and set the state within the plugin list
 * @return true on success
 */
bool Config::readVdrLibDir(const char* libDir)
{
  bool ok =true;
  DIR           *dir = opendir(libDir);
  struct dirent *entry = NULL;
  char          *suffix = NULL;

  debug("config:ReadVdrLib %s",libDir);
  //accept only dynamic Link libs with the current vdr version
  asprintf(&suffix, ".so.%s", APIVERSION);
  if(dir != NULL)
  {
    char *tmp = NULL;
    char *module = NULL;
    const char *prefix = "libvdr-";
    while( (entry=readdir(dir))!=NULL)
    {
      debug("Plugin en directorio[%s]\n",entry->d_name +7);
      if(strncmp(entry->d_name, prefix,7)==0  &&
          (tmp=strstr(entry->d_name, suffix))!= NULL)
        {
          tmp[0]='\0';
          module = entry->d_name +7;
          _activePlugins.SetLibDirPlugin(module);
        }
    }

    closedir(dir);
  }
  else
  {
    error("Could not read directory: [%s]", _libDir);
    ok = false;
  }

  if(suffix != NULL) free(suffix);
  return(ok);
}

bool Config::readVdrLib()
{
  bool result =true;
  char *ldir = Util::Strdupnew(_libDir);
  const char *delim = " ";
  char *dir;

  dir = strtok(ldir,delim);

  while ( dir ) {
	  result = readVdrLibDir(dir)  && result;
	  dir = strtok(NULL,delim);
  }
  int nrPlugins = _activePlugins.GetNr();
  for(int i=0; i<nrPlugins; i++)
  {
    Plugin *p=  _activePlugins.Get(i);

    if(p != NULL && p->GetInSystem()==false)
    {
      _activePlugins.Del(p, false);
      _notInSystemPlugins.Add(p);
      nrPlugins--;
      i--;
    }
  }

  if (ldir) delete (ldir);
  return(result);
}

const char * Config::GetBootLinux( )
{
 return(_bootLinux);
}
