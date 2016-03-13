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

#ifndef SETUPMENU_H
#define SETUPMENU_H
#include <vdr/menu.h>
#include <vdr/interface.h>
#include "config.h"
#include "common.h"

/**
@author Ralf Dotzert
*/
class cSetupPluginParameter : public cOsdMenu
{
  private:
    Plugin  *_plugin;
    char     _editParameter[500];
    bool     _edit;
  public:
    cSetupPluginParameter(const char *title, Plugin *plugin);
    ~cSetupPluginParameter();
    void  Set();
    eOSState ProcessKey(eKeys Key);
};

class cSetupPluginMenu : public cOsdMenu
{
private:
   Config  *_config;
   Plugins *_plugins;
   int      _startIndex;
   bool     _moveMode;
   bool     _sortMode;
public:
    cSetupPluginMenu(Config  *config);
    ~cSetupPluginMenu();
    void  Set();
    eOSState ProcessKey(eKeys Key);
};

class cSetupMenu : public cOsdMenu
{
private:
   bool     _loaded_config;
protected:
   Config  *_config;
public:
    cSetupMenu(bool load=true);
    ~cSetupMenu();
    void  Set();
    eOSState ProcessKey(eKeys Key);
    eOSState StandardProcessKey(eKeys Key);
    eOSState GetCodeProcessKey(eKeys Key);
};

class cSetupGenericMenu : public cSetupMenu
{
  private:
    MenuNode *_node;
    bool      _editItem;
    Config   *_config;
  public:
    cSetupGenericMenu(const char *title, MenuNode *node, Config  *config);
    ~cSetupGenericMenu();
    void       Set();
    eOSState  ProcessKey(eKeys Key);
};

#endif
