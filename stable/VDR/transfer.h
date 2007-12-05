/*
 * transfer.h: Transfer mode
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __TRANSFER_H
#define __TRANSFER_H

#include "player.h"
#include "receiver.h"
#include "remux.h"
#include "ringbuffer.h"
#include "thread.h"
//M7X0 BEGIN AK
class cTransfer : public cReceiver, public cPlayer {
private:
  int pPid, vPid, aPid, tPid;
protected:
  virtual void Activate(bool On);
  virtual void Receive(uchar *Data, int Length) {}
#if defined(USE_RECEIVER_RINGBUFFER) || defined(DISABLE_RINGBUFFER_IN_RECEIVER)
  virtual void Receive(uchar *Data, int Length, const sTsDataHeader *const Header) {}
#endif
public:
  cTransfer(const int PPid, const int VPid, const int APid, const int TPid);
  virtual ~cTransfer();
  virtual void SetAudioTrack(eTrackType Type, const tTrackId *TrackId);
  };

class cTransferControl : public cControl {
private:
  cTransfer *transfer;
  static cDevice *receiverDevice;
public:
  cTransferControl(cDevice *ReceiverDevice, const int PPid, const int VPid, const int APid, const int TPid);
  ~cTransferControl();
  virtual void Hide(void) {}
  static cDevice *ReceiverDevice(void) { return receiverDevice; }
  };

#endif //__TRANSFER_H
