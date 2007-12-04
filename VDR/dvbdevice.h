/*
 * dvbdevice.h: The DVB device interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __DVBDEVICE_H
#define __DVBDEVICE_H
//M7X0 BEGIN AK
#include "m7x0_dvb/frontend.h"
//M7X0 END AK
#include "device.h"
#include "dvbspu.h"
//M7X0 BEGIN AK
#include "libsi/util.h"
// M7x0 haven't got this API-Version
// It's ugly mixed up
/*#if DVB_API_VERSION != 3
#error VDR requires Linux DVB driver API version 3!
#endif*/

#define MAXDVBDEVICES  2

//M7X0 END AK

class cDvbTuner;
//M7X0 BEGIN AK
class c7x0TSBuffer;
class c7x0Replayer;
class c7x0TsReplayer;
//M7X0 END AK
/// The cDvbDevice implements a DVB device which can be accessed through the Linux DVB driver API.

class cDvbDevice : public cDevice {

private:
  static bool Probe(const char *FileName);
         ///< Probes for existing DVB devices.
public:
  static bool Initialize(void);
         ///< Initializes the DVB devices.
         ///< Must be called before accessing any DVB functions.
         ///< \return True if any devices are available.

private:
  fe_type_t frontendType;
  int fd_osd, fd_audio, fd_video, fd_video_v4l, fd_dvr, fd_stc;
protected:
  virtual void MakePrimaryDevice(bool On);
public:
  cDvbDevice(int n);
  virtual ~cDvbDevice();
  virtual bool Ready(void);
  virtual int ProvidesCa(const cChannel *Channel) const;
  virtual bool HasDecoder(void) const;
  virtual void CheckStreamAspect();
  virtual void SetTvSettings(bool);
  virtual void SetTvMode(bool);
  virtual void SetVCRMode(bool);
  
// SPU facilities

private:
  cDvbSpuDecoder *spuDecoder;
public:
  virtual cSpuDecoder *GetSpuDecoder(void);

// Channel facilities

private:
  cDvbTuner *dvbTuner;
  void TurnOffLiveMode(bool LiveView);
public:
  virtual bool ProvidesSource(int Source) const;
  virtual bool ProvidesTransponder(const cChannel *Channel) const;
//M7X0 BEGIN AK
  virtual bool ProvidesChannel(const cChannel *Channel, int Priority = -1, bool *NeedsDetachReceivers = NULL, bool forTransferer = false) const;
//M7X0 END AK
  virtual bool IsTunedToTransponder(const cChannel *Channel);
protected:
  virtual bool SetChannelDevice(const cChannel *Channel, bool LiveView);
public:
  virtual bool HasLock(int TimeoutMs = 0);


// PID handle facilities

private:
  bool SetAudioBypass(bool On);
//M7X0 BEGIN AK
  virtual void StartLiveView(bool On, bool AudioOnly);
//M7X0 END AK
protected:
  virtual bool SetPid(cPidHandle *Handle, int Type, bool On);

// Section filter facilities

protected:
  virtual int OpenFilter(u_short Pid, u_char Tid, u_char Mask);

// Image Grab facilities

private:
  static int devVideoOffset;
  int devVideoIndex;
public:
//M7X0 BEGIN AK
#ifdef WITH_LIBJPEG
  virtual uchar *GrabImage(int &Size, bool Jpeg = true, int Quality = -1, int SizeX = -1, int SizeY = -1);
#endif
//M7X0 END AK


// Video format facilities
// m7x0 auto aspect

public:
  virtual void SetVideoDisplayFormat(eVideoDisplayFormat VideoDisplayFormat);
  virtual void SetVideoFormat(eVideoFormat VideoFormat);
  virtual eVideoSystem GetVideoSystem(void);
// Track facilities

protected:
//M7X0 BEGIN AK
  virtual void SetAudioTrackDevice(eTrackType Type, const tTrackId *TrackId = NULL);
//M7X0 END AK
// Audio facilities

private:
//M7X0 BEGIN AK
	int audioChannel;
//M7X0 END AK
  bool digitalAudio;
  static int setTransferModeForDolbyDigital;
protected:
  virtual int GetAudioChannelDevice(void);
  virtual void SetAudioChannelDevice(int AudioChannel);
  virtual void SetVolumeDevice(int Volume);
  virtual void SetDigitalAudioDevice(bool On);
public:
  static void SetTransferModeForDolbyDigital(int Mode);
         ///< Controls how the DVB device handles Transfer Mode when replaying
         ///< Dolby Digital audio.
         ///< 0 = don't set "audio bypass" in driver/firmware, don't force Transfer Mode
         ///< 1 = set "audio bypass" in driver/firmware, force Transfer Mode
         ///< 2 = don't set "audio bypass" in driver/firmware, force Transfer Mode
//M7X0 BEGIN AK
         ///< 3 = set "audio bypass" in driver/firmware, don't force Transfer Mode (default)
// Player facilities

private:
  friend class c7x0Replayer;
  c7x0Replayer *replayer;
  friend class c7x0TsReplayer;
  c7x0TsReplayer *tsreplayer;
//M7X0 END AK
protected:
  ePlayMode playMode;
  virtual bool CanReplay(void) const;
  virtual bool SetPlayMode(ePlayMode PlayMode);
  virtual int PlayVideo(const uchar *Data, int Length);
  virtual int PlayAudio(const uchar *Data, int Length, uchar Id);
public:
  virtual int64_t GetSTC(void);
//M7X0 BEGIN AK
  virtual int PlayPes(const uchar *Data, int Length, bool VideoOnly);
  virtual int PlayTs(const uchar *Data, int Length);
  virtual void SetTsReplayPids(int pmtPid, int videoPid);
  virtual int GetTsReplayVideoPid(void);
  virtual void TrickSpeed(int Speed, bool UseFastForward);
//M7X0 END AK
  virtual void Clear(void);
  virtual void Play(void);
  virtual void Freeze(void);
  virtual void Mute(void);
  virtual void StillPicture(const uchar *Data, int Length);
  virtual bool Poll(cPoller &Poller, int TimeoutMs = 0);
  virtual bool Flush(int TimeoutMs = 0);

// Receiver facilities

private:
//M7X0 BEGIN AK
  c7x0TSBuffer *tsBuffer;
//M7X0 END AK
protected:
  virtual bool OpenDvr(void);
  virtual void CloseDvr(void);
//M7X0 BEGIN AK
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  virtual bool GetTSPackets(uchar *&Data, int &Length, int &Pid, eTsVideoFrame &videoFrame);
#else
  virtual bool GetTSPackets(uchar *&Data, int &Length, int &Pid);
#endif
//M7X0 END AK
  };
#endif //__DVBDEVICE_H
