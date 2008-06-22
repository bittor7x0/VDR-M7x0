/*
 * server.h: wapd server core
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


#ifndef __WAPD_SERVER_H
#define __WAPD_SERVER_H

#include <string.h>
#include <vdr/thread.h>
#include <vdr/tools.h>
#include "access.h"

#define MYPARSEBUFFER  KILOBYTE(10)
#define WMLBUFFER      3*MYPARSEBUFFER


class cWapServer : cThread {
private:
  enum eMethod { mUnknown, mGet, mPost };

  cWapSocket socket;
  cFile file;
  cRecordings Recordings;
  char *buffer;
  int offsetPageScheduleEvent, offsetPageScheduleNowNext, offsetPageScheduleChannel;
  int offsetPageChannels, offsetPageTimers,offsetPageRecordings, offsetPageLanguage;

  eMethod method;
  char uri[MYPARSEBUFFER];
  char path[MYPARSEBUFFER];
  char opt[MYPARSEBUFFER];
  char host[MYPARSEBUFFER];
  char authorization[MYPARSEBUFFER];
  char getopt[MYPARSEBUFFER];
  char wmlbuffer[WMLBUFFER];

  char *wmltr(const char *s);
  bool Send(const char *s, int length = -1);
  void Reply(const char *fmt, ...);
  void ReplyBuffer(const char *fmt, ...);
  void FlushBuffer(int length = -1);
  void Header(int status, char* title, char* mime_type, char* realm = NULL, size_t size = 0, int refresh = 0);
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

  void ScheduleEvent(cChannel* channel, const cSchedules* schedules);
  void ScheduleNowNext(cChannel* channel, const cSchedules* schedules);
  void ScheduleChannel(cChannel* channel, const cSchedules* schedules);

  void PageSchedule(void);
  void PageChannels(void);
  void PageTimers(void);
  void PageRecordings(void);
  void PageLanguage(void);
  void PageGrabImage(void);
  void PageMain(void);

public:
  cWapServer(int Port);
  ~cWapServer();
};


#endif //__WAPD_SERVER_H
