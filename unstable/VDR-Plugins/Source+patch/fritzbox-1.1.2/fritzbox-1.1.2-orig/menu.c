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
#include <vdr/status.h>
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
	cCallEntry *ce = NULL;
	cKeyOsdItem* currentKeyItem = (cKeyOsdItem*) this->Get(Current());
	if (state == osUnknown) {
		switch (Key) {
		case kOk:
			switch (currentMode) {
			case FONBUCH:
				if (Current() >= 0 && fonbuch->isDisplayable() && fonbuch->isInitialized())
					state = AddSubMenu(new cMenuFonbuchDetail(fonbuch->RetrieveFonbuchEntry(Current()), fonbuch));
				break;
			case IN:
				if (currentKeyItem)
					ce = callList->RetrieveEntry(cCallList::INCOMING, currentKeyItem->key);
				if (ce)
					state = AddSubMenu(new cMenuCallDetail(ce, currentMode, fonbuch));
				break;
			case OUT:				
				if (currentKeyItem)
					ce = callList->RetrieveEntry(cCallList::OUTGOING, currentKeyItem->key);
				if (ce)
					state = AddSubMenu(new cMenuCallDetail(ce, currentMode, fonbuch));
				break;
			case MISSED:
				if (currentKeyItem)
					ce = callList->RetrieveEntry(cCallList::MISSED, currentKeyItem->key);
				if (ce)
					state = AddSubMenu(new cMenuCallDetail(ce, currentMode, fonbuch));
				break;
			}
			break;
		case kRed:
			if (currentMode == FONBUCH) {
				cFonbuchManager::GetFonbuchManager()->NextFonbuch();
			}
			DisplayFonbuch();
			state = osContinue;
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
	if (fonbuch->isInitialized() == false) {
		Add(new cOsdItem(tr("This phonebook is not yet available."), osUnknown, false));
		Add(new cOsdItem(tr("You may need to wait some minutes,"), osUnknown, false));
		Add(new cOsdItem(tr("otherwise there may be a network problem."), osUnknown, false));
	}
	else if (fonbuch->isDisplayable() == false) {
		Add(new cOsdItem(tr("This phonebook is not displayable"), osUnknown, false));
	}
	else {
		std::string lastName;
		for (size_t pos=0; pos < fonbuch->GetFonbuchSize(); pos++) {
			cFonbuchEntry *fe = fonbuch->RetrieveFonbuchEntry(pos);
			if (fe) {
				// build the menu entries
				char *line;
				asprintf(&line,"%s\t%s\t%s", lastName == fe->getName() ? "" : fe->getName().c_str(), fe->getTypeName().c_str(), fe->getNumber().c_str());
				if (fe->getName().length() > nameWidth)
					nameWidth = fe->getName().length();
				Add(new cOsdItem(line));
				lastName = fe->getName();
			}
		}
	}
	SetCols(nameWidth+1, 2);
	SetHelp(tr("> PB"), "|<-", "?|<-", "|->");
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
		callList->MissedCallsSeen();
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
		
		if ( !ce->MatchesFilter())
			continue;

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

// cMenuCallDetail ************************************************************

cMenuCallDetail::cMenuCallDetail(cCallEntry *ce, cMenuFritzbox::mode mode, cFonbuch *fonbuch)
:cOsdMenu(tr("Call details"), 15)
{
	this->ce = ce;
	
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

	//TRANSLATORS: this is the label for the button to initiate a call
	SetHelp(tr("Button$Call"));
	SetText(text.str());
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

// cMenuFonbuchDetail *********************************************************

cMenuFonbuchDetail::cMenuFonbuchDetail(cFonbuchEntry *fe, cFonbuch *fonbuch)
:cOsdMenu(tr("Phone book details"), 15)
{
//  search for other entries with same name but different type	
	for(size_t i = 0; i < fonbuch->GetFonbuchSize(); i++) {
		cFonbuchEntry  *fe2 = fonbuch->RetrieveFonbuchEntry(i);
		if (fe2->getName() == fe->getName())
			numbers[fe2->getType()] = fe2->getNumber();
	}
	
	std::ostringstream sText;
	// if a number of TYPE_NONE is given, a simple version of the details screen is shown
	// this type is set, e.g., with old Fritz!Boxes
	if (numbers[cFonbuchEntry::TYPE_NONE].length() > 0) {
	  sText << tr("Name")          << "\t" << fe->getName()      				   << "\n"
	        << tr("Numbers")       << "\t\n"
		    << tr("Default")       << "\t" << numbers[cFonbuchEntry::TYPE_NONE]    << "\n";
	  numbers[cFonbuchEntry::TYPE_HOME] = numbers[cFonbuchEntry::TYPE_NONE];
		
	} else {
	  sText << tr("Name")          << "\t" << fe->getName()      				   << "\n"
	        << tr("Numbers")       << "\t\n"
		    << tr("Private")       << "\t" << numbers[cFonbuchEntry::TYPE_HOME]    << "\n"
		    << tr("Mobile")        << "\t" << numbers[cFonbuchEntry::TYPE_MOBILE]  << "\n"
		    << tr("Business")      << "\t" << numbers[cFonbuchEntry::TYPE_WORK]    << "\n";
	}
	std::string text = sText.str();
	std::string::size_type pos = 0;
	std::string::size_type npos = text.find('\n', pos);
	size_t line = 0;
	do {
		Add(new cOsdItem(text.substr(pos, npos-pos).c_str(), osUnknown, line < 2 ? false : true));
		pos = npos +1;
		npos = text.find('\n', pos);
		line++;
	} while (npos != std::string::npos);
	if (numbers[cFonbuchEntry::TYPE_NONE].length() > 0)
		SetCurrent(Get(2));
    else
	  SetCurrent(Get(1 + fe->getType()));
	//TRANSLATORS: this is the label for the button to initiate a call
	SetHelp(tr("Button$Call"));
	Display();
}

eOSState cMenuFonbuchDetail::ProcessKey (eKeys Key) {
	eOSState state = cOsdMenu::ProcessKey(Key);
	if (state == osUnknown) {
		switch (Key) {
		case kRed:
			// initiate a call
			if (numbers[Current() - 1].empty()) {
				Skins.Message(mtError, tr("No number to call"));
			} else {
				if (cFritzTools::InitCall(numbers[Current() - 1]))
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

// cKeyOsdItem ****************************************************************

cKeyOsdItem::cKeyOsdItem(const char * text, enum eOSState state, bool selectable, unsigned int key) 
	:cOsdItem(text, state, selectable) {
	this->key = key;
}
