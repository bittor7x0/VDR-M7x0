/*
 * functions.h: Functions for handling VCDs, SVCDs, ...
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

#ifndef __VCD_FUNC_H
#define __VCD_FUNC_H

#include <arpa/inet.h>
#include <linux/cdrom.h>

#define CD_XA21_DATASIZE  (CD_FRAMESIZE_RAW-CD_SYNC_SIZE-CD_HEAD_SIZE-CD_SUBHEAD_SIZE-CD_EDC_SIZE-CD_ECC_SIZE)
#define CD_XA22_DATASIZE  (CD_FRAMESIZE_RAW-CD_SYNC_SIZE-CD_HEAD_SIZE-CD_SUBHEAD_SIZE-CD_EDC_SIZE)

struct cdsector_xa21 {
  __u8 sync   [CD_SYNC_SIZE];
  __u8 head   [CD_HEAD_SIZE];
  union {
     __u8  b  [CD_SUBHEAD_SIZE];
     __u32 dw [CD_SUBHEAD_SIZE/4];
  } subhead;
  __u8 data   [CD_XA21_DATASIZE];
  __u8 edc    [CD_EDC_SIZE];
  __u8 ecc    [CD_ECC_SIZE];
};

struct cdsector_xa22 {
  __u8 sync   [CD_SYNC_SIZE];
  __u8 head   [CD_HEAD_SIZE];
  union {
     __u8  b  [CD_SUBHEAD_SIZE];
     __u32 dw [CD_SUBHEAD_SIZE/4];
  } subhead;
  __u8 data   [CD_XA22_DATASIZE];
  __u8 edc    [CD_EDC_SIZE];
};

#if __BYTE_ORDER == __BIG_ENDIAN
#define U16_TO_LE(w)   ((__u16)(w))
#define U32_TO_LE(dw)  ((__u32)(dw))
#else
#define U16_TO_LE(w)   ((__u16)( \
                          (((__u16)(w) & 0xFF00U) >> 8) | \
                          (((__u16)(w) & 0x00FFU) << 8)) )
#define U32_TO_LE(dw)  ((__u32)( \
                          (((__u32)(dw) & 0xFF000000U) >> 24) | \
                          (((__u32)(dw) & 0x00FF0000U) >>  8) | \
                          (((__u32)(dw) & 0x0000FF00U) <<  8) | \
                          (((__u32)(dw) & 0x000000FFU) << 24)) )
#endif

enum ePlayItem { piNothing, piTrack, piEntry, piSpi, piReserved };

#define PLAY_ITEM_TYP(w)  ( w<2    ? (piNothing) \
                          : w<100  ? (piTrack) \
                          : w<600  ? (piEntry) \
                          : w<1000 ? (piReserved) \
                          : w<2980 ? (piSpi) \
                          :          (piReserved) )
#define PLAY_ITEM_STR(w)  ( w<2    ? ("Play nothing") \
                          : w<100  ? ("Track") \
                          : w<600  ? ("Entry") \
                          : w<1000 ? ("Reserved") \
                          : w<2980 ? ("SPI") \
                          :          ("Reserved") )
#define PLAY_ITEM_NO(w)   ( w<2    ? (0) \
                          : w<100  ? (w - 1) \
                          : w<600  ? (w - 100) \
                          : w<1000 ? (0) \
                          : w<2980 ? (w - 1000) \
                          :          (0) )
#define PLAY_ITEM_WAIT(b) ( b<61   ? (b) \
                          : b<255  ? ((b-60) * 10 + 60) \
                          :          (-1) )

#define INFO_VCD_LBA            150
#define ENTRIES_VCD_LBA         151
#define LOT_VCD_LBA             152
#define LOT_VCD_SIZE            32
#define PSD_VCD_LBA             184
#define PSD_VCD_MAX_SIZE        256
#define TRACKS_SVD_LBA          185
#define SEARCH_DAT_LBA          186
#define VCD_SEGMENTS            1980
#define VCD_ENTRIES             500
#define SPI_SIZE                150

#define SPI_NODATA              0x00
#define SPI_1CH_AUDIO           0x01
#define SPI_2CH_AUDIO           0x02
#define SPI_MUL_AUDIO           0x03
#define SPI_NTSC_STILL          0x04
#define SPI_NTSC_STILL2         0x08
#define SPI_NTSC_VIDEO          0x0c
#define SPI_RESERVED            0x10
#define SPI_PAL_STILL           0x14
#define SPI_PAL_STILL2          0x18
#define SPI_PAL_VIDEO           0x1c
#define SPI_CONTINUED           0x20
#define SPI_AUDIO(b)            (b & 0x07)
#define SPI_VIDEO(b)            (b & 0x1c)

#define PLAY_LIST_HEADER        0x10
#define SELECTION_LIST_HEADER   0x18
#define END_LIST_HEADER         0x1F

struct info_vcd {
  char               system_id[8];
  __u8               version;
  __u8               system_profile_tag;
  char               album_id[16];
  __u16              album_volumes;
  __u16              album_sequence;
  __u8               video_type_map[13];
  __u8               status_flags;
  __u32              psd_size;
  struct cdrom_msf0  first_segment_msf;
  __u8               offset_multiplier;
  __u16              max_list_id;
  __u16              max_segment_number;
  __u8               spi_contents[VCD_SEGMENTS];
  __u8               reserved[12];
};

struct entries_vcd {
  char   file_id[8];
  __u8   version;
  __u8   reserved_byte;
  __u16  entries;
  struct {
     __u8               track_no;
     struct cdrom_msf0  entry_msf;
  } entry[VCD_ENTRIES];
  __u8   reserved[36];
};

struct lot_vcd {
  __u16  list_id_offset[CD_XA21_DATASIZE*LOT_VCD_SIZE/2];
};

union psd_vcd {
  __u8  header;
  struct {
     __u8   header;
     __u8   number_of_items;
     __u16  list_id;
     __u16  previous_list_offset;
     __u16  next_list_offset;
     __u16  return_list_offset;
     __u16  playing_time;
     __u8   play_item_wait_time;
     __u8   auto_pause_wait_time;
     __u16  play_item[];
  } play;
  struct {
     __u8   header;
     __u8   selection_list_flags;
     __u8   nos;
     __u8   bsn;
     __u16  list_id;
     __u16  previous_list_offset;
     __u16  next_list_offset;
     __u16  return_list_offset;
     __u16  default_list_offset;
     __u16  timeout_list_offset;
     __u8   timeout_wait_time;
     __u8   loop_count;
     __u16  play_item;
     __u16  selection_offset[];
  } selection;
};


__u8 bcd_to_bin(__u8 bcd);
int bcdmsf_to_lba(cdrom_msf0 msf0);
struct cdrom_msf0 lba_to_msf(int lba);

class cVcd {
private:
  cVcd *vcdInstance;
  const char *deviceName;
  int cdrom;
  int Command(int Cmd);
public:
  cVcd(void);
  cVcd(const char *DeviceName);
  ~cVcd();
  bool DriveExists(void);
  bool DiscOk(void);
  void Eject(void);
  bool Open(void);
  void Close(void);
  void SetDriveSpeed(int DriveSpeed);
  int getTracks(void) { return tracks; }
  int readTOC(__u8 format);
  bool readSectorRaw(int lba, void *sect);
  bool readSectorXA21(int lba, void *data);
  bool isLabel(void);
  cVcd *getVCD(void);
  const char *DeviceName(void) { return deviceName; }
  struct cdrom_tocentry vcdEntry[100];
  int tracks;
  struct info_vcd vcdInfo;
  struct entries_vcd vcdEntries;
  struct lot_vcd vcdLot;
  __u8 vcdPsd[CD_XA21_DATASIZE*PSD_VCD_MAX_SIZE];
  };


#endif //__VCD_FUNC_H
