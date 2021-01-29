/*
 * config.c: Configuration file handling
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "config.h"
#include <ctype.h>
#include <stdlib.h>
#include "device.h"
#include "i18n.h"
#include "interface.h"
#include "plugin.h"
#include "recording.h"

// IMPORTANT NOTE: in the 'sscanf()' calls there is a blank after the '%d'
// format characters in order to allow any number of blanks after a numeric
// value!

// --- cSVDRPhost ------------------------------------------------------------

cSVDRPhost::cSVDRPhost(void)
{
  addr.s_addr = 0;
  mask = 0;
}

bool cSVDRPhost::Parse(const char *s)
{
  mask = 0xFFFFFFFF;
  const char *p = strchr(s, '/');
  if (p) {
     char *error = NULL;
     int m = strtoul(p + 1, &error, 10);
     if ((error && *error && !isspace(*error)) || m > 32)
        return false;
     *(char *)p = 0; // yes, we know it's 'const' - will be restored!
     if (m == 0)
        mask = 0;
     else {
        mask <<= (32 - m);
        mask = htonl(mask);
        }
     }
  int result = inet_aton(s, &addr);
  if (p)
     *(char *)p = '/'; // there it is again
  return result != 0 && (mask != 0 || addr.s_addr == 0);
}

bool cSVDRPhost::IsLocalhost(void)
{
  return addr.s_addr == htonl(INADDR_LOOPBACK);
}

bool cSVDRPhost::Accepts(in_addr_t Address)
{
  return (Address & mask) == (addr.s_addr & mask);
}

// --- cNestedItem -----------------------------------------------------------

cNestedItem::cNestedItem(const char *Text, bool WithSubItems)
{
  text = strdup(Text ? Text : "");
  subItems = WithSubItems ? new cList<cNestedItem> : NULL;
}

cNestedItem::~cNestedItem()
{
  delete subItems;
  free(text);
}

int cNestedItem::Compare(const cListObject &ListObject) const
{
  return strcasecmp(text, ((cNestedItem *)&ListObject)->text);
}

void cNestedItem::AddSubItem(cNestedItem *Item)
{
  if (!subItems)
     subItems = new cList<cNestedItem>;
  if (Item)
     subItems->Add(Item);
}

void cNestedItem::SetText(const char *Text)
{
  free(text);
  text = strdup(Text ? Text : "");
}

void cNestedItem::SetSubItems(bool On)
{
  if (On && !subItems)
     subItems = new cList<cNestedItem>;
  else if (!On && subItems) {
     delete subItems;
     subItems = NULL;
     }
}

// --- cNestedItemList -------------------------------------------------------

cNestedItemList::cNestedItemList(void)
{
  fileName = NULL;
}

cNestedItemList::~cNestedItemList()
{
  free(fileName);
}

bool cNestedItemList::Parse(FILE *f, cList<cNestedItem> *List, int &Line)
{
  char *s;
  cReadLine ReadLine;
  while ((s = ReadLine.Read(f)) != NULL) {
        Line++;
        char *p = strchr(s, '#');
        if (p)
           *p = 0;
        s = skipspace(stripspace(s));
        if (!isempty(s)) {
           p = s + strlen(s) - 1;
           if (*p == '{') {
              *p = 0;
              stripspace(s);
              cNestedItem *Item = new cNestedItem(s, true);
              List->Add(Item);
              if (!Parse(f, Item->SubItems(), Line))
                 return false;
              }
           else if (*s == '}')
              break;
           else
              List->Add(new cNestedItem(s));
           }
        }
  return true;
}

bool cNestedItemList::Write(FILE *f, cList<cNestedItem> *List, int Indent)
{
  for (cNestedItem *Item = List->First(); Item; Item = List->Next(Item)) {
      if (Item->SubItems()) {
         fprintf(f, "%*s%s {\n", Indent, "", Item->Text());
         Write(f, Item->SubItems(), Indent + 2);
         fprintf(f, "%*s}\n", Indent + 2, "");
         }
      else
         fprintf(f, "%*s%s\n", Indent, "", Item->Text());
      }
  return true;
}

void cNestedItemList::Clear(void)
{
  free(fileName);
  fileName = NULL;
  cList<cNestedItem>::Clear();
}

bool cNestedItemList::Load(const char *FileName)
{
  cList<cNestedItem>::Clear();
  if (FileName) {
     free(fileName);
     fileName = strdup(FileName);
     }
  bool result = false;
  if (fileName && access(fileName, F_OK) == 0) {
     isyslog("loading %s", fileName);
     FILE *f = fopen(fileName, "r");
     if (f) {
        int Line = 0;
        result = Parse(f, this, Line);
        fclose(f);
        }
     else {
        LOG_ERROR_STR(fileName);
        result = false;
        }
     }
  return result;
}

bool cNestedItemList::Save(void)
{
  bool result = true;
  cSafeFile f(fileName);
  if (f.Open()) {
     result = Write(f, this);
     if (!f.Close())
        result = false;
     }
  else
     result = false;
  return result;
}

// --- Folders and Commands --------------------------------------------------

cNestedItemList Folders;
cNestedItemList Commands;
cNestedItemList RecordingCommands;
cNestedItemList TimerCommands;

// --- cSVDRPhosts -----------------------------------------------------------

cSVDRPhosts SVDRPhosts;

bool cSVDRPhosts::LocalhostOnly(void)
{
  cSVDRPhost *h = First();
  while (h) {
        if (!h->IsLocalhost())
           return false;
        h = (cSVDRPhost *)h->Next();
        }
  return true;
}

bool cSVDRPhosts::Acceptable(in_addr_t Address)
{
  cSVDRPhost *h = First();
  while (h) {
        if (h->Accepts(Address))
           return true;
        h = (cSVDRPhost *)h->Next();
        }
  return false;
}

// --- cSetupLine ------------------------------------------------------------

cSetupLine::cSetupLine(void)
{
  plugin = name = value = NULL;
}

cSetupLine::cSetupLine(const char *Name, const char *Value, const char *Plugin)
{
  name = strreplace(strdup(Name), '\n', 0);
  value = strreplace(strdup(Value), '\n', 0);
  plugin = Plugin ? strreplace(strdup(Plugin), '\n', 0) : NULL;
}

cSetupLine::~cSetupLine()
{
  free(plugin);
  free(name);
  free(value);
}

int cSetupLine::Compare(const cListObject &ListObject) const
{
  const cSetupLine *sl = (cSetupLine *)&ListObject;
  if (!plugin && !sl->plugin)
     return strcasecmp(name, sl->name);
  if (!plugin)
     return -1;
  if (!sl->plugin)
     return 1;
  int result = strcasecmp(plugin, sl->plugin);
  if (result == 0)
     result = strcasecmp(name, sl->name);
  return result;
}

bool cSetupLine::Parse(char *s)
{
  char *p = strchr(s, '=');
  if (p) {
     *p = 0;
     char *Name  = compactspace(s);
     char *Value = compactspace(p + 1);
     if (*Name) { // value may be an empty string
        p = strchr(Name, '.');
        if (p) {
           *p = 0;
           char *Plugin = compactspace(Name);
           Name = compactspace(p + 1);
           if (!(*Plugin && *Name))
              return false;
           plugin = strdup(Plugin);
           }
        name = strdup(Name);
        value = strdup(Value);
        return true;
        }
     }
  return false;
}

bool cSetupLine::Save(FILE *f)
{
  return fprintf(f, "%s%s%s = %s\n", plugin ? plugin : "", plugin ? "." : "", name, value) > 0;
}

// --- cSetup ----------------------------------------------------------------

cSetup Setup;

cSetup::cSetup(void)
{
  OSDLanguage = 0;
  strcpy(OSDSkin, "sttng");
  strcpy(OSDTheme, "default");
  WarEagleIcons = 1;
  PrimaryDVB = 1;
  ShowInfoOnChSwitch = 1;
  TimeoutRequChInfo = 1;
  MenuScrollPage = 1;
  MenuScrollWrap = 0;
  MenuButtonCloses = 0;
  MarkInstantRecord = 1;
  strcpy(NameInstantRecord, "TITLE EPISODE");
  InstantRecordTime = 180;
#ifdef M750S
  LnbSLOF    = 11700;
  LnbFrequLo =  9750;
  LnbFrequHi = 10600;
  DiSEqC = 0;
#endif
  SetSystemTime = 0;
  TimeSource = 0;
  TimeTransponder = 0;
  MarginStart = 2;
  MarginStop = 10;
  AudioLanguages[0] = -1;
  EPGLanguages[0] = -1;
  EPGScanTimeout = 5;
  EPGBugfixLevel = 3;
  EPGLinger = 0;
//M7X0 BEGIN AK
  EPGDefaultMode = 0;
  EPGInvDefaultMode = 2;
  EPGChannelNoLimit = 99;
//M7X0 END AK
  SVDRPTimeout = 300;
  ZapTimeout = 3;
  ChannelEntryTimeout = 1000;
  PrimaryLimit = 0;
  DefaultPriority = 50;
  DefaultLifetime = 99;
  PauseKeyHandling = 2;
  PausePriority = 10;
  PauseLifetime = 1;
  UseSubtitle = 1;
  UseVps = 0;
  VpsMargin = 120;
  RecordingDirs = 1;
  FoldersInTimerMenu = 1;
  RecordingsSortMode = 0;
  RecordingsSortDirsFirst = 0;
  CutterAutoDelete = 1;
  CutterMaxBandwidth = 10;
  CutterRelBandwidth = 75;
  CutterWithMarks = 0;
  HotStandby = 0;
  TvMode = 0;
  VCRMode = 0;
  VideoDisplayFormat = 1;
  VideoFormat = 0;
  VideoWithoutSignal = 0;
  UpdateChannels = 5;
  UseDolbyDigital = 1;
  UseDolbyInRecordings = 1;
  UseHDInRecordings = 1;
  UseTSInHD = 1;
  UseTSInSD = 0;
  UseTSInAudio = 0;
  UseTeletextInTSRecordings = 0;
  UseSubtitlesInTSRecordings = 1;
  ChannelInfoPos = 0;
  ChannelInfoTime = 5;
  OSDLeft = 54;
  OSDTop = 45;
  OSDWidth = 624;
  OSDHeight = 486;
  OSDMessageTime = 1;
  UseSmallFont = 1;
  MaxVideoFileSize = MAXVIDEOFILESIZEDEFAULT;
  MaxRecordingSize = DEFAULTRECORDINGSIZE;
  SplitEditedFiles = 0;
  DelTimeshiftRec = 0;
  HardLinkCutter = 0;
  MinEventTimeout = 30;
  MinUserInactivity = 300;
  MultiSpeedMode = 0;
  ShowReplayMode = 0;
  ResumeID = 0;
  CurrentChannel = -1;
  CurrentVolume = MAXVOLUME;
  CurrentDolby = 0;
  ChannelFilter = 0;
  InitialChannel = 0;
  InitialVolume = -1;
  TShift = 0;
  TShiftBufferSize = 60;
  TShiftStartRecord = 120;
  TShiftDelayed = 0;
  TShiftPriority = 0;
  TShiftPause = 1;
  TShiftTimeout = 60;
  LRChannelGroups = 0;
  LRForwardRewind = 0;
  ShowRecDate = 1;
  ShowRecTime = 1;
  ShowRecLength = 1;
  ShowProgressBar = 1;
  MenuCmdPosition = 1;
  JumpSeconds = 60;
  JumpSecondsRepeat = 120;
  JumpFramesRepeat = 15;
#ifdef USE_DIRECT_IO
  ReplayUseDirectIO = 1;
#endif
  ShutdownMessage = 0;
  AbortWhenPluginFails = 0;
  EmergencyExit = 0;
  LircRepeatDelay = 350;
  LircRepeatFreq = 100;
  LircRepeatTimeout = 500;
  CapitalizeFilenames = 0; // default = disabled
  UseVidPrefer = 0;        // default = disabled
  nVidPrefer = 1;
  for (int zz = 1; zz < DVLVIDPREFER_MAX; zz++) {
      VidPreferPrio[ zz ] = 50;
      VidPreferSize[ zz ] = 100;
      }
  VidPreferSize[ 0 ] = 800;
  VidPreferPrio[ 0 ] = 50;
}

cSetup& cSetup::operator= (const cSetup &s)
{
  memcpy(&__BeginData__, &s.__BeginData__, (char *)&s.__EndData__ - (char *)&s.__BeginData__);
  return *this;
}

cSetupLine *cSetup::Get(const char *Name, const char *Plugin)
{
  for (cSetupLine *l = First(); l; l = Next(l)) {
      if ((l->Plugin() == NULL) == (Plugin == NULL)) {
         if ((!Plugin || strcasecmp(l->Plugin(), Plugin) == 0) && strcasecmp(l->Name(), Name) == 0)
            return l;
         }
      }
  return NULL;
}

void cSetup::Store(const char *Name, const char *Value, const char *Plugin, bool AllowMultiple)
{
  if (Name && *Name) {
     cSetupLine *l = Get(Name, Plugin);
     if (l && !AllowMultiple)
        Del(l);
     if (Value)
        Add(new cSetupLine(Name, Value, Plugin));
     }
}

void cSetup::Store(const char *Name, int Value, const char *Plugin)
{
  Store(Name, cString::sprintf("%d", Value), Plugin);
}

bool cSetup::Load(const char *FileName)
{
  if (cConfig<cSetupLine>::Load(FileName)) {
     bool result = true;
     for (cSetupLine *l = First(); l; l = Next(l)) {
         bool error = false;
         if (l->Plugin()) {
            cPlugin *p = cPluginManager::GetPlugin(l->Plugin());
            if (p && !p->SetupParse(l->Name(), l->Value()))
               error = true;
            }
         else {
            if (!Parse(l->Name(), l->Value()))
               error = true;
            }
         if (error) {
            esyslog("ERROR: unknown config parameter: %s%s%s = %s", l->Plugin() ? l->Plugin() : "", l->Plugin() ? "." : "", l->Name(), l->Value());
            result = false;
            }
         }
     return result;
     }
  return false;
}

void cSetup::StoreLanguages(const char *Name, int *Values)
{
  char buffer[I18nNumLanguages * 4];
  char *q = buffer;
  for (int i = 0; i < I18nNumLanguages; i++) {
      if (Values[i] < 0)
         break;
      const char *s = I18nLanguageCode(Values[i]);
      if (s) {
         if (q > buffer)
            *q++ = ' ';
         strncpy(q, s, 3);
         q += 3;
         }
      }
  *q = 0;
  Store(Name, buffer);
}

bool cSetup::ParseLanguages(const char *Value, int *Values)
{
  int n = 0;
  while (Value && *Value && n < I18nNumLanguages) {
        char buffer[4];
        strn0cpy(buffer, Value, sizeof(buffer));
        int i = I18nLanguageIndex(buffer);
        if (i >= 0)
           Values[n++] = i;
        if ((Value = strchr(Value, ' ')) != NULL)
           Value++;
        }
  Values[n] = -1;
  return true;
}

bool cSetup::Parse(const char *Name, const char *Value)
{
  if      (!strcasecmp(Name, "OSDLanguage"))         OSDLanguage        = atoi(Value);
  else if (!strcasecmp(Name, "OSDSkin"))             strn0cpy(OSDSkin, Value, MaxSkinName);
  else if (!strcasecmp(Name, "OSDTheme"))            strn0cpy(OSDTheme, Value, MaxThemeName);
  else if (!strcasecmp(Name, "WarEagleIcons"))       WarEagleIcons      = atoi(Value);
  else if (!strcasecmp(Name, "PrimaryDVB"))          PrimaryDVB         = atoi(Value);
  else if (!strcasecmp(Name, "ShowInfoOnChSwitch"))  ShowInfoOnChSwitch = atoi(Value);
  else if (!strcasecmp(Name, "TimeoutRequChInfo"))   TimeoutRequChInfo  = atoi(Value);
  else if (!strcasecmp(Name, "MenuScrollPage"))      MenuScrollPage     = atoi(Value);
  else if (!strcasecmp(Name, "MenuScrollWrap"))      MenuScrollWrap     = atoi(Value);
  else if (!strcasecmp(Name, "MenuButtonCloses"))    MenuButtonCloses   = atoi(Value);
  else if (!strcasecmp(Name, "MarkInstantRecord"))   MarkInstantRecord  = atoi(Value);
  else if (!strcasecmp(Name, "NameInstantRecord"))   strn0cpy(NameInstantRecord, Value, sizeof(NameInstantRecord));
  else if (!strcasecmp(Name, "InstantRecordTime"))   InstantRecordTime  = atoi(Value);
#ifdef M750S
  else if (!strcasecmp(Name, "LnbSLOF"))             LnbSLOF            = atoi(Value);
  else if (!strcasecmp(Name, "LnbFrequLo"))          LnbFrequLo         = atoi(Value);
  else if (!strcasecmp(Name, "LnbFrequHi"))          LnbFrequHi         = atoi(Value);
  else if (!strcasecmp(Name, "DiSEqC"))              DiSEqC             = atoi(Value);
#endif
  else if (!strcasecmp(Name, "SetSystemTime"))       SetSystemTime      = atoi(Value);
  else if (!strcasecmp(Name, "TimeSource"))          TimeSource         = cSource::FromString(Value);
  else if (!strcasecmp(Name, "TimeTransponder"))     TimeTransponder    = atoi(Value);
  else if (!strcasecmp(Name, "MarginStart"))         MarginStart        = atoi(Value);
  else if (!strcasecmp(Name, "MarginStop"))          MarginStop         = atoi(Value);
  else if (!strcasecmp(Name, "AudioLanguages"))      return ParseLanguages(Value, AudioLanguages);
  else if (!strcasecmp(Name, "EPGLanguages"))        return ParseLanguages(Value, EPGLanguages);
  else if (!strcasecmp(Name, "EPGScanTimeout"))      EPGScanTimeout     = atoi(Value);
  else if (!strcasecmp(Name, "EPGBugfixLevel"))      EPGBugfixLevel     = atoi(Value);
  else if (!strcasecmp(Name, "EPGLinger"))           EPGLinger          = atoi(Value);
//M7X0 BEGIN AK
  else if (!strcasecmp(Name, "EPGDefaultMode"))      EPGDefaultMode     = atoi(Value);
  else if (!strcasecmp(Name, "EPGInvDefaultMode"))   EPGInvDefaultMode  = atoi(Value);
  else if (!strcasecmp(Name, "EPGChannelNoLimit"))   EPGChannelNoLimit  = atoi(Value);
//M7X0 END AK
  else if (!strcasecmp(Name, "SVDRPTimeout"))        SVDRPTimeout       = atoi(Value);
  else if (!strcasecmp(Name, "ZapTimeout"))          ZapTimeout         = atoi(Value);
  else if (!strcasecmp(Name, "ChannelEntryTimeout")) ChannelEntryTimeout= atoi(Value);
  else if (!strcasecmp(Name, "PrimaryLimit"))        PrimaryLimit       = atoi(Value);
  else if (!strcasecmp(Name, "DefaultPriority"))     DefaultPriority    = atoi(Value);
  else if (!strcasecmp(Name, "DefaultLifetime"))     DefaultLifetime    = atoi(Value);
  else if (!strcasecmp(Name, "PauseKeyHandling"))    PauseKeyHandling   = atoi(Value);
  else if (!strcasecmp(Name, "PausePriority"))       PausePriority      = atoi(Value);
  else if (!strcasecmp(Name, "PauseLifetime"))       PauseLifetime      = atoi(Value);
  else if (!strcasecmp(Name, "UseSubtitle"))         UseSubtitle        = atoi(Value);
  else if (!strcasecmp(Name, "UseVps"))              UseVps             = atoi(Value);
  else if (!strcasecmp(Name, "VpsMargin"))           VpsMargin          = atoi(Value);
  else if (!strcasecmp(Name, "RecordingDirs"))       RecordingDirs      = atoi(Value);
  else if (!strcasecmp(Name, "FoldersInTimerMenu"))  FoldersInTimerMenu = atoi(Value);
  else if (!strcasecmp(Name, "RecordingsSortMode"))  RecordingsSortMode = atoi(Value);
  else if (!strcasecmp(Name, "RecordingsSortDirsFirst")) RecordingsSortDirsFirst = atoi(Value);
  else if (!strcasecmp(Name, "CutterAutoDelete"))    CutterAutoDelete   = atoi(Value);
  else if (!strcasecmp(Name, "CutterMaxBandwidth"))  CutterMaxBandwidth = atoi(Value);
  else if (!strcasecmp(Name, "CutterRelBandwidth"))  CutterRelBandwidth = atoi(Value);
  else if (!strcasecmp(Name, "CutterWithMarks"))     CutterWithMarks    = atoi(Value);
  else if (!strcasecmp(Name, "HotStandby"))          HotStandby         = atoi(Value);
  else if (!strcasecmp(Name, "TvMode"))              TvMode             = atoi(Value);
  else if (!strcasecmp(Name, "VCRMode"))             VCRMode            = atoi(Value);
  else if (!strcasecmp(Name, "VideoDisplayFormat"))  VideoDisplayFormat = atoi(Value);
  else if (!strcasecmp(Name, "VideoFormat"))         VideoFormat        = atoi(Value);
  else if (!strcasecmp(Name, "VideoWithoutSignal"))  VideoWithoutSignal = atoi(Value);
  else if (!strcasecmp(Name, "UpdateChannels"))      UpdateChannels     = atoi(Value);
  else if (!strcasecmp(Name, "UseDolbyDigital"))     UseDolbyDigital    = atoi(Value);
  else if (!strcasecmp(Name, "UseDolbyInRecordings")) UseDolbyInRecordings = atoi(Value);
  else if (!strcasecmp(Name, "UseHDInRecordings")) UseHDInRecordings = atoi(Value);
  else if (!strcasecmp(Name, "UseTSInHD"))           UseTSInHD          = atoi(Value);
  else if (!strcasecmp(Name, "UseTSInSD"))           UseTSInSD          = atoi(Value);
  else if (!strcasecmp(Name, "UseTSInAudio"))        UseTSInAudio       = atoi(Value);
  else if (!strcasecmp(Name, "UseTeletextInTSRecordings")) UseTeletextInTSRecordings = atoi(Value);
  else if (!strcasecmp(Name, "UseSubtitlesInTSRecordings")) UseSubtitlesInTSRecordings = atoi(Value);
  else if (!strcasecmp(Name, "ChannelInfoPos"))      ChannelInfoPos     = atoi(Value);
  else if (!strcasecmp(Name, "ChannelInfoTime"))     ChannelInfoTime    = atoi(Value);
  else if (!strcasecmp(Name, "OSDLeft"))             OSDLeft            = atoi(Value);
  else if (!strcasecmp(Name, "OSDTop"))              OSDTop             = atoi(Value);
  else if (!strcasecmp(Name, "OSDWidth"))          { OSDWidth           = atoi(Value); if (OSDWidth  < 100) OSDWidth  *= 12; OSDWidth &= ~0x07; } // OSD width must be a multiple of 8
  else if (!strcasecmp(Name, "OSDHeight"))         { OSDHeight          = atoi(Value); if (OSDHeight < 100) OSDHeight *= 27; }
  else if (!strcasecmp(Name, "OSDMessageTime"))      OSDMessageTime     = atoi(Value);
  else if (!strcasecmp(Name, "UseSmallFont"))        UseSmallFont       = atoi(Value);
  else if (!strcasecmp(Name, "MaxVideoFileSize"))    MaxVideoFileSize   = atoi(Value);
  else if (!strcasecmp(Name, "MaxRecordingSize"))    MaxRecordingSize   = atoi(Value);
  else if (!strcasecmp(Name, "SplitEditedFiles"))    SplitEditedFiles   = atoi(Value);
  else if (!strcasecmp(Name, "DelTimeshiftRec"))     DelTimeshiftRec    = atoi(Value);
  else if (!strcasecmp(Name, "HardLinkCutter"))      HardLinkCutter     = atoi(Value);
  else if (!strcasecmp(Name, "MinEventTimeout"))     MinEventTimeout    = atoi(Value);
  else if (!strcasecmp(Name, "MinUserInactivity"))   MinUserInactivity  = atoi(Value);
  else if (!strcasecmp(Name, "MultiSpeedMode"))      MultiSpeedMode     = atoi(Value);
  else if (!strcasecmp(Name, "ShowReplayMode"))      ShowReplayMode     = atoi(Value);
  else if (!strcasecmp(Name, "ResumeID"))            ResumeID           = atoi(Value);
  else if (!strcasecmp(Name, "CurrentChannel"))      CurrentChannel     = atoi(Value);
  else if (!strcasecmp(Name, "CurrentVolume"))       CurrentVolume      = atoi(Value);
  else if (!strcasecmp(Name, "CurrentDolby"))        CurrentDolby       = atoi(Value);
  else if (!strcasecmp(Name, "ChannelFilter"))       ChannelFilter      = atoi(Value);
  else if (!strcasecmp(Name, "InitialChannel"))      InitialChannel     = atoi(Value);
  else if (!strcasecmp(Name, "InitialVolume"))       InitialVolume      = atoi(Value);
  else if (!strcasecmp(Name, "TShift"))              TShift             = atoi(Value);
  else if (!strcasecmp(Name, "TShiftBufferSize"))    TShiftBufferSize   = atoi(Value);
  else if (!strcasecmp(Name, "TShiftStartRecord"))   TShiftStartRecord  = atoi(Value);
  else if (!strcasecmp(Name, "TShiftDelayed"))       TShiftDelayed      = atoi(Value);
  else if (!strcasecmp(Name, "TShiftPriority"))      TShiftPriority     = atoi(Value);
  else if (!strcasecmp(Name, "TShiftPause"))         TShiftPause        = atoi(Value);
  else if (!strcasecmp(Name, "TShiftTimeout"))       TShiftTimeout      = atoi(Value);
  else if (!strcasecmp(Name, "LRChannelGroups"))     LRChannelGroups    = atoi(Value);
  else if (!strcasecmp(Name, "LRForwardRewind"))     LRForwardRewind    = atoi(Value);
  else if (!strcasecmp(Name, "ShowRecDate"))         ShowRecDate        = atoi(Value);
  else if (!strcasecmp(Name, "ShowRecTime"))         ShowRecTime        = atoi(Value);
  else if (!strcasecmp(Name, "ShowRecLength"))       ShowRecLength      = atoi(Value);
  else if (!strcasecmp(Name, "ShowProgressBar"))     ShowProgressBar    = atoi(Value);
  else if (!strcasecmp(Name, "MenuCmdPosition"))     MenuCmdPosition    = atoi(Value);
  else if (!strcasecmp(Name, "JumpSeconds"))         JumpSeconds        = atoi(Value);
  else if (!strcasecmp(Name, "JumpSecondsRepeat"))   JumpSecondsRepeat  = atoi(Value);
  else if (!strcasecmp(Name, "JumpFramesRepeat"))    JumpFramesRepeat   = atoi(Value);
#ifdef USE_DIRECT_IO
  else if (!strcasecmp(Name, "ReplayUseDirectIO"))   ReplayUseDirectIO  = atoi(Value);
#endif
  else if (!strcasecmp(Name, "ShutdownMessage"))     ShutdownMessage    = atoi(Value);
  else if (!strcasecmp(Name, "AbortWhenPluginFails")) AbortWhenPluginFails = atoi(Value);
  else if (!strcasecmp(Name, "EmergencyExit"))       EmergencyExit      = atoi(Value);
  else if (!strcasecmp(Name, "LircRepeatDelay"))     LircRepeatDelay    = atoi(Value);
  else if (!strcasecmp(Name, "LircRepeatFreq"))      LircRepeatFreq     = atoi(Value);
  else if (!strcasecmp(Name, "LircRepeatTimeout"))   LircRepeatTimeout  = atoi(Value);
  else if (!strcasecmp(Name, "CapitalizeFilenames")) CapitalizeFilenames = atoi(Value);
  else if (!strcasecmp(Name, "UseVidPrefer"))        UseVidPrefer       = atoi(Value);
  else if (!strcasecmp(Name, "nVidPrefer"))          nVidPrefer         = atoi(Value);
  else if (strstr(Name, "VidPrefer") == Name) {
     char *x = (char *)&Name[ strlen(Name) - 1 ];
     int vN;

     if (isdigit(*x) != 0) {
        while (isdigit(*x) != 0)
              x--;
        x++;
        }

     vN = atoi(x);
     if (vN < DVLVIDPREFER_MAX) {
        if (strstr(Name, "VidPreferPrio") == Name) {
           VidPreferPrio[ vN ] = atoi(Value);
           if (VidPreferPrio[ vN ] > 99)
              VidPreferPrio[ vN ] = 99;
           }
        else if (strstr(Name, "VidPreferSize") == Name) {
           VidPreferSize[ vN ] = atoi(Value);
           }
        else
           return false;
        }
     }
  else
     return false;
  return true;
}

bool cSetup::Save(void)
{
  Store("OSDLanguage",        OSDLanguage);
  Store("OSDSkin",            OSDSkin);
  Store("OSDTheme",           OSDTheme);
  Store("WarEagleIcons",      WarEagleIcons);
  Store("PrimaryDVB",         PrimaryDVB);
  Store("ShowInfoOnChSwitch", ShowInfoOnChSwitch);
  Store("TimeoutRequChInfo",  TimeoutRequChInfo);
  Store("MenuScrollPage",     MenuScrollPage);
  Store("MenuScrollWrap",     MenuScrollWrap);
  Store("MenuButtonCloses",   MenuButtonCloses);
  Store("MarkInstantRecord",  MarkInstantRecord);
  Store("NameInstantRecord",  NameInstantRecord);
  Store("InstantRecordTime",  InstantRecordTime);
#ifdef M750S
  Store("LnbSLOF",            LnbSLOF);
  Store("LnbFrequLo",         LnbFrequLo);
  Store("LnbFrequHi",         LnbFrequHi);
  Store("DiSEqC",             DiSEqC);
#endif
  Store("SetSystemTime",      SetSystemTime);
  Store("TimeSource",         cSource::ToString(TimeSource));
  Store("TimeTransponder",    TimeTransponder);
  Store("MarginStart",        MarginStart);
  Store("MarginStop",         MarginStop);
  StoreLanguages("AudioLanguages", AudioLanguages);
  StoreLanguages("EPGLanguages", EPGLanguages);
  Store("EPGScanTimeout",     EPGScanTimeout);
  Store("EPGBugfixLevel",     EPGBugfixLevel);
  Store("EPGLinger",          EPGLinger);
//M7X0 BEGIN AK
  Store("EPGDefaultMode",     EPGDefaultMode);
  Store("EPGInvDefaultMode",  EPGInvDefaultMode);
  Store("EPGChannelNoLimit",  EPGChannelNoLimit);
//M7X0 END AK
  Store("SVDRPTimeout",       SVDRPTimeout);
  Store("ZapTimeout",         ZapTimeout);
  Store("ChannelEntryTimeout",ChannelEntryTimeout);
  Store("PrimaryLimit",       PrimaryLimit);
  Store("DefaultPriority",    DefaultPriority);
  Store("DefaultLifetime",    DefaultLifetime);
  Store("PauseKeyHandling",   PauseKeyHandling);
  Store("PausePriority",      PausePriority);
  Store("PauseLifetime",      PauseLifetime);
  Store("UseSubtitle",        UseSubtitle);
  Store("UseVps",             UseVps);
  Store("VpsMargin",          VpsMargin);
  Store("RecordingDirs",      RecordingDirs);
  Store("FoldersInTimerMenu", FoldersInTimerMenu);
  Store("RecordingsSortMode", RecordingsSortMode);
  Store("RecordingsSortDirsFirst", RecordingsSortDirsFirst);
  Store("CutterAutoDelete",   CutterAutoDelete);
  Store("CutterMaxBandwidth", CutterMaxBandwidth);
  Store("CutterRelBandwidth", CutterRelBandwidth);
  Store("CutterWithMarks",    CutterWithMarks);
  Store("HotStandby", 	      HotStandby);
  Store("TvMode", 	      TvMode);
  Store("VCRMode", 	      VCRMode);
  Store("VideoDisplayFormat", VideoDisplayFormat);
  Store("VideoFormat",        VideoFormat);
  Store("VideoWithoutSignal", VideoWithoutSignal);
  Store("UpdateChannels",     UpdateChannels);
  Store("UseDolbyDigital",    UseDolbyDigital);
  Store("UseDolbyInRecordings", UseDolbyInRecordings);
  Store("UseHDInRecordings", UseHDInRecordings);
  Store("UseTSInHD", UseTSInHD);
  Store("UseTSInSD", UseTSInSD);
  Store("UseTSInAudio", UseTSInAudio);
  Store("UseTeletextInTSRecordings", UseTeletextInTSRecordings);
  Store("UseSubtitlesInTSRecordings", UseSubtitlesInTSRecordings);
  Store("ChannelInfoPos",     ChannelInfoPos);
  Store("ChannelInfoTime",    ChannelInfoTime);
  Store("OSDLeft",            OSDLeft);
  Store("OSDTop",             OSDTop);
  Store("OSDWidth",           OSDWidth);
  Store("OSDHeight",          OSDHeight);
  Store("OSDMessageTime",     OSDMessageTime);
  Store("UseSmallFont",       UseSmallFont);
  Store("MaxVideoFileSize",   MaxVideoFileSize);
  Store("MaxRecordingSize",   MaxRecordingSize);
  Store("SplitEditedFiles",   SplitEditedFiles);
  Store("DelTimeshiftRec",    DelTimeshiftRec);
  Store("HardLinkCutter",     HardLinkCutter);
  Store("MinEventTimeout",    MinEventTimeout);
  Store("MinUserInactivity",  MinUserInactivity);
  Store("MultiSpeedMode",     MultiSpeedMode);
  Store("ShowReplayMode",     ShowReplayMode);
  Store("ResumeID",           ResumeID);
  Store("CurrentChannel",     CurrentChannel);
  Store("CurrentVolume",      CurrentVolume);
  Store("CurrentDolby",       CurrentDolby);
  Store("ChannelFilter",      ChannelFilter);
  Store("InitialChannel",     InitialChannel);
  Store("InitialVolume",      InitialVolume);
  Store("TShift",             TShift);
  Store("TShiftBufferSize",   TShiftBufferSize);
  Store("TShiftStartRecord",  TShiftStartRecord);
  Store("TShiftDelayed",      TShiftDelayed);
  Store("TShiftPriority",     TShiftPriority);
  Store("TShiftPause",        TShiftPause);
  Store("TShiftTimeout",      TShiftTimeout);
  Store("LRChannelGroups",    LRChannelGroups);
  Store("LRForwardRewind",    LRForwardRewind);
  Store("ShowRecDate",        ShowRecDate);
  Store("ShowRecTime",        ShowRecTime);
  Store("ShowRecLength",      ShowRecLength);
  Store("ShowProgressBar",    ShowProgressBar);
  Store("MenuCmdPosition",    MenuCmdPosition);
  Store("JumpSeconds",        JumpSeconds);
  Store("JumpSecondsRepeat",  JumpSecondsRepeat);
  Store("JumpFramesRepeat",   JumpFramesRepeat);
#ifdef USE_DIRECT_IO
  Store("ReplayUseDirectIO",  ReplayUseDirectIO);
#endif
  Store("ShutdownMessage",    ShutdownMessage);
  Store("AbortWhenPluginFails", AbortWhenPluginFails);
  Store("EmergencyExit",      EmergencyExit);
  Store("LircRepeatDelay",    LircRepeatDelay);
  Store("LircRepeatFreq",     LircRepeatFreq);
  Store("LircRepeatTimeout",  LircRepeatTimeout);
  Store("CapitalizeFilenames", CapitalizeFilenames);
  Store ("UseVidPrefer",      UseVidPrefer);
  Store ("nVidPrefer",        nVidPrefer);

  char vidBuf[32];
  for (int zz = 0; zz < nVidPrefer; zz++) {
      sprintf(vidBuf, "VidPreferPrio%d", zz);
      Store (vidBuf, VidPreferPrio[zz]);
      sprintf(vidBuf, "VidPreferSize%d", zz);
      Store (vidBuf, VidPreferSize[zz]);
      }

  Sort();

  if (cConfig<cSetupLine>::Save()) {
     isyslog("saved setup to %s", FileName());
     return true;
     }
  return false;
}
