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
 *   channelscan: plugin for VDR 
 *
 ***************************************************************************/

#ifndef __DIRFILES_H
#define __DIRFILES_H

#include <errno.h>

#include <vdr/plugin.h>
#include <vdr/thread.h>
#include <vector>
#include <string>


#undef  DBG
#undef  ERR
#define DBG " Channelscan DEBUG: -- "
#define ERR " Channelscan ERROR: -- "

#ifdef DEBUG_CHANNELSCAN
#  undef DLOG 
#  define DLOG(x...) dsyslog(x)
#else
# define DLOG(x...)
#endif

// -- Class cDirectoryEntry --------------------------------------

class cDirectoryEntry : public cListObject
{
private:
  std::string fileName_;
  std::string titleBuffer_;
  std::string name_;
  std::string setEdit_;
  bool isDirectory_;
  bool toEdit_;
  int level_;
public: 
  cDirectoryEntry(const std::string& FileName, bool IsDir);
  ~cDirectoryEntry() { }; 
  void StripEnding();
  const char *Title() const; 
  const char *FileName() const;
  bool IsDirectory() const;
  int Level() const;
  int Compare(const cListObject &ListObject) const;
  void SetEdit(); 
  bool ToEdit() const;

  // returns the directory name
};

// -- inline definitions cDirectoryEntry --------------------------------------

inline const char *cDirectoryEntry::Title() const
{ 
  return titleBuffer_.c_str();
}

inline const char *cDirectoryEntry::FileName() const
{
  return fileName_.c_str();
}

inline int cDirectoryEntry::Level() const
{
  return level_;
}

inline bool cDirectoryEntry::IsDirectory() const
{
  return isDirectory_;
}
inline void cDirectoryEntry::SetEdit()
{
  toEdit_ = true;;
}
inline bool cDirectoryEntry::ToEdit() const
{
  return toEdit_;
}


// -- Class cDirectoryFiles --------------------------------------

class cDirectoryFiles : public cList<cDirectoryEntry>
{ 
private:
  std::string path_;
  std::string oldPath_;
public:
  cDirectoryFiles();
  ~cDirectoryFiles() { };
  bool Load(bool oldDir = false);
  bool Load(std::string& Path);
  void DumpEntries();
};

extern cDirectoryFiles DirectoryFiles;

// -- inline definitions cDirectoryEntry --------------------------------------

inline void cDirectoryFiles::DumpEntries()
{
    printf  ("\n\n ***  Dump cDirectoryEntry: \"%s\"  ***\n", path_.c_str());
    for (cDirectoryEntry *de = First(); de; de = Next(de))
    {
       //printf (" -- %c - e:%s, t:%s\n", de->IsDirectory()?'d':'f', de->Entry(), de->Title());
       printf (" -- %c - t:%s\n", de->IsDirectory()?'d':'f', de->Title());
    }
}


#endif // __DIRFILES_H

