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

#define DEFAULT_PATH "/media"
 
// --- cOsdMenuFilebrowserSetup ------------------------------------------------------
bool cOsdMenuFilebrowserSetup::MatchesFilter(dirent64* Entry)
{
    
  if ( _onlyDir ) {
  	 return !(!Statebag->ShowHiddenFiles && Entry->d_name[0]=='.' && strcmp(Entry->d_name, "..")!=0) 
  	 		&& (Entry->d_type == DT_DIR || Entry->d_type == DT_UNKNOWN ) ;
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
	return new cOsdMenuFilebrowserSetup(DEFAULT_PATH,Statebag,e,false);
}
cOsdMenuFilebrowserSetup* cOsdMenuFilebrowserSetup::CreateDirectorybrowser(char* BaseDir,MenuEntry *e) {
	cFilebrowserStatebag* Statebag = new cFilebrowserStatebag();
	//Set some flags
	Statebag->ShowHiddenFiles = false;
	Statebag->ShowFullPath = true;
	//Set Base Directory - that is important
	strcpy(Statebag->BaseDir, BaseDir);	
	return new cOsdMenuFilebrowserSetup(DEFAULT_PATH,Statebag,e,true);
}

// --- cSelectCommand ------------------------------------------------------ 
 cSelectCommand::cSelectCommand(MenuEntry *e,cFilebrowserStatebag* Statebag) : cFilebrowserCommand(NULL)
/* Statebag isn't needed - but will want to pass it to 
 * the ctor if you need access to marked files etc. */
{
  _e = e;
  _Statebag = Statebag;
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
	  char tmp[8912];
	  char tmp2[8912];
	  tmp[0]=0;
	  	  
	  for(cStringContainer* i=_Statebag->GetSelectedFiles()->First(); i; i=_Statebag->GetSelectedFiles()->Next(i)){    	
    	if (tmp[0]!=0) strcat(tmp," ");
      	strcat(tmp,i->GetObject());
      }
      
      if ( tmp[0] == 0 ) {
      	snprintf(tmp2,sizeof(char)*8912,"\"%s\"",CurrentFile);
	  }else {
	  	snprintf(tmp2,sizeof(char)*8912,"\"%s\"",tmp);
	  }
	  	
      debug ("Destination [%s] -- Current [%s] => value [%s]",DestinationFile,tmp2,_e->GetValue());  	
      _e->SetValue(_e->GetType(),tmp2);    	
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
  		case kLeft: 
  		case kRight:			            								  			
  			return osContinue;  		
	}	
  	return cMenuEditItem::ProcessKey(Key);
  };
