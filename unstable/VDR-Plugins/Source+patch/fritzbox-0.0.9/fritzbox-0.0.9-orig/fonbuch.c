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

#include "fonbuch.h"

cFonbuchEntry::cFonbuchEntry(std::string &name, std::string &number, eType type) {
	this->name   = name;
	this->number = number;
	this->type   = type;
}

cFonbuch::~cFonbuch()
{
	title       = "Phonebook";
	techId      = "BASE";
	displayable = false;
}

bool cFonbuchEntry::operator<(const cFonbuchEntry &fe) const {
	int cresult = this->name.compare(fe.name);
	if (cresult == 0) 
		return (this->type < fe.type);
	return (cresult < 0);	
}

std::string cFonbuchEntry::getTypeName() {
	switch (getType()) {
	case cFonbuchEntry::TYPE_HOME:
		//TRANSLATORS: telephonebook number type: this is a one char abbreviation for "home"
		return tr("H");
		break;
	case cFonbuchEntry::TYPE_MOBILE:
		//TRANSLATORS: telephonebook number type: this is a one char abbreviation for "mobile"
		return tr("M");
		break;
	case cFonbuchEntry::TYPE_WORK:
		//TRANSLATORS: telephonebook number type: this is a one char abbreviation for "work"
		return tr("W");
		break;
	default:
		return "";
	}

}
