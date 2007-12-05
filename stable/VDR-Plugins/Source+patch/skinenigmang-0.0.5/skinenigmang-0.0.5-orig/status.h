/*
 * status.h: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINENIGMA_STATUS_H
#define __SKINENIGMA_STATUS_H

#include "common.h"

#include <vdr/status.h>

extern const char *ReplayNames[7];

struct tTimer : public cListObject {
  time_t startTime;
  time_t stopTime;
  int startDay;
  int channelNumber;
  std::string channelName;
  int priority;
  bool isRecording;
  std::string title;

  tTimer(cTimer *timer): startTime(timer->StartTime()),
                         stopTime(timer->StopTime()),
                         startDay(timer->GetMDay(startTime)),
                         channelNumber(timer->Channel()->Number()),
                         channelName(timer->Channel()->Name()),
                         priority(timer->Priority()),
                         isRecording(timer->Recording()),
                         title(timer->File())
  {}

    virtual int Compare(const cListObject & listObj) const {
      tTimer *e = (tTimer *) & listObj;
      int r = startTime - e->startTime;
      if (r == 0)
        r = e->priority - priority;
      return r;
    }
  };

class cEnigmaStatus : public cStatus {
public:
  enum eReplayMode {
    replayNone,
    replayNormal,
    replayMP3,
    replayMPlayer,
    replayDVD,
    replayVCD,
    replayImage,
  };

  void UpdateActiveTimers(void);
  cList<tTimer> mTimers;

private:
  eReplayMode mReplayMode;
  bool mReplayIsLoop;
  bool mReplayIsShuffle;

protected:
  virtual void Replaying(const cControl * Control, const char *Name,
                         const char *FileName, bool On);
  virtual void Recording(const cDevice * Device, const char *Name,
                         const char *FileName, bool On);
  virtual void OsdClear(void);
  virtual void OsdCurrentItem(const char *Text);
  virtual void OsdItem(const char *Text, int Index);

public:
  cEnigmaStatus(void);

  eReplayMode ReplayMode(void) const {
    return mReplayMode;
  }
  const char* ReplayModeName(void) const {
    return ReplayNames[mReplayMode];
  }
};

extern cEnigmaStatus EnigmaStatus;

#endif // __SKINENIGMA_STATUS_H
// vim:et:sw=2:ts=2:
