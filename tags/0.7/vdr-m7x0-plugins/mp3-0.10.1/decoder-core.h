/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
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

#ifndef ___DECODER_CORE_H
#define ___DECODER_CORE_H

#include "decoder.h"

#define DEC_ID(a,b,c,d) (((a)<<24)+((b)<<16)+((c)<<8)+(d))

// ----------------------------------------------------------------

enum eDecodeStatus { dsOK=0, dsPlay, dsStop, dsEof, dsError };

struct DecoderFrame {
  eDecodeStatus status;
  uint64_t offset;
  const uint8_t *pes_frame;
  int pes_frame_length;
  uint32_t frame_param;
  };

// ----------------------------------------------------------------

#define CACHE_VERSION 7

class cCacheData : public cSongInfo, public cFileInfo, public cListObject {
friend class cInfoCache;
private:
  int hash, version;
  time_t touch;
  cMutex lock;
protected:
  bool Save(FILE *f);
  bool Load(FILE *f);
  bool Upgrade(void);
  void Touch(void);
  bool Purge(void);
  void Create(cFileInfo *fi, cSongInfo *si);
public:
  cCacheData(void);
  void Lock(void) { lock.Lock(); }
  void Unlock(void) { lock.Unlock(); }
  };

// ----------------------------------------------------------------

#endif //___DECODER_CORE_H
