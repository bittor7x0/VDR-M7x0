/*
 * externalplayer-control.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "externalplayer-control.h"
#include "externalplayer-player.h"

cStatusExternalplayer::cStatusExternalplayer(sPlayerArgs * nConfig) : cStatus() {
  config = nConfig;
}

void cStatusExternalplayer::OsdTitle(const char * title) {
  if (config->blockMenu) {
    cRemote::Put(kMenu);
    isyslog("externalplayer-plugin: menu blocked");
  }
}

// --- cControlExternalplayer ------------------------------------------------

cControlExternalplayer::cControlExternalplayer(sPlayerArgs * nConfig, int fdsPipe[2])
    : cControl(player = new cPlayerExternalplayer(nConfig->playMode, nConfig, fdsPipe[0])) {
  config = nConfig;
  status = new cStatusExternalplayer(config);
  fdWritePipe = fdsPipe[1];
}

cControlExternalplayer::~cControlExternalplayer() {
  isyslog("externalplayer-plugin: shutting down player");
  delete player;
  delete status;
  player = NULL;
}

eOSState cControlExternalplayer::ProcessKey(eKeys key) {
  if (!(((cPlayerExternalplayer *)player)->isActive())) {
    return osEnd;
  }

  if (config->slaveMode) {
    switch (key) {
      case kUp:
        {
          string * key = config->keys->vdrKeyUp;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kDown:
        {
          string * key = config->keys->vdrKeyDown;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kOk:
        {
          string * key = config->keys->vdrKeyOk;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kBack:
        {
          string * key = config->keys->vdrKeyBack;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kLeft:
        {
          string * key = config->keys->vdrKeyLeft;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kRight:
        {
          string * key = config->keys->vdrKeyRight;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kRed:
        {
          string * key = config->keys->vdrKeyRed;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kGreen:
        {
          string * key = config->keys->vdrKeyGreen;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kYellow:
        {
          string * key = config->keys->vdrKeyYellow;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kBlue:
        {
          string * key = config->keys->vdrKeyBlue;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k0:
        {
          string * key = config->keys->vdrKey0;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k1:
        {
          string * key = config->keys->vdrKey1;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k2:
        {
          string * key = config->keys->vdrKey2;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k3:
        {
          string * key = config->keys->vdrKey3;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k4:
        {
          string * key = config->keys->vdrKey4;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k5:
        {
          string * key = config->keys->vdrKey5;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k6:
        {
          string * key = config->keys->vdrKey6;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k7:
        {
          string * key = config->keys->vdrKey7;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k8:
        {
          string * key = config->keys->vdrKey8;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case k9:
        {
          string * key = config->keys->vdrKey9;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kPlay:
        {
          string * key = config->keys->vdrKeyPlay;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kPause:
        {
          string * key = config->keys->vdrKeyPause;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kStop:
        {
          string * key = config->keys->vdrKeyStop;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kRecord:
        {
          string * key = config->keys->vdrKeyRecord;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kFastFwd:
        {
          string * key = config->keys->vdrKeyFastFwd;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kFastRew:
        {
          string * key = config->keys->vdrKeyFaswRew;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
#if VDRVERSNUM >= 10318
      case kAudio:
        {
          string * key = config->keys->vdrKeyAudio;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
#endif
      case kSchedule:
        {
          string * key = config->keys->vdrKeySchedule;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kChannels:
        {
          string * key = config->keys->vdrKeyChannels;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kTimers:
        {
          string * key = config->keys->vdrKeyTimers;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kRecordings:
        {
          string * key = config->keys->vdrKeyRecordings;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kSetup:
        {
          string * key = config->keys->vdrKeySetup;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kCommands:
        {
          string * key = config->keys->vdrKeyCommands;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kUser1:
        {
          string * key = config->keys->vdrKeyUser1;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kUser2:
        {
          string * key = config->keys->vdrKeyUser2;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kUser3:
        {
          string * key = config->keys->vdrKeyUser3;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kUser4:
        {
          string * key = config->keys->vdrKeyUser4;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kUser5:
        {
          string * key = config->keys->vdrKeyUser5;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kUser6:
        {
          string * key = config->keys->vdrKeyUser6;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kUser7:
        {
          string * key = config->keys->vdrKeyUser7;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kUser8:
        {
          string * key = config->keys->vdrKeyUser8;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kUser9:
        {
          string * key = config->keys->vdrKeyUser9;
          if (key != NULL) {
            write(fdWritePipe, key->c_str(), key->size());
          }
        }
        break;
      case kChanUp:
      case kChanDn:
        return osEnd;
        break;
      default:
        break;
    }
  }
  else {
    switch (key) {
      case kStop:
      case kBlue:
      case kChanUp:
      case kChanDn:
        return osEnd;
      default:
        break;
    }
}

  return osContinue;
}
