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

#ifndef FRITZLISTENER_H_
#define FRITZLISTENER_H_

#include <list>
#include <string>
#include <vdr/thread.h>
#include "calllist.h"
#include "fonbuch.h"
#include "tcpclient.h"

class sCallInfo{
public:
	bool isOutgoing;
	std::string remoteNumber;
	std::string remoteName;
	std::string localNumber;
	std::string medium;
};

class cFritzListener : public cTcpClient, public cThread
{
private:
	cFonbuch *fonbuch;
	cCallList *callList;
	sCallInfo *callInfo;
	std::list<int> connIdList;
	int displayedConnId;
	bool muted;
	bool paused;

	void HandleCall(bool outgoing, int connId, std::string remoteParty, std::string localParty, std::string medium);
	
public:
	
	cFritzListener(std::string &hostname, int port, cFonbuch *fonbuch, cCallList *callList);
	virtual ~cFritzListener();
	virtual void Action();
	sCallInfo *GetCallInfo() { return callInfo; }
};

#endif /*FRITZLISTENER_H_*/
