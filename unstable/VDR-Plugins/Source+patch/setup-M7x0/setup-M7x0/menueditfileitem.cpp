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
 
#include "menueditfileitem.h"
#include <stdlib.h>
#include <vdr/menuitems.h>
#include <vdr/osdbase.h>
#include <string>
 
// --- cOsdMenuFilebrowserSetup ------------------------------------------------------
bool cOsdMenuFilebrowserSetup::MatchesFilter(dirent64* Entry)
{
  
  if ( _onlyDir ) {
  	 return (Entry->d_type == DT_DIR ) ;
  }
    
  return cOsdMenuFilebrowser::MatchesFilter(Entry);
}

cOsdMenuFilebrowserSetup* cOsdMenuFilebrowserSetup::CreateFilebrowser(char* BaseDir,MenuEntry *e) {
	cFilebrowserStatebag* Statebag = new cFilebrowserStatebag();
	//Set some flags
	Statebag->ShowHiddenFiles = false;
	Statebag->ShowFullPath = true;
	//Set Base Directory - that is important
	strcpy(Statebag->BaseDir, BaseDir);	
	return new cOsdMenuFilebrowserSetup("/",Statebag,e,false);
}
cOsdMenuFilebrowserSetup* cOsdMenuFilebrowserSetup::CreateDirectorybrowser(char* BaseDir,MenuEntry *e) {
	cFilebrowserStatebag* Statebag = new cFilebrowserStatebag();
	//Set some flags
	Statebag->ShowHiddenFiles = false;
	Statebag->ShowFullPath = true;
	//Set Base Directory - that is important
	strcpy(Statebag->BaseDir, BaseDir);	
	return new cOsdMenuFilebrowserSetup("/",Statebag,e,true);
}

// --- cSelectCommand ------------------------------------------------------ 
 cSelectCommand::cSelectCommand(MenuEntry *e) : cFilebrowserCommand(NULL)
/* Statebag isn't needed - but will want to pass it to 
 * the ctor if you need access to marked files etc. */
{
  _e = e;
  /* Set a name */
  Name=strdup(tr("Select"));

}

bool cSelectCommand::Execute(cOsdMenu* Menu, char* DestinationFile, char* CurrentFile)
{
/*
 * Now you can do just anything you like - DestinationFile will be NULL if you didn't
 * set appropriate Use* Flags in your constructor*/
/* Another approach would be to set State in your ctor, save the filename(s) here somewhere
 * and react to the state given in your cOsdMenu that is he parent of the filebrowser menu
 * in ProcessKey
 */  
  if ( Menu && CurrentFile ) {
	  char tmp[2048];
	  snprintf(tmp,sizeof(char)*2048,"\"%s\"",CurrentFile);
	  _e->SetValue(_e->GetType(),tmp);    
	  
	  debug ("Destination [%s] -- Current [%s] => value [%s]",DestinationFile,tmp,_e->GetValue());  
		
	  //Repintar el menu
	  Menu->Display();
	  State=osBack;
	  return true;
  }
  return false;
}

// --- cMenuEditFileItem ------------------------------------------------------  
eOSState cMenuEditFileItem::ProcessKey(eKeys Key){
  	debug("cMenuEditFileItem::ProcessKey [%s]", value);
  	switch(Key) {
  		case kRight:
  			debug("cMenuEditFileItem::ProcessKey:kRight");    		
	    case kOk:
	    	debug("cMenuEditFileItem::ProcessKey:kOk");  
	    	SetValue(value);
	        return osBack;	        									
	}
	SetValue(value);
  	return cMenuEditItem::ProcessKey(Key);
  };
