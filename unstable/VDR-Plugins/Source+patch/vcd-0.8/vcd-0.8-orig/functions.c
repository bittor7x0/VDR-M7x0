/*
 * functions.c: Functions for handling VCDs, SVCDs, ...
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


#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "functions.h"
#include "setup.h"

__u8 bcd_to_bin(__u8 bcd)
{
  return 10 * (bcd >> 4) + (bcd & 0x0F);
}

int bcdmsf_to_lba(struct cdrom_msf0 msf0)
{
  return bcd_to_bin(msf0.frame)
     + CD_FRAMES * ( bcd_to_bin(msf0.second)
        + CD_SECS * bcd_to_bin(msf0.minute) )
     - CD_MSF_OFFSET;
}

struct cdrom_msf0 lba_to_msf(int lba)
{
  struct cdrom_msf0 msf0;
  msf0.frame = lba % CD_FRAMES;
  lba /= CD_FRAMES;
  msf0.second = lba % CD_SECS;
  msf0.minute = lba / CD_SECS;
  return msf0;
}


// --- cVcd ------------------------------------------------------------------

cVcd *cVcd::getVCD(void)
{
  if (!vcdInstance)
     new cVcd;
  return vcdInstance;
}

cVcd::cVcd(void)
{
  cdrom = 0;
  vcdInstance = this;
  deviceName = strdup("/dev/cdrom");
}

cVcd::cVcd(const char *DeviceName)
{
  cdrom = 0;
  vcdInstance = this;
  deviceName = strdup(DeviceName);
}

cVcd::~cVcd()
{
  Close();
}

int cVcd::Command(int Cmd)
{
  int result = -1;
  int f;
  if ((f = open(deviceName, O_RDONLY | O_NONBLOCK)) > 0) {
     result = ioctl(f, Cmd, 0);
     close(f);
     }
  return result;
}

bool cVcd::DriveExists(void)
{
  return access(deviceName, F_OK) == 0;
}

bool cVcd::DiscOk(void)
{
  return Command(CDROM_DRIVE_STATUS) == CDS_DISC_OK;
}

void cVcd::Eject(void)
{
  if (vcdInstance)
     vcdInstance->Close();
  Command(CDROMEJECT);
}

bool cVcd::Open(void)
{
  if (!cdrom) {
     cdrom = open(deviceName, O_RDONLY | O_NONBLOCK);
     SetDriveSpeed(VcdSetupData.DriveSpeed);
     return true;
     }
  return false;
}

void cVcd::Close(void)
{
  if (cdrom) {
     SetDriveSpeed(0);
     close(cdrom);
     }
  cdrom = 0;
  tracks = 0;
  for (int i=0; i<100; i++)
     memset(&vcdEntry[i], 0, sizeof(vcdEntry));
  memset(&vcdInfo, 0, sizeof(vcdInfo));
  memset(&vcdEntries, 0, sizeof(vcdEntries));
  memset(&vcdLot, 0, sizeof(vcdLot));
  memset(&vcdPsd, 0, sizeof(vcdPsd));
}

void cVcd::SetDriveSpeed(int DriveSpeed)
{
  ioctl(cdrom, CDROM_SELECT_SPEED, DriveSpeed);
}

int cVcd::readTOC(__u8 format)
{
  struct cdrom_tochdr tochdr;
  int i;
  tracks = -1;
  if (ioctl(cdrom, CDROMREADTOCHDR, &tochdr) == -1)
     return -1;
  for (i=tochdr.cdth_trk0; i<=tochdr.cdth_trk1; i++) {
     vcdEntry[i-1].cdte_track = i;
     vcdEntry[i-1].cdte_format = format;
     if (ioctl(cdrom, CDROMREADTOCENTRY, &vcdEntry[i-1]) == -1)
        return -1;
     }
  vcdEntry[tochdr.cdth_trk1].cdte_track = CDROM_LEADOUT;
  vcdEntry[tochdr.cdth_trk1].cdte_format = format;
  if (ioctl(cdrom, CDROMREADTOCENTRY, &vcdEntry[tochdr.cdth_trk1]) == -1)
     return -1;
  tracks = tochdr.cdth_trk1 - 1;
  return tochdr.cdth_trk1 - 1;
}

bool cVcd::readSectorRaw(int lba, void *sect)
{
  struct cdrom_msf0 msf0 = lba_to_msf(lba+CD_MSF_OFFSET);
  memcpy(sect, &msf0, sizeof(struct cdrom_msf0));
  if (ioctl(cdrom, CDROMREADRAW, sect) < 0)
     return false;
  return true;
}

bool cVcd::readSectorXA21(int lba, void *data)
{
  struct cdsector_xa21 sect;
  if (readSectorRaw(lba, &sect) == false)
     return false;
  memcpy(data, sect.data, sizeof(sect.data));
  return true;
}

bool cVcd::isLabel(void)
{
  if (!vcdInstance)
     return false;
  if (strncmp(vcdInfo.system_id,"VIDEO_CD",8)==0)
     return true;
  if (strncmp(vcdInfo.system_id,"SUPERVCD",8)==0)
     return true;
  if (strncmp(vcdInfo.system_id,"HQ-VCD  ",8)==0)
     return true;
  return false;
}
