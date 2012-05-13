/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: vdrtools.c 0.4 2005/11/16 18:39:18 hflor Exp $
 */

#include "vdrtools.h"
#include "undelete.h"
#include "i18n.h"
#include <vdr/menu.h>
#include <vdr/interface.h>

#if VDRVERSNUM < 10318
// --- cReadLine -------------------------------------------------------------

cReadLine::cReadLine(void)
{
  size = 0;
  buffer = NULL;
}

cReadLine::~cReadLine()
{
  free(buffer);
}

char *cReadLine::Read(FILE *f)
{
  int n = getline(&buffer, &size, f);
  if (n > 0) {
     n--;
     if (buffer[n] == '\n')
        buffer[n] = 0;
     return buffer;
     }
  return NULL;
}
#endif

#ifdef HAVE_SVDRP
bool GetVDRSize(const char *dir, long long &llSize)
{
  bool bRet = false;
  struct stat fileinfo;
  cPipe pipe;
  if (pipe.Open((const char *)cString::sprintf("find '%s' -follow -type f", dir), "r"))
  {
    cReadLine ReadLine;
    char *s;
    while ((s = ReadLine.Read(pipe)) != NULL)
    {
      if (stat(s, &fileinfo) != -1)
      {
        llSize += (long long)fileinfo.st_size;
        bRet = true;
      }
    }
    pipe.Close();
  }
  return bRet;
}
#endif

char *SkipQuote(char *s)
{
  char c = *s;
  strcpy(s, s + 1);
  while (*s && *s != c)
  {
    if (*s == '\\')
      strcpy(s, s + 1);
    if (*s)
      s++;
  }
  if (*s)
  {
    strcpy(s, s + 1);
    return s;
  }
  esyslog("ERROR: missing closing %c", c);
  return NULL;
}

char *ExchangeChars(char *s, bool ToFileSystem)
{
// ToFileSystem is not use, only for comatibility with 'ExchangeChars' from 'recording.c'
  char *p = s;
  while (*p)
  {
    switch (*p)
    {
         // mapped characters:
      case '_':    *p = ' ';
                   break;
      case '/':    *p = '~';
                   break;
         // encodes characters:
      case '#':    if (strlen(p) > 2)
                   {
                     char buf[3];
                     sprintf(buf, "%c%c", *(p + 1), *(p + 2));
                     unsigned char c = strtol(buf, NULL, 16);
                     *p = c;
                     memmove(p + 1, p + 3, strlen(p) - 2);
                   }
                   break;
      // backwards compatibility:
      case '\x01': *p = '\'';
                   break;
      case '\x02': *p = '/';
                   break;
      case '\x03': *p = ':';
                   break;
    }
    p++;
  }
  return s;
}

#ifdef UND_Debug
const char *KeyName(eKeys Key)
{
  switch (RAWKEY(Key))
  {
    case kUp:         return tr("Up");             break;
    case kDown:       return tr("Down");           break;
    case kMenu:       return tr("Menu");           break;
    case kOk:         return tr("Ok");             break;
    case kBack:       return tr("Back");           break;
    case kLeft:       return tr("Left");           break;
    case kRight:      return tr("Right");          break;
    case kRed:        return tr("Red");            break;
    case kGreen:      return tr("Green");          break;
    case kYellow:     return tr("Yellow");         break;
    case kBlue:       return tr("Blue");           break;
    case k0:          return "0";                  break;
    case k1:          return "1";                  break;
    case k2:          return "2";                  break;
    case k3:          return "3";                  break;
    case k4:          return "4";                  break;
    case k5:          return "5";                  break;
    case k6:          return "6";                  break;
    case k7:          return "7";                  break;
    case k8:          return "8";                  break;
    case k9:          return "9";                  break;
    case kPlay:       return tr("Play");           break;
    case kPause:      return tr("Pause");          break;
    case kStop:       return tr("Stop");           break;
    case kRecord:     return tr("Record");         break;
    case kFastFwd:    return tr("FastFwd");        break;
    case kFastRew:    return tr("FastRew");        break;
    case kPower:      return tr("Power");          break;
    case kChanUp:     return tr("ChanUp");         break;
    case kChanDn:     return tr("ChanDn");         break;
    case kVolUp:      return tr("VolUp");          break;
    case kVolDn:      return tr("VolDn");          break;
    case kMute:       return tr("Mute");           break;
    case kSchedule:   return tr("Schedule");       break;
    case kChannels:   return tr("Channels");       break;
    case kTimers:     return tr("Timers");         break;
    case kRecordings: return tr("Recordings");     break;
    case kSetup:      return tr("Setup");          break;
    case kCommands:   return tr("Commands");       break;
    case kUser1:      return tr("User1");          break;
    case kUser2:      return tr("User2");          break;
    case kUser3:      return tr("User3");          break;
    case kUser4:      return tr("User4");          break;
    case kUser5:      return tr("User5");          break;
    case kUser6:      return tr("User6");          break;
    case kUser7:      return tr("User7");          break;
    case kUser8:      return tr("User8");          break;
    case kUser9:      return tr("User9");          break;
    case kNone:       return tr("none");           break;
    case kKbd:        return tr("Kbd");            break;
    case kSalvageRec: return tr("Salvage");        break;
    case kPurgeRec:   return tr("Purge");          break;
    case kDelLine:    return tr("DeleteLine");     break;
    case kHelpKeys:   return tr("HelpKeys");       break;
    default:          return tr("unknow");         break;
  }
}

const char *OSStateName(eOSState OSState)
{
  switch (OSState)
  {
    case osUnknown:    return "osUnknown";
    case osContinue:   return "osContinue";
    case osSchedule:   return "osSchedule";
    case osChannels:   return "osChannels";
    case osTimers:     return "osTimers";
    case osRecordings: return "osRecordings";
    case osPlugin:     return "osPlugin";
    case osSetup:      return "osSetup";
    case osCommands:   return "osCommands";
    case osPause:      return "osPause";
    case osRecord:     return "osRecord";
    case osReplay:     return "osReplay";
    case osStopRecord: return "osStopRecord";
    case osStopReplay: return "osStopReplay";
    case osCancelEdit: return "osCancelEdit";
    case osSwitchDvb:  return "osSwitchDvb";
    case osBack:       return "osBack";
    case osEnd:        return "osEnd";
    case os_User:      return "os_User";
    case osUser1:      return "osUser1";
    case osUser2:      return "osUser2";
    case osUser3:      return "osUser3";
    case osUser4:      return "osUser4";
    case osUser5:      return "osUser5";
    case osUser6:      return "osUser6";
    case osUser7:      return "osUser7";
    case osUser8:      return "osUser8";
    case osUser9:      return "osUser9";
    case osUser10:     return "osUser10";
    default:           return "unknow";
  }
  return "";
}
#endif
