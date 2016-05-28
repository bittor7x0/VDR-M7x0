/*
 * lirc.c: LIRC remote control
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * LIRC support added by Carsten Koch <Carsten.Koch@icem.de>  2000-06-16.
 *
 */

#include "lirc.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include "config.h"

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
  if (!Connect())
     f = -1;
  Start();
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
  int ret, ch;
  int timeout = DEFAULTTIMEOUT;

  while (Running()) {
        bool ready = f >= 0 && cFile::FileReady(f, timeout);
        ret = -1;
        if (ready) {
           // read one line of the line oriented lirc protocol
           for (ret = 0; ret < (int)sizeof(buf); ret++) {
               ch = readchar(f);
               if (ch < 0) {
                  ret = -1;
                  break;
                  }
               if (ch == '\n') {
                  buf[ret++] = '\0';
                  break;
                  }
               buf[ret] = ch;
               }
           }

        if (f < 0 || ready && ret <= 0) {
           esyslog("ERROR: lircd connection broken, trying to reconnect every %.1f seconds", float(RECONNECTDELAY) / 1000);
           if (f >= 0)
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

        if (ready && ret > 0) {
           buf[ret - 1] = 0;
           int count;
           char KeyName[LIRC_KEY_BUF];
           if (sscanf(buf, "%*x %x %29s", &count, KeyName) != 2) { // '29' in '%29s' is LIRC_KEY_BUF-1!
              esyslog("ERROR: unparseable lirc command: %s", buf);
              continue;
              }
           if (count == 0) {
              if (strcmp(KeyName, LastKeyName) == 0 && FirstTime.Elapsed() < (unsigned int)Setup.LircRepeatDelay)
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
              if (LastTime.Elapsed() < (unsigned int)Setup.LircRepeatFreq)
                 continue; // repeat function kicks in after a short delay (after last key instead of first key)
              if (FirstTime.Elapsed() < (unsigned int)Setup.LircRepeatDelay)
                 continue; // skip keys coming in too fast (for count != 0 as well)
              repeat = true;
              timeout = Setup.LircRepeatDelay;
              }
           LastTime.Set();
           Put(KeyName, repeat);
           }
        else if (repeat) { // the last one was a repeat, so let's generate a release
           if (LastTime.Elapsed() >= (unsigned int)Setup.LircRepeatTimeout) {
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
