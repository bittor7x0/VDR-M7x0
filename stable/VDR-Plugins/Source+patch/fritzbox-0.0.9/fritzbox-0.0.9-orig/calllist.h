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
#include <vector>
#include <vdr/thread.h>

#ifndef CALLLIST_H_
#define CALLLIST_H_

class cCallEntry{
public:
	std::string date;
	std::string time;
	std::string remoteName;
	std::string remoteNumber;
	std::string localName;
	std::string localNumber;
	std::string duration;  
};

class cCallList : public cThread
{
private:
	std::vector<cCallEntry> callListIn;
	std::vector<cCallEntry> callListMissed;
	std::vector<cCallEntry> callListOut;
	bool callListRead;
public:
	enum callType {
		INCOMING = 1,
		MISSED   = 2,
		OUTGOING = 3
	};
	cCallList();
	virtual ~cCallList();
	void Action();
	bool isValid() { return callListRead; }
	cCallEntry *RetrieveEntry(callType type, size_t id);
	size_t GetSize(callType type);
};

#endif /*CALLLIST_H_*/
