/*
 * recording.c: Recording file handling
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "recording.h"
#include <ctype.h>
#include "iconpatch.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "channels.h"
#include "i18n.h"
#include "interface.h"
#include "remux.h" //XXX+ I_FRAME
#include "skins.h"
#include "tools.h"
#include "videodir.h"

//#define SUMMARYFALLBACK

#define RECEXT       ".rec"
#define DELEXT       ".del"
/* This was the original code, which works fine in a Linux only environment.
   Unfortunately, because of Windows and its brain dead file system, we have
   to use a more complicated approach, in order to allow users who have enabled
   the --vfat command line option to see their recordings even if they forget to
   enable --vfat when restarting VDR... Gee, do I hate Windows.
   (kls 2002-07-27)
#define DATAFORMAT   "%4d-%02d-%02d.%02d:%02d.%02d.%02d" RECEXT
#define NAMEFORMAT   "%s/%s/" DATAFORMAT
*/
#define DATAFORMATPES   "%4d-%02d-%02d.%02d%*c%02d.%02d.%02d" RECEXT
#define NAMEFORMATPES   "%s/%s/" "%4d-%02d-%02d.%02d.%02d.%02d.%02d" RECEXT
#define DATAFORMATTS    "%4d-%02d-%02d.%02d.%02d.%d-%d" RECEXT
#define NAMEFORMATTS    "%s/%s/" DATAFORMATTS

#define RESUMEFILESUFFIX  "/resume%s%s"
#ifdef SUMMARYFALLBACK
#define SUMMARYFILESUFFIX "/summary.vdr"
#endif
#define INFOFILESUFFIX    "/info"
#define MARKSFILESUFFIX   "/marks"
#define INDEXFILESUFFIX   "/index"

#define MINDISKSPACE 1024 // MB

#define REMOVECHECKDELTA   60 // seconds between checks for removing deleted files
#define DELETEDLIFETIME   300 // seconds after which a deleted recording will be actually removed
#define DISKCHECKDELTA    100 // seconds between checks for free disk space
#define REMOVELATENCY      10 // seconds to wait until next check after removing a file
#define MAXREMOVETIME      10 // seconds after which to return from removing deleted recordings

#define MAX_SUBTITLE_LENGTH  40

#define MAX_LINK_LEVEL  6

bool VfatFileSystem = false;

cRecordings DeletedRecordings(true);

// --- cRemoveDeletedRecordingsThread ----------------------------------------

class cRemoveDeletedRecordingsThread : public cThread {
protected:
  virtual void Action(void);
public:
  cRemoveDeletedRecordingsThread(void);
  };

cRemoveDeletedRecordingsThread::cRemoveDeletedRecordingsThread(void)
:cThread("remove deleted recordings")
{
}

void cRemoveDeletedRecordingsThread::Action(void)
{
  SetPriority(19);
  // Make sure only one instance of VDR does this:
  cLockFile LockFile(VideoDirectory);
  if (LockFile.Lock()) {
     time_t StartTime = time(NULL);
     bool deleted = false;
     bool interrupted = false;
     cThreadLock DeletedRecordingsLock(&DeletedRecordings);
     for (cRecording *r = DeletedRecordings.First(); r; ) {
         if (time(NULL) - StartTime > MAXREMOVETIME)
            interrupted = true; // don't stay here too long
         else if (cRemote::HasKeys())
            interrupted = true; // react immediately on user input
         if (interrupted)
            break;
         if (r->deleted && time(NULL) - r->deleted > DELETEDLIFETIME) {
            cRecording *next = DeletedRecordings.Next(r);
            r->Remove();
            DeletedRecordings.Del(r);
            r = next;
            deleted = true;
            }
         else
            r = DeletedRecordings.Next(r);
         }
     if (deleted) {
        Recordings.TouchUpdate();
        if (!interrupted)
           RemoveEmptyVideoDirectories();
         }
     }
}

static cRemoveDeletedRecordingsThread RemoveDeletedRecordingsThread;

// ---

void RemoveDeletedRecordings(void)
{
  static time_t LastRemoveCheck = 0;
  if (time(NULL) - LastRemoveCheck > REMOVECHECKDELTA) {
     if (!RemoveDeletedRecordingsThread.Active()) {
        cThreadLock DeletedRecordingsLock(&DeletedRecordings);
        for (cRecording *r = DeletedRecordings.First(); r; r = DeletedRecordings.Next(r)) {
            if (r->deleted && time(NULL) - r->deleted > DELETEDLIFETIME) {
               RemoveDeletedRecordingsThread.Start();
               break;
               }
            }
        }
     LastRemoveCheck = time(NULL);
     }
}

void AssertFreeDiskSpace(int Priority, bool Force)
{
  static cMutex Mutex;
  cMutexLock MutexLock(&Mutex);
  // With every call to this function we try to actually remove
  // a file, or mark a file for removal ("delete" it), so that
  // it will get removed during the next call.
  static time_t LastFreeDiskCheck = 0;
  int Factor = (Priority == -1) ? 10 : 1;
  if (Force || time(NULL) - LastFreeDiskCheck > DISKCHECKDELTA / Factor) {
     if (!VideoFileSpaceAvailable(MINDISKSPACE)) {
        // Make sure only one instance of VDR does this:
        cLockFile LockFile(VideoDirectory);
        if (!LockFile.Lock())
           return;
        // Remove the oldest file that has been "deleted":
        isyslog("low disk space while recording, trying to remove a deleted recording...");
        cThreadLock DeletedRecordingsLock(&DeletedRecordings);
        if (DeletedRecordings.Count()) {
           cRecording *r = DeletedRecordings.First();
           cRecording *r0 = r;
           while (r) {
                 if (r->start < r0->start)
                    r0 = r;
                 r = DeletedRecordings.Next(r);
                 }
           if (r0 && r0->Remove()) {
              DeletedRecordings.Del(r0);
              LastFreeDiskCheck += REMOVELATENCY / Factor;
              return;
              }
           }
        // DeletedRecordings was empty, so to be absolutely sure there are no
        // deleted recordings we need to double check:
        DeletedRecordings.Update(true);
        if (DeletedRecordings.Count())
           return; // the next call will actually remove it
        // No "deleted" files to remove, so let's see if we can delete a recording:
        isyslog("...no deleted recording found, trying to delete an old recording...");
        cThreadLock RecordingsLock(&Recordings);
        if (Recordings.Count()) {
           cRecording *r = Recordings.First();
           cRecording *r0 = NULL;
           while (r) {
                 if (!r->IsEdited() && r->lifetime < MAXLIFETIME) { // edited recordings and recordings with MAXLIFETIME live forever
                    if ((r->lifetime == 0 && Priority > r->priority) || // the recording has no guaranteed lifetime and the new recording has higher priority
                        (r->lifetime > 0 && (time(NULL) - r->start) / SECSINDAY >= r->lifetime)) { // the recording's guaranteed lifetime has expired
                       if (r0) {
                          if (r->priority < r0->priority || (r->priority == r0->priority && r->start < r0->start))
                             r0 = r; // in any case we delete the one with the lowest priority (or the older one in case of equal priorities)
                          }
                       else
                          r0 = r;
                       }
                    }
                 r = Recordings.Next(r);
                 }
           if (r0 && r0->Delete()) {
              Recordings.Del(r0);
              return;
              }
           }
        // Unable to free disk space, but there's nothing we can do about that...
        isyslog("...no old recording found, giving up");
        Skins.QueueMessage(mtWarning, tr("Low disk space!"), 5, -1);
        }
     LastFreeDiskCheck = time(NULL);
     }
}

// --- cResumeFile -----------------------------------------------------------

cResumeFile::cResumeFile(const char *FileName, bool IsPesRecording)
{
  isPesRecording = IsPesRecording;
  const char *Suffix = isPesRecording ? RESUMEFILESUFFIX ".vdr" : RESUMEFILESUFFIX;
  fileName = MALLOC(char, strlen(FileName) + strlen(Suffix) + 1);
  if (fileName) {
     strcpy(fileName, FileName);
     sprintf(fileName + strlen(fileName), Suffix, Setup.ResumeID ? "." : "", Setup.ResumeID ? *itoa(Setup.ResumeID) : "");
     }
  else
     esyslog("ERROR: can't allocate memory for resume file name");
}

cResumeFile::~cResumeFile()
{
  free(fileName);
}

int cResumeFile::Read(void)
{
  int resume = -1;
  if (fileName) {
     struct stat st;
     if (stat(fileName, &st) == 0) {
        if ((st.st_mode & S_IWUSR) == 0) // no write access, assume no resume
           return -1;
        }
     if (isPesRecording) {
        int f = open(fileName, O_RDONLY);
        if (f >= 0) {
           if (safe_read(f, &resume, sizeof(resume)) != sizeof(resume)) {
              resume = -1;
              LOG_ERROR_STR(fileName);
              }
//M7X0 BEGIN AK
#if BYTE_ORDER == BIG_ENDIAN
           else
              resume = bswap_32(resume);
#endif
//M7X0 END AK
           close(f);
           }
        else if (errno != ENOENT)
           LOG_ERROR_STR(fileName);
        }
     else {
        FILE *f = fopen(fileName, "r");
        if (f) {
           cReadLine ReadLine;
           char *s;
           while ((s = ReadLine.Read(f)) != NULL) {
                 char *t = skipspace(s + 1);
                 switch (*s) {
                   case 'I':
#if BYTE_ORDER == BIG_ENDIAN
                             resume = bswap_32(atoi(t));
#else
                             resume = atoi(t);
#endif
                             break;
                   }
                 }
           fclose(f);
           }
        else if (errno != ENOENT)
           LOG_ERROR_STR(fileName);
        }
     }
  return resume;
}

bool cResumeFile::Save(int Index)
{
//M7X0 BEGIN AK
#if BYTE_ORDER == BIG_ENDIAN
  Index = bswap_32(Index);
#endif
//M7X0 END AK
  if (fileName) {
     if (isPesRecording) {
        int f = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, DEFFILEMODE);
        if (f >= 0) {
           if (safe_write(f, &Index, sizeof(Index)) < 0)
              LOG_ERROR_STR(fileName);
           close(f);
           Recordings.ResetResume(fileName);
           return true;
           }
        }
     else {
        FILE *f = fopen(fileName, "w");
        if (f) {
           fprintf(f, "I %d\n", Index);
           fclose(f);
           Recordings.ResetResume(fileName);
           }
        else
           LOG_ERROR_STR(fileName);
        return true;
        }
     }
  return false;
}

void cResumeFile::Delete(void)
{
  if (fileName) {
     if (remove(fileName) == 0)
        Recordings.ResetResume(fileName);
     else if (errno != ENOENT)
        LOG_ERROR_STR(fileName);
     }
}

// --- cRecordingInfo --------------------------------------------------------

cRecordingInfo::cRecordingInfo(const cChannel *Channel, const cEvent *Event)
{
  channelID = Channel ? Channel->GetChannelID() : tChannelID::InvalidID;
  channelName = Channel ? strdup(Channel->Name()) : NULL;
  ownEvent = Event ? NULL : new cEvent(0);
  event = ownEvent ? ownEvent : Event;
  aux = NULL;
  priority = MAXPRIORITY;
  lifetime = MAXLIFETIME;
  fileName = NULL;
  if (Channel) {
     // Since the EPG data's component records can carry only a single
     // language code, let's see whether the channel's PID data has
     // more information:
     cComponents *Components = (cComponents *)event->Components();
     if (!Components)
        Components = new cComponents;
     for (int i = 0; i < MAXAPIDS; i++) {
         const char *s = Channel->Alang(i);
         if (*s) {
            tComponent *Component = Components->GetComponent(i, 2, 3);
            if (!Component)
               Components->SetComponent(Components->NumComponents(), 2, 3, s, NULL);
            else if (strlen(s) > strlen(Component->language))
               strn0cpy(Component->language, s, sizeof(Component->language));
            }
         }
     // There's no "multiple languages" for Dolby Digital tracks, but
     // we do the same procedure here, too, in case there is no component
     // information at all:
     for (int i = 0; i < MAXDPIDS; i++) {
         const char *s = Channel->Dlang(i);
         if (*s) {
            tComponent *Component = Components->GetComponent(i, 2, 5);
            if (!Component)
               Components->SetComponent(Components->NumComponents(), 2, 5, s, NULL);
            else if (strlen(s) > strlen(Component->language))
               strn0cpy(Component->language, s, sizeof(Component->language));
            }
         }
     if (Components != event->Components())
        ((cEvent *)event)->SetComponents(Components);
     }
}

cRecordingInfo::cRecordingInfo(const char *FileName)
{
  channelID = tChannelID::InvalidID;
  ownEvent = new cEvent(0);
  event = ownEvent;
  aux = NULL;
  priority = MAXPRIORITY;
  lifetime = MAXLIFETIME;
  fileName = strdup(cString::sprintf("%s%s", FileName, INFOFILESUFFIX));
}

cRecordingInfo::~cRecordingInfo()
{
  delete ownEvent;
  free(aux);
  free(channelName);
  free(fileName);
}

void cRecordingInfo::SetData(const char *Title, const char *ShortText, const char *Description)
{
  if (!isempty(Title))
     ((cEvent *)event)->SetTitle(Title);
  if (!isempty(ShortText))
     ((cEvent *)event)->SetShortText(ShortText);
  if (!isempty(Description))
     ((cEvent *)event)->SetDescription(Description);
}

void cRecordingInfo::SetAux(const char *Aux)
{
  free(aux);
  aux = Aux ? strdup(Aux) : NULL;
}

bool cRecordingInfo::Read(FILE *f)
{
  if (ownEvent) {
     cReadLine ReadLine;
     char *s;
     int line = 0;
     while ((s = ReadLine.Read(f)) != NULL) {
           ++line;
           char *t = skipspace(s + 1);
           switch (*s) {
             case 'C': {
                         char *p = strchr(t, ' ');
                         if (p) {
                            free(channelName);
                            channelName = strdup(compactspace(p));
                            *p = 0; // strips optional channel name
                            }
                         if (*t)
                            channelID = tChannelID::FromString(t);
                       }
                       break;
             case 'E': {
                         unsigned int EventID;
                         time_t StartTime;
                         int Duration;
                         unsigned int TableID = 0;
                         unsigned int Version = 0xFF;
                         int n = sscanf(t, "%u %ld %d %X %X", &EventID, &StartTime, &Duration, &TableID, &Version);
                         if (n >= 3 && n <= 5) {
                            ownEvent->SetEventID(EventID);
                            ownEvent->SetStartTime(StartTime);
                            ownEvent->SetDuration(Duration);
                            ownEvent->SetTableID(uchar(TableID));
                            ownEvent->SetVersion(uchar(Version));
                            }
                       }
                       break;
             case 'F': break;
             case 'L': lifetime = atoi(t);
                       break;
             case 'P': priority = atoi(t);
                       break;
             case '@': free(aux);
                       aux = strdup(t);
                       break;
             case '#': break; // comments are ignored
             default: if (!ownEvent->Parse(s)) {
                         esyslog("ERROR: EPG data problem in line %d", line);
                         return false;
                         }
                      break;
             }
           }
     return true;
     }
  return false;
}

bool cRecordingInfo::Write(FILE *f, const char *Prefix) const
{
  if (channelID.Valid())
     fprintf(f, "%sC %s%s%s\n", Prefix, *channelID.ToString(), channelName ? " " : "", channelName ? channelName : "");
  event->Dump(f, Prefix, true);
  fprintf(f, "%sF %.10g\n", Prefix, double(FRAMESPERSEC));
  fprintf(f, "%sP %d\n", Prefix, priority);
  fprintf(f, "%sL %d\n", Prefix, lifetime);
  if (aux)
     fprintf(f, "%s@ %s\n", Prefix, aux);
  return true;
}

bool cRecordingInfo::Read(void)
{
  bool Result = false;
  if (fileName) {
     FILE *f = fopen(fileName, "r");
     if (f) {
        if (Read(f))
           Result = true;
        else
           esyslog("ERROR: EPG data problem in file %s", fileName);
        fclose(f);
        }
     else if (errno != ENOENT)
        LOG_ERROR_STR(fileName);
     }
  return Result;
}

bool cRecordingInfo::Write(void) const
{
  bool Result = false;
  if (fileName) {
     cSafeFile f(fileName);
     if (f.Open()) {
        if (Write(f))
           Result = true;
        f.Close();
        }
     else
        LOG_ERROR_STR(fileName);
     }
  return Result;
}

// --- cRecording ------------------------------------------------------------

#define RESUME_NOT_INITIALIZED (-2)

struct tCharExchange { char a; char b; };
tCharExchange CharExchange[] = {
  { FOLDERDELIMCHAR,  '/'    },
  { '/',  FOLDERDELIMCHAR    },
  { ' ',  '_'    },
  // backwards compatibility:
  { '\'', '\''   },
  { '\'', '\x01' },
  { '/',  '\x02' },
  { 0, 0 }
  };

char *ExchangeChars(char *s, bool ToFileSystem)
{
  char *p = s;
  while (*p) {
        if (VfatFileSystem) {
           // The VFAT file system can't handle all characters, so we
           // have to take extra efforts to encode/decode them:
           if (ToFileSystem) {
              const char *InvalidChars = "\"\\/:*?|<>#";
              switch (*p) {
                     // characters that can be mapped to other characters:
                     case ' ': *p = '_'; break;
                     case FOLDERDELIMCHAR: *p = '/'; break;
                     // characters that have to be encoded:
                     default:
                       if (strchr(InvalidChars, *p) || *p == '.' && (!*(p + 1) || *(p + 1) == FOLDERDELIMCHAR)) { // Windows can't handle '.' at the end of file/directory names
                          int l = p - s;
                          if (char *NewBuffer = (char *)realloc(s, strlen(s) + 10)) {
                             s = NewBuffer;
                             p = s + l;
                             char buf[4];
                             sprintf(buf, "#%02X", (unsigned char)*p);
                             memmove(p + 2, p, strlen(p) + 1);
                             memcpy(p, buf, 3);
                             p += 2;
                             }
                          else
                             esyslog("ERROR: out of memory");
                          }
                     }
              }
           else {
              switch (*p) {
                // mapped characters:
                case '_': *p = ' '; break;
                case '/': *p = FOLDERDELIMCHAR; break;
                // encoded characters:
                case '#': {
                     if (strlen(p) > 2 && isxdigit(*(p + 1)) && isxdigit(*(p + 2))) {
                        char buf[3];
                        sprintf(buf, "%c%c", *(p + 1), *(p + 2));
                        uchar c = uchar(strtol(buf, NULL, 16));
                        if (c) {
                           *p = c;
                           memmove(p + 1, p + 3, strlen(p) - 2);
                           }
                        }
                     }
                     break;
                // backwards compatibility:
                case '\x01': *p = '\''; break;
                case '\x02': *p = '/';  break;
                case '\x03': *p = ':';  break;
                default: ;
                }
              }
           }
        else {
           for (struct tCharExchange *ce = CharExchange; ce->a && ce->b; ce++) {
               if (*p == (ToFileSystem ? ce->a : ce->b)) {
                  *p = ToFileSystem ? ce->b : ce->a;
                  break;
                  }
               }
           }
        p++;
        }
  return s;
}

cRecording::cRecording(cTimer *Timer, const cEvent *Event)
{
  resume = RESUME_NOT_INITIALIZED;
  titleBuffer = NULL;
  for (int i = 0; i < MAXSORTMODES; i++) {
     sortBuffer[i] = NULL;
     lastDirsFirst[i] = -1;
  }
  fileName = NULL;
  name = NULL;
  fileSizeMB = -1; // unknown
  channel = Timer->Channel()->Number();
  const cChannel *ch = Timer->Channel();
  isPesRecording = !( ((Setup.UseTSInHD)    && (ch->Vpid(Setup.UseHDInRecordings)) && (ch->Vtype()==0x1B))
                   || ((Setup.UseTSInSD)    && (ch->Vpid(false)))
                   || ((Setup.UseTSInAudio) && (!ch->Vpid(Setup.UseHDInRecordings))) );
  instanceId = isPesRecording ? -1 : 0;
  deleted = 0;
  // set up the actual name:
  const char *Title = Event ? Event->Title() : NULL;
  const char *Subtitle = Event ? Event->ShortText() : NULL;
  char SubtitleBuffer[MAX_SUBTITLE_LENGTH];
  if (isempty(Title))
     Title = Timer->Channel()->Name();
  if (isempty(Subtitle))
     Subtitle = " ";
  else if (strlen(Subtitle) > MAX_SUBTITLE_LENGTH) {
     // let's make sure the Subtitle doesn't produce too long a file name:
     strn0cpy(SubtitleBuffer, Subtitle, MAX_SUBTITLE_LENGTH);
     Subtitle = SubtitleBuffer;
     }
  const char *macroTITLE   = strstr(Timer->File(), TIMERMACRO_TITLE);
  const char *macroEPISODE = strstr(Timer->File(), TIMERMACRO_EPISODE);
  if (macroTITLE || macroEPISODE) {
     name = strdup(Timer->File());
     name = strreplace(name, TIMERMACRO_TITLE, Title);
     name = strreplace(name, TIMERMACRO_EPISODE, Subtitle);
     // avoid blanks at the end:
     int l = strlen(name);
     while (l-- > 2) {
           if (name[l] == ' ' && name[l - 1] != FOLDERDELIMCHAR)
              name[l] = 0;
           else
              break;
           }
     if (Timer->IsSingleEvent()) {
        Timer->SetFile(name); // this was an instant recording, so let's set the actual data
        Timers.SetModified();
        }
     }
  else if (Timer->IsSingleEvent() || !Setup.UseSubtitle)
     name = strdup(Timer->File());
  else
     name = strdup(cString::sprintf("%s%c%s", Timer->File(), FOLDERDELIMCHAR, Subtitle));
  // substitute characters that would cause problems in file names:
  strreplace(name, '\n', ' ');
  if((Timer->Channel())&&(Setup.UseHDInRecordings))
     if(Timer->Channel()->Vtype()==0x1B)
        name = strdup(cString::sprintf("%s (HD)",(const char *)cString(name,true)));
  start = Timer->StartTime();
  priority = Timer->Priority();
  lifetime = Timer->Lifetime();
  // handle info:
  info = new cRecordingInfo(Timer->Channel(), Event);
  info->SetAux(Timer->Aux());
  info->priority = priority;
  info->lifetime = lifetime;
}

cRecording::cRecording(const char *FileName)
{
  resume = RESUME_NOT_INITIALIZED;
  fileSizeMB = -1; // unknown
  channel = -1;
  instanceId = -1;
  priority = MAXPRIORITY; // assume maximum in case there is no info file
  lifetime = MAXLIFETIME;
  isPesRecording = true;
  deleted = 0;
  titleBuffer = NULL;
  for (int i = 0; i < MAXSORTMODES; i++) {
     sortBuffer[i] = NULL;
     lastDirsFirst[i] = -1;
  }
  FileName = fileName = strdup(FileName);
  if (*(fileName + strlen(fileName) - 1) == '/')
     *(fileName + strlen(fileName) - 1) = 0;
  FileName += strlen(VideoDirectory) + 1;
  const char *p = strrchr(FileName, '/');

  name = NULL;
  info = new cRecordingInfo;
  if (p) {
     time_t now = time(NULL);
     struct tm tm_r;
     struct tm t = *localtime_r(&now, &tm_r); // this initializes the time zone in 't'
     t.tm_isdst = -1; // makes sure mktime() will determine the correct DST setting
     if (7 == sscanf(p + 1, DATAFORMATTS, &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &channel, &instanceId)
      || 7 == sscanf(p + 1, DATAFORMATPES, &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &priority, &lifetime)) {
        t.tm_year -= 1900;
        t.tm_mon--;
        t.tm_sec = 0;
        start = mktime(&t);
        name = MALLOC(char, p - FileName + 1);
        strncpy(name, FileName, p - FileName);
        name[p - FileName] = 0;
        name = ExchangeChars(name, false);
        isPesRecording = instanceId < 0;
        }
     else
        return;
     GetResume();
     // read an optional info file:
     cString InfoFileName = cString::sprintf("%s%s", fileName, isPesRecording ? INFOFILESUFFIX ".vdr" : INFOFILESUFFIX);
     FILE *f = fopen(InfoFileName, "r");
     while (!f && !FATALERRNO)
           f = fopen(InfoFileName, "r");
     if (f) {
        if (!info->Read(f))
           esyslog("ERROR: EPG data problem in file %s", *InfoFileName);
        else if (!isPesRecording) {
           priority = info->priority;
           lifetime = info->lifetime;
           }
        fclose(f);
        }
     else if (errno != ENOENT)
        LOG_ERROR_STR(*InfoFileName);

#ifdef SUMMARYFALLBACK
     // fall back to the old 'summary.vdr' if there was no 'info.vdr':
     if (isempty(info->Title())) {
        cString SummaryFileName = cString::sprintf("%s%s", fileName, SUMMARYFILESUFFIX);
        FILE *f = fopen(SummaryFileName, "r");
        while (!f && !FATALERRNO)
              f = fopen(SummaryFileName, "r");
        if (f) {
           int line = 0;
           char *data[3] = { NULL };
           cReadLine ReadLine;
           char *s;
           while ((s = ReadLine.Read(f)) != NULL) {
                 if (*s || line > 1) {
                    if (data[line]) {
                       int len = strlen(s);
                       len += strlen(data[line]) + 1;
                       if (char *NewBuffer = (char *)realloc(data[line], len + 1)) {
                          data[line] = NewBuffer;
                          strcat(data[line], "\n");
                          strcat(data[line], s);
                          }
                       else
                          esyslog("ERROR: out of memory");
                       }
                    else
                       data[line] = strdup(s);
                    }
                 else
                    line++;
                 }
           fclose(f);
           if (!data[2]) {
              data[2] = data[1];
              data[1] = NULL;
              }
           else if (data[1] && data[2]) {
              // if line 1 is too long, it can't be the short text,
              // so assume the short text is missing and concatenate
              // line 1 and line 2 to be the long text:
              int len = strlen(data[1]);
              if (len > 80) {
                 if (char *NewBuffer = (char *)realloc(data[1], len + 1 + strlen(data[2]) + 1)) {
                    data[1] = NewBuffer;
                    strcat(data[1], "\n");
                    strcat(data[1], data[2]);
                    free(data[2]);
                    data[2] = data[1];
                    data[1] = NULL;
                    }
                 else
                    esyslog("ERROR: out of memory");
                 }
              }
           info->SetData(data[0], data[1], data[2]);
           for (int i = 0; i < 3; i ++)
               free(data[i]);
           }
        else if (errno != ENOENT)
           LOG_ERROR_STR(*SummaryFileName);
        }
#endif
     }
}

cRecording::~cRecording()
{
  free(titleBuffer);
  for (int i = 0; i < MAXSORTMODES; i++) {
     free(sortBuffer[i]);
  }
  free(fileName);
  free(name);
  delete info;
}

char *cRecording::StripEpisodeName(char *s)
{
  char *t = s, *s1 = NULL, *s2 = NULL;
  while (*t) {
        if (*t == '/') {
           if (s1) {
              if (s2)
                 s1 = s2;
              s2 = t;
              }
           else
              s1 = t;
           }
        t++;
        }

  if (s1 && s2) {
    if (Setup.RecordingsSortDirsFirst)
      *s1 = 'b';

    if ((Setup.RecordingsSortMode <= 1 && s1 != s && !strchr(".-$ª·", *(s1 - 1))) ||
        (Setup.RecordingsSortMode == 1 && s1 == s) ||
        (Setup.RecordingsSortMode == 3))
     memmove(s1 + 1, s2, t - s2 + 1);
  }

  return s;
}

char *cRecording::SortName(void) const
{
  if (!sortBuffer[Setup.RecordingsSortMode] ||
        lastDirsFirst[Setup.RecordingsSortMode] != Setup.RecordingsSortDirsFirst) {
     free(sortBuffer[Setup.RecordingsSortMode]);
     lastDirsFirst[Setup.RecordingsSortMode] = Setup.RecordingsSortDirsFirst;
     char *s = StripEpisodeName(strdup(FileName() + strlen(VideoDirectory)));
     strreplace(s, '/', 'a'); // some locales ignore '/' when sorting
     int l = strxfrm(NULL, s, 0) + 1;
     sortBuffer[Setup.RecordingsSortMode] = MALLOC(char, l);
     strxfrm(sortBuffer[Setup.RecordingsSortMode], s, l);
     free(s);
     }
  return sortBuffer[Setup.RecordingsSortMode];
}

int cRecording::GetResume(void) const
{
  if (resume == RESUME_NOT_INITIALIZED) {
     cResumeFile ResumeFile(FileName(), isPesRecording);
     resume = ResumeFile.Read();
     }
  return resume;
}

int cRecording::Compare(const cListObject &ListObject) const
{
  cRecording *r = (cRecording *)&ListObject;
  return Recordings.GetSortOrder() * strcasecmp(SortName(), r->SortName());
}

const char *cRecording::FileName(void) const
{
  if (!fileName) {
     struct tm tm_r;
     struct tm *t = localtime_r(&start, &tm_r);
     const char *fmt = isPesRecording ? NAMEFORMATPES : NAMEFORMATTS;
     int ch = isPesRecording ? priority : channel;
     int ri = isPesRecording ? lifetime : instanceId;
     name = ExchangeChars(name, true);
     fileName = strdup(cString::sprintf(fmt, VideoDirectory, name, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, ch, ri));
     name = ExchangeChars(name, false);
     }
  return fileName;
}

const char *cRecording::Title(char Delimiter, bool NewIndicator, int Level, bool Original) const
{
  char New = NewIndicator && IsNew() ? Setup.WarEagleIcons ? ICON_NEU : '*' : ' ';
  free(titleBuffer);
  titleBuffer = NULL;
  if (Level < 0 || Level == HierarchyLevels()) {
     struct tm tm_r;
     struct tm *t = localtime_r(&start, &tm_r);
     char *s;
     if (Level > 0 && (s = strrchr(name, FOLDERDELIMCHAR)) != NULL)
        s++;
     else
        s = name;
     if (Original) {
     titleBuffer = strdup(cString::sprintf("%02d.%02d.%02d%c%02d:%02d%c%c%s",
                            t->tm_mday,
                            t->tm_mon + 1,
                            t->tm_year % 100,
                            Delimiter,
                            t->tm_hour,
                            t->tm_min,
                            New,
                            Delimiter,
                            s));
        }
     else {
        cString RecLength("---");
        if (Setup.ShowRecLength && FileName()) {
           int length = cIndexFile::GetLength(FileName(), IsPesRecording());
           if (length >= 0)
              RecLength = cString::sprintf("%d'", length / SecondsToFrames(60));
           }
        cString RecDate = cString::sprintf("%02d.%02d.%02d", t->tm_mday, t->tm_mon + 1, t->tm_year % 100);
        cString RecTime = cString::sprintf("%02d:%02d", t->tm_hour, t->tm_min);
        cString RecDelimiter = cString::sprintf("%c", Delimiter);
        titleBuffer = strdup(cString::sprintf("%s%s%s%c%s%s%s%s",
                               (Setup.ShowRecDate ? *RecDate        : ""),
                               (Setup.ShowRecDate && Setup.ShowRecTime ? *RecDelimiter : ""),
                               (Setup.ShowRecTime ? *RecTime        : ""),
                               New,
                               (Setup.ShowRecTime || Setup.ShowRecDate ? *RecDelimiter : ""),
                               (Setup.ShowRecLength ? *RecLength    : ""),
                               (Setup.ShowRecLength ? *RecDelimiter : ""),
                               s));
        }
     // let's not display a trailing FOLDERDELIMCHAR:
     if (!NewIndicator)
        stripspace(titleBuffer);
     s = &titleBuffer[strlen(titleBuffer) - 1];
     if (*s == FOLDERDELIMCHAR)
        *s = 0;
     }
  else if (Level < HierarchyLevels()) {
     const char *s = name;
     const char *p = s;
     while (*++s) {
           if (*s == FOLDERDELIMCHAR) {
              if (Level--)
                 p = s + 1;
              else
                 break;
              }
           }
     titleBuffer = MALLOC(char, s - p + 3);
     *titleBuffer = Delimiter;
     *(titleBuffer + 1) = Delimiter;
     strn0cpy(titleBuffer + 2, p, s - p + 1);
     }
  else
     return "";
  return titleBuffer;
}

void cRecording::SetStartTime(time_t Start)
{
  start=Start;
  if (fileName) {
  	 free(fileName);
  	 fileName = NULL;
  	 }
}

const char *cRecording::PrefixFileName(char Prefix)
{
  cString p = PrefixVideoFileName(FileName(), Prefix);
  if (*p) {
     free(fileName);
     fileName = strdup(p);
     return fileName;
     }
  return NULL;
}

int cRecording::HierarchyLevels(void) const
{
  const char *s = name;
  int level = 0;
  while (*++s) {
        if (*s == FOLDERDELIMCHAR)
           level++;
        }
  return level;
}

bool cRecording::IsEdited(void) const
{
  const char *s = strrchr(name, FOLDERDELIMCHAR);
  s = !s ? name : s + 1;
  return *s == '%';
}

bool cRecording::WriteInfo(void)
{
  cString InfoFileName = cString::sprintf("%s%s", fileName, isPesRecording ? INFOFILESUFFIX ".vdr" : INFOFILESUFFIX);
  FILE *f = fopen(InfoFileName, "w");
  if (f) {
     info->Write(f);
     fclose(f);
     }
  else
     LOG_ERROR_STR(*InfoFileName);
  return true;
}

bool cRecording::Delete(void)
{
  bool result = true;
  char *NewName = strdup(FileName());
  char *ext = strrchr(NewName, '.');
  if (ext && strcmp(ext, RECEXT) == 0) {
     strncpy(ext, DELEXT, strlen(ext));
     if (access(NewName, F_OK) == 0) {
        // the new name already exists, so let's remove that one first:
        isyslog("removing recording '%s'", NewName);
        RemoveVideoFile(NewName);
        }
     isyslog("deleting recording '%s'", FileName());
     if (access(FileName(), F_OK) == 0)
        result = RenameVideoFile(FileName(), NewName);
     else {
        isyslog("recording '%s' vanished", FileName());
        result = true; // well, we were going to delete it, anyway
        }
     }
  free(NewName);
  return result;
}

bool cRecording::Remove(void)
{
  // let's do a final safety check here:
  if (!endswith(FileName(), DELEXT)) {
     esyslog("attempt to remove recording %s", FileName());
     return false;
     }
  isyslog("removing recording %s", FileName());
  return RemoveVideoFile(FileName());
}

void cRecording::ResetResume(void) const
{
  resume = RESUME_NOT_INITIALIZED;
}

bool cRecording::Rename(const char *newName, int *newPriority, int *newLifetime)
{
  bool result = false;
  if (*newPriority != priority || *newLifetime != lifetime) {
     dsyslog("changing priority/lifetime of '%s' to %d/%d", Name(), *newPriority, *newLifetime);
     if (IsPesRecording()) {
        priority = *newPriority;
        lifetime = *newLifetime;
        }
     else {
        priority = info->priority = *newPriority;
        lifetime = info->lifetime = *newLifetime;
        if (!WriteInfo())
           return result;
        result = true;
        }
     }
  struct tm tm_r;
  struct tm *t = localtime_r(&start, &tm_r);
  char *localNewName = ExchangeChars(strdup(newName), true);
  const char *fmt = isPesRecording ? NAMEFORMATPES : NAMEFORMATTS;
  int ch = isPesRecording ? priority : channel;
  int ri = isPesRecording ? lifetime : instanceId;
  char *newFileName = strdup(cString::sprintf(fmt, VideoDirectory, localNewName, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, ch, ri));
  free(localNewName);
  if (strcmp(FileName(), newFileName)) {
     if (access(newFileName, F_OK) == 0) {
        isyslog("recording %s already exists", newFileName);
        result = false;
        }
     else {
        isyslog("renaming recording %s to %s", FileName(), newFileName);
        result = MakeDirs(newFileName, true);
        if (result)
           result = RenameVideoFile(FileName(), newFileName);
        if (result) {
           free(fileName);
           fileName = strdup(newFileName);
           free(name);
           name = strdup(newName);
#ifdef SORTRECORDINGSVERSNUM
           for (int i = 0; i < MAXSORTMODES; i++) {
              free(sortBuffer[i]);
              sortBuffer[i] = NULL;
           }
#else
           free(sortBuffer);
           sortBuffer = NULL;
#endif
           free(titleBuffer);
           titleBuffer = NULL;
           }
        }
     }
  free(newFileName);
  return result;
}

// --- cRecordings -----------------------------------------------------------

cRecordings Recordings;

char *cRecordings::updateFileName = NULL;

cRecordings::cRecordings(bool Deleted)
:cThread("video directory scanner")
{
  deleted = Deleted;
  lastUpdate = 0;
  state = 0;
  SortOrder = 1;
}

cRecordings::~cRecordings()
{
  Cancel(3);
}

void cRecordings::Action(void)
{
//M7X0 BEGIN AK
  SetPriority(10);
  Refresh();
//M7X0 END AK
}

const char *cRecordings::UpdateFileName(void)
{
  if (!updateFileName)
     updateFileName = strdup(AddDirectory(VideoDirectory, ".update"));
  return updateFileName;
}

void cRecordings::Refresh(bool Foreground)
{
  lastUpdate = time(NULL); // doing this first to make sure we don't miss anything
  Lock();
  Clear();
  ChangeState();
  Unlock();
  ScanVideoDir(VideoDirectory, Foreground);
}

void cRecordings::ScanVideoDir(const char *DirName, bool Foreground, int LinkLevel)
{
  cReadDir d(DirName);
  struct dirent *e;
  while ((Foreground || Running()) && (e = d.Next()) != NULL) {
        if (strcmp(e->d_name, ".") && strcmp(e->d_name, "..")) {
           cString buffer = AddDirectory(DirName, e->d_name);
           struct stat st;
           if (lstat(buffer, &st) == 0) {
              int Link = 0;
              if (S_ISLNK(st.st_mode)) {
                 if (LinkLevel > MAX_LINK_LEVEL) {
                    isyslog("max link level exceeded - not scanning %s", *buffer);
                    continue;
                    }
                 Link = 1;
                 buffer = ReadLink(buffer);
                 if (!*buffer)
                    continue;
                 if (stat(buffer, &st) != 0)
                    continue;
                 }
              if (S_ISDIR(st.st_mode)) {
                 if (endswith(buffer, deleted ? DELEXT : RECEXT)) {
                    cRecording *r = new cRecording(buffer);
                    if (r->Name()) {
                       Lock();
                       Add(r);
                       ChangeState();
                       Unlock();
                       if (deleted) {
                          r->fileSizeMB = DirSizeMB(buffer);
                          r->deleted = time(NULL);
                          }
                       }
                    else
                       delete r;
                    }
                 else
                    ScanVideoDir(buffer, Foreground, LinkLevel + Link);
                 }
              }
           }
        }
}

bool cRecordings::StateChanged(int &State)
{
  int NewState = state;
  bool Result = State != NewState;
  State = state;
  return Result;
}

void cRecordings::TouchUpdate(void)
{
  bool needsUpdate = NeedsUpdate();
  TouchFile(UpdateFileName());
  if (!needsUpdate)
     lastUpdate = time(NULL); // make sure we don't trigger ourselves
}

bool cRecordings::NeedsUpdate(void)
{
  time_t lastModified = LastModifiedTime(UpdateFileName());
  if (lastModified > time(NULL))
     return false; // somebody's clock isn't running correctly
  return lastUpdate < lastModified;
}

bool cRecordings::Update(bool Wait)
{
  if (Wait) {
     Refresh(true);
     return Count() > 0;
     }
  else
     Start();
  return false;
}

cRecording *cRecordings::GetByName(const char *FileName)
{
  if (FileName) {
     for (cRecording *recording = First(); recording; recording = Next(recording)) {
         if (strcmp(recording->FileName(), FileName) == 0)
            return recording;
         }
     }
  return NULL;
}

void cRecordings::AddByName(const char *FileName, bool TriggerUpdate)
{
  LOCK_THREAD;
  cRecording *recording = GetByName(FileName);
  if (!recording) {
     recording = new cRecording(FileName);
     Add(recording);
     ChangeState();
     if (TriggerUpdate)
        TouchUpdate();
     }
}

void cRecordings::DelByName(const char *FileName)
{
  LOCK_THREAD;
  cRecording *recording = GetByName(FileName);
  if (recording) {
     cThreadLock DeletedRecordingsLock(&DeletedRecordings);
     Del(recording, false);
     char *ext = strrchr(recording->FileName(), '.');
     if (ext) {
        strncpy(ext, DELEXT, strlen(ext));
        recording->fileSizeMB = DirSizeMB(recording->FileName());
        recording->deleted = time(NULL);
        DeletedRecordings.Add(recording);
        }
     else
        delete recording;
     ChangeState();
     TouchUpdate();
     }
}

int cRecordings::TotalFileSizeMB(void)
{
  int size = 0;
  LOCK_THREAD;
  for (cRecording *recording = First(); recording; recording = Next(recording)) {
      if (recording->fileSizeMB > 0)
         size += recording->fileSizeMB;
      }
  return size;
}

void cRecordings::ResetResume(const char *ResumeFileName)
{
  LOCK_THREAD;
  for (cRecording *recording = First(); recording; recording = Next(recording)) {
      if (!ResumeFileName || strncmp(ResumeFileName, recording->FileName(), strlen(recording->FileName())) == 0)
         recording->ResetResume();
      }
  ChangeState();
}

// --- cMark -----------------------------------------------------------------

cMark::cMark(int Position, const char *Comment)
{
  position = Position;
  comment = Comment ? strdup(Comment) : NULL;
}

cMark::~cMark()
{
  free(comment);
}

cString cMark::ToText(void)
{
  return cString::sprintf("%s%s%s\n", *IndexToHMSF(position, true), comment ? " " : "", comment ? comment : "");
}

bool cMark::Parse(const char *s)
{
  free(comment);
  comment = NULL;
  position = HMSFToIndex(s);
  const char *p = strchr(s, ' ');
  if (p) {
     p = skipspace(p);
     if (*p)
        comment = strdup(p);
     }
  return true;
}

bool cMark::Save(FILE *f)
{
  return fprintf(f, "%s", *ToText()) > 0;
}

// --- cMarks ----------------------------------------------------------------

bool cMarks::Load(const char *RecordingFileName, bool IsPesRecording)
{
  if (cConfig<cMark>::Load(AddDirectory(RecordingFileName, IsPesRecording ? MARKSFILESUFFIX ".vdr" : MARKSFILESUFFIX))) {
     Sort();
     return true;
     }
  return false;
}

void cMarks::Sort(void)
{
  for (cMark *m1 = First(); m1; m1 = Next(m1)) {
      for (cMark *m2 = Next(m1); m2; m2 = Next(m2)) {
          if (m2->position < m1->position) {
             std::swap(m1->position, m2->position);
             std::swap(m1->comment, m2->comment);
             }
          }
      }
}

cMark *cMarks::Add(int Position)
{
  cMark *m = Get(Position);
  if (!m) {
     cConfig<cMark>::Add(m = new cMark(Position, NULL));
     Sort();
     }
  return m;
}

cMark *cMarks::Get(int Position)
{
  for (cMark *mi = First(); mi; mi = Next(mi)) {
      if (mi->position == Position)
         return mi;
      }
  return NULL;
}

cMark *cMarks::GetPrev(int Position)
{
  for (cMark *mi = Last(); mi; mi = Prev(mi)) {
      if (mi->position < Position)
         return mi;
      }
  return NULL;
}

cMark *cMarks::GetNext(int Position)
{
  for (cMark *mi = First(); mi; mi = Next(mi)) {
      if (mi->position > Position)
         return mi;
      }
  return NULL;
}

// --- cRecordingUserCommand -------------------------------------------------

const char *cRecordingUserCommand::command = NULL;

void cRecordingUserCommand::InvokeCommand(const char *State, const char *RecordingFileName)
{
  if (command) {

     cString cmd = cString::sprintf("%s %s \"%s\"", command, State, *strescape(RecordingFileName, "\\\"$"));
     isyslog("executing '%s'", *cmd);
     SystemExec(cmd);
     }
}

// --- cIndexFile ------------------------------------------------------------

// The number of frames to stay off the end in case of time shift:
#define INDEXSAFETYLIMIT 150 // frames

// The maximum time to wait before giving up while catching up on an index file:
#define MAXINDEXCATCHUP   8 // seconds

// The minimum age of an index file for considering it no longer to be written:
#define MININDEXAGE    60 // seconds

cIndexFile::cIndexFile(const char *FileName, bool Record, bool IsPesRecording)
:resumeFile(FileName, IsPesRecording)
{
  f = -1;
  fileName = NULL;
  size = 0;
  last = -1;
  index = NULL;
  isPesRecording = IsPesRecording;
  if (FileName) {
     const char *Suffix = isPesRecording ? INDEXFILESUFFIX ".vdr" : INDEXFILESUFFIX;
     fileName = MALLOC(char, strlen(FileName) + strlen(Suffix) + 1);
     if (fileName) {
        strcpy(fileName, FileName);
        char *pFileExt = fileName + strlen(fileName);
        strcpy(pFileExt, Suffix);
        int delta = 0;
        if (access(fileName, R_OK) == 0) {
           struct stat buf;
           if (stat(fileName, &buf) == 0) {
              delta = int(buf.st_size % (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs)));
              if (delta) {
                 delta = (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs)) - delta;
                 esyslog("ERROR: invalid file size (%lld) in '%s'", buf.st_size, fileName);
                 }
              last = int((buf.st_size + delta) / (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs)) - 1);
              if (!Record && last >= 0) {
                 size = last + 1;
                 index = MALLOC(tIndex, size);
                 if (index) {
                    f = open(fileName, O_RDONLY);
                    if (f >= 0) {
                       if (safe_read(f, index, size_t(buf.st_size)) != buf.st_size) {
                          esyslog("ERROR: can't read from file '%s'", fileName);
                          free(index);
                          index = NULL;
                          close(f);
                          f = -1;
                          }
//M7X0 BEGIN AK
#if BYTE_ORDER == BIG_ENDIAN
                       else {
                          for (int j=0; j <= last; j++)
                              index[j].SetOffset(isPesRecording, bswap_32(index[j].Offset(isPesRecording)));
                          }
#endif
//M7X0 END AK
                       // we don't close f here, see CatchUp()!
                       }
                    else
                       LOG_ERROR_STR(fileName);

                    }
                 else
                    esyslog("ERROR: can't allocate %zd bytes for index '%s'", size * (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs)), fileName);
                 }
              }
           else
              LOG_ERROR;
           }
        else if (!Record)
           isyslog("missing index file %s", fileName);
        if (Record) {
           if ((f = open(fileName, O_RDWR | O_CREAT | O_APPEND, DEFFILEMODE)) >= 0) {
//M7X0 END AK
              if (delta) {
                 esyslog("ERROR: padding index file with %d '0' bytes", delta);
                 while (delta--)
                       writechar(f, 0);
                 }
              }
           else
              LOG_ERROR_STR(fileName);
           }
        }
     else
        esyslog("ERROR: can't copy file name '%s'", FileName);
     }
}

cIndexFile::~cIndexFile()
{
  if (f >= 0)
     close(f);
  free(fileName);
  free(index);
}

cString cIndexFile::IndexFileName(const char *FileName, bool IsPesRecording)
{
  return cString::sprintf("%s%s", FileName, IsPesRecording ? INDEXFILESUFFIX ".vdr" : INDEXFILESUFFIX);
}

bool cIndexFile::CatchUp(int Index)
{
  // returns true unless something really goes wrong, so that 'index' becomes NULL
  if (index && f >= 0) {
     cMutexLock MutexLock(&mutex);
     for (int i = 0; i <= MAXINDEXCATCHUP && (Index < 0 || Index >= last); i++) {
         struct stat buf;
         if (fstat(f, &buf) == 0) {
            if (time(NULL) - buf.st_mtime > MININDEXAGE) {
               // apparently the index file is not being written any more
               close(f);
               f = -1;
               break;
               }
            int newLast = buf.st_size / (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs)) - 1;
            if (newLast > last) {
               int NewSize = size;
               if (NewSize <= newLast) {
                  NewSize *= 2;
                  if (NewSize <= newLast)
                     NewSize = newLast + 1;
                  }
               if (tIndex *NewBuffer = (tIndex *)realloc(index, NewSize * sizeof(tIndex))) {
                  size = NewSize;
                  index = NewBuffer;
                  int offset = (last + 1) * (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs));
                  int delta = (newLast - last) * (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs));
                  if (lseek(f, offset, SEEK_SET) == offset) {
                     if (safe_read(f, &index[last + 1], delta) != delta) {
                        esyslog("ERROR: can't read from index");
                        free(index);
                        index = NULL;
                        close(f);
                        f = -1;
                        break;
                        }
//M7X0 BEGIN AK
#if BYTE_ORDER == BIG_ENDIAN
                     for (int j=last+1; j <= newLast; j++)
                         index[j].SetOffset(isPesRecording, bswap_32(index[j].Offset(isPesRecording)));
#endif
//M7X0 END AK
                     last = newLast;
                     }
                  else
                     LOG_ERROR_STR(fileName);
                  }
               else {
                  esyslog("ERROR: can't realloc() index");
                  break;
                  }
               }
            }
         else
            LOG_ERROR_STR(fileName);
         if (Index < last - (i ? 2 * INDEXSAFETYLIMIT : 0) || Index > 10 * INDEXSAFETYLIMIT) // keep off the end in case of "Pause live video"
            break;
         cCondWait::SleepMs(1000);
         }
     }
  return index != NULL;
}

bool cIndexFile::Write(uchar PictureType, uint16_t FileNumber, off_t FileOffset)
{
  if (f >= 0) {
     tIndex i;
//M7X0 BEGIN AK
#if BYTE_ORDER == BIG_ENDIAN
     i.Set(isPesRecording, bswap_32(FileOffset), PictureType, FileNumber);
#else
     i.Set(isPesRecording, FileOffset, PictureType, FileNumber);
#endif
//M7X0 END AK

     if (safe_write(f, &i, sizeof(i)) < 0) {
        LOG_ERROR_STR(fileName);
        close(f);
        f = -1;
        return false;
        }
     last++;
     }
  return f >= 0;
}

//M7X0 BEGIN AK
bool cIndexFile::Write(sPesResult *Picture,  int PictureCount, uint16_t FileNumber, off_t FileOffset)
{
  if (f >= 0) {
     tIndex inds[PictureCount];
     int i;
     int count=0;
     for (i = 0;i < PictureCount; i++)
         if (Picture[i].pictureType) {

#if BYTE_ORDER == BIG_ENDIAN
            inds[count].SetOffset(isPesRecording, bswap_32(FileOffset + Picture[i].offset));
#else
            inds[count].SetOffset(isPesRecording, FileOffset + Picture[i].offset);
#endif

            inds[count].SetType(isPesRecording, Picture[i].pictureType);
            inds[count].SetNumber(isPesRecording, FileNumber);
            inds[count++].SetReserved(isPesRecording);
            }

     if (count) {
        if (safe_write(f, inds, (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs))*count) < 0) {
           LOG_ERROR_STR(fileName);
           close(f);
           f = -1;
          return false;
          }
        last+= count;
        }
     }
  return f >= 0;
}

int cIndexFile::StripOffToLastIFrame(uint16_t number)
{
  if (f < 0)
     return -1;

  tIndex ind;
  ind.SetNumber(isPesRecording, number);
  ind.SetType(isPesRecording, 0xFF);
  ind.SetOffset(isPesRecording, 0);
  off_t newEnd = 0;
  int i = 0;
  while ((ind.Number(isPesRecording) == number) & (ind.Type(isPesRecording) != I_FRAME)) {
        i++;
        newEnd = lseek(f, -((isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs)) * i), SEEK_END);
        if (newEnd < 0) {
           LOG_ERROR;
           return -1;
           }

        if (safe_read(f, &ind, (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs))) != (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs))) {
           LOG_ERROR;
           return -1;
           }
        }

  // This should not happen -- to be safe
  if (ind.Number(isPesRecording) != number) {
     esyslog("Internal Error: Recording file starts not with I-Frame");
     newEnd += (isPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs));
     ind.SetOffset(isPesRecording, 0);
     }

  ftruncate(f, newEnd);
#if BYTE_ORDER == BIG_ENDIAN
  return bswap_32(ind.Offset(isPesRecording));
#else
  return ind.Offset(isPesRecording);
#endif
}
//M7X0 END AK
bool cIndexFile::Get(int Index, uint16_t *FileNumber, off_t *FileOffset, uchar *PictureType, int *Length)
{
  if (CatchUp(Index)) {
     if (Index >= 0 && Index < last) {
        *FileNumber = index[Index].Number(isPesRecording);
        *FileOffset = index[Index].Offset(isPesRecording);
        if (PictureType)
           *PictureType = index[Index].Type(isPesRecording);
        if (Length) {
           uint16_t fn = index[Index + 1].Number(isPesRecording);
           off_t fo = index[Index + 1].Offset(isPesRecording);
           if (fn == *FileNumber)
              *Length = int(fo - *FileOffset);
           else
              *Length = -1; // this means "everything up to EOF" (the buffer's Read function will act accordingly)
           }
        return true;
        }
     }
  return false;
}

int cIndexFile::GetNextIFrame(int Index, bool Forward, uint16_t *FileNumber, off_t *FileOffset, int *Length, bool StayOffEnd)
{
  if (CatchUp()) {
     int d = Forward ? 1 : -1;
     for (;;) {
         Index += d;
         if (Index >= 0 && Index < last - ((Forward && StayOffEnd) ? INDEXSAFETYLIMIT : 0)) {
            if (index[Index].Type(isPesRecording) == I_FRAME) {
               uint16_t fn;
               if (!FileNumber)
                  FileNumber = &fn;
               off_t fo;
               if (!FileOffset)
                  FileOffset = &fo;
               *FileNumber = index[Index].Number(isPesRecording);
               *FileOffset = index[Index].Offset(isPesRecording);
               if (Length) {
                  // all recordings end with a non-I_FRAME, so the following should be safe:
                  uint16_t fn = index[Index + 1].Number(isPesRecording);
                  off_t fo = index[Index + 1].Offset(isPesRecording);
                  if (fn == *FileNumber)
                     *Length = int(fo - *FileOffset);
                  else {
                     esyslog("ERROR: 'I' frame at end of file #%d", *FileNumber);
                     *Length = -1;
                     }
                  }
               return Index;
               }
            }
         else
            break;
         }
     }
  return -1;
}

int cIndexFile::Get(uint16_t FileNumber, off_t FileOffset)
{
  if (CatchUp()) {
     //TODO implement binary search!
     int i;
     for (i = 0; i < last; i++) {
         if (index[i].Number(isPesRecording) > FileNumber || (index[i].Number(isPesRecording) == FileNumber) && off_t(index[i].Offset(isPesRecording)) >= FileOffset)
            break;
         }
     return i;
     }
  return -1;
}

bool cIndexFile::IsStillRecording()
{
  return f >= 0;
}

cUnbufferedFile *cIndexFile::NextFile(cFileName *FileName, bool Record)
{
  return FileName->NextFile();
}

int cIndexFile::WaitIndex(int Index)
{
	if (!IsStillRecording())
		return -1;
	return Index-1;
}

int cIndexFile::GetLength(const char *FileName, bool IsPesRecording)
{
  struct stat buf;
  cString s = IndexFileName(FileName, IsPesRecording);
  if (*s && stat(s, &buf) == 0)
     return buf.st_size / (IsPesRecording ? sizeof(tIndexPes) : sizeof(tIndexTs));
  return -1;
}

// --- cFileName -------------------------------------------------------------

#include <errno.h>
#include <unistd.h>
#include "videodir.h"

//M7X0 BEGIN AK
#ifdef USE_DIRECT_IO
cFileName::cFileName(const char *FileName, bool Record, bool Blocking, bool Direct, bool IsPesRecording)
#else
cFileName::cFileName(const char *FileName, bool Record, bool Blocking, bool IsPesRecording)
#endif
{
  file = NULL;
  fileNumber = 0;
  record = Record;
  blocking = Blocking;
  isPesRecording = IsPesRecording;
#ifdef USE_DIRECT_IO
  direct = Direct;
#endif
  // Prepare the file name:
  fileName = MALLOC(char, strlen(FileName) + RECORDFILESUFFIXLEN);
  if (!fileName) {
     esyslog("ERROR: can't copy file name '%s'", FileName);
     return;
     }
  strcpy(fileName, FileName);
  pFileNumber = fileName + strlen(fileName);
  SetOffset(1);
}

cFileName::~cFileName()
{
  Close();
  free(fileName);
}

bool cFileName::GetLastPatPmtVersions(int &PatVersion, int &PmtVersion)
{
  if (fileName && !isPesRecording) {
     // Find the last recording file:
     int Number = 1;
     for (; Number <= MAXFILESPERRECORDINGTS + 1; Number++) { // +1 to correctly set Number in case there actually are that many files
         sprintf(pFileNumber, RECORDFILESUFFIXTS, Number);
         if (access(fileName, F_OK) != 0) { // file doesn't exist
            Number--;
            break;
            }
         }
     for (; Number > 0; Number--) {
         // Search for a PAT packet from the end of the file:
         cPatPmtParser PatPmtParser;
         sprintf(pFileNumber, RECORDFILESUFFIXTS, Number);
         int fd = open(fileName, O_RDONLY | O_LARGEFILE, DEFFILEMODE);
         if (fd >= 0) {
            off_t pos = lseek(fd, -TS_SIZE, SEEK_END);
            while (pos >= 0) {
                  // Read and parse the PAT/PMT:
                  uchar buf[TS_SIZE];
                  while (read(fd, buf, sizeof(buf)) == sizeof(buf)) {
                        if (buf[0] == TS_SYNC_BYTE) {
                           int Pid = TsPid(buf);
                           if (Pid == PATPID)
                              PatPmtParser.ParsePat(buf, sizeof(buf));
                           else if (Pid == PatPmtParser.PmtPid()) {
                              PatPmtParser.ParsePmt(buf, sizeof(buf));
                              if (PatPmtParser.GetVersions(PatVersion, PmtVersion)) {
                                 close(fd);
                                 return true;
                                 }
                              }
                           else
                              break; // PAT/PMT is always in one sequence
                           }
                        else {
                           close(fd);
                           return false;
                           }
                        }
                  pos = lseek(fd, pos - TS_SIZE, SEEK_SET);
                  }
            close(fd);
            }
         else
            break;
         }
     }
  return false;
}

cUnbufferedFile *cFileName::Open(void)
{
  if (!file) {

#ifdef USE_DIRECT_IO
     int BlockingFlag = direct ? O_DIRECT : (blocking ? 0 : O_NONBLOCK);
#else
     int BlockingFlag = blocking ? 0 : O_NONBLOCK;
#endif
//M7X0 END AK
     if (record) {
        dsyslog("recording to '%s'", fileName);
//M7X0 BEGIN AK
#ifdef USE_DIRECT_IO
        file = OpenVideoFile(fileName, O_WRONLY | O_CREAT | O_LARGEFILE | BlockingFlag);
#else
        file = OpenVideoFile(fileName, O_RDWR | O_CREAT | O_LARGEFILE | BlockingFlag);
#endif
//M7X0 END AK
        if (!file)
           LOG_ERROR_STR(fileName);
        }
     else {
        if (access(fileName, R_OK) == 0) {
           dsyslog("playing '%s'", fileName);
           file = cUnbufferedFile::Create(fileName, O_RDONLY | O_LARGEFILE | BlockingFlag);
           if (!file)
              LOG_ERROR_STR(fileName);
           }
        else if (errno != ENOENT)
           LOG_ERROR_STR(fileName);
        }
     }
  return file;
}

void cFileName::Close(void)
{
  if (file) {
     if (CloseVideoFile(file) < 0)
        LOG_ERROR_STR(fileName);
     file = NULL;
     }
}

//M7X0 BEGIN AK
int cFileName::Unlink(void)
{
  Close();
  return RemoveSingleVideoFile(fileName);
}
//M7X0 END AK
cUnbufferedFile *cFileName::SetOffset(int Number, off_t Offset)
{
  if (fileNumber != Number)
     Close();
  int MaxFilesPerRecording = isPesRecording ? MAXFILESPERRECORDINGPES : MAXFILESPERRECORDINGTS;
  if (0 < Number && Number <= MaxFilesPerRecording) {
     fileNumber = uint16_t(Number);
     sprintf(pFileNumber, isPesRecording ? RECORDFILESUFFIXPES : RECORDFILESUFFIXTS, fileNumber);
     if (record) {
        if (access(fileName, F_OK) == 0) {
           // file exists, check if it has non-zero size
           struct stat buf;
           if (stat(fileName, &buf) == 0) {
              if (buf.st_size != 0)
                 return SetOffset(Number + 1); // file exists and has non zero size, let's try next suffix
              else {
                 // zero size file, remove it
                 dsyslog("cFileName::SetOffset: removing zero-sized file %s", fileName);
                 unlink(fileName);
                 }
              }
//M7X0 BEGIN AK
           else {
              LOG_ERROR_STR(fileName);
              return SetOffset(Number + 1); // error with fstat - should not happen, just to be on the safe side
              }
//M7X0 END AK
           }
        else if (errno != ENOENT) { // something serious has happened
           LOG_ERROR_STR(fileName);
           return NULL;
           }
        // found a non existing file suffix
        }
     if (Open()) {
        if (!record && Offset >= 0 && file->Seek(Offset, SEEK_SET) != Offset) {
           LOG_ERROR_STR(fileName);
           return NULL;
           }
        }
     return file;
     }
  esyslog("ERROR: max number of files (%d) exceeded", MaxFilesPerRecording);
  return NULL;
}

off_t cFileName::MaxFileSize() {
  const int maxVideoFileSize = isPesRecording ? MAXVIDEOFILESIZEPES : MAXVIDEOFILESIZETS;
  const int setupMaxVideoFileSize = std::min(maxVideoFileSize, Setup.MaxVideoFileSize);
  const int maxFileNumber = isPesRecording ? MAXFILESPERRECORDINGPES : MAXFILESPERRECORDINGTS;

  const off_t smallFiles = (maxFileNumber * off_t(maxVideoFileSize) - 1024 * Setup.MaxRecordingSize)
                           / std::max(maxVideoFileSize - setupMaxVideoFileSize, 1);

  if (fileNumber <= smallFiles)
     return MEGABYTE(off_t(setupMaxVideoFileSize));
  
  return MEGABYTE(off_t(maxVideoFileSize));
}

cUnbufferedFile *cFileName::NextFile(void)
{
  return SetOffset(fileNumber + 1);
}

// --- Index stuff -----------------------------------------------------------

cString IndexToHMSF(int Index, bool WithFrame)
{
  int f = (Index % FRAMESPERSEC) + 1;
  int s = (Index / FRAMESPERSEC);
  int m = s / 60 % 60;
  int h = s / 3600;
  s %= 60;
  return cString::sprintf(WithFrame ? "%d:%02d:%02d.%02d" : "%d:%02d:%02d", h, m, s, f);
}

int HMSFToIndex(const char *HMSF)
{
  int h, m, s, f = 0;
  if (3 <= sscanf(HMSF, "%d:%d:%d.%d", &h, &m, &s, &f))
     return (h * 3600 + m * 60 + s) * FRAMESPERSEC + f - 1;
  return 0;
}

int SecondsToFrames(int Seconds)
{
  return Seconds * FRAMESPERSEC;
}

// --- ReadFrame -------------------------------------------------------------

int ReadFrame(cUnbufferedFile *f, uchar *b, int Length, int Max)
{
  if (Length == -1)
     Length = Max; // this means we read up to EOF (see cIndex)
  else if (Length > Max) {
     esyslog("ERROR: frame larger than buffer (%d > %d)", Length, Max);
     Length = Max;
     }
  int r = f->Read(b, Length);
  if (r < 0)
     LOG_ERROR;
  return r;
}
