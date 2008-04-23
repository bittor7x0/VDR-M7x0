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

#ifndef FRITZTOOLS_H_
#define FRITZTOOLS_H_

#include <stdexcept>
#include <string>
#include <vdr/thread.h>

class cToolsException : public std::exception {
public:
	enum errorCode {
		ERR_LOGIN_FAILED,
	} errcode;

	cToolsException(errorCode errcode) {
		this->errcode = errcode;
	}

	virtual const char* what() const throw() {
		switch (errcode) {
		case ERR_LOGIN_FAILED:
			return "Login failed.";
		default:
			return "Unknown cToolsException happened.";
		}

	}
};

class cFritzTools
{
public:
	cFritzTools();
	virtual ~cFritzTools();
	static void Login();
	static bool InitCall(std::string &number);
	static cMutex* getFritzBoxMutex(){return fritzBoxMutex;}
private:
	static std::string UrlEncode(std::string &s);
	static cMutex* fritzBoxMutex;
};

#endif /*FRITZTOOLS_H_*/
