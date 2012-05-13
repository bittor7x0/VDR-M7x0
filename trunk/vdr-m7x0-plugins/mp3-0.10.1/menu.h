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

#ifndef ___MENU_H
#define ___MENU_H

#include "common.h"
#if APIVERSNUM >= 10307
#include <vdr/osdbase.h>
#else
#include <vdr/osd.h>
#endif

// ----------------------------------------------------------------

void Status(const char *text);
void Error(const char *text);
void Info(const char *text);

// ----------------------------------------------------------------

class cFileSources;
class cFileSource;
class cFileObj;
class cDirList;

// ----------------------------------------------------------------

class cMenuBrowse : public cOsdMenu {
private:
  eOSState Select(bool isred);
  eOSState Parent(void);
  bool LoadDir(const char *dir);
protected:
  static cFileObj *lastselect;
  //
  cDirList *list;
  cFileSource *source;
  bool dirselectable, withID3;
  char *currentdir, *parent;
  const char * const *excl;
//
  bool NewDir(const char *dir);
  void SetSource(cFileSource *Source);
  cFileObj *CurrentItem(void);
  virtual void SetButtons(void);
  virtual eOSState ID3Info(void);
  virtual eOSState ProcessStdKey(eKeys Key, eOSState state);
public:
  cMenuBrowse(cFileSource *Source, bool Dirselect, bool WithID3, const char *title, const char * const *Excl);
  ~cMenuBrowse();
  static cFileObj *GetSelected(void) { return lastselect; }
  };

// ----------------------------------------------------------------

class cMenuSource : public cOsdMenu {
private:
  static cFileSource *selected;
  cFileSource *current;
  //
  eOSState Mount(void);
  eOSState Unmount(void);
  eOSState Eject(void);
  eOSState Select(void);
  bool DoMount(cFileSource *src);
  bool CheckMount(void);
public:
  cMenuSource(cFileSources *Sources, const char *title);
  virtual eOSState ProcessKey(eKeys Key);
  static cFileSource *GetSelected(void) { return selected; }
  };

// ----------------------------------------------------------------

class cProgressBar : public cBitmap {
public:
  cProgressBar(int Width, int Height, int Current, int Total);
  };

#endif //___MENU_H
