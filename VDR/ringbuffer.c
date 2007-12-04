/*
 * ringbuffer.c: A ring buffer
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * Parts of this file were inspired by the 'ringbuffy.c' from the
 * LinuxDVB driver (see linuxtv.org).
 *
 * $Id$
 */

#include "ringbuffer.h"
//M7X0 BEGIN AK
#include "recording.h" // FileName is needed
//M7X0 END AK
#include <stdlib.h>
#include <unistd.h>
#include "tools.h"

// --- cRingBuffer -----------------------------------------------------------

#define OVERFLOWREPORTDELTA 5 // seconds between reports
#define PERCENTAGEDELTA     10
#define PERCENTAGETHRESHOLD 70
//M7X0 BEGIN AK
cRingBuffer::cRingBuffer(int Size, bool Statistics, const char* Description)
{
  description = Description ? strdup(Description) : NULL;
  putWaitCounter = 0;
  getWaitCounter = 0;
  putSignalCounter = 0;
  getSignalCounter = 0;
  minPutFree = size / 3;
  minGetAvail = size / 3;
//M7X0 END AK
  size = Size;
  statistics = Statistics;
  getThreadTid = 0;
  maxFill = 0;
  lastPercent = 0;
  putTimeout = getTimeout = 0;
  lastOverflowReport = 0;
  overflowCount = overflowBytes = 0;

}

cRingBuffer::~cRingBuffer()
{
//M7X0 BEGIN AK
  if (statistics)
     dsyslog("buffer stats%s%s: %d (%d%%) used",description?" for ":"",
    description?description:"", maxFill, maxFill * 100 / (size - 1));

//  dsyslog("signal buffer stats%s%s: Wait: Put %llu Get %llu Signal: Put %llu Get %llu",description?" for ":"",
//     description?description:"", putWaitCounter, getWaitCounter, putSignalCounter, getSignalCounter);
	free(description);
//M7X0 END AK
}

void cRingBuffer::UpdatePercentage(int Fill)
{
  if (Fill > maxFill)
     maxFill = Fill;
  int percent = Fill * 100 / (Size() - 1) / PERCENTAGEDELTA * PERCENTAGEDELTA;
  if (percent != lastPercent) {
     if (percent >= PERCENTAGETHRESHOLD && percent > lastPercent || percent < PERCENTAGETHRESHOLD && lastPercent >= PERCENTAGETHRESHOLD) {
//M7X0 BEGIN AK
        dsyslog("buffer usage%s%s: %d%% (tid=%d)",description?" for ":"",
	  description?description:"", percent, getThreadTid);
//M7X0 END AK
        lastPercent = percent;
        }
     }
}
//M7X0 BEGIN AK
void cRingBuffer::WaitForPut(void)
{
  if (putTimeout) {
     putWaiting = true;
     // putWaitCounter++;
     readyForPut.Wait(putTimeout);
     putWaiting = false;
     }
}

void cRingBuffer::WaitForGet(void)
{
  if (getTimeout) {
     getWaiting = true;
     //getWaitCounter++;
     readyForGet.Wait(getTimeout);
     getWaiting = false;
     }
}

void cRingBuffer::EnablePut(void)
{
  if (putTimeout && putWaiting && Free() > minPutFree) {
     //putSignalCounter++;
     readyForPut.Signal();
     }
}

void cRingBuffer::EnableGet(void)
{
  if (getTimeout && getWaiting && Available() > minGetAvail) {
     // getSignalCounter++;
     readyForGet.Signal();
     }
}
void cRingBuffer::SetLimits(int PutFree, int GetAvail)
{
  minPutFree = PutFree;
  minGetAvail = GetAvail;
}
//M7X0 END AK
void cRingBuffer::SetTimeouts(int PutTimeout, int GetTimeout)
{
  putTimeout = PutTimeout;
  getTimeout = GetTimeout;
}

void cRingBuffer::ReportOverflow(int Bytes)
{
  overflowCount++;
  overflowBytes += Bytes;
  if (time(NULL) - lastOverflowReport > OVERFLOWREPORTDELTA) {
//M7X0 BEGIN AK
     esyslog("ERROR: %d ring buffer overflow%s (%d bytes dropped)%s%s", overflowCount, overflowCount > 1 ? "s" : "", overflowBytes,
	  description?" on ":"", description?description:"");
//M7X0 END AK
     overflowCount = overflowBytes = 0;
     lastOverflowReport = time(NULL);
     }
}

// --- cRingBufferLinear -----------------------------------------------------

#ifdef DEBUGRINGBUFFERS
#define MAXRBLS 30
#define DEBUGRBLWIDTH 45

cRingBufferLinear *cRingBufferLinear::RBLS[MAXRBLS] = { NULL };

void cRingBufferLinear::AddDebugRBL(cRingBufferLinear *RBL)
{
  for (int i = 0; i < MAXRBLS; i++) {
      if (!RBLS[i]) {
         RBLS[i] = RBL;
         break;
         }
      }
}

void cRingBufferLinear::DelDebugRBL(cRingBufferLinear *RBL)
{
  for (int i = 0; i < MAXRBLS; i++) {
      if (RBLS[i] == RBL) {
         RBLS[i] = NULL;
         break;
         }
      }
}

void cRingBufferLinear::PrintDebugRBL(void)
{
  bool printed = false;
  for (int i = 0; i < MAXRBLS; i++) {
      cRingBufferLinear *p = RBLS[i];
      if (p) {
         printed = true;
         int lh = p->lastHead;
         int lt = p->lastTail;
         int h = lh * DEBUGRBLWIDTH / p->Size();
         int t = lt * DEBUGRBLWIDTH / p->Size();
         char buf[DEBUGRBLWIDTH + 10];
         memset(buf, '-', DEBUGRBLWIDTH);
         if (lt <= lh)
            memset(buf + t, '*', max(h - t, 1));
         else {
            memset(buf, '*', h);
            memset(buf + t, '*', DEBUGRBLWIDTH - t);
            }
         buf[t] = '<';
         buf[h] = '>';
         buf[DEBUGRBLWIDTH] = 0;
         printf("%2d %s %8d %8d %s\n", i, buf, p->lastPut, p->lastGet, p->description);
         }
      }
  if (printed)
     printf("\n");
  }
#endif
//M7X0 BEGIN AK
cRingBufferLinear::cRingBufferLinear(int Size, int Margin, bool Statistics, const char *Description)
:cRingBuffer(Size, Statistics,Description)
{
//Put to cRingBuffer to easier see which buffer report which thing
  //description = Description ? strdup(Description) : NULL;
//M7X0 END AK
  tail = head = margin = Margin;
  gotten = 0;
  buffer = NULL;
  if (Size > 1) { // 'Size - 1' must not be 0!
     if (Margin <= Size / 2) {
        buffer = MALLOC(uchar, Size);
        if (!buffer)
           esyslog("ERROR: can't allocate ring buffer (size=%d)", Size);
        Clear();
        }
     else
        esyslog("ERROR: invalid margin for ring buffer (%d > %d)", Margin, Size / 2);
     }
  else
     esyslog("ERROR: invalid size for ring buffer (%d)", Size);
#ifdef DEBUGRINGBUFFERS
  lastHead = head;
  lastTail = tail;
  lastPut = lastGet = -1;
  AddDebugRBL(this);
#endif
}

cRingBufferLinear::~cRingBufferLinear()
{
#ifdef DEBUGRINGBUFFERS
  DelDebugRBL(this);
#endif
  free(buffer);
//M7X0 BEGIN AK
 // free(description);
//M7X0 END AK
}

int cRingBufferLinear::Available(void)
{
  int diff = head - tail;
  return (diff >= 0) ? diff : Size() + diff - margin;
}

void cRingBufferLinear::Clear(void)
{
  tail = head;
#ifdef DEBUGRINGBUFFERS
  lastHead = head;
  lastTail = tail;
  lastPut = lastGet = -1;
#endif
  maxFill = 0;
  EnablePut();
}

int cRingBufferLinear::Read(int FileHandle, int Max)
{
  int Tail = tail;
  int diff = Tail - head;
  int free = (diff > 0) ? diff - 1 : Size() - head;
  if (Tail <= margin)
     free--;
  int Count = 0;
  if (free > 0) {
     if (0 < Max && Max < free)
        free = Max;
     Count = safe_read(FileHandle, buffer + head, free);
     if (Count > 0) {
        int Head = head + Count;
        if (Head >= Size())
           Head = margin;
        head = Head;
        if (statistics) {
           int fill = head - Tail;
           if (fill < 0)
              fill = Size() + fill;
           else if (fill >= Size())
              fill = Size() - 1;
           UpdatePercentage(fill);
           }
        }
     }
#ifdef DEBUGRINGBUFFERS
  lastHead = head;
  lastPut = Count;
#endif
  EnableGet();
  if (free == 0)
     WaitForPut();
  return Count;
}

int cRingBufferLinear::Put(const uchar *Data, int Count)
{
  if (Count > 0) {
     int Tail = tail;
     int rest = Size() - head;
     int diff = Tail - head;
     int free = ((Tail < margin) ? rest : (diff > 0) ? diff : Size() + diff - margin) - 1;
     if (statistics) {
        int fill = Size() - free - 1 + Count;
        if (fill >= Size())
           fill = Size() - 1;
        UpdatePercentage(fill);
        }
     if (free > 0) {
        if (free < Count)
           Count = free;
        if (Count >= rest) {
           memcpy(buffer + head, Data, rest);
           if (Count - rest)
              memcpy(buffer + margin, Data + rest, Count - rest);
           head = margin + Count - rest;
           }
        else {
           memcpy(buffer + head, Data, Count);
           head += Count;
           }
        }
     else
        Count = 0;
#ifdef DEBUGRINGBUFFERS
     lastHead = head;
     lastPut = Count;
#endif
     EnableGet();
     if (Count == 0)
        WaitForPut();
     }
  return Count;
}

uchar *cRingBufferLinear::Get(int &Count)
{
  uchar *p = NULL;
  int Head = head;
  if (getThreadTid <= 0)
     getThreadTid = cThread::ThreadId();
  int rest = Size() - tail;
  if (rest < margin && Head < tail) {
     int t = margin - rest;
     memcpy(buffer + t, buffer + tail, rest);
     tail = t;
     rest = Head - tail;
     }
  int diff = Head - tail;
  int cont = (diff >= 0) ? diff : Size() + diff - margin;
  if (cont > rest)
     cont = rest;
  if (cont >= margin) {
     p = buffer + tail;
     Count = gotten = cont;
     }
  if (!p)
     WaitForGet();
  return p;
}

void cRingBufferLinear::Del(int Count)
{
  if (Count > gotten) {
     esyslog("ERROR: invalid Count in cRingBufferLinear::Del: %d (limited to %d)", Count, gotten);
     Count = gotten;
     }
  if (Count > 0) {
     int Tail = tail;
     Tail += Count;
     gotten -= Count;
     if (Tail >= Size())
        Tail = margin;
     tail = Tail;
     EnablePut();
     }
#ifdef DEBUGRINGBUFFERS
  lastTail = tail;
  lastGet = Count;
#endif
}
//M7X0 BEGIN AK

// --- cRingBufferReceiver -----------------------------------------------
#ifdef USE_RECEIVER_RINGBUFFER
cRingBufferReceiver::cRingBufferReceiver(int Size, bool Statistics,  const char *Description)
:cRingBuffer((Size + (sizeof(int) - 1)) & ~(sizeof(int) - 1), Statistics,Description)
{

  tail = head  = 0;
  buffer = NULL;

  gotten = 0;
  gottenCount = 0;
  end = (Size + (sizeof(int) - 1)) & ~(sizeof(int) - 1);
  buffer = MALLOC(uchar, (Size + (sizeof(int) - 1)) & ~(sizeof(int) - 1));
  if (!buffer)
     esyslog("ERROR: can't allocate ring buffer (size=%d)", Size);
  Clear();
}

cRingBufferReceiver::~cRingBufferReceiver()
{
  free(buffer);
}

void cRingBufferReceiver::Clear(void)
{
  tail = head;
  end = Size();
  maxFill = 0;
  gotten = 0;
  gottenCount = 0;
  EnablePut();
}

bool cRingBufferReceiver::Put(const uchar *const Data, int Count, const sTsDataHeader *const Header)
{
  if (Count <= 0 || !Header)
     return false;

  int Head = head;
  int Tail = tail;
  const int size = Size();
  const int need = sizeof(sTsDataHeader) + sizeof(int) + Count;
  const int rest = size - Head;
  const int skip = need > rest ? rest : 0;


  int diff = Tail - Head;
  int free = (diff > 0 ? diff : size + diff) - 1 - skip;

  if (free < need) {
     EnableGet();
     WaitForPut();
     Tail = tail;

     diff = Tail - Head;
     free = (diff > 0 ? diff : size + diff) - 1 - skip;
     if (free < need)
        return false;
     }

  if (skip) {
     end = Head;
     Head = 0;
     }

  memcpy(buffer + Head, Header, sizeof(sTsDataHeader));
  Head += sizeof(sTsDataHeader);
  *((int *) (buffer + Head)) = Count;
  Head += sizeof(int);
  memcpy(buffer + Head, Data, Count);
  Head += Count;

  if (Head == size)
     Head = 0;

  head = Head;


  if (statistics) {
     const int fill = size - free - 1 + need;
     UpdatePercentage(fill);
     }
  EnableGet();
  return true;
}

sTsDataHeader *cRingBufferReceiver::Get(uchar *&Data, int &Count)
{
  if (getThreadTid <= 0)
     getThreadTid = cThread::ThreadId();

  if (gotten) {
     Data = gottenData;
     Count = gottenCount;
     return (sTsDataHeader *) (buffer + tail);
     }

  int Tail = tail;
  int Head = head;
  int End = end;

  if (Tail == End && Head < End) {
     tail = 0;
     end = Size();
     Tail = 0;
     }

  if (Tail == Head) {
     EnablePut();
     WaitForGet();
     Head = head;
     End = end;

     if (Tail == End && Head < End) {
        tail = 0;
        end = Size();
        Tail = 0;
        }
     if (Tail == Head)
        return NULL;
     }


  sTsDataHeader *p = (sTsDataHeader *) (buffer + Tail);
  Data = gottenData = buffer + Tail + sizeof(sTsDataHeader) + sizeof(int);
  Count = gottenCount = *((int *) (buffer + Tail + sizeof(sTsDataHeader)));
  gotten = gottenCount + sizeof(sTsDataHeader) + sizeof(int);

  return p;
}

void cRingBufferReceiver::Del(int Count)
{
  if (Count > gottenCount) {
     esyslog("ERROR: invalid Count in cRingBufferReceiver::Del: %d (limited to %d)", Count, gotten);
     Count = gottenCount;
     }

  if (Count <= 0)
      return;

  int Tail = tail;

  if (Count == gottenCount) {
     Tail += gotten;
     gotten = gottenCount = 0;
     tail = Tail;

     EnablePut();
     return;
     }

  sTsDataHeader *p = (sTsDataHeader *) (buffer + Tail);
  if (p->startsWithVideoFrame > tsVideoFrameNone)
     p->startsWithVideoFrame = tsVideoFrameNone;

  gottenData += Count;
  gottenCount -= Count;
}
#endif
// --- cRingBufferResult -----------------------------------------------

cRingBufferResult::cRingBufferResult(int Size, bool IsRadio, bool Statistics,  const char *Description)
:cRingBuffer((Size + 2 * RESULT_BUFFER_ALIGNMENT - 1) & ~(RESULT_BUFFER_ALIGNMENT - 1), Statistics,Description)
{

  isRadio = IsRadio;
  firstHeader = lastHeader = tail = head = 0;
  buffer = NULL;
  realign = false;
  gotten = 0;
  headersGotten = 0;

  void *tmp;
  if (posix_memalign(&tmp, RESULT_BUFFER_ALIGNMENT, (Size + 2 * RESULT_BUFFER_ALIGNMENT - 1) & ~(RESULT_BUFFER_ALIGNMENT - 1)))
     esyslog("ERROR: can't allocate ring buffer (size=%d)", Size);

  buffer = (uchar *) tmp;

  headersSize = ((Size + 2 * RESULT_BUFFER_ALIGNMENT - 1) & ~(RESULT_BUFFER_ALIGNMENT - 1)) / RESULT_BYTES_PER_HEADER + 2;
  pesHeader = new sPesResult[headersSize];
  pesHeadersGotten = MALLOC(sPesResult, RESULT_INIT_GET_HEADERS);
  pesHeadersGottenSize = RESULT_INIT_GET_HEADERS;
  if (!pesHeadersGotten) {
     esyslog("ERROR: cannot allocate get headers");
     pesHeadersGottenSize = 0;
     }
  Clear();
}

cRingBufferResult::~cRingBufferResult()
{
  free(pesHeadersGotten);
  delete[] pesHeader;
  free(buffer);
}

void cRingBufferResult::Clear(void)
{
  tail = 0;
  head = 0;
  firstHeader = 0;
  lastHeader = 0;
  maxFill = 0;
  gotten = 0;
  realign = false;
  EnablePut();
}

void cRingBufferResult::DoRealign(void)
{

  //dsyslog("Ringbuffer: Doing Realign!!");
  int Head = head;
  int Tail = tail;

  const int newTail = Tail & ~(RESULT_BUFFER_ALIGNMENT - 1);

  const int tailDiff = Tail - newTail;

  if (!tailDiff) {
     realign = false;
     EnableGet();
     return;
     }

  const int diff = Head - Tail;

  if (diff == 0) {
     head = newTail;
     tail = newTail;
     realign = false;
     return;
     }

  if (diff > 0) {
     memcpy(buffer + newTail, buffer + Tail, diff);
     Head -= tailDiff;
     head = Head;
     tail = newTail;

     const int FirstHeader = firstHeader;
     const int LastHeader = lastHeader;
     int i = FirstHeader;

     while (i != LastHeader) {
           pesHeader[i].offset -= tailDiff;
           i++;
           if (i == headersSize) {
              i = 0;
              }
           }
     realign = false;
     EnableGet();
     return;
     }

  const int size = Size();
  int rest = size - Tail;
  memcpy(buffer + newTail, buffer + Tail, rest);

  Tail = newTail + rest;
  rest = size - Tail;

  if (rest > Head) {
     if (Head)
        memcpy(buffer + Tail, buffer, Head);
     Head += Tail;
     }
  else {
     memcpy(buffer + Tail, buffer, rest);
     Head -= rest;
     if (Head) {
        memcpy(buffer, buffer + rest, Head);
        }
     }

  head = Head;
  tail = newTail;

  const int FirstHeader = firstHeader;
  const int LastHeader = lastHeader;
  int i = FirstHeader;

  while (i != LastHeader) {
        pesHeader[i].offset -= tailDiff;
        if (pesHeader[i].offset < 0)
           pesHeader[i].offset += size;
        i++;
        if (i == headersSize) {
           i = 0;
           }
        }

  realign = false;
  EnableGet();
}

bool cRingBufferResult::Put(const uchar *const Data, int Count, const sPesResult *const Header)
{
  if (realign)
     DoRealign();

  if (Count <= 0)
     return false;

  int Head = head;
  int Tail = tail;
  Tail &= ~(RESULT_BUFFER_ALIGNMENT - 1);
  const int size = Size();
  int FirstHeader = firstHeader;
  int LastHeader = lastHeader;

  int diff = Tail - Head;
  int free = (diff > 0 ? diff : size + diff) - 1;

  int headerDiff = FirstHeader - LastHeader;
  int headerFree = (headerDiff > 0 ? headerDiff : headerDiff + headersSize) -1;


  const bool headerNeed = Header && Header->pictureType != 0 &&
                             (isRadio || Header->pesPacketType == pesPtVideo);

  if (free < Count || (headerNeed && !headerFree)) {
     EnableGet();
     WaitForPut();

     Tail = tail;
     Tail &= ~(RESULT_BUFFER_ALIGNMENT - 1);
     diff = Tail - Head;
     free = (diff > 0 ? diff : size + diff) - 1;

     FirstHeader = firstHeader;
     headerDiff = FirstHeader - LastHeader;
     headerFree = (headerDiff > 0 ? headerDiff : headerDiff + headersSize) - 1;

     if (free < Count || (headerNeed && !headerFree))
        return false;
     }

  if (headerNeed) {
     memcpy(&pesHeader[LastHeader], Header, sizeof(sPesResult));

     pesHeader[LastHeader].offset = Head;
     LastHeader++;
     if (LastHeader == headersSize)
        LastHeader = 0;
     lastHeader = LastHeader;
     }

 if (statistics) {
     const int fill = size - free - 1 + Count;
     UpdatePercentage(fill);
     }

  const int rest = size - Head;
  if (Count >= rest) {
     memcpy(buffer + Head, Data, rest);
     Count -= rest;
     if (Count)
        memcpy(buffer,Data + rest, Count);
     Head = Count;
     }
  else {
     memcpy(buffer + Head, Data, Count);
     Head += Count;
     }

  head = Head;

  EnableGet();
  return true;
}

uchar *cRingBufferResult::Get(int &Count, sPesResult *&Headers, int &HeaderCount)
{
  if (getThreadTid <= 0)
     getThreadTid = cThread::ThreadId();

  if (realign) {
     EnablePut();
     WaitForGet();

     if (realign) {
        EnablePut();
        return NULL;
        }
     }

  if (gotten) {
     esyslog("cRingbufferResult: Get() without Del()");
     return NULL;
     }

  int Tail = tail;
  int Head = head;

  int diff = Head - Tail;
  const int size = Size();
  int available = diff >= 0 ? diff : diff + size;

  if (available < RESULT_BUFFER_ALIGNMENT) {
     EnablePut();
     WaitForGet();

     Head = head;
     diff = Head - Tail;
     available = diff >= 0 ? diff : diff + size;

     if (available < RESULT_BUFFER_ALIGNMENT)
        return NULL;

     }

  uchar *p = buffer + Tail;
  Count = RESULT_BUFFER_ALIGNMENT;
  gotten = RESULT_BUFFER_ALIGNMENT;

  const int FirstHeader = firstHeader;
  const int LastHeader = lastHeader;

  int i = FirstHeader;
  int count = 0;
  const int end = Tail + RESULT_BUFFER_ALIGNMENT;
  while (i != LastHeader && pesHeader[i].offset >= Tail && pesHeader[i].offset < end) {
        count++;
        i++;
        if (i == headersSize)
           i = 0;
        }


  headersGotten = count;
  HeaderCount = count;
  sPesResult *headers = pesHeadersGotten;
  if (count) {
     if (pesHeadersGottenSize < count) {
        headers = (sPesResult *) realloc(pesHeadersGotten, sizeof(sPesResult) * count);
        if (!headers) {
           esyslog("ERROR Cannot alloc get headers");
           gotten = 0;
           return NULL;
           }
        pesHeadersGottenSize = count;
        pesHeadersGotten = headers;
        }

     Headers = headers;
     int restHeaders = headersSize - FirstHeader;
     if (restHeaders < count) {
        memcpy(headers, &pesHeader[FirstHeader], sizeof(sPesResult) * restHeaders);
        memcpy(&headers[restHeaders], pesHeader, sizeof(sPesResult) * (count - restHeaders));
        }
     else {
        memcpy(headers, &pesHeader[FirstHeader], sizeof(sPesResult) * count);
        }
     }
  for (i = 0; i < count; i++)
      headers[i].offset -= Tail;

  return p;
}

void cRingBufferResult::Del(int Count)
{
  if (Count > gotten) {
     esyslog("ERROR: invalid Count in cRingBufferWithDataInfo::Del: %d (limited to %d)", Count, gotten);
     Count = gotten;
     }

  int Tail = tail;
  int FirstHeader = firstHeader;
  if (Count == gotten) {
     Tail += RESULT_BUFFER_ALIGNMENT;
     if (Tail == Size())
        Tail = 0;

     gotten = 0;

     FirstHeader += headersGotten;
     if (FirstHeader >= headersSize)
        FirstHeader -= headersSize;

     firstHeader = FirstHeader;
     headersGotten = 0;
     tail = Tail;
     EnablePut();
     return;
     }

  Tail += Count;
  gotten = 0;

  while (headersGotten > 0 && pesHeader[FirstHeader].offset < Tail) {
        FirstHeader++;
        headersGotten--;
        if (FirstHeader == headersSize)
           FirstHeader = 0;
        }

  headersGotten = 0;
  firstHeader = FirstHeader;
  tail = Tail;
  realign = true;
  EnablePut();
}

//M7X0 END AK
// --- cFrame ----------------------------------------------------------------

cFrame::cFrame(const uchar *Data, int Count, eFrameType Type, int Index)
{
  count = abs(Count);
  type = Type;
  index = Index;
  if (Count < 0)
     data = (uchar *)Data;
  else {
     data = MALLOC(uchar, count);
     if (data)
        memcpy(data, Data, count);
     else
        esyslog("ERROR: can't allocate frame buffer (count=%d)", count);
     }
  next = NULL;
}

cFrame::~cFrame()
{
  free(data);
}

// --- cRingBufferFrame ------------------------------------------------------

cRingBufferFrame::cRingBufferFrame(int Size, bool Statistics)
:cRingBuffer(Size, Statistics)
{
  head = NULL;
  currentFill = 0;
}

cRingBufferFrame::~cRingBufferFrame()
{
  Clear();
}

void cRingBufferFrame::Clear(void)
{
  Lock();
  cFrame *p;
  while ((p = Get()) != NULL)
        Drop(p);
  Unlock();
  EnablePut();
  EnableGet();
}

bool cRingBufferFrame::Put(cFrame *Frame)
{
  if (Frame->Count() <= Free()) {
     Lock();
     if (head) {
        Frame->next = head->next;
        head->next = Frame;
        head = Frame;
        }
     else {
        head = Frame->next = Frame;
        }
     currentFill += Frame->Count();
     Unlock();
     EnableGet();
     return true;
     }
  return false;
}

cFrame *cRingBufferFrame::Get(void)
{
  Lock();
  cFrame *p = head ? head->next : NULL;
  Unlock();
  return p;
}

void cRingBufferFrame::Delete(cFrame *Frame)
{
  currentFill -= Frame->Count();
  delete Frame;
}

void cRingBufferFrame::Drop(cFrame *Frame)
{
  Lock();
  if (head) {
     if (Frame == head->next) {
        if (head->next != head) {
           head->next = Frame->next;
           Delete(Frame);
           }
        else {
           Delete(head);
           head = NULL;
           }
        }
     else
        esyslog("ERROR: attempt to drop wrong frame from ring buffer!");
     }
  Unlock();
  EnablePut();
}

int cRingBufferFrame::Available(void)
{
  Lock();
  int av = currentFill;
  Unlock();
  return av;
}

//M7X0 BEGIN AK
// --- cRingBufferFrameM7x0 ------------------------------------------------------
cRingBufferFrameM7x0::cRingBufferFrameM7x0(int Size, int Alignment, bool Statistics, const char *Description)
:cRingBuffer((Size + 2 * Alignment - 1) & ~(Alignment - 1), Statistics,Description)
{
  alignment = Alignment;
  void *tmp;
  if (posix_memalign(&tmp, Alignment, (Size + 2 * Alignment - 1) & ~(Alignment - 1)))
     esyslog("ERROR: can't allocate ring buffer (size=%d)", Size);

  buffer = (uchar *) tmp;

  head = 0;
  frameHead = 0;
  Clear();
}

cRingBufferFrameM7x0::~cRingBufferFrameM7x0()
{
  free(buffer);
}

int cRingBufferFrameM7x0::ReadReal(cUnbufferedFile *file, uchar *buf, int count)
{
  int retryCount = 0;
  int bytesRead = 0;
  while (bytesRead != count) {
        int r = file->Read(buf + bytesRead, count - bytesRead);

        if (r > 0) {
           retryCount = 0;
           bytesRead += r;
           continue;
        }

        if (!r)
           return bytesRead;

        if (FATALERRNO) {
           if (retryCount < 20 && errno == EIO) {
              esyslog("ERROR: Got an IO Error -- Retrying (%d)", retryCount);
              retryCount++;
              continue;
              }
           LOG_ERROR;
           return r;
           }

        }

  return bytesRead;
}

int cRingBufferFrameM7x0::Read(cUnbufferedFile *file, int FileNr, int Offset, int FrameSize, int Index)
{

  int ReadAHeadCount = readAHeadCount;
  int CurFileNr = curFileNr;
  int CurFileOffset = curFileOffset;
  int readDirectOffset;
  int readFrameOffset;

  if (CurFileNr == FileNr && CurFileOffset == Offset) {
     readDirectOffset = (Offset + (alignment -1)) & ~(alignment -1);
     readFrameOffset = Offset - readDirectOffset;

     if (-readFrameOffset > ReadAHeadCount && FrameSize > ReadAHeadCount)
        FrameSize = ReadAHeadCount;

     }
  else {
     readDirectOffset = Offset & ~(alignment - 1);
     readFrameOffset = Offset & (alignment - 1);
     ReadAHeadCount = -readFrameOffset;
     }

  int readSize = (FrameSize + readFrameOffset + (alignment - 1)) & ~(alignment - 1);

  const int size = Size();
  int Head = head;

  int Tail = tail;
  int diff = Tail - Head;
  int free = (diff > 0 ? diff : size + diff) - 1;

  int FrameHead = frameHead;
  int FrameTail = frameTail;
  int framesDiff = FrameTail - FrameHead;
  int framesFree = (framesDiff > 0 ? framesDiff : MAX_BUFFERED_FRAMES + 1 + framesDiff) - 1;

  if (free < readSize || !framesFree) {
     EnableGet();
     WaitForPut();

     Tail = tail;
     diff = Tail - Head;
     free = (diff > 0 ? diff : size + diff) - 1;

     FrameTail = frameTail;
     framesDiff = FrameTail - FrameHead;
     framesFree = (framesDiff > 0 ? framesDiff : MAX_BUFFERED_FRAMES + 1 + framesDiff) - 1;

     if (free < readSize || !framesFree) {
        errno = EAGAIN;
        return -1;
        }
     }


  int oldHead = Head;
  int bytesRead = 0;

  if (readSize) {
     if (file->Seek(readDirectOffset, SEEK_SET) != readDirectOffset) {
        LOG_ERROR;
        return 0;
        }

     int rest = size - Head;
     if (readSize >= rest) {
        int r = ReadReal(file, buffer + Head, rest);

        if (r < 0)
           return -1;

        readSize -= rest;
        bytesRead = r;
        Head = readSize;

        if (readSize && r == rest) {
           r = ReadReal(file, buffer, readSize);

           if (r < 0)
              return -1;
           bytesRead += r;
           }
        }
     else {
        int r = ReadReal(file, buffer + Head, readSize);

        if (r < 0)
           return -1;

        bytesRead = r;
        Head += readSize;
        }
     }


  int frameSizeRead = bytesRead + ReadAHeadCount;

  ReadAHeadCount = 0;

  if (frameSizeRead > FrameSize) {
     ReadAHeadCount = frameSizeRead - FrameSize;
     frameSizeRead = FrameSize;
     }
  else if (frameSizeRead <= 0)
     return 0;


  CurFileNr = FileNr;
  CurFileOffset = readDirectOffset + frameSizeRead + readFrameOffset;

  int data1 = oldHead + readFrameOffset;
  if (data1 < 0)
     data1 += size;

  int count1 = frameSizeRead;
  int count2 = 0;

  if (data1 + count1 > size) {
     count2 = data1 + count1 - size;
     count1 -= count2;
     }

  frameBuffer[FrameHead].data1 = buffer + data1;
  frameBuffer[FrameHead].data2 = buffer;
  frameBuffer[FrameHead].count1 = count1;
  frameBuffer[FrameHead].count2 = count2;
  frameBuffer[FrameHead].index = Index;
  frameBuffer[FrameHead].fileNo = FileNr;

  FrameHead++;

  if (FrameHead == MAX_BUFFERED_FRAMES + 1)
      FrameHead = 0;

  frameHead = FrameHead;
  head = Head;
  curFileNr = CurFileNr;
  curFileOffset = CurFileOffset;
  readAHeadCount = ReadAHeadCount;
  EnableGet();
  return frameSizeRead;
}

int cRingBufferFrameM7x0::Read(cFileName *fileName, int FileNr, int Offset, int Size, int Index)
{
  cUnbufferedFile *file = fileName->SetOffset(FileNr, -1);
  return (file ? Read(file, FileNr, Offset, Size, Index) : 0);
}

cFrameM7x0 *cRingBufferFrameM7x0::Get(void)
{
  int FrameHead = frameHead;
  const int FrameTail = frameTail;
  if (FrameHead == FrameTail) {
     EnablePut();
     WaitForGet();
     FrameHead = frameHead;
     if (FrameHead == FrameTail)
        return NULL;
     }

  return &frameBuffer[FrameTail];
}

void cRingBufferFrameM7x0::Drop(cFrameM7x0 *frame)
{
  const int FrameHead = frameHead;
  int FrameTail = frameTail;

  if (FrameHead == FrameTail){
     esyslog("ERROR: cRingBufferFrameM7x0::Drop() no frame in Buffer!");
     return;
     }

  if (frame != NULL && &frameBuffer[FrameTail] != frame) {
     esyslog("ERROR: cRingBufferFrameM7x0::Drop() attempt to drop wrong frame from ring buffer!");
     return;
     }

  int Tail = frameBuffer[FrameTail].data1 - buffer + frameBuffer[FrameTail].count1;

  if (Tail >= Size())
     Tail = frameBuffer[FrameTail].count2;

  FrameTail++;
  if (FrameTail == MAX_BUFFERED_FRAMES + 1)
     FrameTail = 0;

  tail = Tail;
  frameTail = FrameTail;
  EnablePut();
}
