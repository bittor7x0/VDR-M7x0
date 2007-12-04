/***************************************************************************
 *   Copyright (C) 2005 by Reel Multimedia                                 *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *
 *************************************************************************** 
 *   
 *   channelscan: A plugin for VDR 
 *
 ***************************************************************************/

#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include <vdr/channels.h>
#include <vdr/device.h>
#include <vdr/submenu.h> // Filefactory

#include "transponders.h"
#include "channelscan.h"
#include "csmenu.h"
#include "debug.h"


using std::string;
using std::cout;
using std::endl;

#define DIRFILE   ?'d':'f'

cDirectoryFiles DirectoryFiles;

// -- Class cDirectoryEntry --------------------------------------
cDirectoryEntry::cDirectoryEntry(const std::string& FileName, bool IsDir)
{
  fileName_ = FileName;
  titleBuffer_ = fileName_.substr(FileName.find_last_of('/')+1);
  isDirectory_ = IsDir;
  toEdit_ = false; // Backup or New
  StripEnding(); // einstelbar? 
}

void cDirectoryEntry::StripEnding()
{
  string::size_type pos =  titleBuffer_.find_last_of('.');
  if (pos != titleBuffer_.npos)
  {
     printf (" errase ending:  %s  \t", titleBuffer_.c_str());
     titleBuffer_.erase(pos);
     printf ("errased:   %s  \n", titleBuffer_.c_str());
  }
}

int cDirectoryEntry::Compare(const cListObject &ListObject) const
{
   cDirectoryEntry *de = (cDirectoryEntry *)&ListObject;
   // printf ("Compare -- %c - %s vs. %d - %s \n", IsDirectory()DIRFILE, Title(), de->IsDirectory()DIRFILE, de->Title());
   if (de->IsDirectory() == IsDirectory()) 
   {
      string title = Title();
      int ret = strcasecmp(title.c_str(),  de->Title());
      //printf ("Compare %s vs. %s returns %d \n", Title(), de->Title(), ret);
      return ret;
   }
   return IsDirectory()?-100:100;
}

// ----------- Class cDirectoryFiles ------------------------

cDirectoryFiles::cDirectoryFiles()
{ 
}

/*
cDirectoryFiles::~cDirectoryFiles()
{ 
    printf (" Destroy ~cDirectoryFiles \n");
    for (cDirectoryEntry *de = First(); de; de = Next(de))
    {
       if (de->ToEdit())
       {
         
       }
          
    }
  
} */

bool cDirectoryFiles::Load(bool oldPath)
{
      printf (" Load( bool ) \"%s\"\n", oldPath?"Back":"Channels");

   if (!oldPath)
   {
      path_ = setup::FileNameFactory("channels");
      printf (" Load(\"%s\"\n", path_.c_str());
      return Load(path_);
   }
   else 
   {
      printf (" Load(\"%s\"\n", oldPath_.c_str());
      string tmp =  oldPath_; 
      oldPath_ = string(oldPath_, oldPath_.find_last_of('/'));
      return Load(tmp);
   }
}

bool cDirectoryFiles::Load(string& Path)
{
  
  printf (" Load (%s) \n", Path.c_str());

  Clear();

  DIR *dirp;
  struct dirent *dp;
  struct stat buf;
  if (!Path.empty())
  {
     oldPath_ = path_;
     path_ = Path;
  }
  else  // relaod 
  {
    Path = path_;
    printf (" Reload (%s) \n", Path.c_str());
  }


  if ((dirp = opendir(Path.c_str())) == NULL) 
  {
     printf (" Load () error open %s \n", Path.c_str());
     esyslog (" error open %s ", Path.c_str());
     return false;
  }
          
  while ((dp = readdir(dirp)) != NULL)
  {
     printf ("  readdir: %s, \n",dp->d_name);
     string dName = dp->d_name;
     if (dName.find('.') == 0) 
         continue;

     string path = Path + "/" + dName;

     if (stat(path.c_str(), &buf) != 0)
     {
        int err = errno;
        printf (" Error: %s errno %d ", path.c_str(), err);
        if (errno == ENOENT)
           printf(" :No such file or directory \n");
        else 
           printf ("\n");
     }

     if (S_ISDIR(buf.st_mode))
     {
        //printf (" Add DIR new dirEntry %s, \t",dp->d_name);
        //printf ("Path: %s\n", path.c_str());
        Add (new cDirectoryEntry(path, true));
     }
     if (S_ISREG(buf.st_mode))
     {
        printf (" Add FILE new dirEntry %s, isFile \t",dp->d_name);
        if (dName.find(".conf") != string::npos ) 
        {
          //  printf ("Path: %s\n", path.c_str());
           Add(new cDirectoryEntry(path, false));
        }
        else 
        {
          isyslog("Channelscan: no (channels).config file %s \n", dName.c_str());
        }
        
    }
  }

  closedir(dirp);
  Sort();
  return true;
}

