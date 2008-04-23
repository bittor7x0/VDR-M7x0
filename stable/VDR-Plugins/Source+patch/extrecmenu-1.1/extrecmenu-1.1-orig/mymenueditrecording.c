/*
 * See the README file for copyright information and how to reach the author.
 */

#include <sys/statvfs.h>
#include <string>
#include <vdr/videodir.h>
#include <vdr/remote.h>
#include <vdr/menu.h>
#include <vdr/interface.h>
#include "mymenurecordings.h"
#include "tools.h"

using namespace std;

// --- myMenuRenameRecording --------------------------------------------------
myMenuRenameRecording::myMenuRenameRecording(cRecording *Recording,const char *DirBase,const char *DirName):cOsdMenu(tr("Rename"),12)
{
 isdir=false;
 recording=Recording;
 dirbase=DirBase?ExchangeChars(strdup(DirBase),true):NULL;
 dirname=DirName?ExchangeChars(strdup(DirName),true):NULL;
 strn0cpy(name,"",sizeof(name));
 strn0cpy(path,"",sizeof(path));

 if(recording)
 {
  char *p=strrchr(recording->Name(),'~');
  if(p)
  {
   strn0cpy(name,++p,sizeof(name));
   strn0cpy(path,recording->Name(),sizeof(path));
   
   p=strrchr(path,'~');
   if(p)
    *p=0;
  }
  else
   strn0cpy(name,recording->Name(),sizeof(name));  
 }
 else
 {
  isdir=true;
  strn0cpy(name,DirName,sizeof(name));
  if(DirBase)
   strn0cpy(path,DirBase,sizeof(path));
 }
 Add(new cMenuEditStrItem(tr("Name"),name,sizeof(name),tr(FileNameChars)));
 cRemote::Put(kRight);
}

myMenuRenameRecording::~myMenuRenameRecording()
{
 free(dirbase);
 free(dirname);
}

eOSState myMenuRenameRecording::ProcessKey(eKeys Key)
{
  eOSState state=cOsdMenu::ProcessKey(Key);
  if(state==osContinue)
  {
    if(Key==kOk)
    {
      char *oldname=NULL;
      char *newname=NULL;
      char *tmppath=path[0]?ExchangeChars(strdup(path),true):NULL;
      char *tmpname=name[0]?ExchangeChars(strdup(name),true):NULL;

      if(strchr(name,'.')==name||!strlen(name))
      {
        Skins.Message(mtError,tr("Invalid filename!"));
        cRemote::Put(kRight);
        return osContinue;
      }

      if(isdir)
        asprintf(&oldname,"%s%s%s/%s",VideoDirectory,tmppath?"/":"",dirbase?dirbase:"",dirname);
      else
        oldname=strdup(recording->FileName());

      asprintf(&newname,"%s%s%s/%s%s",VideoDirectory,tmppath?"/":"",tmppath?tmppath:"",tmpname,isdir?"":strrchr(recording->FileName(),'/'));

      if(!MakeDirs(newname,true))
        Skins.Message(mtError,tr("Creating directories failed!"));
      else
      {
        if(MoveRename(oldname,newname,isdir?NULL:recording,false))
          state=osBack;
        else
        {
          cRemote::Put(kRight);
          state=osContinue;
        }
      }
      free(oldname);
      free(newname);
      free(tmppath);
      free(tmpname);
    }
    if(Key==kBack)
      return osBack;
  }
  return state;
}

// --- myMenuNewName ----------------------------------------------------------
bool myMenuMoveRecording::clearall=false;
char newname[128];

class myMenuNewName:public cOsdMenu
{
 private:
  char name[128];
 public:
  myMenuNewName();
  virtual eOSState ProcessKey(eKeys Key);
};

myMenuNewName::myMenuNewName():cOsdMenu(tr("New folder"),12)
{
 strn0cpy(name,tr("New folder"),sizeof(name));
 Add(new cMenuEditStrItem(tr("Name"),name,sizeof(name),tr(FileNameChars)));
 cRemote::Put(kRight);
}

eOSState myMenuNewName::ProcessKey(eKeys Key)
{
 eOSState state=cOsdMenu::ProcessKey(Key);
 
 if(state==osContinue)
 {
  if(Key==kOk)
  {
   if(strchr(name,'.')==name||!strlen(name))
   {
    Skins.Message(mtError,tr("Invalid filename!"));
    cRemote::Put(kRight);
    state=osContinue;
   }
   else
   {
    strn0cpy(newname,name,sizeof(newname));
    state=osBack;
   }
  }
  if(Key==kBack)
   state=osBack;
 }
 
 return state;
}

// --- myMenuMoveRecordingItem ------------------------------------------------
class myMenuMoveRecordingItem:public cOsdItem
{
 private:
  int level;
  char *title;
 public:
  myMenuMoveRecordingItem(const char *Title,int Level);
  myMenuMoveRecordingItem(cRecording *Recording,int Level);
  int Level(){return level;}
  void SetLevel(int Level){level=Level;}
};

myMenuMoveRecordingItem::myMenuMoveRecordingItem(const char *Title,int Level)
{
 level=Level;
 title=strdup(Title);
 SetText(title);
}

myMenuMoveRecordingItem::myMenuMoveRecordingItem(cRecording *Recording,int Level)
{
 level=0;

 const char *s=Recording->Name();
 while(*++s)
 {
  if(*s=='~')
   level++;
 }
 if(Level<level)
 {
  s=Recording->Name();
  const char *p=s;
  while(*++s)
  {
   if(*s=='~')
   {
    if(Level--)
     p=s+1;
    else
     break;
   }
  }
  title=MALLOC(char,s-p+1);
  strn0cpy(title,p,s-p+1);
  SetText(title);
 }
 else
  SetText("");
}

// --- myMenuMoveRecording ----------------------------------------------------
myMenuMoveRecording::myMenuMoveRecording(cRecording *Recording,const char *DirBase,const char *DirName,const char *Base,int Level):cOsdMenu(Base?Base:"")
{
 dirbase=DirBase?strdup(DirBase):NULL;
 dirname=DirName?strdup(DirName):NULL;
 strn0cpy(newname,"",sizeof(newname));
 recording=Recording;
 base=Base?strdup(Base):NULL;
 
 level=Level;
 Set();
 SetHelp(tr("Button$Cancel"),NULL,tr("Button$Create"),tr("Button$Move"));
}

myMenuMoveRecording::~myMenuMoveRecording()
{
 free(base);
 free(dirbase);
 free(dirname);
}

void myMenuMoveRecording::Set()
{
  if(level==0)
    Add(new myMenuMoveRecordingItem(tr("[base dir]"),0));

  cThreadLock RecordingsLock(&Recordings);
  Recordings.Sort();

  char *lastitemtext=NULL;
  myMenuMoveRecordingItem *lastitem=NULL;
  for(cRecording *recording=Recordings.First();recording;recording=Recordings.Next(recording))
  {
    if(!base||(strstr(recording->Name(),base)==recording->Name()&&recording->Name()[strlen(base)]=='~'))
    {
      myMenuMoveRecordingItem *item=new myMenuMoveRecordingItem(recording,level);
      if(*item->Text())
      {
        if(lastitemtext&&!strcmp(lastitemtext,item->Text())) // same text
        {
          if(lastitem&&lastitem->Level()<item->Level()) // if level of the previous item is lower, set it to the new value
            lastitem->SetLevel(item->Level());

          delete item;
        }
        else
        {
          Add(item); // different text means a new item to add
          lastitem=item;
          free(lastitemtext);
          lastitemtext=strdup(lastitem->Text());
        }
      }
      else
        delete item;
    }
  }
  free(lastitemtext);
}

eOSState myMenuMoveRecording::Open()
{
 myMenuMoveRecordingItem *item=(myMenuMoveRecordingItem*)Get(Current());
 if(item)
 {
  if(item->Level()>level)
  {
   const char *t=item->Text();
   char buffer[MaxFileName];
   if(base)
   {
    snprintf(buffer,sizeof(buffer),"%s~%s",base,t);
    t=buffer;
   }
   return AddSubMenu(new myMenuMoveRecording(recording,dirbase,dirname,t,level+1));
  }
 }
 return osContinue;
}

eOSState myMenuMoveRecording::MoveRec()
{
  char *oldname=NULL;
  char *newname=NULL;
  char *dir=NULL;
  char *tmpdirbase=dirbase?ExchangeChars(strdup(dirbase),true):NULL;
  char *tmpdirname=dirname?ExchangeChars(strdup(dirname),true):NULL;
 
  eOSState state=osContinue;

  if(dirname)
    asprintf(&oldname,"%s%s%s/%s",VideoDirectory,dirbase?"/":"",tmpdirbase?tmpdirbase:"",tmpdirname);
  else
    oldname=strdup(recording->FileName());

  myMenuMoveRecordingItem *item=(myMenuMoveRecordingItem*)Get(Current());
  if(item)
  {
    if(strcmp(tr("[base dir]"),item->Text()))
    {
      if(dirname)
        asprintf(&dir,"%s%s%s",base?base:"",base?"~":"",item->Text());
      else  // needed for move recording menu
      {
        char *p=strrchr(recording->Name(),'~');
        asprintf(&dir,"%s%s%s~%s",base?base:"",base?"~":"",item->Text(),p?p+1:recording->Name());
      }
    }
    else
    {
      if(!dirname)
      {
        char *p=strrchr(recording->Name(),'~');
        asprintf(&dir,"%s",p?++p:recording->Name());
      }
    }
  }
  else
  {
    if(dirname)
      asprintf(&dir,"%s",base);
    else
    {
      char *p=strrchr(recording->Name(),'~');
      asprintf(&dir,"%s~%s",base,p?p:recording->Name());
    }
  }
  if(dir)
    dir=ExchangeChars(dir,true);
 
  asprintf(&newname,"%s%s%s%s",VideoDirectory,dir?"/":"",dir?dir:"",strrchr(dirname?oldname:recording->FileName(),'/'));

  // getting existing part of target path
  string path=newname;
  string::size_type pos=string::npos;
  do
    pos=path.rfind('/',pos)-1;
  while(access(path.substr(0,pos+1).c_str(),R_OK));
  
  path=path.substr(0,pos+1);

  struct stat stat1,stat2;
  stat(oldname,&stat1);
  stat(path.c_str(),&stat2);
  // are source and target at the same filesystem?
  if(stat1.st_dev==stat2.st_dev)
  {
    if(MoveRename(oldname,newname,dirname?NULL:recording,true))
    {
      clearall=true;
      state=osBack;
    }
  }
  else
  {
    struct statvfs fsstat;
    if(!statvfs(path.c_str(),&fsstat))
    {
      int freemb=int(fsstat.f_bavail/(1024.0*1024.0/fsstat.f_bsize));
      int recmb=0;

      // moving a single recording
      if(recording)
      {
        recmb=DirSizeMB(recording->FileName());
        if(freemb-recmb > 0  || Interface->Confirm(tr("Target filesystem filled - try anyway?")))
        {
          MoveCutterThread->AddToMoveList(oldname,newname);
          clearall=true;
          state=osBack;
        }
      }
      // moving a directory
      else
      {
        string buf=oldname;
        buf+="/";
        if(!buf.compare(0,buf.length(),newname))
          Skins.Message(mtError,tr("Moving into own sub-directory not allowed!"));
        else
        {
          cThreadLock RecordingsLock(&Recordings);
          for(cRecording *rec=Recordings.First();rec;rec=Recordings.Next(rec))
          {
            if(!strncmp(oldname,rec->FileName(),strlen(oldname)))
              recmb+=DirSizeMB(rec->FileName());
          }

          if(freemb-recmb > 0  || Interface->Confirm(tr("Target filesystem filled - try anyway?")))
          {
            for(cRecording *rec=Recordings.First();rec;rec=Recordings.Next(rec))
            {
              if(!strncmp(oldname,rec->FileName(),strlen(oldname)))
              {
                char *buf=ExchangeChars(strdup(oldname+strlen(VideoDirectory)+1),false);
              
                if(strcmp(rec->Name(),buf))
                {
                  free(buf);
                  asprintf(&buf,"%s%s",newname,rec->FileName()+strlen(oldname));
                
                  MoveCutterThread->AddToMoveList(rec->FileName(),buf);
                }
                free(buf);
              }
            }
            clearall=true;
            state=osBack;
          }
        }
      }
    }
    else
    {
      Skins.Message(mtError,tr("Can't get filesystem information"));
      esyslog("[extrecmenu] %s",strerror(errno));
    }
  }
  free(oldname);
  free(newname);
  free(dir);
  free(tmpdirbase);
  free(tmpdirname);
 
  return state;
}

eOSState myMenuMoveRecording::Create()
{
 return AddSubMenu(new myMenuNewName);
}

eOSState myMenuMoveRecording::ProcessKey(eKeys Key)
{
 eOSState state=cOsdMenu::ProcessKey(Key);
 
 if(state==osUnknown)
 {
  switch(Key)
  {
   case kRed: clearall=true;break;
   case kYellow: return Create();
   case kBlue: return MoveRec();
   case kOk: return Open();
   default: break;
  }
 }

 if(newname[0]!=0)
 {
  Add(new myMenuMoveRecordingItem(newname,level+2));
  Display();
  strn0cpy(newname,"",sizeof(newname));
 }

 if(clearall)
  return osBack;

 return state;
}

// --- myMenuRecordingDetails -------------------------------------------------
myMenuRecordingDetails::myMenuRecordingDetails(cRecording *Recording):cOsdMenu(tr("Details"),12)
{
 recording=Recording;
 priority=recording->priority;
 lifetime=recording->lifetime;

 Add(new cMenuEditIntItem(tr("Priority"),&priority,0,MAXPRIORITY));
 Add(new cMenuEditIntItem(tr("Lifetime"),&lifetime,0,MAXLIFETIME));
}

eOSState myMenuRecordingDetails::ProcessKey(eKeys Key)
{
 eOSState state=cOsdMenu::ProcessKey(Key);
 if(state==osUnknown)
 {
  if(Key==kOk)
  {
   char *oldname=strdup(recording->FileName());
   char *newname=strdup(recording->FileName());

   sprintf(newname+strlen(newname)-9,"%02d.%02d.rec",priority,lifetime);

   if(MoveRename(oldname,newname,recording,false))
    state=osBack;
   else
    state=osContinue;

   free(oldname);
   free(newname);
  }
 }
 return state;
}
