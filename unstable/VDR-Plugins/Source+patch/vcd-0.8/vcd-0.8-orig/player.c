/*
 * player.c: Player core
 *
 * VCD Player plugin for VDR (the Video Disk Recorder)
 * Copyright (C) 2002  Thomas Heiligenmann  <thomas@heiligenmann.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 f the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA
 *
 */


#include "player.h"
#include "setup.h"

#define VIDEOBUFSIZE MEGABYTE(1)

#define MAX_VIDEO_SLOWMOTION 63 // max. arg to pass to VIDEO_SLOWMOTION // TODO is this value correct?
#define NORMAL_SPEED  4 // the index of the '1' entry in the following array
#define MAX_SPEEDS    3 // the offset of the maximum speed from normal speed in either direction
#define SPEED_MULT   12 // the speed multiplier


// --- cVcdPlayer ------------------------------------------------------------

class cVcdPlayer : public cPlayer, cThread {
private:
  enum ePlayModes { pmPlay, pmPause, pmSlow, pmFast, pmStill };
  enum ePlayDirs { pdForward, pdBackward };

  cVcd *vcd;
  int readTrack;
  int readLba, writeLba;
  bool continuous;
  bool doskip;
  uchar audioTrack;
  bool canToggleAudioTrack;

  static int Speeds[];

  bool active, running, done, stop;
  ePlayModes playMode;
  ePlayDirs playDir;
  int trickSpeed;
  int loop, autoWait;
  int autoWaitInit;

  int startTrack;
  int trackOffset, trackLast;
  int lba0, lba1, lba;
  int seqhdr_size;
  uchar seqhdr_buf[CD_XA22_DATASIZE];
  int prev_i_lba, next_i_lba;
  __u8 picture_type, last_picture_type;

  cRingBufferFrame *ringBuffer;
  cFrame *readFrame;
  cFrame *playFrame;

  void TrickSpeed(int Increment);
  int Resume(void);
  bool Save(void);
  void Empty(void);
  void StripAudioPackets(uchar *b, int Length, uchar Except = 0x00);
  void TrackInit(int Track);
  int ReadSector(uchar *buf);

protected:
  virtual void Activate(bool On);
  virtual void Action(void);
  cFrame *getNextFrame(void);

public:
  cVcdPlayer(int Track, cVcd *Vcd, int Lba = 0, int Loop = -1, int AutoWait = -1);
  virtual ~cVcdPlayer();
  bool Active(void) { return active; }

  void Stop(void);
  void Pause(void);
  void Play(void);
  void Forward(void);
  void Backward(void);
  void SkipSeconds(int Seconds);
  bool SkipTracks(int Tracks);
  bool SkipEntries(int Entries);
  int GetTrack(void);
  int GetTrack(int LBA);
  void Goto(int Position, bool Still = false);
  void GotoTrack(int Track);
  void GotoEntry(int Entry);
  virtual bool GetIndex(int &Current, int &Total, bool SnapToIFrame = false);
  virtual bool GetReplayMode(bool &Play, bool &Forward, int &Sped);
  virtual void SetAudioTrack(eTrackType Type, const tTrackId *TrackId);
  const char *DeviceName(void) { return vcd->DeviceName(); }
  };

int cVcdPlayer::Speeds[] = { 0, -2, -4, -8, 1, 2, 4, 12, 0 };

cVcdPlayer::cVcdPlayer(int Track, cVcd *Vcd, int Lba, int Loop, int AutoWait)
{
  readTrack = Track;
  vcd = Vcd;
  loop = Loop;
  autoWaitInit = AutoWait;

  continuous = VcdSetupData.PlayTracksContinuously && !VcdSetupData.PlaySequenceReplay ? true : false;

  active = true;
  running = false;
  done = false;
  stop = false;
  autoWait = -1;

  trickSpeed = NORMAL_SPEED;
  playMode = pmPlay;
  playDir = pdForward;
  readLba = writeLba = -1;
  readFrame = NULL;
  playFrame = NULL;
  ringBuffer = new cRingBufferFrame(VIDEOBUFSIZE);

  startTrack = VcdSetupData.BrokenMode && !VcdSetupData.PlaySequenceReplay ? 0 : 1;
  trackOffset = vcd->vcdEntry[startTrack].cdte_addr.lba;
  trackLast = vcd->vcdEntry[vcd->tracks+1].cdte_addr.lba - trackOffset;

  TrackInit(readTrack);
  if (Lba > 0)
     lba = Lba;
}

cVcdPlayer::~cVcdPlayer()
{
  readTrack = 0;
  Detach();
  Save();
}

void cVcdPlayer::TrickSpeed(int Increment)
{
  int nts = trickSpeed + Increment;
  if (Speeds[nts] == 1) {
     trickSpeed = nts;
     if (playMode == pmFast)
        Play();
     else
        Pause();
     }
  else if (Speeds[nts]) {
     trickSpeed = nts;
     int Mult = (playMode == pmSlow && playDir == pdForward) ? 1 : SPEED_MULT;
     int sp = (Speeds[nts] > 0) ? Mult / Speeds[nts] : -Speeds[nts] * Mult;
     if (sp > MAX_VIDEO_SLOWMOTION)
        sp = MAX_VIDEO_SLOWMOTION;
     DeviceTrickSpeed(sp);
     }
}

int cVcdPlayer::Resume(void)
{
//TODO: if meaningful and needed
  return -1;
}

bool cVcdPlayer::Save(void)
{
//TODO: if meaningful and needed
  return false;
}

void cVcdPlayer::Activate(bool On)
{
  if (On) {
     if (readTrack > 0)
        Start();
     }
  else if (active) {
     running = false;
     Cancel(3);
     active = false;
     }
}

void cVcdPlayer::Empty(void)
{
  Lock();
  readFrame = NULL;
  playFrame = NULL;
  ringBuffer->Clear();
  DeviceClear();
  Unlock();
}

void cVcdPlayer::Stop(void)
{
  done = true;
  stop = true;
}

void cVcdPlayer::Pause(void)
{
  if (playMode == pmPause || playMode == pmStill)
     Play();
  else {
     LOCK_THREAD;
     if (playMode == pmFast || (playMode == pmSlow && playDir == pdBackward)) {
        done = false;
        Empty();
        }
     DeviceFreeze();
     playMode = pmPause;
     }
}

void cVcdPlayer::Play(void)
{
  if (playMode != pmPlay) {
     LOCK_THREAD;
     if (playMode == pmPause || playMode == pmStill || playMode == pmFast || (playMode == pmSlow && playDir == pdBackward)) {
        done = false;
        Empty();
        }
     DevicePlay();
     playMode = pmPlay;
     playDir = pdForward;
     }
}

void cVcdPlayer::Forward(void)
{
  switch (playMode) {
     case pmFast:
        if (Setup.MultiSpeedMode) {
           TrickSpeed(playDir == pdForward ? 1 : -1);
           break;
           }
        else if (playDir == pdForward) {
           Play();
           break;
           }
     // run into pmPlay
     case pmPlay: {
        LOCK_THREAD;
        done = false;
        Empty();
        DeviceMute();
        playMode = pmFast;
        playDir = pdForward;
        trickSpeed = NORMAL_SPEED;
        TrickSpeed(Setup.MultiSpeedMode ? 1 : MAX_SPEEDS);
        break;
        }
     case pmSlow:
        if (Setup.MultiSpeedMode) {
           TrickSpeed(playDir == pdForward ? -1 : 1);
           break;
           }
        else if (playDir == pdForward) {
           Pause();
           break;
           }
     // run into pmPause
     case pmStill:
     case pmPause:
        DeviceMute();
        playMode = pmSlow;
        playDir = pdForward;
        trickSpeed = NORMAL_SPEED;
        TrickSpeed(Setup.MultiSpeedMode ? -1 : -MAX_SPEEDS);
        break;
     default:
        break;
     }
}

void cVcdPlayer::Backward(void)
{
  if (prev_i_lba)
     switch (playMode) {
        case pmFast:
           if (Setup.MultiSpeedMode) {
              TrickSpeed(playDir == pdBackward ? 1 : -1);
              break;
              }
           else if (playDir == pdBackward) {
              Play();
              break;
           }
        // run into pmPlay
        case pmPlay: {
           LOCK_THREAD;
           done = false;
           Empty();
           DeviceMute();
           playMode = pmFast;
           playDir = pdBackward;
           trickSpeed = NORMAL_SPEED;
           TrickSpeed(Setup.MultiSpeedMode ? 1 : MAX_SPEEDS);
           break;
           }
        case pmSlow:
           if (Setup.MultiSpeedMode) {
              TrickSpeed(playDir == pdBackward ? -1 : 1);
              break;
              }
           else if (playDir == pdBackward) {
              Pause();
              break;
              }
        // run into pmPause
        case pmStill:
        case pmPause: {
           LOCK_THREAD;
           done = false;
           Empty();
           DeviceMute();
           playMode = pmSlow;
           playDir = pdBackward;
           trickSpeed = NORMAL_SPEED;
           TrickSpeed(Setup.MultiSpeedMode ? -1 : -MAX_SPEEDS);
           }
            break;
        }
}

void cVcdPlayer::SkipSeconds(int Seconds)
{
  if ((lba+CD_FRAMES*Seconds)<lba1) {
     LOCK_THREAD;
     done = false;
     Empty();
     lba += CD_FRAMES*Seconds;
     if (lba<lba0) lba = lba0;
     if (GetTrack(readLba)!=readTrack)
        TrackInit(readTrack = GetTrack(readLba));
     else {
        doskip = true;
        readLba = lba;
        }
     playMode = pmPause;
     Play();
     }
}

void cVcdPlayer::Goto(int Position, bool Still)
{
  if ((lba0+CD_FRAMES*Position)<lba1) {
     LOCK_THREAD;
     done = false;
     Empty();
     lba = lba0 + CD_FRAMES*Position;
     doskip = true;
     readLba = lba;
     if (Still)
        playMode = pmStill;
     }
}

void cVcdPlayer::GotoTrack(int Track)
{
  LOCK_THREAD; 
  done = false;
  Empty();
  TrackInit(Track);
  DevicePlay();
  playMode = pmPlay;
  playDir = pdForward;
}

void cVcdPlayer::GotoEntry(int Entry)
{
  if (Entry >= 0 && Entry < U16_TO_LE(vcd->vcdEntries.entries)) {
     LOCK_THREAD;
     done = false;
     Empty();
     TrackInit(bcd_to_bin(vcd->vcdEntries.entry[Entry].track_no) - startTrack);
     lba = bcdmsf_to_lba(vcd->vcdEntries.entry[Entry].entry_msf);
     DevicePlay();
     playMode = pmPlay;
     playDir = pdForward;
     }
}

bool cVcdPlayer::SkipTracks(int Tracks)
{
  int track = GetTrack();
  if (playMode == pmPlay) {
     LOCK_THREAD;
     if (Tracks == 0)
        readTrack = track;
     else if (track > startTrack && Tracks < 0)
        readTrack = track - 1;
     else if (track < vcd->tracks && Tracks > 0)
        readTrack = track + 1;
     else
        return false;
     done = false;
     Empty();
     TrackInit(readTrack);
     DevicePlay();
     playMode = pmPlay;
     playDir = pdForward;
     return true;
     }
  return false;
}

bool cVcdPlayer::SkipEntries(int Entries)
{
  if (playMode == pmPlay) {
     LOCK_THREAD;
     if (Entries > 0) {
        for (int i=0; i < U16_TO_LE(vcd->vcdEntries.entries); i++) {
           int pos = bcdmsf_to_lba(vcd->vcdEntries.entry[i].entry_msf);
           if (pos > lba) {
              int track = bcd_to_bin(vcd->vcdEntries.entry[i].track_no) - startTrack;
              if (track != GetTrack())
                 TrackInit(track);
              lba = pos;
              readLba = lba;
              dsyslog("VCD: Track %d Entry %d",
                 bcd_to_bin(vcd->vcdEntries.entry[i].track_no)-startTrack, i+1);
              done = false;
              Empty();
              DevicePlay();
              playMode = pmPlay;
              playDir = pdForward;
              return true;
              }
           }
        }
     if (Entries < 0) {
        for (int i=U16_TO_LE(vcd->vcdEntries.entries); i>1; i--) {
           int pos = bcdmsf_to_lba(vcd->vcdEntries.entry[i-1].entry_msf);
           if (pos < lba) {
              int track = bcd_to_bin(vcd->vcdEntries.entry[i-1].track_no) - startTrack;
              if (track != GetTrack())
                 TrackInit(track);
              lba = bcdmsf_to_lba(vcd->vcdEntries.entry[i-2].entry_msf);
              readLba = lba;
              dsyslog("VCD: Track %d Entry %d",
                 bcd_to_bin(vcd->vcdEntries.entry[i-2].track_no)-startTrack, i-1);
              done = false;
              Empty();
              DevicePlay();
              playMode = pmPlay;
              playDir = pdForward;
              return true;
              }
           }
        }
     }
  return false;
}

int cVcdPlayer::GetTrack(void)
{
  return GetTrack(writeLba);
}
 
int cVcdPlayer::GetTrack(int Lba)
{
  int i;
  for (i=startTrack; i<=vcd->tracks; i++)
     if ((Lba >= vcd->vcdEntry[i].cdte_addr.lba) &&
         (Lba < vcd->vcdEntry[i+1].cdte_addr.lba))
        return i;
  return 0;
}

bool cVcdPlayer::GetIndex(int &Current, int &Total, bool SnapToIFrame)
{
  Current = max((writeLba - trackOffset - CD_MSF_OFFSET) / 3, 0);
  Total = trackLast/3;
  return true;
}

bool cVcdPlayer::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  Play = (playMode == pmPlay || playMode == pmFast);
  Forward = (playDir == pdForward);
  if (playMode == pmFast || playMode == pmSlow)
     Speed = Setup.MultiSpeedMode ? abs(trickSpeed - NORMAL_SPEED) : 0;
  else
     Speed = -1;
  return true;
}

void cVcdPlayer::Action(void)
{
  dsyslog("VCD: input thread started (pid=%d)", getpid());
  int fuse = time(NULL);

  const uchar *p = NULL;
  int pc = 0;

  running = true;
  while (running) {

     if (autoWait>=0 && fuse!=time(NULL)) {
        fuse = time(NULL);
        autoWait--;
        }
     
     cPoller Poller;
     if (DevicePoll(Poller, 100)) {

        LOCK_THREAD;
 
        if (!readFrame)
           readFrame = getNextFrame();
 
        if (readFrame)
           if (ringBuffer->Put(readFrame))
              readFrame = NULL;
 
        if (!playFrame) {
           playFrame = ringBuffer->Get();
           if (playFrame) {
              p = playFrame->Data();
              pc = playFrame->Count();
              }
           }

        if (playFrame && autoWait<0) {
           StripAudioPackets((uchar *)playFrame->Data(), playFrame->Count(), (playMode == pmFast || playMode == pmSlow) ? 0x00 : audioTrack);
           int w = PlayPes(p, pc);
           if (w > 0) {
              running = true;
              p += w;
              pc -= w;
              if (pc <= 0) {
                 writeLba = playFrame->Index();
                 ringBuffer->Drop(playFrame);
                 playFrame = NULL;
                 }
              }
           else if (w < 0 && FATALERRNO) {
              LOG_ERROR;
              break;
              }
           }
        if (done) {
           if (!stop) sleep(1);
           running = false;
           }

        }
      else
        sleep(1);

     }
  active = running = false;

  dsyslog("VCD: input thread ended (pid=%d)", getpid());
}

cFrame *cVcdPlayer::getNextFrame(void)
{
  if (done)
     return 0;

  uchar buffer[10 * CD_XA22_DATASIZE];

  if (seqhdr_size==0 && !VcdSetupData.BrokenMode) {
     seqhdr_buf[0] = 0x00;
     seqhdr_buf[1] = 0x00;
     seqhdr_buf[2] = 0x01;
     seqhdr_buf[3] = 0xB3;
     if (strncmp(vcd->vcdInfo.system_id,"VIDEO_CD",8)==0) {
        if (vcd->vcdInfo.video_type_map[(readTrack-1)/8] & (1 << (readTrack-1)%8)) {
           seqhdr_buf[4] = 0x16;   // PAL VCD 352x288 4:3 25fps
           seqhdr_buf[5] = 0x01;
           seqhdr_buf[6] = 0x20;
           seqhdr_buf[7] = 0x23;
           }
        else {
           seqhdr_buf[4] = 0x16;   // NTSC VCD 352x240 4:3 30fps
           seqhdr_buf[5] = 0x00;
           seqhdr_buf[6] = 0xF0;
           seqhdr_buf[7] = 0x24;
           }
        }
     else {
        if (vcd->vcdInfo.video_type_map[(readTrack-1)/8] & (1 << (readTrack-1)%8)) {
           seqhdr_buf[4] = 0x1E;   // PAL SVCD 480x576 4:3 25fps
           seqhdr_buf[5] = 0x02;
           seqhdr_buf[6] = 0x40;
           seqhdr_buf[7] = 0x23;
           }
        else {
           seqhdr_buf[4] = 0x1E;   // NTSC SVCD 480x480 4:3 30fps
           seqhdr_buf[5] = 0x01;
           seqhdr_buf[6] = 0xE0;
           seqhdr_buf[7] = 0x24;
           }
        }
     seqhdr_buf[8] = 0x00;
     seqhdr_buf[9] = 0x00;
     seqhdr_buf[10] = 0x20;
     seqhdr_buf[11] = 0x00;
     seqhdr_size = 12;
     }

  int count = ReadSector(buffer);

  if (lba >= lba1) {
     if (loop>=0)
        lba = lba0;
     else
        done = true;
     }
 
  if (count > 0 &&
      buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0x01 &&
      buffer[3] == PACK_SC) {
     int ofs;
     if (buffer[4] & 0xC0)     // MPEG-2 pack header
        ofs = 14 + (buffer[13] & 0x07);
     else                      // MPEG-1 pack header
        ofs = 12;
     memcpy(buffer, buffer+ofs, count-ofs);

     __u8 strt_code = buffer[3];

     if ((strt_code & SC_MASK) == VIDEO_STREAM_SC || strt_code == SYSTEM_HEADER_SC) {
        __u8 last_strt_code = strt_code;
        int insert_seqhdr = 0;
        for (int i=0; i < count-ofs-6; i++) {
           if (buffer[i] == 0x00 && buffer[i+1] == 0x00 && buffer[i+2] == 0x01) {
              int h,m,s,f;
              switch (buffer[i+3]) {
                 case SEQUENCE_HEADER_SC:
                    doskip = false;
                    last_strt_code = SEQUENCE_HEADER_SC;
                    for (int j=i+4; j < count-ofs-6; j++) {
                       if (buffer[j] == 0x00 && buffer[j+1] == 0x00 && buffer[j+2] == 0x01) {
                          seqhdr_size = j - i;
                          memcpy(seqhdr_buf, buffer+i, seqhdr_size);
                          break;
                          }
                       }
                    break;
                 case GROUP_OF_PICT_SC:
                    doskip = false;
                    h = (buffer[i+4] >> 2) & 0x1F;
                    m = ((buffer[i+4] << 4) & 0x30) | ((buffer[i+5] >> 4) & 0xF0);
                    s = ((buffer[i+5] << 3) & 0x38) | ((buffer[i+6] >> 5) & 0x07);
                    f = ((buffer[i+6] << 1) & 0x3E) | ((buffer[i+7] >> 7) & 0x01);
                    if (last_strt_code == strt_code && seqhdr_size > 0)
                       insert_seqhdr = i;
                    break;
                 case PICTURE_SC:
                    last_picture_type = picture_type;
                    picture_type = (buffer[i+5] >> 3) & 0x07;
                    if (picture_type == I_FRAME) {
                       for (int j=i+4; j < count-ofs-6; j++) {
                          if (buffer[j]   == 0x00 &&
                              buffer[j+1] == 0x00 &&
                              buffer[j+2] == 0x01 &&
                              buffer[j+3] == USER_DATA_SC &&
                              buffer[j+4] == 0x10 &&
                              buffer[j+5] == 0x0E)
                          {
                             if (buffer[j+6] != 0xFF)
                                prev_i_lba = vcd->vcdEntry[readTrack].cdte_addr.lba +
                                   bcd_to_bin(buffer[j+8] & 0x7F) +
                                   CD_FRAMES * ( bcd_to_bin(buffer[j+7] & 0x7F) +
                                   CD_SECS * bcd_to_bin(buffer[j+6]) );
                             else
                                prev_i_lba = 0;
                             if (buffer[j+9] != 0xFF)
                                next_i_lba = vcd->vcdEntry[readTrack].cdte_addr.lba +
                                   bcd_to_bin(buffer[j+11] & 0x7F) +
                                   CD_FRAMES * ( bcd_to_bin(buffer[j+10] & 0x7F) +
                                   CD_SECS * bcd_to_bin(buffer[j+9]) );
                             else
                                next_i_lba = 0;
                             break;
                             }
                          }
                       }
                    break;
                 default:
                    break;
                 }
              }
           }
        if (insert_seqhdr) {
           uchar tmp_buf[10*CD_XA22_DATASIZE];
           int new_pes_len = buffer[4]*256 + buffer[5] + seqhdr_size;
           memcpy(tmp_buf, buffer+insert_seqhdr, count-ofs-insert_seqhdr);
           memcpy(buffer+insert_seqhdr, seqhdr_buf, seqhdr_size);
           memcpy(buffer+insert_seqhdr+seqhdr_size, tmp_buf, count-ofs-insert_seqhdr);
           buffer[4] = new_pes_len / 256;
           buffer[5] = new_pes_len % 256;
           count += seqhdr_size;
           }
        }
 
//FIXME: needs to be moved someplace else
     if (playMode == pmSlow && playDir == pdBackward) {
// pending
        }
     else if (playMode == pmFast) {
        if (picture_type != I_FRAME) {
           doskip = true;
           if (playDir == pdForward && next_i_lba)
              lba = next_i_lba;
           else if (playDir == pdBackward) {
              if (prev_i_lba)
                 lba = prev_i_lba;
              else {
                 DevicePlay();
                 playMode = pmPlay;
                 playDir = pdForward;
                 return 0;
                 }
              }
           }
        }
 
     readLba = lba;
 
     if (!doskip) {
        if ((strt_code & SC_MASK) == VIDEO_STREAM_SC || strt_code == SYSTEM_HEADER_SC)
           return new cFrame(buffer, count-ofs, ftVideo, readLba);
        if ((strt_code & SC_MASK) == AUDIO_STREAM_SC && playMode == pmPlay)
           return new cFrame(buffer, count-ofs, ftAudio, readLba);
        }
     }

  return 0;
}

void cVcdPlayer::StripAudioPackets(uchar *b, int Length, uchar Except)
{
  for (int i=0; i < Length-6; i++) {
     if (b[i]==0x00 && b[i+1]==0x00 && b[i+2]== 0x01) {
        uchar strt_code = b[i+3];
        int l = b[i+4] * 256 + b[i+5] + 6;
        switch (strt_code) {
           case 0xC0 ... 0xC1:
              if (strt_code==0xC1 && !canToggleAudioTrack) {
                 canToggleAudioTrack = true;
                 DeviceSetAvailableTrack(ttAudio, strt_code & 0x1f, strt_code);
                 }
              if (!Except || strt_code!=Except)
                 memset(&b[i], 0x00, min(l, Length-i));
              break;
           case 0xE0 ... 0xEF:
              break;
           default:
              l = 0;
              break;
           }
        if (l)
           i += l - 1;
        }
     }
}

void cVcdPlayer::TrackInit(int track)
{
  dsyslog("VCD: Track %d", track+1-startTrack);
  audioTrack = 0xC0;
  canToggleAudioTrack = false;
  doskip = true;
  if (continuous) {
     lba0 = vcd->vcdEntry[startTrack].cdte_addr.lba;
     lba1 = vcd->vcdEntry[vcd->tracks+1].cdte_addr.lba;
     }
  else {
     lba0 = vcd->vcdEntry[track].cdte_addr.lba;
     lba1 = vcd->vcdEntry[track+1].cdte_addr.lba;
     }
  lba = vcd->vcdEntry[track].cdte_addr.lba;
  seqhdr_size = 0;
  prev_i_lba = 0;
  next_i_lba = 0;
  picture_type = NO_PICTURE;
  last_picture_type = NO_PICTURE;
}

int cVcdPlayer::ReadSector(uchar *buf)
{
  struct cdsector_xa22 sector;

  if (vcd->readSectorRaw(lba++, &sector)) {
     if (sector.subhead.dw[0]==sector.subhead.dw[1]) {  // a (s)vcd sector ?!
        switch (sector.subhead.b[2] & 0x7e) {
           case 0x62:
           case 0x64:
              memcpy(buf, &(sector.data[0]), CD_XA22_DATASIZE);
              return CD_XA22_DATASIZE;
           default:
              break;
           }
        }
     if ((sector.subhead.b[2] & 0x10) && autoWaitInit>=0) {
        autoWait = autoWaitInit;
        dsyslog("VCD: autowait %d sec", autoWait);
        }
     }

  return 0;
}

void cVcdPlayer::SetAudioTrack(eTrackType Type, const tTrackId *TrackId)
{
  if (IS_AUDIO_TRACK(Type)) {
     if ((audioTrack == 0xC0) != (Type <= ttAudioFirst)) {
        audioTrack = (Type > ttAudioFirst) ? 0xC1 : 0xC0;
        Empty();
        }
     }
}

// --- cVcdPlayerControl -----------------------------------------------------

cVcdPlayerControl::cVcdPlayerControl(int Track, cVcd *Vcd)
: cControl(player = new cVcdPlayer(Track, Vcd))
{
}

cVcdPlayerControl::cVcdPlayerControl(int Track, cVcd *Vcd, int Lba, int Loop, int AutoWait)
: cControl(player = new cVcdPlayer(Track, Vcd, Lba, Loop, AutoWait))
{
}

cVcdPlayerControl::~cVcdPlayerControl()
{
  Stop();
}

bool cVcdPlayerControl::Active(void)
{
  return player && player->Active();
}

int cVcdPlayerControl::GetTrack(void)
{
  if (player)
     return player->GetTrack();
  return 0;
}

void cVcdPlayerControl::Stop(void)
{
  if (player) {
     player->Stop();
     delete player;
     player = NULL;
     }
}

void cVcdPlayerControl::Pause(void)
{
  if (player)
     player->Pause();
}

void cVcdPlayerControl::Play(void)
{
  if (player)
     player->Play();
}

void cVcdPlayerControl::Forward(void)
{
  if (player)
     player->Forward();
}

void cVcdPlayerControl::Backward(void)
{
  if (player)
     player->Backward();
}

void cVcdPlayerControl::SkipSeconds(int Seconds)
{
  if (player)
     player->SkipSeconds(Seconds);
}

bool cVcdPlayerControl::SkipTracks(int Tracks)
{
  if (player)
     return player->SkipTracks(Tracks);
  return false;
}

bool cVcdPlayerControl::SkipEntries(int Entries)
{
  if (player)
     return player->SkipEntries(Entries);
  return false;
}

void cVcdPlayerControl::Goto(int Position, bool Still)
{
  if (player)
     player->Goto(Position, Still);
}

void cVcdPlayerControl::GotoTrack(int Track)
{
  if (player)
     player->GotoTrack(Track);
}

void cVcdPlayerControl::GotoEntry(int Entry)
{
  if (player)
     player->GotoEntry(Entry);
}

const char *cVcdPlayerControl::DeviceName(void)
{
  if (player)
     return player->DeviceName();
  return NULL;
}
