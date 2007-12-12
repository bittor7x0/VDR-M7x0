/****************************************************************************
 * DESCRIPTION: 
 *             Creates VDR Menus
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de        
 *
 * Copyright (C) 2004 by Ralf Dotzert 
 ****************************************************************************/

#include <stdlib.h>
#include <vdr/menuitems.h>
#include <string>

#include "setupmenu.h"
#include "config.h"
#include "plugins.h"
#include "debug.h"
#include "i18n.h"
//#include "setupsetup.h"
#include "setupsystemmenu.h"



//#################################################################################################
//   cSetupPluginMenu
//		Main Menu for plugin setup
//#################################################################################################
cSetupPluginMenu::cSetupPluginMenu(Config  *config) : cOsdMenu(tr("Plugins activate / deactivate"), 25)
{
  _moveMode=false;
  _sortMode=false;

  if( config != NULL && config->GetPlugins() != NULL )
  {
    _config  = config;
    _plugins = config->GetPlugins();
    Set();
  }
}

cSetupPluginMenu::~cSetupPluginMenu()
{
}

/**
 * 
 */
void cSetupPluginMenu::Set( )
{

  int current = Current();
  
  Clear();
  for(int i=0; i<_plugins->GetNr(); i++)
  {
    if(_plugins->Get(i)->GetProtect())
    {
        char *title = NULL;

        asprintf(&title, "%s:\t%s", _plugins->Get(i)->GetInfo(),tr("protected")); 
        Add(new cOsdItem (title));
        free(title);
    }
    else
       Add(new cMenuEditBoolItem(_plugins->Get(i)->GetInfo(), _plugins->Get(i)->GetActiveRef(), tr("no"), tr("yes")));
  }
  if( current == -1 && _plugins->GetNr() >0)
    current=0;
    
  SetCurrent(Get(current));
  if( _moveMode)
    SetHelp(tr("PageUp"),  tr("PageDown"), tr("Before"), tr("After"));
  else
    SetHelp(tr("PageUp"),  tr("PageDown"), tr("Parameter"), tr("Move"));
  
  setHelp();
  Display();
}

eOSState cSetupPluginMenu::ProcessKey( eKeys Key )
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);
  if(HasSubMenu() || HadSubMenu)
    return state;

  switch(Key)
    {
            case kOk :
                        _config->SaveFile();
                        return osBack;
                        break;

            case kRed:   PageUp();
                         Set();
                         break;

            case kGreen: PageDown();
                         Set();
                         break;
            case kYellow:

                      if(_moveMode)
                      {
                        _plugins->MovePlugin(_startIndex, Current(), Plugins::BEFORE);
                        _moveMode = !_moveMode;
                        Set();
                      }
                      else // Edit Parameter
                      {
                        return(AddSubMenu(new cSetupPluginParameter(tr("Plugin-Parameter"), _plugins->Get(Current()))));
                      }
                      break;
            case kBlue:
                      if(_moveMode)
                      {
                        _plugins->MovePlugin(_startIndex, Current(), Plugins::BEHIND);
                      }
                      else
                      {
                        _startIndex = Current();
                      }
                      _moveMode = !_moveMode;
                      Set();
                      break;
            case kDown:
            case kUp:
                    setHelp();
                    break;
            default : break;
    }
    return state;
}

void cSetupPluginMenu::setHelp()
{
  char *tmp=NULL;
  int  current = Current();
  if(current >= 0 )
  {
    asprintf(&tmp, "%s: %s", tr("Plugin"), _plugins->Get(current)->GetName());
    SetStatus(tmp);
    free(tmp);
  }

}


//#################################################################################################
//	cSetupPluginParameter
//    Edit Plugin Parameter
//################################################################################################
static char *ALLOW_ALL_PARAM_CHARS =" aäbcdefghijklmnoöpqrsßtuüvwxyz0123456789+-={}[]().,;#*?!@/~\"\'";
cSetupPluginParameter::cSetupPluginParameter( const char *title, Plugin * plugin ) : cOsdMenu(title, 25)
{
  _plugin = plugin;
  _edit   = false;
  Set();
}

cSetupPluginParameter::~ cSetupPluginParameter( )
{
}

void cSetupPluginParameter::Set( )
{
  char *tmp=NULL;
  const char *param = _plugin->GetParameter();
  Clear();
  if( param == NULL)
    _editParameter[0]='\0';
  else
  {
    strncpy(_editParameter, param, sizeof(_editParameter));
    _editParameter[sizeof(_editParameter)]='\0';
  }
  Add(new cMenuEditStrItem(tr("Plugin-Parameter"), _editParameter, sizeof(_editParameter), ALLOW_ALL_PARAM_CHARS));

  asprintf(&tmp, "%s: %s", tr("Plugin"), _plugin->GetName());
  SetStatus(tmp);
  free(tmp);
  Display();
}

eOSState cSetupPluginParameter::ProcessKey( eKeys Key )
{

  eOSState state = cOsdMenu::ProcessKey(Key);

  switch(Key)
  {
     case kOk :
                if(!_edit)
                  return osBack;
                else
                {
                  _plugin->SetParameter(_editParameter);
                  _edit=false;
                }
                break;
    case kRight:_edit = true;
                break;
     default : break;
  }
  return state;
}


//#################################################################################################
//  cSetupVdrMenu
//################################################################################################

/** Removed as we du not uses the submenupatch
static char *ALLOW_ALL_CHARS =" aäbcdefghijklmnoöpqrsßtuüvwxyz0123456789-.";
cSetupVdrMenu::cSetupVdrMenu(const char *title): cOsdMenu(title, 25)
{
  _startIndex           =0;
  _createEditNodeIndex  =0;
  
  // Load Menu Configuration
  char *menuXML = NULL;
  asprintf(&menuXML, "%s/setup/vdr-menu.xml", cPlugin::ConfigDirectory());
  _vdrSubMenu.LoadXml(menuXML);
  free(menuXML);

  _menuState  = UNDEFINED;
  Set();
}

cSetupVdrMenu::~ cSetupVdrMenu( )
{
}

void cSetupVdrMenu::Set( )
{
  int current = Current();
  cSubMenuNode *node = NULL;
  int nr = _vdrSubMenu.GetNrOfNodes();


  Clear();

  switch(_menuState)
  {
    case UNDEFINED:
    case MOVE     :
                for( int i=0; i<nr; i++)
                {
                    if((node = _vdrSubMenu.GetAbsNode(i)) != NULL)
                    {
                        char *tmp=createMenuName(node);
                        Add(new cOsdItem(tmp));
                        free(tmp);
                    }
                }
                current=_createEditNodeIndex;
                if( current >= nr)
                    current = nr-1;
                break;
     case EDIT:
                strncpy(_editTitle, _vdrSubMenu.GetAbsNode(_createEditNodeIndex)->GetName(), sizeof(_editTitle));
                _editTitle[sizeof(_editTitle)]='\0';
                Add(new cMenuEditStrItem(tr("change menu name"), _editTitle, sizeof(_editTitle), ALLOW_ALL_CHARS));
                break;
     case CREATE:
                strncpy(_editTitle, "", sizeof(_editTitle));
                Add(new cMenuEditStrItem(tr("change menu name"), _editTitle, sizeof(_editTitle), ALLOW_ALL_CHARS));
                break;       
     default:
                break;
  }
    

  SetCurrent(Get(current));

  setHelp();
  

  Display();
}




eOSState cSetupVdrMenu::ProcessKey( eKeys Key )
{
    cSubMenuNode *node = NULL;
    eOSState state = cOsdMenu::ProcessKey(Key);
    if( HasSubMenu())
      return state;

    switch(Key)
    {
            case kRed:
                      if(_menuState == UNDEFINED )
                      {
                          _menuState = CREATE;
                          _createEditNodeIndex = Current();
                          Set();
                      }
                      break;
            case kGreen:
                      if(_menuState == MOVE )
                      {
                        if( (node= _vdrSubMenu.GetAbsNode(Current()))!= NULL && node->GetType()== cSubMenuNode::MENU)
                        {
                          _vdrSubMenu.MoveMenu(_startIndex, Current(), cSubMenu::INTO);
                          _menuState = UNDEFINED;
                          _createEditNodeIndex = Current();
                          SetStatus(NULL);
                          Set();
                        }
                      }
                      else
                      if(_menuState == UNDEFINED )
                      {
                        if( (node= _vdrSubMenu.GetAbsNode(Current()))!= NULL && node->GetType()== cSubMenuNode::MENU)
                        {
                            _menuState = EDIT;
                            _createEditNodeIndex = Current();
                            Set();
                        }
                      }
                      setHelp();
                      break;
           case kYellow:
                      if(_menuState == MOVE )
                      {
                        _vdrSubMenu.MoveMenu(_startIndex, Current(), cSubMenu::BEFORE);
                        _menuState = UNDEFINED;
                        _createEditNodeIndex =Current();
                        SetStatus(NULL);
                        Set();
                      }
                      else
                      if(_menuState == UNDEFINED )
                      {
                        if(Interface->Confirm(tr("Delete Menu?")))
                        {
                         _createEditNodeIndex = Current();
                         _vdrSubMenu.DeleteMenu(_createEditNodeIndex);
                          _menuState = UNDEFINED;
                          Set();
                        }
                      }
                      break;
           case kBlue:
                      if(_menuState == MOVE )
                      {
                        _vdrSubMenu.MoveMenu(_startIndex, Current(), cSubMenu::BEHIND);
                         _createEditNodeIndex =Current();
                        _menuState = UNDEFINED;
                        SetStatus(NULL);
                      }
                      else
                      if(_menuState == UNDEFINED )
                      {
                        _startIndex = Current();
                         _createEditNodeIndex = _startIndex;
                        _menuState = MOVE;
                        SetStatus(tr("Up/Dn for new location - color key to move"));
                      }
                      Set();
                      setHelp();
                      break;

          case kOk :
                       switch (_menuState)
                       {
                        case UNDEFINED:
                                    // Save Menus to file and exit submenu
                                     state=osBack;
                                    _vdrSubMenu.SetMenuSuffix(setupSetup._menuSuffix);
                                    _vdrSubMenu.SaveXml();
                                    break;
                        case CREATE:
                                    _vdrSubMenu.CreateMenu(_createEditNodeIndex, _editTitle);
                                    _menuState = UNDEFINED;
                                    Set();
                                    break;
                        case EDIT:
                                    _vdrSubMenu.GetAbsNode(_createEditNodeIndex)->SetName(_editTitle);
                                    _menuState = UNDEFINED;
                                    Set();
                                    break;

                        default:
                                    break;
                       }
                       break;
            case kBack:
                       if( _menuState == CREATE || _menuState==EDIT)
                       {
                         _menuState=UNDEFINED;
                         Set();
                         state=osContinue;
                       }

            case kDown:
            case kUp:
            case kRight:
            case kLeft:
                     if(_menuState == MOVE)
                        setHelp();
                     break;
            default :
                     break;
    }
    return state;
}
// --------------- Private Methods ---------------------
void cSetupVdrMenu::setHelp( )
{
  cSubMenuNode *node = NULL;
  
  if( _menuState == MOVE)
  {
    if( (node= _vdrSubMenu.GetAbsNode(Current()))!= NULL && node->GetType()== cSubMenuNode::MENU)
      SetHelp(NULL, tr("Into"), tr("Before"), tr("After"));
    else
      SetHelp(NULL, NULL, tr("Before"), tr("After"));
  }
  else
    SetHelp(tr("Create"), tr("Edit"), tr("Delete"), tr("Move"));
}

char * cSetupVdrMenu::createMenuName( cSubMenuNode *node )
{
  char *prefix = NULL;
  char *tmp=NULL;
  int   level = node->GetLevel();

  // Set Prefix
  prefix = (char*) malloc(1);
  prefix[0] = '\0';
  for(int i=0; i<level; i++)
    asprintf(&prefix, "|   %s", prefix);

  
  cSubMenuNode::Type type =node->GetType();
  
  if( type == cSubMenuNode::MENU)
    asprintf(&tmp, "%s+%s", prefix, node->GetName());
  else
   if( type == cSubMenuNode::SYSTEM)
    asprintf(&tmp, "%s %s",  prefix, tr(node->GetName()));
   else
    asprintf(&tmp, "%s %s",  prefix, node->GetName());

  free(prefix);
  
  return(tmp);
}

*/

//#################################################################################################
//  cSetupGenericMenu
//		used to display a menupage, based on the data in the minivdr-setup.xml file
//################################################################################################
cSetupGenericMenu::cSetupGenericMenu(const char *title, MenuNode *node, Config  *config)  : cSetupMenu()


{
  debug("cSetupGenericMenu::Constructor");
  _node     = node;
  _editItem = false;
  _currLine = 0;
  _config   = config;

  SetTitle(title);
  SetCols(25); 

  
  if( _node != NULL)
    Set();
}


cSetupGenericMenu::~cSetupGenericMenu()
{
}

const char * cSetupGenericMenu::nohk(const char *str)
{
  static std::string tmp;

//changed to return default value as we have no menu prefix
//  tmp = setupSetup._entryPrefix;

//  if( strlen(setupSetup._entryPrefix)==0 || setupSetup._entryPrefix[0] == ' ')
    tmp = "";
//  else
//  {
//    tmp =  "  ";
//    tmp += setupSetup._entryPrefix;
//    tmp += "  ";
//  }

  tmp += str;
  return(tmp.c_str());
}

/**
 * 
 */
void cSetupGenericMenu::Set( )
{
  int current = Current();
  Clear();
  SetHasHotkeys();
  for(int i=0; i< _node->GetNr(); i++)
  {
    MenuNode::Type type = _node->GetNode(i)->GetType();
    
    if( type == MenuNode::ENTRY )
    {

      MenuEntry *e = _node->GetNode(i)->GetMenuEntry();
      switch( e->GetType() )
      {
        case Util::BOOL:
          Add(new cMenuEditBoolItem(nohk(e->GetName()), e->GetValueBoolRef(), tr("no"), tr("yes")));
             break;
        case Util::NUMBER:
          Add(new cMenuEditIntItem(nohk(e->GetName()), e->GetValueNumberRef(), 0, 999999999));
             break;
        case Util::TEXT:
          Add(new cMenuEditStrItem(nohk((char*)e->GetName()), (char*)e->GetValue(), e->GetValueTextMaxLen(),
                                      " aäbcdefghijklmnoöpqrsßtuüvwxyz0123456789-+.#~_/:"));
             break;
         case Util::NUMBER_TEXT:
           Add(new cMenuEditStrItem(nohk((char*)e->GetName()), (char*)e->GetValue(), e->GetValueTextMaxLen(),
                                      "0123456789"));
             break;
        case Util::IP:

          Add(new cMenuEditStrItem(nohk((char*)e->GetName()), (char*)e->GetValueIp(), e->GetValueIpMaxLen(),
              ".0123456789"));
          break;
        case Util::HEX:
          Add(new cMenuEditStrItem(nohk((char*)e->GetName()), (char*)e->GetValue(), e->GetValueTextMaxLen(),
                                      "0123456789ABCDEF:"));
             break;
        case Util::SELECTION:
        {
              if(  e->GetNrOfSelectionValues() == 0 )
              {
                char *txt=NULL;
                asprintf(&txt, "%s:\t%s", nohk(e->GetName()),tr("missing channels/*.conf"));
                Add(new cOsdItem (txt));
                free(txt);
              }
              else
                Add(new cMenuEditStraItem(nohk(e->GetName()), e->GetReferenceSelection(),
                                       e->GetNrOfSelectionValues(),
                                       e->GetSelectionValues()) );
        }
             break;
        default:
                break;
      }
    }
    else
    {
       char *tmp = NULL;
//       asprintf(&tmp, "%s%s", _node->GetNode(i)->GetName(), setupSetup._menuSuffix);
// we have no menu suffix
    asprintf(&tmp, "%s...", _node->GetNode(i)->GetName());
       Add(new cOsdItem(hk(tmp)));
       free(tmp);
    }
  }
  SetCurrent(Get(current));
  Display();
}

eOSState cSetupGenericMenu::ProcessKey( eKeys Key )
{  
    bool HadSubMenu = HasSubMenu();
    eOSState state = cOsdMenu::ProcessKey(Key);

    if(HasSubMenu() || HadSubMenu)
      return state;

    MenuNode *n = _node->GetNode(Current());

      
      switch(Key)
      {
        case kOk : 
                       if( n!= NULL &&
                           (n->GetType() == MenuNode::MENU ||
                            n->GetType() == MenuNode::MENUSYSTEM) )
                     {
                        if(n->GetType() == MenuNode::MENU){
  							debug("=============================> Create menu page");
                          return(AddSubMenu(new cSetupGenericMenu(n->GetName(), n,  _config)));
                        }else{  // Menu "system"
                          cOsdMenu *menu = cSetupSystemMenu::GetSystemMenu(n->GetMenu()->GetSystem(), _config);
                           if(menu != NULL)
                             return(AddSubMenu(menu));
                        }
                      }
                      else
                      {
                        if(!_editItem) //close submenu only if we do not have edited an textitem
                        {

                          state=osBack;
                          _config->SaveFile(); // Write New Configurationfile

                          for(int i=0;  i < _node->GetNr(); i++)
                          {
                            n = _node->GetNode(i);
                            if( n->GetType() == MenuNode::ENTRY)
                              ExecuteCommand(n->GetMenuEntry()->GetCommand());
                          }
                          ExecuteCommand(_node->GetMenu()->GetCommand());
                        }
  
                        _editItem=false;
                      }
                      break;
              case kRed:
                        {
                          char *help = NULL;
                          if(n->GetType() == MenuNode::ENTRY)
                            help = GetLongHelp(n->GetMenuEntry()->GetHelp2());
                          else
                            help = GetLongHelp(n->GetMenu()->GetHelp2());

                          if( help != NULL )
                          {
                            eOSState newState = AddSubMenu(new cMenuText(tr("Help"), help, fontFix));
                            free(help);
                            return(newState);
                          }

                        }
                        break;
              case kGreen:
                        break;
              case kYellow:
                        break;
              case kBlue:
                        break;
              case kRight:
                        if( n!= NULL &&
                            n->GetType() == MenuNode::ENTRY &&
                            (n->GetMenuEntry()->GetType() == Util::TEXT ||
                            n->GetMenuEntry()->GetType() == Util::NUMBER_TEXT ||
                            n->GetMenuEntry()->GetType() == Util::HEX ||
                            n->GetMenuEntry()->GetType() == Util::NUMBER 
                            )
                          )
                           _editItem = true;
                        break;

                default :if( n!= NULL)
                         {
                          if( n->GetType() == MenuNode::ENTRY)
                          {
                            SetStatus( n->GetMenuEntry()->GetHelp1());
                            if( n->GetMenuEntry()->GetHelp2() != NULL)
                              SetHelp(tr("Help"), NULL, NULL, NULL);
                            else
                              SetHelp(NULL, NULL, NULL, NULL);
                          }
                          else
                          {
                            SetStatus( n->GetMenu()->GetHelp1());
                            if( n->GetMenu()->GetHelp2() != NULL)
                              SetHelp(tr("Help"), NULL, NULL, NULL);
                            else
                              SetHelp(NULL, NULL, NULL, NULL);
                          }
                         }
                        else
                            SetStatus(NULL);
                      break;
      }
    return state;
}

void cSetupGenericMenu::ExecuteCommand( const char * cmd )
{
  char *tmp=NULL;
  int   status=0;

  if( cmd != NULL)
  {
    asprintf(&tmp, "%s: %s", tr("Execute"), cmd);
    SetStatus(tmp);
    free(tmp);
    status=system(cmd);
    if( status == -1 )
      DEBUG3("%s: fork of command %s failed\n", DBG_PREFIX, cmd);
    else
    {
      if( WEXITSTATUS(status) != 0)
        DEBUG4("%s: executing of command %s returned=%d\n", DBG_PREFIX, cmd, WEXITSTATUS(status));
    }
  }
}




//#############################################################################################
// cSetupMenu
// 	The main menu that is diplayed when the plugin ist selected in the main menu 
//#############################################################################################


cSetupMenu::cSetupMenu() : cOsdMenu(tr("Gigaset Firmware setup - open7x0.org (based on setup plugin by minivdr.de)"))
{
   char *configFile=NULL;
   SetCols(20);
	
	//Load config file
   asprintf(&configFile, "%s/setup/open7x0-setup.xml", cPlugin::ConfigDirectory());

   debug("cSetupMenu.constr: Load config file %s", configFile);

  _config    = new Config( configFile );
   free(configFile);

	//Check for child lock
  _number    = 0;
  _error     = false;
  _childLock = false;
  _childLockEntered = NULL;
  _childLockEnteredHidden = NULL;
  
  if( _config!= NULL && _config->LoadFile() == true)
  {
    if(_config->GetChildLock() != NULL && strcmp(_config->GetChildLock(), "0000") != 0)
    {//Ask for childlock code
	   debug("cSetupMenu.constr: Ask child lock coder and open config page");
       _childLock = true;
       _childLockEntered          = Util::Strdupnew(_config->GetChildLock());
       _childLockEntered[0]       = '\0';
        _childLockEnteredHidden   = Util::Strdupnew(_config->GetChildLock());
       _childLockEnteredHidden[0] ='\0';
       SetAskChildCode();
    }
    else//open without childlock
	   debug("cSetupMenu.constr: Open config page");
      Set();
  }
  else
  {//Error loading config file
  debug("cSetupMenu.constr: error loading config file");
    SetStatus(tr("Error in configuration files"));
    _error = true;
  }
}


cSetupMenu::~cSetupMenu()
{
    delete _config;
    delete []_childLockEntered;
    delete [] _childLockEnteredHidden;
}

/**
 * 
 */
void cSetupMenu::Set( )
{
  int current = Current();
  Menus     *m=_config->GetMenus();
  MenuNode  *n = NULL;
  
  Clear();
  SetHasHotkeys();

  // Customized Setup Menus
  for(int i=0; i< m->GetNr(); i++)
  {
    n = m->GetMenuNode(i);
    char *tmp = NULL;
//    asprintf(&tmp, "%s%s", n->GetName(), setupSetup._menuSuffix);
// we have no menu suffix
    asprintf(&tmp, "%s...", n->GetName());
    Add(new cOsdItem (hk(tmp)));
    free(tmp);
  }
  SetCurrent(Get(current));
  SetHelp(NULL,  NULL, tr("Restart VDR"), tr("Reboot"));
  
  Display();
}

/**
 * Ask the user for the secret Child Code
 */
void cSetupMenu::SetAskChildCode( )
{
    Clear();
    char * tmp= NULL;
    asprintf(&tmp, "%s%s", tr("Enter Pin"), _childLockEnteredHidden);
    Add( new cOsdItem (tmp));
    free(tmp);
    Display();
}




/**
 * Procss Key
 * @param Key 
 * @return 
 */
eOSState cSetupMenu::ProcessKey( eKeys Key )
{
  if(_childLock)
    return(GetCodeProcessKey(Key));
  else
    return(StandardProcessKey(Key));
}

/**
 * Standard Key Processing
 * @param Key 
 * @return 
 */
eOSState cSetupMenu::StandardProcessKey( eKeys Key )
{
    bool HadSubMenu = HasSubMenu();

    eOSState state = cOsdMenu::ProcessKey(Key);

    if(HasSubMenu() || HadSubMenu)
      return(state);
    
    int current = Current();
    MenuNode *node = _config->GetMenus()->GetMenuNode(current);
    switch(Key)
    {
            case kOk :
                 if(node != NULL)
                 {
                    if( node->GetType() == MenuNode::MENU )
                    {
                      return AddSubMenu(new cSetupGenericMenu(node->GetName(), node, _config));
                    }
                    else
                      if(node->GetType() == MenuNode::MENUSYSTEM )
                      {
                        cOsdMenu * menu =cSetupSystemMenu::GetSystemMenu (hk(node->GetMenu()->GetSystem()), _config);
                        if(menu != NULL)
                          AddSubMenu(menu);
                      }
                    SetCurrent(Get(current));
                    return(osContinue);
                 }
                      break;
            case kRed: //Show Help
                    {
                      char *help = NULL;
                      if(node->GetType() == MenuNode::ENTRY)
                        help = GetLongHelp(node->GetMenuEntry()->GetHelp2());
                      else
                        help = GetLongHelp(node->GetMenu()->GetHelp2());

                      if( help != NULL )
                      {
                        eOSState newState = AddSubMenu(new cMenuText(tr("Help"), help, fontFix));
                        free(help);
                        return(newState);
                      }

                    }
                      break;
            case kYellow: //Restart VDR
                        if (Interface->Confirm(cRecordControls::Active() ? tr("Recording - restart anyway?") : tr("Really restart?")))
                        {
                          cThread::EmergencyExit(true);
                          return osEnd;
                        }
                        return osContinue;
                      break;
            case kBlue:  //Reeboot Linux
                        if (Interface->Confirm(cRecordControls::Active() ? tr("Recording - restart anyway?") : tr("Really reboot?")))
                        {
                          cThread::EmergencyExit(true);
                          system(_config->GetBootLinux());
                        }
                        return osContinue;
                      break;

            case kNone:
                      break;

              default :if( node!= NULL)
              {
                if( node->GetType() == MenuNode::ENTRY)
                {
                  SetStatus( node->GetMenuEntry()->GetHelp1());
                  if( node->GetMenuEntry()->GetHelp2() != NULL)
                    SetHelp(tr("Help"), NULL, tr("Restart VDR"), tr("Reboot"));
                  else
                    SetHelp(NULL, NULL,  tr("Restart VDR"), tr("Reboot"));
                }
                else
                {
                  SetStatus( node->GetMenu()->GetHelp1());
                  if( node->GetMenu()->GetHelp2() != NULL)
                    SetHelp(tr("Help"), NULL,  tr("Restart VDR"), tr("Reboot"));
                  else
                    SetHelp(NULL, NULL,  tr("Restart VDR"), tr("Reboot"));
                }
              }
              else
                SetStatus(NULL);
              break;
    }

    return state;

}



/**
 * Key Processing for fetching Secret Code
 * @param Key 
 * @return 
 */
eOSState cSetupMenu::GetCodeProcessKey( eKeys Key )
{
  int num;
  eOSState state = cOsdMenu::ProcessKey(Key);
    
    switch(Key)
    {
            case k0 ... k9:
                num = Key - k0;
                sprintf(_childLockEntered, "%s%d", _childLockEntered, num);
                sprintf(_childLockEnteredHidden, "%s*", _childLockEnteredHidden);
                SetAskChildCode();
                if(strlen(_childLockEntered) == strlen(_config->GetChildLock()))
                {
                  if(strcmp(_childLockEntered,_config->GetChildLock())==0)
                  {
                    _childLock=false;
                    Set();
                  }
                  else
                  {
                    _childLockEntered[0]='\0';
                    _childLockEnteredHidden[0]='\0';
                    SetStatus(tr("Invalid Pin !"));
                  }
                }
                else
                {
                  SetStatus(NULL);
                }

                break;


             case kBack:
                      return osBack;
                      break;
             case kNone:
                      break;
            default : SetStatus(NULL);
                      break;
    }
    return state;
}

//
char * cSetupMenu::GetLongHelp( const char * help )
{
  char *helpfile    = NULL;
  char *result = NULL;

  if( help != NULL)
  {
    asprintf(&helpfile, "%s/setup/help/%s", cPlugin::ConfigDirectory(), help);
    FILE *fp = fopen(helpfile, "r");
    if(fp != NULL )
    {
      fseek(fp, 0L, SEEK_END);
    
      long len = ftell(fp);
      fseek(fp, 0L, SEEK_SET);
      int byteRead=0;
      if( (result=(char*) malloc(len+1)) == NULL)
        DEBUG3("%s: can not allocate buffer for Helpfile: %s\n", DBG_PREFIX, helpfile );
      else
        if( (byteRead=fread(result, 1, len, fp)) == -1)
          DEBUG3("%s: can not read file: Helpfile: %s\n", DBG_PREFIX, helpfile);
      else
        result[byteRead] = '\0';
      fclose(fp);
    }
    else
      DEBUG4("%s: can not open Helpfile: %s, %s\n", DBG_PREFIX, helpfile, strerror(errno));
    free(helpfile);
  }
  return(result);
}




