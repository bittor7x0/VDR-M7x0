/*
 * Extends vdr epg by modes of scanning.
 *
 * Copyright (C) 2008 Andreas Koch - the open7x0.org group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 *
 */

#ifndef __EPGMODE_H
#define __EPGMODE_H
#include "config.h"
#include "channels.h"
#include "thread.h"
#include "tools.h"


enum eEpgMode { emDefault, emInvDefault, emFull, emForeign,
    emNoSave, emNowNext, emNone};

class cEpgMode : public cListObject {
private:
  eEpgMode mode;
  tChannelID channelID;
public:
  cEpgMode(void) { mode = emDefault; }
  cEpgMode(const tChannelID *ChannelID, eEpgMode Mode = emDefault)
    { channelID = *ChannelID; mode = Mode; }
  eEpgMode GetMode(bool replaceDefault = true) const;
  void SetMode(eEpgMode Mode) { mode = Mode; }
  bool Valid(void) const
   { return channelID.Valid() && mode >= emDefault && mode <= emNone &&
          Channels.GetByChannelID(channelID); }
  bool operator== (const tChannelID &ChannelID) const
      { return channelID == ChannelID; }
  bool operator== (const cEpgMode &arg) const
      { return channelID == arg.channelID; }
  bool Parse(char *s);
  bool Save(FILE *f) const;
};

class cEpgModes : public cConfig<cEpgMode> {
private:
  cMutex mutex;
  void Cleanup(void);
public:
  cEpgMode *GetModeByChannelID(const tChannelID *ChannelID);
  bool Load(const char *FileName = NULL);
  bool Save(void);
};

extern cEpgModes EpgModes;
#endif
