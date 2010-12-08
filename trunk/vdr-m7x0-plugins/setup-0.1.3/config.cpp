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
#include "tinystr.h"
#include "tinyxml.h"
#include "config.h"
#include "sysconfig.h"
#include "plugins.h"
#include "util.h"

/*
 * Constructor
 * @param fname  XML File
 * @return 
 */
 
// Initialize static private data

const char *Config::childLockCode = NULL; //will be set within Menü
 
Config::Config(char *fname)
{
	debug("Config.constructor");
	//Init sysconfig values
  _filename  	= Util::Strdupnew(fname);
  debug("Load file: [%s]",fname);
  _xmlDoc    	= TiXmlDocument( fname );
  debug("xml document created")

  _sysconfigPre	= NULL;
  _sysconfigPost= NULL;
  _bootLinux    = NULL;
  _libDir    	= NULL;

  childLockCode = NULL; //will not be freed hold only reference to data
}


Config::~Config()
{
  delete [] _bootLinux;
  delete [] _libDir;
  delete [] _filename;
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
  TiXmlElement *root;
  const char* sysconfigFile=NULL;
    
  //Load XML Config file
  if( (ok=_xmlDoc.LoadFile()))
  { 
    if( (root = _xmlDoc.FirstChildElement("setup" ))!=NULL &&
        (sysconfigFile=root->Attribute("sysconfigFile")) )
   //    && (ok=_sysconfig.LoadFile(sysconfigFile)) == true )
    {
        // Set some default Values
        const char *tmp=NULL;
        if( (tmp =root->Attribute("bootLinux"))==NULL)
          _bootLinux=Util::Strdupnew("pic_tool reboot 3 10");
        else
          _bootLinux=Util::Strdupnew(tmp);

		//Read command to get plugin path from XML
        if( (tmp =root->Attribute("VDRlibDirCMD"))==NULL)
          _libDirCMD = Util::Strdupnew("/var/vdr/lib");
        else
          _libDirCMD = Util::Strdupnew(tmp);
          
        //get plugin path from env        
        char *line = NULL;
		FILE *p;
		cReadLine ReadLine;
		
		debug("CMD to get Plugin path: %s", _libDirCMD );
        
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
		debug("We found plugin path: %s", _libDir);


        if( (tmp =root->Attribute("sysconfigPre"))==NULL)
          _sysconfigPre=Util::Strdupnew("/etc/vdr/plugins/setup/local-conf -r -f /tmp/tmp.conf -c /etc");
        else
          _sysconfigPre=Util::Strdupnew(tmp);

        if( (tmp =root->Attribute("sysconfigPost"))==NULL)
          _sysconfigPost=Util::Strdupnew("/etc/vdr/plugins/setup/local-conf -w -f /tmp/tmp.conf -c /etc");
        else
          _sysconfigPost=Util::Strdupnew(tmp);
		
		//read sysconfig
		//excecute pre sysconfig command
  		debug("sysconfigPre: %s",_sysconfigPre);
  		ok = system(_sysconfigPre);
  		if( ok == 0) {//Load sysconfig
  			if ((ok=_sysconfig.LoadFile(sysconfigFile)) != true){
  					debug("Could not load sysconfig: [%s]",sysconfigFile);
  			}
  	  } else{
          error("Could not execute sysconfigPre: [%s], errno=%d", _sysconfigPre, ok);	
  		}

        root=root->FirstChildElement();
        if( root!= NULL &&
            strcmp(root->Value(), "plugins")==0)
        {
          _activePlugins.SetProtectedList(root->Attribute("protected"));
          const char* nameSysconfig = root->Attribute("sysconfig");
          
          if( nameSysconfig != NULL )
          {
           _activePlugins.SetSysconfigName(nameSysconfig);
           debug("SysPlugin [%s]",nameSysconfig);
           if( ok=loadPlugins(root->FirstChildElement())) //Load Plugins
           {
              ok = _menus.LoadXml(root->NextSibling("menus"));//Load Menus
           }
          }
          else
            ok= false;
        }
    }
  }

  if(!ok )
   dumpXMLError("Error while Loading XML-FILE");

  //XML read, now load Plugins
  if( ok)
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
  bool ok = true;
    
  if(_filename != fname)
  {
    delete _filename;
    _filename=Util::Strdupnew(fname);
  }
  
   
  TiXmlComment  comment;
  comment.SetValue("\n\
-   open7x0-VDR Configuration File\n\
-   based on Minisetup by Ralf Dotzert\n\
"
  );

  TiXmlDocument xml = TiXmlDocument( fname );
  TiXmlElement root("setup");
  root.SetAttribute("sysconfigFile", _sysconfig.GetFileName());
  root.SetAttribute("sysconfigPre",  _sysconfigPre);
  root.SetAttribute("sysconfigPost", _sysconfigPost);
  root.SetAttribute("bootLinux", _bootLinux);
  root.SetAttribute("VDRlibDirCMD", _libDirCMD);
  
  TiXmlElement *plugins=savePlugins();

  
  if( plugins!= NULL &&
       root.LinkEndChild(plugins)  != NULL &&
       _menus.SaveXml(&root)     == true &&
       xml.InsertEndChild(comment) != NULL &&
       xml.InsertEndChild(root)    != NULL &&
       xml.SaveFile())
       ok=true;
  else
  {
    ok = false;
    dumpXMLError("Error writing file");
  }

 	//XML saved, now save sysconfig
  if( ok)
  {
   prepareSysConfig();
   //Save sysconfig
   ok=_sysconfig.SaveFile();
  	if( ok) {//excecute pre sysconfig command
  		debug("sysconfigPost: %s",_sysconfigPost);
	  	ok = system(_sysconfigPost);
  	}else{
    	error("Could not execute sysconfigPost: [%s], errno=%d", _sysconfigPost, ok);	
  	}	
  }
    
  return(ok);
}



/**
 * Returns Child Lock Code
 * @return child Lock Code or NULL if not available
 */
const char * Config::GetChildLock( )
{
  return(childLockCode);
}

/**
 * Set a child Clock Code,
 * @param lock lockk code "0000" or NULL means not locked.
 */
void Config::SetChildLock( const char * lock )
{
  childLockCode = lock;
}

//-------------------------------------------------------
// Private Methods
//-------------------------------------------------------
/**
 * Load XML Plugin Information
 * @param elem 
 * @return 
 */
/*
bool Config::loadPlugins(TiXmlNode *node)
{
  bool ok = true;
  TiXmlElement *elem = NULL;

  const char *loaded_plugins =  _sysconfig.GetVariable(_activePlugins.GetSysconfigName());
  debug("Loaded plugins: [%s]",loaded_plugins);
  debug("SysPlugin [%s]",_activePlugins.GetSysconfigName());

  if(node == NULL || strcmp(node->Value(), "plugin") !=0 )
  {
    dumpXMLError("no <plugin> tag found");
    ok=false;
  }
  else
  {
        do
        {
            elem = node->ToElement ();
            const char* name = elem->Attribute("name");
            const char* info = elem->Attribute("info");
            const char* active = elem->Attribute("active");
            const char* param = elem->Attribute("param");
            const char* protect = elem->Attribute("protected");
            bool  b_active;
            bool  b_protect;
            if( protect == NULL || Util::isBool(protect, b_protect) == false)
              b_protect = false;

            if( name != NULL && info != NULL ) {
//ZJ                && active != NULL  && Util::isBool(active, b_active)) {
              if ( loaded_plugins != NULL && strstr(loaded_plugins,name) == NULL ) {
                debug("Plugin Desactivado en config [%s]",name);
                b_active = false;
              }else {
                debug("Plugin Activo en config [%s]",name);
                b_active = true;
              } 
              _activePlugins.AddPlugin(name, param,  info, b_active, b_protect);
            }
            else
            {
              ok = false;
            }
        }while( (node=node->NextSibling())!=NULL && ok == true);

  }
  return(ok) ;
}
*/
bool Config::loadPlugins(TiXmlNode *node)
{
  bool ok = true;
   
  const char *loaded_plugins =  _sysconfig.GetVariable(_activePlugins.GetSysconfigName());
  char *str_plugins = NULL ;
  char *plugin = NULL;
  
      
  info("Loaded plugins: [%s]",loaded_plugins);
  
  if ( loaded_plugins != NULL && strlen(loaded_plugins)>0) {
    str_plugins =  Util::Strdupnew(loaded_plugins);
    plugin = strtok(str_plugins," \"");

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

  
  return(ok) ;
}

/**
 * Save Plugins
 * @return ptr to XML Element
 */
TiXmlElement *Config::savePlugins()
{
  TiXmlElement *xml = new TiXmlElement("plugins");

  xml->SetAttribute("sysconfig", _activePlugins.GetSysconfigName());
  xml->SetAttribute("protected", _activePlugins.GetProtectedList());

  return(xml);
}


/**
 * Save Plugins
 * @return ptr to XML Element
 */
void Config::savePlugins(Plugins *plugins, TiXmlElement *xml)
{
  int           nr = plugins->GetNr();

  for(int i=0; i<nr && xml != NULL; i++)
  {
    Plugin *p = plugins->Get(i);
    if( p!= NULL)
    {
      TiXmlElement *pl =  new TiXmlElement("plugin");
      if( pl != NULL )
      {
        pl->SetAttribute("name",   p->GetName());
        if( p->GetParameter() != NULL)
          pl->SetAttribute("param",  p->GetParameter());
        pl->SetAttribute("info",   p->GetInfo());
        pl->SetAttribute("active", p->GetActiveString());
        if( p->GetProtect() == true)
          pl->SetAttribute("protected",  Util::boolToStr(p->GetProtect()));

        xml->LinkEndChild(pl);
      }
    }
  }
}
/***************************
 * Dump XML-Error
 ***************************/
void Config::dumpXMLError()
{
  const char *errStr;
  int  col, row;
  if(_xmlDoc.Error())
  {
    errStr = _xmlDoc.ErrorDesc();
    col    = _xmlDoc.ErrorCol();
    row    = _xmlDoc.ErrorRow();
    error("Error in %s Col=%d Row=%d :%s", _filename, col, row, errStr );
  }
}
void Config::dumpXMLError(const char* myErrStr)
{
  const char *errStr;
  int  col, row;

  if(_xmlDoc.Error())
  {
    errStr = _xmlDoc.ErrorDesc();
    col    = _xmlDoc.ErrorCol();
    row    = _xmlDoc.ErrorRow();
    debug("%s: %s: Error in %s Col=%d Row=%d", myErrStr, errStr, _filename, col, row);
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
  char          *module=NULL;
  char          *tmp=NULL;
  char          *prefix ="libvdr-";
  char          *suffix = NULL;

  debug("config:ReadVdrLib %s",libDir);
  //accept only dynamic Link libs with the current vdr version
  asprintf(&suffix, ".so.%s", APIVERSION);
  if( dir !=NULL)
  {
    while( (entry=readdir(dir))!=NULL)
    {
      debug("Plugin en directorio[%s]\n",entry->d_name +7);
      if( strncmp(entry->d_name, prefix,7)==0  &&
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
    debug("Could not read directory: [%s]", _libDir);
    ok = false;
  }
  
  if( suffix != NULL) free(suffix);
  return(ok);
}
 
bool Config::readVdrLib()
{
  bool result =true;  
  char *ldir = Util::Strdupnew(_libDir);
  char *delim = " ";
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

    if( p != NULL && p->GetInSystem()==false)
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

char * Config::GetBootLinux( )
{
 return(_bootLinux);
}



/******************************************************
 *****************************************************/
