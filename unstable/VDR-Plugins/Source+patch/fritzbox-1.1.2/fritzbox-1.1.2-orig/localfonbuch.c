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

#include <algorithm>
#define swap vdr_swap
 #include <vdr/tools.h>
#undef swap
#include "fritztools.h"
#include "localfonbuch.h"
#include "setup.h"


cLocalFonbuch::cLocalFonbuch() {
	title = tr("Local phone book"); 
	techId = "LOCL";
	displayable = true;	
}

cLocalFonbuch::~cLocalFonbuch() {
}

bool cLocalFonbuch::Initialize() {
	setInitialized(false);
	fonbuchList.clear();
	char* fileName;
	asprintf(&fileName, "%s/localfonbuch.csv", fritzboxConfig.configDir.c_str());
	if (fileName && access(fileName, F_OK) == 0) {
		isyslog("loading %s", fileName);
		FILE *f = fopen(fileName, "r");
		if (f) {
			char *s;
			cReadLine ReadLine;
			while ((s = ReadLine.Read(f)) != NULL) {
				if (s[0] == '#') continue;
				char* name_buffer 	= strtok(s, ",;");
				char* type_buffer 	= strtok(NULL, ",;");
				char* number_buffer = strtok(NULL, ",;");
				if (name_buffer && type_buffer && number_buffer) {
					std::string name   		 	= name_buffer; 
					cFonbuchEntry::eType type   = (cFonbuchEntry::eType) atoi(type_buffer);
					std::string number 			= number_buffer;
					cFonbuchEntry fe(name, number, type);
					fonbuchList.push_back(fe);
				}
				else {
					esyslog("localfonbuch.c: parse error at %s", s);
				}
			}
			setInitialized(true);
			isyslog("localfonbuch.c: read %i entries.", fonbuchList.size());
			std::sort(fonbuchList.begin(), fonbuchList.end());
			return true;
		}
	}
	return false;
}

void cLocalFonbuch::Reload() {
	Initialize();
}


