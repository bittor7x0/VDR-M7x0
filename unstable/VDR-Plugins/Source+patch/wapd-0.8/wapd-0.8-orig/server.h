/*
 * WAP remote control plugin for VDR
 * server.h: wapd server core
 *
 * See the README file for copyright information and how to reach the author.
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */


#ifndef __WAPD_SERVER_H
#define __WAPD_SERVER_H

#include <vdr/thread.h>
#include <vdr/tools.h>
#include "access.h"

#define MAXPARSEBUFFER KILOBYTE(10)
#define WMLBUFFER      3*MAXPARSEBUFFER

class cWapServer : cThread {
  friend class cPluginWapd;

private:
  enum eMethod { mUnknown, mGet, mPost };

  bool active;
  cWapSocket socket;
  cFile file;
  cRecordings Recordings;
  char *buffer;

  eMethod method;
  char uri[MAXPARSEBUFFER];
  char path[MAXPARSEBUFFER];
  char opt[MAXPARSEBUFFER];
  char host[MAXPARSEBUFFER];
  char authorization[MAXPARSEBUFFER];
  char getopt[MAXPARSEBUFFER];
  char wmlbuffer[WMLBUFFER];

  char *wmltr(const char *s);
  bool Send(const char *s, int length = -1);
  void Reply(const char *fmt, ...);
  void ReplyBuffer(const char *fmt, ...);
  void FlushBuffer(void);
  void Header(int status, char* title, char* mime_type, char* realm = NULL);
  void Error(int status, char* title, char* text);
  bool ProcessHeader(void);
  bool Option(const char *Name);
  virtual void Action(void);

  void SwitchChannel(void);
  void DeleteChannel(void);
  void UpdateChannel(void);
  void OnOffTimer(void);
  void DeleteTimer(void);
  void UpdateTimer(void);
  void DeleteRecording(void);
  void SetLanguage(void);

  void EditChannel(void);
  void EditTimer(void);
  void EditRecording(void);

  void PageSchedule(void);
  void PageChannels(void);
  void PageTimers(void);
  void PageRecordings(void);
  void PageLanguage(void);
  void PageMain(void);

protected:
  void Shutdown(void);

public:
  cWapServer(int Port);
  ~cWapServer();
  };


#endif //__WAPD_SERVER_H
