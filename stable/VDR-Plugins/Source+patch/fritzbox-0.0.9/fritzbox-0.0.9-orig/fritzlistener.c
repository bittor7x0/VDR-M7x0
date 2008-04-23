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

#include <list>
#include <sstream>
#include <vector>
#include <vdr/device.h>
#include <vdr/remote.h>
#include <vdr/player.h>
#include <vdr/skins.h>
#include "fonbuch.h"
#include "fritzlistener.h" 
#include "setup.h"

cFritzListener::cFritzListener(std::string &hostname, int port, cFonbuch *fonbuch, cCallList *callList)
:cTcpClient(hostname, port), cThread("cFritzListener")
{
	this->fonbuch = fonbuch;
	this->callList = callList;
	message = NULL;
}

cFritzListener::~cFritzListener()		
{
}

void cFritzListener::Action() {
	std::string data;
	std::list<int> connIdList;
	int displayedConnId = -1;
	bool muted  = false;
	bool paused = false;
	while (true) {
		try {
			while (true) {
				dsyslog("FritzBox: Waiting for a message.");
				data.erase();	
				*this >> data;
				dsyslog("FritzBox: Got message %s.", data.c_str());

				// parse message
				size_t pos[7];
				memset(pos, -1, 7 * sizeof(size_t));
				pos[0] = 0;
				for (int i=1; i<7; i++) {
					pos[i] = data.find(";", pos[i-1]+1);
					if (pos[i] == std::string::npos)
						break;
				}
				std::string date    = data.substr(pos[0], pos[1]-pos[0]);
				std::string type    = data.substr(pos[1]+1, pos[2]-pos[1]-1);
				int connId = atoi(data.substr(pos[2]+1, pos[3]-pos[2]-1).c_str());
				std::string partA   = pos[4] == std::string::npos ? "" : data.substr(pos[3]+1, pos[4]-pos[3]-1);
				std::string partB   = pos[5] == std::string::npos ? "" : data.substr(pos[4]+1, pos[5]-pos[4]-1);
				std::string partC   = pos[6] == std::string::npos ? "" : data.substr(pos[5]+1, pos[6]-pos[5]-1);

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
				} else if (type.compare("RING") == 0) {
					// partA => caller Id
					// partB => called Id
					// partC => medium (POTS, SIP[1-9], ISDN, ...)
					
					// apply MSN-filter if enabled
					bool notify = fritzboxConfig.msn.size() ? false : true;
					for (std::vector<std::string>::iterator it = fritzboxConfig.msn.begin(); it < fritzboxConfig.msn.end(); it++){
						if (partB.compare(*it) == 0 )
							notify = true;
					} 
					if (notify) {
						connIdList.push_back(connId);
						if (fritzboxConfig.muteOnCall && !cDevice::PrimaryDevice()->IsMute()) {
							dsyslog("FritzBox: Incoming call, muting.");
							cDevice::PrimaryDevice()->ToggleMute();
							muted = true;
						}
						if (fritzboxConfig.pauseOnCall && !paused && control && currPlay) {
							dsyslog("FritzBox: Incoming call, pressing kPause.");
							cRemote::Put(kPause);
							paused = true;
						}
						if (partC.find("SIP") != std::string::npos)
							partC.replace(0, 3, "VoIP ");
						if (partC.find("ISDN") != std::string::npos) {
							partC += " ";
							partC += partB;	
						}
						if (partC.find("POTS") != std::string::npos)
							partC = tr("POTS");
						if (fritzboxConfig.showNumber) {
							// save the message into "message", MainThreadHook will take care of it
							displayedConnId = connId;
							if (partA.size() == 0)
							#ifdef DO_NOT_SET
								trNOOP("ISDN")
								trNOOP("VoIP")
							#endif
								asprintf(&message, "%s [%s]", tr("Call from unknown extension"), tr(partC.c_str()));
							else
								asprintf(&message, "%s %s [%s]", tr("Call from"), (fonbuch->ResolveToName(partA)).c_str(), partC.c_str());	
						}
					}
				} else if (type.compare("CONNECT") == 0) {
					// partA => box port
					// partB => Id
					
					// stop call notification
					if (displayedConnId == connId) {
						free(message);
					    message = NULL;
					    displayedConnId = -1;
					}
				} else if (type.compare("DISCONNECT") == 0) {
					// partA => call duration
					
					// stop call notification
					if (displayedConnId == connId) {
						free(message);
					    message = NULL;
					    displayedConnId = -1;
					}
					// remove current connection from list
					connIdList.remove(connId);
					// unmute, if applicable
					if (connIdList.empty() && muted && cDevice::PrimaryDevice()->IsMute()) {
						dsyslog("FritzBox: Finished all incoming calls, unmuting.");
						cDevice::PrimaryDevice()->ToggleMute();
						muted = false;
					}
					// unpause, if applicable
					if (connIdList.empty() && paused && control && currPlay == false) {
						dsyslog("FritzBox: Finished all incoming calls, pressing kPlay.");
						cRemote::Put(kPlay); // this is an ugly workaround, but it should work
						cRemote::Put(kPlay);
						paused = false;
					}
					// force reload of callList
					callList->Start();
				} else {
					esyslog("FritzBox: Unknown message type: %s.", type.c_str());
				}
			}
		} catch (cTcpException te) {
			dsyslog("fritzlistener.c: Exception - %s", te.what());
		}
		sleep(RETRY_DELAY); // delay the retry
	}
}
