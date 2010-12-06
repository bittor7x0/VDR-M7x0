/*
 *
 * See the main source file 'cridplayer.c' for copyright information and
 * how to reach the author.
 *
 * $Id: cridplayer_menu.c 13 2007-03-06 07:09:16Z andreas $
 *
 */

#include "cridplayer_menu.h"
#include "cridplayer_setup.h"
#include "cridplayer_controller.h"
#include <vdr/remote.h>
#include <vdr/interface.h>

// --- cCridMenuRecording ----------------------------------------------------
class cCridMenuRecording : public cOsdMenu {
private:
  const cCridRecording *rec;
  bool withButtons;
public:
  cCridMenuRecording(const cCridRecording *Rec, bool WithButtons = true);
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cCridMenuRecording::cCridMenuRecording(const cCridRecording *Rec, bool WithButtons)
 : cOsdMenu(tr("Recording info"))
{
  rec = Rec;
  withButtons = WithButtons;
  if (withButtons)
     SetHelp(tr("Button$Play"), tr("Button$Rewind"));
}
void cCridMenuRecording::Display(void)
{
  cOsdMenu::Display();
  DisplayMenu()->SetEvent(rec->Event());
}
eOSState cCridMenuRecording::ProcessKey(eKeys Key)
{
  switch (Key) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
                  DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
                  return osContinue;
    default: break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRed:    if (withButtons)
                        Key = kOk; // will play the recording
       case kGreen:  if (!withButtons)
                        break;
                     cRemote::Put(Key, true);
                     // continue with osBack to close the info menu and process the key
       case kOk:     return osBack;
       default: break;
       }
     }
  return state;
}
// --- cCridMenuItem ---------------------------------------------------------

enum eCridplayerMenuItemType  {
  CridplayerDirectory,
  CridplayerSeries,
  CridplayerRecording
  };

class cCridMenuItem : public cOsdItem {
private:
  eCridplayerMenuItemType type;
  const cPathObject *directory;
  cCridSeriesRecording *series;
  cCridRecording *rec;
public:
  cCridMenuItem(const cPathObject *Directory);
  cCridMenuItem(cCridSeriesRecording *Series);
  cCridMenuItem(cCridRecording *Rec);
  eCridplayerMenuItemType GetType(void) const { return type; }
  const cPathObject *GetDirectory(void) const { return directory; }
  cCridSeriesRecording *GetSeries(void) const { return series; }
  cCridRecording *GetRecording(void) const { return rec; }
  };

cCridMenuItem::cCridMenuItem(const cPathObject *Directory)
{
  type = CridplayerDirectory;
  series = NULL;
  rec = NULL;
  directory = Directory;
  char *text;
  if (asprintf(&text,"%s%s%s",CridplayerSetup.DirMarkerPre, Directory->Name(),
                                       CridplayerSetup.DirMarkerPost)  < 0) {
     esyslog("cCridMenuItem: Cannot alloc memory!");
     return;
     }
  SetText(text,false);
}

cCridMenuItem::cCridMenuItem(cCridSeriesRecording *Series)
{
  type = CridplayerSeries;
  series = Series;
  rec = NULL;
  directory = NULL;
  char *text;
  if (asprintf(&text,"%d\t%d\t%s",Series->Count(), Series->NewCount(),
                                                Series->Title())  < 0) {
     esyslog("cCridMenuItem: Cannot alloc memory!");
     return;
     }
  SetText(text, false);

}

cCridMenuItem::cCridMenuItem(cCridRecording *Rec)
{
  type = CridplayerRecording;
  series = NULL;
  rec = Rec;
  directory = NULL;
  char *text;

  const char New = Rec->IsNew() ? '*' : ' ';
  time_t start = Rec->StartTime();
  struct tm tm_r;
  struct tm *t = localtime_r(&start, &tm_r);

  if (asprintf(&text, "%02d.%02d.%02d\t%02d:%02d%c\t%s",
                      t->tm_mday,
                      t->tm_mon + 1,
                      t->tm_year % 100,
                      t->tm_hour,
                      t->tm_min,
                      New,
                      Rec->Title()) < 0 ) {
     esyslog("cCridMenuItem: Cannot alloc memory!");
     return;
     }

  SetText(text, false);
}

// --- cCridMenuRecordings ---------------------------------------------

cCridMenuRecordings::cCridMenuRecordings(cCridController *Controller, bool OpenSubMenu)
 : cOsdMenu(Controller->GetTitle(), 9, 7)
{
  controller = Controller;
  series = NULL;
  Set();
  if (OpenSubMenu)
     PlayOpen();

  Display();
  helpKeys = -1;
  SetHelpKeys();
}

cCridMenuRecordings::cCridMenuRecordings(cCridController *Controller, cCridSeriesRecording *Series)
 : cOsdMenu(Series->Title(), 9, 7)
{
  controller = Controller;
  series = Series;
  Set();
  Display();
  helpKeys = -1;
  SetHelpKeys();
}

cCridMenuRecordings::~cCridMenuRecordings()
{
  controller->SetSeriesSelected(series);
  controller->SetRecordingSelected(NULL);
  controller->SetPathSelected(NULL);

  cCridMenuItem *item = (cCridMenuItem *)Get(Current());
  if (!item)
     return;

  const eCridplayerMenuItemType type = item->GetType();
  if (type == CridplayerDirectory) {
     controller->SetPathSelected(item->GetDirectory());
     }
  else if(type == CridplayerSeries) {
     controller->SetSeriesSelected(item->GetSeries());
     }
  else {
     controller->SetRecordingSelected(item->GetRecording());
     }
}

void cCridMenuRecordings::Set(void) {
  Clear();
  if (series) {
     for (cCridRecording *rec = series->First(); rec ;
          rec = series->cList<cCridRecording>::Next(rec))
         Add(new cCridMenuItem(rec), rec == controller->GetRecordingSelected());
     }
  else {
     if (!controller->Read()) {
        SetStatus(tr("Cannot read directory"));
        return;
        }
     const cList<cPathObject> *dirs = controller->GetDirectories();
     for (const cPathObject *dir = dirs->First(); dir; dir = dirs->Next(dir))
         Add(new cCridMenuItem(dir), dir == controller->GetPathSelected());

     cCridRecordings *recs = controller->GetRecordings();
     cCridSeriesRecordings *srecs = controller->GetSeriesRecordings();

     cCridRecording *rec = recs->First();
     cCridSeriesRecording *srec = srecs->First();
     if (CridplayerSetup.SortOrder == CRIDPLAYER_SORTSERIESIN) {
        while (rec && srec) {
              if (strcoll(srec->Title(),rec->Title()) <= 0) {
                 Add(new cCridMenuItem(srec),
                     srec == controller->GetSeriesSelected());

                 srec = srecs->Next(srec);
                 }
              else {
                 Add(new cCridMenuItem(rec),
                     rec == controller->GetRecordingSelected());

                 rec = recs->Next(rec);
                 }
              }
        }
     else if (CridplayerSetup.SortOrder == CRIDPLAYER_SORTSERIESLAST) {
        while (rec) {
           Add(new cCridMenuItem(rec),
              rec == controller->GetRecordingSelected());

           rec = recs->Next(rec);
           }
        }

     while (srec) {
           Add(new cCridMenuItem(srec),
               srec == controller->GetSeriesSelected());

           srec = srecs->Next(srec);
           }

     while (rec) {
           Add(new cCridMenuItem(rec),
              rec == controller->GetRecordingSelected());

           rec = recs->Next(rec);
           }
     }
  if (Current() < 0)
     SetCurrent(First());
}

void cCridMenuRecordings::SetHelpKeys(void)
{
  SetStatus(NULL);
  cCridMenuItem *item = (cCridMenuItem *)Get(Current());
  int NewHelpKeys = 0;
  if (item) {
     if (item->GetType() == CridplayerRecording)
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     }

  if (helpKeys == NewHelpKeys)
     return;

  switch (NewHelpKeys) {
    case 0: SetHelp(NULL); break;
    case 1: SetHelp(tr("Button$Open")); break;
    case 2: SetHelp(tr("Button$Play"), tr("Button$Rewind"),
                    tr("Button$Delete"), tr("Button$Info"));
    }
  helpKeys = NewHelpKeys;
}

eOSState cCridMenuRecordings::PlayOpen(void)
{
  cCridMenuItem *item = (cCridMenuItem *)Get(Current());
  if (!item)
     return osContinue;

  const eCridplayerMenuItemType type = item->GetType();
  if (type == CridplayerDirectory) {
     if (controller->ChDir(item->GetDirectory())) {
        controller->SetSeriesSelected(NULL);
        controller->SetRecordingSelected(NULL);
        controller->SetPathSelected(NULL);
        SetTitle(controller->GetTitle());
        Set();
        helpKeys = -1;
        SetHelpKeys();
        Display();
        }
     else {
        SetStatus(tr("Cannot open directory"));
        }
     }
  else if(type == CridplayerSeries) {
     AddSubMenu(new cCridMenuRecordings(controller,item->GetSeries()));
     }
  else {
     controller->Play(item->GetRecording());
     return osEnd;
     }
  return osContinue;
}

eOSState cCridMenuRecordings::Rewind(void)
{
  cCridMenuItem *item = (cCridMenuItem *)Get(Current());
  if (item->GetType() == CridplayerRecording) {
     item->GetRecording()->Rewind();
     controller->Play(item->GetRecording());
     return osEnd;
     }

  return osContinue;
}

eOSState cCridMenuRecordings::Delete(void)
{
  cCridMenuItem *item = (cCridMenuItem *)Get(Current());
  if (item->GetType() == CridplayerRecording) {
     if (item->GetRecording()->IsRunning()) {
        SetStatus(tr("Cannot delete running recording"));
        return osContinue;
        }
     if (!Interface->Confirm(tr("Delete recording?"))) {
        return osContinue;
        }
     if (series) {
        if (!controller->Delete(series, item->GetRecording()))
           SetStatus(tr("Cannot delete recording"));
        else {
           Del(Current());
           if (Count() == 0)
              return osBack;
           Display();
           }
        }
     else {
        if (!controller->Delete(item->GetRecording()))
           SetStatus(tr("Cannot delete recording"));
        else {
           Del(Current());
           Display();
           }
        }
     }

  return osContinue;
}

eOSState cCridMenuRecordings::Info(void)
{
  cCridMenuItem *item = (cCridMenuItem *)Get(Current());
  if (item->GetType() == CridplayerRecording) {
     return AddSubMenu(new cCridMenuRecording(item->GetRecording()));
     }

  return osContinue;
}

eOSState cCridMenuRecordings::ProcessKey(eKeys Key)
{
  const bool HadSub = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);
  SetStatus(NULL);
  if (HasSubMenu() || Count() == 0)
     return state;

  if (state == osUnknown) {
     switch (Key) {
       case kOk:
       case kRed:    return PlayOpen();
       case kGreen:  return Rewind();
       case kYellow: return Delete();
       case kBlue:   return Info();
       default: break;
       }
     }

  if (Key == kYellow && HadSub) {
     Del(Current());
     Display();
     }
  if (Key != kNone)
     SetHelpKeys();

  return state;
}

// -- cCridReplayControl -----------------------------------------------------
#define CRID_STAY_SECONDS_OFF_END 10
#define CRID_MODETIMEOUT       3 // seconds
cCridReplayControl::cCridReplayControl(cCridController *Controller, const cCridRecording *Rec)
:cCridPlayerControl(Rec)
{
  controller = Controller;
  displayReplay = NULL;
  visible = modeOnly = shown = false;
//displayFrames
  lastCurrent = lastTotal = -1;
  lastCurrentTime = lastTotalTime = 0;
  lastPlay = lastForward = false;
  lastSpeed = -2; // an invalid value
  timeoutShow = 0;
  timeSearchActive = false;
  marks = GetMarks();
}

cCridReplayControl::~cCridReplayControl()
{
  Hide();
  Stop();
  controller->ReplayEnded();
}



void cCridReplayControl::ShowTimed(int Seconds)
{
  if (modeOnly)
     Hide();
  if (!visible) {
     shown = ShowProgress(true);
     timeoutShow = (shown && Seconds > 0) ? time(NULL) + Seconds : 0;
     }
}

void cCridReplayControl::Show(void)
{
  ShowTimed();
}

void cCridReplayControl::Hide(void)
{
  if (visible) {
     delete displayReplay;
     displayReplay = NULL;
     needsFastResponse = false;
     visible = false;
     modeOnly = false;
     lastPlay = lastForward = false;
     lastSpeed = -2; // an invalid value
     timeSearchActive = false;
     }
}

void cCridReplayControl::ShowMode(void)
{
  if (visible || Setup.ShowReplayMode && !cOsd::IsOpen()) {
     bool Play, Forward;
     int Speed;
     if (GetReplayMode(Play, Forward, Speed) && (!visible || Play != lastPlay || Forward != lastForward || Speed != lastSpeed)) {
        bool NormalPlay = (Play && Speed == -1);

        if (!visible) {
           if (NormalPlay)
              return; // no need to do indicate ">" unless there was a different mode displayed before
           visible = modeOnly = true;
           displayReplay = Skins.Current()->DisplayReplay(modeOnly);
           }

        if (modeOnly && !timeoutShow && NormalPlay)
           timeoutShow = time(NULL) + CRID_MODETIMEOUT;
        displayReplay->SetMode(Play, Forward, Speed);
        lastPlay = Play;
        lastForward = Forward;
        lastSpeed = Speed;
        }
     }
}

bool cCridReplayControl::ShowProgress(bool Initial)
{
  int Current, Total, CurrentTime, TotalTime;

  if (GetIndex(Current, Total, CurrentTime, TotalTime) && Total > 0) {
     if (!visible) {
        displayReplay = Skins.Current()->DisplayReplay(modeOnly);
        if (marks)
           displayReplay->SetMarks(marks);
        visible = true;
        needsFastResponse = true;
        }
     if (Initial) {
        displayReplay->SetTitle(Recording()->Title());
        lastCurrent = lastTotal = -1;
        }
     if (Total != lastTotal) {
        char buf[16];
        const int h = TotalTime / 3600;
        const int m = (TotalTime % 3600) / 60;
        const int s =  TotalTime % 60;

        snprintf(buf,16,"%d:%02d:%02d",h,m,s);
        displayReplay->SetTotal(buf);
        if (!Initial)
           displayReplay->Flush();
        }
     if (Current != lastCurrent || Total != lastTotal) {
        displayReplay->SetProgress(Current, Total);
        if (!Initial)
           displayReplay->Flush();
        char buf[16];
        const int h = CurrentTime / 3600;
        const int m = (CurrentTime % 3600) / 60;
        const int s =  CurrentTime % 60;

        snprintf(buf,16,"%d:%02d:%02d",h,m,s);
        displayReplay->SetCurrent(buf);
        displayReplay->Flush();
        lastCurrent = Current;
        lastCurrentTime = CurrentTime;
        }
     lastTotal = Total;
     lastTotalTime = TotalTime;
     ShowMode();
     return true;
     }
  return false;
}

void cCridReplayControl::TimeSearchDisplay(void)
{
  char buf[64];
  strcpy(buf, tr("Jump: "));
  int len = strlen(buf);
  char h10 = '0' + (timeSearchTime >> 24);
  char h1  = '0' + ((timeSearchTime & 0x00FF0000) >> 16);
  char m10 = '0' + ((timeSearchTime & 0x0000FF00) >> 8);
  char m1  = '0' + (timeSearchTime & 0x000000FF);
  char ch10 = timeSearchPos > 3 ? h10 : '-';
  char ch1  = timeSearchPos > 2 ? h1  : '-';
  char cm10 = timeSearchPos > 1 ? m10 : '-';
  char cm1  = timeSearchPos > 0 ? m1  : '-';
  sprintf(buf + len, "%c%c:%c%c", ch10, ch1, cm10, cm1);
  displayReplay->SetJump(buf);
}

void cCridReplayControl::TimeSearchProcess(eKeys Key)
{
#define STAY_SECONDS_OFF_END 10
  int Seconds = (timeSearchTime >> 24) * 36000 + ((timeSearchTime & 0x00FF0000) >> 16) * 3600 + ((timeSearchTime & 0x0000FF00) >> 8) * 600 + (timeSearchTime & 0x000000FF) * 60;
  switch (Key) {
    case k0 ... k9:
         if (timeSearchPos < 4) {
            timeSearchTime <<= 8;
            timeSearchTime |= Key - k0;
            timeSearchPos++;
            TimeSearchDisplay();
            }
         break;
    case kFastRew:
    case kLeft:
    case kFastFwd:
    case kRight: {
         int dir = ((Key == kRight || Key == kFastFwd) ? 1 : -1);
         if (dir > 0)
            Seconds = min(lastTotalTime - lastCurrentTime - CRID_STAY_SECONDS_OFF_END, Seconds);
         SkipSeconds(Seconds * dir);
         timeSearchActive = false;
         }
         break;
    case kPlay:
    case kUp:
    case kPause:
    case kDown:
    case kOk:
         Seconds = min(lastTotalTime - CRID_STAY_SECONDS_OFF_END, Seconds);
         GotoTime(Seconds, Key == kDown || Key == kPause || Key == kOk);
         timeSearchActive = false;
         break;
    default:
         timeSearchActive = false;
         break;
    }

  if (!timeSearchActive) {
     if (timeSearchHide)
        Hide();
     else
        displayReplay->SetJump(NULL);
     ShowMode();
     }
}

void cCridReplayControl::TimeSearch(void)
{
  timeSearchTime = timeSearchPos = 0;
  timeSearchHide = false;
  if (modeOnly)
     Hide();
  if (!visible) {
     Show();
     if (visible)
        timeSearchHide = true;
     else
        return;
     }
  timeoutShow = 0;
  TimeSearchDisplay();
  timeSearchActive = true;
}

void cCridReplayControl::MarkToggle(void)
{
  int Current, Total, CurrentTime, TotalTime;
  if (marks && GetIndex(Current, Total, CurrentTime, TotalTime)) {
     cCridMark *m = (cCridMark *)marks->Get(Current);
     lastCurrent = -1; // triggers redisplay
     if (m)
        marks->Del(m);
     else {
        m = marks->Add(Current);
        ShowTimed(2);
        bool Play, Forward;
        int Speed;
        if (GetReplayMode(Play, Forward, Speed) && !Play)
           Goto(m->realPosition, true);
        }
     }
}

void cCridReplayControl::MarkJump(bool Forward)
{

  if (marks && marks->Count()) {
     int Current, Total,t1,t2;
     if (GetIndex(Current, Total,t1,t2)) {
        cCridMark *m = (cCridMark *)(Forward ? marks->GetNext(Current) : marks->GetPrev(Current));
        if (m) {
           Goto(m->realPosition, true);
           }
        }
     }

}

void cCridReplayControl::MarkMove(bool Forward)
{
  int Current, Total,t1,t2;
  if (marks && GetIndex(Current, Total,t1,t2)) {
     cCridMark *m = (cCridMark *)marks->Get(Current);
     if (m) {
        int p = m->position + (Forward ? 1 : -1);
        cCridMark *m2;
        if (Forward) {
           if (p > Total || (m2 = (cCridMark *)marks->Next(m)) != NULL && m2->position <= p)
              return;
           }
        else {
           if (p < 0 || (m2 = (cCridMark *)marks->Prev(m)) != NULL && m2->position >= p)
              return;
           }
        m->position = p;
        m->realPosition += (Forward ? 1 : -1);
        Goto(m->realPosition, true);
        }
     }
}

void cCridReplayControl::EditCut(void)
{
#if 0
  if (fileName) {
     Hide();
     if (!cCutter::Active()) {
        if (!marks.Count())
           Skins.Message(mtError, tr("No editing marks defined!"));
        else if (!cCutter::Start(fileName))
           Skins.Message(mtError, tr("Can't start editing process!"));
        else
           Skins.Message(mtInfo, tr("Editing process started"));
        }
     else
        Skins.Message(mtError, tr("Editing process already active!"));
     ShowMode();
     }
#endif
}

void cCridReplayControl::EditTest(void)
{
#if 0
  int Current, Total;
  if (GetIndex(Current, Total)) {
     cMark *m = marks.Get(Current);
     if (!m)
        m = marks.GetNext(Current);
     if (m) {
        if ((m->Index() & 0x01) != 0)
           m = marks.Next(m);
        if (m) {
           Goto(m->position - SecondsToFrames(3));
           Play();
           }
        }
     }
#endif
}

cOsdObject *cCridReplayControl::GetInfo(void)
{
  return new cCridMenuRecording(Recording(), false);
}

eOSState cCridReplayControl::ProcessKey(eKeys Key)
{
  if (!Active())
     return osEnd;
  if (controller->ReplayRecordingDeleted()) {
     Hide();
     Stop();
     controller->ReplayEnded();
     return osEnd;
     }
  if (visible) {
     if (timeoutShow && time(NULL) > timeoutShow) {
        Hide();
        ShowMode();
        timeoutShow = 0;
        }
     else if (modeOnly)
        ShowMode();
     else
        shown = ShowProgress(!shown) || shown;
     }
  /* bool DisplayedFrames = displayFrames;
  displayFrames = false; */
  if (timeSearchActive && Key != kNone) {
     TimeSearchProcess(Key);
     return osContinue;
     }
  bool DoShowMode = true;
  switch (Key) {
    // Positioning:
    case kPlay:
    case kUp:      Play(); break;
    case kPause:
    case kDown:    Pause(); break;
    case kFastRew|k_Release:
    case kLeft|k_Release:
                   if (Setup.MultiSpeedMode) break;
    case kFastRew:
    case kLeft:    Backward(); break;
    case kFastFwd|k_Release:
    case kRight|k_Release:
                   if (Setup.MultiSpeedMode) break;
    case kFastFwd:
    case kRight:   Forward(); break;
    case kRed:     TimeSearch(); break;
    case kGreen|k_Repeat:
    case kGreen:   SkipSeconds(-60); break;
    case kYellow|k_Repeat:
    case kYellow:  SkipSeconds( 60); break;
    case kStop:
    case kBlue:    Hide();
                   Stop();
                   controller->ReplayEnded();
                   return osEnd;
    default: {
      DoShowMode = false;
      switch (Key) {
        // Editing:
        case kMarkToggle:      MarkToggle(); break;
        case kPrev|k_Repeat:
        case kPrev:
        case kMarkJumpBack|k_Repeat:
        case kMarkJumpBack:    MarkJump(false); break;
        case kNext|k_Repeat:
        case kNext:
        case kMarkJumpForward|k_Repeat:
        case kMarkJumpForward: MarkJump(true); break;
        case kMarkMoveBack|k_Repeat:
        case kMarkMoveBack:    MarkMove(false); break;
        case kMarkMoveForward|k_Repeat:
        case kMarkMoveForward: MarkMove(true); break;
        case kEditCut:         EditCut(); break;
        case kEditTest:        EditTest(); break;
        default: {
          // displayFrames = DisplayedFrames;
          switch (Key) {
            // Menu control:
            case kOk:      if (visible && !modeOnly) {
                              Hide();
                              DoShowMode = true;
                              }
                           else
                              Show();
                           break;
            case kBack:    return osBack;
            default:       return osUnknown;
            }
          }
        }
      }
    }
  if (DoShowMode)
     ShowMode();
  return osContinue;
}

// --- cCridSetupMenu --------------------------------------------------------

cCridSetupMenu::cCridSetupMenu(cCridController *Controller)
{
  controller = Controller;
  strncpy(basePath, CridplayerSetup.BasePath, MAXPATHLEN);
  basePath[MAXPATHLEN - 1] = 0;
  showHidden = CridplayerSetup.ShowHidden;
  recordingPreGap = CridplayerSetup.RecordingPreGap;
  recordingPostGap = CridplayerSetup.RecordingPostGap;
  strncpy(dirMarkerPre, CridplayerSetup.DirMarkerPre, MAXMARKERLEN);
  dirMarkerPre[MAXMARKERLEN - 1] = 0;
  strncpy(dirMarkerPost, CridplayerSetup.DirMarkerPost, MAXMARKERLEN);
  dirMarkerPost[MAXMARKERLEN - 1] = 0;

  Add(new cMenuEditStrItem(tr("CridSetup$Path base"), basePath, MAXPATHLEN, tr(FileNameChars)));
  Add(new cMenuEditBoolItem(tr("CridSetup$Show hidden files"), &showHidden));
  Add(new cMenuEditIntItem(tr("CridSetup$Margin at start (s)"), &recordingPreGap));
  Add(new cMenuEditIntItem(tr("CridSetup$Margin at stop (s)"), &recordingPostGap));
  Add(new cMenuEditStrItem(tr("CridSetup$Directory marking prefix"), dirMarkerPre, MAXMARKERLEN, tr(FileNameChars)));
  Add(new cMenuEditStrItem(tr("CridSetup$Directory marking suffix"), dirMarkerPost, MAXMARKERLEN, tr(FileNameChars)));
}

void cCridSetupMenu::Store(void)
{
  controller->SetBasePath(basePath);
  SetupStore("PathBase",CridplayerSetup.BasePath);
  controller->SetShowHidden(showHidden);
  SetupStore("ShowHiddenFiles",CridplayerSetup.ShowHidden);
  SetupStore("StartMargin", CridplayerSetup.RecordingPreGap = recordingPreGap);
  SetupStore("StopMargin", CridplayerSetup.RecordingPostGap = recordingPostGap);
  strncpy(CridplayerSetup.DirMarkerPre, dirMarkerPre,MAXMARKERLEN);
  CridplayerSetup.DirMarkerPre[MAXMARKERLEN - 1] = 0;
  SetupStore("DirectoryMarkingPrefix", CridplayerSetup.DirMarkerPre);
  strncpy(CridplayerSetup.DirMarkerPost, dirMarkerPost,MAXMARKERLEN);
  CridplayerSetup.DirMarkerPost[MAXMARKERLEN - 1] = 0;
  SetupStore("DirectoryMarkingSuffix",CridplayerSetup.DirMarkerPost);
}
