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
 * $Id: epgmode.c 224 2008-02-17 14:53:58Z andreas $
 *
 */
#include <stdio.h>
#include <string.h>
#include "epgmode.h"
#include "channels.h"
#include "tools.h"


eEpgMode cEpgMode::GetMode(bool replaceDefault) const
{
  if (!replaceDefault || mode > emInvDefault)
     return mode;

  eEpgMode em = mode;
  cChannel *channel = Channels.GetByChannelID(channelID, true);
  if (channel && channel->Number() > Setup.EPGChannelNoLimit)
     em = em == emDefault ? emInvDefault : emDefault;
  if (em == emDefault)
     return eEpgMode(Setup.EPGDefaultMode + emFull);
  return eEpgMode(Setup.EPGInvDefaultMode + emFull);
}


bool cEpgMode::Parse(char *s) {

  compactspace(s);
  char *p = s + strcspn(s," \t");
  if (!*p)
     return false;

  *p++ = 0;
  compactspace(s);
  compactspace(p);
  channelID = tChannelID::FromString(s);
  mode = eEpgMode(atoi(p));
  return true;
}

bool cEpgMode::Save(FILE *f) const {
  return fprintf(f,"%s %d\n",*channelID.ToString(),mode) > 0;
}

void cEpgModes::Cleanup(void) {
  cEpgMode *cur = First();
  while (cur) {
        if (!cur->Valid()) {
           cEpgMode *next = Next(cur);
           Del(cur);
           cur = next;
           continue;
           }
        // Check for duplicates
        cEpgMode  *oth = Next(cur);
        while (oth) {
              cEpgMode *next = Next(oth);
              if (*cur == *oth) {
                 Del(oth);
                 }
              oth = next;
              }
        cur = Next(cur);
        }
}

cEpgMode *cEpgModes::GetModeByChannelID(const tChannelID *ChannelID)
{
  mutex.Lock();
  for (cEpgMode *cur = First(); cur; cur = Next(cur)) {
      if (*cur == *ChannelID) {
         mutex.Unlock();
         return cur;
         }
      }

  cEpgMode *cur = new cEpgMode(ChannelID);
  Add(cur);
  mutex.Unlock();
  return cur;
}

bool cEpgModes::Load(const char* FileName)
{
  mutex.Lock();
  bool r = cConfig<cEpgMode>::Load(FileName);
  Cleanup();
  mutex.Unlock();
  return r;
}

bool cEpgModes::Save(void)
{
  mutex.Lock();
  Cleanup();
  bool r = cConfig<cEpgMode>::Save();
  mutex.Unlock();
  return r;
}

cEpgModes EpgModes;
