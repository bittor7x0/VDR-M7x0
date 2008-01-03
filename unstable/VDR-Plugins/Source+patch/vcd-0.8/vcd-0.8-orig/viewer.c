/*
 * viewer.c: Viewer core
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


#include "viewer.h"
#include "setup.h"

#define VIDEOBUFSIZE MEGABYTE(1)


// --- cVcdViewer ------------------------------------------------------------

class cVcdViewer : public cPlayer, cThread {
private:
  enum ePlayModes { pmPlay, pmStill, pmAudioOnly };
  cVcd *vcd;
  int spi;
  bool active, running, done;
  ePlayModes playMode;
  uchar videoStill;
  uchar audioTrack;
  bool canToggleAudioTrack;
  bool videoHiRes;
  int lba;
  cRingBufferFrame *ringBuffer;
  cFrame *readItem;
  cFrame *playItem;

  void Empty(void);
  bool Save(void);
  void ItemInit(int Spi);
  int ReadSector(uchar *buf);

protected:
  virtual void Activate(bool On);
  virtual void Action(void);
  cFrame *GetItem(void);

public:
  cVcdViewer(int Spi, cVcd *Vcd);
  virtual ~cVcdViewer();
  bool Active(void) { return active; }
  bool Still(void) { return playMode==pmStill; }
  bool SkipItems(int Items);
  int GetSpi(void) { return spi; }
  void GotoItem(int Spi);
  void ToggleStillRes(void);
  const char *DeviceName(void) { return vcd->DeviceName(); }
  };

cVcdViewer::cVcdViewer(int Spi, cVcd *Vcd)
{
  spi = Spi;
  vcd = Vcd;

  active = true;
  running = false;
  done = false;

  readItem = NULL;
  playItem = NULL;
  ringBuffer = new cRingBufferFrame(SPI_SIZE * CD_XA22_DATASIZE);

  ItemInit(spi);
}

cVcdViewer::~cVcdViewer()
{
  spi = -1;
  Detach();
  Save();
}

bool cVcdViewer::Save(void)
{
  return false;
}

void cVcdViewer::Activate(bool On)
{
  if (On) {
     if (spi >= 0)
        Start();
     }
  else if (active) {
     running = false;
     Cancel(3);
     active = false;
     }
}

void cVcdViewer::Empty(void)
{
  Lock();
  readItem = NULL;
  playItem = NULL;
  ringBuffer->Clear();
  DeviceClear();
  Unlock();
}

void cVcdViewer::GotoItem(int Spi)
{
  LOCK_THREAD;
  done = false;
  Empty();
  spi = Spi;
  ItemInit(Spi);
  DevicePlay();
}

void cVcdViewer::ToggleStillRes(void)
{
  if (playMode==pmStill) {
     LOCK_THREAD;
     videoStill = videoStill == 0xE1 ? 0xE2 : 0xE1;
     done = false;
     Empty();
     DevicePlay();
     }
}

bool cVcdViewer::SkipItems(int Items)
{
  int s = spi;
  LOCK_THREAD;
  if (Items < 0) {
     while (--s>=0) {
        if ((vcd->vcdInfo.spi_contents[s] & SPI_CONTINUED) == 0) {
           spi = s;
           done = false;
           Empty();
           ItemInit(spi);
           DevicePlay();
           return true;
           }
        }
     }
  else if (Items > 0) {
     while (++s < U16_TO_LE(vcd->vcdInfo.max_segment_number)) {
        if ((vcd->vcdInfo.spi_contents[s] & SPI_CONTINUED) == 0) {
           spi = s;
           done = false;
           Empty();
           ItemInit(spi);
           DevicePlay();
           return true;
           }
        }
     }
  return false;
}

void cVcdViewer::Action(void)
{
  dsyslog("VCD: spi viewer started (pid=%d)", getpid());

  const uchar *p = NULL;
  int pc = 0;

  running = true;
  while (running) {
     cPoller Poller;
     if (DevicePoll(Poller, 100)) {

        LOCK_THREAD;

        if (!readItem)
           readItem = GetItem();

        if (readItem)
           if (playMode==pmStill) {

#ifdef DEBUG_VCD
              FILE *fp;
              if ((fp = fopen("/tmp/item.mpg", "w")) != NULL) {
                 fwrite(readItem->Data(), sizeof(char), readItem->Count(), fp);
                 fclose(fp);
                 }
#endif //DEBUG_VCD

              DeviceStillPicture(readItem->Data(), readItem->Count());
              readItem = NULL;
              }
           else
              if (ringBuffer->Put(readItem))
                 readItem = NULL;

        if (!playItem) {
           playItem = ringBuffer->Get();
           if (playItem) {
              p = playItem->Data();
              pc = playItem->Count();
              }
           }

        if (done && playMode!=pmStill)
           running = false;
//      if (done) {
//         if (VcdSetupData.PlaySequenceReplay)
//            running = false;
//         else if (playMode!=pmStill)
//            running = false;
//         }
        if (playItem) {
           int w = PlayPes(p, pc);
           if (w>0) {
              running = true;
              p += w;
              pc -= w;
              if (pc<=0) {
                 ringBuffer->Drop(playItem);
                 playItem = NULL;
                 }
              }
           else if (w<0 && FATALERRNO) {
              LOG_ERROR;
              break;
              }
           }

        }
     else
        sleep(1);
  }
  active = running = false;

  dsyslog("VCD: spi viewer ended (pid=%d)", getpid());
}

cFrame *cVcdViewer::GetItem(void)
{
  if (done)
     return 0;

  uchar buffer[SPI_SIZE * CD_XA22_DATASIZE];
  int count = 0;

  lba = bcdmsf_to_lba(vcd->vcdInfo.first_segment_msf) + spi * SPI_SIZE;
  dsyslog("VCD: Item %d (lba=%d) (%s%s)", spi, lba,
     SPI_AUDIO(vcd->vcdInfo.spi_contents[spi]) == SPI_1CH_AUDIO   ? "Audio"
     : SPI_AUDIO(vcd->vcdInfo.spi_contents[spi]) == SPI_2CH_AUDIO   ? "2Ch.Audio"
     : SPI_AUDIO(vcd->vcdInfo.spi_contents[spi]) == SPI_MUL_AUDIO   ? "Multich.Audio"
     : SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_NTSC_STILL  ? "NTSC Image"
     : SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_NTSC_STILL2 ? "NTSC Image+"
     : SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_NTSC_VIDEO  ? "NTSC Video"
     : SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_PAL_STILL   ? "PAL Image"
     : SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_PAL_STILL2  ? "PAL Image+"
     : SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_PAL_VIDEO   ? "PAL Video"
     : "unknown",
     SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_NTSC_STILL ||
     SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_NTSC_STILL2 ||
     SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_PAL_STILL ||
     SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_PAL_STILL2
     ?  (videoStill == 0xE2 ? " HiRes" : " LoRes")
     :  (vcd->vcdInfo.spi_contents[spi] & SPI_CONTINUED ? " continued" : "")
     );
  for (int i=0; i<SPI_SIZE; i++) {
     uchar tmpbuf[CD_XA22_DATASIZE];
     int tmpcnt = ReadSector(tmpbuf);
     if (tmpcnt > 0 &&
         tmpbuf[0] == 0x00 && tmpbuf[1] == 0x00 && tmpbuf[2] == 0x01 &&
         tmpbuf[3] == PACK_SC) {
        int ofs;
        if (tmpbuf[4] & 0xC0)               // MPEG-2 pack header
           ofs = 14 + (tmpbuf[13] & 0x07);
        else                                // MPEG-1 pack header
           ofs = 12;
        if (tmpbuf[ofs+3]==videoStill) {        // still image packet
#ifdef STRIP_STILLIMAGE_PES_HEADER
           int len = 256 * tmpbuf[ofs+4] + tmpbuf[ofs+5];
           ofs += 6;
           // strip PES header
           while (tmpbuf[ofs] == 0xFF && len > 0) {
              ofs++;
              len--;
              }
           if ((tmpbuf[ofs] & 0xC0) == 0x40 && len >= 2) {
              ofs += 2;
              len -= 2;
              }
           if ((tmpbuf[ofs] & 0xF0) == 0x20 && len >= 5) {
              ofs += 5;
              len -= 5;
              }
           else if ((tmpbuf[ofs] & 0xF0) == 0x30 && len >= 10) {
              ofs += 10;
              len -= 10;
              }
           else if (tmpbuf[ofs] == 0x0F && len > 0) {
              ofs++;
              len--;
              }
           memcpy(buffer+count, tmpbuf+ofs, len);
           count += len;
#else
           memcpy(buffer+count, tmpbuf+ofs, tmpcnt-ofs);
           count += tmpcnt-ofs;
#endif //STRIP_STILLIMAGE_PES_HEADER
           }
        else if ((tmpbuf[ofs+3]==audioTrack) || // audio packet
            (tmpbuf[ofs+3]==0xE0)) {            // video packet
           memcpy(buffer+count, tmpbuf+ofs, tmpcnt-ofs);
           count += tmpcnt-ofs;
           }
        }
     }

  if ((vcd->vcdInfo.spi_contents[spi+1] & SPI_CONTINUED))
     spi++;
  else
     done = true;

  if (count>0)
     return new cFrame(buffer, count);
  else
     return 0;
}

void cVcdViewer::ItemInit(int Spi)
{
  canToggleAudioTrack = false;  // not yet implemented
  audioTrack = 0xC0;
  videoStill = 0xE1;            // lores stills
  if ((SPI_AUDIO(vcd->vcdInfo.spi_contents[spi]) == SPI_1CH_AUDIO) ||
      (SPI_AUDIO(vcd->vcdInfo.spi_contents[spi]) == SPI_2CH_AUDIO) ||
      (SPI_AUDIO(vcd->vcdInfo.spi_contents[spi]) == SPI_MUL_AUDIO)) {
     playMode = pmAudioOnly;
     }
  else if ((SPI_VIDEO(vcd->vcdInfo.spi_contents[Spi]) == SPI_NTSC_VIDEO) ||
           (SPI_VIDEO(vcd->vcdInfo.spi_contents[Spi]) == SPI_PAL_VIDEO)) {
     playMode = pmPlay;
     }
  else if ((SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_PAL_STILL2) ||
           (SPI_VIDEO(vcd->vcdInfo.spi_contents[spi]) == SPI_NTSC_STILL2)) {
     videoStill = 0xE2;
     playMode = pmStill;
     }
  else {
     playMode = pmStill;
     }
}

int cVcdViewer::ReadSector(uchar *buf)
{
  struct cdsector_xa22 sector;
  if (vcd->readSectorRaw(lba++, &sector)) {
     if (sector.subhead.dw[0]==sector.subhead.dw[1]) { // a (s)vcd sector ?!
        switch (sector.subhead.b[2] & 0x7e) {
              case 0x62:
              case 0x64:  memcpy(buf, &(sector.data[0]), CD_XA22_DATASIZE);
                          return CD_XA22_DATASIZE;
              default:    return 0;
           }
        }
     }
  return 0;
}


// --- cVcdViewerControl -----------------------------------------------------

cVcdViewerControl::cVcdViewerControl(int Spi, cVcd *Vcd)
:cControl(viewer = new cVcdViewer(Spi, Vcd))
{
}

cVcdViewerControl::~cVcdViewerControl()
{
  Stop();
}

bool cVcdViewerControl::Active(void)
{
  return viewer && viewer->Active();
}

bool cVcdViewerControl::Still(void)
{
  return viewer && viewer->Still();
}

int cVcdViewerControl::GetSpi(void)
{
  if (viewer)
     return viewer->GetSpi();
  return -1;
}

void cVcdViewerControl::Stop(void)
{
  delete viewer;
  viewer = NULL;
}

bool cVcdViewerControl::SkipItems(int Items)
{
  if (viewer)
     return viewer->SkipItems(Items);
  return false;
}

void cVcdViewerControl::GotoItem(int Spi)
{
  if (viewer)
     viewer->GotoItem(Spi);
}

void cVcdViewerControl::ToggleStillRes(void)
{
  if (viewer)
     viewer->ToggleStillRes();
}

const char *cVcdViewerControl::DeviceName(void)
{
  if (viewer)
     return viewer->DeviceName();
  return NULL;
}
