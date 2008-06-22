/*
 * access.c: wapd server access
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


#include <ctype.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vdr/svdrp.h>
#include "access.h"


// -- cWapUser ---------------------------------------------------------------

cWapUser::cWapUser(void)
{
  user = NULL;
  pass = NULL;
}

cWapUser::~cWapUser()
{
  free(user);
  free(pass);
}

bool cWapUser::Parse(const char *s)
{
  user = strdup(s);
  char *p = strchr(user, ':');
  *p++ = 0;
  pass = strdup(p);
  return pass != NULL;
}

bool cWapUser::Accepts(char *User, char *Pass)
{
  if (strcmp(user, User) != 0) return false;
  if (strcmp(pass, crypt(Pass, pass)) != 0) return false;
  return true;
}

// -- cWapAccess -------------------------------------------------------------

cWapAccess WapAccess;

cWapAccess::cWapAccess(void)
{
  user = NULL;
  pass = NULL;
}

cWapAccess::~cWapAccess()
{
  free(user);
  free(pass);
}

bool cWapAccess::Acceptable(char *Auth)
{
  user = strdup(Auth);
  char *p = strchr(user, ':');
  *p = 0;
  pass = strdup(++p);
  cWapUser *a = First();
  while (a) {
     if (a->Accepts(user, pass))
        return true;
     a = (cWapUser *)a->Next();
  }
  return false;
}

// --- cWapHost --------------------------------------------------------------

cWapHost::cWapHost(void)
{
  addr.s_addr = 0;
  mask = 0;
}

bool cWapHost::Parse(const char *s)
{
  mask = 0xFFFFFFFF;
  const char *p = strchr(s, '/');
  if (p) {
     char *error = NULL;
     int m = strtoul(p + 1, &error, 10);
     if (error && *error && !isspace(*error) || m > 32)
        return false;
     *(char *)p = 0; // yes, we know it's 'const' - will be restored!
     if (m == 0)
        mask = 0;
     else
        mask >>= (32 - m);
  }
  int result = inet_aton(s, &addr);
  if (p)
     *(char *)p = '/'; // there it is again
  return result != 0 && (mask != 0 || addr.s_addr == 0);
}

bool cWapHost::Accepts(in_addr_t Address)
{
  return (Address & mask) == addr.s_addr;
}

// --- cWapHosts -------------------------------------------------------------

cWapHosts WapHosts;

bool cWapHosts::Acceptable(in_addr_t Address)
{
  cWapHost *h = First();
  while (h) {
     if (h->Accepts(Address))
        return true;
     h = (cWapHost *)h->Next();
  }
  return false;
}

// --- cWapSocket ------------------------------------------------------------

cWapSocket::cWapSocket(int Port, int Queue)
{
  port = Port;
  sock = -1;
  queue = Queue;
}

cWapSocket::~cWapSocket()
{
  Close();
}

void cWapSocket::Close(void)
{
  if (sock >= 0) {
     close(sock);
     sock = -1;
  }
}

bool cWapSocket::Open(void)
{
  if (sock < 0) {
     // create socket:
     sock = socket(PF_INET, SOCK_STREAM, 0);
     if (sock < 0) {
        LOG_ERROR;
        port = 0;
        return false;
     }
     // allow it to always reuse the same port:
     int ReUseAddr = 1;
     setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ReUseAddr, sizeof(ReUseAddr));
     //
     struct sockaddr_in name;
     name.sin_family = AF_INET;
     name.sin_port = htons(port);
     name.sin_addr.s_addr = htonl(INADDR_ANY);
     if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
        LOG_ERROR;
        Close();
        return false;
     }
     // make it non-blocking:
     int oldflags = fcntl(sock, F_GETFL, 0);
     if (oldflags < 0) {
        LOG_ERROR;
        return false;
     }
     oldflags |= O_NONBLOCK;
     if (fcntl(sock, F_SETFL, oldflags) < 0) {
        LOG_ERROR;
        return false;
     }
     // listen to the socket:
     if (listen(sock, queue) < 0) {
        LOG_ERROR;
        return false;
     }
  }
  return true;
}

int cWapSocket::Accept(void)
{
  if (Open()) {
     struct sockaddr_in clientname;
     uint size = sizeof(clientname);
     int newsock = accept(sock, (struct sockaddr *)&clientname, &size);
     if (newsock > 0) {
        bool accepted = WapHosts.Acceptable(clientname.sin_addr.s_addr);
        if (!accepted) {
           const char *s =
              PROTOCOL " 403 Forbidden\n"
              "Server: " SERVER_NAME "\n"
              "Content-Type: text/plain\n\n"
              "Access denied!\n";
           write(newsock, s, strlen(s));
           close(newsock);
           newsock = -1;
        }
        isyslog("WAPD: connect from %s, port %hu - %s",
           inet_ntoa(clientname.sin_addr), ntohs(clientname.sin_port), accepted ? "accepted" : "DENIED");
     } else if (errno != EINTR && errno != EAGAIN)
        LOG_ERROR;
     return newsock;
  }
  return -1;
}


