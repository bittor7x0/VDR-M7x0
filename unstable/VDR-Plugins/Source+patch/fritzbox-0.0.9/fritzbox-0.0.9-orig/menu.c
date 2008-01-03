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

#include <sstream>
#include <vector>
#include <vdr/menu.h>
#include "fonbuchmanager.h"
#include "fritztools.h"
#include "menu.h"
#include "setup.h"

cMenuFritzbox::cMenuFritzbox(cCallList *callList )
:cOsdMenu("Fritz!Box", 1) // just dummy values
{
	this->fonbuch = cFonbuchManager::GetFonbuch();
	this->callList = callList;
	DisplayFonbuch();
}               

cMenuFritzbox::~cMenuFritzbox()
{
}

eOSState cMenuFritzbox::ProcessKey (eKeys Key) {
	eOSState state = cOsdMenu::ProcessKey(Key);
	std::string numberToCall = "";
	cCallEntry *ce = NULL;
	cKeyOsdItem* currentKeyItem = (cKeyOsdItem*) this->Get(Current());
	if (state == osUnknown) {
		switch (Key) {
		case kOk:
			switch (currentMode) {
			case FONBUCH:
				numberToCall = fonbuch->RetrieveFonbuchEntry(Current())->getNumber();
				if (numberToCall.empty())
					Skins.Message(mtError, tr("No number to call"));
				else
					if (cFritzTools::InitCall(numberToCall))
						Skins.Message(mtInfo, tr("Pick up your phone now"));
					else
						Skins.Message(mtError, tr("Error while initiating call"));
				state = osContinue;
				break;
			case IN:
				ce = callList->RetrieveEntry(cCallList::INCOMING, currentKeyItem->key);
				state = AddSubMenu(new cMenuCallDetail(ce, currentMode, fonbuch));
				break;
			case OUT:
				ce = callList->RetrieveEntry(cCallList::OUTGOING, currentKeyItem->key);
				state = AddSubMenu(new cMenuCallDetail(ce, currentMode, fonbuch));
				break;
			case MISSED:
				ce = callList->RetrieveEntry(cCallList::MISSED, currentKeyItem->key);
				state = AddSubMenu(new cMenuCallDetail(ce, currentMode, fonbuch));
				break;
			}
			break;
		case kRed:
			if (currentMode == FONBUCH) {
				fonbuch->Reload();
				Skins.QueueMessage(mtInfo, tr("Retrieving phone book"));
				state = osEnd;				
			} else {
				DisplayFonbuch();
				state = osContinue;
			}
			break;
		case kGreen:
		case kYellow:
		case kBlue:
			if (callList->isValid()) {
				DisplayCalls((cCallList::callType)(Key - kRed));
			} else {
				Skins.Message(mtError, tr("Error fetching the call list"));
			}
			state = osContinue;
			break;
		default:
			break;
		}
	}
	return state;
}

void cMenuFritzbox::DisplayFonbuch() {
	unsigned int nameWidth = 0;
	currentMode = FONBUCH;
	SetTitle(fonbuch->GetTitle().c_str()); 
	Clear();
	if (fonbuch->isDisplayable() == false)
		Add(new cOsdItem(tr("This phonebook is not displayable")));
	else {
		for (size_t pos=0; pos < fonbuch->GetFonbuchSize(); pos++) {
			cFonbuchEntry *fe = fonbuch->RetrieveFonbuchEntry(pos);
			if (fe){
				// build the menu entries
				char *line;
				asprintf(&line,"%s\t%s\t%s", fe->getName().c_str(), fe->getTypeName().c_str(), fe->getNumber().c_str());
				if (fe->getName().length() > nameWidth)
					nameWidth = fe->getName().length();
				Add(new cOsdItem(line));
			}
		}
	}
	SetCols(nameWidth+1, 2);
	SetHelp(tr("Reload"), "|<-", "?|<-", "|->");
	Display();
}

void cMenuFritzbox::DisplayCalls(cCallList::callType ct) {
	currentMode = (mode) ct;
	std::string title=tr("Fritz!Box call list");
	Clear();
	title += " (";
	switch(ct) {
	case cCallList::INCOMING:
		title += tr("incoming");
		break;
	case cCallList::MISSED:
		title += tr("missed");
		break;
	case cCallList::OUTGOING:
		title += tr("outgoing");
		break;
	}
	title += ")";
	unsigned int destWidth = 0;
	std::string oldDate;
	for (unsigned int pos=0; pos < callList->GetSize(ct); pos++) {
		cCallEntry *ce = callList->RetrieveEntry(ct, pos);
		// build the menu entries
		
		if (fritzboxConfig.msn.size() != 0) {
			// check if this entry should be displayed (the entries are filtered
			// according to the MSN filter
			bool display = false;
			// if local number does contain a MSN out of the MSN filter, it is displayed...
			for (unsigned int pos=0; pos < fritzboxConfig.msn.size(); pos++) {
				if (ce->localNumber.find(fritzboxConfig.msn[pos]) != std::string::npos ) {
					display = true;
					break;
				}
			}
			// if local number does not contain any of the MSNs in MSN filter, we test
			// if it does contain any number (if POTS is used fritzbox reports "Festnetz"
			// instead of the local number) 
			if ( display == false ) {
				bool containsNumber = false;
				for (unsigned int pos=0; pos < ce->localNumber.size(); pos++) {
					if (ce->localNumber[pos] >= '0' && ce->localNumber[pos] <= '9') {
						containsNumber = true;
						break;
					}
				}
				if (containsNumber == false)
					display = true;
			}
			
			if (!display)
				continue;
		}
		// show remote name, remote number or "unknown"
		std::string sLine = ce->remoteName.length()   > 0 ? ce->remoteName   : 
						    ce->remoteNumber.length() > 0 ? ce->remoteNumber : 
			                tr("unknown");
		// determine destWidth
		if (destWidth < sLine.length())
			destWidth = sLine.length();
		// show local number if enabled		
		if (fritzboxConfig.showNumberInCallList == true) {
			sLine += "\t" + ce->localNumber;
		}
		sLine = ce->time + "\t" + sLine;
		if (fritzboxConfig.showDaySeparator == false) {
			sLine = ce->date + " " + sLine;
		} else {
			if (ce->date.compare(oldDate) != 0) {
				oldDate = ce->date;
				Add(new cKeyOsdItem(ce->date.c_str(), osUnknown, false, pos));
			}
		}
		Add(new cKeyOsdItem(sLine.c_str(), osUnknown, true, pos));	
	}
	// dynamic column layout
	// ugly dirty hack for maybe better column setup, 
	// VDR shouldn't set width in chars when using a proportional font :-(
	destWidth++; 
	if (fritzboxConfig.showDaySeparator == false)
		SetCols(14, destWidth);
	else
		SetCols(6,  destWidth);
	
	SetTitle(title.c_str());
	//TRANSLATORS: this is the short form of "phone book"
	SetHelp(tr("PB"), "|<-", "?|<-", "|->");
	Display();
}

cMenuCallDetail::cMenuCallDetail(cCallEntry *ce, cMenuFritzbox::mode mode, cFonbuch *fonbuch)
:cOsdMenu(tr("Call details"), 15) {
	this->ce = ce;
	this->fonbuch = fonbuch;
	
	if (ce->remoteName.size() == 0 && ce->remoteNumber.size() > 0) {
		ce->remoteName = fonbuch->ResolveToName(ce->remoteNumber);
		if (ce->remoteName.compare(ce->remoteNumber) == 0)
			ce->remoteName.erase();
	}
	
	std::ostringstream text;
	text << tr("Date")          << "\t" << ce->date      << "\n"
	     << tr("Time")          << "\t" << ce->time      << "\n"
	     << tr("Duration")      << "\t"
	     << (mode == cMenuFritzbox::MISSED ? tr("call was not accepted") :
	    	                               ce->duration) << "\n"
	     << tr("Extension")     << "\t" << ce->localName 
	     << (ce->localName.size() > 0 ? " (" : "") 
	     <<                                ce->localNumber 
	     << (ce->localName.size() > 0 ? ")" : "")        << "\n"
	     << (mode == cMenuFritzbox::OUT ? 
	        tr("Callee") : 
	        tr("Caller"))       << "\t" << ce->remoteName
	     << (ce->remoteName.size() > 0 ? "\n\t" : "")
	     << (ce->remoteNumber.size() > 0 ? ce->remoteNumber : 
	                                      tr("unknown")) << "\n";
	SetText(text.str());
	//TRANSLATORS: this is the label for the button to initiate a call
	SetHelp(tr("Call"));
	Display();
}

void cMenuCallDetail::SetText(std::string text) {
	std::string::size_type pos = 0;
	std::string::size_type npos = text.find('\n', pos);
	do {
		Add(new cOsdItem(text.substr(pos, npos-pos).c_str(), osUnknown, false));
		pos = npos +1;
		npos = text.find('\n', pos);
	} while (npos != std::string::npos);
}

eOSState cMenuCallDetail::ProcessKey (eKeys Key) {
	eOSState state = cOsdMenu::ProcessKey(Key);
	if (state == osUnknown) {
		switch (Key) {
		case kRed:
			// initiate a call
			if (ce->remoteNumber.empty()) {
				Skins.Message(mtError, tr("No number to call"));
			} else {
				if (cFritzTools::InitCall(ce->remoteNumber))
					Skins.Message(mtInfo, tr("Pick up your phone now"));
				else
					Skins.Message(mtError, tr("Error while initiating call"));
			}
			state = osContinue;
			break;
		default:
			break;
		}
	}
	return state;
}

cKeyOsdItem::cKeyOsdItem(const char * text, enum eOSState state, bool selectable, unsigned int key) 
	:cOsdItem(text, state, selectable) {
	this->key = key;
}
