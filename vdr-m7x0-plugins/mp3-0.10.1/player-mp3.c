/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2007 Stefan Huelswitt <s.huelswitt@gmx.de>
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <math.h>

#include <vdr/player.h>
#include <vdr/thread.h>
#include <vdr/tools.h>

#include "common.h"
#include "setup-mp3.h"
#include "player-mp3.h"
#include "data-mp3.h"
#include "decoder.h"
#include "decoder-core.h"


#ifndef NO_DEBUG
#define DEBUG_MODE      // debug playmode changes
#define DEBUG_BGR       // debug backround scan thread
#define DEBUG_DELAY 300 // debug write/decode delays
//#define FRAME_DUMP
//#define ACC_DUMP        // dump limiter lookup table to /tmp/limiter
//#define DEBUG_BUFFER  //Debug Hardbeat/buffer
#endif

#if !defined(NO_DEBUG) && defined(DEBUG_MODE)
#define dm(x) { (x); }
#else
#define dm(x) ;
#endif

#if !defined(NO_DEBUG) && defined(DEBUG_BGR)
#define db(x) { (x); }
#else
#define db(x) ;
#endif

// ----------------------------------------------------------------

#define MP3BUFSIZE (1024*1024)                               // output ringbuffer size
#define OUT_BITS 16                                          // output 16 bit samples to DVB driver
#define OUT_FACT (OUT_BITS/8*2)                              // output factor is 16 bit & 2 channels -> 4 bytes

// --- cShuffle ----------------------------------------------------------------

class cShuffle {
private:
  int *shuffle, max;
  unsigned int seed;
  //
  int Index(int pos);
public:
  cShuffle(void);
  ~cShuffle();
  void Shuffle(int num, int curr);
  void Del(int pos);
  void Flush(void);
  int First(void);
  int Next(int curr);
  int Prev(int curr);
  int Goto(int pos, int curr);
  };

cShuffle::cShuffle(void)
{
  shuffle=0; max=0;
  seed=time(0);
}

cShuffle::~cShuffle(void)
{
  Flush();
}

void cShuffle::Flush(void)
{
  delete shuffle; shuffle=0;
  max=0;
}

int cShuffle::Index(int pos)
{
  if(pos>=0)
    for(int i=0; i<max; i++) if(shuffle[i]==pos) return i;
  return -1;
}

void cShuffle::Shuffle(int num, int curr)
{
  int oldmax=0;
  if(num!=max) {
    int *ns=new int[num];
    if(shuffle) {
      if(num>max) {
        memcpy(ns,shuffle,max*sizeof(int));
        oldmax=max;
        }
      delete shuffle;
      }
    shuffle=ns; max=num;
    }
  if(!oldmax) curr=-1;
  for(int i=oldmax ; i<max ; i++) shuffle[i]=i;

  int in=Index(curr)+1; if(in<0) in=0;
  if((max-in)>=2) {
    for(int i=in ; i<max ; i++) {
      int ran=(rand_r(&seed) % ((max-in)*4-4))/4; ran+=((ran+in) >= i);
      int t=shuffle[i];
      shuffle[i]=shuffle[ran+in];
      shuffle[ran+in]=t;
      }
    }
#ifdef DEBUG
  isyslog("shuffle: order (%d , %d -> %d) ",num,curr,in);
  for(int i=0 ; i<max ; i++) isyslog("%d ",shuffle[i]);
#endif
}

void cShuffle::Del(int pos)
{
  int i=Index(pos);
  if(i>=0) {
    if(i+1<max) memmove(&shuffle[i],&shuffle[i+1],(max-i-1)*sizeof(int));
    max--;
    }
}

int cShuffle::First(void)
{
  return shuffle[0];
}

int cShuffle::Next(int curr)
{
  int i=Index(curr);
  return (i>=0 && i+1<max) ? shuffle[i+1] : -1;
}

int cShuffle::Prev(int curr)
{
  int i=Index(curr);
  return (i>0) ? shuffle[i-1] : -1;
}

int cShuffle::Goto(int pos, int curr)
{
  int i=Index(curr);
  int g=Index(pos);
  if(g>=0) {
    if(g<i) {
      for(int l=g; l<i; l++) shuffle[l]=shuffle[l+1];
      shuffle[i]=pos;
      }
    else if(g>i) {
      for(int l=g; l>i+1; l--) shuffle[l]=shuffle[l-1];
      shuffle[i+1]=pos;
      }
#ifdef DEBUG
    isyslog("shuffle: goto order (%d -> %d , %d -> %d) ",pos,g,curr,i);
    for(int i=0 ; i<max ; i++) isyslog("%d ",shuffle[i]);
    isyslog("\n");
#endif
    return pos;
    }
  return -1;
}

// --- cPlayManager ------------------------------------------------------------

#define SCANNED_ID3 1

cPlayManager *mgr=0;

cPlayManager::cPlayManager(void)
{
  curr=0; currIndex=-1;
  scan=0; stopscan=throttle=release=false;
  play=0; playNew=eol=false;
  shuffle=new cShuffle;
  loopMode=(MP3Setup.InitLoopMode>0);
  shuffleMode=(MP3Setup.InitShuffleMode>0);
}

cPlayManager::~cPlayManager()
{
  Flush();
  Release();
  listMutex.Lock();
  stopscan=true; bgCond.Broadcast();
  listMutex.Unlock();
  Cancel(2);
  delete shuffle;
}

void cPlayManager::ThrottleWait(void)
{
  while(!stopscan && !release && throttle) {
    db(isyslog("mgr: background scan throttled\n"))
    bgCond.Wait(listMutex);
    db(isyslog("mgr: background scan throttle wakeup\n"))
    }
}

void cPlayManager::Action(void)
{
  db(isyslog("mgr: background scan thread started (pid=%d)\n", getpid()))
  nice(5);
  listMutex.Lock();
  while(!stopscan) {
    for(scan=list.First(); !stopscan && !release && scan; scan=list.Next(scan)) {
      ThrottleWait();
      listMutex.Unlock();
      if(!(scan->user & SCANNED_ID3)) {
        db(isyslog("mgr: scanning (id3) %s\n",scan->Name()))
        scan->Info(true);
        scan->user|=SCANNED_ID3;
        }
      listMutex.Lock();
      }
    do {
      scan=0; release=false; fgCond.Broadcast();
      db(isyslog("mgr: background scan idle\n"))
      bgCond.Wait(listMutex);
      db(isyslog("mgr: background scan idle wakeup\n"))
      } while(!stopscan && (release || throttle));
    }
  listMutex.Unlock();
  db(isyslog("mgr: background scan thread ended (pid=%d)\n", getpid()))
}

void cPlayManager::Throttle(bool thr)
{
  if(MP3Setup.BgrScan) {
    if(!thr && throttle) {
      db(isyslog("mgr: bgr-scan -> run (%d)\n",time_ms()))
      listMutex.Lock();
      throttle=false; bgCond.Broadcast();
      listMutex.Unlock();
      }
    if(thr && !throttle) {
      db(isyslog("mgr: bgr-scan -> throttle (%d)\n",time_ms()))
      throttle=true;
      }
    }
}

void cPlayManager::ToggleShuffle(void)
{
  shuffleMode=!shuffleMode;
  d(isyslog("mgr: shuffle mode toggled : %d\n",shuffleMode))
  if(shuffleMode && !eol) {
    curr=0; currIndex=-1;
    shuffle->Shuffle(maxIndex+1,-1);
    Next();
    }
}

void cPlayManager::ToggleLoop(void)
{
  loopMode=!loopMode;
  d(isyslog("mgr: loop mode toggled : %d\n",loopMode))
}

bool cPlayManager::Info(int num, cMP3PlayInfo *pi)
{
  cSong *s;
  int idx=num-1;
  if(idx<0) { idx=currIndex; s=curr; }
  else      { s=list.Get(idx); }
  memset(pi,0,sizeof(*pi));
  pi->Num=idx+1;
  pi->MaxNum=maxIndex+1;
  pi->Loop=loopMode;
  pi->Shuffle=shuffleMode;
  bool res=false;
  if(s) {
    strn0cpy(pi->Title,s->Name(),sizeof(pi->Title));
    strn0cpy(pi->Filename,s->FullPath(),sizeof(pi->Filename));
    cSongInfo *si=s->Info(false);
    if(si && si->HasInfo()) {
      static const char *modestr[] = {"Stereo", "Joint-Stereo", "Dual", "Mono"};

      if(si->Title)  strn0cpy(pi->Title,si->Title,sizeof(pi->Title));
      if(si->Artist) strn0cpy(pi->Artist,si->Artist,sizeof(pi->Artist));
      if(si->Album)  strn0cpy(pi->Album,si->Album,sizeof(pi->Album));
      strn0cpy(pi->SMode,modestr[si->ChMode],sizeof(pi->SMode));
      pi->Year=si->Year;
      pi->SampleFreq=si->SampleFreq;
      pi->Bitrate=si->Bitrate;
      pi->MaxBitrate=si->MaxBitrate;
      res=true;
      }
    }
  pi->Hash=MakeHashBuff((char *)pi,(char *)&pi->Loop-(char *)pi);
  return res;
}

void cPlayManager::Add(cPlayList *pl)
{
  cMutexLock lock(&listMutex);
  bool real=false;
  for(cSong *song=pl->First(); song; song=pl->cList<cSong>::Next(song)) {
    cSong *ns=new cSong(song);
    list.Add(ns);
    real=true;
    }
  if(real) {
    if(MP3Setup.BgrScan) { stopscan=false; if(!Active()) Start(); }
    else stopscan=true;
    bgCond.Broadcast();
    maxIndex=list.Count()-1;
    if(shuffleMode) shuffle->Shuffle(maxIndex+1,currIndex);
    if(!curr) Next();
    }
}

void cPlayManager::Flush(void)
{
  cMutexLock lock(&listMutex);
  Halt();
  list.Clear();
  shuffle->Flush();
}

void cPlayManager::Halt(void)
{
  cMutexLock lock(&listMutex);
  curr=0; currIndex=-1;
  playNew=true;
  stopscan=true; bgCond.Broadcast();
  NoScan(0);
  NoPlay(0);
}

void cPlayManager::NoScan(cSong *nono)
{
  // call with listMutex locked!!
  while((nono && scan==nono) || (!nono && scan)) {
    release=true; bgCond.Broadcast();
    d(isyslog("mgr: waiting for bgr release ... (nono=%p scan=%p)\n",nono,scan))
    fgCond.Wait(listMutex);
    }
}

void cPlayManager::NoPlay(cSong *nono)
{
  // call with listMutex locked!!
  while((nono && play==nono) || (!nono && play)) {
    playNew=true;
    fgCond.Wait(listMutex);
    }
}

bool cPlayManager::Next(void)
{
  cMutexLock lock(&listMutex);
  int ni;
  cSong *n;
  if(shuffleMode) {
    if(curr) {
      ni=shuffle->Next(currIndex);
      if(ni<0) {
        if(loopMode || eol) {
          shuffle->Shuffle(maxIndex+1,-1);
          ni=shuffle->First();
          }
        else eol=true;
        }
      }
    else
      ni=shuffle->First();
    n=(ni>=0) ? list.Get(ni) : 0;
    }
  else {
    if(curr) {
      n=list.cList<cSong>::Next(curr);
      if(!n) {
        if(loopMode || eol) n=list.First();
        else eol=true;
        }
      }
    else
      n=list.First();
    ni=n ? n->Index() : -1;
    }
  if(n) {
    curr=n; currIndex=ni;
    playNew=true; eol=false;
    d(isyslog("mgr: next -> %d\n",currIndex))
    return true;
    }
  return false;
}

bool cPlayManager::Prev(void)
{
  cMutexLock lock(&listMutex);
  int ni;
  cSong *n;
  if(shuffleMode) {
    ni=shuffle->Prev(currIndex);
    n=(ni>=0) ? list.Get(ni) : 0;
    }
  else {
    n=list.cList<cSong>::Prev(curr);
    ni=n ? n->Index() : -1;
    }
  if(n) {
    curr=n; currIndex=ni;
    playNew=true; eol=false;
    d(isyslog("mgr: prev -> %d\n",currIndex))
    return true;
    }
  return false;
}

void cPlayManager::Goto(int num)
{
  cMutexLock lock(&listMutex);
  if(num>0 && num<=maxIndex+1) {
    int idx=num-1;
    if(shuffleMode) {
      if(eol) {
        shuffle->Shuffle(maxIndex+1,-1);
        currIndex=shuffle->Goto(idx,-1);
        }
      else
        currIndex=shuffle->Goto(idx,currIndex);
      }
    else
      currIndex=idx;
    curr=(currIndex>=0) ? list.Get(currIndex) : 0;
    playNew=true; eol=false;
    d(isyslog("mgr: goto -> %d\n",currIndex))
    }
}

cSong *cPlayManager::Current(void)
{
  cMutexLock lock(&listMutex);
  if(!play) {
    NoScan(curr);
    play=curr;
    playNew=false;
    if(play) d(isyslog("mgr: playing %s\n",play->Name()))
    else d(isyslog("mgr: nothing to play\n"))
    fgCond.Broadcast();
    }
  return play;
}

bool cPlayManager::NextCurrent(void)
{
  cMutexLock lock(&listMutex);
  return (!eol && (playNew || Next()));
}

bool cPlayManager::NewCurrent(void)
{
  return playNew;
}

void cPlayManager::Release(void)
{
  cMutexLock lock(&listMutex);
  play=0;
  fgCond.Broadcast();
}

// --- cOutputDvb --------------------------------------------------------------

//cDevice::PrimaryDevice()->SetCurrentAudioTrack(ttAudio);
// --- cMP3Player --------------------------------------------------------------

cMP3Player::cMP3Player()
:cPlayer(pmAudioOnlyBlack), cThread("mp3-player")
{
  active=true;
  started=false;
  isStream=false;
  playMode = pmStartup;
  playedTime = 0;
  totalTime = 0;
  decoder = NULL;
}

cMP3Player::~cMP3Player()
{
  Detach();
}

void cMP3Player::Activate(bool On)
{
  if(On) {
    Lock();
    if(!started) {
      playMode = pmStartup;
      Start(); 
      started = true;
      WaitPlayMode(pmStartup, true); // wait for the player to become ready
      }
    Unlock();
    }
  else { 
    Lock(); 
    if (started & active) {
       Cancel(10);
       // For safety. (StopPlay() is exceptionally allowed here, as thread is
       // already stopped)
       const uint8_t *data = NULL;
       int length = 0;
       uint64_t offset = 0;
       StopPlay(data, length, offset);
       started = false;
       active = false;
       }
    Unlock();
    }
}

inline void cMP3Player::WaitPlayMode(ePlayMode mode, bool inv)
{
  while (active & ((!inv & (mode != playMode)) | (inv & (mode == playMode)))) 
        cCondWait::SleepMs(25);
}

inline void cMP3Player::ActionInit(void)
{
  decoder = NULL;
  playedTime = 0;
  totalTime = 0;
  doSeek = false;
  getPlayTimes = false;
  newPlayMode = pmPlay;
  playMode = pmPlay;
}

inline void cMP3Player::UpdatePlayTime(uint64_t lastPlayedOffset)
{ 
  if (!getPlayTimes)
     return;
  if (decoder)
     playedTime = decoder->GetTimes(lastPlayedOffset, totalTime);
  getPlayTimes = false;
}

inline bool cMP3Player::PlayCurData(const uint8_t *&data, int &length) 
{
  if (!data)
     return true;
  if (!DevicePoll(poll,100))
     return true;

  int r = PlayPes(data, length);
  if (unlikely(r < 0)) {
     if (FATALERRNO) {
        LOG_ERROR;
        decoder->DropFrame();
        data = NULL;
        length = 0;
        Skins.QueueMessage(mtWarning, tr("mp3: output failed\n"));
        return false;
        }
     return true;
     }

  data += r;
  length -= r;
  if (likely(length <= 0)) {
     decoder->DropFrame();
     data = NULL;
     }
  return true;
}

inline void cMP3Player::DisplayImage(cSong *song)
{
  if (MP3Setup.BackgrMode < 1)
     return;
  unsigned char *mem;
  int len;
  bool alloced = false;

  mem = (unsigned char *)decoder->GetImage(len);
  if (!mem)
     song->Image(mem,len, alloced);
  if (mem) {
     DeviceStillPicture(mem,len);
     if (alloced)
        free(mem);
     cCondWait::SleepMs(40);
     }
}

inline bool cMP3Player::StartPlay(void)
{
  cSong *song;
  mgr->Throttle(true);
  playedTime = 0;
  totalTime = 0;
  song = mgr->Current();
  if (!song)
     return false;

  decoder = song->Decoder();
  if (!decoder || !decoder->Activate(true)) {
     esyslog("ERROR: playlist entry %s is not a valid file",song->Name());
     char buffer[128];
     snprintf(buffer, 128, tr("Playlist entry %s not playable"),song->Name());
     buffer[127] = 0;
     Skins.QueueMessage(mtWarning, buffer);
     mgr->Release();
     decoder = NULL;
     return false;
     }

  isStream = decoder->IsStream();
  DisplayImage(song);
  return true;
}

inline bool cMP3Player::GetNextFrame(const uint8_t *&data, int &length,
      uint64_t &offset, uint32_t &frame_param)
{
   const DecoderFrame *frame;
   frame = decoder->GetFrame();

   if (frame->status == dsPlay) {
      data = frame->pes_frame;
      length = frame->pes_frame_length;
      offset = frame->offset;
      if (frame_param != frame->frame_param) {
         DeviceFlush();
         DeviceClear();
         frame_param = frame->frame_param;
         }
      return true;
      }

   if (frame->status == dsOK)
      return true;

   if (frame->status == dsError) {
      cSong *song = mgr->Current();
      esyslog("ERROR: playlist entry %s corrupt",song->Name());
      char buffer[128];
      snprintf(buffer, 128, tr("Playlist entry %s corrupt"),song->Name());
      buffer[127] = 0;
      Skins.QueueMessage(mtWarning, buffer);
      }
   return false;
}

inline void cMP3Player::StopPlay(const uint8_t *&data, int &length,
   uint64_t &offset)
{
  if (!decoder)
     return;
  if (data)
     decoder->DropFrame();
  data = NULL;
  length = 0;
  decoder->Activate(false);
  decoder = NULL;
  mgr->Release();
  playedTime = 0;
  totalTime = 0;
  offset = 0;
}

inline void cMP3Player::DoSeek(const uint8_t *&data, int &length)
{
  if (!doSeek) 
     return;
  doSeek = false;
  const int SeekToSeconds = seekToSeconds;
  decoder->Seek(SeekToSeconds);
  if (data)
     decoder->DropFrame();
  data = NULL;
  length = 0;
  DeviceClear();
}

void cMP3Player::Action(void)
{
  const uint8_t *data = NULL;
  int data_length = 0;
  uint64_t lastPlayedOffset = 0;
  uint32_t lastFrameParam = 0;
  bool start = true;
  ActionInit();

  while (active & Running()) {
        UpdatePlayTime(lastPlayedOffset);

        if (playMode != newPlayMode) {
           playMode = newPlayMode;
           if (playMode == pmStopped) {
              StopPlay(data, data_length, lastPlayedOffset);
              DeviceClear();
              start = true;
              }
           }

        if (playMode != pmPlay) {
           mgr->Throttle(false);
           cCondWait::SleepMs(100);
           continue;
           }

        if (mgr->NewCurrent()) {
           StopPlay(data, data_length, lastPlayedOffset);
           DeviceClear();
           start = true;
           }

        if (!decoder) { 
           if (!start && !mgr->NextCurrent()) {
              DeviceFlush();
              newPlayMode = pmStopped;
              if (MP3Setup.AbortAtEOL)
                 break;
              continue;
              }
           start = false;
           if (!StartPlay())
              continue;
           doSeek = false;
           }

        DoSeek(data, data_length);

        if (!data && !GetNextFrame(data, data_length, lastPlayedOffset, lastFrameParam)) {
           StopPlay(data, data_length, lastPlayedOffset);
           continue;
           }

        if (!PlayCurData(data, data_length))
           break;

        mgr->Throttle(!decoder->BufferFilled());
        }

  StopPlay(data, data_length, lastPlayedOffset);
  DeviceClear();
  playMode = pmStopped;
  active = false;
}

void cMP3Player::Pause(void)
{
  if (playMode == pmPaused) 
     newPlayMode = pmPlay;
  else if ((playMode == pmPlay) & (!isStream))
    newPlayMode = pmPaused;
}

void cMP3Player::Play(void)
{
  newPlayMode = pmPlay;
}

bool cMP3Player::PrevCheck(void)
{
  if (isStream)
     return false;

  getPlayTimes = true;
  for (int i = 0; getPlayTimes & (i < 3); i++)
      cCondWait::SleepMs(25);

  if (playedTime <= 3)
     return false;

  seekToSeconds = 0;
  doSeek = true;
  return true;
}

void cMP3Player::SkipSeconds(int secs)
{
  if ((playMode == pmStopped) | isStream)
     return;

  getPlayTimes = true;
  for (int i = 0; getPlayTimes & (i < 3); i++)
      cCondWait::SleepMs(25);

  secs += playedTime;
  if (secs < 0)
     secs = 0;
  seekToSeconds = secs;
  doSeek = true;

  if (playMode == pmPaused) 
     newPlayMode = pmPlay;
}

bool cMP3Player::GetIndex(int &Current, int &Total, bool SnapToIFrame)
{
  getPlayTimes = true;
  for (int i = 0; getPlayTimes & (i < 3); i++)
      cCondWait::SleepMs(25);

  Current=SecondsToFrames(playedTime); 
  Total=SecondsToFrames(totalTime);
  return true;
}

bool cMP3Player::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  Play = (playMode==pmPlay);
  Forward = true;
  Speed = -1;
  return true;
}
