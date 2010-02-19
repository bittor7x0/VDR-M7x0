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

#include <unistd.h>
#include <vdr/tools.h>
#include "oertlichesfonbuch.h"
#include "tcpclient.h"
#include "fritztools.h"

cOertlichesFonbuch::cOertlichesFonbuch()
{
	title = "das-oertliche.de"; 
	techId = "OERT";
	displayable = false;
}

cOertlichesFonbuch::~cOertlichesFonbuch()
{
}

bool cOertlichesFonbuch::Initialize() {
	setInitialized(true);
	return true;
}

std::string cOertlichesFonbuch::ResolveToName(std::string &number) {
	// resolve only german phone numbers
	if (cFritzTools::NormalizeNumber(number).find("0049") != 0)
		return number;
	
	std::string msg;
	std::string name;
	try {
		dsyslog("oertlichesfonbuch.c: sending reverse lookup request for %s to www.dasoertliche.de", cFritzTools::NormalizeNumber(number).c_str());
		std::string host = "www.dasoertliche.de";
		cHttpClient tc(host, PORT_WWW);
		tc << "GET /?id=9999999999999999999999&form_name=detail&lastFormName=search_inv&ph=" << cFritzTools::NormalizeNumber(number) << "&recFrom=1&hitno=0&zvo_ok=1&page=TREFFERLISTE&context=TREFFERLISTE&action=TEILNEHMER&detvert_ok=1 HTTP/1.1\nHost: www.dasoertliche.de\nAccept-Charset: ISO-8859-1\nUser-Agent: Lynx/2.8.5\nConnection: close\n\n\0";		
		tc >> msg;
	} catch (cTcpException te) {
		esyslog("oertlichesfonbuch.c: Exception - %s", te.what());
		return number;
	}
	// parse answer
	size_t start = msg.find("<div class=\"detail_top\">");
	if (start == std::string::npos) {
		isyslog("oertlichesfonbuch.c: no entry found.");
		return number;
	}
	// add the length of search pattern
	start += 24;
	size_t stop  = msg.find("<", start);
	name = msg.substr(start, stop - start);
	// convert the string from latin1 to current system character table
	cCharSetConv *conv = new cCharSetConv("ISO-8859-1", cCharSetConv::SystemCharacterTable());
	const char *s_converted = conv->Convert(name.c_str());
	name = s_converted;
	delete (conv);
	isyslog("oertlichesfonbuch.c: resolves to %s.", name.c_str());
	return name;
}

