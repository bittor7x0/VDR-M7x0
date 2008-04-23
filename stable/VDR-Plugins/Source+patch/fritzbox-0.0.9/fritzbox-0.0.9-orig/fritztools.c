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
#include "fritztools.h"
#include "setup.h"
#include "tcpclient.h"

cMutex* cFritzTools::fritzBoxMutex = new cMutex();

cFritzTools::cFritzTools()
{
}

cFritzTools::~cFritzTools()
{
}

void cFritzTools::Login() {
	std::ostringstream sMsg;

	dsyslog("fritztools.c: sending login request to fritz.box.");
	cHttpClient *tc = new cHttpClient(fritzboxConfig.url, PORT_WWW);
	*tc  <<		"POST /cgi-bin/webcm HTTP/1.1\n"
	     <<  	"Content-Type: application/x-www-form-urlencoded\n"
	     <<  	"Content-Length: "
	     <<  	23 + UrlEncode(fritzboxConfig.password).size()
	     <<  	"\n\nlogin:command/password="
	     <<  	UrlEncode(fritzboxConfig.password)
	     <<  	"\n";
	do {
		*tc >> sMsg;
		usleep(100000);
	} while (sMsg.str().find(std::string("</html>")) == std::string::npos);
	delete tc;
	if (sMsg.str().find("FEHLER") != std::string::npos) { // is the Fritz!Box-Webinterface always in German?
		dsyslog("fritztools.c: login failed.");
		throw cToolsException(cToolsException::ERR_LOGIN_FAILED);
	}
	dsyslog("fritztools.c: login successful.");
}

std::string cFritzTools::UrlEncode(std::string &s_input) {
	std::string result;
	std::string s;
	std::string hex = "0123456789abcdef";
#if VDRVERSNUM >= 10503
	cCharSetConv *conv = new cCharSetConv(cCharSetConv::SystemCharacterTable(), "ISO-8859-15");
	s = conv->Convert(s_input.c_str());
	delete(conv);
#else
	s = s_input;
#endif	
	for (unsigned int i=0; i<s.length(); i++) {
		if( 'a' <= s[i] && s[i] <= 'z'
			|| 'A' <= s[i] && s[i] <= 'Z'
			|| '0' <= s[i] && s[i] <= '9' ) {
			result += s[i];
		} else {
			result += '%';
			result += hex[(unsigned char) s[i] >> 4];
			result += hex[(unsigned char) s[i] & 0x0f];
		}
	}
	return result;
}

bool cFritzTools::InitCall(std::string &number) {
	std::string msg;
	try {
		Login();
		dsyslog("fritztools.c: sending fonbuch request.");
		cHttpClient tc(fritzboxConfig.url, PORT_WWW);
		tc  <<	"POST /cgi-bin/webcm HTTP/1.1\n"
			<<	"Content-Type: application/x-www-form-urlencoded\n"
			<<	"Content-Length: "
			<<	107 + number.length()
			<<	"\n\n"
			<<	"getpage=..%2Fhtml%2Fde%2Fmenus%2Fmenu2.html&var%3Apagename=fonbuch&var%3Amenu=home&telcfg%3Acommand%2FDial="
			<<	number
			<<	"\n";
		do {
			tc >> msg;
			usleep(100000);
		} while (msg.find(std::string("</html>")) == std::string::npos);

	} catch (cTcpException te) {
		dsyslog("fritztools.c: Exception - %s", te.what());
		return false;
	}
	return true;	
}

