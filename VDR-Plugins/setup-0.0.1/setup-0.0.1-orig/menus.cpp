/****************************************************************************
 * DESCRIPTION: 
 *             Configuration Menus
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2004 by Ralf Dotzert 
 ****************************************************************************/

#include <sys/utsname.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>

#include <vdr/device.h>
#include "menus.h"
#include "config.h"
#include "sysconfig.h"
#include "debug.h"
#include "common.h"

#define VALUETEXTMAXLEN 250
#define VALUEIPMAXLEN   16

// ###################################################################################
//  MenuNode
// ###################################################################################

/**
 * Constructor of Object
 *
 */
MenuNode::MenuNode( )
{
  _menu      = NULL;
  _menuEntry = NULL;
  _objType   = MenuNode::UNDEFINED;
}

/**
 * Destructor of Object 
 *
 */
MenuNode::~ MenuNode( )
{
  destroy();
}

/**
 * Destroy Object
 */
void MenuNode::destroy( )
{
  delete _menu;
  delete _menuEntry;
  _objType   = MenuNode::UNDEFINED;
  _menu      = NULL;
  _menuEntry = NULL;
}

/**
 * 
 * @param menu 
 */
void MenuNode::SetNode( Menu * menu )
{
     _menu    = menu;
     if( menu->GetSystem() != NULL )
       _objType = MENUSYSTEM;
     else
       _objType = MENU;
}

/**
 * 
 * @param menuEntry 
 */
void MenuNode::SetNode( MenuEntry * menuEntry )
{
     _menuEntry  = menuEntry;
     _objType    = ENTRY;
}

/**
 * 
 * @return 
 */
MenuNode::Type MenuNode::GetType( )
{
  return(_objType);
}

/**
 * 
 * @return 
 */
Menu * MenuNode::GetMenu( )
{
  if( _objType == MENU || _objType == MENUSYSTEM)
    return(_menu);
  else
    return(NULL);
}

/**
 * 
 * @return 
 */
MenuEntry * MenuNode::GetMenuEntry( )
{
  if( _objType == ENTRY)
    return(_menuEntry);
  else
    return(NULL);
}

int MenuNode::GetNr( )
{
  if( _objType == MENU)
    return(_menu->GetNr());
  else
    return(_menuEntry->GetNr());

}

MenuNode * MenuNode::GetNode( int index )
{
  if( _objType == MENU)
    return(_menu->GetNode(index));
  else
    return(_menuEntry->GetNode(index));
}


/**
 * 
 * @return 
 */
const char * MenuNode::GetName( )
{
  const char* result = NULL;

  if( _menu != NULL)  result= _menu->GetName();
  else
  if( _menuEntry != NULL)  result= _menuEntry->GetName();

  return(result);
}


/**
 * 
 */
 void MenuNode::Print( int offset)
{
  if( _objType == MenuNode::MENU)
  {
    _menu->Print(offset);
  }
  else
   if( _objType == MenuNode::ENTRY)
   {
    _menuEntry->Print(offset);
   }
}
/**
 * 
 */
 void MenuNode::Print()
{
  Print(0);
}

void MenuNode::SetSysConfig( Sysconfig *sysconf )
{
  if( _objType == ENTRY)
  {
     _menuEntry->SetSysConfig(sysconf);
  }
  else
    _menu->SetSysConfig(sysconf);
}



// ###################################################################################
//  Menu
// ###################################################################################

/**
 * 
 * @return 
 */
Menu::Menu()
{
  _name    = NULL;
  _command = NULL;
  _help    = NULL;
  _help2   = NULL;
  _system  = NULL;
}

/**
 * 
 * @param name 
 * @return 
 */
Menu::Menu(const char * name )
{
  _name    = NULL;
  _command = NULL;
  _help    = NULL;
  _help2   = NULL;
  _system  = NULL;
  SetName(name);
}


/**
 * 
 * @return 
 */
Menu::~ Menu( )
{
  destroy();
}



/**
 * Destroy Object
 */
void Menu::destroy( )
{
  delete [] _name;
  delete [] _command;
  delete [] _help;
  delete [] _help2;
  delete [] _system;
  _name    = NULL;
  _command = NULL;
  _system  = NULL;
  _help    = NULL;
  _help2    = NULL;
}




/**
 * set name of menu
 * @param nam specifies the name of the menu
 */
void Menu::SetName( const char * nam )
{
  delete [] _name;
  _name=Util::Strdupnew(nam);
}

/**
 * return Menu Name
 * @return name of menu
 */
const char * Menu::GetName( )
{
 return(_name);
}

/**
 * return Menu Comand
 * @return command of menu
 */
const char * Menu::GetCommand( )
{
  return(_command);
}


/**
 * set command of menu
 * @param command specifies the command of the menu
 */
void Menu::SetCommand( const char * command )
{
  delete [] _command;
  _command=Util::Strdupnew(command);
}


/**
 * return Menu System
 * @return system command of menu
 */
const char * Menu::GetSystem( )
{
  return(_system);
}


/**
 * set system command of menu
 * @param system specifies the system command of the menu
 */
void Menu::SetSystem( const char * system )
{
  delete [] _system;
  _system=Util::Strdupnew(system);
}

/**
 * Add the given node to the Node List
 * @param menu pointer to allocate Menu Object
 */
void Menu::AddNode( Menu * menu )
{
     MenuNode *n = new MenuNode;
     n->SetNode(menu);
     _subMenus.Add(n);
}

/**
 * Add the given node to the Node List
 * @param menuEntry pointer to allocate MenuEntry Object
 */
void Menu::AddNode( MenuEntry * menuEntry )
{
     MenuNode *n = new MenuNode;
     n->SetNode(menuEntry);
     _subMenus.Add(n);
}


/**
 * Load XMl subtrre
 * @param xmlNode current XMl Node to Parse
 * @return true on success
 */
bool Menu::LoadXml( TiXmlNode * xmlNode )
{
  bool ok = true;
  TiXmlElement *elem = NULL;

  if(xmlNode != NULL && xmlNode->Type() == TiXmlNode::ELEMENT)
  {
    do
    {
        if(xmlNode->Type() == TiXmlNode::ELEMENT)
        {
          elem = xmlNode->ToElement ();
          if( strcmp(xmlNode->Value(), "menu")== 0)
          {
                const char* name    = elem->Attribute("name");
                const char* command = elem->Attribute("command");
                const char* system  = elem->Attribute("system");
                const char* help1   = elem->Attribute("help1");
                const char* help2   = elem->Attribute("help2");
                if( name != NULL )
                {
                    Menu * m = new Menu(name);

                    if( m!= NULL && (ok=m->LoadXml(xmlNode->FirstChild()))== true )
                    {
                      m->SetCommand(command);
                      m->SetHelp1(help1);
                      m->SetHelp2(help2);
                      m->SetSystem(system);
                      AddNode(m);
                    }
                    else
                        delete m;
                }
                else
                    ok = false;

           }
           else
            if( strcmp(xmlNode->Value(), "entry")== 0)
            {   const char* command = elem->Attribute("command");
                const char* help1   = elem->Attribute("help1");
                const char* help2   = elem->Attribute("help2");
                MenuEntry * e = new MenuEntry();

                if( e != NULL && (ok=e->AddEntry(xmlNode))== true )
                {
                    e->SetCommand(command);
                    e->SetHelp1(help1);
                    e->SetHelp2(help2);
                    AddNode(e);
                }
                else
                    delete e;
            }
        }
    
    }while( (xmlNode=xmlNode->NextSibling())!=NULL && ok == true);
    
  }
  
  return(ok);
}

/**
 * Save Menu to XML Structure
 * @param xml xml structure
 * @return true on success
 */
TiXmlNode *Menu::SaveXml()
{   
  bool ok = true;
  TiXmlElement  *m   = new TiXmlElement("menu");
  TiXmlNode  *tmp = NULL;

  m->SetAttribute("name", GetName());
  if( GetCommand()!= NULL)
    m->SetAttribute("command", GetCommand());
  if( GetHelp1() != NULL)
    m->SetAttribute("help1", GetHelp1());
  
  if( GetHelp2() != NULL)
    m->SetAttribute("help2", GetHelp2());
  
  if( GetSystem()!= NULL)
    m->SetAttribute("system", GetSystem());
  
  for (MenuNode *subMenu = _subMenus.First(); subMenu; subMenu = _subMenus.Next(subMenu))
  {
    if(subMenu->GetType() == MenuNode::MENU ||
       subMenu->GetType() == MenuNode::MENUSYSTEM)
    {
       tmp=subMenu->GetMenu()->SaveXml();
    }
    else
     if(subMenu->GetType() == MenuNode::ENTRY)
     {
       tmp=subMenu->GetMenuEntry()->SaveXml();
     }

    
    if( tmp != NULL)
       m->LinkEndChild(tmp);
    else
      ok=false;
  }

  if( ok=false)
  {
    delete m;
    m = NULL;
  }
  
  return(m);
}


/**
 * return Help for Menu/Entry
 * @return help of menu/entry
 */
const char * Menu::GetHelp1( )
{
  return(_help);
}


/**
 * set help for menu/entry
 * @param command specifies the help for the menu/entry
 */
void Menu::SetHelp1( const char * help )
{
  delete [] _help;
  _help=Util::Strdupnew(help);
}

/**
 * return Detailed Help for Menu/Entry
 * @return help of menu/entry
 */
const char * Menu::GetHelp2( )
{
  return(_help2);
}


/**
 * set detailed help for menu/entry
 * @param command specifies the help for the menu/entry
 */
void Menu::SetHelp2( const char * help )
{
  delete [] _help2;
  _help2=Util::Strdupnew(help);
}

int Menu::GetNr( )
{
  return(_subMenus.Count());
}

/**
 * 
 * @param index 
 * @return 
 */
MenuNode * Menu::GetNode( int index )
{
  return(_subMenus.Get(index));
}




void Menu::SetSysConfig( Sysconfig *sysconf )
{

  for (MenuNode *subMenu = _subMenus.First(); subMenu; subMenu = _subMenus.Next(subMenu))
  {
    subMenu->SetSysConfig(sysconf );
  }
}



/**
 * 
 */
 void Menu::Print( int offset)
{
  for(int i=0; i<offset; i++)
     printf("-");
    
  printf("Menu: Name: %s \n",   _name);

  for (MenuNode *subMenu = _subMenus.First(); subMenu; subMenu = _subMenus.Next(subMenu))
  {
    for(int j=0; j<offset; j++)
        printf("-");
    subMenu->Print(offset+4);
  }
}
/**
 * 
 */
 void Menu::Print()
{
  Print(0);
}


// ###################################################################################
//  MenuEntry
// ###################################################################################
/**
 * 
 * @return 
 */
MenuEntry::MenuEntry( )
{
  _sysconfigName = NULL;
  _valueText     = NULL;
  _valueIp       = NULL;
  _valueTextMaxLen = 0;
  _valueIpMaxLen = 0;
  _valueNumber   = 0;
  _valueBool     = 0;
  _setupCommand  = NULL;
  _type          = Util::UNDEFINED;
  _command       = NULL;
  _help          = NULL;
  _help2         = NULL;
  _system        = NULL;
}

/**
 * 
 * @return 
 */
MenuEntry::~ MenuEntry( )
{
  destroy();
}


/**
 * Destroy Object
 */
void MenuEntry::destroy( )
{
  delete [] _sysconfigName;
  delete [] _valueText;
  delete [] _valueIp;
  delete [] _setupCommand;
  delete [] _help;
  delete [] _help2;
  _sysconfigName = NULL;
  _valueText     = NULL;
  _valueIp       = NULL;
  _setupCommand  = NULL;
  _help          = NULL;
  _help2         = NULL;
}



/**
 * set the corrspondig name in the sysconfig file
 * @param nam the name used in the sysconfig file
 */
void MenuEntry::SetSysconfigName( const char * name )
{
 _sysconfigName = Util::Strdupnew(name);
}

/**
 * 
 * @return the name used in the sysconfig file
 */
const char * MenuEntry::GetSysconfigName( )
{
 return(_sysconfigName);
}



/**
 * 
 * @param typ Set Type
 */
void MenuEntry::SetType( Util::Type typ )
{
  _type = typ;
}


/**
 * 
 * @return 
 */
Util::Type MenuEntry::GetType( )
{
  return(_type);
}


/**
 * Set the Value
 * @param val 
 */
void MenuEntry::SetValue(Util::Type type,const char * val )
{
  _type = type;
  switch(_type)
  {
    case Util::BOOL: Util::isBool(val, _valueBool);
                     break;

    case Util::TEXT: _valueText = Util::Strdupnew(val, VALUETEXTMAXLEN);
                     _valueTextMaxLen = VALUETEXTMAXLEN;
                     break;
    case Util::IP:   _valueIp = Util::Strdupnew(val, VALUEIPMAXLEN);
                     _valueIpMaxLen = VALUEIPMAXLEN;
                     break;
    case Util::NUMBER_TEXT: _valueText = Util::Strdupnew(val, VALUETEXTMAXLEN);
                     _valueTextMaxLen = VALUETEXTMAXLEN;
                     break;
    case Util::HEX:  _valueText = Util::Strdupnew(val, VALUETEXTMAXLEN);
                     _valueTextMaxLen = VALUETEXTMAXLEN;
                     break;
    case Util::NUMBER: Util::isNumber(val, _valueNumber);
                      break;
    
    case Util::SELECTION:
                     _valueText = Util::Strdupnew(val, VALUETEXTMAXLEN);
                     _valueTextMaxLen = VALUETEXTMAXLEN;
                      break;
    case Util::UNDEFINED:
                 break;
  }
  
}

const char * MenuEntry::GetSelectedValue( )
{
   return(_selectionValues.GetSelectedValue());
}


const char * MenuEntry::GetSelectionValue( int index )
{
  return(_selectionValues.GetValue(index));
}

int MenuEntry::GetNrOfSelectionValues( )
{
  return(_selectionValues.GetNr());
}

const char ** MenuEntry::GetSelectionValues( )
{
  return(_selectionValues.GetValues());
}

void MenuEntry::SetSelection( const char * val)
{
  _selectionValues.SetSelection(val);
}
void MenuEntry::SetSetupCommand( const char * command )
{
  _setupCommand = Util::Strdupnew(command);
}

const char * MenuEntry::GetSetupCommand( )
{
  return(_setupCommand);
}


/**
 * Return the Value
 * @return 
 */
const char * MenuEntry::GetValue( )
{
 return(_valueText);
}


int MenuEntry::GetValueTextMaxLen( )
{
  return(_valueTextMaxLen);
}

/**
 * 
 * @return 
 */
const char * MenuEntry::GetValueAsString( )
{
  const char * result=NULL;
  static char numberStr[20];
  
  switch(_type)
  {
    case Util::BOOL: result   = Util::boolToStr(_valueBool); 
                     break;

    case Util::NUMBER_TEXT:
    case Util::HEX:
    case Util::TEXT:   result   =_valueText;
                       break;
    case Util::IP:     result = _valueIp;
                       break;
    case Util::NUMBER: sprintf(numberStr,"%d", _valueNumber);
                       result = (const char*) numberStr;
                       break;
    case Util::SELECTION:
                   result=_selectionValues.GetSelectedValue();
                   break;
    default:
                 result=Util::typeToStr(_type);
                 break;
  }
  return(result);
}

/**
 * 
 * @return 
 */
int * MenuEntry::GetValueBoolRef( )
{
  return(&_valueBool);
}

/**
 * 
 * @return 
 */
int * MenuEntry::GetValueNumberRef( )
{
  return(&_valueNumber);
}



/**
 * 
 * @param node 
 * @param selection 
 * @return 
 */
bool MenuEntry::AddSelectionValues( TiXmlNode * node, const char*selection )
{
    bool ok = true;

    if( _setupCommand != NULL)
    {
     if(strcmp(_setupCommand, "networkdriver")==0)
     {
       ok = ReadNetworkDrivers();
       _selectionValues.SetSelection(selection);
     }
     if( strcmp(_setupCommand, "channellist")==0 )
     {
       ok = ChannelList();
     }
    }
    else
        if( node != NULL && node->Type() == TiXmlNode::ELEMENT)
        {
            do
            {
                _selectionValues.Add(node->FirstChild()->Value());
            }while( (node=node->NextSibling("value"))!= NULL  && ok==true);
            _selectionValues.SetSelection(selection);
        }
    return(ok);
}



/**
 * 
 * @return 
 */
bool MenuEntry::ChannelList( )
{
  bool ok = true;
  DIR        *dirFP = NULL;
  char * channelDir=NULL;
  char * channelFile=NULL;
  struct dirent *entry = NULL;

  asprintf(&channelDir,"%s", cPlugin::ConfigDirectory());
  char *tmp = strrchr(channelDir,'/');
  *tmp = '\0';

  asprintf(&channelFile, "%s/channels.conf",channelDir);
  asprintf(&channelDir,  "%s/channels",channelDir);

  
   if(  (dirFP = opendir(channelDir))!= NULL)
   {
        while( (entry=readdir(dirFP))!=NULL)
        {
            char *tmp=NULL;

            if( (tmp=strstr(entry->d_name, ".conf"))!= NULL)
            {
              tmp[0]='\0';
              _selectionValues.Add(entry->d_name);
            }
        }
        char buf[PATH_MAX];
        if( realpath(channelFile, buf) != NULL)
        {
          _selectionValues.SetSelection(buf);
        }
        else
        {
          DEBUG4("%s Can not resolve realpath of %s  errno=%d\n", DBG_PREFIX, channelFile, errno );
          ok = false;
        }
   }
   else
   {
     DEBUG4("%s Can not read directory:%s  errno=%d\n", DBG_PREFIX, channelDir, errno);
   }
   
   if( dirFP!=NULL ) closedir(dirFP);
   if( channelDir != NULL) free(channelDir);
   if( channelFile != NULL) free(channelFile);
   
   return(ok);
  
}



/**
 * 
 * @return 
 */
bool MenuEntry::ReadNetworkDrivers( )
{
 struct utsname uName;
 bool ok=true;
 
 if( uname(&uName) == 0)
 {
   char * dir = NULL;
      
   asprintf(&dir, "/lib/modules/%s/kernel/drivers/net", uName.release);
   readNetworkDriversDir(dir, "");
   if( dir!= NULL) free(dir);
 }
 else
 {
   DEBUG3("%s UNAME failed, errno=%d\n", DBG_PREFIX, errno);
   ok=false;
 }

 return(ok);
}

/**
 * 
 * @param dir 
 * @return 
 */
bool MenuEntry::readNetworkDriversDir(char *dir, char *prefix)
{
 DIR           *modDir = NULL;
 struct dirent *entry  = NULL;
 struct stat   buf;
 bool ok=true;
 char *path=NULL;

   asprintf(&path, "%s/%s", dir, prefix);

   if(  (modDir = opendir(path))!= NULL)
   {
        while( (entry=readdir(modDir))!=NULL)
        {   

            if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char *tmp=NULL;                
                asprintf(&tmp, "%s/%s", path, entry->d_name);
                stat(tmp, &buf);
                free(tmp);
                
                if( S_ISDIR(buf.st_mode))
                {

                  char *newPrefix=NULL;
                  if(strlen(prefix)>0)
                    asprintf(&newPrefix, "%s/%s", prefix, entry->d_name);
                  else
                    asprintf(&newPrefix, "%s", entry->d_name);
                  readNetworkDriversDir(dir, newPrefix);
                  free(newPrefix);
                }
                else
                if( S_ISREG(buf.st_mode))
                {
                    if( (tmp=strstr(entry->d_name, ".o")) != NULL ||
                        (tmp=strstr(entry->d_name, ".ko"))!= NULL)
                    {
                      tmp[0] = '\0';
                      char *myEntry=NULL;
                      if(strlen(prefix)!= 0)
                         asprintf(&myEntry, "%s/%s", prefix, entry->d_name);
                      else
                         asprintf(&myEntry, "%s", entry->d_name);
                      _selectionValues.Add(myEntry);
                      free(myEntry);
                    }
                }
            }
        }
   }
   else
   {
     ok=false;
     DEBUG4("%s Can not read directory:%s  errno=%d\n", DBG_PREFIX, path, errno);
   }
   if( modDir!=NULL ) closedir(modDir);
   if( path!=NULL) free(path);
   
   return(ok);
}

int * MenuEntry::GetReferenceSelection( )
{
 return(_selectionValues.GetReferenceSelection());
}


/**
 * Add SubEntry to
 * @param node XML poiter to subtree
 * @return ok on success
 */
bool MenuEntry::AddEntry( TiXmlNode *node  )
{
    bool          ok   = true;
    TiXmlElement *elem = NULL;

    if(node != NULL && node->Type() == TiXmlNode::ELEMENT)
    {

      elem = node->ToElement ();
      const char* nam      = elem->Attribute("name");
      const char* sysconf  = elem->Attribute("sysconfig");
      const char* typStr   = elem->Attribute("type");
      const char* val      = elem->Attribute("value");
      const char* setup    = elem->Attribute("setup");
    
      Util::Type  typ      = Util::UNDEFINED;
      if( nam != NULL && sysconf!= NULL && typStr != NULL && val != NULL &&
          Util::isType(typStr, typ))
      {
        if( Sysconfig::GetVariable(sysconf) != NULL)//Check if variable is in sysconf
          {
          	debug("Variable %s found, xml:%s, sys:%s",sysconf,val,Sysconfig::GetVariable(sysconf));
            val=Sysconfig::GetVariable(sysconf);
          }else {
          	debug("Variable %s not found, xml:%s",sysconf,val);
          }
    
        SetName(nam);
        SetSysconfigName(sysconf);
        SetValue(typ, val);
        SetSetupCommand(setup);

        //CHILDLOCK
        if( setup != NULL && strcmp(setup, "childlock")== 0)
           Config::SetChildLock(GetValue());
    
        ok = AddSelectionValues(node->FirstChild("value"), val);
        if(ok    == true &&
           _type == Util::SELECTION)  // Set Selection
        {
         SetSelection (val);
        }
      }
      else
      {
        DEBUG4("%s Error in XML File, Column=%d Row%d\n", DBG_PREFIX, node->Column (), node->Row());
        ok = false;
      }

    }

    return(ok);
}


/**
 * Convert ENtry in XML Structure
 * @return xml Eölement or NULL if error
 */
TiXmlNode *MenuEntry::SaveXml( )
{

  TiXmlElement *e = new TiXmlElement("entry");
  if( e != NULL )
  {
    e->SetAttribute("name", GetName());
    e->SetAttribute("sysconfig", GetSysconfigName());
    e->SetAttribute("type", Util::typeToStr(_type));

    if( _setupCommand != NULL )
        e->SetAttribute("setup", _setupCommand);

    if( _type == Util::SELECTION)
    {
        if( _setupCommand != NULL )
        {
            if(strcmp(_setupCommand, "channellist")==0)
              Util::SwitchChannelList(GetSelectedValue());
        }
        else
        {
            for(int i=0; i< GetNrOfSelectionValues(); i++)
            {
            TiXmlElement *e1 = new TiXmlElement("value");
            e1->LinkEndChild( new TiXmlText (GetSelectionValue(i)));
            e->LinkEndChild(e1);
            }
    
        }
        e->SetAttribute("value", GetSelectedValue());
    }
    else
      e->SetAttribute("value", GetValueAsString());
    
    if( GetCommand()!= NULL)
      e->SetAttribute("command", GetCommand());
    
    if( GetHelp1()!= NULL)
      e->SetAttribute("help1", GetHelp1());

    if( GetHelp2()!= NULL)
      e->SetAttribute("help2", GetHelp2());

  }

  for (MenuNode *subMenu = _subMenus.First(); subMenu; subMenu = _subMenus.Next(subMenu))
  {
    e->LinkEndChild(subMenu->GetMenuEntry()->SaveXml());
  }

  
  return(e);
}




void MenuEntry::SetSysConfig( Sysconfig *sysconf )
{
  sysconf->SetVariable(GetSysconfigName(), GetValueAsString());

  for (MenuNode *subMenu = _subMenus.First(); subMenu; subMenu = _subMenus.Next(subMenu))
  {
    subMenu->SetSysConfig(sysconf );
  }
}


/**
 * 
 */
void MenuEntry::Print( int offset)
{
  for(int i=0; i<offset; i++)
     printf("-");
    
  printf("Entry: Name: %s SysconfigName=%s Type=%s Value=%s",
          _name, _sysconfigName, Util::typeToStr(_type), GetValueAsString());
  if( _command != NULL ) printf(" command=%s", _command);
  if( _help    != NULL ) printf(" command=%s", _help);
  _selectionValues.Print(offset);
  

    for (MenuNode *subMenu = _subMenus.First(); subMenu; subMenu = _subMenus.Next(subMenu))
    {
       for(int j=0; j<offset; j++)
         printf("-");
          subMenu->Print(offset+4);
    }

}
/**
 * 
 */
void MenuEntry::Print()
{
  Print(0);
}


// ###################################################################################
//  Menus
// ###################################################################################


/**
 * Constructor of Object
 * 
 */
Menus::Menus()
{

}


/**
 * Destructor of Object
 * 
 */
Menus::~Menus()
{
}

/**
 * return the numer of menus
 * @return 
 */
int Menus::GetNr( )
{
  return(Count());
}


/**
 * Load XML representation
 * @param node current node in XML Tree
 * @return true on success
 */
bool Menus::LoadXml( TiXmlNode * node )
{
   bool ok = true;
   TiXmlElement *elem = NULL;
   if(node != NULL && node->Type() == TiXmlNode::ELEMENT && (node=node->FirstChild())!=NULL)
   {
        do
        {
           if(node->Type() == TiXmlNode::ELEMENT && strcmp(node->Value(), "menu")== 0)
           {
            elem = node->ToElement ();
            const char* name = elem->Attribute("name");
            const char* command = elem->Attribute("command");
            const char* system = elem->Attribute("system");
            const char* help1   = elem->Attribute("help1");
            const char* help2   = elem->Attribute("help2");
            if( name != NULL )
            {
              Menu * m = new Menu(name);
              m->SetCommand(command);
              m->SetSystem(system);
              m->SetHelp1(help1);
              m->SetHelp2(help2);
              if( m!= NULL && (ok=m->LoadXml(node->FirstChild()))== true )
                 ok=AddNode(m);
              else
                 delete m;
            }
            else
            {
             ok = false;
            }
           }
        }while( (node=node->NextSibling())!=NULL && ok == true);
   }
   else
     ok = false;

   return(ok);
}


/**
 * Save Menues to XML Structure
 * @param xml XML structure
 * @return true on success
 */
bool Menus::SaveXml(TiXmlNode * root)
{
  bool ok = true;
  TiXmlNode *tmp   = NULL;
  TiXmlElement  *m   = new TiXmlElement("menus"); // Root element of menus
  for(int i=0; i<Count() && ok==true; i++)
  {
    if( (tmp=Get(i)->GetMenu()->SaveXml()) != NULL)
    {
      m->LinkEndChild(tmp);
    }
    else
      ok=false;
  }
  root->LinkEndChild(m);
  return(ok);
}


/**
 * Add a Menu Node
 * @param name name of Menu
 * @param xmlNode current XML Node
 * @return true on success
 */
bool Menus::AddNode( Menu *menu )
{
    bool ok = true;
    
     MenuNode *n = new MenuNode;
     Add(n);

     n->SetNode(menu);

    return(ok);

}

/**
 * Return refernce to specified Menu
 * @param index of menu
 * @return pointer to menu
 */
MenuNode  *Menus::GetMenuNode(int index)
{
  return(Get(index));
}


/**
 * Print Nodes
 */
void Menus::Print( )
{
  for( int i=0; i<Count(); i++)
    Get(i)->Print();
}


// ###################################################################################
//  MenuEntryValueList
// ###################################################################################

MenuEntryValueList::MenuEntryValueList( )
{
  _values = NULL;
  _nr    = 0;
  _selection =0;

}

MenuEntryValueList::~ MenuEntryValueList( )
{
  destroy();
}

void MenuEntryValueList::copy( MenuEntryValueList const & other )
{

  for(int i=0; i<other._nr;i++)
  {   
    Add(other._values[i]);
  }
  _nr = other._nr;
  _selection = other._selection;

}

void MenuEntryValueList::destroy( )
{
 if( _values != NULL )
 {
    for( int i=0; i<_nr; i++)
        delete [] _values[i];
    delete [] _values;
    _nr=0;
 }
 _values = NULL;
}



MenuEntryValueList const &MenuEntryValueList::operator =(MenuEntryValueList const &right )
{
 if( this != &right)
 {
   destroy();
   copy(right);
 }
 return(*this);
}


void MenuEntryValueList::Add( const char * value )
{
  const char **vals = new  const char*[_nr + 1];

  for(int i=0; i<_nr; i++)
  {
    vals[i] = Util::Strdupnew(_values[i]);
  }
  
  vals[_nr] =  Util::Strdupnew(value);
  int nr =_nr;
  destroy();
  _nr =++nr;
  _values  = vals;
  
  
}

void MenuEntryValueList::Print( )
{
  Print(0);
}

void MenuEntryValueList::Print( int offset )
{
 if( _nr >0)
 {

    
    for(int i=0; i<_nr; i++)
    {
        for(int j=0; j<offset+2; j++)
           printf("-");
        printf("Value[%d]=%s\n", i, _values[i]);
    }
 }
}


int MenuEntryValueList::GetNr( )
{
  return(_nr);
}

const char * MenuEntryValueList::GetValue( int index )
{
  const char*result=NULL;
  if(_nr > 0 && index <_nr && index >=0)
  {
    result=_values[index];
  }
  return(result);
}

const char ** MenuEntryValueList::GetValues( )
{
 return(_values);
}

int * MenuEntryValueList::GetReferenceSelection( )
{
 return(&_selection);
}




void MenuEntryValueList::SetSelection( const char * value )
{
  bool found =false;
  _selection = 0;

  for(int i=0; i<_nr && found==false; i++)
  {
    if( strcmp(_values[i], value)== 0)
    {
      _selection =i;
      found=true;
    }
  }
}



const char * MenuEntryValueList::GetSelectedValue( )
{
  if(_nr == 0)
    return(NULL);
  else
  {
    return(_values[_selection]);
  }
}
