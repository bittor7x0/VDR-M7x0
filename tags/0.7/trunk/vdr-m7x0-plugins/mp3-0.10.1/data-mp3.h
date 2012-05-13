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

#ifndef ___DATA_MP3_H
#define ___DATA_MP3_H

#include <vdr/thread.h>
#include <vdr/tools.h>
#include "data.h"

// ----------------------------------------------------------------

class cDecoder;
class cSongInfo;

extern const char *img_suff[], *excl_pl[], *excl_br[];

// ----------------------------------------------------------------

class cSong : public cListObject {
public:
  int user;
private:
  cFileObj *obj;
  bool fromDOS, decoderFailed;
  cDecoder *decoder;
  cMutex decLock;
  //
  const char *image;
  //
  void Init(void);
  char *Convert2Unix(const char *name) const;
  bool FindImage(void);
  const char *CheckImage(const char *base) const;
public:
  cSong(cFileObj *Obj);
  cSong(cFileSource *Source, const char *Subdir, const char *Name);
  cSong(cSong *Song);
  ~cSong();
  virtual int Compare(const cListObject &ListObject) const;
  bool Parse(char *s, const char *reldir) const;
  bool Save(FILE *f, const char *reldir) const;
  void Convert(void);
  cSongInfo *Info(bool get=true);
  cDecoder *Decoder(void);
  bool Image(unsigned char * &mem, int &len, bool &alloced);
  inline const char *Name(void) const { return obj->Name(); }
  inline const char *FullPath(void) const { return obj->FullPath(); }
  };

// ----------------------------------------------------------------

class cPlayList : public cList<cSong>, public cListObject {
private:
  bool isWinAmp;
  char *extbuffer;
  //
  void Init(void);
  void Set(void);
  const char *AddExt(const char *Name, const char *Ext);
protected:
  cFileObj *obj;
  char *basename;
public:
  cPlayList(cFileObj *Obj);
  cPlayList(cFileSource *Source, const char *Subdir, const char *Name);
  cPlayList(cPlayList *List);
  ~cPlayList();
  virtual bool Load(void);
  virtual bool Save(void);
#if APIVERSNUM >= 10315
  virtual int Compare(const cListObject &ListObject) const;
#else
  virtual bool operator<(const cListObject &ListObject);
#endif
  //
  bool Rename(const char *newName);
  bool Delete(void);
  bool Create(const char *newName);
  bool Exists(void);
  bool TestName(const char *newName);
  //
  inline const char *Name(void) const { return obj->Name(); }
  inline const char *BaseName(void) const { return basename; }
  inline bool IsWinAmp(void) const { return isWinAmp; }
  };

// ----------------------------------------------------------------

class cInstantPlayList : public cScanDir, public cPlayList {
protected:
  virtual void DoItem(cFileSource *src, const char *subdir, const char *name);
public:
  cInstantPlayList(cFileObj *Obj);
  virtual bool Load(void);
  virtual bool Save(void) { return false; }
  };

// ----------------------------------------------------------------

class cPlayLists : public cScanDir, public cList<cPlayList> {
protected:
  virtual void DoItem(cFileSource *src, const char *subdir, const char *name);
public:
  bool Load(cFileSource *Source);
  };

#endif //___DATA_MP3_H
