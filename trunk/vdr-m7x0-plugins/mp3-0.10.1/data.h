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

#ifndef ___DATA_H
#define ___DATA_H

#include <vdr/tools.h>

// ----------------------------------------------------------------

class cFileSource;

extern char *Quote(const char *str);
extern char *AddPath(const char *dir, const char *filename);
extern bool CheckVDRVersion(int Version, int Major, int Minor, const char *text=0);

// ----------------------------------------------------------------

class cScanDir {
protected:
  enum eScanType { stFile, stDir };
  virtual void DoItem(cFileSource *src, const char *subdir, const char *name)=0;
public:
  virtual ~cScanDir() {}
  bool ScanDir(cFileSource *src, const char *subdir, eScanType type, const char * const *spec, const char * const *excl, bool recursiv);
  };

// ----------------------------------------------------------------

enum eObjType { otDir, otParent, otFile, otBase };

class cFileObj : public cListObject {
private:
  cFileSource *source;
  char *subdir, *name, *path, *fpath;
  eObjType type;
  //
  void Set(void);
  void MakeFullName(char **fp, const char *Name);
public:
  cFileObj(cFileSource *Source, const char *Subdir, const char *Name, const eObjType Type);
  cFileObj(const cFileObj *obj);
  virtual ~cFileObj();
#if APIVERSNUM >= 10315
  virtual int Compare(const cListObject &ListObject) const;
#else
  virtual bool operator<(const cListObject &ListObject);
#endif
  void SetName(const char *Name);
  void SplitAndSet(const char *Path);
  bool GuessType(void);
  //
  bool Exists(void);
  bool TestName(const char *newName);
  bool Rename(const char *newName);
  bool Create(const char *newName);
  bool Delete(void);
  //
  inline const char *Name(void) const { return name; }
  inline const char *Subdir(void) const { return subdir; }
  inline cFileSource *Source(void) const { return source; }
  inline eObjType Type(void) const { return type; }
  inline const char *Path(void) const { return path; }
  inline const char *FullPath(void) const { return fpath; }
  };

// ----------------------------------------------------------------

class cDirList : public cScanDir, public cList<cFileObj> {
private:
  eObjType otype;
protected:
  virtual void DoItem(cFileSource *src, const char *subdir, const char *name);
public:
  bool Load(cFileSource *src, const char *subdir, const char * const *excl=0);
  };

// ----------------------------------------------------------------

class cFileSource : public cListObject {
private:
  enum eAction { acMount, acUnmount, acEject, acStatus };
  char *basedir, *realbasedir;
  char *description;
  char **include;
  bool needsmount;
  int useCount, incCount;
  // remember last browse position
  char *browsedir, *browseparent;
  //
  void Set(const char *Basedir, const char *Description, const bool NeedsMount, const char *Include);
  bool Action(eAction act);
  void ClearRemember(void);
  void Clear(void);
public:
  cFileSource(void);
  cFileSource(const char *Basedir, const char *Description, const bool NeedsMount, const char *Include=0);
  ~cFileSource();
  bool Parse(char *s);
  bool Mount(void);
  bool Unmount(void);
  bool Eject(void);
  bool Status(void);
  void Block(void) { useCount++; }
  void Unblock(void) { useCount--; }
  void SetRemember(const char *dir, const char *parent);
  bool GetRemember(char * &dir, char * &parent);
  inline const char *BaseDir(void) const { return basedir; }
  inline const char *RealBaseDir(void) const { return realbasedir; }
  inline const char *Description(void) const { return description; }
  inline const char * const * Include(void) const { return include; }
  inline bool NeedsMount(void) const { return needsmount; }
  };

#endif //___DATA_H
