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
#include "setup.h"


cFonbuchManager* cFonbuchManager::me = NULL;

cFonbuchManager::cFonbuchManager()
{
	// create all fonbooks
	fonbooks.push_back(new cFritzFonbuch());
	fonbooks.push_back(new cOertlichesFonbuch());
	// initialize the fonbooks that are used 
	for (size_t i=0; i<fritzboxConfig.selectedFonbookIDs.size(); i++) {
		fonbooks[fritzboxConfig.selectedFonbookIDs[i]]->Initialize();
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

std::string cFonbuchManager::ResolveToName(std::string &number) {
	for (size_t i=0; i<fritzboxConfig.selectedFonbookIDs.size(); i++) {	
		std::string result = fonbooks[fritzboxConfig.selectedFonbookIDs[i]]->ResolveToName(number);
		if (result != number)
			return result;
	}
	return number;
}

cFonbuch *cFonbuchManager::GetActiveFonbook() {
	size_t pos = 0;
	if (fritzboxConfig.selectedFonbookIDs.size() == 0)
		return NULL;
	while (pos < fritzboxConfig.selectedFonbookIDs.size() &&
			fonbooks[fritzboxConfig.selectedFonbookIDs[pos]]->isDisplayable() == false)
		pos++;
	// if no displayable fonbook found -> return the first fonbook
	if (pos == fritzboxConfig.selectedFonbookIDs.size())
		pos = 0;
	return fonbooks[fritzboxConfig.selectedFonbookIDs[pos]];
}

cFonbuchEntry *cFonbuchManager::RetrieveFonbuchEntry(size_t id) {
	return GetActiveFonbook() ? GetActiveFonbook()->RetrieveFonbuchEntry(id) : NULL;
}

bool cFonbuchManager::isDisplayable() {
	return GetActiveFonbook() ? GetActiveFonbook()->isDisplayable() : false;
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
