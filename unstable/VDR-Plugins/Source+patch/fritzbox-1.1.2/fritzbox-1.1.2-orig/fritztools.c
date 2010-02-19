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

std::string cFritzTools::GetLang() {
	if (fritzboxConfig.lang.size() == 0) {
		std::ostringstream sMsg;

		dsyslog("fritztools.c: detecting fritz.box interface language...");
		cHttpClient tc(fritzboxConfig.url, PORT_WWW);
		tc << "GET /cgi-bin/webcm?getpage=../html/index_inhalt.html HTTP/1.1\n\n\0";
		tc >> sMsg;

		// determine language of webinterface
		unsigned int p = sMsg.str().find("name=\"var:lang\"");
		if (p != std::string::npos) {
			p += 16; // skip lang-tag
			unsigned int langStart = sMsg.str().find('"', p);
			unsigned int langStop  = sMsg.str().find('"', ++langStart);
			if (langStart != std::string::npos && langStop != std::string::npos) {
				fritzboxConfig.lang    = sMsg.str().substr(langStart, langStop - langStart);
				dsyslog("fritztools.c: interface language is '%s'", fritzboxConfig.lang.c_str());
			}
		} 
		if (fritzboxConfig.lang.size() == 0) {
			fritzboxConfig.lang = "de";
			dsyslog("fritztools.c: error parsing interface language, assuming 'de'");
		}
	}
	return fritzboxConfig.lang;
}

void cFritzTools::Login() {
	// no password, no login
	if (fritzboxConfig.password.length() == 0)
		return;
	
	std::ostringstream sMsg;

	dsyslog("fritztools.c: sending login request to fritz.box.");
	cHttpClient tc(fritzboxConfig.url, PORT_WWW);
	tc   <<		"POST /cgi-bin/webcm HTTP/1.1\n"
	     <<  	"Content-Type: application/x-www-form-urlencoded\n"
	     <<  	"Content-Length: "
	     <<  	23 + UrlEncode(fritzboxConfig.password).size()
	     <<  	"\n\nlogin:command/password="
	     <<  	UrlEncode(fritzboxConfig.password)
	     <<  	"\n";
	tc >> sMsg;

	// determine if login was successful
	if (sMsg.str().find("class=\"errorMessage\"") != std::string::npos) { 
		esyslog("fritztools.c: login failed, check your password settings.");
		throw cToolsException(cToolsException::ERR_LOGIN_FAILED);
	}
	dsyslog("fritztools.c: login successful.");
}

std::string cFritzTools::UrlEncode(std::string &s_input) {
	std::string result;
	std::string s;
	std::string hex = "0123456789abcdef";
	cCharSetConv *conv = new cCharSetConv(cCharSetConv::SystemCharacterTable(), "ISO-8859-15");
	s = conv->Convert(s_input.c_str());
	delete(conv);
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
		isyslog("fritztools.c: sending call init request (%s).", number.c_str());
		cHttpClient tc(fritzboxConfig.url, PORT_WWW);
		tc  <<	"POST /cgi-bin/webcm HTTP/1.1\n"
			<<	"Content-Type: application/x-www-form-urlencoded\n"
			<<	"Content-Length: "
			<<	95 + number.length() + cFritzTools::GetLang().length()
			<<	"\n\n"
			<<	"getpage=../html/"  
			<<  cFritzTools::GetLang()  
			<< "/menus/menu2.html&var%3Apagename=fonbuch&var%3Amenu=home&telcfg%3Acommand/Dial=" 
			<<	number
			<<	"\n";
		tc >> msg;
		isyslog("fritztools.c: call initiated.");
	} catch (cTcpException te) {
		esyslog("fritztools.c: Exception - %s", te.what());
		return false;
	}
	return true;	
}

std::string cFritzTools::NormalizeNumber(std::string number) {
	GetPhoneSettings();
	// Only for Germany: Remove Call-By-Call Provider Selection Codes 010(0)xx
	if (fritzboxConfig.countryCode == "49") {
		if (number[0] == '0' && number[1] == '1' && number[2] == '0') {
			if (number[3] == '0') 
				number.erase(0, 6);
			else
				number.erase(0, 5);
		}
	}
	// Modifies 'number' to the following format
	// '00' + countryCode + regionCode + phoneNumber
	if (number[0] == '+') {
		//international prefix given in form +49 -> 0049
		number.replace(0, 1, "00");
	} else if (number[0] == '0' && number[1] != '0') {
		//national prefix given 089 -> 004989
		number.replace(0, 1, fritzboxConfig.countryCode.c_str());
		number = "00" + number;
	} else if (number[0] != '0') {
		// number without country or region code, 1234 -> +49891234
		number = "00" + fritzboxConfig.countryCode + fritzboxConfig.regionCode + number; 		
	} // else: number starts with '00', do not change
	return number;
}

int cFritzTools::CompareNormalized(std::string number1, std::string number2) {
	return NormalizeNumber(number1).compare(NormalizeNumber(number2));
}

void cFritzTools::GetPhoneSettings() {
	// if countryCode or regionCode are already set, exit here...
	if (fritzboxConfig.countryCode.size() > 0 || fritzboxConfig.regionCode.size() > 0)
		return;		
	// ...otherwise get settings from Fritz!Box.
	dsyslog("fritztools.c: Looking up Phone Settings...");
	std::string msg;
	try {
		Login();
		cHttpClient hc(fritzboxConfig.url, PORT_WWW);
		hc << "GET /cgi-bin/webcm?getpage=../html/"
		   <<  cFritzTools::GetLang()
		   << "/menus/menu2.html&var%3Alang="
		   <<  cFritzTools::GetLang()
		   << "&var%3Apagename=sipoptionen&var%3Amenu=fon HTTP/1.1\n\n";
		hc >> msg;
	} catch (cTcpException te) {
		esyslog("fritztools.c: cTcpException - %s", te.what());
		return;
	} catch (cToolsException te) {
		esyslog("fritztools.c: cToolsException - %s", te.what());
		return;
	}
	unsigned int lkzStart = msg.find("telcfg:settings/Location/LKZ");
	if (lkzStart == std::string::npos) {
		esyslog("fritztools.c: Parser error in GetPhoneSettings(). Could not find LKZ.");
		esyslog("fritztools.c: LKZ not set! Assuming 49 (Germany).");
		esyslog("fritztools.c: OKZ not set! Resolving phone numbers may not always work.");
		fritzboxConfig.countryCode = "49";
		return;
	}
	lkzStart += 37;
	unsigned int lkzStop  = msg.find("\"", lkzStart);
	unsigned int okzStart = msg.find("telcfg:settings/Location/OKZ");
	if (okzStart == std::string::npos) {
		esyslog("fritztools.c: Parser error in GetPhoneSettings(). Could not find OKZ.");
		esyslog("fritztools.c: OKZ not set! Resolving phone numbers may not always work.");
		return;
	}
	okzStart += 37;
	unsigned int okzStop = msg.find("\"", okzStart);
	fritzboxConfig.countryCode = msg.substr(lkzStart, lkzStop - lkzStart);
	fritzboxConfig.regionCode  = msg.substr(okzStart, okzStop - okzStart);
	if (fritzboxConfig.countryCode.size() > 0) {
		dsyslog("fritztools.c: Found LKZ %s.", fritzboxConfig.countryCode.c_str());
	} else {
		esyslog("fritztools.c: LKZ not set! Assuming 49 (Germany).");
		fritzboxConfig.countryCode = "49";
	}
	if (fritzboxConfig.regionCode.size() > 0) {
		dsyslog("fritztools.c: Found OKZ %s.", fritzboxConfig.regionCode.c_str());
	} else {
		esyslog("fritztools.c: OKZ not set! Resolving phone numbers may not always work.");
	}
}

std::string cFritzTools::ComposeCallMessage(sCallInfo *callInfo) {
	// medium gets MSN appended if ISDN is used
	std::string medium = callInfo->medium;
	if (callInfo->medium.find("ISDN") != std::string::npos)
		medium += " " + callInfo->localNumber;
	char *msg;
	// compose the message to be displayed
	if (callInfo->isOutgoing == true) {
		asprintf(&msg, tr("Calling %s [%s]"), callInfo->remoteName.c_str(), medium.c_str());
	} else {
		if (callInfo->remoteNumber.size() == 0) {
			// unknown caller
			asprintf(&msg, "%s [%s]", tr("Call"), medium.c_str());
		} else {
			// known caller
			asprintf(&msg, "%s %s [%s]", tr("Call from"), callInfo->remoteName.c_str(), medium.c_str());
		}
	}
	std::string rMsg = msg;
	free(msg);
	return rMsg;
}
