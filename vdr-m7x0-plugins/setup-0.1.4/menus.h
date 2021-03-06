/*********************************************************
 * DESCRIPTION:
 *             Header File
 *
 * $Id$
 *
 * Contact:    ranga@vdrtools.de
 *
 * Copyright (C) 2004 by Ralf Dotzert
 *********************************************************/

#ifndef MENUS_H
#define MENUS_H
#include "common.h"
#include "util.h"
#include "tinyxml2.h"
#include "sysconfig.h"

using namespace tinyxml2;

/**
@author Ralf Dotzert
*/
class Menu;
class MenuEntry;
class MenuEntryValueList;
class MenuNode;

class SubMenus : public cList<MenuNode> {};

class MenuNode :  public cListObject
{
public:
    enum Type {UNDEFINED, MENU, MENUSYSTEM, ENTRY};
    MenuNode();
    virtual ~MenuNode();
    void destroy();
    void SetNode(Menu *menu);
    void SetNode(MenuEntry *menuEntry);
    Menu      *GetMenu();
    MenuEntry *GetMenuEntry();
    MenuNode::Type GetType();
    const char *GetName();
    int GetNr();
    MenuNode *GetNode(int index);
    void SetSysConfig(Sysconfig *sysconf);
private:
    MenuNode::Type  _objType;
    Menu           *_menu;
    MenuEntry      *_menuEntry;
};

class Menu
{
protected:
    const char *_name;
    const char *_system;
    SubMenus   _subMenus;
    const char *_help;
    XMLDocument _xmlDoc;
public:
    Menu();
    Menu(const char *name);
    virtual ~Menu();
    void destroy();
    void SetName(const char *nam);
    const char *GetName();
    void SetSystem(const char *system);
    const char *GetSystem();
    int  GetNr();
    const char * GetHelp( );
    void SetHelp( const char * help );
    MenuNode *GetNode(int index);
    void AddNode(Menu* menu);
    void AddNode(MenuEntry* menuEntry);
    bool LoadXml( XMLNode *node  );
    void SetSysConfig(Sysconfig *sysconf);
};

class MenuEntryValueList
{
  private:
    const char **_values;
    int          _nr;
    int         _selection;
  public:
    MenuEntryValueList();
    ~MenuEntryValueList();
    MenuEntryValueList const &operator =(MenuEntryValueList const &right   );
    void copy(MenuEntryValueList const &other);
    void destroy();
    void Add(const char *value);
    int  GetNr();
    const char*GetValue(int index);
    const char**GetValues();
    int  *GetReferenceSelection();
    const char *GetSelectedValue();
    void SetSelection(const char *value);
};

class MenuEntry : public Menu
{
protected:
    const char *_sysconfigName;
    const char *_valueText;
    const char *_valueIp;
    const char *_setupCommand;
    int         _valueTextMaxLen;
    int         _valueIpMaxLen;
    int         _valueNumber;
    int         _valueBool;
    MenuEntryValueList  _selectionValues;
    Util::Type  _type;
public:
    MenuEntry();
    ~MenuEntry();
    void destroy();
    bool AddEntry(XMLNode *node);
    bool AddSubEntry(XMLNode *node);
    bool AddSelectionValues(XMLNode *node, const char *selection);
    void SetSysconfigName(const char* nam);
    const char*GetSysconfigName();
    void SetValue(Util::Type type,const char* val);
    const char*GetValue();
    const char*GetValueIp() {return _valueIp;}
    void SetSetupCommand(const char *command);
    const char* GetSetupCommand();
    int   GetNrOfSelectionValues();
    const char *GetSelectionValue(int index);
    const char *GetSelectedValue();
    const char **GetSelectionValues();
    int   *GetReferenceSelection();
    void SetSelection(const char* val);
    int   GetValueTextMaxLen();
    int   GetValueIpMaxLen() {return _valueIpMaxLen;}
    int   *GetValueBoolRef();
    int   *GetValueNumberRef();
    const char*GetValueAsString();
    void SetType(Util::Type typ);
    void SetSysConfig(Sysconfig *sysconf);
    Util::Type GetType();
};

class Menus : public cList<MenuNode>
{
public:
    Menus();
    ~Menus();
    bool LoadXml(XMLNode *node);
    bool AddNode( Menu *menu );
    int  GetNr();
    MenuNode  *GetMenuNode(int index);
};

#endif
