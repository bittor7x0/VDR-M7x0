/***************************************************************************
 *       Copyright (c) 2003,2004 by Marcel Wiesweg                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <dirent.h>

#include "txtrecv.h"
#include "tables.h"
#include "setup.h"
#include "menu.h"

#include <vdr/channels.h>
#include <vdr/device.h>
#include <vdr/config.h>

#include <pthread.h> 
#include <signal.h> 
#include <errno.h>
#include <sys/vfs.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const char *RootDir::root = "/var/cache/vdr/vtx";

void RootDir::setRootDir(const char *newRoot) {
   root=newRoot;
}

const char *RootDir::getRootDir() {
   return root;
}

int Storage::doCleanUp() {
   DIR *top=opendir(root);
   int pagesDeleted=0;
   if (top) {
      struct dirent *chandir, path;
      struct stat chandirstat;
      char fullPath[PATH_MAX];
      while ( (!readdir_r(top, &path, &chandir) && chandir != NULL) ) {
         if (strcmp(chandir->d_name, "..")==0 || strcmp(chandir->d_name, ".")==0)
            continue;
         snprintf(fullPath, PATH_MAX, "%s/%s", root, chandir->d_name);
         if (stat(fullPath, &chandirstat)==0) {
            if (S_ISDIR(chandirstat.st_mode)) {
               pagesDeleted+=cleanSubDir(fullPath);
            }
         }
      }
      closedir(top);
   } else {
      esyslog("OSD-Teletext: Error opening teletext storage directory \"%s\": %s", root, strerror(errno));
   }
   return pagesDeleted;
}

int Storage::cleanSubDir(const char *dir) {
   static bool reportedError=false; //avoid filling up syslog
   DIR *d=opendir(dir);
   bool hadError=false;
   int bytesDeleted=0, filesize;
   if (d) {
      struct dirent *txtfile, path;
      struct stat txtfilestat;
      char fullPath[PATH_MAX];
      while ( (!readdir_r(d, &path, &txtfile) && txtfile != NULL) ) {
         int len=strlen(txtfile->d_name);
         //check that the file end with .vtx to avoid accidents and disasters
         if (strcmp(txtfile->d_name+len-4, ".vtx")==0) {
            snprintf(fullPath, PATH_MAX, "%s/%s", dir, txtfile->d_name);
            stat(fullPath, &txtfilestat);
            filesize=actualFileSize(txtfilestat.st_size);
            int ret=unlink(fullPath);
            if (ret==0)
               bytesDeleted+=filesize;
            else
               hadError=ret;
         }
      }
      closedir(d);
      rmdir(dir);
   } else {
      if (!reportedError) {
         esyslog("OSD-Teletext: Error opening teletext storage subdirectory \"%s\": %s", dir, strerror(errno));
         reportedError=true;
      }
   }

   if (hadError && !reportedError) {
      esyslog("OSD-Teletext: Error removing teletext storage subdirectory \"%s\": %s", dir, strerror(hadError));
      reportedError=true;
   }
   return bytesDeleted;
}

Storage *Storage::s_self = 0;
Storage::StorageSystem Storage::system = Storage::StorageSystemPacked;

Storage::Storage() {
   s_self=this;
   byteCount=0;
   storageOption=-1;
   failedFreeSpace=false;
}

Storage::~Storage() {
}

void Storage::setSystem(StorageSystem s) {
   system=s;
}

Storage *Storage::instance() {
   if (!s_self) {
      switch (system) {
      case StorageSystemLegacy:
         s_self=new LegacyStorage();
         break;
      case StorageSystemPacked:
      default:
         s_self=new PackedStorage();
         break;
      }
   }
   return s_self;
}

void Storage::setMaxStorage(int maxMB) {
   storageOption=maxMB;
}

void Storage::init() {
   cleanUp();
   initMaxStorage(storageOption);
}

void Storage::freeSpace() {
   //there might be a situation where only the current directory is left and
   //occupies the whole space. We cannot delete anything. Don't waste time scanning.
   if (failedFreeSpace)
      return;

   //printf("freeSpace()\n");
   time_t min=time(0);
   char minDir[PATH_MAX];
   char fullPath[PATH_MAX];
   int haveDir=0;
   DIR *top=opendir(getRootDir());
   if (top) {
      struct dirent *chandir, path;
      struct stat chandirstat;
      while ( (!readdir_r(top, &path, &chandir) && chandir != NULL) ) {
         if (strcmp(chandir->d_name, "..")==0 || strcmp(chandir->d_name, ".")==0)
            continue;
         snprintf(fullPath, PATH_MAX, "%s/%s", getRootDir(), chandir->d_name);
         if (stat(fullPath, &chandirstat)==0) {
            if (S_ISDIR(chandirstat.st_mode)) {
               if (chandirstat.st_ctime < min && strcmp(fullPath, currentDir)) {
                  min=chandirstat.st_ctime;
                  strcpy(minDir, fullPath);
                  haveDir++;
               }
            }
         }
      }
      closedir(top);

      //if haveDir, only current directory present, which must not be deleted
      if (haveDir>=2)
         byteCount-=cleanSubDir(minDir);
      else
         failedFreeSpace=true;
   }
}

bool Storage::exists(const char* file) {
   struct stat s;
   return (stat(file, &s)==0);
}

void Storage::getFilename(char *buffer, int bufLength, PageID page) {
   snprintf(buffer, bufLength, "%s/%s/%03x_%02x.vtx", getRootDir(),
            *page.channel.ToString(), page.page, page.subPage);
}

void Storage::prepareDirectory(tChannelID chan) {
   currentDir = cString::sprintf("%s/%s", root, *chan.ToString());
   if (!MakeDirs(currentDir, 1)) {
      esyslog("OSD-Teletext: Error preparing directory for channel \"%s\"",
              *chan.ToString());
      return;
   }
   failedFreeSpace=false;
}

#define TELETEXT_PAGESIZE 972

LegacyStorage::LegacyStorage() {
   maxBytes=0;
   fsBlockSize=1;
   pageBytes=TELETEXT_PAGESIZE;
}

LegacyStorage::~LegacyStorage() {
}

/*
static inline int FilesForMegabytes(double MB, int blocksize) {
   double pageBytes;
   if (TELETEXT_PAGESIZE<=blocksize)
      pageBytes=blocksize;
   else
      pageBytes=((TELETEXT_PAGESIZE/blocksize)+1)*blocksize;
   //reserve 10% for directories
   return (int)( (1024.0 * 1024.0 * (MB-MB*0.1)) / pageBytes );
}*/

int LegacyStorage::actualFileSize(int netFileSize) {
   if (netFileSize<=0)
      return 0;
   if (netFileSize<=fsBlockSize)
      return fsBlockSize;
   else
      return ((netFileSize/fsBlockSize)+1)*fsBlockSize;
}

//max==0 means unlimited, max==-1 means a reasonable default value shall be calculated
void LegacyStorage::initMaxStorage(int maxMB) {
   struct statfs fs;
   if (statfs(getRootDir(), &fs)!=0) {
      esyslog("OSD-Teletext: Error statfs'ing root directory \"%s\": %s, cache size uncontrolled", getRootDir(), strerror(errno));
      return;
   }
   fsBlockSize=fs.f_bsize;

   pageBytes=actualFileSize(TELETEXT_PAGESIZE);

   if (maxMB>=0) {
      if (maxMB<3) {
         esyslog("OSD-Teletext: Request to use at most %d MB for caching. This is not enough, using 3 MB", maxMB);
         maxMB=3;
      }
      maxBytes=MEGABYTE(maxMB);
   } else if (maxMB==-1) {
      //calculate a default value
      double blocksPerMeg = 1024.0 * 1024.0 / fs.f_bsize;
      double capacityMB=fs.f_blocks / blocksPerMeg;
      double freeMB=(fs.f_bavail / blocksPerMeg);
      if (capacityMB<=50 || freeMB<50) {
         //small (<=50MB) filesystems as root dir are assumed to be dedicated for use as a teletext cache
         //for others, the maximum default size is set to 50 MB
         maxBytes=MEGABYTE((int)freeMB);
         //maxPages= FilesForMegabytes(freeMB, fs.f_bsize);
         if (freeMB<3.0) {
            esyslog("OSD-Teletext: Less than %.1f MB free on filesystem of root directory \"%s\"!", freeMB, getRootDir());
            maxBytes=MEGABYTE(3);
         }
      } else {
         //the maximum default size is set to 50 MB
         maxBytes=MEGABYTE(50);
      }
      //printf("Set maxBytes to %ld, %.2f %.2f\n", maxBytes, capacityMB, freeMB);
   }
}

void LegacyStorage::cleanUp() {
   byteCount -= Storage::doCleanUp();
}

void LegacyStorage::registerFile(PageID page) {
   //pageBytes is already effective size
   if ( maxBytes && (byteCount+=pageBytes)>maxBytes )
      freeSpace();
}

StorageHandle LegacyStorage::openForReading(PageID page, bool countAsAccess) {
   //the countAsAccess argument was intended for use in a LRU cache, currently unused
   char filename[PATH_MAX];
   getFilename(filename, sizeof(filename), page);
   StorageHandle ret=(StorageHandle)open(filename, O_RDONLY);
   return ret;
}

StorageHandle LegacyStorage::openForWriting(PageID page) {
   static bool wroteError=false;
   char filename[PATH_MAX];
   getFilename(filename, sizeof(filename), page);
   bool existed=exists(filename);
   //first try
   StorageHandle fd=(StorageHandle)open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   if (fd) {
      if (!existed)
         registerFile(page);
      return fd;
   }
   //no space on disk? make some space available
   if (errno == ENOSPC)
      freeSpace();
   //second try
   fd=(StorageHandle)open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   if (!fd && !wroteError) {
      //report error to syslog - once!
      wroteError=true;
      esyslog("OSD-Teletext: Error opening teletext file %s: %s", filename, strerror(errno));
   }
   //make sure newly created files are counted
   if (fd && !existed)
      registerFile(page);
   return fd;
}

ssize_t LegacyStorage::write(const void *ptr, size_t size, StorageHandle stream) {
   ssize_t written;
   if (!(written=::write((int)stream, ptr, size)) ) {
      switch (errno) {
      case ENOSPC:
         freeSpace();
         return ::write((int)stream, ptr, size);
      case EINTR:
         esyslog("OSD-Teletext: EINTR while writing. Please contact the author and tell him this happened.");
         break;
      default:
         break;
      }
   }
   return written;
}



PackedStorage::PackedStorage() {
}
#define TOC_SIZE 8
//The file structure is simple:
// TOC_SIZE*PageAddress contains the numbers of the following pages
// TOC_SIZE*TELETEXT_PAGESIZE contains the page data
//and the same again.
bool PackedStorage::seekTo(PageID page, int desc, bool create) {
   lseek(desc, 0, SEEK_SET);
   PageAddress addr[TOC_SIZE];

   while (::read(desc, addr, sizeof(addr)) == sizeof(addr)) {
      lseek(desc, 0, SEEK_CUR);
      for (int index=0; index<TOC_SIZE; index++) {
         if (addr[index]==page) {
            lseek(desc, index*TELETEXT_PAGESIZE, SEEK_CUR);
            return true;
         } else if (addr[index].page==0) {
            //0 means: no more pages follow
            if (create) {
               //rewind what was read
               lseek(desc, -(off_t)sizeof(addr), SEEK_CUR);
               //update index
               addr[index]=page;
               if (::write(desc, addr, sizeof(addr)) != sizeof(addr))
                  return false;
               //seek to data position
               lseek(desc, TELETEXT_PAGESIZE*index, SEEK_CUR);
               return true;
            } else
               return false;
         }
      }

      //seek over data area
      lseek(desc, TELETEXT_PAGESIZE*TOC_SIZE, SEEK_CUR);
   }

   int oldSize=actualFileSize(lseek(desc, 0, SEEK_CUR));
   if (create) {
      //create a new set of a TOC and a TOC_SIZE*TELETEXT_PAGESIZE data area
      memset(addr, 0, sizeof(addr));
      //first entry is our page
      addr[0]=page;
      if (::write(desc, addr, sizeof(addr)) != sizeof(addr))
         return false;
      //seek beyond end of file
      lseek(desc, (TELETEXT_PAGESIZE*TOC_SIZE)-1, SEEK_CUR);
      //write one byte to enlarge the file to the sought position
      char c=1;
      if (::write(desc, &c, 1) != 1)
         return false;
      //Now, calculate new file size
      byteCount += ( actualFileSize(lseek(desc, 0, SEEK_CUR)) - oldSize );
      //seek to beginning of data, which is requested
      lseek(desc, -(off_t)(TELETEXT_PAGESIZE*TOC_SIZE), SEEK_CUR);
      return true;
   } else
      return false;
}

void PackedStorage::getFilename(char *buffer, int bufLength, PageID page) {
   //This is a different scheme: page 576_07 will have the name 570s.vtx, the same as e.g. 571_01 or 575_00
   //Think of "the five hundred seventies"
   snprintf(buffer, bufLength, "%s/%s/%03xs.vtx", getRootDir(),
            *page.channel.ToString(), (page.page & 0xFF0));
}

StorageHandle PackedStorage::openForWriting(PageID page) {
   static bool wroteError=false;
   char filename[PATH_MAX];
   getFilename(filename, sizeof(filename), page);
   //first try
   int desc=open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   if (desc != -1) {
      if (!seekTo(page, desc, true)) {
         ::close(desc);
         return StorageHandle();
      }
      if ( maxBytes && byteCount>maxBytes )
         freeSpace();
      return (StorageHandle)desc;
   }
   //no space on disk? make some space available
   if (errno == ENOSPC)
      freeSpace();
   //second try
   desc=open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   if (desc==-1 && !wroteError) {
      //report error to syslog - once!
      wroteError=true;
      esyslog("OSD-Teletext: Error opening teletext file %s: %s", filename, strerror(errno));
   }

   if (desc==-1)
      return StorageHandle();
   else if (!seekTo(page, desc, true)) {
      ::close(desc);
      return StorageHandle();
   }

   if ( maxBytes && byteCount>maxBytes )
      freeSpace();
   return (StorageHandle)desc;
}

StorageHandle PackedStorage::openForReading(PageID page, bool countAsAccess) {
   int desc;
   if ( (desc=(int)LegacyStorage::openForReading(page, false))!= -1 ) {
      if (!seekTo(page, desc, false)) {
         //this is not an error condition here, may and shall happen!
         ::close(desc);
      } else {
         return (StorageHandle)desc;
      }
   }
   return StorageHandle();
}




cTelePage::cTelePage(PageID t_page, uchar t_flags, uchar t_lang,int t_mag)
  : mag(t_mag), flags(t_flags), lang(t_lang), page(t_page)
{
   memset(pagebuf,' ',26*40);
}

cTelePage::~cTelePage() {
}

void cTelePage::SetLine(int line, uchar *myptr)
{
   memcpy(pagebuf+40*line,myptr,40);
}

void cTelePage::save()
{
   Storage *s=Storage::instance();
   unsigned char buf;
   StorageHandle fd;
   if ( (fd=s->openForWriting(page)) ) {
      s->write("VTXV4",5,fd);
      buf=0x01; s->write(&buf,1,fd);
      buf=mag;  s->write(&buf,1,fd);
      buf=page.page; s->write(&buf,1,fd);
      buf=flags; s->write(&buf,1,fd);
      buf=lang; s->write(&buf,1,fd);
      buf=0x00; s->write(&buf,1,fd);
      buf=0x00; s->write(&buf,1,fd);
      s->write(pagebuf,24*40,fd);
      s->close(fd);
   }         
}


cTxtStatus::cTxtStatus(void)
{
   receiver = NULL;
   currentLiveChannel = tChannelID::InvalidID;
}

cTxtStatus::~cTxtStatus()
{
    delete receiver;
}

void cTxtStatus::ChannelSwitch(const cDevice *Device, int ChannelNumber)
{
   // ignore if channel is 0
   if (ChannelNumber == 0) return;

   // ignore if channel is invalid (highly unlikely, this will ever
   // be the case, but defensive coding rules!)
   cChannel* newLiveChannel = Channels.GetByNumber(ChannelNumber);
   if (newLiveChannel == NULL) return;

   // ignore if channel hasn't changed
   if (currentLiveChannel == newLiveChannel->GetChannelID()) return;

   // ignore non-live-channel-switching
   if (!Device->IsPrimaryDevice() ||
      ChannelNumber != cDevice::CurrentChannel()) return;

   // At this point it seems to be pretty sure to me, that the live
   // channel was changed to a new channel and OSDTeletext can
   // now re-attach the receiver to the new live channel

   currentLiveChannel = newLiveChannel->GetChannelID();

   delete receiver;
   receiver = NULL;

   int TPid = newLiveChannel->Tpid();

   if (TPid) {
      receiver = new cTxtReceiver(TPid, currentLiveChannel);
      cDevice::ActualDevice()->AttachReceiver(receiver);
   }

   TeletextBrowser::ChannelSwitched(ChannelNumber);
}


cTxtReceiver::cTxtReceiver(int TPid, tChannelID chan)
 : cReceiver(chan, -1, TPid), cThread("osdteletext-receiver"),
   chan(chan), TxtPage(0), buffer((188+60)*75), running(false)
{
   Storage::instance()->prepareDirectory(chan);
   // 10 ms timeout on getting TS frames
   buffer.SetTimeouts(0, 10);
}


cTxtReceiver::~cTxtReceiver()
{
   cReceiver::Detach();
   if (running) {
      running=false;
      buffer.Signal();
      Cancel(2);
   }
   buffer.Clear();
   delete TxtPage;
}

void cTxtReceiver::Stop()
{
   Activate(false);
}

void cTxtReceiver::Activate(bool On)
{
  if (On) {
     if (!running) {
        running=true;
        Start();
        }
     }
  else if (running) {
     running = false;
     buffer.Signal();
     Cancel(2);
     }
}

void cTxtReceiver::Receive(uchar *Data, int Length)
{
   int len = Length+60;

   if (!buffer.Check(len)) {
      // Buffer overrun
      buffer.Signal();
      return;
   }
   cFrame *frame=new cFrame(Data, len);
   if (frame && !buffer.Put(frame)) {
      // Buffer overrun
      delete frame;
      buffer.Signal();
   }
}

void cTxtReceiver::Action() {

   while (running) {
      cFrame *frame=buffer.Get();
      if (frame) {
         uchar *Datai=frame->Data();

         for (int i=0; i < 4; i++) {
            if (Datai[4+i*46]==2 || Datai[4+i*46]==3) {
               for (int j=(8+i*46);j<(50+i*46);j++)
                  Datai[j]=invtab[Datai[j]];
               DecodeTXT(&Datai[i*46]);
            }
         }

         buffer.Drop(frame);
      } else
         buffer.Wait();
   }

   buffer.Clear();
   running=false;
}

uchar cTxtReceiver::unham16 (uchar *p)
{
  unsigned short c1,c2;
  c1=unhamtab[p[0]];
  c2=unhamtab[p[1]];
  return (c1 & 0x0F) | (c2 & 0x0F) *16;
}

void cTxtReceiver::DecodeTXT(uchar* TXT_buf)
{
   // Format of buffer:
   //   0x00-0x04  ?
   //   0x05-0x06  Clock Run-In?
   //   0x07       Framing Code?
   //   0x08       Magazine number (100-digit of page number)
   //   0x09       Line number
   //   0x0A..0x31 Line data
   // Line 0 only:
   //   0x0A       10-digit of page number
   //   0x0B       1-digit of page number
   //   0x0C       Sub-Code bits 0..3
   //   0x0D       Sub-Code bits 4..6 + C4 flag
   //   0x0E       Sub-Code bits 8..11
   //   0x0F       Sub-Code bits 12..13 + C5,C6 flag
   //   0x10       C7-C10 flags
   //   0x11       C11-C14 flags
   //
   // Flags:
   //   C4 - Erase last page, new page transmitted
   //   C5 - News flash, boxed display
   //   C6 - Subtitle, boxed display
   //   C7 - Suppress Header, dont show line 0
   //   C8 - Update, page has changed
   //   C9 - Interrupt Sequence, page number is out of order
   //   C10 - Inhibit Display
   //   C11 - Magazine Serial mode
   //   C12-C14 - Language selection, lower 3 bits


   int hdr,mag,mag8,line;
   uchar *ptr;
   uchar flags,lang;

   hdr = unham16 (&TXT_buf[0x8]);
   mag = hdr & 0x07;
   mag8 = mag ?: 8;
   line = (hdr>>3) & 0x1f;
   ptr = &TXT_buf[10];

   switch (line) {
   case 0: 
      {
      unsigned char b1, b2, b3, b4;
      int pgno, subno;
      b1 = unham16 (ptr);    
      // Page no, 10- and 1-digit

      if (b1 == 0xff) break;
      if (TxtPage) {
         TxtPage->save();
         delete TxtPage;
         TxtPage=NULL;
      }

      b2 = unham16 (ptr+2); // Sub-code 0..6 + C4
      b3 = unham16 (ptr+4); // Sub-code 8..13 + C5,C6
      b4 = unham16 (ptr+6); // C7..C14

      // flags:
      //   0x80  C4 - Erase page
      //   0x40  C5 - News flash
      //   0x20  C6 - Subtitle
      //   0x10  C7 - Suppress Header
      //   0x08  C8 - Update
      //   0x04  C9 - Interrupt Sequence
      //   0x02  C9 (Bug?)
      //   0x01  C11 - Magazine Serial mode
      flags=b2 & 0x80;
      flags|=(b3&0x40)|((b3>>2)&0x20); //??????
      flags|=((b4<<4)&0x10)|((b4<<2)&0x08)|(b4&0x04)|((b4>>1)&0x02)|((b4>>4)&0x01);
      lang=((b4>>5) & 0x07);

      pgno = mag8 * 256 + b1;
      subno = (b2 + b3 * 256) & 0x3f7f;         // Sub Page Number

      TxtPage = new cTelePage(PageID(chan, pgno, subno), flags, lang, mag);
      TxtPage->SetLine((int)line,(uchar *)ptr);
      break;
      }
   case 1 ... 25: 
      {
      if (TxtPage) TxtPage->SetLine((int)line,(uchar *)ptr); 
      break;
      }
   /*case 23: 
      {
      if (TxtPage) {
         TxtPage->save();
         delete TxtPage;
         TxtPage=NULL;
      }
      break;
      }*/
   default:
      break;
   }
}


