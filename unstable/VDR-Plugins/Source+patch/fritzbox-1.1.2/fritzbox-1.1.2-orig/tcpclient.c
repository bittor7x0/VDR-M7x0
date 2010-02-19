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
	fd = -1;
}

cTcpClient::~cTcpClient() {
        if (fd >= 0) {
	        shutdown(fd, SHUT_RDWR);
	        close(fd);
	}
}

void cTcpClient::Connect() {
	if (connected) {
		close(fd);
		connected = false;
	}

	struct addrinfo *ainfo;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int res;

	res = getaddrinfo(hostname.c_str(), NULL, &hints, &ainfo);
	if (res != 0) {
		throw cTcpException(cTcpException::ERR_HOST_NOT_RESOLVABLE);
	}

	fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw cTcpException(cTcpException::ERR_SOCKET_CREATE);
	((sockaddr_in*)(ainfo->ai_addr))->sin_port = htons(port);
	((sockaddr_in*)(ainfo->ai_addr))->sin_family = AF_INET;
	
	res = connect(fd, ainfo->ai_addr, sizeof(struct sockaddr));
	if (res < 0) {
		int errCode = errno;
		close(fd);
		freeaddrinfo(ainfo);
		switch (errCode) {
		case EALREADY:
			throw cTcpException(cTcpException::ERR_ALREADY_TRYING);
		case ECONNREFUSED:
			throw cTcpException(cTcpException::ERR_CONNECTION_REFUSED);
		case EHOSTUNREACH:
		case ENETUNREACH:
		case ENETDOWN:
			throw cTcpException(cTcpException::ERR_HOST_NOT_REACHABLE);
		case EINVAL:
			throw cTcpException(cTcpException::ERR_INVALID_ARGUMENT);
		case EOPNOTSUPP:
			throw cTcpException(cTcpException::ERR_OPERATION_NOT_SUPPORTED);
		case EPROTOTYPE:
			throw cTcpException(cTcpException::ERR_PROTOTYPE);
		case EISCONN:
			throw cTcpException(cTcpException::ERR_ALREADY_CONNECTED);
		case ENOBUFS:
			throw cTcpException(cTcpException::ERR_OUT_OF_MEMORY);
		case ETIMEDOUT:
			throw cTcpException(cTcpException::ERR_TIMEOUT);
		default:
			throw cTcpException(cTcpException::ERR_UNKNOWN);
		}
		
	}
	freeaddrinfo(ainfo);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	connected = true;
}

void cTcpClient::Disconnect() {
	if (connected) {
		close(fd);
		connected = false;
	}
}

// ----- Stuff for reading from socket ----------------------------------------

std::string cTcpClient::Receive() {
	int size = 0;
	std::string result;
	do {
		struct pollfd fds[1];
		fds[0].fd = fd;
		fds[0].events = POLLIN | POLLPRI;
		poll(&(fds[0]), 1, 500);
		size = recv(fd, buffer, TCP_BUF_SIZE - 1, 0);
	} while (size == -1 && errno == EAGAIN);
	if (size == 0) 	{
		// connection was orderly closed
		connected = false;
		close(fd);	
	} else if (size == -1) {
		// there occured an error
		connected = false;
		close(fd);
		throw cTcpException(cTcpException::ERR_SOCKET_ERROR);
	}
	buffer[size] = '\0';
	//	dsyslog("tcpclient.c: receiving %s", result.c_str());
	result = buffer;
	return result;
}

std::string cTcpClient::Read() {
	if (!connected)
		Connect();
	return Receive();
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
	//dsyslog("tcpclient.c: sending %s", s.c_str());
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
	unsigned int length = 0;
	if (!connected)
		Connect();
	do {
		length = result.length();
		result += Receive();
	} while (result.length() > length);
	return result;
}

cHttpClient::cHttpClient(std::string &hostname, int port) 
:cTcpClient(hostname, port) {
	
}

