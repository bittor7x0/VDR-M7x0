/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2006 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <vdr/tools.h>

#include "common.h"
#include "setup-mp3.h"
#include "network.h"

#define CON_TIMEOUT      30*1000   // default timeout (ms) for connect operation
#define RW_TIMEOUT       30*1000   // default timeout (ms) for read/write operations
#define BUFFERSIZE       128*1024  // default ringbuffer size (bytes) for async read

#define NETDOWN_TIMEOUT  30        // timeout (s) for shutting down network

const char *netscript=0;

#if APIVERSNUM == 10131
#error Using this plugin with vdr 1.1.31 is not recommended (may cause high cpu load during streaming)
#endif

// -----------------------------------------------------------------------------

int RunCommand(const char *cmd, const char *State, const char *Name=0)
{
  int res=-1;
  if(cmd) {
    char *tmp=0;
    if(Name)
#if APIVERSNUM < 10318
      asprintf(&tmp,"%s %s \"%s\"",cmd,State,strescape(Name,"\"$"));
#else
      asprintf(&tmp,"%s %s \"%s\"",cmd,State,*strescape(Name,"\"$"));
#endif
    else asprintf(&tmp,"%s %s",cmd,State);

    d(isyslog("run: executing '%s'\n",tmp))
    res=SystemExec(tmp);
    free(tmp);
    }
  return res;
}

// -- cNetScript ---------------------------------------------------------------

class cNetScript : public cThread {
private:
  int count;
  bool pending;
protected:
  virtual void Action(void);
public:
  cNetScript(void);
  ~cNetScript();
  void Up(void);
  void Down(void);
  };

cNetScript ns;

cNetScript::cNetScript(void)
{
  count=0; pending=false;
}

cNetScript::~cNetScript()
{
  if(pending) Cancel(0);
}

void cNetScript::Up(void)
{
  Lock();
  if(netscript) {
    if(pending) { Cancel(0); pending=false; }
    RunCommand(netscript,"up");
    count++;
    }
  Unlock();
}

void cNetScript::Down(void)
{
  Lock();
  if(netscript) {
    if(--count==0) { Start(); pending=true; }
    }
  Unlock();
}

void cNetScript::Action(void)
{
  d(isyslog("net: netscript down delay\n"))
  sleep(NETDOWN_TIMEOUT);
  Lock();
  RunCommand(netscript,"down");
  Unlock();
}

// -- cNetConnect --------------------------------------------------------------

class cNetConnect : public cThread {
private:
  int fd;
  const char *hostname;
  int port;
  cMutex conMutex;
  cCondVar conCond;
  int result;
protected:
  virtual void Action(void);
  void Done(int res);
public:
  cNetConnect(int Fd, const char *Hostname, int Port);
  ~cNetConnect();
  int Wait(int timeoutMs);
  };

cNetConnect::cNetConnect(int Fd, const char *Hostname, int Port)
{
  fd=Fd;
  hostname=Hostname;
  port=Port;
  result=0;
  Start();
}

cNetConnect::~cNetConnect()
{
  Cancel(1);
}

int cNetConnect::Wait(int timeoutMs)
{
  conMutex.Lock();
  if(!result) conCond.TimedWait(conMutex,timeoutMs);
  conMutex.Unlock();
  return result;
}

void cNetConnect::Done(int res)
{
  conMutex.Lock();
  result=res;
  conCond.Broadcast();
  conMutex.Unlock();
}

void cNetConnect::Action(void)
{
  d(isyslog("net: name lookup %s\n",hostname))
  struct hostent *hp=gethostbyname(hostname);
  if(hp) {
    struct sockaddr_in sin;
    sin.sin_port=htons(port);
    sin.sin_family=AF_INET;
    memcpy((char *)&sin.sin_addr,hp->h_addr,hp->h_length);
    d(isyslog("net: connecting to %s:%d\n",hostname,port))
    if(connect(fd,(struct sockaddr *)&sin,sizeof(sin))==0) {
      d(isyslog("net: connected\n"))
      Done(1);
      }
    else { esyslog("connect() failed: %s",strerror(errno)); Done(-1); }
    }
  else { esyslog("Unknown host '%s'",hostname); Done(-1); }
}

// -- cNet ---------------------------------------------------------------------

cNet::cNet(int size, int ConTimeoutMs, int RwTimeoutMs)
:cRingBufferLinear(size>0?size:BUFFERSIZE,1,false)
{
  fd=-1; deferedErrno=0; count=0;
  connected=netup=false;
  rwTimeout =RwTimeoutMs  ? RwTimeoutMs :RW_TIMEOUT;
  conTimeout=ConTimeoutMs ? ConTimeoutMs:CON_TIMEOUT;
#if APIVERSNUM >= 10132
  SetTimeouts(50,50);
#endif
}

cNet::~cNet()
{
  Disconnect();
}

void cNet::Close(void)
{
  if(connected) {
    connected=false;
    Cancel(2);
    deferedErrno=0;
    }
  if(fd>=0) { close(fd); fd=-1; }
  Clear(); count=0;
}

void cNet::Disconnect(void)
{
  Close();
  if(netup) { ns.Down(); netup=false; }
}

bool cNet::Connect(const char *hostname, const int port)
{
  Close();
  fd=socket(AF_INET,SOCK_STREAM,0);
  if(fd>=0) {
    ns.Up(); netup=true;
    cNetConnect *con=new cNetConnect(fd,hostname,port);
    int res=con->Wait(conTimeout);
    delete con;
    if(res>0) {
      if(fcntl(fd,F_SETFL,O_NONBLOCK)>=0) {
        deferedErrno=0; connected=true;
        Start();
        return(true);
        }
      else esyslog("fnctl() failed: %s",strerror(errno)); 
      }
    else if(res==0) esyslog("Connection timed out"); 
    }
  else esyslog("socket() failed: %s",strerror(errno)); 
  Disconnect();
  return false;
}

void cNet::CopyFromBuff(unsigned char *dest, int n)
{
  memcpy(dest,lineBuff,n);
  count-=n;
  if(count>0) memmove(lineBuff,lineBuff+n,count);
}

int cNet::Gets(char *dest, int len)
{
  len--; // let room for trailing zero
  int c=0;
  while(c<len) {
    if(count<=0) {
      int r=RingRead(lineBuff,sizeof(lineBuff));
      if(r<0) {
        if(c==0) return -1;
        break;
        }
      count=r;
      }
    int n=0;
    while(n<count && n+c<len) {
      if(lineBuff[n]=='\n') len=0;
      n++;
      }
    CopyFromBuff((unsigned char *)dest,n);
    dest+=n; c+=n;
    }
  *dest=0;
  return c;
}

int cNet::Read(unsigned char *dest, int len)
{
  int c=0;
  if(count>0) {
    c=count; if(c>len) c=len;
    CopyFromBuff(dest,c);
    }
  else {
    c=RingRead(dest,len);
    }
  return c;
}

int cNet::Write(unsigned char *dest, int len)
{
  int t=0, r;
  cPoller poll(fd,true);
  do {
    if(poll.Poll(rwTimeout)) {
      r=write(fd,dest,len);
      if(r<0 && errno!=EAGAIN) {
        esyslog("write() failed: %s",strerror(errno));
        break;
        }
      dest+=r; len-=r; t+=r;
      }
    else { esyslog("Write timed out"); break; }
    } while(len>0);
  return t;
}

int cNet::Puts(char *dest)
{
  return Write((unsigned char *)dest,strlen(dest));
}

int cNet::RingRead(unsigned char *dest, int len)
{
  int r=0;
  const uchar *rd;
  for(;;) {
    if(!Available() && deferedErrno) {
      d(isyslog("net: ringbuffer empty, async read bailed out\n"))
      return -1;
      }
    rd=Get(r);
    if(rd && r>0) {
      if(r>len) r=len;
      memcpy(dest,rd,r);
      Del(r);
      return r;
      }
    }
}

void cNet::Action(void)
{
  d(isyslog("net: async read started\n"))

  cPoller poll(fd,false);
  while(connected) {
    if(poll.Poll(rwTimeout)) {
      unsigned char buff[8192];
      int r=read(fd,buff,sizeof(buff));
      if(r>0) {
        int d=0;
        do { d+=Put(buff+d,r-d); } while(d<r && connected);
        }
      else if(r<0 && errno!=EAGAIN) {
        deferedErrno=errno;
        esyslog("read() failed: %s",strerror(errno));
        break;
        }
      else if(r==0) {
        deferedErrno=-1;
        d(isyslog("EOF from read()\n"))
        break;
        }
      }
    else {
      deferedErrno=-1;
      esyslog("Read timed out");
      break;
      }
    }
  EnableGet();
  connected=false;
}
