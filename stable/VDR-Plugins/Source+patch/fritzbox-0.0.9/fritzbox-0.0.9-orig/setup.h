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

#ifndef SETUP_H_
#define SETUP_H_

#include <string>
#include <vector>
#include <vdr/menuitems.h>
#include "fonbooks.h"

#define RETRY_DELAY 60
#define MAX_MSN_COUNT 22

class cMenuSetupFritzbox : public cMenuSetupPage
{
private:
	char *url;
	char *password;
	int muteOnCall;
	int pauseOnCall;
	int showNumber;
	int hideMainMenu;
	int showNumberInCallList;
	int showDaySeparator;
	const char *StoreMsn(void);
	const char *StoreFonbooks(void);
	int msnFilter;
	int msnFilterBefore;
	int msnCount;
	int msnCountBefore;
	char **msn;
	std::vector <std::string> selectedFonbookIDs;
	void Setup(void);
	eOSState ProcessKey(eKeys Key);
protected:
	virtual void Store(void);
public:
	cMenuSetupFritzbox();
	virtual ~cMenuSetupFritzbox();
};

class cMenuSetupFritzboxFonbooks : public cOsdMenu
{
private:
	cFonbooks *fonbooks;
	int  **selectedFonbookPos;
	char **fonbookTitles;
	size_t numberOfSelectedFonbooks;
	std::vector<std::string> *selectedFonbookIDs;
	void Setup(void);
	eOSState ProcessKey(eKeys Key);
public:
	cMenuSetupFritzboxFonbooks(std::vector<std::string> *selectedFonbookIDs);
	virtual ~cMenuSetupFritzboxFonbooks();
};

struct sFritzboxConfig {
public:
	sFritzboxConfig(void);
	bool SetupParseMsn(const char *value);
	bool SetupParseFonbooks(const char *value);
	bool SetupParse(const char *Name, const char *Value);
	std::string url;
	std::string password;
	int muteOnCall;
	int pauseOnCall;
	int showNumber;
	int showNumberInCallList;
	int showDaySeparator;
	int hideMainMenu;
	std::vector <std::string> msn;
	std::vector <std::string> selectedFonbookIDs;
};

extern sFritzboxConfig fritzboxConfig;

#endif /*SETUP_H_*/
