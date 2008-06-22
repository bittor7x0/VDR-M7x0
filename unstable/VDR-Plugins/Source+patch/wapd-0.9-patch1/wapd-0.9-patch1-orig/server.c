/*
 * server.c: wapd server core
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
#include <stdarg.h>
#include <vdr/tools.h>
#include <vdr/i18n.h>
#include <vdr/recording.h>
#include <vdr/device.h>
#include <vdr/channels.h>
#include <vdr/epg.h>
#include <vdr/timers.h>
#include <vdr/font.h>
#include <vdr/tools.h>
#include <vdr/config.h>
#include "server.h"
#include "tools.h"
#include "i18n.h"

#define MAXLINES                   9

#define RFC1123FMT                 "%a, %d %b %Y %H:%M:%S GMT"

#define URI_MAIN                   "/"
#define URI_SCHEDULE               "/schedule"
#define URI_CHANNELS               "/channels"
#define URI_TIMERS                 "/timers"
#define URI_RECORDINGS             "/recordings"
#define URI_LANGUAGE               "/language"
#define URI_GRABIMAGE              "/grabimage"
#define URI_EDITCHANNEL            "/editchannel"
#define URI_UPDATECHANNEL          "/updatechannel"
#define URI_SWITCHCHANNEL          "/switchchannel"
#define URI_DELETECHANNEL          "/deletechannel"
#define URI_EDITTIMER              "/edittimer"
#define URI_UPDATETIMER            "/updatetimer"
#define URI_ONOFFTIMER             "/onofftimer"
#define URI_DELETETIMER            "/deletetimer"
#define URI_EDITRECORDING          "/editrecording"
#define URI_DELETERECORDING        "/deleterecording"
#define URI_SETLANGUAGE            "/setlanguage"



// --- cWapServer ------------------------------------------------------------

cWapServer::cWapServer(int Port) : cThread(), socket(Port)
{
  buffer = NULL;
  memset(wmlbuffer, 0, WMLBUFFER);
  offsetPageScheduleNowNext = 0;
  offsetPageScheduleChannel = 0;
  offsetPageChannels = 0;
  offsetPageTimers = 0;
  offsetPageRecordings = 0;
  offsetPageLanguage = 0;
  Start();
}

cWapServer::~cWapServer()
{
  Cancel(0);
  free(buffer);
}

char *cWapServer::wmltr(const char *s)
{
  buffer = wmlescape(strdup(tr(s)));
  return buffer;
}

bool cWapServer::Send(const char *s, int length)
{
  if (length < 0)
     length = strlen(s);
  if (safe_write(file, s, length) < 0) {
     LOG_ERROR;
     file.Close();
     return false;
  }
  return true;
}

void cWapServer::Reply(const char *fmt, ...)
{
  if (file.IsOpen()) {
     va_list ap;
     va_start(ap, fmt);
     char *buf;
     vasprintf(&buf, fmt, ap);
     Send(buf);
     free(buf);
     va_end(ap);
  }
}

void cWapServer::ReplyBuffer(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *buf;
  vasprintf(&buf, fmt, ap);
  if (strlen(wmlbuffer) + strlen(buf) < WMLBUFFER)
     strcat(wmlbuffer, buf);
  free(buf);
  va_end(ap);
}

void cWapServer::FlushBuffer(int length)
{
  if (file.IsOpen())
     Send(wmlbuffer, length);
  memset(wmlbuffer, 0, WMLBUFFER);
}

void cWapServer::Header(int status, char* title, char* mime_type, char* realm, size_t size, int refresh)
{
  time_t now;
  char timebuf[100];

  Reply("%s %d %s\n", PROTOCOL, status, title);
  Reply("Server: %s\n", SERVER_NAME);
  now = time((time_t*)0);
  struct tm gmt;
  gmtime_r(&now, &gmt);
  strftime(timebuf, sizeof(timebuf), RFC1123FMT, &gmt);
  Reply("Date: %s\n", timebuf);
  if (mime_type != NULL) {
     Reply("Content-Type: %s\n", mime_type);
     Reply("Content-Length: %d\n", size ? size : strlen(wmlbuffer));
  }
  if (refresh)
     Reply("Refresh: %d\n", refresh);
  if (realm != NULL)
     Reply("WWW-Authenticate: Basic realm=\"%s\"\n", realm);
  Reply("Expires: Thu, 01 Jan 1970 00:00:00 GMT\n");
  Reply("Pragma: no-cache\n");
  Reply("Connection: close\n\n");
}

void cWapServer::Error(int status, char* title, char* text)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s") "%s", title, text);
  ReplyBuffer(WML_A(" " URI_MAIN, "%s"), wmltr("VDR"));
  ReplyBuffer(WML_END());
  Header(status, title, MIME_TYPE, SERVER_NAME);
  FlushBuffer();
}

void cWapServer::SwitchChannel(void)
{
  if (Option("nr")) {
     int n = -1;
     if (isnumber(getopt)) {
        int o = strtol(getopt, NULL, 10);
        if (o >= 1 && o <= Channels.MaxNumber())
           n = o;
     }
     cChannel *channel = Channels.GetByNumber(n);
     if (channel)
        cDevice::PrimaryDevice()->SwitchChannel(channel, true);
  }
  memset(getopt, 0, MYPARSEBUFFER);
  PageMain();
}

void cWapServer::DeleteChannel(void)
{
  if (Option("nr")) {
     if (isnumber(getopt)) {
        cChannel *channel = Channels.GetByNumber(strtol(getopt, NULL, 10));
        if (channel) {
           for (cTimer *timer = Timers.First(); timer; timer = Timers.Next(timer)) {
              if (timer->Channel() == channel) {
                 ReplyBuffer(WML_PROLOGUE());
                 ReplyBuffer(WML_START("%s"), wmltr("Edit channel"));
                 ReplyBuffer("%s" WML_BR() " " WML_A(URI_MAIN, "%s"),
                    wmltr("Channel is being used by a timer!"), wmltr("VDR"));
                 ReplyBuffer(WML_A_KEY("", URI_CHANNELS "/?offset=%d", "0", "%s"),
                    offsetPageChannels, wmltr("Channels"));
                 ReplyBuffer(WML_END());
                 Header(200, "OK", MIME_TYPE, SERVER_NAME);
                 FlushBuffer();
                 return;
              }
           }
           Channels.Del(channel);
           Channels.ReNumber();
           Channels.SetModified();
        }
     }
  }
  memset(getopt, 0, MYPARSEBUFFER);
  PageChannels();
}

void cWapServer::UpdateChannel(void)
{
  Option("nr");
  int nr = strtol(getopt, NULL, 10);
  if (Option("settings")) {
     unurilize(getopt);
     cChannel ch;
     if (ch.Parse(getopt)) {
        if (nr) {
           cChannel *channel = Channels.GetByNumber(nr);
           if (Channels.HasUniqueChannelID(&ch, channel)) {
              *channel = ch;
              Channels.ReNumber();
              Channels.SetModified();
           }
        } else {
           if (Channels.HasUniqueChannelID(&ch)) {
              cChannel *channel = new cChannel;
              *channel = ch;
              Channels.Add(channel);
              Channels.ReNumber();
              Channels.SetModified();
           }
        }
     }
  }
  PageChannels();
}

void cWapServer::OnOffTimer(void)
{
  if (Option("id")) {
     if (isnumber(getopt)) {
        cTimer *timer = Timers.Get(strtol(getopt, NULL, 10) - 1);
        if (timer)
           timer->OnOff();
     }
  }
  memset(getopt, 0, MYPARSEBUFFER);
  PageTimers();
}

void cWapServer::DeleteTimer(void)
{
  if (Option("id")) {
     if (isnumber(getopt)) {
        cTimer *timer = Timers.Get(strtol(getopt, NULL, 10) - 1);
        if (timer) {
           if (!timer->Recording()) {
              Timers.Del(timer);
              Timers.Save();
           } else {
              ReplyBuffer(WML_PROLOGUE());
              ReplyBuffer(WML_START("%s"), wmltr("Edit timer"));
              ReplyBuffer("%s" WML_BR() " " WML_A(URI_MAIN, "%s"),
                 wmltr("Timer is recording!"), wmltr("VDR"));
              ReplyBuffer(WML_A_KEY("", URI_TIMERS "/?offset=%d", "0", "%s"),
                 offsetPageTimers, wmltr("Timers"));
              ReplyBuffer(WML_END());
              Header(200, "OK", MIME_TYPE, SERVER_NAME);
              FlushBuffer();
              return;
           }
        }
     }
  }
  memset(getopt, 0, MYPARSEBUFFER);
  PageTimers();
}

void cWapServer::UpdateTimer(void)
{
  Option("id");
  int id = strtol(getopt, NULL, 10);
  if (Option("settings")) {
     unurilize(getopt);
     cTimer *timer = new cTimer;
     if (timer->Parse(getopt)) {
        cTimer *t;
        if (id)
           t = Timers.Get(id - 1);
        else
           t = Timers.GetTimer(timer);
        if (t) {
           t->Parse(getopt);
           delete timer;
           timer = t;
        } else
           Timers.Add(timer);
        Timers.Save();
        PageTimers();
        return;
     }
     delete timer;
  }
  PageTimers();
}

void cWapServer::DeleteRecording(void)
{
  if (Option("id")) {
     if (isnumber(getopt)) {
        cRecording *recording = Recordings.Get(strtol(getopt, NULL, 10) - 1);
        if (recording)
           recording->Delete();
     }
  }
  memset(getopt, 0, MYPARSEBUFFER);
  PageRecordings();
}

void cWapServer::SetLanguage(void)
{
#if APIVERSNUM >= 10507

#else
  if (Option("id")) {
     if (isnumber(getopt)) {
        Setup.OSDLanguage = strtol(getopt, NULL, 10);
        cFont::SetCode(I18nCharSets()[Setup.OSDLanguage]);
     }
  }
#endif
  PageMain();
}

void cWapServer::EditChannel(void)
{
  time_t base = time(NULL);
  char *totext = NULL;
  char *name = NULL;;
  char *frequency = NULL;
  char *parameters = NULL;
  char *source = NULL;
  char *srate = NULL;
  char *vpid = NULL;
  char *apid = NULL;
  char *tpid = NULL;
  char *ca = NULL;
  char *sid = NULL;
  char *nid = NULL;
  char *tid = NULL;
  char *rid = NULL;
  cChannel *channel = NULL;
  int channel_nr = 0;
  if (Option("nr")) {
     if (isnumber(getopt)) {
        channel_nr = strtol(getopt, NULL, 10);
        channel = Channels.GetByNumber(channel_nr);
        if (channel) {
           totext = strdup(channel->ToText());
           char *tok;
           name = wmlescape(strdup(strtok_r(totext, ":", &tok)));
           frequency = strtok_r(NULL, ":", &tok);
           parameters = strtok_r(NULL, ":", &tok);
           source = strtok_r(NULL, ":", &tok);
           srate = strtok_r(NULL, ":", &tok);
           vpid = strtok_r(NULL, ":", &tok);
           apid = strtok_r(NULL, ":", &tok);
           tpid = strtok_r(NULL, ":", &tok);
           ca = strtok_r(NULL, ":", &tok);
           sid = strtok_r(NULL, ":", &tok);
           nid = strtok_r(NULL, ":", &tok);
           tid = strtok_r(NULL, ":", &tok);
           rid = strtok_r(NULL, "\n", &tok);
        }
     }
  }
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Edit channel"));
  ReplyBuffer("%s:" WML_BR() WML_INPUT("Name%d", "%s", "*M") WML_BR()
     "%s:" WML_BR() WML_INPUT("Freq%d", "%s", "*N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Para%d", "%s", "MM*N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Src%d", "%s", "*M") WML_BR()
     "%s:" WML_BR() WML_INPUT("Srat%d", "%s", "*N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Vpid%d", "%s", "*N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Apid%d", "%s", "*M") WML_BR()
     "%s:" WML_BR() WML_INPUT("Tpid%d", "%s", "*N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Ca%d", "%s", "N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Sid%d", "%s", "*N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Nid%d", "%s", "*N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Tid%d", "%s", "*N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Rid%d", "%s", "N") WML_BR(),
     wmltr("Name"), base, name ? name : wmltr("Button$New"),
     wmltr("Frequency"), base, frequency ? frequency : "",
     wmltr("Parameters"), base, parameters ? parameters : "",
     wmltr("Source"), base, source ? source : "",
     wmltr("Srate"), base, srate ? srate : "",
     wmltr("Vpid"), base, vpid ? vpid : "",
     wmltr("Apid"), base, apid ? apid : "",
     wmltr("Tpid"), base, tpid ? tpid : "",
     wmltr("CA"), base, ca ? ca : "0",
     wmltr("Sid"), base, sid ? sid : "",
     wmltr("Nid"), base, nid ? nid : "1",
     wmltr("Tid"), base, tid ? tid : "",
     wmltr("Rid"), base, rid ? rid : "0");
  free(totext);
  ReplyBuffer(WML_A(" " URI_MAIN, "%s"), wmltr("VDR"));
  ReplyBuffer(WML_A_KEY("", URI_CHANNELS "/?offset=%d", "0", "%s"),
     offsetPageChannels, wmltr("Channels"));
  if (channel) {
     ReplyBuffer(WML_A_KEY("", URI_UPDATECHANNEL "/?offset=%d&amp;nr=%d&amp;settings="
        "$(Name%d):$(Freq%d):$(Para%d):$(Src%d):$(Srat%d):$(Vpid%d):$(Apid%d):"
        "$(Tpid%d):$(Ca%d):$(Sid%d):$(Nid%d):$(Tid%d):$(Rid%d)", "#", "%s"),
        offsetPageChannels, channel_nr, base, base, base, base, base, base, base, base,
        base, base, base, base, base, wmltr("Save"));
     ReplyBuffer(" " WML_A(URI_SCHEDULE "/?offset=%d&amp;channel_nr=%d", "%s"),
        offsetPageScheduleChannel, channel_nr, wmltr("Schedule"));
     ReplyBuffer(WML_A_KEY("", URI_SWITCHCHANNEL "/?nr=%d", "*", "%s"),
        channel_nr, wmltr("Button$Switch"));
     ReplyBuffer(" " WML_A(URI_DELETECHANNEL "/?nr=%d", "%s"),
        channel_nr, wmltr("Button$Delete"));
  } else
     ReplyBuffer(WML_A_KEY("", URI_UPDATECHANNEL "/?offset=%d&amp;settings="
        "$(Name%d):$(Freq%d):$(Para%d):$(Src%d):$(Srat%d):$(Vpid%d):$(Apid%d):"
        "$(Tpid%d):$(Ca%d):$(Sid%d):$(Nid%d):$(Tid%d):$(Rid%d)", "#", "%s"),
        offsetPageChannels, base, base, base, base, base, base, base, base, base, base,
        base, base, base, wmltr("Save"));
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
}

void cWapServer::EditTimer(void)
{
  time_t base = time(NULL);
  struct tm lt;
  localtime_r(&base, &lt);
  int flags = 1;
  int channel_nr = cDevice::CurrentChannel();
  char *day = NULL;
  int start = 100 * lt.tm_hour + lt.tm_min;
  int stop = start;
  int priority = Setup.DefaultPriority;
  int lifetime = Setup.DefaultLifetime;
  char *file = NULL;
  asprintf(&day, "%d", lt.tm_mday);
  asprintf(&file, "%s%02d%02d%02d", wmltr("Button$New"), lt.tm_hour, lt.tm_min, lt.tm_sec);
  cTimer *timer = NULL;
  if (Option("id")) {
     if (isnumber(getopt)) {
        timer = Timers.Get(strtol(getopt, NULL, 10) - 1);
        if (timer) {
           flags = timer->Flags();
           channel_nr = timer->Channel()->Number();
#if APIVERSNUM >= 10503
           day = strdup(timer->PrintDay(timer->Day(), timer->WeekDays(), true));
#else
           day = strdup(timer->PrintDay(timer->Day(), timer->WeekDays()));
#endif
           start = timer->Start();
           stop = timer->Stop();
           priority = timer->Priority();
           lifetime = timer->Lifetime();
           file = wmlescape(strdup(timer->File()));
        }
     }
  }
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Edit timer"));
  ReplyBuffer("%s:" WML_BR() WML_INPUT("Chan%d", "%d", "*N") WML_BR()
     "%s:" WML_BR() WML_INPUT("Day%d", "%s", "*M") WML_BR()
     "%s:" WML_BR() WML_INPUT("Strt%d", "%02d%02d", "NNNN") WML_BR()
     "%s:" WML_BR() WML_INPUT("Stop%d", "%02d%02d", "NNNN") WML_BR()
     "%s:" WML_BR() WML_INPUT("Prio%d", "%d", "NN") WML_BR()
     "%s:" WML_BR() WML_INPUT("Life%d", "%d", "NN") WML_BR()
     "%s:" WML_BR() WML_INPUT("File%d", "%s", "*M") WML_BR(),
     wmltr("Channel"), base, channel_nr,
     wmltr("Day"), base, day,
     wmltr("Start"), base, start / 100, start % 100,
     wmltr("Stop"), base, stop / 100, stop %100,
     wmltr("Priority"), base, priority,
     wmltr("Lifetime"), base, lifetime,
     wmltr("File"), base, file);
  free(day);
  free(file);
  ReplyBuffer(WML_A(" " URI_MAIN, "%s"), wmltr("VDR"));
  ReplyBuffer(WML_A_KEY("", URI_TIMERS "/?offset=%d", "0", "%s"),
     offsetPageTimers, wmltr("Timers"));
  if (timer) {
     int id = strtol(getopt, NULL, 10);
     ReplyBuffer(WML_A_KEY("", URI_UPDATETIMER "/?offset=%d&amp;id=%d&amp;settings="
        "%d:$(Chan%d):$(Day%d):$(Strt%d):$(Stop%d):$(Prio%d):$(Life%d):$(File%d):", "#", "%s"),
        offsetPageTimers, id, flags, base, base, base, base, base, base, base, wmltr("Save"));
     ReplyBuffer(" " WML_A(URI_DELETETIMER "/?id=%d", "%s"),
        id, wmltr("Button$Delete"));
     ReplyBuffer(WML_A_KEY("", URI_ONOFFTIMER "/?id=%d", "*", "%s"),
        id, wmltr("Button$On/Off"));
  } else
     ReplyBuffer(WML_A_KEY("", URI_UPDATETIMER "/?offset=%d&amp;settings="
        "%d:$(Chan%d):$(Day%d):$(Strt%d):$(Stop%d):$(Prio%d):$(Life%d):$(File%d):", "#", "%s"),
        offsetPageTimers, flags, base, base, base, base, base, base, base, wmltr("Save"));
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
}

void cWapServer::EditRecording(void)
{
  int id = -1;
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Recording info"));
  cRecording *first = NULL;
  cRecording *prev = NULL;
  cRecording *next = NULL;
  cRecording *last = NULL;
  if (Option("id")) {
     if (isnumber(getopt)) {
        cRecording *recording = Recordings.Get(strtol(getopt, NULL, 10) - 1);
        if (recording) {
           first = Recordings.First();
           prev = Recordings.Prev(recording);
           next = Recordings.Next(recording);
           last = Recordings.Last();
           char *title = recording->Info()->Title()
              ?  wmlescape(strdup(recording->Info()->Title()))
              :  NULL;
           char *shtxt = recording->Info()->ShortText()
              ?  wmlescape(strdup(recording->Info()->ShortText()))
              :  NULL;
           char *descr = recording->Info()->Description()
              ?  wmlescape(strdup(recording->Info()->Description()))
              :  NULL;
           char *rec = wmlescape(strdup(recording->Title('|', true, recording->HierarchyLevels())));
           char *tok;
           char *date = strtok_r(rec, "|", &tok);
           char *time = strtok_r(NULL, "|", &tok);
           ReplyBuffer("%s %s" WML_BR() "%s" WML_BR() "%s" WML_BR() "%s" WML_BR(),
              date ? date : "-",
              time ? time : "-",
              title ? title : wmltr("no title"),
              shtxt ? shtxt : "",
              descr ? descr : wmltr("no description"));
           free(title);
           free(shtxt);
           free(descr);
           free(rec);
           id = strtol(getopt, NULL, 10);
        } else
           ReplyBuffer("%s" WML_BR(), wmltr("Error in recording id."));
     } else
        ReplyBuffer("%s" WML_BR(), wmltr("Error in recording id."));
  } else
     ReplyBuffer("%s" WML_BR(), wmltr("Error in recording id."));
  ReplyBuffer(" " WML_A(URI_MAIN, "%s"), wmltr("VDR"));
  ReplyBuffer(WML_A_KEY("", URI_RECORDINGS "/?offset=%d", "0", "%s"),
     offsetPageRecordings, wmltr("Recordings"));
  if (id > -1)
     ReplyBuffer(" " WML_A(URI_DELETERECORDING "/?id=%d", "%s"),
        id, wmltr("Button$Delete"));
  if (prev)
     ReplyBuffer(WML_A_KEY("", URI_EDITRECORDING "/?id=%d", "*", "&#060;"), prev->Index() + 1);
  else if (last && Setup.MenuScrollWrap)
     ReplyBuffer(WML_A_KEY("", URI_EDITRECORDING "/?id=%d", "*", "&#060;"), last->Index() + 1);
  if (next)
     ReplyBuffer(WML_A_KEY("", URI_EDITRECORDING "/?id=%d", "#", "&#062;"), next->Index() + 1);
  else if (first && Setup.MenuScrollWrap)
     ReplyBuffer(WML_A_KEY("", URI_EDITRECORDING "/?id=%d", "#", "&#062;"), first->Index() + 1);
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
}

void cWapServer::ScheduleEvent(cChannel* channel, const cSchedules* schedules)
{
  ReplyBuffer(WML_START("%s - %s"), wmltr("Schedule"), channel->Name());
  time_t start = time(NULL);
  time_t stop = time(NULL);
  struct tm tm_start;
  struct tm tm_stop;
  char *file = NULL;
  cEvent *first = NULL;
  cEvent *prev = NULL;
  cEvent *next = NULL;
  cEvent *last = NULL;
  time_t now = time(NULL);
  if (isnumber(getopt)) {
     const cSchedule *Schedule = schedules->GetSchedule(channel->GetChannelID());
     const cEvent *Event = Schedule->GetEvent(strtol(getopt, NULL, 10));
     if (Event) {
        char *title = Event->Title() ? wmlescape(strdup(Event->Title())) : NULL;
        char *shtxt = Event->ShortText() ? wmlescape(strdup(Event->ShortText())) : NULL;
        char *descr = Event->Description() ? wmlescape(strdup(Event->Description())) : NULL;
        file = Event->Title() ? wmlescape(strdup(Event->Title())) : NULL;
        ReplyBuffer("%s %s - %s" WML_BR() "%s" WML_BR() "%s" WML_BR() "%s" WML_BR(),
           *Event->GetDateString(),
           *Event->GetTimeString(), *Event->GetEndTimeString(),
           title ? title : wmltr("no title"),
           shtxt ? shtxt : "",
           descr ? descr : wmltr("no description"));
        free(title);
        free(shtxt);
        free(descr);
        start = Event->StartTime() - Setup.MarginStart * 60;
        stop = Event->EndTime() + Setup.MarginStop * 60;
        for (first = Schedule->Events()->First();
             first && first->EndTime() <= now;
             first = Schedule->Events()->Next(first));
        prev = Schedule->Events()->Prev(Event);
        if (prev->EndTime() <= now) prev = NULL;
        next = Schedule->Events()->Next(Event);
        last = Schedule->Events()->Last();
     } else
        ReplyBuffer(wmltr("Event \"%s\" not found."), getopt);
  } else
     ReplyBuffer(wmltr("Error in event id \"%s\"."), getopt);
  localtime_r(&start, &tm_start);
  localtime_r(&stop, &tm_stop);
  strreplace(file, ':', '|');
  strreplace(file, ' ', '_');
  ReplyBuffer(" " WML_A(URI_MAIN, "%s"), wmltr("VDR"));
  ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?offset=%d&amp;channel_nr=%d", "0", "%s"),
     offsetPageScheduleChannel, channel->Number(), wmltr("Schedule"));
  ReplyBuffer(" " WML_A(URI_SWITCHCHANNEL "/?nr=%d", "%s"),
     channel->Number(), wmltr("Button$Switch"));
  ReplyBuffer(" " WML_A(URI_UPDATETIMER "/?settings=1:%d:%d:%02d%02d:%02d%02d:%d:%d:%s:", "%s"),
     channel->Number(), tm_start.tm_mday, tm_start.tm_hour, tm_start.tm_min,
     tm_stop.tm_hour, tm_stop.tm_min, Setup.DefaultPriority, Setup.DefaultLifetime,
     file, wmltr("Button$Record"));
  if (prev)
     ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?event_id=%d&amp;channel_nr=%d", "*", "&#060;"),
        prev->EventID(), channel->Number());
  else if (last && Setup.MenuScrollWrap)
     ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?event_id=%d&amp;channel_nr=%d", "*", "&#060;"),
        last->EventID(), channel->Number());
  if (next)
     ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?event_id=%d&amp;channel_nr=%d", "#", "&#062;"),
        next->EventID(), channel->Number());
  else if (first && Setup.MenuScrollWrap)
     ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?event_id=%d&amp;channel_nr=%d", "#", "&#062;"),
        first->EventID(), channel->Number());
  free(file);
}

void cWapServer::ScheduleNowNext(cChannel* channel, const cSchedules* schedules)
{
  bool Now = Option("now");
  ReplyBuffer(WML_START("%s - %s"),
     wmltr("Schedule"), Now ? wmltr("What's on now?") : wmltr("What's on next?"));
  Option("offset");
  int offset = strtol(getopt, NULL, 10);
  int i = 0;
  for (channel = Channels.First(); channel; channel = Channels.Next(channel)) {
     if (!channel->GroupSep()) {
        const cSchedule *Schedule = schedules->GetSchedule(channel->GetChannelID());
        if (Schedule) {
           const cEvent *Event = Now ? Schedule->GetPresentEvent() : Schedule->GetFollowingEvent();
           if (Event) {
              if (i >= offset && i < (offset + MAXLINES)) {
                 int k = i - offset + 1;
                 char *chname = wmlescape(strdup(channel->Name()));
                 char *title = Event->Title() ? wmlescape(strdup(Event->Title())) : wmltr("no title");
                 ReplyBuffer("%d %d"
                    WML_A_KEY("",  URI_SCHEDULE "/?offset=%d&amp;channel_nr=%d", "%d", "%s") WML_BR(),
                    k, channel->Number(), offsetPageScheduleChannel, channel->Number(), k, chname);
                 ReplyBuffer(". %s %s" WML_BR(),
                    *Event->GetDateString(), *Event->GetTimeString());
                 ReplyBuffer(". " WML_A(URI_SCHEDULE "/?event_id=%d&amp;channel_nr=%d", "%s") WML_BR(),
                    Event->EventID(), channel->Number(), title);
                 free(chname);
                 free(title);
              }
              i++;
           }
        }
     }
  }
  int page = offset/MAXLINES + 1;
  int pages = (i - 1)/MAXLINES + 1;
  ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
  ReplyBuffer(WML_A(URI_SCHEDULE "/?offset=%d&amp;%s=y", "%s"),
     offsetPageScheduleNowNext, Now ? "next" : "now", Now ? wmltr("Button$Next") : wmltr("Button$Now"));
  if (offset >= MAXLINES)
     ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?offset=%d&amp;%s=y", "*", "&#060;"),
        offset - MAXLINES, Now ? "now" : "next");
  else if (Setup.MenuScrollWrap)
     ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?offset=%d&amp;%s=y", "*", "&#060;"),
        (pages - 1) * MAXLINES, Now ? "now" : "next");
  if (offset + MAXLINES < i)
     ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?offset=%d&amp;%s=y", "#", "&#062;"),
        offset + MAXLINES, Now ? "now" : "next");
  else if (Setup.MenuScrollWrap)
     ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?offset=%d&amp;%s=y", "#", "&#062;"),
        0, Now ? "now" : "next");
  ReplyBuffer(" %d/%d", page, pages);
  offsetPageScheduleNowNext = offset;
}

void cWapServer::ScheduleChannel(cChannel* channel, const cSchedules* schedules)
{
  ReplyBuffer(WML_START("%s - %s"), wmltr("Schedule"), channel->Name());
  const cSchedule *Schedule = schedules->GetSchedule(channel->GetChannelID());
  if (Schedule) {
     Option("offset");
     int offset = strtol(getopt, NULL, 10);
     int i = 0;
     const cEvent *PresentEvent = Schedule->GetPresentEvent();
     time_t now = time(NULL) - Setup.EPGLinger * 60;
     for (const cEvent *Event = Schedule->Events()->First(); Event; Event = Schedule->Events()->Next(Event)) {
        if (Event->EndTime() > now || Event == PresentEvent) {
           if (i >= offset && i < (offset + MAXLINES)) {
              int k = i - offset + 1;
              char *title = Event->Title() ? wmlescape(strdup(Event->Title())) : wmltr("no title");
              ReplyBuffer("%d %s %s " WML_BR(),
                 k, *Event->GetDateString(), *Event->GetTimeString());
              ReplyBuffer(WML_A_KEY(".", URI_SCHEDULE "/?event_id=%d&amp;channel_nr=%d",
                 "%d", "%s") WML_BR(),
                 Event->EventID(), channel->Number(), k, title);
              free(title);
           }
           i++;
        }
     }
     int page = offset/MAXLINES + 1;
     int pages = (i - 1)/MAXLINES + 1;
     ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
     ReplyBuffer(WML_A(URI_SCHEDULE "/?offset=%d&amp;now=y", "%s") " ",
        offsetPageScheduleNowNext, wmltr("Button$Now"));
     ReplyBuffer(WML_A(URI_SCHEDULE "/?offset=%d&amp;next=y", "%s"),
        offsetPageScheduleNowNext, wmltr("Button$Next"));
     if (offset >= MAXLINES)
        ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?offset=%d&amp;channel_nr=%d", "*", "&#060;"),
           offset - MAXLINES, channel->Number());
     else if (Setup.MenuScrollWrap)
        ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?offset=%d&amp;channel_nr=%d", "*", "&#060;"),
           (pages - 1) * MAXLINES, channel->Number());
     if (offset + MAXLINES < i)
        ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?offset=%d&amp;channel_nr=%d", "#", "&#062;"),
           offset + MAXLINES, channel->Number());
     else if (Setup.MenuScrollWrap)
        ReplyBuffer(WML_A_KEY("", URI_SCHEDULE "/?offset=%d&amp;channel_nr=%d", "#", "&#062;"),
           0, channel->Number());
     ReplyBuffer(" %d/%d", page, pages);
     offsetPageScheduleChannel = offset;
  } else {
     ReplyBuffer("%s" WML_BR(), wmltr("No schedule available."));
     ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
     ReplyBuffer(WML_A(URI_SCHEDULE "/?offset=%d&amp;now=y", "%s") " ",
        offsetPageScheduleNowNext, wmltr("Button$Now"));
     ReplyBuffer(WML_A(URI_SCHEDULE "/?offset=%d&amp;next=y", "%s"),
        offsetPageScheduleNowNext, wmltr("Button$Next"));
  }
}

void cWapServer::PageSchedule(void)
{
  int ChannelNr;
  if (Option("channel_nr") && isnumber(getopt))
     ChannelNr = strtol(getopt, NULL, 10);
  else
     ChannelNr = cDevice::CurrentChannel();
  cChannel* Channel = Channels.GetByNumber(ChannelNr);

  ReplyBuffer(WML_PROLOGUE());

  cSchedulesLock schedulesLock;
  const cSchedules *Schedules = cSchedules::Schedules(schedulesLock);
  if (Schedules) {
     if (Option("event_id"))
        ScheduleEvent(Channel, Schedules);
     else if (Option("now") || Option("next"))
        ScheduleNowNext(Channel, Schedules);
     else if (Channel)
        ScheduleChannel(Channel, Schedules);
     else {
        ReplyBuffer(WML_START("%s"), wmltr("Schedule"));
        ReplyBuffer(wmltr("Channel %d not defined."), ChannelNr);
        ReplyBuffer(WML_BR() WML_A_KEY("", URI_MAIN, "0", "%s"), wmltr("VDR"));
     }
  } else
     ReplyBuffer("%s" WML_BR() WML_A_KEY("", URI_MAIN, "0", "%s"),
        wmltr("Can't get EPG data."), wmltr("VDR"));
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
}

void cWapServer::PageChannels(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Channels"));
  Option("offset");
  int offset = strtol(getopt, NULL, 10);
  if (Channels.MaxNumber() >= 1) {
     int i = 0;
     for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
        if (!channel->GroupSep()) {
           if (i >= offset && i < (offset + MAXLINES)) {
              int k = i - offset + 1;
              char *name = wmlescape(strdup(channel->Name()));
              ReplyBuffer(WML_A_KEY("%d %d", URI_EDITCHANNEL "/?nr=%d", "%d", "%s") WML_BR(),
                 k, channel->Number(), channel->Number(), k, name);
              free(name);
           }
           i++;
        }
     }
     int page = offset/MAXLINES + 1;
     int pages = (i - 1)/MAXLINES + 1;
     ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
     ReplyBuffer(WML_A(URI_EDITCHANNEL, "%s"), wmltr("Button$New"));
     if (offset >= MAXLINES)
        ReplyBuffer(WML_A_KEY("", URI_CHANNELS "/?offset=%d", "*", "&#060;"), offset - MAXLINES);
     else if (Setup.MenuScrollWrap)
        ReplyBuffer(WML_A_KEY("", URI_CHANNELS "/?offset=%d", "*", "&#060;"), (pages - 1) * MAXLINES);
     if (offset + MAXLINES < i)
        ReplyBuffer(WML_A_KEY("", URI_CHANNELS "/?offset=%d", "#", "&#062;"), offset + MAXLINES);
     else if (Setup.MenuScrollWrap)
        ReplyBuffer(WML_A_KEY("", URI_CHANNELS "/?offset=%d", "#", "&#062;"), 0);
     ReplyBuffer(" %d/%d", page, pages);
  } else {
     ReplyBuffer("%s" WML_BR(), wmltr("No channels defined."));
     ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
  }
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
  offsetPageChannels = offset;
}

void cWapServer::PageTimers(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Timers"));
  Option("offset");
  int offset = strtol(getopt, NULL, 10);
  if (Timers.Count()) {
     int i;
     for (i = 0; i < Timers.Count(); i++) {
        cTimer *timer = Timers.Get(i);
        if (i >= offset & i < (offset + MAXLINES)) {
           int k = i - offset + 1;
           if (timer) {
              char *file = wmlescape(strdup(timer->File()));
              ReplyBuffer("%d %02d:%02d %02d:%02d %d %s %s" WML_BR(),
                 k,
                 timer->Start() / 100, timer->Start() % 100,
                 timer->Stop() / 100, timer->Stop() % 100,
                 timer->Channel()->Number(),
#if APIVERSNUM >= 10503
                 *timer->PrintDay(timer->Day(), timer->WeekDays(), true),
#else
                 *timer->PrintDay(timer->Day(), timer->WeekDays()),
#endif
                 !(timer->HasFlags(tfActive))
                    ?  " "
                    :  timer->FirstDay()
                    ?  "!"
                    :  timer->Recording()
                    ?  "#"
                    : "&#062;");
              ReplyBuffer(WML_A_KEY(".", URI_EDITTIMER "/?id=%d", "%d", "%s") WML_BR(),
                 timer->Index() + 1, k, file ? file : wmltr("no title"));
              free(file);
           } else {
              ReplyBuffer(wmltr("Timer \"%d\" not found."), i + 1);
              ReplyBuffer(WML_BR());
           }
        }
     }
     int page = offset/MAXLINES + 1;
     int pages = (i - 1)/MAXLINES + 1;
     ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
     ReplyBuffer(WML_A(URI_EDITTIMER, "%s"), wmltr("Button$New"));
     if (offset >= MAXLINES)
        ReplyBuffer(WML_A_KEY("", URI_TIMERS "/?offset=%d", "*", "&#060;"), offset - MAXLINES);
     else if (Setup.MenuScrollWrap)
        ReplyBuffer(WML_A_KEY("", URI_TIMERS "/?offset=%d", "*", "&#060;"), (pages - 1) * MAXLINES);
     if (offset + MAXLINES < i)
        ReplyBuffer(WML_A_KEY("", URI_TIMERS "/?offset=%d", "#", "&#062;"), offset + MAXLINES);
     else if (Setup.MenuScrollWrap)
        ReplyBuffer(WML_A_KEY("", URI_TIMERS "/?offset=%d", "#", "&#062;"), 0);
     ReplyBuffer(" %d/%d", page, pages);
  } else {
     ReplyBuffer("%s" WML_BR(), wmltr("No timers defined."));
     ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
     ReplyBuffer(WML_A(URI_EDITTIMER, "%s"), wmltr("Button$New"));
  }
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
  offsetPageTimers = offset;
}

void cWapServer::PageRecordings(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Recordings"));
  Option("offset");
  int offset = strtol(getopt, NULL, 10);
  bool recordings = Recordings.Load();
  if (recordings) {
     Recordings.Sort();
     int i = 0;
     for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording)) {
        int level = recording->HierarchyLevels();
        if (i >= offset && i < (offset + MAXLINES)) {
           int k = i - offset + 1;
           char *title = recording->Info()->Title()
              ?  wmlescape(strdup(recording->Info()->Title()))
              :  NULL;
           char *rec = wmlescape(strdup(recording->Title('|', true, level)));
           char *tok;
           char *date = strtok_r(rec, "|", &tok);
           char *time = strtok_r(NULL, "|", &tok);
           ReplyBuffer("%d %s %s" WML_BR(),
              k, date ? date : "-", time ? time : "-");
           ReplyBuffer(WML_A_KEY(".", URI_EDITRECORDING "/?id=%d", "%d", "%s") WML_BR(),
              recording->Index() + 1, k, title ? title : wmltr("no title"));
           free(title);
           free(rec);
        }
        i++;
     }
     int page = offset/MAXLINES + 1;
     int pages = (i - 1)/MAXLINES + 1;
     ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
     if (offset >= MAXLINES)
        ReplyBuffer(WML_A_KEY("", URI_RECORDINGS "/?offset=%d", "*", "&#060;"), offset - MAXLINES);
     else if (Setup.MenuScrollWrap)
        ReplyBuffer(WML_A_KEY("", URI_RECORDINGS "/?offset=%d", "*", "&#060;"), (pages - 1) * MAXLINES);
     if (offset + MAXLINES < i)
        ReplyBuffer(WML_A_KEY("", URI_RECORDINGS "/?offset=%d", "#", "&#062;"), offset + MAXLINES);
     else if (Setup.MenuScrollWrap)
        ReplyBuffer(WML_A_KEY("", URI_RECORDINGS "/?offset=%d", "#", "&#062;"), 0);
     ReplyBuffer(" %d/%d", page, pages);
  } else {
     ReplyBuffer("%s" WML_BR(), wmltr("No recordings available."));
     ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
  }
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
  offsetPageRecordings = offset;
}

void cWapServer::PageLanguage(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Setup.OSD$Language"));
  Option("offset");
  int offset = strtol(getopt, NULL, 10);
  int i;
#if APIVERSNUM >= 10507
  ReplyBuffer("Not implemented yet." WML_BR());
  i = 1;
#else
  const char * const *lang = I18nLanguages();
  for (i = 0; i < I18nNumLanguages; i++) {
     if (i >= offset && i < (offset + MAXLINES)) {
        int k = i - offset + 1;
        char *l = wmlescape(strdup(lang[i]));
        ReplyBuffer(WML_A_KEY("%d", URI_SETLANGUAGE "/?id=%d", "%d", "%s") WML_BR(),
           k, i, k, l);
        free(l);
     }
  }
#endif
  int page = offset/MAXLINES + 1;
  int pages = (i - 1)/MAXLINES + 1;
  ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
  if (offset >= MAXLINES)
     ReplyBuffer(WML_A_KEY("", URI_LANGUAGE "/?offset=%d", "*", "&#060;"), offset - MAXLINES);
  else if (Setup.MenuScrollWrap)
     ReplyBuffer(WML_A_KEY("", URI_LANGUAGE "/?offset=%d", "*", "&#060;"), (pages - 1) * MAXLINES);
  if (offset + MAXLINES < i)
     ReplyBuffer(WML_A_KEY("", URI_LANGUAGE "/?offset=%d", "#", "&#062;"), offset + MAXLINES);
  else if (Setup.MenuScrollWrap)
     ReplyBuffer(WML_A_KEY("", URI_LANGUAGE "/?offset=%d", "#", "&#062;"), 0);
  ReplyBuffer(" %d/%d", page, pages);
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
  offsetPageLanguage = offset;
}

void cWapServer::PageGrabImage(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Image"));

  int size;
  char *img = (char*)cDevice::PrimaryDevice()->GrabImage(size, true, 100, 128, 128);

  if (img) {
     if (size < WMLBUFFER) {
        memcpy(wmlbuffer, img, size);
        Header(200, "OK", "image/jpeg", SERVER_NAME, size, 5);
        FlushBuffer(size);
        return;
     } else
        ReplyBuffer(wmltr("Image size too big."));
  } else
     ReplyBuffer(wmltr("No image available."));
  ReplyBuffer(WML_A_KEY("", URI_MAIN, "0", "%s") " ", wmltr("VDR"));
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
}

void cWapServer::PageMain(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("VDR"));
  ReplyBuffer(WML_A_KEY("1", URI_SCHEDULE "/?offset=%d", "1", "%s") WML_BR(),
     offsetPageScheduleChannel, wmltr("Schedule"));
  ReplyBuffer(WML_A_KEY("2", URI_CHANNELS "/?offset=%d", "2", "%s") WML_BR(),
     offsetPageChannels,  wmltr("Channels"));
  ReplyBuffer(WML_A_KEY("3", URI_TIMERS "/?offset=%d", "3", "%s") WML_BR(),
     offsetPageTimers, wmltr("Timers"));
  ReplyBuffer(WML_A_KEY("4", URI_RECORDINGS "/?offset=%d", "4", "%s") WML_BR(),
     offsetPageRecordings, wmltr("Recordings"));
  ReplyBuffer(WML_A_KEY("5", URI_LANGUAGE "/?offset=%d", "5", "%s") WML_BR(),
     offsetPageLanguage, wmltr("Setup.OSD$Language"));
  ReplyBuffer(WML_A_KEY("6", URI_GRABIMAGE, "6", "%s") WML_BR(),
     wmltr("Image"));
  ReplyBuffer(WML_END());
  Header(200, "OK", MIME_TYPE, SERVER_NAME);
  FlushBuffer();
}

bool cWapServer::ProcessHeader(void)
{
  char request[MYPARSEBUFFER];
  char line[MYPARSEBUFFER];

  method = mUnknown;
  memset(uri, 0, MYPARSEBUFFER);
  memset(path, 0, MYPARSEBUFFER);
  memset(opt, 0, MYPARSEBUFFER);
  memset(getopt, 0, MYPARSEBUFFER);
  memset(host, 0, MYPARSEBUFFER);
  memset(authorization, 0, MYPARSEBUFFER);

  ssize_t length = safe_read(file, request, sizeof(request));

  if (length > 0) {
     ssize_t position = strcspn(request, "\r\n");
     ssize_t oldpos;
     strncpy(line, request, position);
     line[position] = 0;
     if (strncasecmp(line, "GET", 3) == 0) {
        method = mGet;
        oldpos = 4;
     } else if (strncasecmp(line, "POST", 4) == 0) {
        method = mPost;
        oldpos = 5;
     } else {
        Error(501, "Not implemented", "Method not implemented");
        return false;
     }
     line[strcspn(line + oldpos, " \t") + oldpos] = 0;
     strcpy(uri, line + oldpos);
     strcpy(path, uri + 2 + strcspn(uri + 1, "/"));
     uri[1 + strcspn(uri + 1, "/")] = 0;
     strcpy(opt, path + 1 + strcspn(path, "?"));
     path[strcspn(path, "?")] = 0;
     oldpos = position;
     while (position < length) {
        while (iscntrl(request[position++]) && position < length) ;
        oldpos = position - 1;
        while (!iscntrl(request[position++]) && position < length) ;
        if (position > oldpos) {
           strncpy(line, request + oldpos, position - oldpos -1);
           line[position - oldpos - 1] = 0;
           if (strncasecmp(line, "Host: ", 6) == 0)
              strcpy(host, line + 6);
           else if (strncasecmp(line, "Authorization: Basic ", 21) == 0) {
              strcpy(authorization, line + 21);
              base64decode(authorization);
           }
        }
     }
#ifdef WAPD_DEBUG
     dsyslog("WAPD: uri = '%s'", uri);
     dsyslog("WAPD: path = '%s'", path);
     dsyslog("WAPD: opt = '%s'", opt);
     dsyslog("WAPD: host = '%s'", host);
     dsyslog("WAPD: authorization = '%s'", authorization);
#endif // WAPD_DEBUG
  } else if (length < 0) {
     isyslog("WAPD: error receiving from HTTP client");
     return false;
  }
  return true;
}

bool cWapServer::Option(const char *Name)
{
  ssize_t olen = strlen(opt);
  ssize_t nlen = strlen(Name);

  if (olen > 0) {
     ssize_t pos = 0;
     while (pos < olen) {
        ssize_t len = strcspn(opt + pos, "&");
        strncpy(getopt, opt + pos, len);
        getopt[len] = 0;
        if (strncmp(getopt, Name, nlen) == 0 && getopt[nlen] == '=') {
           strcpy(getopt, getopt + nlen + 1);
           return true;
        }
        pos += len + 1;
     }
  }
  return false;
}

void cWapServer::Action(void)
{
  usleep(5000000); //initial delay
  isyslog("WAPD: finished initial delay");
  while (1) {
     if (file.Open(socket.Accept())) {
        if (ProcessHeader()) {
           if (*authorization == 0 || !WapAccess.Acceptable(authorization))
              Header(401, "Unauthorized", MIME_TYPE, SERVER_NAME);
           else if (strncmp(uri, URI_SCHEDULE, sizeof(URI_SCHEDULE)) == 0)
              PageSchedule();
           else if (strncmp(uri, URI_CHANNELS, sizeof(URI_CHANNELS)) == 0)
              PageChannels();
           else if (strncmp(uri, URI_TIMERS, sizeof(URI_TIMERS)) == 0)
              PageTimers();
           else if (strncmp(uri, URI_RECORDINGS, sizeof(URI_RECORDINGS)) == 0)
              PageRecordings();
           else if (strncmp(uri, URI_LANGUAGE, sizeof(URI_LANGUAGE)) == 0)
              PageLanguage();
           else if (strncmp(uri, URI_GRABIMAGE, sizeof(URI_GRABIMAGE)) == 0)
              PageGrabImage();
           else if (strncmp(uri, URI_MAIN, sizeof(URI_MAIN)) == 0)
              PageMain();
           else if (strncmp(uri, URI_EDITCHANNEL, sizeof(URI_EDITCHANNEL)) == 0)
              EditChannel();
           else if (strncmp(uri, URI_EDITTIMER, sizeof(URI_EDITTIMER)) == 0)
              EditTimer();
           else if (strncmp(uri, URI_EDITRECORDING, sizeof(URI_EDITRECORDING)) == 0)
              EditRecording();
           else if (strncmp(uri, URI_SWITCHCHANNEL, sizeof(URI_SWITCHCHANNEL)) == 0)
              SwitchChannel();
           else if (strncmp(uri, URI_DELETECHANNEL, sizeof(URI_DELETECHANNEL)) == 0)
              DeleteChannel();
           else if (strncmp(uri, URI_UPDATECHANNEL, sizeof(URI_UPDATECHANNEL)) == 0)
              UpdateChannel();
           else if (strncmp(uri, URI_ONOFFTIMER, sizeof(URI_ONOFFTIMER)) == 0)
              OnOffTimer();
           else if (strncmp(uri, URI_DELETETIMER, sizeof(URI_DELETETIMER)) == 0)
              DeleteTimer();
           else if (strncmp(uri, URI_UPDATETIMER, sizeof(URI_UPDATETIMER)) == 0)
              UpdateTimer();
           else if (strncmp(uri, URI_DELETERECORDING, sizeof(URI_DELETERECORDING)) == 0)
              DeleteRecording();
           else if (strncmp(uri, URI_SETLANGUAGE, sizeof(URI_SETLANGUAGE)) == 0)
              SetLanguage();
           else
              Error(404, "Not Found", "URL not found");
        }
        file.Close();
     }
     usleep(10000);
  }
}

