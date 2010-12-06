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
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>

#include <vdr/tools.h>

#include "common.h"
#include "data.h"
#include "data-src.h"

// ----------------------------------------------------------------

const char *mountscript = "mount.sh";

char *Quote(const char *str)
{
  char *nstr=MALLOC(char,strlen(str)*2);
  char *p=nstr;
  while(*str) {
    switch(*str) {
      case '$':  // dollar
      case '\\': // backslash
      case '\"': // double quote
      case '`':  // back tick
                 *p++='\\'; break;
      }
    *p++=*str++;
    }
  *p=0;
  return nstr;
}

char *AddPath(const char *dir, const char *filename)
{
  char *name=0;
  asprintf(&name,"%s/%s",dir,filename);
  return name;
}

bool CheckVDRVersion(int Version, int Major, int Minor, const char *text)
{
  static char vv[] = VDRVERSION;
  int version, major, minor;
  if(sscanf(vv,"%d.%d.%d",&version,&major,&minor)==3) {
    if(version<Version ||
       (version==Version && major<Major) ||
       (version==Version && major==Major && minor<Minor)) {
      if(text) {
        esyslog("ERROR: %s plugin needs at least VDR version %d.%d.%d",text,Version,Major,Minor);
        fprintf(stderr,"%s plugin needs at least VDR version %d.%d.%d\n",text,Version,Major,Minor);
        }
      return false;
      }
    }
  else esyslog("ERROR: cannot parse VDR version string '%s'",vv);
  return true;
}

// -- cScanDir --------------------------------------------------------------

bool cScanDir::ScanDir(cFileSource *src, const char *subdir, eScanType type, const char * const *spec, const char * const *excl, bool recursiv)
{
  bool res=true;
  char *dir, *f=0;
  asprintf(&dir,subdir ? "%s/%s":"%s",src->BaseDir(),subdir);
  DIR *d=opendir(dir);
  if(d) {
    struct dirent64 *e;
    while((e=readdir64(d))) {
      //Skip files beginning with '.' like '.','..', '.test'
      if(!strncmp(e->d_name,".",1)) continue;
      free(f);
      if(!(f=AddPath(dir,e->d_name))) continue;
      struct stat64 st;
      if(stat64(f,&st)<0) {
        esyslog("ERROR: stat(1) %s: %s",f,strerror(errno));
        continue;
        }
      if(S_ISLNK(st.st_mode)) {
        char *of=f; f=ReadLink(of); free(of);
        if(!f) continue;
        if(stat64(f,&st)<0) {
          esyslog("ERROR: stat(2) %s: %s",f,strerror(errno));
          continue;
          }
        }
      if(S_ISDIR(st.st_mode)) {
        if(type==stFile && recursiv) {
          char *s;
          asprintf(&s,subdir ? "%2$s/%1$s":"%s",e->d_name,subdir);
          res=ScanDir(src,s,type,spec,excl,recursiv);
          free(s);
          if(!res) break;
          continue;
          }
        if(type!=stDir) continue;
        }
      if(S_ISREG(st.st_mode)) {
        if(type!=stFile) continue;
        if(spec) {
          bool ok=false;
          for(const char * const *m=spec; *m; m++) {
            int n=fnmatch(*m,e->d_name,FNM_CASEFOLD);
            if(n==0) { ok=true; break; }
            if(n!=FNM_NOMATCH) esyslog("ERROR: fnmatch(1) %s: %s",*m,strerror(errno));
            }
          if(!ok) continue;
          }
        if(excl) {
          bool ok=true;
          for(const char * const *m=excl; *m; m++) {
            int n=fnmatch(*m,e->d_name,FNM_CASEFOLD);
            if(n==0) { ok=false; break; }
            if(n!=FNM_NOMATCH) esyslog("ERROR: fnmatch(2) %s: %s",*m,strerror(errno));
            }
          if(!ok) continue;
          }
        }
      DoItem(src,subdir,e->d_name);
      }
    closedir(d);
    }
  else {
    esyslog("ERROR: opendir %s: %s",dir,strerror(errno));
    res=false;
    }
  free(dir); free(f);
  return res;
}

// -- cFileObj --------------------------------------------------------------

cFileObj::cFileObj(cFileSource *Source, const char *Subdir, const char *Name, const eObjType Type)
{
  path=fpath=0;
  source=Source;
  subdir=Subdir ? strdup(Subdir):0;
  name=Name ? strdup(Name):0;
  type=Type;
  Set();
}

cFileObj::cFileObj(const cFileObj *obj)
{
  path=fpath=0;
  source=obj->source;
  subdir=obj->subdir ? strdup(obj->subdir):0;
  name=obj->name ? strdup(obj->name):0;
  type=obj->type;
  Set();
}

cFileObj::~cFileObj()
{
  free(name);
  free(subdir);
  free(path);
  free(fpath);
}

#if APIVERSNUM >= 10315
int cFileObj::Compare(const cListObject &ListObject) const
{
  cFileObj *obj=(cFileObj *)&ListObject;
  if(type==otParent) return obj->type==otParent ? 0:-1;
  if(obj->type==otParent) return 1;
  if(type==otBase) return obj->type==otBase ? 0:1;
  if(obj->type==otBase) return -1;
  if(type!=obj->type) {
    if(type==otFile) return 1;
    return -1;
    }
  return strcasecmp(path,obj->path);
}
#else
bool cFileObj::operator<(const cListObject &ListObject)
{
  cFileObj *obj=(cFileObj *)&ListObject;
  if(type==otParent) return obj->type==otParent ? false:true;
  if(obj->type==otParent) return false;
  if(type==otBase) return false;
  if(obj->type==otBase) return true;
  if(type!=obj->type) {
    if(type==otFile) return false;
    return true;
    }
  return strcasecmp(path,obj->path)<0;
}
#endif

void cFileObj::SplitAndSet(const char *Path)
{
  free(subdir); subdir=0;
  const char *p=Path;
  if(Path[0]=='/') {
    int l=strlen(source->BaseDir());
    if(!strncasecmp(Path,source->BaseDir(),l)) p+=l+1;
    else {
      l=strlen(source->RealBaseDir());
      if(!strncasecmp(Path,source->RealBaseDir(),l)) p+=l+1;
      else {
        char buff[strlen(Path)+5];
        strcpy(buff,"/");
        p++;
        while(1) {
          char real[PATH_MAX+1];
          if(!realpath(buff,real)) {
            if(errno!=ENOENT && errno!=ENOTDIR)
              esyslog("ERROR: realpath: %s: %s",buff,strerror(errno));
            p=Path+1;
            break;
            }
          if(!strncasecmp(real,source->RealBaseDir(),l))
            break;
          const char *r=index(p,'/');
          if(!r) {
            esyslog("ERROR: can't find source basedir in '%s'. Outside source?",Path);
            p=Path+1;
            break;
            }
          strn0cpy(buff,Path,r-Path+1);
          p=r+1;
          }
        }
      }
    }

  const char *s=rindex(p,'/');
  if(s) {
    const int l=s-p+1;
    subdir=MALLOC(char,l);
    if(subdir) strn0cpy(subdir,p,l);
    SetName(s+1);
    }
  else
    SetName(p);
}

void cFileObj::SetName(const char *Name)
{
  free(name);
  name=Name ? strdup(Name):0;
  Set();
}

void cFileObj::Set(void)
{
  free(path); path=0;
  asprintf(&path,subdir ? "%2$s/%1$s":"%s",name,subdir);
  free(fpath); fpath=0;
  MakeFullName(&fpath,name);
}

void cFileObj::MakeFullName(char **fp, const char *Name)
{
  asprintf(fp,subdir ? "%1$s/%3$s/%2$s":"%s/%s",source->BaseDir(),Name,subdir);
}

bool cFileObj::GuessType(void)
{
  struct stat64 ds;
  if(!stat64(fpath,&ds)) {
    if(S_ISREG(ds.st_mode))      type=otFile;
    else if(S_ISDIR(ds.st_mode)) type=subdir ? otDir:otBase;
    else return false;
    return true;
    }
  return false;
}

bool cFileObj::Exists(void)
{
  if(type==otFile) {
    struct stat64 ds;
    if(!stat64(fpath,&ds) &&
       S_ISREG(ds.st_mode) &&
       !access(fpath,R_OK)) return true;
    }
  return false;
}

bool cFileObj::TestName(const char *newName)
{
  bool r=false;
  if(type==otFile) {
    char *fname;
    MakeFullName(&fname,newName);
    if(access(fname,F_OK)==0) r=true;
    free(fname);
    }
  return r;
}

bool cFileObj::Rename(const char *newName)
{
  bool r=false;
  if(type==otFile) {
    char *fname;
    MakeFullName(&fname,newName);
    if(access(fname,F_OK) && (!rename(fpath,fname))) {
      SetName(newName);
      r=true;
      }
    free(fname);
    }
  return r;
}

bool cFileObj::Create(const char *newName)
{
  bool r=false;
  if(type==otFile) {
    char *fname;
    MakeFullName(&fname,newName);
    FILE *newf;
    if(access(fname,F_OK) && (newf=fopen(fname,"w"))) {
      fclose(newf);
      SetName(newName);
      r=true;
      }
    free(fname);
    }
  return r;
}

bool cFileObj::Delete(void)
{
  if(type==otFile && !unlink(fpath)) return true;
  return false;
}

// -- cDirList --------------------------------------------------------------

bool cDirList::Load(cFileSource *src, const char *subdir, const char * const *excl)
{
  static const char *excl_s[] = { ".*",0 };

  bool res=false;
  Clear();
  if(subdir) Add(new cFileObj(src,subdir,"..",otParent));
  otype=otDir;
  if(ScanDir(src,subdir,stDir,0,0,false)) {
    otype=otFile;
    if(!excl) excl=excl_s;
    if(ScanDir(src,subdir,stFile,src->Include(),excl,false)) res=true;
    }
  Sort();
  return res;
}

void cDirList::DoItem(cFileSource *src, const char *subdir, const char *name)
{
  Add(new cFileObj(src,subdir,name,otype));
}

// -- cFileSource --------------------------------------------------------------

cFileSource::cFileSource(void)
{
  browsedir=browseparent=0;
  basedir=realbasedir=description=0; useCount=0;
  needsmount=false;
  include=0; incCount=0;
}

cFileSource::cFileSource(const char *Basedir, const char *Description, const bool NeedsMount, const char *Include)
{
  browsedir=browseparent=0;
  basedir=realbasedir=description=0; useCount=0;
  include=0; incCount=0;
  Set(Basedir,Description,NeedsMount,Include);
}

cFileSource::~cFileSource()
{
  ClearRemember();
  Clear();
}

void cFileSource::Clear(void)
{
  free(basedir); basedir=0;
  free(realbasedir); realbasedir=0;
  free(description); description=0;
  for(int i=0; i<incCount; i++) free(include[i]);
  free(include); include=0; incCount=0;
}

void cFileSource::Set(const char *Basedir, const char *Description, const bool NeedsMount, const char *Include)
{
  Clear();
  basedir=strdup(Basedir);
  description=strdup(Description);
  if(Include) {
    do {
      char *s=index(Include,'/');
      int l=s ? s-Include : strlen(Include);
      if(l) {
        char **s=(char **)realloc(include,(incCount+2)*sizeof(char *));
        if(s) {
          include=s;
          include[incCount]=strndup(Include,l);
          incCount++;
          include[incCount]=0;
          }
        }
      Include+=l+(s ? 1:0);
      } while(*Include>0);
    }
#ifdef DEBUG
  if(include) {
    isyslog("sources: filesource %s includes (count=%d):",basedir,incCount);
    for(int i=0; i<incCount; i++) printf(" '%s'",include[i]);
    printf("\n");
    }
  else
    isyslog("sources: filesource %s has no includes set\n",basedir);
#endif
  needsmount=NeedsMount;

  realbasedir=MALLOC(char,PATH_MAX+1);
  if(realpath(basedir,realbasedir)) {
    if(strcmp(basedir,realbasedir)) { esyslog("WARNING: source base %s expands to %s",basedir,realbasedir); }
    }
  else {
    switch(errno) {
      case EACCES:  esyslog("ERROR: source base %s permission denied",basedir); break;
      case ENOENT:  esyslog("ERROR: source base %s not found",basedir); break;
      case ENOTDIR: esyslog("ERROR: source base %s has invalid path",basedir); break;
      default:      esyslog("ERROR: source base %s realpath: %s",basedir,strerror(errno)); break;
      }
    strn0cpy(realbasedir,basedir,PATH_MAX);
    }
}

void cFileSource::SetRemember(const char *dir, const char *parent)
{
  ClearRemember();
  if(dir) browsedir=strdup(dir);
  if(parent) browseparent=strdup(parent);
}

void cFileSource::ClearRemember(void)
{
  free(browsedir); browsedir=0;
  free(browseparent); browseparent=0;
}

bool cFileSource::GetRemember(char * &dir, char * &parent)
{
  dir=parent=0;
  if(browsedir) {
    if(browseparent) parent=strdup(browseparent);
    dir=strdup(browsedir);
    return true;
    }
  return false;
}

bool cFileSource::Parse(char *s)
{
  char base[256], des[256], incl[256];
  int needsmount, n;
  if((n=sscanf(s,"%255[^;];%255[^;];%d;%255[^;]",base,des,&needsmount,incl))>=3) {
    char *base2=skipspace(stripspace(base));
    int l=strlen(base2);
    while(l>0 && base2[l-1]=='/') {
      esyslog("WARNING: removing trailing '/' from base %s",base2);
      base2[l-1]=0;
      l--;
      }
    Set(base2,skipspace(stripspace(des)),needsmount!=0,n>3?skipspace(stripspace(incl)):0);

    // do some checking of the basedir and issue a warning if apropriate
    if(access(realbasedir,R_OK)) { esyslog("WARNING: source base %s not found/permission denied",realbasedir); }
    else {
      struct stat64 ds;
      if(lstat64(realbasedir,&ds)) { esyslog("WARNING: can't stat source base %s",realbasedir); }
      else if(!S_ISDIR(ds.st_mode)) { esyslog("WARNING: source base %s is not a directory",realbasedir); }
      }
    return true;
    }
  return false;
}

bool cFileSource::Action(eAction act)
{
  static const char *str[] = { "mount","unmount","eject","status" };

  char *cmd=0;
  asprintf(&cmd,"%s %s %s",mountscript,str[act],basedir);
  bool res=(system(cmd)==0);
  free(cmd);
  return res;
}

bool cFileSource::Mount(void)
{
  bool res=false;
  if(needsmount && (res=Action(acMount))) ClearRemember();
  return res;
}

bool cFileSource::Unmount(void)
{
  bool res=false;
  if(needsmount) {
    if(!useCount && (res=Action(acUnmount))) ClearRemember();
    }
  return res;
}

bool cFileSource::Eject(void)
{
  bool res=false;
  if(needsmount) {
    if(!useCount && (res=Action(acEject))) ClearRemember();
    }
  return res;
}

bool cFileSource::Status(void)
{
  if(needsmount) return Action(acStatus);
  return true;
}

// -- cFileSources --------------------------------------------------------------

bool cFileSources::Load(const char *filename, bool dummy)
{
  if(cConfig<cFileSource>::Load(filename,true,true)) {
    SetSource(First());
    return true;
    }
  return false;
}

cFileSource *cFileSources::FindSource(const char *filename)
{
  cFileSource *src=First();
  while(src) {
    if(startswith(filename,src->RealBaseDir())) return src;
    src=Next(src);
    }
  return 0;
}
