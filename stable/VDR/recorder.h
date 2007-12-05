/*
 * recorder.h: The actual DVB recorder
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __RECORDER_H
#define __RECORDER_H

#include "receiver.h"
#include "recording.h"
#include "remux.h"
#include "ringbuffer.h"
#include "thread.h"

class cFileWriter;

class cRecorder : public cReceiver
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
{
#else
, cThread {
#endif
private:
//M7X0 BEGIN AK
#ifdef DISABLE_RINGBUFFER_IN_RECEIVER
  int lostBytes;
#else
#ifdef USE_RECEIVER_RINGBUFFER
  cRingBufferReceiver *ringBuffer;
#else
  cRingBufferLinear *ringBuffer;
#endif
#endif
//M7X0 END AK
  cRemux *remux;
  cFileWriter *writer;
protected:
  virtual void Activate(bool On);
  virtual void Receive(uchar *Data, int Length);
//M7X0 BEGIN AK
#if defined(USE_RECEIVER_RINGBUFFER) || defined(DISABLE_RINGBUFFER_IN_RECEIVER)
  virtual void Receive(uchar *Data, int Length, const sTsDataHeader *const Header);
#endif

#ifndef DISABLE_RINGBUFFER_IN_RECEIVER
  virtual void Action(void);
#endif
//M7X0 END AK
public:
  cRecorder(const char *FileName, int Ca, int Priority, int VPid, const int *APids, const int *DPids, const int *SPids);
               // Creates a new recorder that requires conditional access Ca, has
               // the given Priority and will record the given PIDs into the file FileName.
  virtual ~cRecorder();
  };

#endif //__RECORDER_H
