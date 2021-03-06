/*
 * device.h: The basic device interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef __DEVICE_H
#define __DEVICE_H

#include "channels.h"
#include "ci.h"
#include "eit.h"
#include "filter.h"
#include "nit.h"
#include "pat.h"
#include "remux.h"
#include "ringbuffer.h"
#include "sdt.h"
#include "sections.h"
#include "spu.h"
#include "thread.h"
#include "tools.h"

//M7X0 BEGIN AK
// More then 2 receivers is definetly too much.
#define MAXDEVICES         3 // the maximum number of devices in the system
#define MAXPIDHANDLES      64 // the maximum number of different PIDs per device
#define MAXRECEIVERS       2 // the maximum number of receivers per device
//M7X0 END AK
#define MAXVOLUME         255
#define VOLUMEDELTA         5 // used to increase/decrease the volume

#define TS_SIZE          188
#define TS_SYNC_BYTE     0x47
#define PID_MASK_HI      0x1F

#define TSPLAY_PATCH_VERSION 2 // Presence detection of TSPlay-Patch

extern bool scanning_on_receiving_device;

enum eSetChannelResult { scrOk, scrNotAvailable, scrNoTransfer, scrFailed };

enum ePlayMode { pmNone,           // audio/video from decoder
                 pmAudioVideo,     // audio/video from player
                 pmAudioOnly,      // audio only from player, video from decoder
                 pmAudioOnlyBlack, // audio only from player, no video (black screen)
                 pmVideoOnly,      // video only from player, audio from decoder
                 pmTsAudioVideo,     // audio/video from player
                 pmTsAudioOnly,      // audio only from player, video from decoder
                 pmTsAudioOnlyBlack, // audio only from player, no video (black screen)
                 pmTsVideoOnly,      // video only from player, audio from decoder
                 pmExtern_THIS_SHOULD_BE_AVOIDED,
                 // external player (e.g. MPlayer), release the device
                 // WARNING: USE THIS MODE ONLY AS A LAST RESORT, IF YOU
                 // ABSOLUTELY, POSITIVELY CAN'T IMPLEMENT YOUR PLAYER
                 // THE WAY IT IS SUPPOSED TO WORK. FORCING THE DEVICE
                 // TO RELEASE ITS FILES HANDLES (OR WHATEVER RESOURCES
                 // IT MAY USE) TO ALLOW AN EXTERNAL PLAYER TO ACCESS
                 // THEM MEANS THAT SUCH A PLAYER WILL NEED TO HAVE
                 // DETAILED KNOWLEDGE ABOUT THE INTERNALS OF THE DEVICE
                 // IN USE. AS A CONSEQUENCE, YOUR PLAYER MAY NOT WORK
                 // IF A PARTICULAR VDR INSTALLATION USES A DEVICE NOT
                 // KNOWN TO YOUR PLAYER.
//M7X0 BEGIN AK
                 pmTransferer,
                 pmTransfererAudioOnly
//M7X0 END AK
               };

enum eVideoSystem { vsPAL,
                    vsNTSC
                  };

enum eVideoDisplayFormat { vdfPanAndScan,
                           vdfLetterBox
                         };
//m7x0 auto aspect			 
enum eVideoFormat { vf4_3,
		    vf16_9,
		    vfauto
		   };

enum eTrackType { ttNone,
                  ttAudio,
                  ttAudioFirst = ttAudio,
                  ttAudioLast  = ttAudioFirst + 31, // MAXAPIDS - 1
                  ttDolby,
                  ttDolbyFirst = ttDolby,
                  ttDolbyLast  = ttDolbyFirst + 15, // MAXDPIDS - 1
                  /* future...
                  ttSubtitle,
                  ttSubtitleFirst = ttSubtitle,
                  ttSubtitleLast  = ttSubtitleFirst + 7, // MAXSPIDS - 1
                  */
                  ttMaxTrackTypes
                };

#define IS_AUDIO_TRACK(t) (ttAudioFirst <= (t) && (t) <= ttAudioLast)
#define IS_DOLBY_TRACK(t) (ttDolbyFirst <= (t) && (t) <= ttDolbyLast)

struct tTrackId {
  uint16_t id;                  // The PES packet id or the PID.
  char language[MAXLANGCODE2];  // something like either "eng" or "deu+eng"
  char description[32];         // something like "Dolby Digital 5.1"
  };

class cPlayer;
class cReceiver;
class cPesAssembler;
class cTransfer;

/// The cDevice class is the base from which actual devices can be derived.

class cDevice : public cThread {
private:
  static int numDevices;
  static int useDevice;
  static cDevice *device[MAXDEVICES];
  static cDevice *primaryDevice;
public:
  static int NumDevices(void) { return numDevices; }
         ///< Returns the total number of devices.
  static bool WaitForAllDevicesReady(int Timeout = 0);
         ///< Waits until all devices have become ready, or the given Timeout
         ///< (seconds) has expired. While waiting, the Ready() function of each
         ///< device is called in turn, until they all return true.
         ///< Returns true if all devices have become ready within the given
         ///< timeout.
  static void SetUseDevice(int n);
         ///< Sets the 'useDevice' flag of the given device.
         ///< If this function is not called before initializing, all devices
         ///< will be used.
  static bool UseDevice(int n) { return useDevice == 0 || (useDevice & (1 << n)) != 0; }
         ///< Tells whether the device with the given card index shall be used in
         ///< this instance of VDR.
  static bool SetPrimaryDevice(int n);
         ///< Sets the primary device to 'n'.
         ///< n must be in the range 1...numDevices.
         ///< Returns true if this was possible.
  static cDevice *PrimaryDevice(void) { return primaryDevice; }
         ///< Returns the primary device.
  static cDevice *ActualDevice(void);
         ///< Returns the actual receiving device in case of Transfer Mode, or the
         ///< primary device otherwise.
  static cDevice *GetDevice(int Index);
         ///< Gets the device with the given Index.
         ///< Index must be in the range 0..numDevices-1.
         ///< Returns a pointer to the device, or NULL if the Index was invalid.
//M7X0 BEGIN AK
  static cDevice *GetDevice(const cChannel *Channel, int Priority = -1, bool *NeedsDetachReceivers = NULL, bool forTransferer = false);
//M7X0 END AK
         ///< Returns a device that is able to receive the given Channel at the
         ///< given Priority, with the least impact on active recordings and
         ///< live viewing.
         ///< See ProvidesChannel() for more information on how
         ///< priorities are handled, and the meaning of NeedsDetachReceivers.
  static void Shutdown(void);
         ///< Closes down all devices.
         ///< Must be called at the end of the program.
private:
  static int nextCardIndex;
  int cardIndex;
protected:
  cDevice(void);
  virtual ~cDevice();
  virtual bool Ready(void);
         ///< Returns true if this device is ready. Devices with conditional
         ///< access hardware may need some time until they are up and running.
         ///< This function is called in a loop at startup until all devices
         ///< are ready (see WaitForAllDevicesReady()).
  static int NextCardIndex(int n = 0);
         ///< Calculates the next card index.
         ///< Each device in a given machine must have a unique card index, which
         ///< will be used to identify the device for assigning Ca parameters and
         ///< deciding whether to actually use that device in this particular
         ///< instance of VDR. Every time a new cDevice is created, it will be
         ///< given the current nextCardIndex, and then nextCardIndex will be
         ///< automatically incremented by 1. A derived class can determine whether
         ///< a given device shall be used by checking UseDevice(NextCardIndex()).
         ///< If a device is skipped, or if there are possible device indexes left
         ///< after a derived class has set up all its devices, NextCardIndex(n)
         ///< must be called, where n is the number of card indexes to skip.
  virtual void MakePrimaryDevice(bool On);
         ///< Informs a device that it will be the primary device. If there is
         ///< anything the device needs to set up when it becomes the primary
         ///< device (On = true) or to shut down when it no longer is the primary
         ///< device (On = false), it should do so in this function.
public:
  bool IsPrimaryDevice(void) const { return this == primaryDevice; }
  int CardIndex(void) const { return cardIndex; }
         ///< Returns the card index of this device (0 ... MAXDEVICES - 1).
  int DeviceNumber(void) const;
         ///< Returns the number of this device (0 ... numDevices).
  virtual int ProvidesCa(const cChannel *Channel) const;
         ///< Checks whether this device provides the conditional access
         ///< facilities to decrypt the given Channel.
         ///< Returns 0 if the Channel can't be decrypted, 1 if this is a
         ///< Free To Air channel or only exactly this device can decrypt it,
         ///< and > 1 if this device can decrypt the Channel.
         ///< If the result is greater than 1 and the device has more than one
         ///< CAM, the value will be increased by the number of CAMs, which
         ///< allows to select the device with the smallest number of CAMs
         ///< in order to preserve resources for other recordings.
  virtual bool HasDecoder(void) const;
         ///< Tells whether this device has an MPEG decoder.
  virtual void CheckStreamAspect(bool);
  virtual void SetTvSettings(bool);
  virtual void SetTvMode(bool);
  virtual void SetVCRMode(bool);
// SPU facilities

public:
  virtual cSpuDecoder *GetSpuDecoder(void);
         ///< Returns a pointer to the device's SPU decoder (or NULL, if this
         ///< device doesn't have an SPU decoder).

// Channel facilities

protected:
  static int currentChannel;
public:
  virtual bool ProvidesSource(int Source) const;
         ///< Returns true if this device can provide the given source.
  virtual bool ProvidesTransponder(const cChannel *Channel) const;
         ///< XXX -> PLUGINS.html!
  virtual bool ProvidesTransponderExclusively(const cChannel *Channel) const;
         ///< Returns true if this is the only device that is able to provide
         ///< the given channel's transponder.
//M7X0 BEGIN AK
  virtual bool ProvidesChannel(const cChannel *Channel, int Priority = -1, bool *NeedsDetachReceivers = NULL, bool forTransferer = false) const;
//M7X0 END AK
         ///< Returns true if this device can provide the given channel.
         ///< In case the device has cReceivers attached to it or it is the primary
         ///< device, Priority is used to decide whether the caller's request can
         ///< be honored.
         ///< The special Priority value -1 will tell the caller whether this device
         ///< is principally able to provide the given Channel, regardless of any
         ///< attached cReceivers.
         ///< If NeedsDetachReceivers is given, the resulting value in it will tell the
         ///< caller whether or not it will have to detach any currently attached
         ///< receivers from this device before calling SwitchChannel. Note
         ///< that the return value in NeedsDetachReceivers is only meaningful if the
         ///< function itself actually returns true.
         ///< The default implementation always returns false, so a derived cDevice
         ///< class that can provide channels must implement this function.
  virtual bool IsTunedToTransponder(const cChannel *Channel);
         ///< Returns true if this device is currently tuned to the given Channel's
         ///< transponder.
  virtual bool MaySwitchTransponder(void);
         ///< Returns true if it is ok to switch the transponder on this device,
         ///< without disturbing any other activities.
  bool SwitchChannel(const cChannel *Channel, bool LiveView);
         ///< Switches the device to the given Channel, initiating transfer mode
         ///< if necessary.
  static bool SwitchChannel(int Direction);
         ///< Switches the primary device to the next available channel in the given
         ///< Direction (only the sign of Direction is evaluated, positive values
         ///< switch to higher channel numbers).
private:
  eSetChannelResult SetChannel(const cChannel *Channel, bool LiveView);
         ///< Sets the device to the given channel (general setup).
protected:
  virtual bool SetChannelDevice(const cChannel *Channel, bool LiveView);
         ///< Sets the device to the given channel (actual physical setup).
public:
  static int CurrentChannel(void) { return primaryDevice ? currentChannel : 0; }
         ///< Returns the number of the current channel on the primary device.
#define DEPRECATED_SETCURRENTCHANNEL
#ifdef DEPRECATED_SETCURRENTCHANNEL
  static void SetCurrentChannel(const cChannel *Channel) { currentChannel = Channel ? Channel->Number() : 0; }
#endif
  static void SetCurrentChannel(int ChannelNumber) { currentChannel = ChannelNumber; }
         ///< Sets the number of the current channel on the primary device, without
         ///< actually switching to it. This can be used to correct the current
         ///< channel number while replaying.
  void ForceTransferMode(void);
         ///< Forces the device into transfermode for the current channel.
  virtual bool HasLock(int TimeoutMs = 0) const;//XXX PLUGINS.html
         ///< Returns true if the device has a lock on the requested transponder.
         ///< Default is true, a specific device implementation may return false
         ///< to indicate that it is not ready yet.
         ///< If TimeoutMs is not zero, waits for the given number of milliseconds
         ///< before returning false.
  virtual bool HasProgramme(void) const;
         ///< Returns true if the device is currently showing any programme to
         ///< the user, either through replaying or live.

// PID handle facilities
//M7X0 BEGIN AK
friend class cTransfer;
private:
  virtual void Action(void);
  virtual void StartLiveView(bool On, bool AudioOnly) {}
protected:
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  enum ePidType { ptAudio, ptVideo, ptPcr, ptTeletext, ptDolby, ptRecVideo, ptOther };
#else
  enum ePidType { ptAudio, ptVideo, ptPcr, ptTeletext, ptDolby, ptOther };
#endif
//M7X0 END AK
  class cPidHandle {
  public:
    int pid;
    int handle;
    int used;
    cPidHandle(void) { pid = used = 0; handle = -1; }
    };
  cPidHandle pidHandles[MAXPIDHANDLES];
  bool HasPid(int Pid) const;
         ///< Returns true if this device is currently receiving the given PID.
  bool AddPid(int Pid, ePidType PidType = ptOther);
         ///< Adds a PID to the set of PIDs this device shall receive.
  void DelPid(int Pid, ePidType PidType = ptOther);
         ///< Deletes a PID from the set of PIDs this device shall receive.
  virtual bool SetPid(cPidHandle *Handle, int Type, bool On);
         ///< Does the actual PID setting on this device.
         ///< On indicates whether the PID shall be added or deleted.
         ///< Handle->handle can be used by the device to store information it
         ///< needs to receive this PID (for instance a file handle).
         ///< Handle->used indicates how many receivers are using this PID.
         ///< Type indicates some special types of PIDs, which the device may
         ///< need to set in a specific way.

// Section filter facilities

private:
  cSectionHandler *sectionHandler;
  cEitFilter *eitFilter;
  cPatFilter *patFilter;
  cSdtFilter *sdtFilter;
  cNitFilter *nitFilter;
protected:
  void StartSectionHandler(void);
       ///< A derived device that provides section data must call
       ///< this function (typically in its constructor) to actually set
       ///< up the section handler.
  void StopSectionHandler(void);
       ///< A device that has called StartSectionHandler() must call this
       ///< function (typically in its destructor) to stop the section
       ///< handler.
public:
  virtual int OpenFilter(u_short Pid, u_char Tid, u_char Mask);
       ///< Opens a file handle for the given filter data.
       ///< A derived device that provides section data must
       ///< implement this function.
  void AttachFilter(cFilter *Filter);
       ///< Attaches the given filter to this device.
  void Detach(cFilter *Filter);
       ///< Detaches the given filter from this device.

// Common Interface facilities:

protected:
  cCiHandler *ciHandler;
public:
  cCiHandler *CiHandler(void) { return ciHandler; }

// Image Grab facilities
#ifdef WITH_LIBJPEG
public:
  virtual uchar *GrabImage(int &Size, bool Jpeg = true, int Quality = -1, int SizeX = -1, int SizeY = -1);
         ///< Grabs the currently visible screen image.
         ///< Size is the size of the returned data block.
         ///< If Jpeg is true it will write a JPEG file. Otherwise a PNM file will be written.
         ///< Quality is the compression factor for JPEG. 1 will create a very blocky
         ///<        and small image, 70..80 will yield reasonable quality images while keeping the
         ///<        image file size around 50 KB for a full frame. The default will create a big
         ///<        but very high quality image.
         ///< SizeX is the number of horizontal pixels in the frame (default is the current screen width).
         ///< SizeY is the number of vertical pixels in the frame (default is the current screen height).
         ///< Returns a pointer to the grabbed image data, or NULL in case of an error.
         ///< The caller takes ownership of the returned memory and must free() it once it isn't needed any more.
  bool GrabImageFile(const char *FileName, bool Jpeg = true, int Quality = -1, int SizeX = -1, int SizeY = -1);
         ///< Calls GrabImage() and stores the resulting image in a file with the given name.
         ///< Returns true if all went well.
         ///< The caller is responsible for making sure that the given file name
         ///< doesn't lead to overwriting any important other file.
#endif

// Video format facilities

public:
  virtual void SetVideoDisplayFormat(eVideoDisplayFormat VideoDisplayFormat);
         ///< Sets the video display format to the given one (only useful
         ///< if this device has an MPEG decoder).
         ///< A derived class must first call the base class function!
  virtual void SetVideoFormat(eVideoFormat VideoFormat);
         ///< Sets the output video format to either 16:9 or 4:3 (only useful
         ///< if this device has an MPEG decoder).
	 ///< m7x0 auto aspect
  virtual eVideoSystem GetVideoSystem(void);
         ///< Returns the video system of the currently displayed material
         ///< (default is PAL).

// Track facilities

private:
  tTrackId availableTracks[ttMaxTrackTypes];
  eTrackType currentAudioTrack;

  int currentAudioTrackMissingCount;
  bool pre_1_3_19_PrivateStream;
protected:
//M7X0 BEGIN AK
// Was private before
  cMutex mutexCurrentAudioTrack;
  virtual void SetAudioTrackDevice(eTrackType Type, const tTrackId *TrackId = NULL);
       ///< Sets the current audio track to the given value.
//M7X0 END AK
public:
  void ClrAvailableTracks(bool DescriptionsOnly = false, bool IdsOnly = false);
       ///< Clears the list of currently available tracks. If DescriptionsOnly
       ///< is true, only the track descriptions will be cleared. With IdsOnly
       ///< set to true only the ids will be cleared. IdsOnly is only taken
       ///< into account if DescriptionsOnly is false.
  bool SetAvailableTrack(eTrackType Type, int Index, uint16_t Id, const char *Language = NULL, const char *Description = NULL);
       ///< Sets the track of the given Type and Index to the given values.
       ///< Type must be one of the basic eTrackType values, like ttAudio or ttDolby.
       ///< Index tells which track of the given basic type is meant.
       ///< If Id is 0 any existing id will be left untouched and only the
       ///< given Language and Description will be set.
       ///< Returns true if the track was set correctly, false otherwise.
  const tTrackId *GetTrack(eTrackType Type);
       ///< Returns a pointer to the given track id, or NULL if Type is not
       ///< less than ttMaxTrackTypes.
  int NumAudioTracks(void) const;
       ///< Returns the number of audio tracks that are currently available.
       ///< This is just for information, to quickly find out whether there
       ///< is more than one audio track.
  eTrackType GetCurrentAudioTrack(void) { return currentAudioTrack; }
  bool SetCurrentAudioTrack(eTrackType Type);
       ///< Sets the current audio track to the given Type.
       ///< Returns true if Type is a valid audio track, false otherwise.
  void EnsureAudioTrack(bool Force = false);
       ///< Makes sure an audio track is selected that is actually available.
       ///< If Force is true, the language and Dolby Digital settings will
       ///< be verified even if the current audio track is available.

// Audio facilities

private:
  bool mute;
  int volume;
protected:
  virtual int GetAudioChannelDevice(void);
       ///< Gets the current audio channel, which is stereo (0), mono left (1) or
       ///< mono right (2).
  virtual void SetAudioChannelDevice(int AudioChannel);
       ///< Sets the audio channel to stereo (0), mono left (1) or mono right (2).
  virtual void SetVolumeDevice(int Volume);
       ///< Sets the audio volume on this device (Volume = 0...255).
  virtual void SetDigitalAudioDevice(bool On);
       ///< Tells the actual device that digital audio output shall be switched
       ///< on or off.
public:
  bool IsMute(void) const { return mute; }
  bool ToggleMute(void);
       ///< Turns the volume off or on and returns the new mute state.
  int GetAudioChannel(void);
       ///< Gets the current audio channel, which is stereo (0), mono left (1) or
       ///< mono right (2).
  void SetAudioChannel(int AudioChannel);
       ///< Sets the audio channel to stereo (0), mono left (1) or mono right (2).
       ///< Any other values will be silently ignored.
  void SetVolume(int Volume, bool Absolute = false);
       ///< Sets the volume to the given value, either absolutely or relative to
       ///< the current volume.
  static int CurrentVolume(void) { return primaryDevice ? primaryDevice->volume : 0; }//XXX???

// Player facilities

private:
  cPlayer *player;
  cPesAssembler *pesAssembler;
  cPatPmtParser patPmtParser;
  cTsToPes tsToPesVideo;
  cTsToPes tsToPesAudio;
  //cTsToPes tsToPesSubtitle;
#ifdef TS_PLAYER_BACKPORT
  bool isPlayingVideo;
#endif
protected:
  const cPatPmtParser *PatPmtParser(void) const { return &patPmtParser; }
       ///< Returns a pointer to the patPmtParser, so that a derived device
       ///< can use the stream information from it.
  virtual bool CanReplay(void) const;
       ///< Returns true if this device can currently start a replay session.
public:
  virtual bool SetPlayMode(ePlayMode PlayMode);
       ///< Sets the device into the given play mode.
       ///< Returns true if the operation was successful.
protected:
  virtual int PlayVideo(const uchar *Data, int Length);
       ///< Plays the given data block as video.
       ///< Data points to exactly one complete PES packet of the given Length.
       ///< PlayVideo() shall process the packet either as a whole (returning
       ///< Length) or not at all (returning 0 or -1 and setting 'errno' to EAGAIN).
       ///< Returns the number of bytes actually taken from Data, or -1
       ///< in case of an error.
  virtual int PlayAudio(const uchar *Data, int Length, uchar Id);
       ///< Plays the given data block as audio.
       ///< Data points to exactly one complete PES packet of the given Length.
       ///< Id indicates the type of audio data this packet holds.
       ///< PlayAudio() shall process the packet either as a whole (returning
       ///< Length) or not at all (returning 0 or -1 and setting 'errno' to EAGAIN).
       ///< Returns the number of bytes actually taken from Data, or -1
       ///< in case of an error.
  virtual int PlayPesPacket(const uchar *Data, int Length, bool VideoOnly = false);
       ///< Plays the single PES packet in Data with the given Length.
       ///< If VideoOnly is true, only the video will be displayed,
       ///< which is necessary for trick modes like 'fast forward'.
       ///< Data must point to one single, complete PES packet.
#ifdef TS_PLAYER_BACKPORT
  virtual int PlayTsVideo(const uchar *Data, int Length);
       ///< Plays the given data block as video.
       ///< Data points to exactly one complete TS packet of the given Length
       ///< (which is always TS_SIZE).
       ///< PlayTsVideo() shall process the packet either as a whole (returning
       ///< Length) or not at all (returning 0 or -1 and setting 'errno' accordingly).
       ///< The default implementation collects all incoming TS payload belonging
       ///< to one PES packet and calls PlayVideo() with the resulting packet.
  virtual int PlayTsAudio(const uchar *Data, int Length);
       ///< Plays the given data block as audio.
       ///< Data points to exactly one complete TS packet of the given Length
       ///< (which is always TS_SIZE).
       ///< PlayTsAudio() shall process the packet either as a whole (returning
       ///< Length) or not at all (returning 0 or -1 and setting 'errno' accordingly).
       ///< The default implementation collects all incoming TS payload belonging
       ///< to one PES packet and calls PlayAudio() with the resulting packet.
/*
  virtual int PlayTsSubtitle(const uchar *Data, int Length);
       ///< Plays the given data block as a subtitle.
       ///< Data points to exactly one complete TS packet of the given Length
       ///< (which is always TS_SIZE).
       ///< PlayTsSubtitle() shall process the packet either as a whole (returning
       ///< Length) or not at all (returning 0 or -1 and setting 'errno' accordingly).
       ///< The default implementation collects all incoming TS payload belonging
       ///< to one PES packet and displays the resulting subtitle via the OSD.
*/
#endif
public:
  virtual int64_t GetSTC(void);
       ///< Gets the current System Time Counter, which can be used to
       ///< synchronize audio and video. If this device is unable to
       ///< provide the STC, -1 will be returned.
#ifdef TS_PLAYER_BACKPORT
  virtual bool IsPlayingVideo(void) const { return isPlayingVideo; }
       ///< Returns true if the currently attached player has delivered
       ///< any video packets.
#endif
//M7X0 BEGIN AK
  virtual void TrickSpeed(int Speed, bool UseFastForward);
//M7X0 END AK
       ///< Sets the device into a mode where replay is done slower.
       ///< Every single frame shall then be displayed the given number of
       ///< times.
  virtual void Clear(void);
       ///< Clears all video and audio data from the device.
       ///< A derived class must call the base class function to make sure
       ///< all registered cAudio objects are notified.
  virtual void Play(void);
       ///< Sets the device into play mode (after a previous trick
       ///< mode).
  virtual void Freeze(void);
       ///< Puts the device into "freeze frame" mode.
  virtual void Mute(void);
       ///< Turns off audio while replaying.
       ///< A derived class must call the base class function to make sure
       ///< all registered cAudio objects are notified.
  virtual void StillPicture(const uchar *Data, int Length);
       ///< Displays the given I-frame as a still picture.
  virtual bool Poll(cPoller &Poller, int TimeoutMs = 0);
       ///< Returns true if the device itself or any of the file handles in
       ///< Poller is ready for further action.
       ///< If TimeoutMs is not zero, the device will wait up to the given number
       ///< of milliseconds before returning in case it can't accept any data.
  virtual bool Flush(int TimeoutMs = 0);
       ///< Returns true if the device's output buffers are empty, i. e. any
       ///< data which was buffered so far has been processed.
       ///< If TimeoutMs is not zero, the device will wait up to the given
       ///< number of milliseconds before returning in case there is still
       ///< data in the buffers.
  virtual int PlayPes(const uchar *Data, int Length, bool VideoOnly = false);
       ///< Plays all valid PES packets in Data with the given Length.
       ///< If Data is NULL any leftover data from a previous call will be
       ///< discarded. If VideoOnly is true, only the video will be displayed,
       ///< which is necessary for trick modes like 'fast forward'.
       ///< Data should point to a sequence of complete PES packets. If the
       ///< last packet in Data is not complete, it will be copied and combined
       ///< to a complete packet with data from the next call to PlayPes().
       ///< That way any functions called from within PlayPes() will be
       ///< guaranteed to always receive complete PES packets.
#ifndef TS_PLAYER_BACKPORT
  virtual int PlayTs(const uchar *Data, int Length) { return -1; }
       ///< Plays all valid TS packets in Data with the given Length.
       ///< If Data is NULL any leftover data from a previous call will be
       ///< discarded.
       ///< Data should point to a sequence of complete TS packets. If the
       ///< last packet in Data is not complete, it will be copied and combined
       ///< to a complete packet with data from the next call to PlayTs().
       ///< A valid PAT and PMT is expected in stream. If there is more than
       ///< one program in the stream the first
  virtual void SetTsReplayPids(int pmtPid, int videoPid) { }
  virtual int GetTsReplayVideoPid(void) { return 0; }
#else
  virtual int PlayTs(const uchar *Data, int Length, bool VideoOnly = false);
       ///< Plays the given TS packet.
       ///< If VideoOnly is true, only the video will be displayed,
       ///< which is necessary for trick modes like 'fast forward'.
       ///< Data points to a single TS packet, Length is always TS_SIZE (the total
       ///< size of a single TS packet).
       ///< If Data is NULL any leftover data from a previous call will be
       ///< discarded.
       ///< A derived device can reimplement this function to handle the
       ///< TS packets itself. Any packets the derived function can't handle
       ///< must be sent to the base class function. This applies especially
       ///< to the PAT/PMT packets.
       ///< Returns -1 in case of error, otherwise the number of actually
       ///< processed bytes is returned.
       ///< PlayTs() shall process the TS packets either as a whole (returning
       ///< n*TS_SIZE) or not at all, returning 0 or -1 and setting 'errno' accordingly).
#endif
  bool Replaying(void) const;
       ///< Returns true if we are currently replaying.
  bool Transferring(void) const;
       ///< Returns true if we are currently in Transfer Mode.
  void StopReplay(void);
       ///< Stops the current replay session (if any).
  bool AttachPlayer(cPlayer *Player);
       ///< Attaches the given player to this device.
  void Detach(cPlayer *Player);
       ///< Detaches the given player from this device.

// Receiver facilities

private:
  cMutex mutexReceiver;
//M7X0 BEGIN AK
  cReceiver *receiver[MAXRECEIVERS + 1];

  volatile int actionLock;
  volatile int otherLock;
  inline void FasterLockAction(void) __attribute__ ((always_inline));
  inline void FasterUnlockAction(void) __attribute__ ((always_inline));
  inline void FasterLockOther(void) __attribute__ ((always_inline));
  inline void FasterUnlockOther(void) __attribute__ ((always_inline));
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  bool firstFrameEventGotten;
  int ReceiverWantsFrameEvents(int Pid = 0);
#endif
//M7X0 END AK
protected:
  int Priority(void) const;
      ///< Returns the priority of the current receiving session (0..MAXPRIORITY),
      ///< or -1 if no receiver is currently active. The primary device will
      ///< always return at least Setup.PrimaryLimit-1.
  virtual bool OpenDvr(void);
      ///< Opens the DVR of this device and prepares it to deliver a Transport
      ///< Stream for use in a cReceiver.
  virtual void CloseDvr(void);
      ///< Shuts down the DVR.
//M7X0 BEGIN AK
#ifdef USE_HW_VIDEO_FRAME_EVENTS
  virtual bool GetTSPackets(uchar *&Data, int &Length, int &Pid, eTsVideoFrame &videoFrame);
#else
  virtual bool GetTSPackets(uchar *&Data, int &Length, int &Pid);
#endif
      ///< Gets n TS packets, with the same PID (Pid), from the DVR of this device and
      ///< returns a pointer to it in Data. Only the first Length bytes (n * TS_SIZE)
      ///< Data points to are valid and may be accessed. If there is currently no
      ///< new data available, Data will be set to NULL. The function returns
      ///< false in case of a non recoverable error, otherwise it returns true,
      ///< even if Data is NULL.
//M7X0 END AK
public:
  int  Ca(void) const;
       ///< Returns the ca of the current receiving session(s).
  bool Receiving(bool CheckAny = false) const;
       ///< Returns true if we are currently receiving.
//M7X0 BEGIN AK
  bool FreeReceiverSlot(void) const;
       ///< Returns true if we have a receiver slot left.
//M7X0 END AK
  bool AttachReceiver(cReceiver *Receiver);
       ///< Attaches the given receiver to this device.
  void Detach(cReceiver *Receiver);
       ///< Detaches the given receiver from this device.
  void DetachAll(int Pid);
       ///< Detaches all receivers from this device for this pid.
  void DetachAllReceivers(void);
       ///< Detaches all receivers from this device.
  };

/// Derived cDevice classes that can receive channels will have to provide
/// Transport Stream (TS) packets one at a time. cTSBuffer implements a
/// simple buffer that allows the device to read a larger amount of data
/// from the driver with each call to Read(), thus avoiding the overhead
/// of getting each TS packet separately from the driver. It also makes
/// sure the returned data points to a TS packet and automatically
/// re-synchronizes after broken packets.

class cTSBuffer : public cThread {
private:
  int f;
  int cardIndex;
  bool delivered;
  cRingBufferLinear *ringBuffer;
  virtual void Action(void);
public:
  cTSBuffer(int File, int Size, int CardIndex);
  ~cTSBuffer();
  uchar *Get(void);
  };



#endif //__DEVICE_H
