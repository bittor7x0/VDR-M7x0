/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
 * (C) 2009 Andreas Koch <andreas@open7x0.org> 
 *
 * Andreas: Rewrite for open7x0 hw the player code 
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

#ifndef ___DVB_MP3_H
#define ___DVB_MP3_H

#include <vdr/thread.h>
#include <vdr/player.h>

// -------------------------------------------------------------------

struct frame {
	  uchar *data; //Pointer to the data
	  int count;   //Size of frame
	  int index;   //Position of frame
};
// -------------------------------------------------------------------

class cPlayList;
class cSong;
class cSongInfo;
class cDecoder;
class cOutputDvb;
class cShuffle;

// -------------------------------------------------------------------

class cMP3PlayInfo {
public:
  char Title[64], Artist[64], Album[64], SMode[32], Filename[256];
  int Year, SampleFreq, Bitrate, MaxBitrate;
  int Num, MaxNum;
  // not in hash
  bool Loop, Shuffle;
  int Hash;
  };

// -------------------------------------------------------------------

class cPlayManager : public cThread {
private:
  cMutex listMutex;
  cCondVar fgCond, bgCond;
  cList<cSong> list;
  cSong *curr;
  int currIndex, maxIndex;
  //
  cSong *play;
  bool playNew, eol;
  //
  cShuffle *shuffle;
  bool shuffleMode, loopMode;
  //
  cSong *scan;
  bool stopscan, throttle, pass2, release;
  //
  virtual void Action(void);
  void NoScan(cSong *nono);
  void NoPlay(cSong *nono);
  void ThrottleWait(void);
public:
  cPlayManager(void);
  ~cPlayManager();
  // Control interface (to be called from frontend thread only!)
  void Flush(void);
  void Add(cPlayList *pl);
  bool Next(void);
  bool Prev(void);
  void Goto(int num);
  void ToggleShuffle(void);
  void ToggleLoop(void);
  bool Info(int num, cMP3PlayInfo *info);
  void Halt(void);
  // Player interface (to be called from player thread only!)
  cSong *Current(void);
  bool NewCurrent(void);
  bool NextCurrent(void);
  void Release(void);
  void Throttle(bool thr);
  };

extern cPlayManager *mgr;


// -------------------------------------------------------------------

class cMP3Player : public cPlayer, cThread {
private:
  enum ePlayMode { pmPlay, pmStopped, pmPaused, pmStartup };
  ePlayMode playMode;
  volatile ePlayMode newPlayMode;
  volatile bool doSeek;
  volatile int seekToSeconds;
  bool active;
  bool started;
  volatile bool getPlayTimes;
  uint32_t playedTime;
  uint32_t totalTime;

  cDecoder *decoder;
  cPoller poll;
  bool isStream;

  // This Functions are only allow to be used in Action().
  inline void ActionInit(void);
  inline void UpdatePlayTime(uint64_t lastPlayedOffset);
  inline bool PlayCurData(const uint8_t *&data, int &length);  
  inline void DisplayImage(cSong *song);
  inline bool StartPlay(void);
  inline bool GetNextFrame(const uint8_t *&data, int &length, uint64_t &offset, uint32_t &frame_param);
  inline void StopPlay(const uint8_t *&data, int &length, uint64_t &offset);
  inline void DoSeek(const uint8_t *&data, int &length);

  inline void WaitPlayMode(ePlayMode mode, bool inv);
protected:
  virtual void Activate(bool On);
  virtual void Action(void);
public:
  cMP3Player(void);
  virtual ~cMP3Player();
  void Pause(void);
  void Play(void);
  bool PrevCheck(void);
  void SkipSeconds(int secs);
  virtual bool GetIndex(int &Current, int &Total, bool SnapToIFrame=false);
  virtual bool GetReplayMode(bool &Play, bool &Forward, int &Speed);
  bool Active(void) { return active; }
  bool IsStream(void) { return isStream; }
  };

#endif //___DVB_MP3_H
