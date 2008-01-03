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

#include "calllist.h"
#include "fritztools.h"
#include "tcpclient.h"
#include "setup.h"

cCallList::cCallList()
:cThread("cCallList")
{
	callListRead = false;
	this->Start();
}

cCallList::~cCallList()
{
}

void cCallList::Action() {
	unsigned int retry_delay = RETRY_DELAY / 2;
	std::string msg;
	do {
		try {
			retry_delay = retry_delay > 1800 ? 3600 : retry_delay * 2;

			cFritzTools::getFritzBoxMutex()->Lock();
			cFritzTools::Login();
			dsyslog("calllist.c: sending callList request.");
			// force an update of the fritz!box csv list
			cHttpClient tc2(fritzboxConfig.url, PORT_WWW);
			tc2 << "GET /cgi-bin/webcm?getpage=../html/de/menus/menu2.html&var:lang=de&var:pagename=foncalls&var:menu=fon HTTP/1.1\n\n\0";
			// retrieve csv list
			cHttpClient tc(fritzboxConfig.url, PORT_WWW);
			tc << "GET /cgi-bin/webcm?getpage=..%2Fhtml%2Fde%2FFRITZ%21Box_Anrufliste.csv HTTP/1.1\n\n\0";
			tc >> msg;
			cFritzTools::getFritzBoxMutex()->Unlock();
			
#if VDRVERSNUM >= 10503
			// convert answer to current SystemCodeSet (we assume, Fritz!Box sends its answer in latin15)
			cCharSetConv *conv = new cCharSetConv("ISO-8859-15", cCharSetConv::SystemCharacterTable());
			const char *msg_converted = conv->Convert(msg.c_str());
			msg = msg_converted;
			delete(conv);
#endif	
			// parse answer
			callListIn.clear();
			callListOut.clear();
			callListMissed.clear();
			size_t pos = 0, p1;
			int count = 0;
			pos = msg.find(";Dauer", pos);
			while ((p1 = msg.find("\n", pos)) != std::string::npos && msg[p1+1] != '\n') {
				pos = p1+1;
				int type          = p1                           +1;
				if (type == '#') // ignore lines starting with '#' (seen in Fritz!Box Labor firmware) 
					continue;
				int dateStart     = msg.find(';', type)          +1;
				int timeStart	  = msg.find(' ', dateStart)     +1;
				int nameStart     = msg.find(';', timeStart)     +1;
				int numberStart   = msg.find(';', nameStart)     +1;
				int lNameStart    = msg.find(';', numberStart)   +1;
				int lNumberStart  = msg.find(';', lNameStart)    +1;
				int durationStart = msg.find(';', lNumberStart)  +1;
				int durationStop  = msg.find("\n", durationStart)-1;
				if (msg[durationStop] == '\r') // fix for new Fritz!Box Firmwares that use "\r\n" on linebreak
					durationStop--;
				cCallEntry ce;
				ce.date           = msg.substr(dateStart,     timeStart     - dateStart     -1);
				ce.time           = msg.substr(timeStart,     nameStart     - timeStart     -1);
				ce.remoteName     = msg.substr(nameStart,     numberStart   - nameStart     -1);
				ce.remoteNumber   = msg.substr(numberStart,   lNameStart    - numberStart   -1);
				ce.localName      = msg.substr(lNameStart,    lNumberStart  - lNameStart    -1);
				ce.localNumber    = msg.substr(lNumberStart,  durationStart - lNumberStart  -1);
				ce.duration       = msg.substr(durationStart, durationStop -  durationStart +1);
//				dsyslog("fritzfonbuch.c: (%s / %s / %s / %s / %s / %s / %s)", ce.date.c_str(), ce.time.c_str(), ce.duration.c_str(), ce.remoteNumber.c_str(), ce.remoteName.c_str(), ce.localName.c_str(), ce.localNumber.c_str());
				switch (atoi(msg.substr(type, 1).c_str())) {
				case INCOMING:
					callListIn.push_back(ce);
					break;
				case OUTGOING:
					callListOut.push_back(ce);
					break;
				case MISSED:
					callListMissed.push_back(ce);
					break;
				default:
					continue;
				}
				count++;
			}
			dsyslog("calllist.c: CallList -> read %i entries.", count);
			callListRead = true;
		} catch (cTcpException te) {
			dsyslog("calllist.c: Exception - %s", te.what());
			dsyslog("calllist.c: waiting %u seconds before retrying", retry_delay);
			sleep(retry_delay); // delay a possible retry
		} catch (cToolsException te) {
			dsyslog("calllist.c: Exception - %s", te.what());
			dsyslog("calllist.c: waiting %u seconds before retrying", retry_delay);
			sleep(retry_delay); // delay a possible retry
		}
	} while (!callListRead);
}

cCallEntry *cCallList::RetrieveEntry(callType type, size_t id) {
	switch (type) {
	case INCOMING:
		return &callListIn[id];
	case OUTGOING:
		return &callListOut[id];
	case MISSED:
		return &callListMissed[id];
	default:
		return NULL;
	}
}

size_t cCallList::GetSize(callType type) {
	switch (type) {
	case INCOMING:
		return callListIn.size();
	case OUTGOING:
		return callListOut.size();
	case MISSED:
		return callListMissed.size();
	default:
		return 0;
	}
}
