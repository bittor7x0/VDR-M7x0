/****************************************************************************
 * DESCRIPTION: 
 *             Plugin Representation of XML Tree
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2004 by Ralf Dotzert 
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include "vdr/plugin.h"
#include "plugins.h"
#include "util.h"
#include "common.h"


/**
 * Construktor
 * @return empty Plugins Object
 */
Plugins::Plugins()
{
    _activePlugins = NULL;
    _sysconfigName = Util::Strdupnew("PLUGINLIST");
    _protectedList = Util::Strdupnew("");
}


/**
 * Destructor
 * @return 
 */
Plugins::~Plugins()
{
    delete [] _activePlugins;
    delete [] _sysconfigName;
    delete [] _protectedList;
}

//---------------------------------------------
/**
 * Add the given Plugin to the List of Plugins
 * @param name name of plugin
 * @param param parameters for plugin, could null if not available
 * @param info description of plugin
 * @param flag active flag
 * @param protected true if entry is protected
 * @param inSystem true if active in System
 */
void Plugins::AddPlugin(const char*name, const char*param, const char*info, bool flag,
                  bool protect, bool inSystem)
{
    Plugin *p = new Plugin;

    p->SetName(name);
    p->SetParameter(param);
    p->SetInfo(info);
    p->SetActive(flag);
    p->SetProtect(protect);
    p->SetInSystem(inSystem);
    debug("AddPlugin: [%s] act [%d] inSystem [%d] Protect[%d]\n", name,flag, inSystem,protect);	
    Add(p);
}



/**
 * Move elemnt in plugin list to specified position
 * @param index   index of element to move
 * @param toindex index to insert
 * @param where After or before the given position
 */
void Plugins::MovePlugin( int index, int toindex, enum Where where)
{
    int nr=Count();
    if(index<0   || index   > nr ||  // invalid index is ignored
       toindex<0 || toindex > nr)
        return;

    if( index > toindex)
    {
       if( where == BEHIND)
         toindex++;
       Move(index, toindex);
    }
    else
     if( index <toindex)
     {
       if( where == BEFORE)
         toindex--;
       Move(index, toindex);
     }
}


/**
 * Sort the Pplugins
 * @param order specifies the direction of sorting
 */
void Plugins::Sort( enum Order order)
{
 int nr= Count();
    if(order==ASC)
    {

        // Bubble up i'th record
        for (int i = 0; i < nr-1; i++)
            for (int j = nr-1; j > i; j--)
            {
                if ( *Get(j) > *Get(j-1) )
                    Move(j, j-1 );
            }
    }
    else
    {
        for (int i = 0; i < nr-1; i++)
            for (int j = nr-1; j > i; j--)
            {
                if( *Get(j) < *Get(j-1) )
                    Move(j, j-1 );
            }
    }

}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const char * Plugin::_trueStr =STR_YES;
const char * Plugin::_falseStr = STR_NO;

/**
 * Constructor 
 * @return 
 */
Plugin::Plugin()
{
    _name=NULL;
    _info=NULL;
    _parameter=NULL;
    _active=false;
    _inSystem=false;
    _protect = false;
}

/**
 * Destructor
 * @return 
 */
Plugin::~Plugin()
{
  delete []_name;
  delete []_info;
  delete []_parameter;
}

/**
 * Overload operator < to compare Plugins

 * @param param the Plugin to compare
 * @return true if name of left plugin is less the right plugin
 */
bool Plugin::operator <( Plugin &param )
{
    if(strcmp(_info, param._info)<0)
        return(true);
    else
        return(false);
}

/**
 * Overload operator > to compare Plugins

 * @param param the Plugin to compare
 * @return true if name of left plugin is greater the right plugin
 */
bool Plugin::operator >( Plugin &param )
{
    if(strcmp(_info, param._info)>0)
        return(true);
    else
        return(false);
}


/**
 * Set the name of the plugin
 * @param nam value to store
 */
void Plugin::SetName(const char* nam)
{
    delete []_name;
    _name= Util::Strdupnew(nam);
}

/**
 * Set the description of the plugin
 * @param inf description to be set
 */
void Plugin::SetInfo(const char* inf)
{
   delete []_info;
   _info= Util::Strdupnew(inf);
}

//---------------------------------------------
/**
 * get the number of registerted Plugins
 * @return number of plugins stored
 */
int Plugins::GetNr()
{
    return(Count());
}

/**
 * Prints the Plugins on stdout
 */
void  Plugins::Print()
{
    int nr = Count();
    for(int i=0; i<nr; i++)
    {
        debug("Plugin[%d] ", i);
        Get(i)->Print();
    }
}


/**
 * Prints a plugin on stdout
 */
void  Plugin::Print()
{
    debug("Name=%s  Parameter=%s Info=%s Active=%d inSystem=%d\n",
            _name, _parameter, _info, _active, _inSystem);
}
void Plugins::SetSysconfigName( const char * name )
{
  delete [] _sysconfigName;
  _sysconfigName =  Util::Strdupnew(name);
}

const char * Plugins::GetSysconfigName( )
{
 return(_sysconfigName);
}

void Plugins::SetProtectedList( const char * name )
{
  delete [] _protectedList;
  debug("Protected plugins [%s]",name);  
  if ( name != NULL ) {
    _protectedList =  Util::Strdupnew(name);
  }else {
    _protectedList = Util::Strdupnew("");
  }
}

const char* Plugins::GetProtectedList()
{
  return _protectedList;  
}

bool Plugins::checkProtected(const char* pluginName)
{
  debug ("Check protected [%s] [%s] [%d]",pluginName,_protectedList,(_protectedList != NULL) && (strstr(_protectedList,pluginName) != NULL ));
  return (_protectedList != NULL) && (strstr(_protectedList,pluginName) != NULL );
}
/**
 * retrieves the name of the plugin
 * @return the name
 */
const char* Plugin::GetName()
{
    return(_name);
}
/**
 * retrieves the description of the plugin
 * @return description
 */
const char* Plugin::GetInfo()
{
    return(_info);
}
/**
 * get plugin parameters
 * @return plugin parameters or null if none available
 */
const char * Plugin::Plugin::GetParameter( )
{
  return(_parameter);
}

/**
 * set plugin command parameters 
 * @param p NULL or plugin parameter string
 */
void Plugin::Plugin::SetParameter( const char * p )
{
  delete [] _parameter;
  _parameter = Util::Strdupnew(p);
}

/**
 * Set Active State
 * @param flag true if plugin is active
 */
void Plugin::Plugin::SetActive( int flag )
{
 _active=flag; 
}

/**
 * get Active State of plugin
 * @return true if active
 */
int Plugin::Plugin::GetActive( )
{
  return(_active);
}
/**
 * Set Flag if Plugin is in System
 * @param flag true if plugin is active
 */
void Plugin::Plugin::SetInSystem( bool flag )
{
 _inSystem=flag;
}

/**
 * get Flag if Plugin is in System
 * @return true if active
 */
bool Plugin::Plugin::GetInSystem( )
{
  return(_inSystem);
}

/**
 * get Active State of plugin as String value
 * @return true if active
 */
const char* Plugin::Plugin::GetActiveString( )
{
  if(_active)
    return(_trueStr);
  else
    return(_falseStr);
}



/**
 * get regerence to active State
 * @return poiinter to active State
 */
int * Plugin::Plugin::GetActiveRef( )
{
  return(&_active);
}

void Plugin::Plugin::SetProtect( bool protect )
{
   debug("SetProtect [%s] [%d]",GetName(),protect);
  _protect = protect;
}

bool Plugin::Plugin::GetProtect( )
{
  debug("GetProtect [%s] [%d]",GetName(),_protect);
  return(_protect);
}




/**
 * returns a String holding space separated List of plugins
 * @return 
 */
const char * Plugins::GetActivePlugins( )
{
  char buf[2000];
  buf[0] = '"';
  buf[1] = '\0';
  int  nr =Count();
  
  for(int i=0; i<nr; i++)
  {
    Plugin * plugin = Get(i);
    if(plugin->GetActive() && plugin->GetInSystem() )
    {
      const char *p = plugin->GetParameter();
      if( p == NULL)
        sprintf(buf, "%s -P%s ", buf,  plugin->GetName());
      else
        sprintf(buf, "%s -P%s %s", buf, plugin->GetName(), p);
    }
  } 
  //HaPe: Add " around the string
  //sprintf(buf, "\"%s\"", buf);
  int iLen = strlen(buf);
  buf[iLen+1] = '\0';
  buf[iLen] = '"';

  if( _activePlugins != NULL)
    delete [] _activePlugins;
    
  _activePlugins = new char[iLen+2];
  strcpy(_activePlugins, buf);
  return((const char*) _activePlugins);
}

/**
 * Sets the flag "inSystem" of the corresponding plugin to true.
 * If the plugin is not found an unknown Plugin will be added.
 * @param pluginName 
 */
void Plugins::SetLibDirPlugin( const char * pluginName )
{
  Plugin *p =GetPluginByName(pluginName);
  if( p != NULL)
  { 
    p->SetInSystem(true);
    debug("In System plugin [%s]\n",pluginName);
  }
  else
  {
    AddPlugin(pluginName,NULL,  pluginName, false, false, true);
  }
 
}

/**
 * returns a pointer to the plugin with the specified name
 * or NULL if not found
 * @param name of the plugin 
 * @return pointer to plugin otherwise NULL
 */
Plugin * Plugins::GetPluginByName( const char * name )
{
  Plugin *p=NULL;
  int nr = Count();
 
  for(int i=0; i<nr && p==NULL; i++)
  { 
   if( strcmp(Get(i)->GetName(),name)==0) {
      p= Get(i);
   }
  }
  return(p);
}


