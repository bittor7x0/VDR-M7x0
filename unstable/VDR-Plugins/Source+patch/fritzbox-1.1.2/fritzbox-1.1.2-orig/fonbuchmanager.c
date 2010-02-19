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

#include <string>
#include "fonbuchmanager.h"
#include "fritzfonbuch.h"
#include "oertlichesfonbuch.h"
#include "localfonbuch.h"
#include "setup.h"


cFonbuchManager* cFonbuchManager::me = NULL;

cFonbuchManager::cFonbuchManager()
{
	// create all fonbooks
	fonbooks.push_back(new cFritzFonbuch());
	fonbooks.push_back(new cOertlichesFonbuch());
	fonbooks.push_back(new cLocalFonbuch());
	// initialize the fonbooks that are used 
	for (int i=fritzboxConfig.selectedFonbookIDs.size()-1; i>=0; i--) {
		cFonbuch *fb = fonbooks[fritzboxConfig.selectedFonbookIDs[i]];
		if (fb)
			fb->Initialize();
		else
			fritzboxConfig.selectedFonbookIDs.erase(fritzboxConfig.selectedFonbookIDs.begin()+i);
	}
	// track the currently active (=shown) fonbuch
	activeFonbookPos = std::string::npos;
    // set activeFonbookPos to the last displayed fonbook (if this is still valid and displayable)
	size_t pos = 0;
	while (pos < fritzboxConfig.selectedFonbookIDs.size() && 
			fritzboxConfig.selectedFonbookIDs[pos] != fritzboxConfig.activeFonbookID)
		pos++;
	if (pos < fritzboxConfig.selectedFonbookIDs.size()) {
		if (fonbooks[fritzboxConfig.selectedFonbookIDs[pos]]->isDisplayable())
			activeFonbookPos = pos;
	}
}

cFonbuchManager::~cFonbuchManager()
{
	for (size_t i= 0; i < fonbooks.size(); i++) {
		delete(fonbooks[i]);
	}
}

cFonbuch* cFonbuchManager::GetFonbuch() {
	if (!me)
		me = new cFonbuchManager();
	return (cFonbuch*) me;
}

cFonbuchManager* cFonbuchManager::GetFonbuchManager() {
	if (!me)
		me = new cFonbuchManager();
	return me;
}

void cFonbuchManager::NextFonbuch() {
	size_t pos = activeFonbookPos + 1;
    // no phonebooks -> no switching
	if (fritzboxConfig.selectedFonbookIDs.size() == 0)
		return;
	while (pos < fritzboxConfig.selectedFonbookIDs.size() &&
			fonbooks[fritzboxConfig.selectedFonbookIDs[pos]]->isDisplayable() == false)
		pos++;
	// if no displayable fonbook found -> start from beginning
	if (pos == fritzboxConfig.selectedFonbookIDs.size()) {
		pos = 0;
		while (pos < fritzboxConfig.selectedFonbookIDs.size() &&
				fonbooks[fritzboxConfig.selectedFonbookIDs[pos]]->isDisplayable() == false)
			pos++;
		// if this fails, too, just return 0
		if (pos == fritzboxConfig.selectedFonbookIDs.size()) {
			pos = 0;
		}
	}
	activeFonbookPos = pos;
	// save the tech-id of the active fonbook in setup
	fritzboxConfig.activeFonbookID = fritzboxConfig.selectedFonbookIDs[pos];
}

std::string cFonbuchManager::ResolveToName(std::string &number) {
	for (size_t i=0; i<fritzboxConfig.selectedFonbookIDs.size(); i++) {	
		std::string result = fonbooks[fritzboxConfig.selectedFonbookIDs[i]]->ResolveToName(number);
		if (result != number)
			return result;
	}
	return number;
}

cFonbuch *cFonbuchManager::GetActiveFonbook() {
	if (activeFonbookPos == std::string::npos) {
		NextFonbuch();
	}
	return fonbooks[fritzboxConfig.selectedFonbookIDs[activeFonbookPos]];
}

cFonbuchEntry *cFonbuchManager::RetrieveFonbuchEntry(size_t id) {
	return GetActiveFonbook() ? GetActiveFonbook()->RetrieveFonbuchEntry(id) : NULL;
}

bool cFonbuchManager::isDisplayable() {
	return GetActiveFonbook() ? GetActiveFonbook()->isDisplayable() : false;
}

bool cFonbuchManager::isInitialized() {
	return GetActiveFonbook() ? GetActiveFonbook()->isInitialized() : false;		
}

void cFonbuchManager::setInitialized(bool isInitialized) {
	if (GetActiveFonbook())
		GetActiveFonbook()->setInitialized(isInitialized);
}

size_t cFonbuchManager::GetFonbuchSize() {
	return GetActiveFonbook() ? GetActiveFonbook()->GetFonbuchSize() : 0;
}

std::string cFonbuchManager::GetTitle() {	
	return GetActiveFonbook() ? GetActiveFonbook()->GetTitle() : "";
}

void cFonbuchManager::Reload() {
	for (size_t i=0; i<fritzboxConfig.selectedFonbookIDs.size(); i++) {
		fonbooks[fritzboxConfig.selectedFonbookIDs[i]]->Reload();
	}
}

cFonbooks *cFonbuchManager::GetFonbooks() {
	return &fonbooks;
}
