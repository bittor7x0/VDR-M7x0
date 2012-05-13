/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
 * (C) 2009 Andreas Koch <andreas@open7x0.org> 
 *
 * Andreas: Rewrite for open7x0 hw nearly the complete file
 * update to gpl version 3 (using option of gpl version 2 license text)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ___DECODER_MP3_H
#define ___DECODER_MP3_H

#define DEC_MP3     DEC_ID('M','P','3',' ')
#define DEC_MP3_STR "MP3"

#include "decoder.h"
#include "decoder-core.h"
#include <vdr/thread.h>
#include <vdr/ringbuffer.h>

class cStream;
struct mp3_frame;
struct mp3_vbr_header;

class cID3Reader;
class cMP3FrameSyncer;
// ----------------------------------------------------------------


class cScanID3 : public cSongInfo {
friend class cMP3Decoder;
private:
  time_t lastCacheUpdate;
  cID3Reader *id3Reader;
  bool CheckInfoCache(cStream *str);
  void UpdateInfo(const mp3_frame *frame, int newFrames, int newTotal);
  void UpdateCache(cStream *str);
  void SetFromID3Reader(cID3Reader *reader, cStream *str);
protected:
  bool modified;
  virtual bool DoScan(const char *filename);
  virtual void InfoHook(void) {}
public:
  cScanID3();
  virtual ~cScanID3() {  }
  };

// ----------------------------------------------------------------
#define MAX_MP3_PES_FRAME_SIZE 4096

class cMP3Decoder : public cDecoder,public cThread, cRingBuffer {
private:
  struct PesBuffer {
    int64_t offset;
    int pes_frame_length;
    uint32_t frame_param;
    uint8_t pes_frame_buffer[MAX_MP3_PES_FRAME_SIZE];
    };

  struct PesBuffer *pesBuffer;
  const struct mp3_vbr_header *vbr_header;
  struct DecoderFrame decoderFrame;
  const struct mp3_frame *cur_frame;

  eDecodeStatus cur_state;
  bool haveID3;
  bool on;

  int64_t dataOffset;
  int64_t dataSize;

  uint64_t playedFramesSizes;
  uint32_t playedFrames;
  uint32_t samples_per_frame;
  uint32_t sample_rate;
  uint32_t totalFrames;
  uint32_t totalTime;
  uint32_t avgFrameSize;

  volatile uint32_t newSeek;
  volatile uint32_t seekOffset;
  volatile int putIndex;
  volatile int getIndex;
  volatile bool getDropInProgress;
  volatile bool disableGet;

  cID3Reader *id3Reader;
  cMP3FrameSyncer *frameSyncer;

  void InitCalc(void);
  bool InitBuffer(void);
  bool Init(void);
  void Clean(void);

  void CalcTimes(void);
  void UpdateInfos(void);

  bool PutFrame(void);
  bool ClearSafe(void);
protected:
  cStream *str;
  cScanID3 *scan;
  
  bool isStream;
  virtual int Available(void) {
     const int diff = putIndex - getIndex;
     return (diff >= 0) ? diff : Size() + diff;
     }
  virtual int Free(void) {
     const int diff = getIndex - putIndex;
     return (diff > 0) ? diff : Size() + diff;
     }
  virtual void Clear(void) { putIndex = getIndex; }
  virtual void Action(void);
public:
  cMP3Decoder(const char *Filename, bool preinit=true);
  virtual ~cMP3Decoder();
  virtual bool Valid(void);
  virtual cFileInfo *FileInfo(void);
  virtual cSongInfo *SongInfo(bool get);
  virtual bool Activate(bool On);
  virtual bool Seek(int Seconds);
  virtual uint32_t GetTimes(uint64_t offset, uint32_t &TotalTime);
  virtual const struct DecoderFrame *GetFrame(void);
  virtual void DropFrame(void);
  virtual bool BufferFilled(void) { 
     const int size = Size();
     const int diff = putIndex - getIndex;
     return ((diff >= 0) ? diff : size + diff) > size / 2;
     }
  virtual const uint8_t *GetImage(int &length);
};
#endif //___DECODER_MP3_H
