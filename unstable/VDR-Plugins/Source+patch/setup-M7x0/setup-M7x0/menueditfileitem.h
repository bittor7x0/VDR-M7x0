/*
 *      menueditfileitem.cpp
 *      
 *      Copyright 2008 zjuanma <zjuanma@yahoo.es>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#ifndef EDITFILE_H
#define EDITFILE_H
#include <vdr/menuitems.h>
#include "common.h"
#include "statebag.h"
#include "menu-filebrowser.h"
#include "commands.h" 
#include "config.h"

class cMenuEditFileItem : public cMenuEditStrItem {

public:
  cMenuEditFileItem(const char *Name, char *Value,int len):cMenuEditStrItem(Name,Value,len,NULL){};
  eOSState ProcessKey(eKeys Key);
};


class cSelectCommand : public cFilebrowserCommand
{
  private:
  	MenuEntry *_e;  	
  public:
    cSelectCommand(MenuEntry *e);
    bool Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile);
    bool Matches(const char* Filename) { return true; }; 
};

  
class cOsdMenuFilebrowserSetup : public cOsdMenuFilebrowser
{   
  protected:
  
  	bool _onlyDir;
  	
  public:
    cOsdMenuFilebrowserSetup(char* Directory,cFilebrowserStatebag* Statebag,MenuEntry *e,bool only_dir=false): cOsdMenuFilebrowser(Directory,Statebag) {
    	_onlyDir = only_dir;
    	Statebag->GetCommands()->Add(new cFilebrowserCommandContainer(new cSelectCommand(e)));
    };
    
    bool MatchesFilter(dirent64* Entry);
    
    static cOsdMenuFilebrowserSetup* CreateFilebrowser(char* BaseDir,MenuEntry *e);
    
    static cOsdMenuFilebrowserSetup* CreateDirectorybrowser(char* BaseDir,MenuEntry *e);
        
};
  
#endif //EDITFILE_H

