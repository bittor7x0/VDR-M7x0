/*
 * WAP remote control plugin for VDR
 * server.c: wapd server core
 *
 * See the README file for copyright information and how to reach the author.
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */


#include <ctype.h>
#include <stdarg.h>
#include <vdr/tools.h>
#include <vdr/i18n.h>
#include <vdr/recording.h>
#include <vdr/font.h>
#include <vdr/device.h>
#include "server.h"
#include "tools.h"
#include "i18n.h"

#define MAXLINES                   10

#define PROTOCOL                   "HTTP/1.0"
#define SERVER_NAME                "VDR WAPD"
#define RFC1123FMT                 "%a, %d %b %Y %H:%M:%S GMT"
#define MIME_WAP                   "text/vnd.wap.wml"

#define URI_MAIN                   "/"
#define URI_SCHEDULE               "/schedule"
#define URI_CHANNELS               "/channels"
#define URI_TIMERS                 "/timers"
#define URI_RECORDINGS             "/recordings"
#define URI_LANGUAGE               "/language"
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

#define WML_PROLOGUE()             "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n" \
                                   "<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.3//EN\" " \
                                   "\"http://www.wapforum.org/DTD/wml13.dtd\">\n"
#define WML_START(s)               "<wml><card title=\"" s "\"><p>\n"
#define WML_END(s)                 "</p>\n" s "</card></wml>\n"
#define WML_BR()                   "<br/>\n"
#define WML_A(href,name)           "<a href=\"" href "\">" name "</a>"
#define WML_ANCHOR(name,href)      "<anchor>" name "<go href=\"" href "\">\n"
#define WML_ENDANCHOR()            "</go></anchor>\n"
#define WML_DO(lbl,href)           "<do label=\"" lbl "\" type=\"unknown\">" \
                                   "<go href=\"" href "\">\n"
#define WML_ENDDO()                "</go></do>\n"
#define WML_POSTFIELD(name, value) "<postfield name=\"" name "\" value=\"" value "\"/>\n"
#define WML_INPUT(name, value, fmt) \
                    "<input name=\"" name "\" value=\"" value "\" format=\"" fmt "\"/>\n"
#define WML_LT                     "&#060;"
#define WML_GT                     "&#062;"


// --- cWapServer ------------------------------------------------------------

cWapServer::cWapServer(int Port) : cThread(), socket(Port)
{
  buffer = NULL;
  memset(wmlbuffer, 0, WMLBUFFER);
  Start();
}

cWapServer::~cWapServer()
{
  Cancel(3);
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

void cWapServer::FlushBuffer(void)
{
  if (file.IsOpen())
     Send(wmlbuffer);
  memset(wmlbuffer, 0, WMLBUFFER);
}

void cWapServer::Header(int status, char* title, char* mime_type, char* realm)
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
     Reply("Content-Length: %d\n", strlen(wmlbuffer));
     }
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
  ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()), wmltr("VDR"));
  Header(status, title, MIME_WAP);
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
  memset(getopt, 0, MAXPARSEBUFFER);
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
                 ReplyBuffer(wmltr("Channel is being used by a timer!"));
                 ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
                    WML_DO("%s", URI_CHANNELS) WML_ENDDO()),
                    wmltr("VDR"), wmltr("Channels"));
                 Header(200, "OK", MIME_WAP);
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
  memset(getopt, 0, MAXPARSEBUFFER);
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
           }
        else {
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
  memset(getopt, 0, MAXPARSEBUFFER);
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
              }
           else {
              ReplyBuffer(WML_PROLOGUE());
              ReplyBuffer(WML_START("%s"), wmltr("Edit timer"));
              ReplyBuffer(wmltr("Timer is recording!"));
              ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
                 WML_DO("%s", URI_TIMERS) WML_ENDDO()),
                 wmltr("VDR"), wmltr("Timers"));
              Header(200, "OK", MIME_WAP);
              FlushBuffer();
              return;
              }
           }
        }
     }
  memset(getopt, 0, MAXPARSEBUFFER);
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
           }
        else
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
  memset(getopt, 0, MAXPARSEBUFFER);
  PageRecordings();
}

void cWapServer::SetLanguage(void)
{
  if (Option("id")) {
     if (isnumber(getopt)) {
        Setup.OSDLanguage = strtol(getopt, NULL, 10);
        cFont::SetCode(I18nCharSets()[Setup.OSDLanguage]);
        }
     }
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
           name = strtok_r(totext, ":", &tok);
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
     "%s:" WML_BR() WML_INPUT("Rid%d", "%s", "N"),
     wmltr("Name"), base, name ? name : wmltr("New"),
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
  if (channel)
     ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
        WML_DO("%s", URI_CHANNELS) WML_ENDDO()
        WML_DO("%s", URI_UPDATECHANNEL "/")
           WML_POSTFIELD("nr", "%d")
           WML_POSTFIELD("settings",
              "$(Name%d):$(Freq%d):$(Para%d):$(Src%d):$(Srat%d):$(Vpid%d):$(Apid%d):"
              "$(Tpid%d):$(Ca%d):$(Sid%d):$(Nid%d):$(Tid%d):$(Rid%d)")
        WML_ENDDO()
        WML_DO("%s", URI_SWITCHCHANNEL "/?nr=%d") WML_ENDDO()
        WML_DO("%s", URI_DELETECHANNEL "/?nr=%d") WML_ENDDO()),
        wmltr("VDR"), wmltr("Channels"),
        wmltr("Save"), channel_nr,
        base, base, base, base, base, base, base, base, base, base, base, base, base,
        wmltr("Switch"), channel_nr,
        wmltr("Delete"), channel_nr);
  else
     ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
        WML_DO("%s", URI_CHANNELS) WML_ENDDO()
        WML_DO("%s", URI_UPDATECHANNEL
           "/?settings=$(Name%d):$(Freq%d):$(Para%d):$(Src%d):$(Srat%d):$(Vpid%d):$(Apid%d):"
           "$(Tpid%d):$(Ca%d):$(Sid%d):$(Nid%d):$(Tid%d):$(Rid%d)")
        WML_ENDDO()),
        wmltr("VDR"), wmltr("Channels"),
        wmltr("Save"),
        base, base, base, base, base, base, base, base, base, base, base, base, base);
  free(totext);
  Header(200, "OK", MIME_WAP);
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
  asprintf(&file, "%s%02d%02d%02d", wmltr("New"), lt.tm_hour, lt.tm_min, lt.tm_sec);
  cTimer *timer = NULL;
  if (Option("id")) {
     if (isnumber(getopt)) {
        timer = Timers.Get(strtol(getopt, NULL, 10) - 1);
        if (timer) {
           flags = timer->Flags();
           channel_nr = timer->Channel()->Number();
           day = strdup(timer->PrintDay(timer->Day(), 0));
           start = timer->Start();
           stop = timer->Stop();
           priority = timer->Priority();
           lifetime = timer->Lifetime();
           file = strdup(timer->File());
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
     "%s:" WML_BR() WML_INPUT("File%d", "%s", "*M"),
     wmltr("Channel"), base, channel_nr,
     wmltr("Day"), base, day,
     wmltr("Start"), base, start / 100, start % 100,
     wmltr("Stop"), base, stop / 100, stop %100,
     wmltr("Priority"), base, priority,
     wmltr("Lifetime"), base, lifetime,
     wmltr("File"), base, wmlescape(file));
  if (timer)
     ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
        WML_DO("%s", URI_TIMERS) WML_ENDDO()
        WML_DO("%s", URI_UPDATETIMER "/")
           WML_POSTFIELD("id", "%s")
           WML_POSTFIELD("settings",
              "%d:$(Chan%d):$(Day%d):$(Strt%d):$(Stop%d):$(Prio%d):$(Life%d):$(File%d):")
        WML_ENDDO()
        WML_DO("%s", URI_DELETETIMER "/?id=%s") WML_ENDDO()
        WML_DO("%s", URI_ONOFFTIMER "/?id=%s") WML_ENDDO()),
        wmltr("VDR"), wmltr("Timers"),
        wmltr("Save"), getopt, flags, base, base, base, base, base, base, base,
        wmltr("Delete"), getopt,
        wmltr("On/Off"), getopt);
  else
     ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
        WML_DO("%s", URI_TIMERS) WML_ENDDO()
        WML_DO("%s", URI_UPDATETIMER
           "/?settings=%d:$(Chan%d):$(Day%d):$(Strt%d):$(Stop%d):$(Prio%d):$(Life%d):$(File%d):")
        WML_ENDDO()),
        wmltr("VDR"), wmltr("Timers"),
        wmltr("Save"), flags, base, base, base, base, base, base, base);
  free(day);
  free(file);
  Header(200, "OK", MIME_WAP);
  FlushBuffer();
}

void cWapServer::EditRecording(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Summary"));
  if (Option("id")) {
     if (isnumber(getopt)) {
        cRecording *recording = Recordings.Get(strtol(getopt, NULL, 10) - 1);
        if (recording) {
           const cRecordingInfo *info = recording->Info();
           int level = recording->HierarchyLevels();
           char *title = wmlescape(strdup(recording->Title(' ', false, level)));
           ReplyBuffer("%s", title);
           if (!isempty(info->ShortText())) {
              char *shorttext = wmlescape(strdup(info->ShortText()));
              ReplyBuffer(WML_BR() "%s", shorttext);
              free(shorttext);
              }
           if (!isempty(info->Description())) {
              char *description = wmlescape(strdup(info->Description()));
              ReplyBuffer(WML_BR() "%s", description);
              free(description);
              }
           else
              ReplyBuffer(wmltr("No description available."));
           free(title);
           }
        else
           ReplyBuffer("Recording \"%s\" not found.", getopt);
        ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
           WML_DO("%s", URI_RECORDINGS) WML_ENDDO()
           WML_DO("%s", URI_DELETERECORDING "/?id=%s") WML_ENDDO()),
           wmltr("VDR"), wmltr("Recordings"),
           wmltr("Delete"), getopt);
        Header(200, "OK", MIME_WAP);
        FlushBuffer();
        return;
        }
     else
        ReplyBuffer(wmltr("Error in recording number \"%s\"."), getopt);
     }
  else
     ReplyBuffer(wmltr("Missing recording id."));
  ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
     WML_DO("%s", URI_RECORDINGS) WML_ENDDO()),
     wmltr("VDR"), wmltr("Recordings"));
  Header(200, "OK", MIME_WAP);
  FlushBuffer();
}

void cWapServer::PageSchedule(void)
{
  int ChannelNr;
  if (Option("channel_nr") && isnumber(getopt))
     ChannelNr = strtol(getopt, NULL, 10);
  else
     ChannelNr = cDevice::CurrentChannel();
  cChannel* channel = Channels.GetByNumber(ChannelNr);
  bool Now = Option("now");
  bool Next = Option("next");

  ReplyBuffer(WML_PROLOGUE());
  cSchedulesLock schedulesLock;
  const cSchedules *Schedules = cSchedules::Schedules(schedulesLock);
  if (Schedules) {
     if (Option("event_id")) {
        time_t start = time(NULL);
        time_t stop = time(NULL);
        struct tm tm_start;
        struct tm tm_stop;
        char *file = NULL;
        ReplyBuffer(WML_START("%s"), channel->Name());
        if (isnumber(getopt)) {
           const cSchedule *Schedule = Schedules->GetSchedule(channel->GetChannelID());
           const cEvent *Event = Schedule->GetEvent(strtol(getopt, NULL, 10));
           if (Event) {
              char *title = Event->Title() ? wmlescape(strdup(Event->Title())) : NULL;
              char *shtxt = Event->ShortText() ? wmlescape(strdup(Event->ShortText())) : NULL;
              char *descr = Event->Description() ? wmlescape(strdup(Event->Description())) : NULL;
              file = Event->Title() ? wmlescape(strdup(Event->Title())) : NULL;
              ReplyBuffer("%s %s - %s" WML_BR() "%s" WML_BR() "%s" WML_BR() "%s",
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
              }
           else
              ReplyBuffer(wmltr("Event \"%s\" not found."), getopt);
           }
        else
           ReplyBuffer(wmltr("Error in event id \"%s\"."), getopt);
        localtime_r(&start, &tm_start);
        localtime_r(&stop, &tm_stop);
        strreplace(file, ':', '|');
        strreplace(file, ' ', '_');
        ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
           WML_DO("%s", URI_SCHEDULE "/?channel_nr=%d") WML_ENDDO()
           WML_DO("%s", URI_SWITCHCHANNEL "/?nr=%d") WML_ENDDO()
           WML_DO("%s", URI_UPDATETIMER "/")
              WML_POSTFIELD("settings", "1:%d:%d:%02d%02d:%02d%02d:%d:%d:%s:")
           WML_ENDDO()),
           wmltr("VDR"),
           wmltr("Schedule"), channel->Number(),
           wmltr("Switch"), channel->Number(),
           wmltr("Record"), channel->Number(), tm_start.tm_mday,
              tm_start.tm_hour, tm_start.tm_min, tm_stop.tm_hour, tm_stop.tm_min,
              Setup.DefaultPriority, Setup.DefaultLifetime, file);
        free(file);
        Header(200, "OK", MIME_WAP);
        FlushBuffer();
        return;
        }
     else if (Now || Next) {
        ReplyBuffer(WML_START("%s"), Now ? wmltr("What's on now?") : wmltr("What's on next?"));
        Option("offset");
        int offset = strtol(getopt, NULL, 10);
        int i = 0;
        for (channel = Channels.First(); channel; channel = Channels.Next(channel)) {
           if (!channel->GroupSep()) {
              const cSchedule *Schedule = Schedules->GetSchedule(channel->GetChannelID());
              if (Schedule) {
                 const cEvent *Event = Now
                    ?  Schedule->GetPresentEvent()
                    :  Schedule->GetFollowingEvent();
                 if (Event) {
                    if (i >= offset && i < (offset + MAXLINES)) {
                       char *chname = wmlescape(strdup(channel->Name()));
                       char *title = Event->Title()
                          ?  wmlescape(strdup(Event->Title()))
                          :  wmltr("no title");
                       ReplyBuffer("%d "
                          WML_ANCHOR("%s", URI_SCHEDULE "/")
                             WML_POSTFIELD("channel_nr", "%d")
                          WML_ENDANCHOR() WML_BR()
                          ". %s %s" WML_BR() ". "
                          WML_ANCHOR("%s", URI_SCHEDULE "/")
                             WML_POSTFIELD("event_id", "%d")
                             WML_POSTFIELD("channel_nr", "%d")
                          WML_ENDANCHOR() "%s",
                          channel->Number(),
                          chname, channel->Number(),
                          *Event->GetDateString(), *Event->GetTimeString(),
                          title, Event->EventID(), channel->Number(),
                          (i + 1) < (offset + MAXLINES) ? WML_BR() : "");
                       free(chname);
                       free(title);
                       }
                    i++;
                    }
                 }
              }
           }
        ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
           WML_DO("%s", URI_SCHEDULE "/?%s=y") WML_ENDDO()
           WML_DO(WML_LT, URI_SCHEDULE "/")
              WML_POSTFIELD("offset", "%d")
              WML_POSTFIELD("%s", "y")
           WML_ENDDO()
           WML_DO(WML_GT, URI_SCHEDULE "/")
              WML_POSTFIELD("offset", "%d")
              WML_POSTFIELD("%s", "y")
           WML_ENDDO()),
           wmltr("VDR"),
           Now ? wmltr("Next") : wmltr("Now"),
           Now ? "next" : "now",
           offset - MAXLINES > 0 ? offset - MAXLINES : 0,
           Now ? "now" : "next",
           offset + MAXLINES < i ? offset + MAXLINES : offset,
           Now ? "now" : "next");
        Header(200, "OK", MIME_WAP);
        FlushBuffer();
        return;
        }
     else if (channel) {
        ReplyBuffer(WML_START("%s - %s"), wmltr("Schedule"), channel->Name());
        const cSchedule *Schedule = Schedules->GetSchedule(channel->GetChannelID());
        if (Schedule) {
           Option("offset");
           int offset = strtol(getopt, NULL, 10);
           int i = 0;
           const cEvent *PresentEvent =
              Schedule->GetPresentEvent(channel->Number() == cDevice::CurrentChannel());
           time_t now = time(NULL) - Setup.EPGLinger * 60;
           for (const cEvent *Event = Schedule->Events()->First();
                Event;
                Event = Schedule->Events()->Next(Event)) {
              if (Event->EndTime() > now || Event == PresentEvent) {
                 if (i >= offset && i < (offset + MAXLINES)) {
                    char *title = Event->Title()
                       ?  wmlescape(strdup(Event->Title()))
                       :  wmltr("no title");
                    ReplyBuffer("%s %s " WML_BR() ". "
                       WML_ANCHOR("%s", URI_SCHEDULE "/")
                          WML_POSTFIELD("event_id", "%d")
                          WML_POSTFIELD("channel_nr", "%d")
                       WML_ENDANCHOR() "%s",
                       *Event->GetDateString(),
                       *Event->GetTimeString(),
                       title,
                       Event->EventID(),
                       channel->Number(),
                       (i + 1) < (offset + MAXLINES) && Schedule->Events()->Next(Event)
                          ?  WML_BR()
                          :  ""
                       );
                    free(title);
                    }
                 i++;
                 }
              }
           ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
              WML_DO("%s", URI_SCHEDULE "/?now=y") WML_ENDDO()
              WML_DO("%s", URI_SCHEDULE "/?next=y") WML_ENDDO()
              WML_DO(WML_LT, URI_SCHEDULE "/")
                 WML_POSTFIELD("offset", "%d")
                 WML_POSTFIELD("channel_nr", "%d")
              WML_ENDDO()
              WML_DO(WML_GT, URI_SCHEDULE "/")
                 WML_POSTFIELD("offset", "%d")
                 WML_POSTFIELD("channel_nr", "%d")
              WML_ENDDO()),
              wmltr("VDR"), wmltr("Now"), wmltr("Next"),
              offset - MAXLINES > 0 ? offset - MAXLINES : 0,
              ChannelNr,
              offset + MAXLINES < i ? offset + MAXLINES : offset,
              ChannelNr);
           Header(200, "OK", MIME_WAP);
           FlushBuffer();
           return;
           }
        else
           ReplyBuffer(wmltr("No schedule available."));
        }
     else
        ReplyBuffer(wmltr("Channel %d not defined."), ChannelNr);
     }
  else
     ReplyBuffer(WML_START("%s"), wmltr("Schedule"));
     ReplyBuffer(wmltr("Can't get EPG data."));
  ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()), wmltr("VDR"));
  Header(200, "OK", MIME_WAP);
  FlushBuffer();
}

void cWapServer::PageChannels(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Channels"));
  if (Channels.MaxNumber() >= 1) {
     Option("offset");
     int offset = strtol(getopt, NULL, 10);
     int i = 0;
     while (i < Channels.MaxNumber()) {
        cChannel *channel = Channels.GetByNumber(i + 1, 1);
        if (i >= offset && i < (offset + MAXLINES)) {
           if (channel) {
              char *name = wmlescape(strdup(channel->Name()));
              ReplyBuffer("%d " WML_A(URI_EDITCHANNEL "/?nr=%d", "%s") "%s",
                 channel->Number(),
                 channel->Number(),
                 name,
                 (i + 1) < (offset + MAXLINES) && (i + 1) < Channels.MaxNumber() ? WML_BR() : "");
              free(name);
              }
           else
              ReplyBuffer(wmltr("Channel \"%d\" not found."), i);
           }
        i = channel->Number();
        }
     ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
        WML_DO("%s", URI_EDITCHANNEL) WML_ENDDO()
        WML_DO(WML_LT, URI_CHANNELS "/?offset=%d") WML_ENDDO()
        WML_DO(WML_GT, URI_CHANNELS "/?offset=%d") WML_ENDDO()),
        wmltr("VDR"), wmltr("New"),
        offset - MAXLINES > 0 ? offset - MAXLINES : 0,
        offset + MAXLINES < i ? offset + MAXLINES : offset);
     Header(200, "OK", MIME_WAP);
     FlushBuffer();
     return;
     }
  else
     ReplyBuffer(wmltr("No channels defined."));
  ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()), wmltr("VDR"));
  Header(200, "OK", MIME_WAP);
  FlushBuffer();
}

void cWapServer::PageTimers(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Timers"));
  if (Timers.Count()) {
     Option("offset");
     int offset = strtol(getopt, NULL, 10);
     int i;
     for (i = 0; i < Timers.Count(); i++) {
        cTimer *timer = Timers.Get(i);
        if (i >= offset & i < (offset + MAXLINES)) {
           if (timer) {
              char *file = wmlescape(strdup(timer->File()));
              ReplyBuffer("%02d:%02d %02d:%02d %d %s %s" WML_BR()
                 ". " WML_A(URI_EDITTIMER "/?id=%d", "%s") "%s",
                 timer->Start() / 100,
                 timer->Start() % 100,
                 timer->Stop() / 100,
                 timer->Stop() % 100,
                 timer->Channel()->Number(),
                 *timer->PrintDay(timer->Day(), 0),
                 !(timer->HasFlags(tfActive))
                    ?  " "
                    :  timer->FirstDay()
                    ?  "!"
                    :  timer->Recording()
                    ?  "#"
                    :  WML_GT,
                 timer->Index() + 1,
                 file,
                 (i + 1) < (offset + MAXLINES) && (i + 1) < Timers.Count() ? WML_BR() : "");
              free(file);
              }
           else
              ReplyBuffer(wmltr("Timer \"%d\" not found."), i + 1);
           }
        }
     ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
        WML_DO("%s", URI_EDITTIMER) WML_ENDDO()
        WML_DO(WML_LT, URI_TIMERS "/?offset=%d") WML_ENDDO()
        WML_DO(WML_GT, URI_TIMERS "/?offset=%d") WML_ENDDO()),
        wmltr("VDR"), wmltr("New"),
        offset - MAXLINES > 0 ? offset - MAXLINES : 0,
        offset + MAXLINES < i ? offset + MAXLINES : offset);
     Header(200, "OK", MIME_WAP);
     FlushBuffer();
     return;
     }
  else
     ReplyBuffer(wmltr("No timers defined."));
  ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
     WML_DO("%s", URI_EDITTIMER) WML_ENDDO()),
     wmltr("VDR"), wmltr("New"));
  Header(200, "OK", MIME_WAP);
  FlushBuffer();
}

void cWapServer::PageRecordings(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Recordings"));
  bool recordings = Recordings.Load();
  if (recordings) {
     Option("offset");
     int offset = strtol(getopt, NULL, 10);
     cRecording *recording = Recordings.First();
     int i = 0;
     while (recording) {
        int level = recording->HierarchyLevels();
        if (i >= offset && i < (offset + MAXLINES)) {
           char *rec = wmlescape(strdup(recording->Title('|', true, level)));
           char *tok;
           char *date = strtok_r(rec, "|", &tok);
           char *time = strtok_r(NULL, "|", &tok);
           char *title = strtok_r(NULL, "|", &tok);
           ReplyBuffer("%s %s" WML_BR() ". " WML_A(URI_EDITRECORDING "/?id=%d", "%s") "%s",
              date ? date : "-",
              time ? time : "-",
              recording->Index() + 1,
              title ? title : "-",
              (i + 1) < (offset + MAXLINES) && Recordings.Next(recording) ? WML_BR() : "");
           free(rec);
           }
        recording = Recordings.Next(recording);
        i++;
        }
     ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
        WML_DO(WML_LT, URI_RECORDINGS "/?offset=%d") WML_ENDDO()
        WML_DO(WML_GT, URI_RECORDINGS "/?offset=%d") WML_ENDDO()),
        wmltr("VDR"),
        offset - MAXLINES > 0 ? offset - MAXLINES : 0,
        offset + MAXLINES < i ? offset + MAXLINES : offset);
     Header(200, "OK", MIME_WAP);
     FlushBuffer();
     return;
     }
  else
     ReplyBuffer(wmltr("No recordings available."));
  ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()), wmltr("VDR"));
  Header(200, "OK", MIME_WAP);
  FlushBuffer();
}

void cWapServer::PageLanguage(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("Setup.OSD$Language"));
  Option("offset");
  int offset = strtol(getopt, NULL, 10);
  int i;
  const char * const *lang = I18nLanguages();
  for (i = 0; i < I18nNumLanguages; i++) {
     if (i >= offset && i < (offset + MAXLINES)) {
        char *l = wmlescape(strdup(lang[i]));
        ReplyBuffer(WML_A(URI_SETLANGUAGE "/?id=%d", "%s") "%s",
           i, l,
           (i + 1) < (offset + MAXLINES) && (i + 1) < I18nNumLanguages ? WML_BR() : "");
        free(l);
        }
     }
  ReplyBuffer(WML_END(WML_DO("%s", URI_MAIN) WML_ENDDO()
     WML_DO(WML_LT, URI_LANGUAGE "/?offset=%d") WML_ENDDO()
     WML_DO(WML_GT, URI_LANGUAGE "/?offset=%d") WML_ENDDO()),
     wmltr("VDR"),
     offset - MAXLINES > 0 ? offset - MAXLINES : 0,
     offset + MAXLINES < i ? offset + MAXLINES : offset);
  Header(200, "OK", MIME_WAP);
  FlushBuffer();
}

void cWapServer::PageMain(void)
{
  ReplyBuffer(WML_PROLOGUE());
  ReplyBuffer(WML_START("%s"), wmltr("VDR"));
  ReplyBuffer(WML_A(URI_SCHEDULE, "%s") WML_BR(), wmltr("Schedule"));
  ReplyBuffer(WML_A(URI_CHANNELS, "%s") WML_BR(), wmltr("Channels"));
  ReplyBuffer(WML_A(URI_TIMERS, "%s") WML_BR(), wmltr("Timers"));
  ReplyBuffer(WML_A(URI_RECORDINGS, "%s") WML_BR(), wmltr("Recordings"));
  ReplyBuffer(WML_A(URI_LANGUAGE, "%s"), wmltr("Setup.OSD$Language"));
  ReplyBuffer(WML_END(""));
  Header(200, "OK", MIME_WAP);
  FlushBuffer();
}

bool cWapServer::ProcessHeader(void)
{
  char request[MAXPARSEBUFFER];
  char line[MAXPARSEBUFFER];

  method = mUnknown;
  memset(uri, 0, MAXPARSEBUFFER);
  memset(path, 0, MAXPARSEBUFFER);
  memset(opt, 0, MAXPARSEBUFFER);
  memset(getopt, 0, MAXPARSEBUFFER);
  memset(host, 0, MAXPARSEBUFFER);
  memset(authorization, 0, MAXPARSEBUFFER);

  ssize_t length = safe_read(file, request, sizeof(request));

  if (length > 0) {
     ssize_t position = strcspn(request, "\r\n");
     ssize_t oldpos;
     strncpy(line, request, position);
     line[position] = 0;
     if (strncasecmp(line, "GET", 3) == 0) {
        method = mGet;
        oldpos = 4;
        }
     else if (strncasecmp(line, "POST", 4) == 0) {
        method = mPost;
        oldpos = 5;
        }
     else {
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
//   dsyslog("uri = '%s'", uri);
//   dsyslog("path = '%s'", path);
//   dsyslog("opt = '%s'", opt);
//   dsyslog("authorization = '%s'", authorization);
     }
  else if (length < 0) {
     isyslog("error receiving from HTTP client");
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

void cWapServer::Shutdown(void)
{
  active = false;
}

void cWapServer::Action(void)
{
  active = true;
  cCondWait::SleepMs(5000); //initial delay
  dsyslog("WAPD: finished initial delay");

  while (active) {
     if (file.Open(socket.Accept())) {
        if (ProcessHeader()) {
           if (*authorization == 0 || !WapAccess.Acceptable(authorization))
              Header(401, "Unauthorized", MIME_WAP, SERVER_NAME);
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
     cCondWait::SleepMs(10);
     }
}

