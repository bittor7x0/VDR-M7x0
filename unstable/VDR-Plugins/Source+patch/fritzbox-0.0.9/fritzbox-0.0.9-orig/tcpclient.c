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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <string>
#include <sstream>

#include <vdr/tools.h>
#include "tcpclient.h"


cTcpClient::cTcpClient(std::string &hostname, int port) {
	connected = false;
	this->hostname = hostname;
	this->port = port;
}

cTcpClient::~cTcpClient() {
	close(fd);
}

void cTcpClient::Connect() {
	if (connected) {
		close(fd);
		connected = false;
	}

	struct hostent *host = 0;
	struct sockaddr_in addr;

	if (!host && !inet_aton(hostname.c_str(), &addr.sin_addr)) {
		host = gethostbyname(hostname.c_str());
		if (!host)
			throw cTcpException(cTcpException::ERR_HOST_NOT_RESOLVABLE);
		addr.sin_addr = *(struct in_addr*)host->h_addr;
//		dsyslog("tcpclient.c: Resolved hostname.");
	}

	fd = socket(PF_INET, SOCK_STREAM, 0);

	if (fd < 0)
		throw cTcpException(cTcpException::ERR_SOCKET_CREATE);
//	dsyslog("tcpclient.c: Created socket.");
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr))< 0){
		close(fd);
		throw cTcpException(cTcpException::ERR_HOST_NOT_REACHABLE);
	}
//	dsyslog("tcpclient.c: Successfully connected.");
	connected = true;
}

// ----- Stuff for reading from socket ----------------------------------------

std::string cTcpClient::Read() {
	std::string result;
	int size = 0;
	if (!connected)
		Connect();
	size = recv(fd, buffer, TCP_BUF_SIZE - 1, 0);
	if (size == -1) {
		connected = false;
		close(fd);
		throw cTcpException(cTcpException::ERR_CONNECTION_RESET);
	}
	buffer[size] = '\0';
	result += buffer;
//	dsyslog("tcpclient.c: receiving %s", result.c_str());
	return result;
}

cTcpClient &cTcpClient::operator>>(std::ostringstream &ss) {
	ss << Read();
	return *this;
}

cTcpClient &cTcpClient::operator>>(std::string &s) {
	s += Read();
	return *this;
}

// ----- Stuff for writing to socket ------------------------------------------

void cTcpClient::Write(std::string s) {
	if (!connected)
		Connect();
	int size = send(fd, s.c_str(), s.length(), 0);
//	dsyslog("tcpclient.c: sending %s", s.c_str());
	if (size == -1) {
		connected = false;
		close(fd);
		throw cTcpException(cTcpException::ERR_CONNECTION_RESET);
	}
}

cTcpClient &cTcpClient::operator<<(std::string s) {
	Write(s);
	return *this;
}

cTcpClient &cTcpClient::operator<<(char c[]) {
	Write(std::string(c));
	return *this;
}

cTcpClient &cTcpClient::operator<<(int i) {
	std::ostringstream sData;
	sData << i;
	Write(sData.str());
	return *this;
}

// ----- cHttpClient ----------------------------------------------------------

std::string cHttpClient::Read() {
	std::string result;
	int size = 0;
	if (!connected)
		Connect();
	do {
		size = recv(fd, buffer, TCP_BUF_SIZE - 1, 0);
		if (size == -1) {
			connected = false;
			close(fd);
			throw cTcpException(cTcpException::ERR_CONNECTION_RESET);
		}
		buffer[size] = '\0';
		result += buffer;
	} while (size > 0);
//	dsyslog("tcpclient.c: receiving %s", result.c_str());
	return result;
}

cHttpClient::cHttpClient(std::string &hostname, int port) 
:cTcpClient(hostname, port) {
	
}

