/*
 * receiver.h: The basic receiver interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
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
protected:
  cDevice *device;
private:
  int ca;
  tChannelID channelID;
protected:
  int priority;
private:
  int pids[MAXRECEIVEPIDS];
  int numPids;
  bool WantsPid(int Pid);
//M7X0 BEGIN AK
  bool activated;
  int lastPid;
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  bool frameEventsWanted;
  bool WantsFrameEvents(void) { return frameEventsWanted;}
#endif
//M7X0 END AK
protected:
  void Detach(void);
  virtual void Activate(bool On) {activated = On;}
  int GetLastPid(void) {return lastPid;}
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
  cReceiver(const cChannel *Channel = NULL, int Priority = MINPRIORITY);
               ///< Creates a new receiver for the given Channel with the given Priority.
               ///< If Channel is not NULL, its pids are set by a call to SetPids().
               ///< Otherwise pids can be added to the receiver by separate calls to the AddPid[s]
               ///< functions.
               ///< The total number of PIDs added to a receiver must not exceed MAXRECEIVEPIDS.
               ///< Priority may be any value in the range MINPRIORITY...MAXPRIORITY. Negative values indicate
               ///< that this cReceiver may be detached at any time in favor of a timer recording
               ///< or live viewing (without blocking the cDevice it is attached to).
  cReceiver(int Ca, int Priority, int Pid, const int *Pids1 = NULL, const int *Pids2 = NULL, const int *Pids3 = NULL, int Pid2 = 0, int Pid3 = 0);
               ///< Creates a new receiver that requires conditional access Ca and has
               ///< the given Priority. Pid is a single PID (typically the video PID), while
               ///< Pids1...Pids3 are pointers to zero terminated lists of PIDs.
               ///< If any of these PIDs are 0, they will be silently ignored.
               ///< The total number of non-zero PIDs must not exceed MAXRECEIVEPIDS.
               ///< Priority may be any value in the range -99..99. Negative values indicate
               ///< that this cReceiver may be detached at any time (without blocking the
               ///< cDevice it is attached to).
  virtual ~cReceiver();
  virtual bool Activated(void) const { return activated; }
  bool AddPid(int Pid);
               ///< Adds the given Pid to the list of PIDs of this receiver.
  bool AddPids(const int *Pids);
               ///< Adds the given zero terminated list of Pids to the list of PIDs of this
               ///< receiver.
  bool AddPids(int Pid1, int Pid2, int Pid3 = 0, int Pid4 = 0, int Pid5 = 0, int Pid6 = 0, int Pid7 = 0, int Pid8 = 0, int Pid9 = 0);
               ///< Adds the given Pids to the list of PIDs of this receiver.
  bool SetPids(const cChannel *Channel);
               ///< Sets the PIDs of this receiver to those of the given Channel,
               ///< replacing any previously stored PIDs. If Channel is NULL, all
               ///< PIDs will be cleared. Parameters in the Setup may control whether
               ///< certain types of PIDs (like Dolby Digital, for instance) are
               ///< actually set. The Channel's ID is stored and can later be retrieved
               ///< through ChannelID(). The ChannelID is necessary to allow the device
               ///< that will be used for this receiver to detect and store whether the
               ///< channel can be decrypted in case this is an encrypted channel.
  bool IsAttached(void) { return device != NULL; }
               ///< Returns true if this receiver is (still) attached to a device.
               ///< A receiver may be automatically detached from its device in
               ///< case the device is needed otherwise, so code that uses a cReceiver
               ///< should repeatedly check whether it is still attached, and if
               ///< it isn't, delete it (or take any other appropriate measures).
  };

#endif //__RECEIVER_H
