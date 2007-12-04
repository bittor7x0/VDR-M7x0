/*
 * See the README file for copyright information and how to reach the author.
 */

#include <string>
#include <fstream>
#include <iostream>
#include <vdr/plugin.h>
#include <vdr/videodir.h>
#include <vdr/recording.h>
#include "tools.h"
#include "mymenusetup.h"

using namespace std;

#define CONFIGFILE "/extrecmenu.sort.conf"
#define BUFFERSIZE 20972 // (2*1024*1024)/100

MoveBetweenFileSystems MoveThread;

string myStrEscape(string S,const char *Chars)
{
  int i=0;
  while(Chars[i]!=0)
  {
    string::size_type j=0;
    while((j=S.find(Chars[i],j))!=string::npos)
    {
      S.insert(j,1,'\\');
      j+=2;
     }
     i++;
  }
  return S;
}

string myStrReplace(string S,char C1,char C2)
{
  string::size_type i=0;
  while((i=S.find(C1,i))!=string::npos)
  {
    S.replace(i,1,1,C2);
    i++;
  }
  return S;
}

// --- SortList ---------------------------------------------------------------
void SortList::ReadConfigFile()
{
 string configfile(cPlugin::ConfigDirectory());
 configfile+=CONFIGFILE;
 
 ifstream in(configfile.c_str());
 if(in)
 {
  string buf;
  while(!in.eof())
  {
   getline(in,buf);
   if(buf.length()>0)
    Add(new SortListItem(buf.c_str()));
  }
 }
}

void SortList::WriteConfigFile()
{
 string configfile(cPlugin::ConfigDirectory());
 configfile+=CONFIGFILE;
 
 ofstream outfile(configfile.c_str());
 
 for(SortListItem *item=First();item;item=Next(item))
  outfile << item->Path() << endl;
}

bool SortList::Find(char *Path)
{
 for(SortListItem *item=First();item;item=Next(item))
 {
  if(!strcmp(item->Path(),Path))
   return true;
 }
 return false;
}

// --- MoveBetweenFileSystems -------------------------------------------------
MoveBetweenFileSystems::MoveBetweenFileSystems():cThread("moving files between filesystems")
{
}

bool MoveBetweenFileSystems::Start(string OldName,string NewName,cRecording *Recording)
{
  oldname=OldName;
  newname=NewName;
  recording=Recording;
  
  return cThread::Start();
}

bool MoveBetweenFileSystems::IsMoving(string Path)
{
  if(Active())
  {
   if(recording)
    {
      if(Path==oldname)
        return true;
    }
    else
    {
      if(!strncmp(oldname.c_str(),Path.c_str(),oldname.length()))
        return true;
    }
  }
  return false;
}

bool MoveBetweenFileSystems::Move(string From,string To,cRecording *Recording)
{
  if(Recording)
  {
    isyslog("[extrecmenu] moving %s to %s",From.c_str(),To.c_str());
    
    DIR *dir;
    struct dirent *entry;
    dir=opendir(From.c_str());
    while((entry=readdir(dir))!=NULL)
    {
      string from,to;
      from=From+'/'+entry->d_name;
      to=To+'/'+entry->d_name;
      
      struct stat st;
      stat(from.c_str(),&st);
      if(S_ISREG(st.st_mode))
      {
        isyslog("[extrecmenu] moving %s",entry->d_name);
        
        time_t copytime=time(NULL);

        char buf[BUFFERSIZE];
        int infile=-1,outfile=-1;
        struct stat from_stat;
        ssize_t sz,sz_read=1,sz_write;
    
        if(stat(from.c_str(),&from_stat)!=0 ||
           (infile=open(from.c_str(),O_RDONLY))<0 ||
           (outfile=open(to.c_str(),O_WRONLY|O_CREAT|O_EXCL,from_stat.st_mode))<0)
        {
          if(infile>=0)
            close(infile);
          closedir(dir);
          
          Skins.Message(mtError,strerror(errno));
          return false;
        }

        while(sz_read>0 && (sz_read=read(infile,buf,BUFFERSIZE))>0)
        {
          sz_write=0;
          do
          {
            if((sz=write(outfile,buf+sz_write,sz_read-sz_write))<0)
            {
              close(infile);
              close(outfile);
              closedir(dir);

              Skins.Message(mtError,tr("Rename/Move failed!"));
              esyslog("[extrecmenu] %s",strerror(errno));
              return false;
            }
            sz_write+=sz;
          }
          while(sz_write<sz_read);
          
          if(mysetup.LimitBandwidth)
            usleep(10);
        }
        close(infile);
        close(outfile);

        copytime=time(NULL)-copytime;
        isyslog("[extrecmenu] needed %d secs for %d bytes",(int)copytime,(int)st.st_size);
      }
    }
    closedir(dir);
    
    Recordings.AddByName(To.c_str());
    Recording->Delete();
    Recordings.DelByName(From.c_str());
    
    string buf="move \"";
    buf+=myStrEscape(From,"'\\\"$");
    buf+="\"";
    cRecordingUserCommand::InvokeCommand(buf.c_str(),To.c_str());
  }
  else
  {
    string buf;
    buf=From+'/';
    if(!strncmp(buf.c_str(),To.c_str(),buf.length()))
    {
      Skins.Message(mtError,tr("Moving into own sub-directory not allowed!"));
      return false;
    }
    
    myRecList *list=new myRecList();
    for(cRecording *recording=Recordings.First();recording;recording=Recordings.Next(recording))
      list->Add(new myRecListItem(recording));
    
    myRecListItem *item=list->First();
    while(item)
    {
      if(!strncmp(From.c_str(),item->recording->FileName(),From.length()))
      {
        char *buffer=NULL;
        buffer=strdup(From.c_str()+strlen(VideoDirectory)+1);
        buffer=ExchangeChars(buffer,false);
        
        if(strcmp(item->recording->Name(),buffer))
        {
          buf=To+(item->recording->FileName()+From.length());
          if(!MakeDirs(buf.c_str(),true))
          {
            Skins.Message(mtError,tr("Rename/Move failed!"));
            esyslog("[extrecmenu] %s",strerror(errno));
            free(buffer);
            delete list;
            return false;
          }
          if(Move(item->recording->FileName(),buf,item->recording)==false)
          {
            free(buffer);
            delete list;
            return false;
          }
        }
        free(buffer);
      }
      item=list->Next(item);
    }
    delete list;
  }
  return true;
}

void MoveBetweenFileSystems::Action()
{
  Move(oldname,newname,recording);
}

// --- MoveRename -------------------------------------------------------------
// creates the necassery directories and renames the given old name to the new name
bool MoveRename(const char *OldName,const char *NewName,cRecording *Recording,bool Move)
{
  char *buf=NULL;

  if(!strcmp(OldName,NewName))
    return true;

  if(Recording)
  {
    isyslog("[extrecmenu] moving %s to %s",OldName,NewName);
  
    if(rename(OldName,NewName)==-1)
    {
      remove(NewName); // remove created directory
      Skins.Message(mtError,tr("Rename/Move failed!"));
      esyslog("[extrecmenu] %s",strerror(errno));
      return false;
    }

    Recordings.AddByName(NewName);
    Recordings.Del(Recording);
  
    // set user command for '-r'-option of VDR
    asprintf(&buf,"%s \"%s\"",Move?"move":"rename",*strescape(OldName,"'\\\"$"));
    cRecordingUserCommand::InvokeCommand(buf,NewName);
    free(buf);
  }
  else
  {
    // is the new path within the old?
    asprintf(&buf,"%s/",OldName); // we have to append a / to make sure that we search for a directory
    if(!strncmp(buf,NewName,strlen(buf)))
    {
      Skins.Message(mtError,tr("Moving into own sub-directory not allowed!"));
      free(buf);
      return false;
    }
    free(buf);
 
    myRecList *list=new myRecList();
    for(cRecording *recording=Recordings.First();recording;recording=Recordings.Next(recording))
      list->Add(new myRecListItem(recording));

    myRecListItem *item=list->First();
    while(item)
    {
      if(!strncmp(OldName,item->recording->FileName(),strlen(OldName)))
      {
        buf=strdup(OldName+strlen(VideoDirectory)+1);
        buf=ExchangeChars(buf,false);
    
        if(strcmp(item->recording->Name(),buf))
        {
          free(buf);
          asprintf(&buf,"%s%s",NewName,item->recording->FileName()+strlen(OldName));
          if(!MakeDirs(buf,true))
          {
            Skins.Message(mtError,tr("Creating directories failed!"));
            esyslog("[extrecmenu] %s",strerror(errno));
            free(buf);
            delete list;
            return false;
          }
          if(MoveRename(item->recording->FileName(),buf,item->recording,Move)==false)
          {
            free(buf);
            delete list;
            return false;
          }
        }
        free(buf);
      }
      item=list->Next(item);
    }
    delete list;
  }
  return true;
}

// --- myRecListItem ----------------------------------------------------------
bool myRecListItem::SortByName=false;

myRecListItem::myRecListItem(cRecording *Recording)
{
 recording=Recording;
 filename=strdup(recording->FileName());
}

myRecListItem::~myRecListItem()
{
 free(filename);
}

char *myRecListItem::StripEpisodeName(char *s)
{
 char *t=s,*s1=NULL,*s2=NULL;
 while(*t)
 {
  if(*t=='/')
  {
   if(s1)
   {
    if(s2)
     s1=s2;
    s2=t;
   }
   else
    s1=t;
  }
  t++;
 }
 if(mysetup.DescendSorting)
 {
   if(SortByName)
     *s1=1;
   else
     *(s2+1)=255;
 }
 else
   *s1=255;
 
 if(s1&&s2&&!SortByName)
   memmove(s1+1,s2,t-s2+1);

 return s;
}

int myRecListItem::Compare(const cListObject &ListObject)const
{
 myRecListItem *item=(myRecListItem*)&ListObject;
 
 char *s1=StripEpisodeName(strdup(filename+strlen(VideoDirectory)));
 char *s2=StripEpisodeName(strdup(item->filename+strlen(VideoDirectory)));

 int compare;
 if(mysetup.DescendSorting)
   compare=strcasecmp(s2,s1);
 else
   compare=strcasecmp(s1,s2);
 
 free(s1);
 free(s2);
 
 return compare;
}

// --- myRecList --------------------------------------------------------------
void myRecList::Sort(bool SortByName)
{
 myRecListItem::SortByName=SortByName;
 cListBase::Sort();
}
