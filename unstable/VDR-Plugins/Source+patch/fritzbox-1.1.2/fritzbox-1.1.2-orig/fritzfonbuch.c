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
#define swap vdr_swap
 #include <vdr/tools.h>
#undef swap
#include <vdr/config.h>
#include "fritzfonbuch.h"
#include "fritztools.h"
#include "tcpclient.h"
#include "setup.h"

const char *Entities[97][2] = {
	{"&nbsp;",  " "},
	{"&iexcl;", "¡"},
	{"&cent;",  "¢"},
	{"&pound;", "£"},
	{"&curren;","¤"},
	{"&yen;",   "¥"},
	{"&brvbar;","¦"},
	{"&sect;",  "§"},
	{"&uml;",   "¨"},
	{"&copy;",  "©"},
	{"&ordf;",  "ª"},
	{"&laquo;", "«"},
	{"&not;",   "¬"},
	{"&shy;",   "­"},
	{"&reg;",   "®"},
	{"&macr;",  "¯"},
	{"&deg;",   "°"},
	{"&plusmn;","±"},
	{"&sup2;",  "²"},
	{"&sup3;",  "³"},
	{"&acute;", "´"},
	{"&micro;", "µ"},
	{"&para;",  "¶"},
	{"&middot;","·"},
	{"&cedil;", "¸"},
	{"&sup1;",  "¹"},
	{"&ordm;",  "º"},
	{"&raquo;", "»"},
	{"&frac14;","¼"},
	{"&frac12;","½"},
	{"&frac34;","¾"},
	{"&iquest;","¿"},
	{"&Agrave;","À"},
	{"&Aacute;","Á"},
	{"&Acirc;", "Â"},
	{"&Atilde;","Ã"},
	{"&Auml;",  "Ä"},
	{"&Aring;", "Å"},
	{"&AElig;", "Æ"},
	{"&Ccedil;","Ç"},
	{"&Egrave;","È"},
	{"&Eacute;","É"},
	{"&Ecirc;", "Ê"},
	{"&Euml;",  "Ë"},
	{"&Igrave;","Ì"},
	{"&Iacute;","Í"},
	{"&Icirc;", "Î"},
	{"&Iuml;",  "Ï"},
	{"&ETH;",   "Ð"},
	{"&Ntilde;","Ñ"},
	{"&Ograve;","Ò"},
	{"&Oacute;","Ó"},
	{"&Ocirc;", "Ô"},
	{"&Otilde;","Õ"},
	{"&Ouml;",  "Ö"},
	{"&times;", "×"},
	{"&Oslash;","Ø"},
	{"&Ugrave;","Ù"},
	{"&Uacute;","Ú"},
	{"&Ucirc;", "Û"},
	{"&Uuml;",  "Ü"},
	{"&Yacute;","Ý"},
	{"&THORN;", "Þ"},
	{"&szlig;", "ß"},
	{"&agrave;","à"},
	{"&aacute;","á"},
	{"&acirc;", "â"},
	{"&atilde;","ã"},
	{"&auml;",  "ä"},
	{"&aring;", "å"},
	{"&aelig;", "æ"},
	{"&ccedil;","ç"},
	{"&egrave;","è"},
	{"&eacute;","é"},
	{"&ecirc;", "ê"},
	{"&euml;",  "ë"},
	{"&igrave;","ì"},
	{"&iacute;","í"},
	{"&icirc;", "î"},
	{"&iuml;",  "ï"},
	{"&eth;",   "ð"},
	{"&ntilde;","ñ"},
	{"&ograve;","ò"},
	{"&oacute;","ó"},
	{"&ocirc;", "ô"},
	{"&otilde;","õ"},
	{"&ouml;",  "ö"},
	{"&divide;","÷"},
	{"&oslash;","ø"},
	{"&ugrave;","ù"},
	{"&uacute;","ú"},
	{"&ucirc;", "û"},
	{"&uuml;",  "ü"},
	{"&yacute;","ý"},
	{"&thorn;", "þ"},
	{"&yuml;",  "ÿ"},
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
		// convert the string from latin15 to current system character table
		cCharSetConv *conv = new cCharSetConv("ISO-8859-15", cCharSetConv::SystemCharacterTable());
		const char *s_converted = conv->Convert(s.c_str());
		s = s_converted;
		delete (conv);		
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
	setInitialized(false);
}

cFritzFonbuch::~cFritzFonbuch() {
}

bool cFritzFonbuch::Initialize() {
	return Start();
}

void cFritzFonbuch::Action() {
	unsigned int retry_delay = RETRY_DELAY / 2;
	std::string msg;
	cFritzTools::GetFritzBoxMutex()->Lock();
	do {
		try {
			retry_delay = retry_delay > 1800 ? 3600 : retry_delay * 2;
			setInitialized(false);
			fonbuchList.clear();
			
			cFritzTools::Login();
			dsyslog("fritzfonbuch.c: sending fonbuch request.");
			cHttpClient tc(fritzboxConfig.url, PORT_WWW);
			tc  <<	"GET /cgi-bin/webcm?getpage=../html/"
				<<  cFritzTools::GetLang()
				<<	"/menus/menu2.html"
				<<	"&var:lang="
				<<  cFritzTools::GetLang()
				<<  "&var:pagename=fonbuch&var:menu=fon HTTP/1.1\n\n\0";
			tc >> msg;
			
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
			isyslog("fritzfonbuch.c: read %i entries.", count);
			setInitialized(true);
		} catch (cTcpException te) {
			//cFritzTools::getFritzBoxMutex()->Unlock();
			esyslog("fritzfonbuch.c: cTcpException - %s", te.what());
			esyslog("fritzfonbuch.c: waiting %u seconds before retrying", retry_delay);
			sleep(retry_delay); // delay a possible retry
		} catch (cToolsException te) {
			//cFritzTools::getFritzBoxMutex()->Unlock();
			esyslog("fritzfonbuch.c: cToolsException - %s", te.what());
			esyslog("fritzfonbuch.c: waiting %u seconds before retrying", retry_delay);
			sleep(retry_delay); // delay a possible retry
		}
	} while (!isInitialized());
	cFritzTools::GetFritzBoxMutex()->Unlock();
	std::sort(fonbuchList.begin(), fonbuchList.end());
}

void cFritzFonbuch::Reload() {
	this->Start();
}
