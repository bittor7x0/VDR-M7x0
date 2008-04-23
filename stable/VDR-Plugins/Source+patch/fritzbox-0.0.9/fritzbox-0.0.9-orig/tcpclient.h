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

#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

#include <string>
#include <sstream>
#include <stdexcept>

#define PORT_WWW 80
#define PORT_MONITOR 1012
#define TCP_BUF_SIZE 4096

class cTcpException : public std::exception {
public:
	enum errorCode {
		ERR_HOST_NOT_RESOLVABLE,
		ERR_HOST_NOT_REACHABLE,
		ERR_SOCKET_CREATE,
		ERR_CONNECTION_RESET
	} errcode;

	cTcpException(errorCode errcode) {
		this->errcode = errcode;
	}

	virtual const char* what() const throw() {
		switch (errcode) {
		case ERR_HOST_NOT_RESOLVABLE:
			return "Host is not resolvable.";
		case ERR_HOST_NOT_REACHABLE:
			return "Host is not reachable.";
		case ERR_SOCKET_CREATE:
			return "Could not create socket.";
		case ERR_CONNECTION_RESET:
			return "Connection was reset.";
		default:
			return "Unknown cTcpException happened.";
		}

	}
};

class cTcpClient {
private:
	std::string hostname;
	int port;
	void Write(std::string s);
protected:
	char buffer[TCP_BUF_SIZE];
	bool connected;
	int fd;
	void Connect();
	virtual std::string Read();
public:
	cTcpClient(std::string &hostname, int port);
	virtual ~cTcpClient();
	// ">>" is a blocking operation 
	cTcpClient &operator>>(std::ostringstream &ss);
	cTcpClient &operator>>(std::string &s);
	cTcpClient &operator<<(std::string s);
	cTcpClient &operator<<(char c[]);
	cTcpClient &operator<<(int i);
};

class cHttpClient : public cTcpClient {
protected:
	std::string Read();
public:
	cHttpClient(std::string &hostname, int port);
};

#endif /*TCPCLIENT_H_*/
