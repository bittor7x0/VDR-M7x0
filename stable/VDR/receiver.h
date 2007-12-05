/*
 * receiver.h: The basic receiver interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __RECEIVER_H
#define __RECEIVER_H

#ifdef USE_HW_VIDEO_FRAME_EVENTS
#define USE_DATAHEADER_IN_RECEIVER 1
#endif
#include "device.h"
//M7X0 BEGIN AK


#include "ringbuffer.h"
class cTransfer;
//M7X0 END AK
#define MAXRECEIVEPIDS  64 // the maximum number of PIDs per receiver

class cReceiver {
  friend class cDevice;
//M7X0 BEGIN AK
  friend class cTransfer;
//M7X0 END AK
private:
  cDevice *device;
  int ca;
  int priority;
  int pids[MAXRECEIVEPIDS];
  int numPids;
  bool WantsPid(int Pid);
//M7X0 BEGIN AK
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  bool frameEventsWanted;
  bool WantsFrameEvents(void) { return frameEventsWanted;}
#endif
//M7X0 END AK
protected:
  void Detach(void);
  virtual void Activate(bool On) {}
               ///< This function is called just before the cReceiver gets attached to
               ///< (On == true) or detached from (On == false) a cDevice. It can be used
               ///< to do things like starting/stopping a thread.
               ///< It is guaranteed that Receive() will not be called before Activate(true).
  virtual void Receive(uchar *Data, int Length) = 0;
               ///< This function is called from the cDevice we are attached to, and
               ///< delivers one TS packet from the set of PIDs the cReceiver has requested.
               ///< The data packet must be accepted immediately, and the call must return
               ///< as soon as possible, without any unnecessary delay. Each TS packet
               ///< will be delivered only ONCE, so the cReceiver must make sure that
               ///< it will be able to buffer the data if necessary.
//M7X0 BEGIN AK
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  void setFrameEventsWanted(bool On) { frameEventsWanted = On; }
#endif
#if defined(USE_RECEIVER_RINGBUFFER) || defined(DISABLE_RINGBUFFER_IN_RECEIVER)
  virtual void Receive(uchar *Data, int Length, const sTsDataHeader *const Header) { Receive(Data, Length); }
#endif
//M7X0 END AK
public:
  cReceiver(int Ca, int Priority, int Pid, const int *Pids1 = NULL, const int *Pids2 = NULL, const int *Pids3 = NULL);
               ///< Creates a new receiver that requires conditional access Ca and has
               ///< the given Priority. Pid is a single PID (typically the video PID), while
               ///< Pids1...Pids3 are pointers to zero terminated lists of PIDs.
               ///< If any of these PIDs are 0, they will be silently ignored.
               ///< The total number of non-zero PIDs must not exceed MAXRECEIVEPIDS.
               ///< Priority may be any value in the range -99..99. Negative values indicate
               ///< that this cReceiver may be detached at any time (without blocking the
               ///< cDevice it is attached to).
  virtual ~cReceiver();
  };

#endif //__RECEIVER_H
