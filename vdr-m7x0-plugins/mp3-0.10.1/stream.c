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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "common.h"
#include "setup-mp3.h"
#include "stream.h"
#include "network.h"
#include "menu-async.h"
#include "i18n.h"
#include "version.h"

#define USE_MMAP
#define MAX_MMAP_SIZE   (256*1024*1024)
#define MP3FILE_BUFSIZE (32*1024)

#ifdef USE_MMAP
#include <sys/mman.h>
#endif

#define DEFAULT_PORT 80 // default port for streaming (HTTP)

//#define DUMP_HEAD "/var/tmp/headers"

// --- cStream -----------------------------------------------------------------

cStream::cStream(const char *Filename)
:cFileInfo(Filename)
{
  fd=-1; ismmap=false; buffer=NULL;
}

cStream::~cStream()
{
  Close();
}

bool cStream::Open(bool log)
{
  if(fd>=0) return Seek();

  if(FileInfo(log)) {
    if((fd=open(Filename,O_RDONLY))>=0) {
      buffpos=readpos=0; fill=0;

#ifdef USE_MMAP
      if(Filesize<=MAX_MMAP_SIZE) {
        buffer=(unsigned char *)mmap(0,Filesize,PROT_READ,MAP_SHARED,fd,0);
        if(buffer!=MAP_FAILED) {
          ismmap=true;
          return true;
          }
        else dsyslog("mmap() failed for %s: %s",Filename,strerror(errno));
        }
#endif

        buffer = new unsigned char[MP3FILE_BUFSIZE];
        if(buffer) return true;
        else { esyslog("ERROR: not enough memory for buffer: %s",Filename); }
      }
    else if(log) { esyslog("ERROR: failed to open file %s: %s",Filename,strerror(errno)); }
    }

  Close();
  return false;
}

void cStream::Close(void)
{
  if(fd>=0) {
    close(fd);
    fd=-1;
    }
#ifdef USE_MMAP
  if(ismmap) {
    munmap(buffer,Filesize);
    buffer= NULL;
    ismmap=false;
    return;
    }
#endif
  delete[] buffer;
  buffer=NULL;
}

bool cStream::Seek(uint64_t pos)
{
  if(fd>=0 && pos<=Filesize) {
    buffpos=0; fill=0;
    if(ismmap) {
      readpos=pos;
      return true;
      }
    else {
      if((readpos=lseek64(fd,pos,SEEK_SET))>=0) {
        if(readpos!=pos) { dsyslog("seek mismatch in %s, wanted %lld, got %lld",Filename,pos,readpos); }
        return true;
        }
      else { esyslog("ERROR: seeking failed in %s: %d,%s",Filename,errno,strerror(errno)); }
      }
    }
  else d(isyslog("mp3: bad seek call fd=%d pos=%lld name=%s\n",fd,pos,Filename))
  return false;
}

bool cStream::Stream(const uint8_t * &data, uint32_t &len, const uint8_t *rest)
{
  if (fd < 0)
     return false;

  if (readpos >= Filesize) {
     len=0;
     return true;
     }

  if (ismmap) {
     if (rest && fill)
        readpos=(rest-buffer);   // take care of remaining data
     len=Filesize-readpos;
     fill=Filesize;
     data=buffer+readpos;
     readpos = Filesize;
     return true;
     }

  if (rest && fill) {       // copy remaining data to start of buffer
     fill-=(rest-buffer);   // remaing bytes
     memmove(buffer,rest,fill);
     }
  else
     fill=0;

  buffpos=readpos-fill;
  data=buffer; len=fill;
  int r;
  do {
     r=read(fd,buffer+fill,MP3FILE_BUFSIZE-fill);
     } while(r==-1 && errno==EINTR);

  if (r>=0) {
     readpos+=r; fill+=r;
     len=fill;
     return true;
     }

  esyslog("ERROR: read failed in %s: %d,%s",Filename,errno,strerror(errno));
  return false;
}

bool cStream::StreamSafer(const uint8_t *&data, uint32_t &len, const uint8_t *rest,
 uint32_t need)
{
  if (likely((rest != NULL) & (len >= need))) {
     data = rest;
     return true;
     }

  if (unlikely(need > MP3FILE_BUFSIZE)) {
     len = 1;
     return false;
     }

  int retry = 0;
  do {
     if (unlikely(!Stream(data, len, rest))) {
        len = 1;
        return false;
        }
     rest = data;
     retry++;
     } while (unlikely((len < need) & (len != 0) & (retry <= 3)));

  return len >= need;
}

int64_t cStream::Tell(const uint8_t *rest)
{
   if (fd < 0)
      return 0;
   if (rest && fill)
      return buffpos + (rest - buffer);

   return readpos;
}

bool cStream::Skip(const uint8_t *&data, uint32_t &len, const uint8_t *rest,
    int64_t skip)
{
   if ((fd < 0) | (skip < 0))
      return false;

   if (rest && fill)
      skip -= fill - (rest - buffer);

   if (skip <= 0) {
      data = buffer + fill + skip;
      len = -skip;
      return true;
      }

   len = 0;
   data = NULL;
   return Seek(readpos + skip);
}
// -----------------------------------------------------------------------------

#ifdef DUMP_HEAD
void Dump(const char *name, char *buffer)
{
  FILE *f=fopen(name,"a");
  if(f) {
    fprintf(f,"<<<< %s\n",buffer);
/*
    int n=strlen(buffer);
    for(int i=0 ; i<n ; i+=8) {
      fprintf(f,"%04x: ",i);
      for(int l=0 ; l<8 && i+l<n ; l++) fprintf(f,"%02x ",buffer[i+l]);
      fprintf(f,"\n");
      }
*/
    fclose(f);
    }
}
#endif

// --- cNetStream -----------------------------------------------------------------

cNetStream::cNetStream(const char *Filename)
:cStream(Filename)
{
  net=0; host=path=auth=0; cc=0;
  icyName=icyUrl=icyTitle=0; icyChanged=false;
  metaInt=0;
  InfoDone();
}

cNetStream::~cNetStream()
{
  free(host); free(path); free(auth);
  free(icyName); free(icyUrl); free(icyTitle);
}

bool cNetStream::ParseURL(const char *line, bool log)
{
  char pr[32], h[512], p[512], a[512];
  int r=sscanf(line," %31[^:]://%511[^/]%511[^\r\n]",pr,h,p);
  if(r==2) {
    d(isyslog("netstream: adding default path '/'\n"))
    strcpy(p,"/");
    r++;
    }
  if(r==3) {
    a[0]=0;
    char *s=index(h,'@');
    if(s) {
      *s=0;
      strcpy(a,h);
      strcpy(h,s+1);
      }
    d(isyslog("netstream: parsed proto='%s' host='%s' path='%s' auth='%s'\n",pr,h,p,a))
    if(!strcasecmp(pr,"http")) {
      int pp=DEFAULT_PORT;
      s=strrchr(h,':');
      if(s) { *s++=0; pp=atoi(s); }

      free(host); host=strdup(h);
      free(path); path=strdup(p);
      free(auth); auth=a[0] ? strdup(a) : 0;
      port=pp;
      return true;
      }
    else if(log) esyslog("Unsupported protocol %s in: %s",pr,line);
    }
  else if(log) esyslog("Bad URL line: %s",line);
  return false;
}

bool cNetStream::ParseURLFile(const char *name, bool log)
{
  bool res=false;
  FILE *f=fopen(name,"r");
  if(f) {
    char line[2048];
    if(fgets(line,sizeof(line),f)) {
      res=ParseURL(line,log);
      }
    else if(log) esyslog("Nothing to read from URL file %s. File empty?",name);
    fclose(f);
    }
  else if(log) esyslog("fopen() failed on URL file %s: %s",name,strerror(errno));
  return res;
}

bool cNetStream::SendRequest(void)
{
  bool res=false;
  char buff[2048];

  char *h, *p;
  asprintf(&h,port!=DEFAULT_PORT ? "%s:%d":"%s",host,port);
  if(MP3Setup.UseProxy) asprintf(&p,"http://%s%s",h,path);
  else asprintf(&p,"%s",path);

  char a[1024];
  a[0]=0;
  if(auth) {
    cBase64Encoder b64((uchar *)auth,strlen(auth),76);
    int q=0;
    const char *l;
    while((l=b64.NextLine())) {
      q+=snprintf(&a[q],sizeof(a)-q,"%s%s\r\n",q==0?"Authorization: Basic ":" ",l);
      }
    }

  snprintf(buff,sizeof(buff),
           "GET %s HTTP/1.0\r\n"
           "User-Agent: %s/%s\r\n"
           "Host: %s\r\n"
           "Accept: audio/mpeg\r\n"   //XXX audio/x-mpegurl, */*
           "Icy-MetaData: 1\r\n"
           "%s\r\n",
           p,PLUGIN_NAME,PLUGIN_VERSION,h,a);
  free(p); free(h);

  if(++cc==1) asyncStatus.Set(tr("Connecting to stream server ..."));

  if(net->Connect(MP3Setup.UseProxy ? MP3Setup.ProxyHost:host , MP3Setup.UseProxy ? MP3Setup.ProxyPort:port)) {
    d(isyslog("netstream: -> %s",buff))
    if(net->Puts(buff)>0) res=GetHTTPResponse();
    }

  if(cc--==1) asyncStatus.Set(0);
  return res;
}

bool cNetStream::ParseHeader(const char *buff, const char *name, char **value)
{
  char *s=index(buff,':');
  if(s && !strncasecmp(buff,name,s-buff)) {
    s=skipspace(s+1);
    d(isyslog("netstream: found header '%s' contents '%s'\n",name,s))
    free(*value); *value=strdup(s);
    return true;
    }
  return false;
}

bool cNetStream::GetHTTPResponse(void)
{
  bool res=false;
  char buff[1024], text[128], *newurl=0;
  int code=-1, hcount=0;
  while(net->Gets(buff,sizeof(buff))>0) {
    stripspace(buff);
#ifdef DUMP_HEAD
    Dump(DUMP_HEAD,buff);
#endif
    d(isyslog("netstream: <- %s\n",buff))
    hcount++;
    if(hcount==1) {   // parse status line
      if(sscanf(buff,"%*[^ ] %d %128s",&code,text)!=2) {
        esyslog("Bad HTTP response '%s' from %s:%d",buff,host,port);
        goto out;
        }
      }
    else {            // parse header lines
      if(buff[0]==0) { // headers finish if we receive a empty line
        switch(code) {
          case 200: // OK
             res=true;
             goto out;
          case 300: // MULTIPLE_CHOICES
          case 301: // MOVED_PERMANENTLY
          case 302: // MOVED_TEMPORARILY
             if(newurl) {
               if(ParseURL(newurl,true)) res=SendRequest();
               }
             else esyslog("No location header for redirection from %s:%d",host,port);
             goto out;
          default:
             esyslog("Unhandled HTTP response '%d %s' from %s:%d",code,text,host,port);
             goto out;
          }
        }

      ParseHeader(buff,"Location",&newurl);
      ParseHeader(buff,"icy-name",&icyName);
      ParseHeader(buff,"icy-url",&icyUrl);
      char *meta=0;
      if(ParseHeader(buff,"icy-metaint",&meta)) {
        metaInt=metaCnt=atol(meta);
        d(isyslog("netstream: meta interval set to %d\n",metaInt));
        }
      free(meta);
      }
    }
out:
  free(newurl);
  return res;
}

bool cNetStream::Open(bool log)
{
  if(net && net->Connected()) return true;

  if(!net) net=new cNet(0,0,0);
  net->Disconnect();

  if(ParseURLFile(Filename,log)) {
    buffpos=readpos=0; fill=0;
    buffer = new unsigned char[MP3FILE_BUFSIZE];
    if(buffer) {
      if(SendRequest()) {
        return true;
        }
      }
    else esyslog("Not enough memory for buffer");
    }

  Close();
  return false;
}

void cNetStream::Close(void)
{
  delete buffer; buffer=0;
  delete net; net=0;
}

bool cNetStream::Seek(uint64_t pos)
{
  return false;
}

bool cNetStream::Stream(const uint8_t *&data, uint32_t &len, const uint8_t *rest)
{
  if(net && net->Connected()) {
    if(rest && fill) {       // copy remaining data to start of buffer
      fill-=(rest-buffer);   // remaing bytes
      memmove(buffer,rest,fill);
      }
    else fill=0;

    data=buffer; len=fill;

    int r=MP3FILE_BUFSIZE-fill;
    if(metaInt && r>metaCnt) r=metaCnt;
    r=net->Read(buffer+fill,r);
    if(r>=0) {
      fill+=r; len=fill;
      metaCnt-=r;
      if(metaInt && metaCnt<=0) {
        ParseMetaData();
        metaCnt=metaInt;
        }
      return true;
      }
    }
  return false;
}

bool cNetStream::Skip(const uint8_t *&data, uint32_t &len, const uint8_t *rest,
    int64_t skip)
{
   if (!net || !net->Connected() || skip < 0)
      return false;

   if (rest && fill)
      skip -= fill - (rest - buffer);

   uint32_t l = 1;
   while ((skip > 0) & (l > 0)) {
         const uint8_t *d;
         if (!Stream(d, l, NULL))
            return false;
         skip -= fill;
         }

   if (skip > 0)
      return false;
   data = buffer + fill + skip;
   len = -skip;
   return true;
}


char *cNetStream::ParseMetaString(const char *buff, const char *name, char **value)
{
  char *s=index(buff,'=');
  if(s && !strncasecmp(buff,name,s-buff)) {
    char *end=index(s+2,'\'');
    if(s[1]=='\'' && end) {
      *end=0;
      s=stripspace(skipspace(s+2));
      if(strlen(s)>0) {
        d(isyslog("netstream: found metadata '%s' contents '%s'\n",name,s))
        free(*value); *value=strdup(s);
        }
      //else d(isyslog("netstream: found empty metadata '%s'\n",name))
      return end+1;
      }
    else d(isyslog("netstream: bad metadata format\n"))
    }
  return 0;
}


bool cNetStream::ParseMetaData(void)
{
  unsigned char byte;
  int r=net->Read(&byte,1);
  if(r<=0) return false;
  int metalen=byte*16;
  if(metalen>0) {
    char data[metalen+1];
    data[metalen]=0;
    int cnt=0;
    do {
      r=net->Read((unsigned char *)data+cnt,metalen-cnt);
      if(r<=0) return false;
      cnt+=r;
      } while(cnt<metalen);

#ifdef DUMP_HEAD
    Dump(DUMP_HEAD,data);
#endif

    char *p=data;
    while(*p && p-data<metalen) {
      char *n;
      if((n=ParseMetaString(p,"StreamTitle",&icyTitle)) ||
         (n=ParseMetaString(p,"StreamUrl",&icyUrl))) {
        p=n;
        icyChanged=true;
        }
      else  p++;
      }
    }
  return true;
}

bool cNetStream::IcyChanged(void)
{
  bool c=icyChanged;
  icyChanged=false;
  return c;
}
