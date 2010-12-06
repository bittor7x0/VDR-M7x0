/* 
 * status.c: 'EnigmaNG' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "common.h"
#include "status.h"

#include <vdr/timers.h>
#include <vdr/plugin.h>
#include <vdr/menu.h>

const char *ReplayNames[7] =
  { "", "recordings", "mp3", "mplayer", "dvd", "vcd", "image" };

cEnigmaStatus EnigmaStatus;

cEnigmaStatus::cEnigmaStatus(void) : mReplayMode(replayNone),
                                     mReplayIsLoop(false),
                                     mReplayIsShuffle(false)
{}

void cEnigmaStatus::Replaying(const cControl * /*Control */ , const char *Name,
                              const char *FileName, bool On)
{
  debug("cEnigmaStatus::Replaying(%s)", Name);

  if (Name != NULL) {
    mReplayMode = replayMPlayer;
    if (strlen(Name) > 6 && Name[0] == '[' && Name[3] == ']' && Name[5] == '(') {
      int i;
      for (i = 6; Name[i]; ++i) {
        if (Name[i] == ' ' && Name[i - 1] == ')')
          break;
      }
      if (Name[i]) {            // replaying mp3
        mReplayMode = replayMP3;
        mReplayIsLoop = Name[1] == 'L';
        mReplayIsShuffle = Name[2] == 'S';
      }
    } else if (FileName ? Recordings.GetByName(FileName) : NULL) {
      mReplayMode = replayNormal;
    } else if (strcmp(Name, "DVD") == 0)
      mReplayMode = replayDVD;
    else if (strcmp(Name, "VCD") == 0)
      mReplayMode = replayVCD;
    else if (access(Name, F_OK) == 0)
      mReplayMode = replayMPlayer;
    else if (strncmp(Name, "[image]", 7) == 0)
      mReplayMode = replayImage;
    else if (strlen(Name) > 7) {
      int i, n;
      for (i = 0, n = 0; Name[i]; ++i) {
        if (Name[i] == ' ' && Name[i - 1] == ',' && ++n == 4)
          break;
      }
      if (Name[i]) {            // replaying DVD
        mReplayMode = replayDVD;
      }
    }
  } else {
    mReplayMode = replayNone;
    mReplayIsLoop = false;
    mReplayIsShuffle = false;
  }
}

void cEnigmaStatus::Recording(const cDevice * Device, const char *Name, const char *FileName, bool On)
{
}

void cEnigmaStatus::OsdClear(void)
{
}

void cEnigmaStatus::OsdCurrentItem(const char *Text)
{
}

void cEnigmaStatus::OsdItem(const char *Text, int Index)
{
}

void cEnigmaStatus::UpdateActiveTimers(void)
{
  mTimers.Clear();
  Timers.IncBeingEdited();

  for (cTimer * tim = Timers.First(); tim; tim = Timers.Next(tim)) {
    if (tim->HasFlags(tfActive)) {
      int i = 0;
      cTimer dummy;
      dummy = *tim;

      do {
        mTimers.Add(new tTimer(&dummy));

        if (!dummy.IsSingleEvent())     // add 4 additional rep. timer
        {
          int j = 0;
          do {
            j++;                // just to avoid a endless loop
            dummy.Skip();
            dummy.Matches();    // Refresh start- and end-time
          } while (!dummy.DayMatches(dummy.StartTime()) && (j < 7));
        }

        i++;
      } while (!dummy.IsSingleEvent() && i < 5);
    }
  }

  Timers.DecBeingEdited();
  mTimers.Sort();
}
// vim:et:sw=2:ts=2:
