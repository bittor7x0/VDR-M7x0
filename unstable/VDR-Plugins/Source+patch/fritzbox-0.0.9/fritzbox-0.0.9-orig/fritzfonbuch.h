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

#ifndef FRITZFONBUCH_H_
#define FRITZFONBUCH_H_

#include <string>
#include <vector>
#include <vdr/thread.h>
#include "fonbuch.h"

class cFritzFonbuch : public cThread, public cFonbuch {
			friend class cFonbuchManager;
private:
	std::vector<cFonbuchEntry> fonbuchList;
	bool fonbuchRead;
	cFritzFonbuch();
public:
	~cFritzFonbuch();
	bool Initialize();
	void Action();
	std::string ResolveToName(std::string &number);
	cFonbuchEntry *RetrieveFonbuchEntry(size_t id);
	size_t GetFonbuchSize();
	void Reload() { this->Start(); }
};

#endif /*FRITZFONBUCH_H_*/
