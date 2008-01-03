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

cOertlichesFonbuch::cOertlichesFonbuch()
{
	title = "Das Örtliche"; 
	techId = "OERT";
	displayable = false;
}

cOertlichesFonbuch::~cOertlichesFonbuch()
{
}

std::string cOertlichesFonbuch::ResolveToName(std::string &number){

	std::string msg;
	std::string name;
	try {
		dsyslog("oertlichesfonbuch.c: sending reverse lookup request to www.dasoertliche.de");
		std::string host = "www.dasoertliche.de";
		cHttpClient tc(host, PORT_WWW);
		tc << "GET /?form_name=detail&lastFormName=search_inv&ph=" << number << "&recFrom=1&hitno=99&zvo_ok=1&page=TREFFERLISTE&context=TREFFERLISTE&action=TEILNEHMER&detvert_ok=1 HTTP/1.1\nHost: www.dasoertliche.de\nAccept-Charset: ISO-8859-1\n\n\0";
		do {
			tc >> msg;
			usleep(100000);
		} while (msg.find(std::string("</html>")) == std::string::npos);

	} catch (cTcpException te) {
		dsyslog("oertlichesfonbuch.c: Exception - %s", te.what());
		return number;
	}
	// parse answer
	size_t start = msg.find("<div class=\"detail_top\">");
	if (start == std::string::npos)
		return number;
	// add the length of search pattern
	start += 24;
	size_t stop  = msg.find("<", start);
	name = msg.substr(start, stop - start);
#if VDRVERSNUM >= 10503
		// convert the string from latin1 to current system character table
		cCharSetConv *conv = new cCharSetConv("ISO-8859-1", cCharSetConv::SystemCharacterTable());
		const char *s_converted = conv->Convert(name.c_str());
		name = s_converted;
		delete (conv);
#endif
	return name;
}

