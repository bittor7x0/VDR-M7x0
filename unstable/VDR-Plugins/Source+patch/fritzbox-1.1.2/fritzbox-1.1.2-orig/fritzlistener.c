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
#include <vdr/device.h>
#include <vdr/remote.h>
#include <vdr/player.h>
#include <vdr/skins.h>
#include "fonbuch.h"
#include "fritzbox.h"
#include "fritzlistener.h"
#include "notifyosd.h"
#include "setup.h"

cFritzListener::cFritzListener(std::string &hostname, int port, cFonbuch *fonbuch, cCallList *callList)
:cTcpClient(hostname, port), cThread("cFritzListener")
{
	this->fonbuch  = fonbuch;
	this->callList = callList;
	callInfo = NULL;
	displayedConnId = -1;
	muted  = false;
	paused = false;

}

cFritzListener::~cFritzListener()		
{
}

void cFritzListener::HandleCall(bool outgoing, int connId, std::string remoteParty, std::string localParty, std::string medium) {

	bool currPlay, currForw;
	int currSpeed;
	cControl *control = cControl::Control();
	if (control) {
	  control->GetReplayMode(currPlay, currForw, currSpeed);
	}
	
	// apply MSN-filter if enabled
	bool notify = fritzboxConfig.msn.size() ? false : true;
	for (std::vector<std::string>::iterator it = fritzboxConfig.msn.begin(); it < fritzboxConfig.msn.end(); it++){
		if (localParty.compare(*it) == 0 )
			notify = true;
	} 
	if (notify) {
		connIdList.push_back(connId);
		if (fritzboxConfig.muteOnCall && !cDevice::PrimaryDevice()->IsMute()) {
			isyslog("fritzlistener.c: %s call, muting.", outgoing ? "outgoing": "incoming");
			cDevice::PrimaryDevice()->ToggleMute();
			muted = true;
		}
		if (fritzboxConfig.pauseOnCall && !paused && control && currPlay) {
			isyslog("fritzlistener.c: %s call, pressing kPause.", outgoing ? "outgoing": "incoming");
			cRemote::Put(kPause);
			paused = true;
		}
		if (medium.find("SIP") != std::string::npos)
			medium.replace(0, 3, "VoIP ");
		if (medium.find("POTS") != std::string::npos)
			medium = tr("POTS");
		if (fritzboxConfig.showNumber) {
			// save the message into "message", MainThreadHook or MainMenuAction will take care of it
			displayedConnId = connId;
#ifdef DO_NOT_SET
			// trigger translation of string coming from the Fritz!Box - do not compile!
			trNOOP("ISDN")
			trNOOP("VoIP")
#endif
			callInfo = new sCallInfo();
			callInfo->isOutgoing   = outgoing;
			callInfo->remoteNumber = remoteParty;
			callInfo->remoteName   = fonbuch->ResolveToName(remoteParty);
			callInfo->localNumber  = localParty;
			callInfo->medium       = medium;
			// trigger notification using own osd
			if (fritzboxConfig.useNotifyOsd && !cNotifyOsd::isOpen()) {
				dsyslog("fritzlistener.c: triggering NotifyOsd");
				cRemote::CallPlugin(fritzboxConfig.pluginName.c_str());
			}
		}
	}
}
	

void cFritzListener::Action() {
	std::string data = "";
	unsigned int retry_delay = RETRY_DELAY / 2;
	while (true) {
		try {
			retry_delay = retry_delay > 1800 ? 3600 : retry_delay * 2;
			while (true) {
				dsyslog("fritzlistener.c: Waiting for a message.");
				//data.erase();
				if (data.length() == 0) {
					*this >> data;
					if (data.length() == 0) {
						// i.e., connection to Fritz!Box closed
						// wait, then retry by setting up a new connection
						throw cTcpException(cTcpException::ERR_INVALID_DATA);
					}
					dsyslog("fritzlistener.c: Got message %s.", data.c_str());
				}
				// parse message
				size_t pos[8];
				memset(pos, -1, 8 * sizeof(size_t));
				pos[0] = 0;
				for (int i=1; i<8; i++) {
					pos[i] = data.find(";", pos[i-1]+1);
					if (pos[i] == std::string::npos)
						break;
				}
				std::string date    = data.substr(pos[0], pos[1]-pos[0]);
				std::string type    = data.substr(pos[1]+1, pos[2]-pos[1]-1);
				int connId 			= atoi(data.substr(pos[2]+1, pos[3]-pos[2]-1).c_str());
				std::string partA   = pos[4] == std::string::npos ? "" : data.substr(pos[3]+1, pos[4]-pos[3]-1);
				std::string partB   = pos[5] == std::string::npos ? "" : data.substr(pos[4]+1, pos[5]-pos[4]-1);
				std::string partC   = pos[6] == std::string::npos ? "" : data.substr(pos[5]+1, pos[6]-pos[5]-1);
				std::string partD   = pos[7] == std::string::npos ? "" : data.substr(pos[6]+1, pos[7]-pos[6]-1);

				bool currPlay, currForw;
				int currSpeed;
				cControl *control = cControl::Control();
				if (control) {
				  control->GetReplayMode(currPlay, currForw, currSpeed);
				}
				
				if (type.compare("CALL") == 0) {
					// partA => box port
					// partB => caller Id
					// partC => called Id
					// partD => medium (POTS, SIP[1-9], ISDN, ...)
					if (fritzboxConfig.reactOnDirection == fritzboxConfig.DIRECTION_ANY || fritzboxConfig.reactOnDirection == fritzboxConfig.DIRECTION_OUT) {
						// an '#' can be appended to outgoing calls by the phone, so delete it
						if (partC[partC.length()-1] == '#')
							partC = partC.substr(0, partC.length()-1);
						// init display, muting or replay pausing
						HandleCall(true, connId, partC, partB, partD);
					}
					
				} else if (type.compare("RING") == 0) {
					// partA => caller Id
					// partB => called Id
					// partC => medium (POTS, SIP[1-9], ISDN, ...)
					if (fritzboxConfig.reactOnDirection == fritzboxConfig.DIRECTION_ANY || fritzboxConfig.reactOnDirection == fritzboxConfig.DIRECTION_IN)
						// init display, muting or replay pausing
						HandleCall(false, connId, partA, partB, partC);
					
				} else if (type.compare("CONNECT") == 0) {
					// partA => box port
					// partB => Id
					
					// stop call notification
					if (displayedConnId == connId) {
						delete(callInfo);
					    callInfo = NULL;
					    displayedConnId = -1;
					}
				} else if (type.compare("DISCONNECT") == 0) {
					// partA => call duration
					
					// stop call notification
					if (displayedConnId == connId) {
						delete(callInfo);
					    callInfo = NULL;
					    displayedConnId = -1;
					}
					// remove current connection from list
					connIdList.remove(connId);
					// unmute, if applicable
					if (connIdList.empty() && muted && cDevice::PrimaryDevice()->IsMute()) {
						isyslog("fritzlistener.c: Finished all incoming calls, unmuting.");
						cDevice::PrimaryDevice()->ToggleMute();
						muted = false;
					}
					// unpause, if applicable
					if (connIdList.empty() && paused && control && currPlay == false) {
						isyslog("fritzlistener.c: Finished all incoming calls, pressing kPlay.");
						cRemote::Put(kPlay); // this is an ugly workaround, but it should work
						cRemote::Put(kPlay);
						paused = false;
					}
					// force reload of callList
					callList->Start();
				} else {
					throw cTcpException(cTcpException::ERR_INVALID_DATA);
				}
				// remove first line in data
				unsigned int nl = data.find('\n', 0);
				if (nl != std::string::npos) {
					data = data.substr(nl+1);
				} else {
					data.erase();
				}
			}
		} catch (cTcpException te) {
			esyslog("fritzlistener.c: Exception - %s", te.what());
			if (te.errcode == cTcpException::ERR_HOST_NOT_REACHABLE || te.errcode == cTcpException::ERR_CONNECTION_REFUSED) {
				esyslog("fritzlistener.c: Make sure to enable the Fritz!Box call monitor by dialing #96*5* once.");
			}
			this->Disconnect();
		}
		esyslog("fritzlistener.c: waiting %u seconds before retrying", retry_delay);
		sleep(retry_delay); // delay the retry
	}
}
