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

#ifndef PLUGINS_H
#define PLUGINS_H
//#include <vdr/tools.h>
#include "common.h"

/**
@author Ralf Dotzert
*/
class Plugin :  public cListObject
{
private:
static const char * _trueStr;
static const char * _falseStr;
const char * _name;
const char * _info;
const char * _parameter;
int   _active;
bool  _inSystem;
bool  _protect;
public:
    Plugin();
    ~Plugin();
    bool operator< (Plugin &param);
    bool operator> (Plugin &param);
    void Print();
    const char* GetName();
    const char* GetInfo();
    const char* GetParameter();
    void SetName(const char* name);
    void SetInfo(const char* info);
    void SetParameter(const char* parameter);
    void SetActive(int flag);
    void SetProtect(bool protect);
    int  GetActive();
    int  *GetActiveRef();
    void SetInSystem(bool flag);
    bool GetInSystem();
    bool GetProtect();
    const char* GetActiveString();
};

class Plugins  : public cList<Plugin> 
{
private:
const char  *_sysconfigName;
const char  *_protectedList;
char        *_activePlugins;
public:
   
    Plugins();
    ~Plugins();
    enum Order { ASC, DESC};
    enum Where { BEFORE, BEHIND};
    int  GetNr();
    void AddPlugin(const char*name, const char*parameter,  const char*info, bool active,
                   bool protect, bool inSystem=false );
    void SetSysconfigName(const char *name);
    const char *GetSysconfigName();
    Plugin *GetPluginByName(const char*name);
    
    void SetProtectedList(const char *name);  
    const char *GetProtectedList();     
    bool checkProtected(const char* pluginName);  
    
    void MovePlugin(int index, int toindex, enum Where);
    void Sort( enum Order order);
    const char *GetActivePlugins();
    
    void SetLibDirPlugin(const char* pluginName);
    void Print();

};

   



#endif
