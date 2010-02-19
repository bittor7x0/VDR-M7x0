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
#include <time.h>

cCallList::cCallList()
:cThread("cCallList")
{
	lastMissedCall = 0;
	missedCalls = 0;
	callListRead = false;
	this->Start();
}

cCallList::~cCallList()
{
}

void cCallList::Action() {
	unsigned int retry_delay = RETRY_DELAY / 2;
	std::string msg;
	cFritzTools::GetFritzBoxMutex()->Lock();
	do {
		try {
			retry_delay = retry_delay > 1800 ? 3600 : retry_delay * 2;
			// first, preload phone settings from Fritz!Box
			cFritzTools::GetPhoneSettings();
			// now, process call list
			cFritzTools::Login();
			dsyslog("calllist.c: sending callList request.");
			// force an update of the fritz!box csv list and wait until all data is received
			cHttpClient tc2(fritzboxConfig.url, PORT_WWW);
			tc2 << "GET /cgi-bin/webcm?getpage=../html/"
			    <<  cFritzTools::GetLang()
				<< "/menus/menu2.html&var:lang="
				<<  cFritzTools::GetLang()
			    << "&var:pagename=foncalls&var:menu=fon HTTP/1.1\n\n\0";
			tc2 >> msg;
			// get the URL of the CSV-File-Export			
			unsigned int urlPos   = msg.find(".csv");
			unsigned int urlStop  = msg.find('"', urlPos);
			unsigned int urlStart = msg.rfind('"', urlPos) + 1;
			std::string csvUrl    = msg.substr(urlStart, urlStop-urlStart);
			// retrieve csv list
			msg = "";
			cHttpClient tc(fritzboxConfig.url, PORT_WWW);
			tc << "GET /cgi-bin/webcm?getpage="
			   <<  csvUrl
			   << " HTTP/1.1\n\n\0";
			tc >> msg;
			// convert answer to current SystemCodeSet (we assume, Fritz!Box sends its answer in latin15)
			cCharSetConv *conv = new cCharSetConv("ISO-8859-15", cCharSetConv::SystemCharacterTable());
			const char *msg_converted = conv->Convert(msg.c_str());
			msg = msg_converted;
			delete(conv);	
			// parse answer
			callListIn.clear();
			callListOut.clear();
			callListMissed.clear();
			missedCalls = 0;
			lastMissedCall = 0;
			size_t pos = 0;
			// skip HTTP header data
			while (msg[pos] != '\r' && msg[pos] != '\n') {
				pos = msg.find("\n", pos) + 1;
			}
			pos += 2;
			// parse body
			int count = 0;
			while ((pos = msg.find("\n", pos)) != std::string::npos /*&& msg[pos+1] != '\n'*/) {
			    pos++;
				int type          = pos;
				if (msg[type] < '0' || msg[type] > '9') { // ignore lines not starting with a number (headers, comments, etc.) {
				  dsyslog("calllist.c: parser skipped line in calllist");
				  continue;
				}
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
//				dsyslog("calllist.c: (%s / %s / %s / %s / %s / %s / %s)", ce.date.c_str(), ce.time.c_str(), ce.duration.c_str(), ce.remoteNumber.c_str(), ce.remoteName.c_str(), ce.localName.c_str(), ce.localNumber.c_str());
				
				// workaround for AVM debugging entries in CVS list
				if (ce.remoteNumber.compare("1234567") == 0 && ce.date.compare("12.03.2005") == 0)
					continue;
				
				switch (atoi(&msg[type])) {
				case INCOMING:
					callListIn.push_back(ce);
					break;
				case OUTGOING:
					callListOut.push_back(ce);
					break;
				case MISSED:
					callListMissed.push_back(ce);
					//       01234567        01234
					// date: dd.mm.yy, time: hh:mm
					tm tmCallTime;
					tmCallTime.tm_mday = atoi(ce.date.substr(0, 2).c_str());
					tmCallTime.tm_mon  = atoi(ce.date.substr(3, 2).c_str()) - 1;
					tmCallTime.tm_year = atoi(ce.date.substr(6, 2).c_str()) + 100;
					tmCallTime.tm_hour = atoi(ce.time.substr(0, 2).c_str());
					tmCallTime.tm_min  = atoi(ce.time.substr(2, 2).c_str());
					tmCallTime.tm_sec  = 0;
					{
					time_t callTime = mktime(&tmCallTime);
					// track number of new missed calls
					if (callTime > fritzboxConfig.lastKnownMissedCall && ce.MatchesFilter())
						missedCalls++;
					// track newest missed call
					if (callTime > lastMissedCall)
						lastMissedCall = callTime;
					}
					break;
				default:
					dsyslog("calllist.c: parser skipped unknown call type");
					continue;
				}
				count++;
			}
			isyslog("calllist.c: CallList -> read %i entries.", count);
			callListRead = true;
		} catch (cTcpException te) {
			esyslog("calllist.c: Exception - %s", te.what());
			esyslog("calllist.c: waiting %u seconds before retrying", retry_delay);
			sleep(retry_delay); // delay a possible retry
		} catch (cToolsException te) {
			esyslog("calllist.c: Exception - %s", te.what());
			esyslog("calllist.c: waiting %u seconds before retrying", retry_delay);
			sleep(retry_delay); // delay a possible retry
		}
	} while (!callListRead);
	cFritzTools::GetFritzBoxMutex()->Unlock();
}

cCallEntry *cCallList::RetrieveEntry(callType type, size_t id) {
	if (id < 0)
		return NULL;
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

void cCallList::MissedCallsSeen() {
	fritzboxConfig.lastKnownMissedCall = LastMissedCall();
	missedCalls = 0;
}

bool cCallEntry::MatchesFilter() {
	if (fritzboxConfig.msn.size() != 0) {
		// check if this entry should be displayed (the entries are filtered
		// according to the MSN filter)
		bool display = false;
		// if local number does contain a MSN out of the MSN filter, it is displayed...
		for (unsigned int pos=0; pos < fritzboxConfig.msn.size(); pos++) {
			if (localNumber.find(fritzboxConfig.msn[pos]) != std::string::npos ) {
				display = true;
				break;
			}
		}
		// if local number does not contain any of the MSNs in MSN filter, we test
		// if it does contain any number (if POTS is used fritzbox reports "Festnetz"
		// instead of the local number) 
		if ( display == false ) {
			bool containsNumber = false;
			for (unsigned int pos=0; pos < localNumber.size(); pos++) {
				if (localNumber[pos] >= '0' && localNumber[pos] <= '9') {
					containsNumber = true;
					break;
				}
			}
			if (containsNumber == false)
				display = true;
		}
		return display;
	}
	return true;
}
