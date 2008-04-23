/*
 * WAP remote control plugin for VDR
 * access.h: wapd server access
 *
 * See the README file for copyright information and how to reach the author.
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */


#ifndef __WAPD_ACCESS_H
#define __WAPD_ACCESS_H

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
