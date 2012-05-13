/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2007 Stefan Huelswitt <s.huelswitt@gmx.de>
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
#include <errno.h>
#include <sys/stat.h>
#include <sys/vfs.h>

#include <vdr/videodir.h>

#include "common.h"
#include "data-mp3.h"
#include "data-src.h"
#include "decoder.h"
#include "decoder-core.h"
#include "decoder-mp3.h"
#include "decoder-mp3-stream.h"

#define CACHEFILENAME     "id3info.cache"
#define CACHESAVETIMEOUT  120 // secs
#define CACHEPURGETIMEOUT 120 // days

extern cFileSources MP3Sources;

cInfoCache InfoCache;
char *cachedir=0;

int MakeHashBuff(const char *buff, int len)
{
  int h=len;
  while(len--) h=(h*13 + *buff++) & 0x7ff;
  return h;
}

// --- cSongInfo ---------------------------------------------------------------

cSongInfo::cSongInfo(void)
{
  Title=Artist=Album=0;
  Clear();
}

cSongInfo::~cSongInfo()
{
  Clear();
}

void cSongInfo::Clear(void)
{
  Frames=0; Total=-1; DecoderID=0;
  SampleFreq=Channels=Bitrate=MaxBitrate=ChMode=-1;
  free(Title); Title=0;
  free(Artist); Artist=0;
  free(Album); Album=0;
  Year=-1;
  infoDone=false;
}

void cSongInfo::Set(cSongInfo *si)
{
  Clear(); InfoDone();
  Frames=si->Frames;
  Total=si->Total;
  SampleFreq=si->SampleFreq;
  Channels=si->Channels;
  Bitrate=si->Bitrate;
  MaxBitrate=si->MaxBitrate;
  ChMode=si->ChMode;
  Year=si->Year;
  Title=si->Title ? strdup(si->Title):0;
  Artist=si->Artist ? strdup(si->Artist):0;
  Album=si->Album ? strdup(si->Album):0;
  DecoderID=si->DecoderID;
}

void cSongInfo::FakeTitle(const char *filename, const char *extention)
{
  // if no title, try to build a reasonable from the filename
  if(!Title && filename)  {
    char *s=strrchr(filename,'/');
    if(s && *s=='/') {
      s++;
      Title=strdup(s);
      strreplace(Title,'_',' ');
      if(extention) {                            // strip given extention
        int l=strlen(Title)-strlen(extention);
        if(l>0 && !strcasecmp(Title+l,extention)) Title[l]=0;
        }
      else {                                     // strip any extention
        s=strrchr(Title,'.');
        if(s && *s=='.' && strlen(s)<=5) *s=0;
        }
      d(isyslog("mp3: faking title '%s' from filename '%s'\n",Title,filename))
      }
    }
}

// --- cFileInfo ---------------------------------------------------------------

cFileInfo::cFileInfo(void)
{
  Filename=FsID=0; Clear();
}

cFileInfo::cFileInfo(const char *Name)
{
  Filename=FsID=0; Clear();
  Filename=strdup(Name);
}

cFileInfo::~cFileInfo()
{
  Clear();
}

void cFileInfo::Clear(void)
{
  free(Filename); Filename=0;
  free(FsID); FsID=0;
  Filesize=0; CTime=0; FsType=0; removable=-1;
  infoDone=false;
}

bool cFileInfo::Removable(void)
{
  if(removable<0 && Filename) {
    cFileSource *src=MP3Sources.FindSource(Filename);
    if(src) removable=src->NeedsMount();
    else removable=1;
    }
  return (removable!=0);
}

void cFileInfo::Set(cFileInfo *fi)
{
  Clear(); InfoDone();
  Filename=fi->Filename ? strdup(fi->Filename):0;
  FsID=fi->FsID ? strdup(fi->FsID):0;
  Filesize=fi->Filesize;
  CTime=fi->CTime;
}


bool cFileInfo::FileInfo(bool log)
{
  if(Filename) {
    struct stat64 ds;
    if(!stat64(Filename,&ds)) {
      if(S_ISREG(ds.st_mode)) {
        free(FsID); FsID=0;
        FsType=0;
        struct statfs64 sfs;
        if(!statfs64(Filename,&sfs)) {
          if(Removable()) asprintf(&FsID,"%llx:%llx",sfs.f_blocks,sfs.f_files);
          FsType=sfs.f_type;
          }
        else if(errno!=ENOSYS && log) { esyslog("ERROR: can't statfs %s: %s",Filename,strerror(errno)); }
        Filesize=ds.st_size;
        CTime=ds.st_ctime;
#ifdef CDFS_MAGIC
        if(FsType==CDFS_MAGIC) CTime=0; // CDFS returns mount time as ctime
#endif
        InfoDone();
        return true;
        }
      else if(log) { esyslog("ERROR: %s is not a regular file",Filename); }
      }
    else if(log) { esyslog("ERROR: can't stat %s: %s",Filename,strerror(errno)); }
    }
  return false;
}

// --- cDecoders ---------------------------------------------------------------

cDecoder *cDecoders::FindDecoder(cFileObj *Obj)
{
  const char *full=Obj->FullPath();
  cFileInfo fi(full);
  cCacheData *dat;
  cDecoder *decoder=0;
  if(fi.FileInfo(false) && (dat=InfoCache.Search(&fi))) {
    if(dat->DecoderID) {
      d(isyslog("mp3: found DecoderID '%s' for %s from cache\n",cDecoders::ID2Str(dat->DecoderID),fi.Filename))
      switch(dat->DecoderID) {
        case DEC_MP3:  decoder=new cMP3Decoder(full); break;
        case DEC_MP3S: decoder=new cMP3StreamDecoder(full); break;
        default:       esyslog("ERROR: bad DecoderID '%s' from info cache: %s",cDecoders::ID2Str(dat->DecoderID),full); break;
        }
      }
    dat->Unlock();
    }

  if(!decoder || !decoder->Valid()) {
    // no decoder in cache or cached decoder doesn't matches.
    // try to detect a decoder

    delete decoder; decoder=0;
    if(!decoder) {
      decoder=new cMP3StreamDecoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
    if(!decoder) {
      decoder=new cMP3Decoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
    if(!decoder) esyslog("ERROR: no decoder found for %s",Obj->Name());
    }
  return decoder;
}

const char *cDecoders::ID2Str(int id)
{
  switch(id) {
    case DEC_MP3:  return DEC_MP3_STR;
    case DEC_MP3S: return DEC_MP3S_STR;
    }
  return 0;
}

int cDecoders::Str2ID(const char *str)
{
  if     (!strcmp(str,DEC_MP3_STR )) return DEC_MP3;
  else if(!strcmp(str,DEC_MP3S_STR)) return DEC_MP3S;
  return 0;
}

// --- cDecoder ----------------------------------------------------------------

cDecoder::cDecoder(const char *Filename)
{
  filename=strdup(Filename);
  playing=false;
}

cDecoder::~cDecoder()
{
  free(filename);
}

// --- cCacheData -----------------------------------------------------

cCacheData::cCacheData(void)
{
  touch=0; version=0;
}

void cCacheData::Touch(void)
{
  touch=time(0);
}

#define SECS_PER_DAY (24*60*60)

bool cCacheData::Purge(void)
{
  time_t now=time(0);
  //XXX does this realy made sense?
  //if(touch+CACHEPURGETIMEOUT*SECS_PER_DAY < now) {
  //  d(isyslog("cache: purged: timeout %s\n",Filename))
  //  return true;
  //  }
  if(touch+CACHEPURGETIMEOUT*SECS_PER_DAY/10 < now) {
    if(!Removable()) {                            // is this a permant source?
      struct stat64 ds;                           // does the file exists? if not, purge
      if(stat64(Filename,&ds) || !S_ISREG(ds.st_mode) || access(Filename,R_OK)) {
        d(isyslog("cache: purged: file not found %s\n",Filename))
        return true;
        }
      }
    }
  return false;
}

bool cCacheData::Upgrade(void)
{
  if(version<7) {
    if(Removable()) {
      if(!FsID) FsID=strdup("old"); // Dummy entry, will be replaced in InfoCache::Search()
      }
    else { free(FsID); FsID=0; }
    }
  if(version<4) {
    Touch();                     // Touch entry
    }
  if(version<3 && !Title) {
    FakeTitle(Filename,".mp3");  // Fake title
    }
  if(version<2 && Bitrate<=0) {
    return false;                // Trash entry without bitrate
    }
  return true;
}

void cCacheData::Create(cFileInfo *fi, cSongInfo *si)
{
  cFileInfo::Set(fi);
  cSongInfo::Set(si);
  hash=MakeHash(Filename);
  Touch();
}

bool cCacheData::Save(FILE *f)
{
  fprintf(f,"##BEGIN\n"
            "Filename=%s\n"
            "Filesize=%lld\n"
            "Timestamp=%ld\n"
            "Touch=%ld\n"
            "Version=%d\n"
            "Frames=%d\n"
            "Total=%d\n"
            "SampleFreq=%d\n"
            "Channels=%d\n"
            "Bitrate=%d\n"
            "MaxBitrate=%d\n"
            "ChMode=%d\n"
            "Year=%d\n",
            Filename,Filesize,CTime,touch,CACHE_VERSION,Frames,Total,SampleFreq,Channels,Bitrate,MaxBitrate,ChMode,Year);
  if(Title)     fprintf(f,"Title=%s\n"    ,Title);
  if(Artist)    fprintf(f,"Artist=%s\n"   ,Artist);
  if(Album)     fprintf(f,"Album=%s\n"    ,Album);
  if(DecoderID) fprintf(f,"DecoderID=%s\n",cDecoders::ID2Str(DecoderID));
  if(FsID)      fprintf(f,"FsID=%s\n"     ,FsID);
  fprintf(f,"##END\n");
  return ferror(f)==0;
}

bool cCacheData::Load(FILE *f)
{
  char buf[1024];
  const char *delimiters="=\n";

  cFileInfo::Clear();
  cSongInfo::Clear();
  while(fgets(buf,sizeof(buf),f)) {
    char *ptrptr;
    char *name =strtok_r(buf ,delimiters,&ptrptr);
    char *value=strtok_r(0,delimiters,&ptrptr);
    if(name) {
      if(!strcasecmp(name,"##END")) break;
      if(value) {
        if     (!strcasecmp(name,"Filename"))   Filename  =strdup(value);
        else if(!strcasecmp(name,"Filesize") ||
                !strcasecmp(name,"Size"))       Filesize  =atoll(value);
        else if(!strcasecmp(name,"FsID"))       FsID      =strdup(value);
        else if(!strcasecmp(name,"Timestamp"))  CTime     =atol(value);
        else if(!strcasecmp(name,"Touch"))      touch     =atol(value);
        else if(!strcasecmp(name,"Version"))    version   =atoi(value);
        else if(!strcasecmp(name,"DecoderID"))  DecoderID =cDecoders::Str2ID(value);
        else if(!strcasecmp(name,"Frames"))     Frames    =atoi(value);
        else if(!strcasecmp(name,"Total"))      Total     =atoi(value);
        else if(!strcasecmp(name,"SampleFreq")) SampleFreq=atoi(value);
        else if(!strcasecmp(name,"Channels"))   Channels  =atoi(value);
        else if(!strcasecmp(name,"Bitrate"))    Bitrate   =atoi(value);
        else if(!strcasecmp(name,"MaxBitrate")) MaxBitrate=atoi(value);
        else if(!strcasecmp(name,"ChMode"))     ChMode    =atoi(value);
        else if(!strcasecmp(name,"Year"))       Year      =atoi(value);
        else if(!strcasecmp(name,"Title"))      Title     =strdup(value);
        else if(!strcasecmp(name,"Artist"))     Artist    =strdup(value);
        else if(!strcasecmp(name,"Album"))      Album     =strdup(value);
        else if(!strcasecmp(name,"Level"))      ;
        else if(!strcasecmp(name,"Peak"))       ;
        else d(isyslog("cache: ignoring bad token '%s' from cache file\n",name))
        }
      }
    }

  if(ferror(f) || !Filename) return false;
  hash=MakeHash(Filename);
  return true;
}

// --- cInfoCache ----------------------------------------------------

cInfoCache::cInfoCache(void)
{
  lasttime=0; modified=false;
  lastpurge=time(0)-(50*60);
}

void cInfoCache::Shutdown(void)
{
  Cancel(10);
  Save(true);
}

void cInfoCache::Cache(cSongInfo *info, cFileInfo *file)
{
  lock.Lock();
  cCacheData *dat=Search(file);
  if(dat) {
    dat->Create(file,info);
    Modified();
    dat->Unlock();
    d(isyslog("cache: updating infos for %s\n",file->Filename))
    }
  else {
    dat=new cCacheData;
    dat->Create(file,info);
    AddEntry(dat);
    d(isyslog("cache: caching infos for %s\n",file->Filename))
    }
  lock.Unlock();
}

cCacheData *cInfoCache::Search(cFileInfo *file)
{
  int hash=MakeHash(file->Filename);
  lock.Lock();
  cCacheData *dat=FirstEntry(hash);
  while(dat) {
    if(dat->hash==hash && !strcmp(dat->Filename,file->Filename) && dat->Filesize==file->Filesize) {
      dat->Lock();
      if(file->FsID && dat->FsID && !strcmp(dat->FsID,"old")) { // duplicate FsID for old entries
        dat->FsID=strdup(file->FsID);
        dat->Touch(); Modified();
        //d(isyslog("adding FsID for %s\n",dat->Filename))
        }

      if((!file->FsID && !dat->FsID) || (file->FsID && dat->FsID && !strcmp(dat->FsID,file->FsID))) {
        //d(isyslog("cache: found cache entry for %s\n",dat->Filename))
        dat->Touch(); Modified();
        if(dat->CTime!=file->CTime) {
          d(isyslog("cache: ctime differs, removing from cache: %s\n",dat->Filename))
          DelEntry(dat); dat=0;
          }
        break;
        }
      dat->Unlock();
      }
    dat=(cCacheData *)dat->Next();
    }
  lock.Unlock();
  return dat;
}

void cInfoCache::AddEntry(cCacheData *dat)
{
  lists[dat->hash%CACHELINES].Add(dat);
  Modified();
}

void cInfoCache::DelEntry(cCacheData *dat)
{
  dat->Lock();
  lists[dat->hash%CACHELINES].Del(dat);
  Modified();
}

cCacheData *cInfoCache::FirstEntry(int hash)
{
  return lists[hash%CACHELINES].First();
}

bool cInfoCache::Purge(void)
{
  time_t now=time(0);
  if(now-lastpurge>(60*60)) {
    isyslog("cleaning up id3 cache");
    Start();
    lastpurge=now;
    }
  return Active();
}

void cInfoCache::Action(void)
{
  d(isyslog("cache: id3 cache purge thread started (pid=%d)\n",getpid()))
  nice(3);
  lock.Lock();
  for(int i=0,n=0 ; i<CACHELINES && Running(); i++) {
    cCacheData *dat=FirstEntry(i);
    while(dat && Running()) {
      cCacheData *ndat=(cCacheData *)dat->Next();
      if(dat->Purge()) DelEntry(dat);
      dat=ndat;

      if(++n>30) {
        lastmod=false; n=0;
        lock.Unlock(); lock.Lock();    // give concurrent thread an access chance
        if(lastmod) dat=FirstEntry(i); // restart line, if cache changed meanwhile
        }
      }
    }
  lock.Unlock();
  d(isyslog("cache: id3 cache purge thread ended (pid=%d)\n",getpid()))
}

char *cInfoCache::CacheFile(void)
{
  return AddPath(cachedir?cachedir:VideoDirectory,CACHEFILENAME);
}

void cInfoCache::Save(bool force)
{
  if(modified && (force || (!Purge() && time(0)>lasttime))) {
    char *name=CacheFile();
    cSafeFile f(name);
    free(name);
    d(isyslog("cache: saved cache to file start\n"))
    if(f.Open()) {
      lock.Lock();
      fprintf(f,"## This is a generated file. DO NOT EDIT!!\n"
                "## This file will be OVERWRITTEN WITHOUT WARNING!!\n");
      for(int i=0 ; i<CACHELINES ; i++) {
        cCacheData *dat=FirstEntry(i);
        while(dat) {
          if(!dat->Save(f)) { i=CACHELINES+1; break; }
          dat=(cCacheData *)dat->Next();
          }
        }
      lock.Unlock();
      f.Close();
      modified=false; lasttime=time(0)+CACHESAVETIMEOUT;
      d(isyslog("cache: saved cache to file end\n"))
      }
    }
}

void cInfoCache::Load(void)
{
  char *name=CacheFile();
  if(access(name,F_OK)==0) {
    isyslog("loading id3 cache from %s",name);
    FILE *f=fopen(name,"r");
    if(f) {
      char buf[256];
      bool mod=false;
      lock.Lock();
      for(int i=0 ; i<CACHELINES ; i++) lists[i].Clear();
      while(fgets(buf,sizeof(buf),f)) {
        if(!strcasecmp(buf,"##BEGIN\n")) {
          cCacheData *dat=new cCacheData;
          if(dat->Load(f)) {
            if(dat->version!=CACHE_VERSION) {
              if(dat->Upgrade()) mod=true;
              else { delete dat; continue; }
              }
            AddEntry(dat);
            }
          else {
            delete dat;
            if(ferror(f)) {
              esyslog("ERROR: failed to load id3 cache");
              break;
              }
            }
          }
        }
      lock.Unlock();
      fclose(f);
      d(isyslog("loading id3 cache from end"));
      modified=false; if(mod) Modified();
      }
    else LOG_ERROR_STR(name);
    }
  free(name);
}
