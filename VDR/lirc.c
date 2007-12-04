/*
 * lirc.c: LIRC remote control
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * LIRC support added by Carsten Koch <Carsten.Koch@icem.de>  2000-06-16.
 *
 * $Id$
 */

#include "lirc.h"
#include <netinet/in.h>
#include <sys/socket.h>

#define REPEATDELAY 350 // ms
#define REPEATFREQ 100 // ms
#define REPEATTIMEOUT 500 // ms
#define RECONNECTDELAY 3000 // ms
//M7X0 BEGIN AK
#define DEFAULTTIMEOUT 2000 // ms
//M7X0 END AK
cLircRemote::cLircRemote(const char *DeviceName)
:cRemote("LIRC")
,cThread("LIRC remote control")
{
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, DeviceName);
  if (Connect()) {
     Start();
     return;
     }
  f = -1;
}

cLircRemote::~cLircRemote()
{
  int fh = f;
  f = -1;
//M7X0 BEGIN AK
  Cancel(3);
//M7X0 END AK
  if (fh >= 0)
     close(fh);
}

bool cLircRemote::Connect(void)
{
  if ((f = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0) {
     if (connect(f, (struct sockaddr *)&addr, sizeof(addr)) >= 0)
        return true;
     LOG_ERROR_STR(addr.sun_path);
     close(f);
     f = -1;
     }
  else
     LOG_ERROR_STR(addr.sun_path);
  return false;
}

bool cLircRemote::Ready(void)
{
  return f >= 0;
}

void cLircRemote::Action(void)
{
  cTimeMs FirstTime;
  cTimeMs LastTime;
  char buf[LIRC_BUFFER_SIZE];
  char LastKeyName[LIRC_KEY_BUF] = "";
  bool repeat = false;
//M7X0 BEGIN AK
// This should fix broken shutdown
  int timeout = DEFAULTTIMEOUT;
  int i,ret=0;
  char *end=NULL;
//M7X0 END AK

  while (Running() && f >= 0) {
        int endcount = end ? end - buf + 1 : ret;
        if (endcount < ret) {
           memcpy (buf,buf + endcount, ret - endcount);
           ret -= endcount;
           end = NULL;
           }
        else if (ret < 0 || endcount == ret || ret >= LIRC_BUFFER_SIZE) {
           ret = 0;
           end = NULL;
           }
        bool ready = cFile::FileReady(f, timeout);
//M7X0 BEGIN AK
        if (ready) {
           //ret = 0;
           do {
              i = safe_read(f, buf + ret, LIRC_BUFFER_SIZE - ret);
              if (i < 0) {
                 ret = i;
                 break;
                 }
              ret += i;
              } while ((!ret || !(end = (char *)memchr(buf,'\n',ret))) && ret < LIRC_BUFFER_SIZE);
           buf[ret<0 ? 0 : (ret<LIRC_BUFFER_SIZE?ret:LIRC_BUFFER_SIZE-1)]=0;
          // dsyslog("LIRC Buffer '%s' read with %d Bytes",buf,ret);
           }
        //else
        //   ret = -1;
//M7X0 END AK
        if (ready && ret <= 0 ) {
           esyslog("ERROR: lircd connection broken, trying to reconnect every %.1f seconds", float(RECONNECTDELAY) / 1000);
           close(f);
           f = -1;
           while (Running() && f < 0) {
                 cCondWait::SleepMs(RECONNECTDELAY);
                 if (Connect()) {
                    isyslog("reconnected to lircd");
                    break;
                    }
                 }
           }
//M7X0 BEGIN AK (especially for Lemmi ;))
        if (ready && ret > 6) {
//M7X0 END AK
           int count;
           char KeyName[LIRC_KEY_BUF];
           if (sscanf(buf, "%*x %x %29s", &count, KeyName) != 2) { // '29' in '%29s' is LIRC_KEY_BUF-1!
              esyslog("ERROR: unparseable lirc command: %s", buf);
              continue;
              }
           if (count == 0) {
              if (strcmp(KeyName, LastKeyName) == 0 && FirstTime.Elapsed() < REPEATDELAY)
                 continue; // skip keys coming in too fast
              if (repeat)
                 Put(LastKeyName, false, true);
              strcpy(LastKeyName, KeyName);
              repeat = false;
              FirstTime.Set();
//M7X0 BEGIN AK
              timeout = DEFAULTTIMEOUT;
//M7X0 END AK
              }
           else {
              if (LastTime.Elapsed() < REPEATFREQ)
                 continue; // repeat function kicks in after a short delay (after last key instead of first key)
              if (FirstTime.Elapsed() < REPEATDELAY)
                 continue; // skip keys coming in too fast (for count != 0 as well)
              repeat = true;
              timeout = REPEATDELAY;
              }
           LastTime.Set();
           Put(KeyName, repeat);
           }
        else if (repeat) { // the last one was a repeat, so let's generate a release
           if (LastTime.Elapsed() >= REPEATTIMEOUT) {
              Put(LastKeyName, false, true);
              repeat = false;
              *LastKeyName = 0;
//M7X0 BEGIN AK
              timeout = DEFAULTTIMEOUT;
//M7X0 END AK
              }
           }
        }
}
