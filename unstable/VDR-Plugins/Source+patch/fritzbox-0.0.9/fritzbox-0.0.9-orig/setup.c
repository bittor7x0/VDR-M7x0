/*
 * Fritz!Box plugin for VDR
 *
 * Copyright (C) 2007 Joachim Wilke <vdr@joachim-wilke.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

#include "setup.h"
#include "fonbuchmanager.h"
#include <vdr/menuitems.h>

#if VDRVERSNUM < 10509
#define trVDR(s) tr(s)
#endif

sFritzboxConfig fritzboxConfig;

const char *cMenuSetupFritzbox::StoreMsn(){
	std::vector<std::string>::iterator it;
	std::string *msnLine = new std::string();
	
	
	for (it = fritzboxConfig.msn.begin(); it < fritzboxConfig.msn.end(); it++){
		*msnLine += *it;
		*msnLine += ' ';
	}
	return msnLine->c_str(); // TODO: MemLeak?
}

const char *cMenuSetupFritzbox::StoreFonbooks() {
	std::vector<std::string>::iterator it;
	std::string *fbLine = new std::string();


	for (it = fritzboxConfig.selectedFonbookIDs.begin(); it < fritzboxConfig.selectedFonbookIDs.end(); it++){
		*fbLine += *it;
		*fbLine += ' ';
	}		
	return fbLine->c_str(); // TODO: MemLeak?
}

void cMenuSetupFritzbox::Setup(void) {
	// save current postion
	int current = Current();
	// clear entries, if any
		Clear();
	// build up setup menu
	Add(new cMenuEditStrItem (tr("Fritz!Box URL"),                  		url,           			MaxFileName, tr(FileNameChars)));
	Add(new cMenuEditStrItem (tr("Password"),                       		password, 	   			MaxFileName, tr(FileNameChars)));
	Add(new cMenuEditBoolItem(tr("Mute on call"),                   		&muteOnCall,   			trVDR("no"), 	 trVDR("yes")));
	Add(new cMenuEditBoolItem(tr("Pause on call"),                   		&pauseOnCall,  			trVDR("no"), 	 trVDR("yes")));
	Add(new cMenuEditBoolItem(tr("Show incoming calls"),            		&showNumber,   			trVDR("no"), 	 trVDR("yes")));
	Add(new cMenuEditBoolItem(tr("Detailed call lists"),                    &showNumberInCallList, 	trVDR("no"), 	 trVDR("yes")));
	Add(new cMenuEditBoolItem(tr("Group call lists by date"),               &showDaySeparator,      trVDR("no"),     trVDR("yes")));
	Add(new cMenuEditBoolItem(tr("Hide main menu entry"), 		    		&hideMainMenu, 			trVDR("no"),  	 trVDR("yes")));
	Add(new cOsdItem         (tr("Setup phonebooks to use..."), osUser1));
	Add(new cMenuEditBoolItem(tr("Restrict monitor to certain extensions"), &msnFilter,    			trVDR("no"), 	 trVDR("yes")));
	if (msnFilter) {
		Add(new cMenuEditIntItem (tr("Number of monitored extensions"), &msnCount, 1, MAX_MSN_COUNT));
		for (int p = 0; p < msnCount; p++)
			Add(new cMenuEditStrItem(tr("Extension"), msn[p], MaxFileName, "1234567890"));	
	}	
	// restore current position
	SetCurrent(Get(current));
	// refresh display
	Display();
}	

eOSState cMenuSetupFritzbox::ProcessKey(eKeys Key) {

	eOSState state = cMenuSetupPage::ProcessKey(Key);
	
	if (state == osUser1) {
		return AddSubMenu(new cMenuSetupFritzboxFonbooks(&selectedFonbookIDs));
	}

	if (Key != kNone) {
		if (msnFilter != msnFilterBefore) {
			// deactivate MSN Filter
			if (msnFilter == 0) {
				msnCount = 0;
			} else {
				msnCount = 1;
			}
			msnFilterBefore = msnFilter;
		}
		if (msnCount != msnCountBefore) {
			// add new MSN elements
			if (msnCount > msnCountBefore) {
				for (int i=msnCountBefore; i<msnCount; i++) {
					msn[i] = (char *) malloc(MaxFileName * sizeof(char));
					msn[i][0] = 0;
				}
			} 
			// remove existing MSN elements				
			else {				
				for (int i=msnCountBefore; i>msnCount; i--)
					free(msn[i-1]);
			}
			Setup();
			msnCountBefore = msnCount;
		}
	}
	return state;
}

void cMenuSetupFritzbox::Store(void) {
	fritzboxConfig.url            		= url;
	int i = 0;
	// only store the password if it was changed
	while (password[i]) {
		if (password[i] != '*') {
			fritzboxConfig.password       		= password;		
			break;
		}
		i++;
	}
	fritzboxConfig.muteOnCall     		= muteOnCall;
	fritzboxConfig.pauseOnCall     		= pauseOnCall;
	fritzboxConfig.showNumber     		= showNumber;
	fritzboxConfig.showNumberInCallList = showNumberInCallList;
	fritzboxConfig.showDaySeparator     = showDaySeparator;
	fritzboxConfig.hideMainMenu   		= hideMainMenu;
	fritzboxConfig.msn.clear();
	for (int i=0; i < msnCount; i++) {
		std::string s = msn[i];
		fritzboxConfig.msn.push_back(s);
	}
	for (size_t i=0; i<selectedFonbookIDs.size(); i++) {
		bool initialize = true;
		for (size_t j=0; j<fritzboxConfig.selectedFonbookIDs.size(); j++) {
			if (selectedFonbookIDs[i].compare(fritzboxConfig.selectedFonbookIDs[j]) == 0) {
				initialize = false;
				break;
			}
		}
		if (initialize)
			(*(cFonbuchManager::GetFonbuchManager()->GetFonbooks()))[selectedFonbookIDs[i]]->Initialize();
	}
	fritzboxConfig.selectedFonbookIDs   = selectedFonbookIDs;
	
	SetupStore("Url",          			url);
	SetupStore("Password",     			fritzboxConfig.password.c_str());
	SetupStore("MuteOnCall",   			muteOnCall);
	SetupStore("PauseOnCall",  			pauseOnCall);
	SetupStore("ShowNumber",   			showNumber);
	SetupStore("ShowNumberInCallList", 	showNumberInCallList);
	SetupStore("ShowDaySeparator",      showDaySeparator);
	SetupStore("HideMainMenu", 			hideMainMenu);
	SetupStore("MsnList",      			StoreMsn());
	SetupStore("Fonbooks",              StoreFonbooks());
}

cMenuSetupFritzbox::cMenuSetupFritzbox()	
{
	// copy setup to temporary parameters
	msn = (char **) malloc(MAX_MSN_COUNT * sizeof(char *));
	url             	 = strdup(fritzboxConfig.url.c_str());
	password        	 = strdup(fritzboxConfig.password.c_str());
	// the original password is not visible in the setup page
	// every single character of the password is displayed as "*"
	for (unsigned int i = 0; i < fritzboxConfig.password.length(); i++) {
		password[i] = '*';
	}
	muteOnCall      	 = fritzboxConfig.muteOnCall;
	pauseOnCall      	 = fritzboxConfig.pauseOnCall;
	showNumber      	 = fritzboxConfig.showNumber;
	showNumberInCallList = fritzboxConfig.showNumberInCallList;
	showDaySeparator     = fritzboxConfig.showDaySeparator;
	hideMainMenu    	 = fritzboxConfig.hideMainMenu;
	msnCount        	 = fritzboxConfig.msn.size();
	msnCountBefore  	 = msnCount; // needed for menu refresh
	msnFilter       	 = fritzboxConfig.msn.empty() ? 0 : 1;
	msnFilterBefore 	 = msnFilter;
	selectedFonbookIDs   = fritzboxConfig.selectedFonbookIDs;
	
	size_t p = 0;
	for(std::vector<std::string>::iterator itStr = fritzboxConfig.msn.begin(); itStr < fritzboxConfig.msn.end(); itStr++)
		asprintf(&msn[p++], "%s", itStr->c_str());
	// build up menu entries
	Setup();
}

cMenuSetupFritzbox::~cMenuSetupFritzbox()
{
	// free up malloced space from constructor
	free(url);
	free(password);
	for (int i=0; i<msnCount; i++)
		free(msn[i]);
	free(msn);
}

cMenuSetupFritzboxFonbooks::cMenuSetupFritzboxFonbooks(std::vector<std::string> *selectedFonbookIDs)
:cOsdMenu(tr("Setup phonebooks to use"), 4)
{
	fonbooks = cFonbuchManager::GetFonbuchManager()->GetFonbooks();
	this->selectedFonbookIDs = selectedFonbookIDs;
	// copy setup to temporary parameters
	numberOfSelectedFonbooks = selectedFonbookIDs->size();
	selectedFonbookPos = (int **)  malloc(fonbooks->size() * sizeof(int *));
	fonbookTitles      = (char **) malloc(fonbooks->size() * sizeof(char *));
	for (size_t i=0; i<fonbooks->size(); i++) {			
		asprintf(&fonbookTitles[i], "%s", (*fonbooks)[i]->GetTitle().c_str());
		selectedFonbookPos[i] = (int *) malloc(sizeof(int));
	}
	// build up menu entries
	SetHelp(tr("More"), tr("Less"), NULL, NULL);
	Setup();
}

cMenuSetupFritzboxFonbooks::~cMenuSetupFritzboxFonbooks()
{
	// free up malloced space from constructor
	for (size_t i=0; i<fonbooks->size(); i++) {
		free(fonbookTitles[i]);
		free(selectedFonbookPos[i]);
	}
	free(fonbookTitles);
	free(selectedFonbookPos);
}

void cMenuSetupFritzboxFonbooks::Setup(void) {
	size_t fbCount = fonbooks->size();
	// save current postion
	int current = Current();
	// clear entries, if any
		Clear();
	// build up setup menu
	for (size_t i=0; i<numberOfSelectedFonbooks; i++) {
		char *numberStr;
		asprintf(&numberStr, "%i.", i+1);
		size_t pos = 0;
		if (i < selectedFonbookIDs->size())
			while (pos < fbCount && 
				  (*fonbooks)[pos]->GetTechId().compare((*selectedFonbookIDs)[i]) != 0) 
				pos++;
		*(selectedFonbookPos[i]) = (int) pos;
		Add(new cMenuEditStraItem(numberStr, selectedFonbookPos[i], fbCount, fonbookTitles));
	}
	// restore current position
	SetCurrent(Get(current));
	// refresh display
	Display();
}

eOSState cMenuSetupFritzboxFonbooks::ProcessKey(eKeys Key) {

	eOSState state = cOsdMenu::ProcessKey(Key);

	if (Key != kNone) {
		switch (Key) {
		case kRed:
			if (numberOfSelectedFonbooks < fonbooks->size()) {
				numberOfSelectedFonbooks++;
				Setup();
			}
			state = osContinue;
			break;
		case kGreen:
			if (numberOfSelectedFonbooks > 0) {
				numberOfSelectedFonbooks--;
				Setup();
			}
			state = osContinue;
			break;
		case kOk:
			selectedFonbookIDs->clear();
			for (size_t i=0; i<numberOfSelectedFonbooks; i++) {
				std::string s = (*fonbooks)[*selectedFonbookPos[i]]->GetTechId();	
				selectedFonbookIDs->push_back(s);
			}
			state = osBack;
		default:
			break;
		}
	}
	return state;
}

sFritzboxConfig::sFritzboxConfig() {
	url             		= "fritz.box";
	password        		= "your password";
	muteOnCall      		= 0;
	pauseOnCall      		= 0;
	showNumber      		= 1;
	showNumberInCallList    = 0;
	showDaySeparator        = 1;
	hideMainMenu    		= 0;
	selectedFonbookIDs.push_back("FRITZ");
}

bool sFritzboxConfig::SetupParseMsn(const char *value){
	std::string currentMsn;
	unsigned int pos = 0;
	// walk through the complete value-line
	while (value[pos] != 0) {
		currentMsn.erase(); 
		// stop at each <space> or EOL
		while (value[pos] != ' ' && value[pos] != 0) {
			currentMsn += value[pos];
			pos++;
		}
		msn.push_back(currentMsn);
		// at a <space> we have to advance to the next MSN
		if (value[pos] != 0)
			pos++;
	}
	return true;
}

bool sFritzboxConfig::SetupParseFonbooks(const char *value){
	std::string currentFb;
	unsigned int pos = 0;
	selectedFonbookIDs.clear();
	// walk through the complete value-line
	while (value[pos] != 0) {
		currentFb.erase(); 
		// stop at each <space> or EOL
		while (value[pos] != ' ' && value[pos] != 0) {
			currentFb += value[pos];
			pos++;
		}
		selectedFonbookIDs.push_back(currentFb);
		// at a <space> we have to advance to the next MSN
		if (value[pos] != 0)
			pos++;
	}
	return true;
}

bool sFritzboxConfig::SetupParse(const char *name, const char *value) {
	if      (!strcasecmp(name, "Url"))          		url          		 = value;
	else if (!strcasecmp(name, "Password"))     		password     		 = value;
	else if (!strcasecmp(name, "MuteOnCall"))   		muteOnCall   		 = atoi(value);
	else if (!strcasecmp(name, "PauseOnCall"))   		pauseOnCall   		 = atoi(value);
	else if (!strcasecmp(name, "ShowNumber"))   		showNumber   		 = atoi(value);
	else if (!strcasecmp(name, "ShowNumberInCallList")) showNumberInCallList = atoi(value);
	else if (!strcasecmp(name, "ShowDaySeparator"))     showDaySeparator     = atoi(value);
	else if (!strcasecmp(name, "HideMainMenu")) 		hideMainMenu 		 = atoi(value);
	else if (!strcasecmp(name, "MsnList"))      		return SetupParseMsn(value);
	else if (!strcasecmp(name, "Fonbooks"))      		return SetupParseFonbooks(value);
	else return false;
	return true;
}
