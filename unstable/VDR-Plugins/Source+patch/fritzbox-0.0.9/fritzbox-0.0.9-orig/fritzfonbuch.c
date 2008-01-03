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
#include <sstream>
#include <vdr/tools.h>
#include <vdr/config.h>
#include "fritzfonbuch.h"
#include "fritztools.h"
#include "tcpclient.h"
#include "setup.h"

const char *Entities[97][2] = {
	{"&nbsp;",  " "},
	{"&iexcl;", "�"},
	{"&cent;",  "�"},
	{"&pound;", "�"},
	{"&curren;","�"},
	{"&yen;",   "�"},
	{"&brvbar;","�"},
	{"&sect;",  "�"},
	{"&uml;",   "�"},
	{"&copy;",  "�"},
	{"&ordf;",  "�"},
	{"&laquo;", "�"},
	{"&not;",   "�"},
	{"&shy;",   "�"},
	{"&reg;",   "�"},
	{"&macr;",  "�"},
	{"&deg;",   "�"},
	{"&plusmn;","�"},
	{"&sup2;",  "�"},
	{"&sup3;",  "�"},
	{"&acute;", "�"},
	{"&micro;", "�"},
	{"&para;",  "�"},
	{"&middot;","�"},
	{"&cedil;", "�"},
	{"&sup1;",  "�"},
	{"&ordm;",  "�"},
	{"&raquo;", "�"},
	{"&frac14;","�"},
	{"&frac12;","�"},
	{"&frac34;","�"},
	{"&iquest;","�"},
	{"&Agrave;","�"},
	{"&Aacute;","�"},
	{"&Acirc;", "�"},
	{"&Atilde;","�"},
	{"&Auml;",  "�"},
	{"&Aring;", "�"},
	{"&AElig;", "�"},
	{"&Ccedil;","�"},
	{"&Egrave;","�"},
	{"&Eacute;","�"},
	{"&Ecirc;", "�"},
	{"&Euml;",  "�"},
	{"&Igrave;","�"},
	{"&Iacute;","�"},
	{"&Icirc;", "�"},
	{"&Iuml;",  "�"},
	{"&ETH;",   "�"},
	{"&Ntilde;","�"},
	{"&Ograve;","�"},
	{"&Oacute;","�"},
	{"&Ocirc;", "�"},
	{"&Otilde;","�"},
	{"&Ouml;",  "�"},
	{"&times;", "�"},
	{"&Oslash;","�"},
	{"&Ugrave;","�"},
	{"&Uacute;","�"},
	{"&Ucirc;", "�"},
	{"&Uuml;",  "�"},
	{"&Yacute;","�"},
	{"&THORN;", "�"},
	{"&szlig;", "�"},
	{"&agrave;","�"},
	{"&aacute;","�"},
	{"&acirc;", "�"},
	{"&atilde;","�"},
	{"&auml;",  "�"},
	{"&aring;", "�"},
	{"&aelig;", "�"},
	{"&ccedil;","�"},
	{"&egrave;","�"},
	{"&eacute;","�"},
	{"&ecirc;", "�"},
	{"&euml;",  "�"},
	{"&igrave;","�"},
	{"&iacute;","�"},
	{"&icirc;", "�"},
	{"&iuml;",  "�"},
	{"&eth;",   "�"},
	{"&ntilde;","�"},
	{"&ograve;","�"},
	{"&oacute;","�"},
	{"&ocirc;", "�"},
	{"&otilde;","�"},
	{"&ouml;",  "�"},
	{"&divide;","�"},
	{"&oslash;","�"},
	{"&ugrave;","�"},
	{"&uacute;","�"},
	{"&ucirc;", "�"},
	{"&uuml;",  "�"},
	{"&yacute;","�"},
	{"&thorn;", "�"},
	{"&yuml;",  "�"},
	{"&amp;",   "&"},
};

std::string &convertEntities(std::string &s) {
	if (s.find("&") != std::string::npos) {
		for (int i=0; i<97; i++) {
			std::string::size_type pos = s.find(Entities[i][0]);
			if (pos != std::string::npos) {
				s.replace(pos, strlen(Entities[i][0]), Entities[i][1]);
				i--; //search for the same entity again
			}
		}
#if VDRVERSNUM >= 10503
		// convert the string from latin15 to current system character table
		cCharSetConv *conv = new cCharSetConv("ISO-8859-15", cCharSetConv::SystemCharacterTable());
		const char *s_converted = conv->Convert(s.c_str());
		s = s_converted;
		delete (conv);
#endif		
	}	
	return s;
}

cFritzFonbuch::cFritzFonbuch() 
:cThread("cFritzFonbuch")
//cFonbuch()
{
	title = tr("Fritz!Box phone book");
	techId = "FRITZ";
	displayable = true;
	fonbuchRead = false;
}

cFritzFonbuch::~cFritzFonbuch() {
}

bool cFritzFonbuch::Initialize() {
	return Start();
}

void cFritzFonbuch::Action() {
	unsigned int retry_delay = RETRY_DELAY / 2;
	std::string msg;
	do {
		try {
			retry_delay = retry_delay > 1800 ? 3600 : retry_delay * 2;
			fonbuchList.clear();
			
			cFritzTools::getFritzBoxMutex()->Lock();
			cFritzTools::Login();
			dsyslog("fritzfonbuch.c: sending fonbuch request.");
			cHttpClient tc(fritzboxConfig.url, PORT_WWW);
			tc  <<	"GET /cgi-bin/webcm?getpage=../html/de/menus/menu2.html"
					"&var:lang=de&var:pagename=fonbuch&var:menu=fon HTTP/1.1\n\n\0";

			tc >> msg;
			cFritzTools::getFritzBoxMutex()->Unlock();
			
			// parse answer
			size_t pos = 0, p1, p2;
			int count = 0;
			// parser for old format
			const std::string tag("(TrFon(");
			while ((p1 = msg.find(tag, pos)) != std::string::npos) {
				p1 += 7; // points to the first "
				int nameStart     = msg.find(',', p1)          +3;
				int nameStop      = msg.find('"', nameStart)   -1;
				int numberStart   = msg.find(',', nameStop)    +3;
				int numberStop    = msg.find('"', numberStart) -1;
				if (msg[nameStart] == '!') // skip '!' char, older firmware versions use to mark VIPs
					nameStart++;
				std::string namePart = msg.substr(nameStart, nameStop - nameStart+1);
				std::string namePart2 = convertEntities(namePart);
				std::string numberPart = msg.substr(numberStart, numberStop - numberStart+1);
				if (namePart2.length() && numberPart.length()) {
					cFonbuchEntry fe(namePart2, numberPart, cFonbuchEntry::TYPE_NONE);
					fonbuchList.push_back(fe);
					//dsyslog("fritzfonbuch.c: (%s / %s)", fe.number.c_str(), fe.name.c_str());
				}
				pos = p1+10;
				count++;
			}
			// parser for new format
			pos = 0;
			const std::string tag2("TrFonName(");
			const std::string tag3("TrFonNr("	);
			while ((p2 = msg.find(tag3, pos)) != std::string::npos) {
				int typeStart     = p2 + 9;
				int numberStart   = msg.find(',', p2)    +3;
				int typeStop      = numberStart - 5;
				int numberStop    = msg.find('"', numberStart) -1;
				p1 = msg.rfind(tag2, p2);
				p1 += 7; // points to the first "
				int nameStart     = msg.find(',', p1)          +3;
				int nameStop      = msg.find('"', nameStart)   -1;
				std::string namePart   = msg.substr(nameStart, nameStop - nameStart+1);
				std::string namePart2  = convertEntities(namePart);
				std::string numberPart = msg.substr(numberStart, numberStop - numberStart+1);
				
				std::string typePart   = msg.substr(typeStart, typeStop - typeStart+1);
				cFonbuchEntry::eType type = cFonbuchEntry::TYPE_NONE;
				if      (typePart.compare("home") == 0)
					type = cFonbuchEntry::TYPE_HOME;
				else if (typePart.compare("mobile") == 0)
					type = cFonbuchEntry::TYPE_MOBILE;
				else if (typePart.compare("work") == 0)
					type = cFonbuchEntry::TYPE_WORK;
				
				if (namePart2.length() && numberPart.length()) {
					cFonbuchEntry fe(namePart2, numberPart, type);
					fonbuchList.push_back(fe);
					//dsyslog("fritzfonbuch.c: (%s / %s / %i)", fe.number.c_str(), fe.name.c_str(), fe.type);
				}
				pos = p2+10;
				count++;
			}
			dsyslog("fritzfonbuch.c: read %i entries.", count);
			fonbuchRead = true;
		} catch (cTcpException te) {
			dsyslog("fritzfonbuch.c: Exception - %s", te.what());
			dsyslog("calllist.c: waiting %u seconds before retrying", retry_delay);
			sleep(retry_delay); // delay a possible retry
		} catch (cToolsException te) {
			dsyslog("fritzfonbuch.c: Exception - %s", te.what());
			dsyslog("calllist.c: waiting %u seconds before retrying", retry_delay);
			sleep(retry_delay); // delay a possible retry
		}
	} while (!fonbuchRead);
	std::sort(fonbuchList.begin(), fonbuchList.end());
}

std::string cFritzFonbuch::ResolveToName(std::string &number) {
	for (unsigned int pos=0; pos < fonbuchList.size(); pos++) {
		//cFonbuchEntry fe = fonbuchList[pos];
		if (number.compare(fonbuchList[pos].getNumber()) == 0)
			return fonbuchList[pos].getName()+" "+fonbuchList[pos].getTypeName();
	}
	return number;
}

cFonbuchEntry *cFritzFonbuch::RetrieveFonbuchEntry(size_t id) {
	if (id >= GetFonbuchSize())
		return NULL;
	return &fonbuchList[id];
}

size_t cFritzFonbuch::GetFonbuchSize() {
	return fonbuchList.size();
}
