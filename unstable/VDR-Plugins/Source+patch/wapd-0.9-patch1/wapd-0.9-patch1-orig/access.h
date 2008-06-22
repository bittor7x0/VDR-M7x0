/*
 * access.h: wapd server access
 *
 * WAP remote control plugin for VDR (the Video Disk Recorder)
 * Copyright (C) 2002  Thomas Heiligenmann  <thomas@heiligenmann.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA
 *
 */


#ifndef __WAPD_ACCESS_H
#define __WAPD_ACCESS_H

#define PROTOCOL                   "HTTP/1.0"
#define SERVER_NAME                "VDR WAPD"

#include <vdr/config.h>

class cWapUser : public cListObject {
private:
  char *user;
  char *pass;
public:
  cWapUser(void);
  virtual ~cWapUser();
  bool Parse(const char *s);
  bool Accepts(char *User, char *Pass);
};

class cWapAccess : public cConfig<cWapUser> {
private:
  char *user;
  char *pass;
public:
  cWapAccess(void);
  virtual ~cWapAccess();
  bool Acceptable(char *Auth);
};

extern cWapAccess WapAccess;

class cWapHost : public cListObject {
private:
  struct in_addr addr;
  in_addr_t mask;
public:
  cWapHost(void);
  bool Parse(const char *s);
  bool Accepts(in_addr_t Address);
};

class cWapHosts : public cConfig<cWapHost> {
public:
  bool Acceptable(in_addr_t Address);
};

extern cWapHosts WapHosts;

class cWapSocket {
private:
  int port;
  int sock;
  int queue;
  void Close(void);
public:
  cWapSocket(int Port, int Queue = 1);
  ~cWapSocket();
  bool Open(void);
  int Accept(void);
};

#endif //__WAPD_ACCESS_H
