/*
 * ci.hh: Common Interface
 *
 * Copyright (C) 2000 Klaus Schmidinger
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 *
 * The author can be reached at kls@cadsoft.de
 *
 * The project's page is at http://www.cadsoft.de/people/kls/vdr
 *
 */

#ifndef __CI_H
#define __CI_H

#include <stdint.h>
#include <stdio.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/uio.h>

#define MAXCASYSTEMIDS 16

class cMutex {
  friend class cCondVar;
private:
  pthread_mutex_t mutex;
  pid_t lockingPid;
  int locked;
public:
  cMutex(void);
  ~cMutex();
  void Lock(void);
  void Unlock(void);
  };

class cMutexLock {
private:
  cMutex *mutex;
  bool locked;
public:
  cMutexLock(cMutex *Mutex = NULL);
  ~cMutexLock();
  bool Lock(cMutex *Mutex);
  };


class cCiMMI;

class cCiMenu {
  friend class cCiMMI;
private:
  enum { MAX_CIMENU_ENTRIES = 64 }; ///< XXX is there a specified maximum?
  cCiMMI *mmi;
  bool selectable;
  char *titleText;
  char *subTitleText;
  char *bottomText;
  char *entries[MAX_CIMENU_ENTRIES];
  int numEntries;
  bool AddEntry(char *s);
  cCiMenu(cCiMMI *MMI, bool Selectable);
public:
  ~cCiMenu();
  const char *TitleText(void) { return titleText; }
  const char *SubTitleText(void) { return subTitleText; }
  const char *BottomText(void) { return bottomText; }
  const char *Entry(int n) { return n < numEntries ? entries[n] : NULL; }
  int NumEntries(void) { return numEntries; }
  bool Selectable(void) { return selectable; }
  bool Select(int Index);
  bool Cancel(void);
  };

class cCiEnquiry {
  friend class cCiMMI;
private:
  cCiMMI *mmi;
  char *text;
  bool blind;
  int expectedLength;
  cCiEnquiry(cCiMMI *MMI);
public:
  ~cCiEnquiry();
  const char *Text(void) { return text; }
  bool Blind(void) { return blind; }
  int ExpectedLength(void) { return expectedLength; }
  bool Reply(const char *s);
  bool Cancel(void);
  };

class cCiCaPmt {
  friend class cCiConditionalAccessSupport;
private:
  int length;
  int esInfoLengthPos;
  uint8_t capmt[2048]; ///< XXX is there a specified maximum?
public:
  cCiCaPmt(int ProgramNumber);
  void AddPid(int Pid);
  void AddCaDescriptor(int Length, uint8_t *Data);
  };

#define MAX_CI_SESSION  16 //XXX

class cCiSession;
class cCiTransportLayer;
class cCiTransportConnection;

class cCiHandler {
private:
  cMutex mutex;
  int numSlots;
  bool newCaSupport;
  bool hasUserIO;
  cCiSession *sessions[MAX_CI_SESSION];
  cCiTransportLayer *tpl;
  cCiTransportConnection *tc;
  int ResourceIdToInt(const uint8_t *Data);
  bool Send(uint8_t Tag, int SessionId, int ResourceId = 0, int Status = -1);
  cCiSession *GetSessionBySessionId(int SessionId);
  cCiSession *GetSessionByResourceId(int ResourceId, int Slot);
  cCiSession *CreateSession(int ResourceId);
  bool OpenSession(int Length, const uint8_t *Data);
  bool CloseSession(int SessionId);
  int CloseAllSessions(int Slot);
  cCiHandler(int Fd, int NumSlots);
public:
  ~cCiHandler();
  static cCiHandler *CreateCiHandler(const char *FileName);
  int NumSlots(void) { return numSlots; }
  bool Process(void);
  bool HasUserIO(void) { return hasUserIO; }
  bool EnterMenu(int Slot);
  cCiMenu *GetMenu(void);
  cCiEnquiry *GetEnquiry(void);
  bool SetCaPmt(cCiCaPmt &CaPmt);
  const unsigned short *GetCaSystemIds(int Slot);
  bool SetCaPmt(cCiCaPmt &CaPmt, int Slot);
  bool Reset(int Slot);
  };

int tcp_listen(struct sockaddr_in *name,int sckt,unsigned long address=INADDR_ANY);
int accept_tcp(int ip_sock,struct sockaddr_in *ip_name);
int udp_listen(struct sockaddr_un *name,char const * const filename);
int accept_udp(int ip_sock,struct sockaddr_un *ip_name);

#endif //__CI_H
