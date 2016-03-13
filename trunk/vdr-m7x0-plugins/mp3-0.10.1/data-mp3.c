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

#include "common.h"
#include "data-mp3.h"
#include "data.h"
#include "decoder.h"

//#define DEBUG_IMAGE

#ifdef DEBUG_IMAGE
#define di(x) { (x); }
#else
#define di(x) ;
#endif


// image suffixes to search
const char *img_suff[] = { "iFrame",0 };
// exclude list for instant playlist creation
const char *excl_pl[] = { "*"PLAYLISTEXT,"*.jpg","*.gif","*.png","*.iFrame",0 };
// exclude list for song browser
const char *excl_br[] = { ".*","*.jpg","*.gif","*.png","*.iFrame",0 };

// --- cSong -------------------------------------------------------------------

cSong::cSong(cFileObj *Obj)
{
  obj=new cFileObj(Obj);
  Init();
}

cSong::cSong(cFileSource *Source, const char *Subdir, const char *Name)
{
  obj=new cFileObj(Source,Subdir,Name,otFile);
  Init();
}

cSong::cSong(cSong *Song)
{
  obj=new cFileObj(Song->obj);
  Init();
}

cSong::~cSong()
{
  delete decoder;
  obj->Source()->Unblock();
  delete obj;
  free((char *)image);
}

void cSong::Init(void)
{
  decoder=0; user=0; image=0;
  fromDOS=decoderFailed=false;
  obj->Source()->Block();
}

int cSong::Compare(const cListObject &ListObject) const
{
  cSong *song=(cSong *)&ListObject;
  return strcasecmp(obj->Path(),song->obj->Path());
}

cSongInfo *cSong::Info(bool get)
{
  Decoder();
  cSongInfo *si=0;
  if(decoder) si=decoder->SongInfo(get);
  return si;
}

cDecoder *cSong::Decoder(void)
{
  decLock.Lock();
  if(!decoder && !decoderFailed) {
    decoder=cDecoders::FindDecoder(obj);
    if(!decoder) decoderFailed=true;
    }
  decLock.Unlock();
  return decoder;
}

void cSong::Convert(void)
{
  char *Name=Convert2Unix(obj->Name());
  obj->SetName(Name);
  fromDOS=true;
  free(Name);
}

char *cSong::Convert2Unix(const char *name) const
{
  char *Name=strdup(name);
  char *p=Name;
  while(*p) {
    if(*p=='/') *p='?';
    if(*p=='\\') *p='/';
    p++;
    }
  return Name;
}

/*
char *cSong::Convert2Dos(const char *name)
{
  char *Name=strdup(name);
  char *p=Name;
  while(*p) {
    if(*p=='\\') *p='?';
    if(*p=='/') *p='\\';
    p++;
    }
  return Name;
}
*/

bool cSong::Parse(char *s, const char *reldir) const
{
  s=skipspace(stripspace(s));
  if(*s) {
    if(s[0]=='/' || !reldir)
      obj->SplitAndSet(s);
    else {
      s=AddPath(reldir,s);
      obj->SplitAndSet(s);
      free(s);
      }
    return true;
    }
  return false;
}

bool cSong::Save(FILE *f, const char *reldir) const
{
  const char *path=obj->Path();
  if(reldir) {
    int l=strlen(reldir);
    if(!strncasecmp(path,reldir,l)) path+=l+1;
    }
  return fprintf(f,"%s\n",path)>0;
}

bool cSong::FindImage(void)
{
  if(image) return true;

  char base[strlen(obj->Path())+32];
  strcpy(base,obj->Path());
  di(isyslog("image: checking image for %s\n",obj->Path()))

  // song specific image
  char *m=strrchr(base,'.');
  if(m) *m=0;
  if((image=CheckImage(base))) return true;

  // album specific image in song directory
  if(!(m=strrchr(base,'/'))) m=base-1;
  strcpy(m+1,"folder");
  if((image=CheckImage(base))) return true;

  // artist specific image in parent directory
  if((m=strrchr(base,'/'))) {
    *m=0;
    if(!(m=strrchr(base,'/'))) m=base-1;
    strcpy(m+1,"folder");
    if((image=CheckImage(base))) return true;
    }

  // default image in source basedir
  if((image=CheckImage("background"))) return true;

  di(isyslog("image: no image for %s\n",obj->Path()))
  return false;
}

const char *cSong::CheckImage(const char *base) const
{
  char *p;
  int n;
  asprintf(&p,"%s/%s.%n          ",obj->Source()->BaseDir(),base,&n);
  for(const char **s=img_suff; *s; s++) {
#ifdef DEBUG
    if(strlen(*s)>10) isyslog("ERROR: buffer overflow in CheckImage ext=%s\n",*s);
#endif
    strcpy(&p[n],*s);
    di(isyslog("image: check %s\n",p))
    if(!access(p,R_OK)) {
      di(isyslog("image: found\n"))
      return p;
      }
    }
  free(p);
  return 0;
}

#include "data-mp3-image.c"
extern void PropagateImage(const char *image);

bool cSong::Image(unsigned char * &mem, int &len, bool &alloced)
{
  mem=0;

  int res=0;
  if(image || FindImage()) {
    di(isyslog("image: loading image %s\n",image))
      int f=open(image,O_RDONLY);
      if(f>=0) {
        struct stat64 st;
        fstat64(f,&st);
        len=st.st_size;
        mem=MALLOC(unsigned char,len);
        if(mem) {
          if (read(f,mem,len)==len) {
             res=1;
             alloced = true;
             }
          else free(mem);
          }
        close(f);
        }
    }

  PropagateImage(res==1 ? image : 0);

  if(res<=0) {
    di(isyslog("image: using static default image\n"))
    len = sizeof(defaultImage);
    mem = defaultImage;
    alloced = false;
    }
  return true;
}

// -- cPlayList --------------------------------------------------------------

cPlayList::cPlayList(cFileObj *Obj)
{
  obj=new cFileObj(Obj);
  Init();
}

cPlayList::cPlayList(cFileSource *Source, const char *Subdir, const char *Name)
{
  obj=new cFileObj(Source,Subdir,Name,otFile);
  Init();
}

cPlayList::cPlayList(cPlayList *List)
{
  obj=new cFileObj(List->obj);
  Init();
}

cPlayList::~cPlayList()
{
  free(basename);
  free(extbuffer);
  obj->Source()->Unblock();
  delete obj;
}

void cPlayList::Init(void)
{
  extbuffer=basename=0;
  isWinAmp=false;
  obj->Source()->Block();
  Set();
}

void cPlayList::Set(void)
{
  free(basename); basename=0;
  if(obj->Name()) {
    basename=strdup(obj->Name());
    int l=strlen(basename)-strlen(PLAYLISTEXT);
    if(l>0 && !strcasecmp(basename+l,PLAYLISTEXT)) basename[l]=0;
    }
}

#if APIVERSNUM >= 10315
int cPlayList::Compare(const cListObject &ListObject) const
#else
bool cPlayList::operator<(const cListObject &ListObject)
#endif
{
  cPlayList *list=(cPlayList *)&ListObject;
#if APIVERSNUM >= 10315
  return strcasecmp(obj->Name(),list->obj->Name());
#else
  return strcasecmp(obj->Name(),list->obj->Name())<0;
#endif
}

bool cPlayList::Load(void)
{
  Clear();
  bool result=false;
  FILE *f=fopen(obj->FullPath(),"r");
  if(f) {
    char buffer[512];
    result=true;
    while(fgets(buffer,sizeof(buffer),f)>0) {
      if(buffer[0]=='#') {
        if(!strncmp(buffer,WINAMPEXT,strlen(WINAMPEXT))) {
          d(isyslog("mp3: detected WinAmp style playlist\n"))
          isWinAmp=true;
          }
        continue;
        }
      if(!isempty(buffer)) {
        cSong *song=new cSong(obj->Source(),0,0);
        if(song->Parse(buffer,obj->Subdir())) Add(song);
        else {
          esyslog("error loading playlist %s\n",obj->FullPath());
          delete song;
          result=false;
          break;
          }
        }
      }
    fclose(f);
    }
  else LOG_ERROR_STR(obj->FullPath());

  if(result && isWinAmp) {
    cSong *song=First();
    while(song) {   // if this is a WinAmp playlist, convert \ to /
      song->Convert();
      song=cList<cSong>::Next(song);
      }
    }
  return result;
}

bool cPlayList::Save(void)
{
  bool result=true;
  cSafeFile f(obj->FullPath());
  if(f.Open()) {
    cSong *song=First();
    while(song) {
      if(!song->Save(f,obj->Subdir())) {
         result=false;
         break;
         }
      song=cList<cSong>::Next(song);
      }
    if(!f.Close()) result=false;
    }
  else result=false;
  return result;
}

bool cPlayList::Exists(void)
{
  return obj->Exists();
}

bool cPlayList::TestName(const char *newName)
{
  return obj->TestName(AddExt(newName,PLAYLISTEXT));
}

bool cPlayList::Rename(const char *newName)
{
  bool r=obj->Rename(AddExt(newName,PLAYLISTEXT));
  if(r) Set();
  return r;
}

bool cPlayList::Create(const char *newName)
{
  bool r=obj->Create(AddExt(newName,PLAYLISTEXT));
  if(r) {
    Set();
    r=Load();
    }
  return r;
}

bool cPlayList::Delete(void)
{
  return obj->Delete();
}

const char *cPlayList::AddExt(const char *FileName, const char *Ext)
{
  free(extbuffer); extbuffer=0;
  asprintf(&extbuffer,"%s%s",FileName,Ext);
  return extbuffer;
}

// -- cInstantPlayList ------------------------------------------------------

cInstantPlayList::cInstantPlayList(cFileObj *Obj)
:cPlayList(Obj)
{
  if(!Obj->Name()) Obj->SetName("instant");
}

bool cInstantPlayList::Load(void)
{
  bool res=false;
  Clear();
  switch(obj->Type()) {
    case otFile:
      d(isyslog("instant: file %s\n",obj->Name()))
      if(strcasecmp(obj->Name(),basename)) {
        d(isyslog("instant: detected as playlist\n"))
        res=cPlayList::Load();
        }
      else {
        Add(new cSong(obj));
        res=true;
        }
      break;
    case otDir:
      {
      d(isyslog("instant: dir %s\n",obj->Name()))
      res=ScanDir(obj->Source(),obj->Path(),stFile,obj->Source()->Include(),excl_pl,true);
      Sort();
      break;
      }
    case otBase:
      d(isyslog("instant: base\n"))
      res=ScanDir(obj->Source(),0,stFile,obj->Source()->Include(),excl_pl,true);
      Sort();
      break;
    default: break;
    }
  return res;
}

void cInstantPlayList::DoItem(cFileSource *src, const char *subdir, const char *name)
{
  Add(new cSong(src,subdir,name));
}

// -- cPlayLists --------------------------------------------------------------

bool cPlayLists::Load(cFileSource *Source)
{
  static const char *spec[] = { "*"PLAYLISTEXT,0 };
  Clear();
  bool res=ScanDir(Source,0,stFile,spec,0,false);
  Sort();
  return res;
}

void cPlayLists::DoItem(cFileSource *src, const char *subdir, const char *name)
{
  Add(new cPlayList(src,subdir,name));
}
