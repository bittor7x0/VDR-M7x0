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
  if(_objType == MENU || _objType == MENUSYSTEM)
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
  if(_objType == ENTRY)
    return(_menuEntry);
  else
    return(NULL);
}

int MenuNode::GetNr( )
{
  if(_objType == MENU)
    return(_menu->GetNr());
  else
    return(_menuEntry->GetNr());

}

MenuNode * MenuNode::GetNode( int index )
{
  if(_objType == MENU)
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

  if(_menu != NULL)  result= _menu->GetName();
  else
  if(_menuEntry != NULL)  result= _menuEntry->GetName();

  return(result);
}

void MenuNode::SetSysConfig( Sysconfig *sysconf )
{
  if(_objType == ENTRY)
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
  _help    = NULL;
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
  _help    = NULL;
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
  delete [] _help;
  delete [] _system;
  _name    = NULL;
  _system  = NULL;
  _help    = NULL;
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
bool Menu::LoadXml( XMLNode * xmlNode )
{
  bool ok = true;
  XMLElement *elem = NULL;

  if(xmlNode != NULL && xmlNode->ToElement())
  {
    do
    {
        if(xmlNode->ToElement())
        {
          elem = xmlNode->ToElement ();
          if(strcmp(xmlNode->Value(), "menu") == 0)
          {
                const char* name    = elem->Attribute("name");
                const char* system  = elem->Attribute("system");
                const char* help    = elem->Attribute("help");
                if( name != NULL )
                {
                    Menu * m = new Menu(name);

                    if( m!= NULL && (ok=m->LoadXml(xmlNode->FirstChild()))== true )
                    {
                      m->SetHelp(help);
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
            if(strcmp(xmlNode->Value(), "entry")== 0)
            {
                const char* help    = elem->Attribute("help");
                MenuEntry * e = new MenuEntry();

                if( e != NULL && (ok=e->AddEntry(xmlNode))== true )
                {
                    e->SetHelp(help);
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
 * return Help for Menu/Entry
 * @return help of menu/entry
 */
const char * Menu::GetHelp( )
{
  return(_help);
}

/**
 * set help for menu/entry
 * @param command specifies the help for the menu/entry
 */
void Menu::SetHelp( const char * help )
{
  delete [] _help;
  _help=Util::Strdupnew(help);
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
  _help          = NULL;
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
  _sysconfigName = NULL;
  _valueText     = NULL;
  _valueIp       = NULL;
  _setupCommand  = NULL;
  _help          = NULL;
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

    case Util::TEXT: if ( _valueText ) { delete[] _valueText; _valueText=NULL;}
    				 _valueText = Util::Strdupnew(val, VALUETEXTMAXLEN);
                     _valueTextMaxLen = VALUETEXTMAXLEN;
                     break;
    case Util::IP:
    				 if ( _valueIp ) { delete[] _valueIp; _valueIp=NULL;}
    				 _valueIp = Util::Strdupnew(val, VALUEIPMAXLEN);
                     _valueIpMaxLen = VALUEIPMAXLEN;
                     break;
    case Util::HEX:  if ( _valueText ) { delete[] _valueText; _valueText=NULL;}
    				 _valueText = Util::Strdupnew(val, VALUETEXTMAXLEN);
                     _valueTextMaxLen = VALUETEXTMAXLEN;
                     break;
    case Util::NUMBER: Util::isNumber(val, _valueNumber);
                      break;

    case Util::SELECTION:
                     if ( _valueText ) { delete[] _valueText; _valueText=NULL;}
                     _valueText = Util::Strdupnew(val, VALUETEXTMAXLEN);
                     _valueTextMaxLen = VALUETEXTMAXLEN;
                      break;
    case Util::FILE:
    case Util::DIR:
                     if ( _valueText ) { delete[] _valueText; _valueText=NULL;}
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
    case Util::BOOL:   result = Util::boolToStr(_valueBool);
                       break;
    case Util::HEX:
    case Util::TEXT:   result = _valueText;
                       break;
    case Util::IP:     result = _valueIp;
                       break;
    case Util::NUMBER: sprintf(numberStr,"%d", _valueNumber);
                       result = (const char*) numberStr;
                       break;
    case Util::SELECTION:
                       result = _selectionValues.GetSelectedValue();
                       break;
    case Util::FILE:
    case Util::DIR:    result = _valueText;
                       break;
    default:
                       result = Util::typeToStr(_type);
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
bool MenuEntry::AddSelectionValues( XMLNode * node, const char*selection )
{
    bool ok = true;

    if(_setupCommand == NULL)
        if(node != NULL && node->ToElement())
        {
            do
            {
                _selectionValues.Add(node->FirstChild()->Value());
            }while( (node=node->NextSiblingElement("value"))!= NULL  && ok==true);
            _selectionValues.SetSelection(selection);
        }
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
bool MenuEntry::AddEntry( XMLNode *node  )
{
    bool          ok   = true;
    XMLElement *elem = NULL;

    if(node != NULL && node->ToElement())
    {

      elem = node->ToElement ();
      const char* nam      = elem->Attribute("name");
      const char* sysconf  = elem->Attribute("sysconfig");
      const char* typStr   = elem->Attribute("type");
      const char* val      = sysconf != NULL ? Sysconfig::GetVariable(sysconf) : NULL;
      const char* setup    = elem->Attribute("setup");

      Util::Type  typ      = Util::UNDEFINED;
      if(nam != NULL && sysconf != NULL && typStr != NULL && val != NULL &&
          Util::isType(typStr, typ))
      {
        SetName(nam);
        SetSysconfigName(sysconf);
        SetValue(typ, val);
        SetSetupCommand(setup);

        ok = AddSelectionValues(node->FirstChildElement("value"), val);
        if(ok    == true &&
           _type == Util::SELECTION)  // Set Selection
        {
         SetSelection(val);
        }
      }
      else
      {
        error("Error in XML File %s", node->Value());
        ok = false;
      }

    }

    return(ok);
}

void MenuEntry::SetSysConfig( Sysconfig *sysconf )
{
  sysconf->SetVariable(GetSysconfigName(), GetValueAsString());

  for (MenuNode *subMenu = _subMenus.First(); subMenu; subMenu = _subMenus.Next(subMenu))
  {
    subMenu->SetSysConfig(sysconf );
  }
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
bool Menus::LoadXml( XMLNode * node )
{
   bool ok = true;
   XMLElement *elem = NULL;
   if(node != NULL && node->ToElement() && (node=node->FirstChild())!=NULL)
   {
        do
        {
           if(node->ToElement() && strcmp(node->Value(), "menu")== 0)
           {
            elem = node->ToElement ();
            const char* name = elem->Attribute("name");
            const char* system  = elem->Attribute("system");
            const char* help    = elem->Attribute("help");
            if( name != NULL )
            {
              Menu * m = new Menu(name);
              m->SetSystem(system);
              m->SetHelp(help);
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
 if(this != &right)
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
    if(strcmp(_values[i], value)== 0)
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
