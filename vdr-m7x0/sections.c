/*
 * sections.c: Section data handling
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "sections.h"
#include <unistd.h>
#include "channels.h"
#include "device.h"
#include "thread.h"

// --- cFilterHandle----------------------------------------------------------

class cFilterHandle : public cListObject {
public:
  cFilterData filterData;
//M7X0 BEGIN AK
  time_t lastData;
//M7X0 END AK
  int handle;
  int used;
  cFilterHandle(const cFilterData &FilterData);
  };

cFilterHandle::cFilterHandle(const cFilterData &FilterData)
{
  filterData = FilterData;
  handle = -1;
  used = 0;
}

// --- cSectionHandlerPrivate ------------------------------------------------

class cSectionHandlerPrivate {
public:
  cChannel channel;
  };

// --- cSectionHandler -------------------------------------------------------

cSectionHandler::cSectionHandler(cDevice *Device)
:cThread("section handler")
{
  shp = new cSectionHandlerPrivate;
  device = Device;
  statusCount = 0;
  on = false;
  waitForLock = false;
  Start();
}

cSectionHandler::~cSectionHandler()
{
  Cancel(-1);
  cFilter *fi;
  while ((fi = filters.First()) != NULL)
        Detach(fi);
  delete shp;
}

int cSectionHandler::Source(void)
{
  return shp->channel.Source();
}

int cSectionHandler::Transponder(void)
{
  return shp->channel.Transponder();
}

const cChannel *cSectionHandler::Channel(void)
{
  return &shp->channel;
}

void cSectionHandler::Add(const cFilterData *FilterData)
{
  Lock();
  statusCount++;
  cFilterHandle *fh;
  for (fh = filterHandles.First(); fh; fh = filterHandles.Next(fh)) {
      if (fh->filterData.Is(FilterData->pid, FilterData->tid, FilterData->mask))
         break;
      }
  if (!fh) {
     int handle = device->OpenFilter(FilterData->pid, FilterData->tid, FilterData->mask);
     if (handle >= 0) {
        fh = new cFilterHandle(*FilterData);
        fh->handle = handle;
//M7X0 BEGIN AK
        fh->lastData = time(NULL);
//M7X0 END AK
        filterHandles.Add(fh);
        }
     }
  if (fh)
     fh->used++;
  Unlock();
}

void cSectionHandler::Del(const cFilterData *FilterData)
{
  Lock();
  statusCount++;
  cFilterHandle *fh;
  for (fh = filterHandles.First(); fh; fh = filterHandles.Next(fh)) {
      if (fh->filterData.Is(FilterData->pid, FilterData->tid, FilterData->mask)) {
         if (--fh->used <= 0) {
            close(fh->handle);
            filterHandles.Del(fh);
            break;
            }
         }
      }
  Unlock();
}

void cSectionHandler::Attach(cFilter *Filter)
{
  Lock();
  statusCount++;
  filters.Add(Filter);
  Filter->sectionHandler = this;
  if (on)
     Filter->SetStatus(true);
  Unlock();
}

void cSectionHandler::Detach(cFilter *Filter)
{
  Lock();
  statusCount++;
  Filter->SetStatus(false);
  Filter->sectionHandler = NULL;
  filters.Del(Filter, false);
  Unlock();
}

void cSectionHandler::SetChannel(const cChannel *Channel)
{
  Lock();
  shp->channel = Channel ? *Channel : cChannel();
  Unlock();
}

void cSectionHandler::SetStatus(bool On)
{
// M7X0 BEGIN AK
  if (on != On) {
     Lock();
     if (!On || device->HasLock()) {
        statusCount++;
        for (cFilter *fi = filters.First(); fi; fi = filters.Next(fi)) {
            fi->SetStatus(false);
            if (On)
               fi->SetStatus(true);
            }
        on = On;
        waitForLock = false;
        }
     else
        waitForLock = On;
     Unlock();
     }
// M7X0 END AK
}

void cSectionHandler::Action(void)
{
  SetPriority(19);
  while (Running()) {

        Lock();
        if (waitForLock)
           SetStatus(true);
        int NumFilters = filterHandles.Count();
        pollfd pfd[NumFilters];
        for (cFilterHandle *fh = filterHandles.First(); fh; fh = filterHandles.Next(fh)) {
            int i = fh->Index();
            pfd[i].fd = fh->handle;
//M7X0 BEGIN AK
            pfd[i].events = POLLIN | POLLPRI;
//M7X0 END AK
            pfd[i].revents = 0;
            }
        int oldStatusCount = statusCount;
        Unlock();

        if (poll(pfd, NumFilters, 1000) > 0) {
           bool DeviceHasLock = device->HasLock();
           if (!DeviceHasLock)
              cCondWait::SleepMs(100);
//M7X0 BEGIN AK
              time_t now = time(NULL);
           for (int i = 0; i < NumFilters; i++) {

               cFilterHandle *fh = NULL;
               LOCK_THREAD;
               if (statusCount != oldStatusCount)
                  break;

               for (fh = filterHandles.First(); fh; fh = filterHandles.Next(fh)) {
                   if (pfd[i].fd == fh->handle)
                      break;
                   }

               if (!fh)
                  continue;

               if ( pfd[i].revents & (POLLPRI | POLLIN) ){
                  // Read section data:
                  unsigned char buf[8192]; // max. allowed size for any EIT section

                  int r = safe_read(fh->handle, buf, sizeof(buf)) ;
                  fh->lastData = now;

                  if (!DeviceHasLock)
                     continue; // we do the read anyway, to flush any data that might have come from a different transponder
                  if (r > 3) { // minimum number of bytes necessary to get section length
                     int len = (((buf[1] & 0x0F) << 8) | (buf[2] & 0xFF)) + 3;
                     if (len <= r) {
                        // Distribute data to all attached filters:
                        int pid = fh->filterData.pid;
                        int tid = buf[0];
                        for (cFilter *fi = filters.First(); fi; fi = filters.Next(fi)) {
                            if (fi->Matches(pid, tid))
                               fi->Process(pid, tid, buf, len);
                            }
                        }
                        else
                           dsyslog("tp %d (%d/%02X) read incomplete section - len = %d, r = %d", Transponder(), fh->filterData.pid, buf[0], len, r);
                     }
                  }
               else if (on && fh->filterData.timeout + fh->lastData < now) {
                  dsyslog("Section Filter with Pid 0x%x timeouts. Reopen", fh->filterData.pid);
                  close(fh->handle);
                  fh->handle = device->OpenFilter(fh->filterData.pid, fh->filterData.tid, fh->filterData.mask);
                  if (fh->handle >= 0)
                     fh->lastData = time(NULL);
                  }
               }
           }
        }
}
//M7X0 END AK
