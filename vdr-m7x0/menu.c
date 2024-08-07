/*
 * menu.c: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 */

#include "menu.h"
#include "iconpatch.h"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "channels.h"
#include "config.h"
#include "cutter.h"
#include "eitscan.h"
//M7X0 BEGIN AK
#include "epgmode.h"
//M7X0 END AK
#include "i18n.h"
#include "interface.h"
#include "plugin.h"
#include "recording.h"
#include "remote.h"
#include "shutdown.h"
#include "sources.h"
#include "status.h"
#include "themes.h"
#include "timers.h"
#include "transfer.h"
#include "videodir.h"
#ifdef USE_PINPLUGIN
#include "childlock.h"
#endif
#include "tshift.h"

#define MAXWAIT4EPGINFO   3 // seconds
#define MODETIMEOUT       3 // seconds
#define DISKSPACECHEK     5 // seconds between disk space checks in the main menu
#define NEWTIMERLIMIT   120 // seconds until the start time of a new timer created from the Schedule menu,
                            // within which it will go directly into the "Edit timer" menu to allow
                            // further parameter settings

#define MAXRECORDCONTROLS (MAXDEVICES * MAXRECEIVERS)
#define MAXINSTANTRECTIME (24 * 60 - 1) // 23:59 hours
#define MAXWAITFORCAMMENU 4 // seconds to wait for the CAM menu to open
#define PROGRESSTIMEOUT   100 // milliseconds to wait before updating the replay progress display
#define MINFREEDISK       300 // minimum free disk space (in MB) required to start recording
#define NODISKSPACEDELTA  300 // seconds between "Not enough disk space to start recording!" messages

#define CHNUMWIDTH  (numdigits(Channels.MaxNumber()) + 1)

#define MB_PER_MINUTE 25.75 // this is just an estimate!

// --- cMenuEditCaItem -------------------------------------------------------

class cMenuEditCaItem : public cMenuEditIntItem {
protected:
  virtual void Set(void);
public:
  cMenuEditCaItem(const char *Name, int *Value);
  eOSState ProcessKey(eKeys Key);
  };

cMenuEditCaItem::cMenuEditCaItem(const char *Name, int *Value)
:cMenuEditIntItem(Name, Value, 0)
{
  Set();
}

void cMenuEditCaItem::Set(void)
{
  if (*value == CA_FTA)
     SetValue(tr("Free To Air"));
  else if (*value >= CA_ENCRYPTED_MIN)
     SetValue(tr("encrypted"));
  else
     cMenuEditIntItem::Set();
}

eOSState cMenuEditCaItem::ProcessKey(eKeys Key)
{
  eOSState state = cMenuEditItem::ProcessKey(Key);

  if (state == osUnknown) {
     if (NORMALKEY(Key) == kLeft && *value >= CA_ENCRYPTED_MIN)
        *value = CA_FTA;
     else
        return cMenuEditIntItem::ProcessKey(Key);
     Set();
     state = osContinue;
     }
  return state;
}

#ifdef M750S

// --- cMenuEditSrcItem ------------------------------------------------------

class cMenuEditSrcItem : public cMenuEditIntItem {
private:
  const cSource *source;
protected:
  virtual void Set(void);
public:
  cMenuEditSrcItem(const char *Name, int *Value);
  eOSState ProcessKey(eKeys Key);
  };

cMenuEditSrcItem::cMenuEditSrcItem(const char *Name, int *Value)
:cMenuEditIntItem(Name, Value, 0)
{
  source = Sources.Get(*Value);
  Set();
}

void cMenuEditSrcItem::Set(void)
{
  if (source)
     SetValue(cString::sprintf("%s - %s", *cSource::ToString(source->Code()), source->Description()));
  else
     cMenuEditIntItem::Set();
}

eOSState cMenuEditSrcItem::ProcessKey(eKeys Key)
{
  eOSState state = cMenuEditItem::ProcessKey(Key);

  if (state == osUnknown) {
     bool IsRepeat = Key & k_Repeat;
     Key = NORMALKEY(Key);
     if (Key == kLeft) { // TODO might want to increase the delta if repeated quickly?
        if (source) {
           if (source->Prev())
              source = (cSource *)source->Prev();
           else if (!IsRepeat)
              source = Sources.Last();
           *value = source->Code();
           }
        }
     else if (Key == kRight) {
        if (source) {
           if (source->Next())
              source = (cSource *)source->Next();
           else if (!IsRepeat)
              source = Sources.First();
           }
        else
           source = Sources.First();
        if (source)
           *value = source->Code();
        }
     else
        return state; // we don't call cMenuEditIntItem::ProcessKey(Key) here since we don't accept numerical input
     Set();
     state = osContinue;
     }
  return state;
}
#endif

// --- cMenuEditMapItem ------------------------------------------------------

class cMenuEditMapItem : public cMenuEditItem {
protected:
  int *value;
  const tChannelParameterMap *map;
  const char *zeroString;
  virtual void Set(void);
public:
  cMenuEditMapItem(const char *Name, int *Value, const tChannelParameterMap *Map, const char *ZeroString = NULL);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuEditMapItem::cMenuEditMapItem(const char *Name, int *Value, const tChannelParameterMap *Map, const char *ZeroString)
:cMenuEditItem(Name)
{
  value = Value;
  map = Map;
  zeroString = ZeroString;
  Set();
}

void cMenuEditMapItem::Set(void)
{
  int n = MapToUser(*value, map);
  if (n == 999)
     SetValue(tr("auto"));
  else if (n == 0 && zeroString)
     SetValue(zeroString);
  else if (n >= 0) {
     char buf[16];
     snprintf(buf, sizeof(buf), "%d", n);
     SetValue(buf);
     }
  else
     SetValue("???");
}

eOSState cMenuEditMapItem::ProcessKey(eKeys Key)
{
  eOSState state = cMenuEditItem::ProcessKey(Key);

  if (state == osUnknown) {
     int newValue = *value;
     int n = DriverIndex(*value, map);
     if (NORMALKEY(Key) == kLeft) { // TODO might want to increase the delta if repeated quickly?
        if (n-- > 0)
           newValue = map[n].driverValue;
        }
     else if (NORMALKEY(Key) == kRight) {
        if (map[++n].userValue >= 0)
           newValue = map[n].driverValue;
        }
     else
        return state;
     if (newValue != *value) {
        *value = newValue;
        Set();
        }
     state = osContinue;
     }
  return state;
}

// --- cMenuEditChannel ------------------------------------------------------

class cMenuEditChannel : public cOsdMenu {
private:
  cChannel *channel;
  cChannel data;
  char name[256];
  void Setup(void);
public:
  cMenuEditChannel(cChannel *Channel, bool New = false);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuEditChannel::cMenuEditChannel(cChannel *Channel, bool New)
:cOsdMenu(tr("Edit channel"), 16)
{
  channel = Channel;
  if (channel) {
     data = *channel;
     if (New) {
        channel = NULL;
        // clear non-editable members:
        data.nid = 0;
        data.tid = 0;
        data.rid = 0;
        *data.shortName  = 0;
        *data.provider   = 0;
        *data.portalName = 0;
        }
     Setup();
     }
}

void cMenuEditChannel::Setup(void)
{
  int current = Current();
#ifdef M750S
  char type = **cSource::ToString(data.source);
#define ST(s) if (strchr(s, type))
#else
#define ST(s) if (strchr(s, 'T'))
#endif
  Clear();

  // Parameters for all types of sources:
  strn0cpy(name, data.name, sizeof(name));
  Add(new cMenuEditStrItem( tr("Name"),          name, sizeof(name), tr(FileNameChars)));
#ifdef M750S
  Add(new cMenuEditSrcItem( tr("Source"),       &data.source));
#endif
  Add(new cMenuEditIntItem( tr("Frequency"),    &data.frequency));
  Add(new cMenuEditIntItem( tr("Vpid"),         &data.vpid,  0, 0x1FFF));
  Add(new cMenuEditIntItem( tr("Ppid"),         &data.ppid,  0, 0x1FFF));
  Add(new cMenuEditIntItem( tr("Apid1"),        &data.apids[0], 0, 0x1FFF));
  Add(new cMenuEditIntItem( tr("Apid2"),        &data.apids[1], 0, 0x1FFF));
  Add(new cMenuEditIntItem( tr("Dpid1"),        &data.dpids[0], 0, 0x1FFF));
  Add(new cMenuEditIntItem( tr("Dpid2"),        &data.dpids[1], 0, 0x1FFF));
  Add(new cMenuEditIntItem( tr("Tpid"),         &data.tpid,  0, 0x1FFF));
  Add(new cMenuEditCaItem(  tr("CA"),           &data.caids[0]));
  Add(new cMenuEditIntItem( tr("Sid"),          &data.sid, 1, 0xFFFF));
  /* XXX not yet used
  Add(new cMenuEditIntItem( tr("Nid"),          &data.nid, 0));
  Add(new cMenuEditIntItem( tr("Tid"),          &data.tid, 0));
  Add(new cMenuEditIntItem( tr("Rid"),          &data.rid, 0));
  XXX*/
  // Parameters for specific types of sources:
  ST(" S ")  Add(new cMenuEditChrItem( tr("Polarization"), &data.polarization, "hvlr"));
  ST("CS ")  Add(new cMenuEditIntItem( tr("Srate"),        &data.srate));
  ST("CST")  Add(new cMenuEditMapItem( tr("Inversion"),    &data.inversion,    InversionValues, tr("off")));
  ST("CST")  Add(new cMenuEditMapItem( tr("CoderateH"),    &data.coderateH,    CoderateValues, tr("none")));
  ST("  T")  Add(new cMenuEditMapItem( tr("CoderateL"),    &data.coderateL,    CoderateValues, tr("none")));
  ST("C T")  Add(new cMenuEditMapItem( tr("Modulation"),   &data.modulation,   ModulationValues, "QPSK"));
  ST("  T")  Add(new cMenuEditMapItem( tr("Bandwidth"),    &data.bandwidth,    BandwidthValues));
  ST("  T")  Add(new cMenuEditMapItem( tr("Transmission"), &data.transmission, TransmissionValues));
  ST("  T")  Add(new cMenuEditMapItem( tr("Guard"),        &data.guard,        GuardValues));
  ST("  T")  Add(new cMenuEditMapItem( tr("Hierarchy"),    &data.hierarchy,    HierarchyValues, tr("none")));

  SetCurrent(Get(current));
  Display();
}

eOSState cMenuEditChannel::ProcessKey(eKeys Key)
{
  int oldSource = data.source;
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     if (Key == kOk) {
        if (Channels.HasUniqueChannelID(&data, channel)) {
           data.name = strcpyrealloc(data.name, name);
           if (channel) {
              *channel = data;
              isyslog("edited channel %d %s", channel->Number(), *data.ToText());
              state = osBack;
              }
           else {
              channel = new cChannel;
              *channel = data;
              Channels.Add(channel);
              Channels.ReNumber();
              isyslog("added channel %d %s", channel->Number(), *data.ToText());
              state = osUser1;
              }
           Channels.SetModified(true);
           }
        else {
           Skins.Message(mtError, tr("Channel settings are not unique!"));
           state = osContinue;
           }
        }
     }
  if (Key != kNone && (data.source & cSource::st_Mask) != (oldSource & cSource::st_Mask))
     Setup();
  return state;
}

// --- cMenuChannelItem ------------------------------------------------------

class cMenuChannelItem : public cOsdItem {
public:
  enum eChannelSortMode { csmNumber, csmName, csmProvider };
private:
  static eChannelSortMode sortMode;
  cChannel *channel;
public:
  cMenuChannelItem(cChannel *Channel);
  static void SetSortMode(eChannelSortMode SortMode) { sortMode = SortMode; }
  static void IncSortMode(void) { sortMode = eChannelSortMode((sortMode == csmProvider) ? csmNumber : sortMode + 1); }
  static eChannelSortMode SortMode(void) { return sortMode; }
  virtual int Compare(const cListObject &ListObject) const;
  virtual void Set(void);
  cChannel *Channel(void) { return channel; }
  };

cMenuChannelItem::eChannelSortMode cMenuChannelItem::sortMode = csmNumber;

cMenuChannelItem::cMenuChannelItem(cChannel *Channel)
{
  channel = Channel;
  if (channel->GroupSep())
     SetSelectable(false);
  Set();
}

int cMenuChannelItem::Compare(const cListObject &ListObject) const
{
  cMenuChannelItem *p = (cMenuChannelItem *)&ListObject;
  int r = -1;
  if (sortMode == csmProvider)
     r = strcoll(channel->Provider(), p->channel->Provider());
  if (sortMode == csmName || r == 0)
     r = strcoll(channel->Name(), p->channel->Name());
  if (sortMode == csmNumber || r == 0)
     r = channel->Number() - p->channel->Number();
  return r;
}

void cMenuChannelItem::Set(void)
{
  cString buffer;
  if (!channel->GroupSep()) {
     if (sortMode == csmProvider)
        buffer = cString::sprintf("%d\t%s - %s", channel->Number(), channel->Provider(), channel->Name());
     else if (Setup.WarEagleIcons) {
        if (channel->Vpid() == 1 || channel->Vpid() == 0)
            buffer = cString::sprintf("%d\t%c %-30s", channel->Number(), ICON_RADIO, channel->Name());
        else if (channel->Ca() == 0)
            buffer = cString::sprintf("%d\t%c %-30s", channel->Number(), ICON_TV, channel->Name());
        else
            buffer = cString::sprintf("%d\t%c %-30s", channel->Number(), ICON_TV_VERSCHL, channel->Name());
        }
     else
        buffer = cString::sprintf("%2d  %s%s  %s", channel->Number(),
	                                 channel->Vpid()>0 ? "t":" ",
					 (channel->Vpid()==0) && (channel->Apid(0) > 0) ? "r":" ",
					 channel->Name()
	);
     }
  else
     buffer = cString::sprintf("---\t%s ----------------------------------------------------------------", channel->Name());
  SetText(buffer);
}

// --- cMenuChannels ---------------------------------------------------------

#define CHANNELNUMBERTIMEOUT 1000 //ms

class cMenuChannels : public cOsdMenu {
private:
  int number;
  cTimeMs numberTimer;
  void Setup(void);
  cChannel *GetChannel(int Index);
  void Propagate(void);
  bool IsFiltered(void);
  int onlyTV, onlyRadio;
protected:
  eOSState Number(eKeys Key);
  eOSState Switch(void);
  eOSState Edit(void);
  eOSState New(void);
  eOSState Delete(void);
  virtual void Move(int From, int To);
public:
  cMenuChannels(void);
  ~cMenuChannels();
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuChannels::cMenuChannels(void)
:cOsdMenu(tr("Channels setup"), CHNUMWIDTH)
{
  number = 0;
  onlyTV = onlyRadio = 0;
  Setup();
  Channels.IncBeingEdited();
}

cMenuChannels::~cMenuChannels()
{
  Channels.DecBeingEdited();
}

void cMenuChannels::Setup(void)
{
  cChannel *currentChannel = GetChannel(Current());
  if (!currentChannel)
     currentChannel = Channels.GetByNumber(cDevice::CurrentChannel());
  cMenuChannelItem *currentItem = NULL;
  cMenuChannelItem *lastUsedItem = NULL;
  cMenuChannelItem *firstUsedItem = NULL;
  Clear();

  for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
       if ((!channel->GroupSep() || (cMenuChannelItem::SortMode() == cMenuChannelItem::csmNumber && ( channel == currentChannel || channel->Filtered() )))
            && *channel->Name()  //Channel must have a name
 	    && (channel->GroupSep() || //Do not filter Groupseperators
             	(  !(onlyTV      && !channel->IsTV()     )
   		&& !(onlyRadio   && !channel->IsRadio()  )
 	    ))) {
             cMenuChannelItem *item = new cMenuChannelItem(channel);
             Add(item);
            if (channel == currentChannel)
         	currentItem = item;
 	    if (!channel->GroupSep()) {
 		lastUsedItem = item;
 		if( !firstUsedItem )
 		    firstUsedItem = item;
 	    }
       } else {
             if (channel == currentChannel)
         	currentItem = lastUsedItem; // current channel is now invisible, so we use the nearest channel
      }
  }
  if (cMenuChannelItem::SortMode() != cMenuChannelItem::csmNumber)
     Sort();
  if (!currentItem) // happens, if after filtering the first line is a groupsep
     currentItem = firstUsedItem;
  SetCurrent(currentItem);
  SetHelp(tr("Button$Edit"), tr("Button$New"), tr("Button$Delete"), tr("Button$Mark"));
  Display();
}

cChannel *cMenuChannels::GetChannel(int Index)
{
  cMenuChannelItem *p = (cMenuChannelItem *)Get(Index);
  return p ? (cChannel *)p->Channel() : NULL;
}

void cMenuChannels::Propagate(void)
{
  Channels.ReNumber();
  for (cMenuChannelItem *ci = (cMenuChannelItem *)First(); ci; ci = (cMenuChannelItem *)ci->Next())
      ci->Set();
  Display();
  Channels.SetModified(true);
}

eOSState cMenuChannels::Number(eKeys Key)
{
  if (HasSubMenu())
     return osContinue;
  if (numberTimer.TimedOut())
     number = 0;
  if (!number && Key == k0) {
     cMenuChannelItem::IncSortMode();
     Setup();
     }
  else {
     number = number * 10 + Key - k0;
     for (cMenuChannelItem *ci = (cMenuChannelItem *)First(); ci; ci = (cMenuChannelItem *)ci->Next()) {
         if (!ci->Channel()->GroupSep() && ci->Channel()->Number() == number) {
            SetCurrent(ci);
            Display();
            break;
            }
         }
     numberTimer.Set(CHANNELNUMBERTIMEOUT);
     }
  return osContinue;
}

bool cMenuChannels::IsFiltered(void)
{
  return onlyTV || onlyRadio;
}

eOSState cMenuChannels::Switch(void)
{
  if (HasSubMenu())
     return osContinue;
  cChannel *ch = GetChannel(Current());
  if (ch)
     return cDevice::PrimaryDevice()->SwitchChannel(ch, true) ? osEnd : osContinue;
  return osEnd;
}

eOSState cMenuChannels::Edit(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cChannel *ch = GetChannel(Current());
  if (ch)
     return AddSubMenu(new cMenuEditChannel(ch));
  return osContinue;
}

eOSState cMenuChannels::New(void)
{
  if (HasSubMenu())
     return osContinue;
  return AddSubMenu(new cMenuEditChannel(GetChannel(Current()), true));
}

eOSState cMenuChannels::Delete(void)
{
  if (!HasSubMenu() && Count() > 0) {
     int CurrentChannelNr = cDevice::CurrentChannel();
     cChannel *CurrentChannel = Channels.GetByNumber(CurrentChannelNr);
     int Index = Current();
     cChannel *channel = GetChannel(Current());
     int DeletedChannel = channel->Number();
     // Check if there is a timer using this channel:
     if (channel->HasTimer()) {
        Skins.Message(mtError, tr("Channel is being used by a timer!"));
        return osContinue;
        }
     if (Interface->Confirm(tr("Delete channel?"))) {
        if (CurrentChannel && channel == CurrentChannel) {
           int n = Channels.GetNextNormal(CurrentChannel->Index());
           if (n < 0)
              n = Channels.GetPrevNormal(CurrentChannel->Index());
           CurrentChannel = Channels.Get(n);
           CurrentChannelNr = 0; // triggers channel switch below
           }
        Channels.Del(channel);
        cOsdMenu::Del(Index);
        Propagate();
        Channels.SetModified(true);
        isyslog("channel %d deleted", DeletedChannel);
        if (CurrentChannel && CurrentChannel->Number() != CurrentChannelNr) {
           if (!cDevice::PrimaryDevice()->Replaying() || cDevice::PrimaryDevice()->Transferring())
              Channels.SwitchTo(CurrentChannel->Number());
           else
              cDevice::SetCurrentChannel(CurrentChannel->Number());
           }
        }
     }
  return osContinue;
}

void cMenuChannels::Move(int From, int To)
{
  int CurrentChannelNr = cDevice::CurrentChannel();
  cChannel *CurrentChannel = Channels.GetByNumber(CurrentChannelNr);
  cChannel *FromChannel = GetChannel(From);
  cChannel *ToChannel = GetChannel(To);
  if (FromChannel && ToChannel) {
     int FromNumber = FromChannel->Number();
     int ToNumber = ToChannel->Number();
     Channels.Move(FromChannel, ToChannel);
     cOsdMenu::Move(From, To);
     Propagate();
     Channels.SetModified(true);
     isyslog("channel %d moved to %d", FromNumber, ToNumber);
     if (CurrentChannel && CurrentChannel->Number() != CurrentChannelNr) {
        if (!cDevice::PrimaryDevice()->Replaying() || cDevice::PrimaryDevice()->Transferring())
           Channels.SwitchTo(CurrentChannel->Number());
        else
           cDevice::SetCurrentChannel(CurrentChannel->Number());
        }
     }
}

eOSState cMenuChannels::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  switch (state) {
    case osUser1: {
         if (cChannel *channel = Channels.Last()) {
            Add(new cMenuChannelItem(channel), true);
            return CloseSubMenu();
            }
         }
         break;
    default:
         if (state == osUnknown) {
            switch (Key) {
              case k0:
	                    cMenuChannelItem::IncSortMode();
	                    Setup();
	                    break;
              case k1:      // show all channels (default)
	                    onlyTV = onlyRadio = 0;
                            Setup();
			    Skins.Message(mtStatus, tr("All channels"));
                            break;
              case k2:      // show only TV-channels with sound
	                    onlyTV = !0;
			    onlyRadio = 0;
                            Setup();
			    Skins.Message(mtStatus, tr("Show only tv channels"));
                            break;
              case k3:      // show only radio/broadcast
	                    onlyTV = 0;
	                    onlyRadio = !0;
                            Setup();
			    Skins.Message(mtStatus, tr("Show only radio channels"));
                            break;
              case kOk:     return Switch();
              case kRed:    return Edit();
              case kGreen:  return New();
              case kYellow: return Delete();
              case kBlue:   if (!HasSubMenu())
                               Mark();
                            break;
              default: break;
              }
            }
    }
  return state;
}

// --- cMenuText -------------------------------------------------------------

cMenuText::cMenuText(const char *Title, const char *Text, eDvbFont Font)
:cOsdMenu(Title)
{
  text = NULL;
  font = Font;
  SetText(Text);
}

cMenuText::~cMenuText()
{
  free(text);
}

void cMenuText::SetText(const char *Text)
{
  free(text);
  text = Text ? strdup(Text) : NULL;
}

void cMenuText::Display(void)
{
  cOsdMenu::Display();
  DisplayMenu()->SetText(text, font == fontFix); //XXX define control character in text to choose the font???
  if (text)
     cStatus::MsgOsdTextItem(text);
}

eOSState cMenuText::ProcessKey(eKeys Key)
{
  switch (int(Key)) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
                  DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
                  cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft);
                  return osContinue;
    default: break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk: return osBack;
       default:  state = osContinue;
       }
     }
  return state;
}

// --- cMenuFolderItem -------------------------------------------------------

class cMenuFolderItem : public cOsdItem {
private:
  cNestedItem *folder;
public:
  cMenuFolderItem(cNestedItem *Folder);
  cNestedItem *Folder(void) { return folder; }
  };

cMenuFolderItem::cMenuFolderItem(cNestedItem *Folder)
:cOsdItem(Folder->Text())
{
  folder = Folder;
  if (folder->SubItems())
     SetText(cString::sprintf("%s...", folder->Text()));
}

// --- cMenuEditFolder -------------------------------------------------------

class cMenuEditFolder : public cOsdMenu {
private:
  cList<cNestedItem> *list;
  cNestedItem *folder;
  char name[PATH_MAX];
  int subFolder;
  eOSState Confirm(void);
public:
  cMenuEditFolder(const char *Dir, cList<cNestedItem> *List, cNestedItem *Folder = NULL);
  cString GetFolder(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuEditFolder::cMenuEditFolder(const char *Dir, cList<cNestedItem> *List, cNestedItem *Folder)
:cOsdMenu(Folder ? tr("Edit folder") : tr("New folder"), 12)
{
  list = List;
  folder = Folder;
  if (folder) {
     strn0cpy(name, folder->Text(), sizeof(name));
     subFolder = folder->SubItems() != NULL;
     }
  else {
     *name = 0;
     subFolder = 0;
     cRemote::Put(kRight, true); // go right into string editing mode
     }
  if (!isempty(Dir)) {
     cOsdItem *DirItem = new cOsdItem(Dir);
     DirItem->SetSelectable(false);
     Add(DirItem);
     }
  Add(new cMenuEditStrItem( tr("Name"), name, sizeof(name), tr(FileNameChars)));
  Add(new cMenuEditBoolItem(tr("Sub folder"), &subFolder));
}

cString cMenuEditFolder::GetFolder(void)
{
  return folder ? folder->Text() : "";
}

eOSState cMenuEditFolder::Confirm(void)
{
  if (!folder || strcmp(folder->Text(), name) != 0) {
     // each name may occur only once in a folder list
     for (cNestedItem *Folder = list->First(); Folder; Folder = list->Next(Folder)) {
         if (strcmp(Folder->Text(), name) == 0) {
            Skins.Message(mtError, tr("Folder name already exists!"));
            return osContinue;
            }
         }
     char *p = strpbrk(name, "\\{}#~"); // FOLDERDELIMCHAR
     if (p) {
        Skins.Message(mtError, cString::sprintf(tr("Folder name must not contain '%c'!"), *p));
        return osContinue;
        }
     }
  if (folder) {
     folder->SetText(name);
     folder->SetSubItems(subFolder);
     }
  else
     list->Add(folder = new cNestedItem(name, subFolder));
  return osEnd;
}

eOSState cMenuEditFolder::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:     return Confirm();
       case kRed:
       case kGreen:
       case kYellow:
       case kBlue:   return osContinue;
       default: break;
       }
     }
  return state;
}

// --- cMenuFolder -----------------------------------------------------------

cMenuFolder::cMenuFolder(const char *Title, cNestedItemList *NestedItemList, const char *Path)
:cOsdMenu(Title)
{
  list = nestedItemList = NestedItemList;
  firstFolder = NULL;
  editing = false;
  Set();
  SetHelpKeys();
  DescendPath(Path);
}

cMenuFolder::cMenuFolder(const char *Title, cList<cNestedItem> *List, cNestedItemList *NestedItemList, const char *Dir, const char *Path)
:cOsdMenu(Title)
{
  list = List;
  nestedItemList = NestedItemList;
  dir = Dir;
  firstFolder = NULL;
  editing = false;
  Set();
  SetHelpKeys();
  DescendPath(Path);
}

void cMenuFolder::SetHelpKeys(void)
{
  SetHelp(firstFolder ? tr("Button$Select") : NULL, tr("Button$New"), firstFolder ? tr("Button$Delete") : NULL, firstFolder ? tr("Button$Edit") : NULL);
}

void cMenuFolder::Set(const char *CurrentFolder)
{
  firstFolder = NULL;
  Clear();
  if (!isempty(dir)) {
     cOsdItem *DirItem = new cOsdItem(dir);
     DirItem->SetSelectable(false);
     Add(DirItem);
     }
  list->Sort();
  for (cNestedItem *Folder = list->First(); Folder; Folder = list->Next(Folder)) {
      cOsdItem *FolderItem = new cMenuFolderItem(Folder);
      Add(FolderItem, CurrentFolder ? strcmp(Folder->Text(), CurrentFolder) == 0 : false);
      if (!firstFolder)
         firstFolder = FolderItem;
      }
}

void cMenuFolder::DescendPath(const char *Path)
{
  if (Path) {
     const char *p = strchr(Path, FOLDERDELIMCHAR);
     if (p) {
        for (cMenuFolderItem *Folder = (cMenuFolderItem *)firstFolder; Folder; Folder = (cMenuFolderItem *)Next(Folder)) {
            if (strncmp(Folder->Folder()->Text(), Path, p - Path) == 0) {
               SetCurrent(Folder);
               if (Folder->Folder()->SubItems())
                  AddSubMenu(new cMenuFolder(Title(), Folder->Folder()->SubItems(), nestedItemList, !isempty(dir) ? *cString::sprintf("%s%c%s", *dir, FOLDERDELIMCHAR, Folder->Folder()->Text()) : Folder->Folder()->Text(), p + 1));
               break;
               }
            }
        }
    }
}

eOSState cMenuFolder::Select(void)
{
  if (firstFolder) {
     cMenuFolderItem *Folder = (cMenuFolderItem *)Get(Current());
     if (Folder) {
        if (Folder->Folder()->SubItems())
           return AddSubMenu(new cMenuFolder(Title(), Folder->Folder()->SubItems(), nestedItemList, !isempty(dir) ? *cString::sprintf("%s%c%s", *dir, FOLDERDELIMCHAR, Folder->Folder()->Text()) : Folder->Folder()->Text()));
        else
           return osEnd;
        }
     }
  return osContinue;
}

eOSState cMenuFolder::New(void)
{
  editing = true;
  return AddSubMenu(new cMenuEditFolder(dir, list));
}

eOSState cMenuFolder::Delete(void)
{
  if (!HasSubMenu() && firstFolder) {
     cMenuFolderItem *Folder = (cMenuFolderItem *)Get(Current());
     if (Folder && Interface->Confirm(Folder->Folder()->SubItems() ? tr("Delete folder and all sub folders?") : tr("Delete folder?"))) {
        list->Del(Folder->Folder());
        Del(Folder->Index());
        firstFolder = Get(isempty(dir) ? 0 : 1);
        Display();
        SetHelpKeys();
        nestedItemList->Save();
        }
     }
  return osContinue;
}

eOSState cMenuFolder::Edit(void)
{
  if (!HasSubMenu() && firstFolder) {
     cMenuFolderItem *Folder = (cMenuFolderItem *)Get(Current());
     if (Folder) {
        editing = true;
        return AddSubMenu(new cMenuEditFolder(dir, list, Folder->Folder()));
        }
     }
  return osContinue;
}

eOSState cMenuFolder::SetFolder(void)
{
  if (cMenuEditFolder *mef = (cMenuEditFolder *)SubMenu()) {
     Set(mef->GetFolder());
     SetHelpKeys();
     Display();
     nestedItemList->Save();
     }
  return CloseSubMenu();
}

cString cMenuFolder::GetFolder(void)
{
  if (firstFolder) {
     cMenuFolderItem *Folder = (cMenuFolderItem *)Get(Current());
     if (Folder) {
        if (cMenuFolder *mf = (cMenuFolder *)SubMenu())
           return cString::sprintf("%s%c%s", Folder->Folder()->Text(), FOLDERDELIMCHAR, *mf->GetFolder());
        return Folder->Folder()->Text();
        }
     }
  return "";
}

eOSState cMenuFolder::ProcessKey(eKeys Key)
{
  if (!HasSubMenu())
     editing = false;
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:
       case kRed:    return Select();
       case kGreen:  return New();
       case kYellow: return Delete();
       case kBlue:   return Edit();
       default:      state = osContinue;
       }
     }
  else if (state == osEnd && HasSubMenu() && editing)
     state = SetFolder();
  return state;
}

// --- cMenuEditTimer --------------------------------------------------------

cMenuEditTimer::cMenuEditTimer(cTimer *Timer, bool New)
:cOsdMenu(tr("Edit timer"), 12)
{
  fileItem = NULL;
  folderItem = NULL;
  firstday = NULL;
  timer = Timer;
  addIfConfirmed = New;
  if (timer) {
     data = *timer;
     if (New)
        data.SetFlags(tfActive);
     channel = data.Channel()->Number();

     path = data.file;
     *DirList.folder = 0;
     *name = 0;
     const char *s = strrchr(path, FOLDERDELIMCHAR);
     if (s) {
        strn0cpy(DirList.folder, cString(path, s), sizeof(DirList.folder));
        s++;
        }
     else
        s = path;
     strn0cpy(name, s, sizeof(name));

     Add(fileItem   = new cMenuEditStrItem(tr("File"), name, sizeof(name), tr(FileNameChars)));
     Add(folderItem = new cMenuEditStrItem(tr("Directory"), DirList.folder, sizeof(DirList.folder), tr(FileNameChars)));
     Add(new cMenuEditBitItem( tr("Active"),       &data.flags, tfActive));
     Add(new cMenuEditChanItem(tr("Channel"),      &channel));
     Add(new cMenuEditDateItem(tr("Day"),          &data.day, &data.weekdays));
     Add(new cMenuEditTimeItem(tr("Start"),        &data.start));
     Add(new cMenuEditTimeItem(tr("Stop"),         &data.stop));
     Add(new cMenuEditBitItem( tr("VPS"),          &data.flags, tfVps));
     Add(new cMenuEditIntItem( tr("Priority"),     &data.priority, 0, MAXPRIORITY));
     Add(new cMenuEditIntItem( tr("Lifetime"),     &data.lifetime, 0, MAXLIFETIME));

#ifdef USE_PINPLUGIN
     if (PinPatch::ChildLock::IsUnlocked())
        Add(new cMenuEditBitItem(tr("Timer$Childlock"), &data.flags, tfProtected));
#endif

     SetFirstDayItem();
     }
  Timers.IncBeingEdited();
}

cMenuEditTimer::~cMenuEditTimer()
{
  if (timer && addIfConfirmed)
     delete timer; // apparently it wasn't confirmed
  Timers.DecBeingEdited();
}

// Menu folder position: 0=File, 1=Directory (Folder)
#define MENU_FOLDER_POS 1

void cMenuEditTimer::SetHelpKeys(void)
{
  if (Current() == MENU_FOLDER_POS)
     SetHelp(NULL, NULL, NULL, tr("Button$Select"));
  else
     SetHelp(NULL, NULL, NULL, NULL);
}

void cMenuEditTimer::SetFirstDayItem(void)
{
  if (!firstday && !data.IsSingleEvent()) {
     Add(firstday = new cMenuEditDateItem(tr("First day"), &data.day));
     Display();
     }
  else if (firstday && data.IsSingleEvent()) {
     Del(firstday->Index());
     firstday = NULL;
     Display();
     }
}

eOSState cMenuEditTimer::SetFolder(void)
{
  if (cMenuFolder *mf = dynamic_cast<cMenuFolder *>(SubMenu())) {
     strn0cpy(DirList.folder, mf->GetFolder(), sizeof(DirList.folder));
     SetCurrent(folderItem);
     Display();
     }
  return CloseSubMenu();
}

eOSState cMenuEditTimer::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:     {
                       cChannel *ch = Channels.GetByNumber(channel);
                       if (ch)
                          data.channel = ch;
                       else {
                          Skins.Message(mtError, tr("*** Invalid Channel ***"));
                          break;
                          }
                       cString NewName;
                       if (!*name) // name must not be empty!
                          NewName = *DirList.folder ? cString::sprintf("%s%c%s", DirList.folder, FOLDERDELIMCHAR, strdup(data.Channel()->ShortName(true))) : strdup(data.Channel()->ShortName(true));
                       else
                          NewName = *DirList.folder ? cString::sprintf("%s%c%s", DirList.folder, FOLDERDELIMCHAR, name) : name;
                       NewName.CompactChars(FOLDERDELIMCHAR);
                       if (!*data.file)
                          strcpy(data.file, NewName);
                       else if (data.file != NewName)
                          strn0cpy(data.file, NewName, sizeof(data.file));
                       if (timer) {
                          if (memcmp((void *)timer, &data, sizeof(data)) != 0)
                             *timer = data;
                          if (addIfConfirmed)
                             Timers.Add(timer);
                          timer->SetEventFromSchedule();
                          timer->Matches();
                          Timers.SetModified();
                          isyslog("timer %s %s (%s)", *timer->ToDescr(), addIfConfirmed ? "added" : "modified", timer->HasFlags(tfActive) ? "active" : "inactive");
                          addIfConfirmed = false;
                          }
                     }
                     return osBack;
       case kRed:
       case kGreen:
       case kYellow: return osContinue;
       case kBlue:
                     if (Current() != MENU_FOLDER_POS)
                        return osContinue;
                     if (cPluginManager::CallFirstService("EpgsearchDirectoryList-v1.0", &DirList))
                        return AddSubMenu(DirList.Menu);
                     return AddSubMenu(new cMenuFolder(tr("Select folder"), &Folders, name));
       default: break;
       }
     }
  else if (state == osEnd && HasSubMenu())
     state = SetFolder();
  if (Key != kNone) {
     SetFirstDayItem();
     if (!HasSubMenu() && !fileItem->InEditMode() && !folderItem->InEditMode())
        SetHelpKeys();
     }
  return state;
}

// --- cMenuCommands ---------------------------------------------------------
// declaration shifted so it can be used in cMenuTimers
class cMenuCommands : public cOsdMenu {
private:
  cList<cNestedItem> *commands;
  cString parameters;
  cString title;
  cString command;
  bool confirm;
  char *result;
  bool Parse(const char *s);
  eOSState Execute(void);
public:
  cMenuCommands(const char *Title, cList<cNestedItem> *Commands, const char *Parameters = NULL);
  virtual ~cMenuCommands();
  virtual eOSState ProcessKey(eKeys Key);
  };

// --- cMenuTimerItem --------------------------------------------------------
class cMenuTimerItem : public cOsdItem {
private:
  cTimer *timer;
  void DoSet(void);
  char diskStatus;
public:
  cMenuTimerItem(cTimer *Timer);
  void SetDiskStatus(char DiskStatus);
  virtual int Compare(const cListObject &ListObject) const;
  virtual void Set(void);
  cTimer *Timer(void) { return timer; }
  };

cMenuTimerItem::cMenuTimerItem(cTimer *Timer)
{
  timer = Timer;
  diskStatus = ' ';
  DoSet();
}

int cMenuTimerItem::Compare(const cListObject &ListObject) const
{
  return timer->Compare(*((cMenuTimerItem *)&ListObject)->timer);
}

void cMenuTimerItem::Set(void)
{
  // check for deleted timer
  for (cTimer *t = Timers.First(); ; t = Timers.Next(t)) {
     if (t == timer)
       break;  // timer still there
     if (t == NULL)
       return; // no matching timer found
     }
  DoSet();
}

void cMenuTimerItem::DoSet(void)
{
  cString day, name("");
  if (timer->WeekDays())
     day = timer->PrintDay(0, timer->WeekDays());
  else if (timer->Day() - time(NULL) < 28 * SECSINDAY) {
     day = itoa(timer->GetMDay(timer->Day()));
     name = WeekDayName(timer->Day());
     }
  else {
     struct tm tm_r;
     time_t Day = timer->Day();
     localtime_r(&Day, &tm_r);
     char buffer[16];
     strftime(buffer, sizeof(buffer), "%Y%m%d", &tm_r);
     day = buffer;
     }
  const char *File = Setup.FoldersInTimerMenu ? NULL : strrchr(timer->File(), FOLDERDELIMCHAR);
  if (File && strcmp(File + 1, TIMERMACRO_TITLE) && strcmp(File + 1, TIMERMACRO_EPISODE))
     File++;
  else
     File = timer->File();
  SetText(cString::sprintf("%c%c\t%d\t%s%s%s\t%02d:%02d\t%02d:%02d\t%s",
                    diskStatus,
                    !(timer->HasFlags(tfActive)) ? ' ' : timer->FirstDay() ? Setup.WarEagleIcons ? ICON_PFEIL : '!' : timer->Recording() ? Setup.WarEagleIcons ? ICON_REC : '#' : Setup.WarEagleIcons ? ICON_UHR : '>',
                    timer->Channel()->Number(),
                    *name,
                    *name && **name ? " " : "",
                    *day,
                    timer->Start() / 100,
                    timer->Start() % 100,
                    timer->Stop() / 100,
                    timer->Stop() % 100,
                    File));
}

void cMenuTimerItem::SetDiskStatus(char DiskStatus)
{
  diskStatus = DiskStatus;
  Set();
}

// --- cTimerEntry -----------------------------------------------------------

class cTimerEntry : public cListObject {
private:
  cMenuTimerItem *item;
  const cTimer *timer;
  time_t start;
public:
  cTimerEntry(cMenuTimerItem *item) : item(item), timer(item->Timer()), start(timer->StartTime()) {}
  cTimerEntry(const cTimer *timer, time_t start) : item(NULL), timer(timer), start(start) {}
  virtual int Compare(const cListObject &ListObject) const;
  bool active(void) const { return timer->HasFlags(tfActive); }
  time_t startTime(void) const { return start; }
  int priority(void) const { return timer->Priority(); }
  int duration(void) const;
  bool repTimer(void) const { return !timer->IsSingleEvent(); }
  bool isDummy(void) const { return item == NULL; }
  const cTimer *Timer(void) const { return timer; }
  void SetDiskStatus(char DiskStatus);
  };

int cTimerEntry::Compare(const cListObject &ListObject) const
{
  cTimerEntry *entry = (cTimerEntry *)&ListObject;
  int r = startTime() - entry->startTime();
  if (r == 0)
     r = entry->priority() - priority();
  return r;
}

int cTimerEntry::duration(void) const
{
  int dur = (timer->Stop()  / 100 * 60 + timer->Stop()  % 100) -
            (timer->Start() / 100 * 60 + timer->Start() % 100);
  if (dur < 0)
     dur += 24 * 60;
  return dur;
}

void cTimerEntry::SetDiskStatus(char DiskStatus)
{
  if (item)
     item->SetDiskStatus(DiskStatus);
}

// --- cMenuTimers -----------------------------------------------------------

class cMenuTimers : public cOsdMenu {
private:
  int helpKeys;
  eOSState Edit(bool New = false);
  eOSState Delete(void);
  eOSState OnOff(void);
  eOSState Info(void);
  cTimer *CurrentTimer(void);
  void SetHelpKeys(void);
  void ActualiseDiskStatus(void);
  bool actualiseDiskStatus;
public:
  cMenuTimers(void);
  virtual ~cMenuTimers();
  virtual void Display(void);
  virtual eOSState Commands(eKeys Key = kNone);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuTimers::cMenuTimers(void)
:cOsdMenu(tr("Timers"), 3, CHNUMWIDTH, 10, 6, 6)
{
  helpKeys = -1;
  for (cTimer *timer = Timers.First(); timer; timer = Timers.Next(timer)) {
      timer->SetEventFromSchedule(); // make sure the event is current
      Add(new cMenuTimerItem(timer));
      }
  Sort();
  SetCurrent(First());
  SetHelpKeys();
  Timers.IncBeingEdited();
  actualiseDiskStatus = true;
}

cMenuTimers::~cMenuTimers()
{
  Timers.DecBeingEdited();
}

cTimer *cMenuTimers::CurrentTimer(void)
{
  cMenuTimerItem *item = (cMenuTimerItem *)Get(Current());
  return item ? item->Timer() : NULL;
}

void cMenuTimers::SetHelpKeys(void)
{
  int NewHelpKeys = 0;
  cTimer *timer = CurrentTimer();
  if (timer) {
     if (timer->Event())
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     }
  if (NewHelpKeys != helpKeys) {
     helpKeys = NewHelpKeys;
     SetHelp(helpKeys > 0 ? tr("Button$On/Off") : NULL, tr("Button$New"), helpKeys > 0 ? tr("Button$Delete") : NULL, helpKeys > 0 ? tr("Button$Edit") : NULL);
     }
}

eOSState cMenuTimers::OnOff(void)
{
  if (HasSubMenu())
     return osContinue;
  cTimer *timer = CurrentTimer();
  if (timer) {
     timer->OnOff();
     timer->SetEventFromSchedule();
     RefreshCurrent();
     Display();
     if (timer->FirstDay())
        isyslog("timer %s first day set to %s", *timer->ToDescr(), *timer->PrintFirstDay());
     else
        isyslog("timer %s %sactivated", *timer->ToDescr(), timer->HasFlags(tfActive) ? "" : "de");
     Timers.SetModified();
     }
  return osContinue;
}

eOSState cMenuTimers::Edit(bool New)
{
  if (HasSubMenu() || (Count() == 0 && !New))
     return osContinue;
  if (!New)
     isyslog("editing timer %s", *CurrentTimer()->ToDescr());

  // Data structure for service "Epgsearch-exttimeredit-v1.0"
  struct Epgsearch_exttimeredit_v1_0
  {
    // in
    cTimer* timer;          // pointer to the timer to edit
    bool bNew;              // flag that indicates, if this is a new timer or an existing one
    const cEvent* event;    // pointer to the event corresponding to this timer (may be NULL)
    // out
    cOsdMenu* pTimerMenu;   // pointer to the menu of results
  } exttimeredit;
  exttimeredit.timer = New ? (new cTimer) : CurrentTimer();
  exttimeredit.bNew = New;
  exttimeredit.event = exttimeredit.timer->Event();
  if (cPluginManager::CallFirstService("Epgsearch-exttimeredit-v1.0", &exttimeredit))
    return AddSubMenu(exttimeredit.pTimerMenu);

  return AddSubMenu(new cMenuEditTimer(exttimeredit.timer, New));
}

eOSState cMenuTimers::Delete(void)
{
  // Check if this timer is active:
  cTimer *ti = CurrentTimer();
  if (ti) {
     if (Interface->Confirm(tr("Delete timer?"))) {
        if (ti->Recording()) {
           if (Interface->Confirm(tr("Timer still recording - really delete?"))) {
              ti->Skip();
              cRecordControls::Process(time(NULL));
              }
           else
              return osContinue;
           }
        isyslog("deleting timer %s", *ti->ToDescr());
        Timers.Del(ti);
        cOsdMenu::Del(Current());
        Timers.SetModified();
        Display();
        }
     }
  return osContinue;
}

eOSState cMenuTimers::Info(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cTimer *ti = CurrentTimer();
  if (ti && ti->Event())
     return AddSubMenu(new cMenuEvent(ti->Event()));
  return osContinue;
}

void cMenuTimers::ActualiseDiskStatus(void)
{
  if (!actualiseDiskStatus || !Count())
     return;

  // compute free disk space
  int freeMB, freeMinutes, runshortMinutes;
  VideoDiskSpace(&freeMB);
  freeMinutes = int(double(freeMB) * 1.1 / MB_PER_MINUTE); // overestimate by 10 percent
  runshortMinutes = freeMinutes / 5; // 20 Percent

  // fill entries list
  cTimerEntry *entry;
  cList<cTimerEntry> entries;
  for (cOsdItem *item = First(); item; item = Next(item))
     entries.Add(new cTimerEntry((cMenuTimerItem *)item));

  // search last start time
  time_t last = 0;
  for (entry = entries.First(); entry; entry = entries.Next(entry))
     last = std::max(entry->startTime(), last);

  // add entries for repeating timers
  for (entry = entries.First(); entry; entry = entries.Next(entry))
     if (entry->repTimer() && !entry->isDummy())
        for (time_t start = cTimer::IncDay(entry->startTime(), 1);
             start <= last;
             start = cTimer::IncDay(start, 1))
           if (entry->Timer()->DayMatches(start))
              entries.Add(new cTimerEntry(entry->Timer(), start));

  // set the disk-status
  entries.Sort();
  for (entry = entries.First(); entry; entry = entries.Next(entry)) {
     char status = ' ';
     if (entry->active()) {
        freeMinutes -= entry->duration();
        status = freeMinutes > runshortMinutes ? '+' : freeMinutes > 0 ? 177 /* +/- */ : '-';
        }
     entry->SetDiskStatus(status);
#ifdef DEBUG_TIMER_INFO
     dsyslog("timer-info: %c | %d | %s | %s | %3d | %+5d -> %+5d",
             status,
             entry->startTime(),
             entry->active() ? "aktiv " : "n.akt.",
             entry->repTimer() ? entry->isDummy() ? "  dummy  " : "mehrmalig" : "einmalig ",
             entry->duration(),
             entry->active() ? freeMinutes + entry->duration() : freeMinutes,
             freeMinutes);
#endif
     }

  actualiseDiskStatus = false;
}

void cMenuTimers::Display(void)
{
  ActualiseDiskStatus();
  cOsdMenu::Display();
}

#define CHECK_2PTR_NULL(x_,y_) ((x_)? ((y_)? y_:""):"")

eOSState cMenuTimers::Commands(eKeys Key)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cTimer *ti = CurrentTimer();
  if (ti) {
     cString parameter;
     const cEvent *pEvent = ti->Event();
     int iRecNumber=0;

     if(!pEvent) {
        Timers.SetEvents();
        pEvent = ti->Event();
     }
     if(pEvent) {
// create a dummy recording to get the real filename
        cRecording *rc_dummy = new cRecording(ti, pEvent);
        Recordings.Load();
        cRecording *rc = Recordings.GetByName(rc_dummy->FileName());

        delete rc_dummy;
        if(rc)
           iRecNumber=rc->Index() + 1;
     }
//Parameter format TimerNumber 'ChannelId' Start Stop 'Titel' 'Subtitel' 'file' RecNumer
//                 1           2           3     4    5       6          7      8
     parameter = cString::sprintf("%d '%s' %d %d '%s' '%s' '%s' %d", ti->Index(),
                                                             *ti->Channel()->GetChannelID().ToString(),
                                                             (int)ti->StartTime(),
                                                             (int)ti->StopTime(),
                                                             CHECK_2PTR_NULL(pEvent, pEvent->Title()),
                                                             CHECK_2PTR_NULL(pEvent, pEvent->ShortText()),
                                                             ti->File(),
                                                             iRecNumber);
     isyslog("timercmd: %s", *parameter);
     cMenuCommands *menu;
     eOSState state = AddSubMenu(menu = new cMenuCommands(tr("Timer commands"), &TimerCommands, parameter));
     if (Key != kNone)
        state = menu->ProcessKey(Key);
     return state;
     }
  return osContinue;
}

eOSState cMenuTimers::ProcessKey(eKeys Key)
{
  int TimerNumber = HasSubMenu() ? Count() : -1;
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kBlue:
       case kOk:     return Edit();
       case kRed:    actualiseDiskStatus = true;
                     state = OnOff(); break; // must go through SetHelpKeys()!
       case kGreen:  return Edit(true);
       case kYellow: actualiseDiskStatus = true;
                     state = Delete(); break;
       case kInfo:   return Info();
                     break;
       case k1...k9: return Commands(Key);
       case k0:      return (TimerCommands.Count()? Commands():osContinue);
       default: break;
       }
     }
  if (TimerNumber >= 0 && !HasSubMenu()) {
     if (Timers.Get(TimerNumber)) // a newly created timer was confirmed with Ok
        Add(new cMenuTimerItem(Timers.Get(TimerNumber)), true);
     Sort();
     actualiseDiskStatus = true;
     Display();
     }
  if (!HasSubMenu() && Timers.Count()<Count()) {
     // timer was deleted
     cOsdMenu::Del(Current());
     Display();
     }
  if (Key != kNone)
     SetHelpKeys();
  return state;
}

// --- cMenuEvent ------------------------------------------------------------

cMenuEvent::cMenuEvent(const cEvent *Event, bool CanSwitch, bool Buttons)
:cOsdMenu(tr("Event"))
{
  event = Event;
  if (event) {
     cChannel *channel = Channels.GetByChannelID(event->ChannelID(), true);
     if (channel) {
        SetTitle(channel->Name());
        if (Buttons) {
           int TimerMatch = tmNone;
           Timers.GetMatch(event, &TimerMatch);
           SetHelp(TimerMatch == tmFull ? tr("Button$Timer") : tr("Button$Record"), NULL, NULL, CanSwitch ? tr("Button$Switch") : NULL);
           }
        }
     }
}

void cMenuEvent::Display(void)
{
  cOsdMenu::Display();
  DisplayMenu()->SetEvent(event);
  if (event->Description())
     cStatus::MsgOsdTextItem(event->Description());
}

eOSState cMenuEvent::ProcessKey(eKeys Key)
{
  switch (int(Key)) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
                  DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
                  cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft);
                  return osContinue;
    case kInfo:   return osBack;
    default: break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kGreen:
       case kYellow: return osContinue;
       case kOk:     return osBack;
       default: break;
       }
     }
  return state;
}

// --- cMenuScheduleItem -----------------------------------------------------

class cMenuScheduleItem : public cOsdItem {
public:
  enum eScheduleSortMode { ssmAllThis, ssmThisThis, ssmThisAll, ssmAllAll }; // "which event(s) on which channel(s)"
private:
  static eScheduleSortMode sortMode;
public:
  const cEvent *event;
  const cChannel *channel;
  bool withDate;
  int timerMatch;
  bool withBar;
  cMenuScheduleItem(const cEvent *Event, cChannel *Channel = NULL, bool WithDate = false, bool WithBar = false);
  static void SetSortMode(eScheduleSortMode SortMode) { sortMode = SortMode; }
  static void IncSortMode(void) { sortMode = eScheduleSortMode((sortMode == ssmAllAll) ? ssmAllThis : sortMode + 1); }
  static eScheduleSortMode SortMode(void) { return sortMode; }
  virtual int Compare(const cListObject &ListObject) const;
  bool Update(bool Force = false);
  };

cMenuScheduleItem::eScheduleSortMode cMenuScheduleItem::sortMode = ssmAllThis;

cMenuScheduleItem::cMenuScheduleItem(const cEvent *Event, cChannel *Channel, bool WithDate, bool WithBar)
{
  event = Event;
  channel = Channel;
  withDate = WithDate;
  timerMatch = tmNone;
  withBar = WithBar;
  Update(true);
}

int cMenuScheduleItem::Compare(const cListObject &ListObject) const
{
  cMenuScheduleItem *p = (cMenuScheduleItem *)&ListObject;
  int r = -1;
  if (sortMode != ssmAllThis)
     r = strcoll(event->Title(), p->event->Title());
  if (sortMode == ssmAllThis || r == 0)
     r = event->StartTime() - p->event->StartTime();
  return r;
}

static const char *TimerMatchChars = " tT";

static const char * const ProgressBar[7] =
{
  "[      ]",
  "[|     ]",
  "[||    ]",
  "[|||   ]",
  "[||||  ]",
  "[||||| ]",
  "[||||||]"
};

bool cMenuScheduleItem::Update(bool Force)
{
  bool result = false;
  int OldTimerMatch = timerMatch;
  Timers.GetMatch(event, &timerMatch);
  if (Force || timerMatch != OldTimerMatch) {
     cString buffer;
     char t = TimerMatchChars[timerMatch];
     char v = event->Vps() ? (event->Vps() - event->StartTime() ? 'V' : 'v' ) : ' ';
     char r = event->SeenWithin(30) && event->IsRunning() ? '*' : ' ';
     if (channel && withDate)
        buffer = cString::sprintf("%d\t%.*s\t%.*s\t%s\t%c%c%c\t%s", channel->Number(), 6, channel->ShortName(true), 6, *event->GetDateString(), *event->GetTimeString(), t, v, r, event->Title());
     else if (channel)
        if (Setup.ShowProgressBar && withBar) {
           int progress = (int)roundf( (float)(time(NULL) - event->StartTime()) / (float)(event->Duration()) * 6.0 );
           if (progress < 0) progress = 0;
           else if (progress > 6) progress = 6;
           buffer = cString::sprintf("%d\t%.*s\t%s\t%s\t%c%c%c\t%s", channel->Number(), 6, channel->ShortName(true), *event->GetTimeString(), ProgressBar[progress], t, v, r, event->Title());
           }
        else
           buffer = cString::sprintf("%d\t%.*s\t%s\t%c%c%c\t%s", channel->Number(), 6, channel->ShortName(true), *event->GetTimeString(), t, v, r, event->Title());
     else
        buffer = cString::sprintf("%.*s\t%s\t%c%c%c\t%s", 6, *event->GetDateString(), *event->GetTimeString(), t, v, r, event->Title());
     SetText(buffer);
     result = true;
     }
  return result;
}

// --- cMenuWhatsOn ----------------------------------------------------------

class cMenuWhatsOn : public cOsdMenu {
private:
  bool now;
  int helpKeys;
  int timerState;
  eOSState Record(void);
  eOSState Switch(void);
  static int currentChannel;
  static const cEvent *scheduleEvent;
  bool Update(void);
  void SetHelpKeys(void);
public:
  cMenuWhatsOn(const cSchedules *Schedules, bool Now, int CurrentChannelNr);
  static int CurrentChannel(void) { return currentChannel; }
  static void SetCurrentChannel(int ChannelNr) { currentChannel = ChannelNr; }
  static const cEvent *ScheduleEvent(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

int cMenuWhatsOn::currentChannel = 0;
const cEvent *cMenuWhatsOn::scheduleEvent = NULL;

cMenuWhatsOn::cMenuWhatsOn(const cSchedules *Schedules, bool Now, int CurrentChannelNr)
:cOsdMenu(Now ? tr("What's on now?") : tr("What's on next?"), CHNUMWIDTH, 7, 6, 4, 4)
{
  now = Now;
  helpKeys = -1;
  timerState = 0;
  Timers.Modified(timerState);
  for (cChannel *Channel = Channels.First(); Channel; Channel = Channels.Next(Channel)) {
      if (!Channel->GroupSep()) {
         const cSchedule *Schedule = Schedules->GetSchedule(Channel);
         if (Schedule) {
            const cEvent *Event = Now ? Schedule->GetPresentEvent() : Schedule->GetFollowingEvent();
            if (Event)
               Add(new cMenuScheduleItem(Event, Channel, false, Now), Channel->Number() == CurrentChannelNr);
            }
         }
      }
  currentChannel = CurrentChannelNr;
  Display();
  SetHelpKeys();
}

bool cMenuWhatsOn::Update(void)
{
  bool result = false;
  if (Timers.Modified(timerState)) {
     for (cOsdItem *item = First(); item; item = Next(item)) {
         if (((cMenuScheduleItem *)item)->Update())
            result = true;
         }
     }
  return result;
}

void cMenuWhatsOn::SetHelpKeys(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  int NewHelpKeys = 0;
  if (item) {
     if (item->timerMatch == tmFull)
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     }
  if (NewHelpKeys != helpKeys) {
     const char *Red[] = { NULL, tr("Button$Record"), tr("Button$Timer") };
     SetHelp(Red[NewHelpKeys], now ? tr("Button$Next") : tr("Button$Now"), tr("Button$Schedule"), tr("Button$Switch"));
     helpKeys = NewHelpKeys;
     }
}

const cEvent *cMenuWhatsOn::ScheduleEvent(void)
{
  const cEvent *ei = scheduleEvent;
  scheduleEvent = NULL;
  return ei;
}

eOSState cMenuWhatsOn::Switch(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  if (item) {
     cChannel *channel = Channels.GetByChannelID(item->event->ChannelID(), true);
     if (channel && cDevice::PrimaryDevice()->SwitchChannel(channel, true))
        return osEnd;
     }
  Skins.Message(mtError, tr("Can't switch channel!"));
  return osContinue;
}

eOSState cMenuWhatsOn::Record(void)
{
  if (cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current())) {
     if (item->timerMatch == tmFull) {
        int tm = tmNone;
        cTimer *timer = Timers.GetMatch(item->event, &tm);
        if (timer)
           return AddSubMenu(new cMenuEditTimer(timer));
        }
     cTimer *timer = new cTimer(item->event);
     cTimer *t = Timers.GetTimer(timer);
     if (t) {
        delete timer;
        timer = t;
        return AddSubMenu(new cMenuEditTimer(timer));
        }
     else {
        Timers.Add(timer);
        Timers.SetModified();
        isyslog("timer %s added (active)", *timer->ToDescr());
        if (timer->Matches(0, false, NEWTIMERLIMIT))
           return AddSubMenu(new cMenuEditTimer(timer));
        if (HasSubMenu())
           CloseSubMenu();
        if (Update())
           Display();
        SetHelpKeys();
        }
     }
  return osContinue;
}

eOSState cMenuWhatsOn::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRecord:
       case kRed:    return Record();
       case kYellow: state = osBack;
                     // continue with kGreen
       case kGreen:  {
                       cMenuScheduleItem *mi = (cMenuScheduleItem *)Get(Current());
                       if (mi) {
                          scheduleEvent = mi->event;
                          currentChannel = mi->channel->Number();
                          }
                     }
                     break;
       case kBlue:   return Switch();
       case kInfo:
       case kOk:     if (Count())
                        return AddSubMenu(new cMenuEvent(((cMenuScheduleItem *)Get(Current()))->event, true, true));
                     break;
       default:      break;
       }
     }
  else if (!HasSubMenu()) {
     if (HadSubMenu && Update())
        Display();
     if (Key != kNone)
        SetHelpKeys();
     }
  return state;
}

// --- cMenuSchedule ---------------------------------------------------------

class cMenuSchedule : public cOsdMenu {
private:
  cSchedulesLock schedulesLock;
  const cSchedules *schedules;
  bool now, next;
  int otherChannel;
  int helpKeys;
  int timerState;
  eOSState Number(void);
  eOSState Record(void);
  eOSState Switch(void);
  void PrepareScheduleAllThis(const cEvent *Event, const cChannel *Channel);
  void PrepareScheduleThisThis(const cEvent *Event, const cChannel *Channel);
  void PrepareScheduleThisAll(const cEvent *Event, const cChannel *Channel);
  void PrepareScheduleAllAll(const cEvent *Event, const cChannel *Channel);
  bool Update(void);
  void SetHelpKeys(void);
public:
  cMenuSchedule(void);
  virtual ~cMenuSchedule();
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuSchedule::cMenuSchedule(void)
:cOsdMenu(tr("Schedule")), schedulesLock(false, 500)
{
  now = next = false;
  otherChannel = 0;
  helpKeys = -1;
  timerState = 0;
  Timers.Modified(timerState);
  cMenuScheduleItem::SetSortMode(cMenuScheduleItem::ssmAllThis);
  cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
  if (channel) {
     cMenuWhatsOn::SetCurrentChannel(channel->Number());
     schedules = cSchedules::Schedules(schedulesLock);
     PrepareScheduleAllThis(NULL, channel);
     SetHelpKeys();
     }
}

cMenuSchedule::~cMenuSchedule()
{
  cMenuWhatsOn::ScheduleEvent(); // makes sure any posted data is cleared
}

void cMenuSchedule::PrepareScheduleAllThis(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(7, 6, 4);
  SetTitle(cString::sprintf(tr("Schedule - %s"), Channel->Name()));
  if (schedules && Channel) {
     if (const cSchedule *Schedule = schedules->GetSchedule(Channel)) {
        const cEvent *PresentEvent = Event ? Event : Schedule->GetPresentEvent();
        time_t now = time(NULL) - Setup.EPGLinger * 60;
        for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
            if (ev->EndTime() > now || ev == PresentEvent)
               Add(new cMenuScheduleItem(ev), ev == PresentEvent);
            }
        }
     }
}

void cMenuSchedule::PrepareScheduleThisThis(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(7, 6, 4);
  SetTitle(cString::sprintf(tr("This event - %s"), Channel->Name()));
  if (schedules && Channel && Event) {
     if (const cSchedule *Schedule = schedules->GetSchedule(Channel)) {
        time_t now = time(NULL) - Setup.EPGLinger * 60;
        for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
            if ((ev->EndTime() > now || ev == Event) && !strcmp(ev->Title(), Event->Title()))
               Add(new cMenuScheduleItem(ev), ev == Event);
            }
        }
     }
}

void cMenuSchedule::PrepareScheduleThisAll(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(CHNUMWIDTH, 7, 7, 6, 4);
  SetTitle(tr("This event - all channels"));
  if (schedules && Event) {
     for (cChannel *ch = Channels.First(); ch; ch = Channels.Next(ch)) {
         const cSchedule *Schedule = schedules->GetSchedule(ch);
         if (Schedule) {
            time_t now = time(NULL) - Setup.EPGLinger * 60;
            for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
                if ((ev->EndTime() > now || ev == Event) && !strcmp(ev->Title(), Event->Title()))
                   Add(new cMenuScheduleItem(ev, ch, true), ev == Event && ch == Channel);
                }
            }
         }
     }
}

void cMenuSchedule::PrepareScheduleAllAll(const cEvent *Event, const cChannel *Channel)
{
  Clear();
  SetCols(CHNUMWIDTH, 7, 7, 6, 4);
  SetTitle(tr("All events - all channels"));
  if (schedules) {
     for (cChannel *ch = Channels.First(); ch; ch = Channels.Next(ch)) {
         const cSchedule *Schedule = schedules->GetSchedule(ch);
         if (Schedule) {
            time_t now = time(NULL) - Setup.EPGLinger * 60;
            for (const cEvent *ev = Schedule->Events()->First(); ev; ev = Schedule->Events()->Next(ev)) {
                if (ev->EndTime() > now || ev == Event)
                   Add(new cMenuScheduleItem(ev, ch, true), ev == Event && ch == Channel);
                }
            }
         }
     }
}

bool cMenuSchedule::Update(void)
{
  bool result = false;
  if (Timers.Modified(timerState)) {
     for (cOsdItem *item = First(); item; item = Next(item)) {
         if (((cMenuScheduleItem *)item)->Update())
            result = true;
         }
     }
  return result;
}

void cMenuSchedule::SetHelpKeys(void)
{
  cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current());
  int NewHelpKeys = 0;
  if (item) {
     if (item->timerMatch == tmFull)
        NewHelpKeys = 2;
     else
        NewHelpKeys = 1;
     }
  if (NewHelpKeys != helpKeys) {
     const char *Red[] = { NULL, tr("Button$Record"), tr("Button$Timer") };
     SetHelp(Red[NewHelpKeys], tr("Button$Now"), tr("Button$Next"));
     helpKeys = NewHelpKeys;
     }
}

eOSState cMenuSchedule::Number(void)
{
  cMenuScheduleItem::IncSortMode();
  cMenuScheduleItem *CurrentItem = (cMenuScheduleItem *)Get(Current());
  const cChannel *Channel = NULL;
  const cEvent *Event = NULL;
  if (CurrentItem) {
     Event = CurrentItem->event;
     Channel = Channels.GetByChannelID(Event->ChannelID(), true);
     }
  else
     Channel = Channels.GetByNumber(cDevice::CurrentChannel());
  switch (cMenuScheduleItem::SortMode()) {
    case cMenuScheduleItem::ssmAllThis:  PrepareScheduleAllThis(Event, Channel); break;
    case cMenuScheduleItem::ssmThisThis: PrepareScheduleThisThis(Event, Channel); break;
    case cMenuScheduleItem::ssmThisAll:  PrepareScheduleThisAll(Event, Channel); break;
    case cMenuScheduleItem::ssmAllAll:   PrepareScheduleAllAll(Event, Channel); break;
    default: esyslog("ERROR: unknown SortMode %d (%s %d)", cMenuScheduleItem::SortMode(), __FUNCTION__, __LINE__);
    }
  CurrentItem = (cMenuScheduleItem *)Get(Current());
  Sort();
  SetCurrent(CurrentItem);
  Display();
  return osContinue;
}

eOSState cMenuSchedule::Record(void)
{
  if (cMenuScheduleItem *item = (cMenuScheduleItem *)Get(Current())) {
     if (item->timerMatch == tmFull) {
        int tm = tmNone;
        cTimer *timer = Timers.GetMatch(item->event, &tm);
        if (timer)
           return AddSubMenu(new cMenuEditTimer(timer));
        }
     cTimer *timer = new cTimer(item->event);
     if (cTimer *t = Timers.GetTimer(timer)) {
        delete timer;
        timer = t;
        return AddSubMenu(new cMenuEditTimer(timer));
        }
     else {
        Timers.Add(timer);
        Timers.SetModified();
        isyslog("timer %s added (active)", *timer->ToDescr());
        if (timer->Matches(0, false, NEWTIMERLIMIT))
           return AddSubMenu(new cMenuEditTimer(timer));
        if (HasSubMenu())
           CloseSubMenu();
        if (Update())
           Display();
        SetHelpKeys();
        }
     }
  return osContinue;
}

eOSState cMenuSchedule::Switch(void)
{
  if (otherChannel) {
     if (Channels.SwitchTo(otherChannel))
        return osEnd;
     }
  Skins.Message(mtError, tr("Can't switch channel!"));
  return osContinue;
}

eOSState cMenuSchedule::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case k0:      return Number();
       case kRecord:
       case kRed:    return Record();
       case kGreen:  if (schedules) {
                        if (!now && !next) {
                           int ChannelNr = 0;
                           if (Count()) {
                              cChannel *channel = Channels.GetByChannelID(((cMenuScheduleItem *)Get(Current()))->event->ChannelID(), true);
                              if (channel)
                                 ChannelNr = channel->Number();
                              }
                           now = true;
                           return AddSubMenu(new cMenuWhatsOn(schedules, true, ChannelNr));
                           }
                        now = !now;
                        next = !next;
                        return AddSubMenu(new cMenuWhatsOn(schedules, now, cMenuWhatsOn::CurrentChannel()));
                        }
       case kYellow: if (schedules)
                        return AddSubMenu(new cMenuWhatsOn(schedules, false, cMenuWhatsOn::CurrentChannel()));
                     break;
       case kBlue:   if (Count() && otherChannel)
                        return Switch();
                     break;
       case kInfo:
       case kOk:     if (Count())
                        return AddSubMenu(new cMenuEvent(((cMenuScheduleItem *)Get(Current()))->event, otherChannel, true));
                     break;
       default:      break;
       }
     }
  else if (!HasSubMenu()) {
     now = next = false;
     const cEvent *ei = cMenuWhatsOn::ScheduleEvent();
     if (ei) {
        cChannel *channel = Channels.GetByChannelID(ei->ChannelID(), true);
        if (channel) {
           cMenuScheduleItem::SetSortMode(cMenuScheduleItem::ssmAllThis);
           PrepareScheduleAllThis(NULL, channel);
           if (channel->Number() != cDevice::CurrentChannel()) {
              otherChannel = channel->Number();
              SetHelp(Count() ? tr("Button$Record") : NULL, tr("Button$Now"), tr("Button$Next"), tr("Button$Switch"));
              }
           Display();
           }
        }
     else if (HadSubMenu && Update())
        Display();
     if (Key != kNone)
        SetHelpKeys();
     }
  return state;
}

// --- cMenuCommands ---------------------------------------------------------

cMenuCommands::cMenuCommands(const char *Title, cList<cNestedItem> *Commands, const char *Parameters)
:cOsdMenu(Title)
{
  result = NULL;
  SetHasHotkeys();
  commands = Commands;
  parameters = Parameters;
  for (cNestedItem *Command = commands->First(); Command; Command = commands->Next(Command)) {
      const char *s = Command->Text();
      if (Command->SubItems())
         Add(new cOsdItem(hk(cString::sprintf("%s...", s))));
      else if (Parse(s))
         Add(new cOsdItem(hk(title)));
      }
}

cMenuCommands::~cMenuCommands()
{
  free(result);
}

bool cMenuCommands::Parse(const char *s)
{
  const char *p = strchr(s, ':');
  if (p) {
     int l = p - s;
     if (l > 0) {
        char t[l + 1];
        stripspace(strn0cpy(t, s, l + 1));
        l = strlen(t);
        if (l > 1 && t[l - 1] == '?') {
           t[l - 1] = 0;
           confirm = true;
           }
        else
           confirm = false;
        title = t;
        command = skipspace(p + 1);
        return true;
        }
     }
  return false;
}

eOSState cMenuCommands::Execute(void)
{
  cNestedItem *Command = commands->Get(Current());
  if (Command) {
     if (Command->SubItems())
        return AddSubMenu(new cMenuCommands(Title(), Command->SubItems(), parameters));
     if (Parse(Command->Text())) {
        if (!confirm || Interface->Confirm(cString::sprintf("%s?", *title))) {
           Skins.Message(mtStatus, cString::sprintf("%s...", *title));
           free(result);
           result = NULL;
           cString cmdbuf;
           if (!isempty(parameters))
              cmdbuf = cString::sprintf("%s %s", *command, *parameters);
           const char *cmd = *cmdbuf ? *cmdbuf : *command;
           dsyslog("executing command '%s'", cmd);
           cPipe p;
           if (p.Open(cmd, "r")) {
              int l = 0;
              int c;
              while ((c = fgetc(p)) != EOF) {
                    if (l % 20 == 0) {
                       if (char *NewBuffer = (char *)realloc(result, l + 21))
                          result = NewBuffer;
                       else {
                          esyslog("ERROR: out of memory");
                          break;
                          }
                       }
                    result[l++] = char(c);
                    }
              if (result)
                 result[l] = 0;
              p.Close();
              }
           else
              esyslog("ERROR: can't open pipe for command '%s'", cmd);
           Skins.Message(mtStatus, NULL);
           if (result)
              return AddSubMenu(new cMenuText(title, result, fontFix));
           return osEnd;
           }
        }
     }
  return osContinue;
}

eOSState cMenuCommands::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRed:
       case kGreen:
       case kYellow:
       case kBlue:   return osContinue;
       case kOk:     return Execute();
       default:      break;
       }
     }
  return state;
}

// --- cMenuCam --------------------------------------------------------------

cMenuCam::cMenuCam(cCiMenu *CiMenu)
:cOsdMenu("")
{
  dsyslog("CAM: Menu ------------------");
  ciMenu = CiMenu;
  selected = false;
  offset = 0;
  if (ciMenu->Selectable())
     SetHasHotkeys();
  SetTitle(*ciMenu->TitleText() ? ciMenu->TitleText() : "CAM");
  dsyslog("CAM: '%s'", ciMenu->TitleText());
  if (*ciMenu->SubTitleText()) {
     dsyslog("CAM: '%s'", ciMenu->SubTitleText());
     AddMultiLineItem(ciMenu->SubTitleText());
     offset = Count();
     }
  for (int i = 0; i < ciMenu->NumEntries(); i++) {
      Add(new cOsdItem(hk(ciMenu->Entry(i)), osUnknown, ciMenu->Selectable()));
      dsyslog("CAM: '%s'", ciMenu->Entry(i));
      }
  if (*ciMenu->BottomText()) {
     AddMultiLineItem(ciMenu->BottomText());
     dsyslog("CAM: '%s'", ciMenu->BottomText());
     }
  Display();
}

cMenuCam::~cMenuCam()
{
  if (!selected)
     ciMenu->Abort();
  delete ciMenu;
}

void cMenuCam::AddMultiLineItem(const char *s)
{
  while (s && *s) {
        const char *p = strchr(s, '\n');
        int l = p ? p - s : strlen(s);
        cOsdItem *item = new cOsdItem;
        item->SetSelectable(false);
        item->SetText(strndup(s, l), false);
        Add(item);
        s = p ? p + 1 : p;
        }
}

eOSState cMenuCam::Select(void)
{
  if (ciMenu->Selectable()) {
     ciMenu->Select(Current() - offset);
     dsyslog("CAM: select %d", Current() - offset);
     }
  else
     ciMenu->Cancel();
  selected = true;
  return osEnd;
}

eOSState cMenuCam::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:     return Select();
       default: break;
       }
     }
  else if (state == osBack) {
     ciMenu->Cancel();
     selected = true;
     return osEnd;
     }
  if (ciMenu->HasUpdate()) {
     selected = true;
     return osEnd;
     }
  return state;
}

// --- cMenuCamEnquiry -------------------------------------------------------

cMenuCamEnquiry::cMenuCamEnquiry(cCiEnquiry *CiEnquiry)
:cOsdMenu("", 1)
{
  ciEnquiry = CiEnquiry;
  int Length = ciEnquiry->ExpectedLength();
  input = MALLOC(char, Length + 1);
  *input = 0;
  replied = false;
  SetTitle("CAM");
  Add(new cOsdItem(ciEnquiry->Text(), osUnknown, false));
  Add(new cOsdItem("", osUnknown, false));
  Add(new cMenuEditNumItem("", input, Length, ciEnquiry->Blind()));
  Display();
}

cMenuCamEnquiry::~cMenuCamEnquiry()
{
  if (!replied)
     ciEnquiry->Abort();
  free(input);
  delete ciEnquiry;
}

eOSState cMenuCamEnquiry::Reply(void)
{
  if (ciEnquiry->ExpectedLength() < 0xFF && int(strlen(input)) != ciEnquiry->ExpectedLength()) {
     char buffer[64];
     snprintf(buffer, sizeof(buffer), tr("Please enter %d digits!"), ciEnquiry->ExpectedLength());
     Skins.Message(mtError, buffer);
     return osContinue;
     }
  ciEnquiry->Reply(input);
  replied = true;
  return osEnd;
}

eOSState cMenuCamEnquiry::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk:     return Reply();
       default: break;
       }
     }
  else if (state == osBack) {
     ciEnquiry->Cancel();
     replied = true;
     return osEnd;
     }
  return state;
}

// --- CamControl ------------------------------------------------------------

cOsdObject *CamControl(void)
{
  for (int d = 0; d < cDevice::NumDevices(); d++) {
      cDevice *Device = cDevice::GetDevice(d);
      if (Device) {
         cCiHandler *CiHandler = Device->CiHandler();
         if (CiHandler && CiHandler->HasUserIO()) {
            cCiMenu *CiMenu = CiHandler->GetMenu();
            if (CiMenu)
               return new cMenuCam(CiMenu);
            else {
               cCiEnquiry *CiEnquiry = CiHandler->GetEnquiry();
               if (CiEnquiry)
                  return new cMenuCamEnquiry(CiEnquiry);
               }
            }
         }
      }
  return NULL;
}

// --- cMenuRecording --------------------------------------------------------

class cMenuRecording : public cOsdMenu {
private:
  const cRecording *recording;
  bool withButtons;
public:
  cMenuRecording(const cRecording *Recording, bool WithButtons = false);
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
};

cMenuRecording::cMenuRecording(const cRecording *Recording, bool WithButtons)
:cOsdMenu(tr("Recording info"))
{
  recording = Recording;
  withButtons = WithButtons;
  if (withButtons)
     SetHelp(tr("Button$Play"), tr("Button$Rewind"));
}

void cMenuRecording::Display(void)
{
  cOsdMenu::Display();
  DisplayMenu()->SetRecording(recording);
  if (recording->Info()->Description())
     cStatus::MsgOsdTextItem(recording->Info()->Description());
}

eOSState cMenuRecording::ProcessKey(eKeys Key)
{
  switch (int(Key)) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
                  DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
                  cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft);
                  return osContinue;
    case kInfo:   return osBack;
    default: break;
    }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRed:    if (withButtons)
                        Key = kOk; // will play the recording, even if recording commands are defined
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

// --- cMenuRecordingItem ----------------------------------------------------

class cMenuRecordingItem : public cOsdItem {
private:
  char *fileName;
  char *name;
  int totalEntries, newEntries;
public:
  cMenuRecordingItem(cRecording *Recording, int Level);
  ~cMenuRecordingItem();
  void IncrementCounter(bool New);
  const char *Name(void) const { return name; }
  const char *FileName(void) { return fileName; }
  bool IsDirectory(void) const { return name != NULL; }
  };

cMenuRecordingItem::cMenuRecordingItem(cRecording *Recording, int Level)
{
  fileName = strdup(Recording->FileName());
  name = NULL;
  totalEntries = newEntries = 0;
  SetText(Recording->Title('\t', true, Level, false));
  if (*Text() == '\t')
     name = strdup(Text() + 2); // 'Text() + 2' to skip the two '\t'
}

cMenuRecordingItem::~cMenuRecordingItem()
{
  free(fileName);
  free(name);
}

void cMenuRecordingItem::IncrementCounter(bool New)
{
  totalEntries++;
  if (New)
     newEntries++;
  switch (Setup.ShowRecTime + Setup.ShowRecDate + Setup.ShowRecLength) {
     case 0:
          SetText(cString::sprintf("%s", name));
          break;
     case 1:
          SetText(cString::sprintf("%d\t%s", totalEntries, name));
          break;
     case 2:
     default:
          SetText(cString::sprintf("%d\t%d\t%s", totalEntries, newEntries, name));
          break;
     case 3:
          SetText(cString::sprintf("%d\t%d\t\t%s", totalEntries, newEntries, name));
          break;
     }
}

// --- cMenuRenameRecording --------------------------------------------------

class cMenuRenameRecording : public cOsdMenu {
private:
  // Data structure for service "EpgsearchDirectoryList-v1.0"
  struct EpgSearchDirectoryList_v1_0 {
    char folder[PATH_MAX];   // folder selected
    cOsdMenu* Menu;          // pointer to the menu
    } DirList;
  char name[NAME_MAX + 1];
  int priority;
  int lifetime;
  cMenuEditStrItem *fileItem;
  cMenuEditStrItem *folderItem;
  cOsdItem *marksItem, *resumeItem;
  bool isResume, isMarks;
  cRecording *recording;
  void SetHelpKeys(void);
  eOSState SetFolder(void);
public:
  cMenuRenameRecording(cRecording *Recording);
  virtual eOSState ProcessKey(eKeys Key);
};

cMenuRenameRecording::cMenuRenameRecording(cRecording *Recording)
:cOsdMenu(tr("Rename recording"), 12)
{
  cMarks marks;

  fileItem = NULL;
  folderItem = NULL;
  recording = Recording;
  
  if (recording) {
     strn0cpy(DirList.folder, recording->Folder(), sizeof(DirList.folder));
     strn0cpy(name, recording->BaseName(), sizeof(name));

     Add(fileItem   = new cMenuEditStrItem(tr("File"), name, sizeof(name), tr(FileNameChars)));
     Add(folderItem = new cMenuEditStrItem(tr("Directory"), DirList.folder, sizeof(DirList.folder), tr(FileNameChars)));

     priority = recording->priority;
     lifetime = recording->lifetime;
     Add(new cMenuEditIntItem(tr("Priority"), &priority, 0, MAXPRIORITY));
     Add(new cMenuEditIntItem(tr("Lifetime"), &lifetime, 0, MAXLIFETIME));

     Add(new cOsdItem("", osUnknown, false));

     Add(new cOsdItem(cString::sprintf("%s:\t%s", tr("Date"), *DayDateTime(recording->start)), osUnknown, false));

     cChannel *channel = Channels.GetByChannelID(((cRecordingInfo *)recording->Info())->ChannelID());
     if (channel)
        Add(new cOsdItem(cString::sprintf("%s:\t%s", tr("Channel"), *ChannelString(channel, 0)), osUnknown, false));

     int recLen = cIndexFile::GetLength(recording->FileName(), recording->IsPesRecording());
     if (recLen >= 0)
        Add(new cOsdItem(cString::sprintf("%s:\t%s", tr("Length"), *IndexToHMSF(recLen, false)), osUnknown, false));
     else
        recLen = 0;

     int dirSize = DirSizeMB(recording->FileName());
     double seconds = recLen / FRAMESPERSEC;
     cString bitRate = seconds ? cString::sprintf(" (%.2f MBit/s)", 8.0 * dirSize / seconds) : cString("");
     Add(new cOsdItem(cString::sprintf("%s:\t%s", tr("Format"), recording->IsPesRecording() ? tr("PES") : tr("TS")), osUnknown, false));
     Add(new cOsdItem((dirSize > 1023) ? cString::sprintf("%s:\t%.2f GB%s", tr("Size"), dirSize / 1024.0, *bitRate) : cString::sprintf("%s:\t%d MB%s", tr("Size"), dirSize, *bitRate), osUnknown, false));

     Add(new cOsdItem("", osUnknown, false));

     isMarks = marks.Load(recording->FileName(), recording->IsPesRecording()) && marks.Count();
     marksItem = new cOsdItem(tr("Delete marks information?"), osUser1, isMarks);
     Add(marksItem);

     cResumeFile ResumeFile(recording->FileName(), recording->IsPesRecording());
     isResume = (ResumeFile.Read() != -1);
     resumeItem = new cOsdItem(tr("Delete resume information?"), osUser2, isResume);
     Add(resumeItem);
     }
}

void cMenuRenameRecording::SetHelpKeys(void)
{
  if (Current() == MENU_FOLDER_POS)
     SetHelp(NULL, NULL, NULL, tr("Button$Select"));
  else
     SetHelp(NULL, NULL, NULL, NULL);
}

eOSState cMenuRenameRecording::SetFolder(void)
{
  if (cMenuFolder *mf = dynamic_cast<cMenuFolder *>(SubMenu())) {
     strn0cpy(DirList.folder, mf->GetFolder(), sizeof(DirList.folder));
     SetCurrent(folderItem);
     Display();
     }
  return CloseSubMenu();
}

eOSState cMenuRenameRecording::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kOk: {
            if (!*name) {
               *name = ' '; // name must not be empty!
               name[1] = 0;
               }
            cString NewName = *DirList.folder ? cString::sprintf("%s%c%s", DirList.folder, FOLDERDELIMCHAR, name) : name;
            NewName.CompactChars(FOLDERDELIMCHAR);
            if (recording->Rename(NewName, &priority, &lifetime)) {
               Recordings.ChangeState();
               Recordings.TouchUpdate();
               return osUser1;
               }
            else
               Skins.Message(mtError, tr("Error while accessing recording!"));
            }
            break;
       case kBlue:
            if (Current() != MENU_FOLDER_POS)
               return osContinue;
            if (cPluginManager::CallFirstService("EpgsearchDirectoryList-v1.0", &DirList))
               return AddSubMenu(DirList.Menu);
            return AddSubMenu(new cMenuFolder(tr("Select folder"), &Folders, recording->Name()));
       default:
            break;
       }
     return osContinue;
     }
  else if (state == osEnd && HasSubMenu())
     state = SetFolder();
  else if (state == osUser1) {
     if (isMarks && Interface->Confirm(tr("Delete marks information?"))) {
        cMarks marks;
        marks.Load(recording->FileName(), recording->IsPesRecording());
        cMark *mark = marks.First();
        while (mark) {
          cMark *nextmark = marks.Next(mark);
          marks.Del(mark);
          mark = nextmark;
          }
        marks.Save();
        isMarks = false;
        marksItem->SetSelectable(isMarks);
        SetCurrent(First());
        Display();
        }
     return osContinue;
     }
  else if (state == osUser2) {
     if (isResume && Interface->Confirm(tr("Delete resume information?"))) {
        cResumeFile ResumeFile(recording->FileName(), recording->IsPesRecording());
        ResumeFile.Delete();
        isResume = false;
        resumeItem->SetSelectable(isResume);
        SetCurrent(First());
        Display();
        }
     return osContinue;
     }
  if (Key != kNone && !HasSubMenu() && !fileItem->InEditMode() && !folderItem->InEditMode())
     SetHelpKeys();
  return state;
}

// --- cMenuRecordings -------------------------------------------------------

cMenuRecordings::cMenuRecordings(const char *Base, int Level, bool OpenSubMenus)
:cOsdMenu(Base ? Base : tr("Recordings"), 9, 7, 7)
{
  base = Base ? strdup(Base) : NULL;
  level = Setup.RecordingDirs ? Level : -1;
  Recordings.StateChanged(recordingsState); // just to get the current state
  helpKeys = -1;
  Display(); // this keeps the higher level menus from showing up briefly when pressing 'Back' during replay
  Set();
  if (Current() < 0)
     SetCurrent(First());
  else if (OpenSubMenus && cReplayControl::LastReplayed() && Open(true))
     return;
  Display();
  SetHelpKeys();
}

cMenuRecordings::~cMenuRecordings()
{
  helpKeys = -1;
  free(base);
}

void cMenuRecordings::SetHelpKeys(void)
{
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  int NewHelpKeys = 0;
  if (ri) {
     if (ri->IsDirectory())
        NewHelpKeys = 1;
     else {
        NewHelpKeys = 2;
        cRecording *recording = GetRecording(ri);
        if (recording && recording->Info()->Title())
           NewHelpKeys = 3;
        }
     }
  if (NewHelpKeys != helpKeys) {
     switch (NewHelpKeys) {
       case 0: SetHelp(NULL); break;
       case 1: SetHelp(tr("Button$Open")); break;
       case 2:
       case 3: SetHelp(RecordingCommands.Count() ? tr("Commands") : tr("Button$Play"), tr("Button$Rewind"), tr("Button$Delete"), tr("Button$Edit"));
       default: ;
       }
     helpKeys = NewHelpKeys;
     }
}

void cMenuRecordings::Set(bool Refresh)
{
  const char *CurrentRecording = cReplayControl::LastReplayed();
  cMenuRecordingItem *LastItem = NULL;
  char *LastItemText = NULL;
  cThreadLock RecordingsLock(&Recordings);
  if (Refresh) {
     cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
     if (ri) {
        cRecording *Recording = Recordings.GetByName(ri->FileName());
        if (Recording)
           CurrentRecording = Recording->FileName();
        }
     }
  Clear();
  Recordings.Sort();
  for (cRecording *recording = Recordings.First(); recording; recording = Recordings.Next(recording)) {
      if (!base || (strstr(recording->Name(), base) == recording->Name() && recording->Name()[strlen(base)] == FOLDERDELIMCHAR)) {
         cMenuRecordingItem *Item = new cMenuRecordingItem(recording, level);
         if ((*Item->Text() && (!Item->IsDirectory() || (!LastItem || !LastItem->IsDirectory() || strcmp(Item->Text(), LastItemText) != 0)))
#ifdef USE_PINPLUGIN
            && (!PinPatch::ChildLock::IsRecordingHidden(GetRecording(Item), Item->Name(), base, Item->IsDirectory()))
#endif
         ) {
            Add(Item);
            LastItem = Item;
            free(LastItemText);
            LastItemText = strdup(LastItem->Text()); // must use a copy because of the counters!
            }
         else
            delete Item;
         if (LastItem) {
            if (CurrentRecording && strcmp(CurrentRecording, recording->FileName()) == 0)
               SetCurrent(LastItem);
            if (LastItem->IsDirectory())
               LastItem->IncrementCounter(recording->IsNew());
            }
         }
      }
  free(LastItemText);

//kw
  int FreeMB;
  int Percent = VideoDiskSpace(&FreeMB);
  int Minutes = int(double(FreeMB) / MB_PER_MINUTE);
  int Hours = Minutes / 60;
  Minutes %= 60;
  SetTitle(cString::sprintf("%s   %d%%  %2d:%02d %s  (%.1f GB)", tr("Disk"), Percent, Hours, Minutes, tr("free"), FreeMB / 1024.0 ));
//kw

  if (Refresh)
     Display();
}

cRecording *cMenuRecordings::GetRecording(cMenuRecordingItem *Item)
{
  cRecording *recording = Recordings.GetByName(Item->FileName());
  if (!recording)
     Skins.Message(mtError, tr("Error while accessing recording!"));
  return recording;
}

bool cMenuRecordings::Open(bool OpenSubMenus)
{
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && ri->IsDirectory()) {
     const char *t = ri->Name();
     cString buffer;
     if (base) {
        buffer = cString::sprintf("%s%c%s", base, FOLDERDELIMCHAR, t);
        t = buffer;
        }
     AddSubMenu(new cMenuRecordings(t, level + 1, OpenSubMenus));
     return true;
     }
  return false;
}

eOSState cMenuRecordings::Play(void)
{
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri) {
#ifdef USE_PINPLUGIN
     if (PinPatch::ChildLock::IsRecordingProtected(GetRecording(ri), ri->Name(), base, ri->IsDirectory()))
        return osContinue;
#endif
     if (ri->IsDirectory())
        Open();
     else {
        if (cRecording *recording = GetRecording(ri)) {
           cReplayControl::SetRecording(recording->FileName(), recording->Title());
           return osReplay;
           }
        }
     }
  return osContinue;
}

eOSState cMenuRecordings::Rewind(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     if (cRecording *recording = GetRecording(ri)) {
        cDevice::PrimaryDevice()->StopReplay(); // must do this first to be able to rewind the currently replayed recording
        cResumeFile ResumeFile(ri->FileName(), recording->IsPesRecording());
        ResumeFile.Delete();
        return Play();
        }
     }
  return osContinue;
}

eOSState cMenuRecordings::Delete(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     if (Interface->Confirm(tr("Delete recording?"))) {
        if (cRecordControl *rc = cRecordControls::GetRecordControl(ri->FileName())) {
           if (Interface->Confirm(tr("Timer still recording - really delete?"))) {
              if (cTimer *timer = rc->Timer()) {
                 timer->Skip();
                 cRecordControls::Process(time(NULL));
                 if (timer->IsSingleEvent()) {
                    isyslog("deleting timer %s", *timer->ToDescr());
                    Timers.Del(timer);
                    }
                 Timers.SetModified();
                 }
              }
           else
              return osContinue;
           }
        if (cRecording *recording = GetRecording(ri)) {
           if (recording->Delete()) {
              cReplayControl::ClearLastReplayed(ri->FileName());
              Recordings.DelByName(ri->FileName());
              cOsdMenu::Del(Current());
              SetHelpKeys();
              Display();
              if (!Count())
                 return osBack;
              }
           else
              Skins.Message(mtError, tr("Error while deleting recording!"));
           }
        }
     }
  return osContinue;
}

eOSState cMenuRecordings::Info(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     cRecording *recording = GetRecording(ri);
     if (recording && recording->Info()->Title())
        return AddSubMenu(new cMenuRecording(recording, true));
     }
  return osContinue;
}

eOSState cMenuRecordings::Commands(eKeys Key)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     if (cRecording *recording = GetRecording(ri)) {
        cMenuCommands *menu;
        eOSState state = AddSubMenu(menu = new cMenuCommands(tr("Recording commands"), &RecordingCommands, cString::sprintf("\"%s\"", *strescape(recording->FileName(), "\\\"$"))));
        if (Key != kNone)
           state = menu->ProcessKey(Key);
        return state;
        }
     }
  return osContinue;
}

eOSState cMenuRecordings::Rename(void)
{
  if (HasSubMenu() || Count() == 0)
     return osContinue;
  cMenuRecordingItem *ri = (cMenuRecordingItem *)Get(Current());
  if (ri && !ri->IsDirectory()) {
     if (cRecording *recording = GetRecording(ri))
        return AddSubMenu(new cMenuRenameRecording(recording));
     }
  return osContinue;
}

eOSState cMenuRecordings::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     const char *RecordingsSortModeTexts[MAXSORTMODES];
     RecordingsSortModeTexts[0] = tr("main dir alphabetically, subdirs flexible");
     RecordingsSortModeTexts[1] = tr("main dir by date, subdirs flexible");
     RecordingsSortModeTexts[2] = tr("all alphabetically");
     RecordingsSortModeTexts[3] = tr("all by date");

     switch (int(Key)) {
       case kOk:     return Play();
       case kRed:    return (helpKeys > 1 && RecordingCommands.Count()) ? Commands() : Play();
       case kGreen:  return Rewind();
       case kYellow: return Delete();
       case kInfo:   return Info();
       case k0:      Setup.RecordingsSortMode = (Setup.RecordingsSortMode + 1) % MAXSORTMODES;
                     Set(true);
                     Skins.Message(mtStatus, cString::sprintf("%s %d: %s", tr("Sorting"), Setup.RecordingsSortMode, RecordingsSortModeTexts[Setup.RecordingsSortMode]));
                     return osContinue;
       case k1...k7: return Commands(Key);
       case kBlue:
       case k8:      return Rename();
       case k9:      Recordings.ToggleSortOrder();
                     Set(true);
                     return osContinue;
       case kNone:   if (Recordings.StateChanged(recordingsState))
                        Set(true);
                     break;
       default: break;
       }
     }
  else if (state == osUser1) {
     // a recording or path was renamed, so let's refresh the menu
     CloseSubMenu(false); // this is the cMenuRenameRecording or the now empty submenu if the subfolder became empty
     Set(); // in case a recording was moved into a new subfolder of this folder
     if (base && !Count()) // base: don't go up beyond the top level Recordings menu
        return state;
     Open(true); // open any necessary submenus to show the original folder
     Display();
     state = osContinue;
     }
  if (Key == kYellow && HadSubMenu && !HasSubMenu()) {
     // the last recording in a subdirectory was deleted, so let's go back up
     cOsdMenu::Del(Current());
     if (!Count())
        return osBack;
     Display();
     }
  if (!HasSubMenu() && Key != kNone)
     SetHelpKeys();
  return state;
}

// --- cMenuSetupBase --------------------------------------------------------

class cMenuSetupBase : public cMenuSetupPage {
protected:
  cSetup data;
  virtual void Store(void);
public:
  cMenuSetupBase(void);
  };

cMenuSetupBase::cMenuSetupBase(void)
{
  data = Setup;
}

void cMenuSetupBase::Store(void)
{
  Setup = data;
  Setup.Save();
}

// --- cMenuSetupOSD ---------------------------------------------------------

class cMenuSetupOSD : public cMenuSetupBase {
private:
  const char *useSmallFontTexts[3];
  int numSkins;
  int originalSkinIndex;
  int skinIndex;
  const char **skinDescriptions;
  cThemes themes;
  int themeIndex;
  virtual void Set(void);
public:
  cMenuSetupOSD(void);
  virtual ~cMenuSetupOSD();
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuSetupOSD::cMenuSetupOSD(void)
{
  numSkins = Skins.Count();
  skinIndex = originalSkinIndex = Skins.Current()->Index();
  skinDescriptions = new const char*[numSkins];
  themes.Load(Skins.Current()->Name());
  themeIndex = Skins.Current()->Theme() ? themes.GetThemeIndex(Skins.Current()->Theme()->Description()) : 0;
  Set();
}

cMenuSetupOSD::~cMenuSetupOSD()
{
  cFont::SetCode(I18nCharSets()[Setup.OSDLanguage]);
  delete[] skinDescriptions;
}

void cMenuSetupOSD::Set(void)
{
  int current = Current();
  for (cSkin *Skin = Skins.First(); Skin; Skin = Skins.Next(Skin))
      skinDescriptions[Skin->Index()] = Skin->Description();
  useSmallFontTexts[0] = tr("never");
  useSmallFontTexts[1] = tr("skin dependent");
  useSmallFontTexts[2] = tr("always");
  Clear();
  SetSection(tr("OSD"));
  Add(new cMenuEditStraItem(tr("Setup.OSD$Language"),               &data.OSDLanguage, I18nNumLanguages, I18nLanguages()));
  Add(new cMenuEditStraItem(tr("Setup.OSD$Skin"),                   &skinIndex, numSkins, skinDescriptions));
  if (themes.NumThemes())
  Add(new cMenuEditStraItem(tr("Setup.OSD$Theme"),                  &themeIndex, themes.NumThemes(), themes.Descriptions()));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$WarEagle icons"),         &data.WarEagleIcons));
  Add(new cMenuEditIntItem( tr("Setup.OSD$Left"),                   &data.OSDLeft, 0, MAXOSDWIDTH));
  Add(new cMenuEditIntItem( tr("Setup.OSD$Top"),                    &data.OSDTop, 0, MAXOSDHEIGHT));
  Add(new cMenuEditIntItem( tr("Setup.OSD$Width"),                  &data.OSDWidth, MINOSDWIDTH, MAXOSDWIDTH));
  Add(new cMenuEditIntItem( tr("Setup.OSD$Height"),                 &data.OSDHeight, MINOSDHEIGHT, MAXOSDHEIGHT));
  Add(new cMenuEditIntItem( tr("Setup.OSD$Message time (s)"),       &data.OSDMessageTime, 1, 60));
  Add(new cMenuEditStraItem(tr("Setup.OSD$Use small font"),         &data.UseSmallFont, 3, useSmallFontTexts));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$Channel info position"),  &data.ChannelInfoPos, tr("bottom"), tr("top")));
  Add(new cMenuEditIntItem( tr("Setup.OSD$Channel info time (s)"),  &data.ChannelInfoTime, 1, 60));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$Info on channel switch"), &data.ShowInfoOnChSwitch));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$Timeout requested channel info"), &data.TimeoutRequChInfo));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$Scroll pages"),           &data.MenuScrollPage));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$Scroll wraps"),           &data.MenuScrollWrap));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$Menu button closes"),     &data.MenuButtonCloses));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$Recording directories"),  &data.RecordingDirs));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$Folders in timer menu"),  &data.FoldersInTimerMenu));
  Add(new cMenuEditBoolItem(tr("Setup.OSD$Main menu command position"), &data.MenuCmdPosition, tr("bottom"), tr("top")));
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupOSD::ProcessKey(eKeys Key)
{
  if (Key == kOk) {
     if (skinIndex != originalSkinIndex) {
        cSkin *Skin = Skins.Get(skinIndex);
        if (Skin) {
           strn0cpy(data.OSDSkin, Skin->Name(), sizeof(data.OSDSkin));
           Skins.SetCurrent(Skin->Name());
           }
        }
     if (themes.NumThemes() && Skins.Current()->Theme()) {
        Skins.Current()->Theme()->Load(themes.FileName(themeIndex));
        strn0cpy(data.OSDTheme, themes.Name(themeIndex), sizeof(data.OSDTheme));
        }
     data.OSDWidth &= ~0x07; // OSD width must be a multiple of 8
     }

  int osdLanguage = data.OSDLanguage;
  int oldSkinIndex = skinIndex;
  eOSState state = cMenuSetupBase::ProcessKey(Key);

  if (data.OSDLanguage != osdLanguage || skinIndex != oldSkinIndex) {
     int OriginalOSDLanguage = Setup.OSDLanguage;
     Setup.OSDLanguage = data.OSDLanguage;
     cFont::SetCode(I18nCharSets()[Setup.OSDLanguage]);

     cSkin *Skin = Skins.Get(skinIndex);
     if (Skin) {
        char *d = themes.NumThemes() ? strdup(themes.Descriptions()[themeIndex]) : NULL;
        themes.Load(Skin->Name());
        if (skinIndex != oldSkinIndex)
           themeIndex = d ? themes.GetThemeIndex(d) : 0;
        free(d);
        }

     Set();
     Setup.OSDLanguage = OriginalOSDLanguage;
     }
  return state;
}

// --- cMenuSetupEPG ---------------------------------------------------------
//M7X0 BEGIN AK
class cMenuSetupEPG : public cMenuSetupBase {
private:
  int originalNumLanguages;
  int numLanguages;
  const char *epgModeStr[5];
  void Setup(void);
public:
  cMenuSetupEPG(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuSetupEPG::cMenuSetupEPG(void)
{
  for (numLanguages = 0; numLanguages < I18nNumLanguages && data.EPGLanguages[numLanguages] >= 0; numLanguages++)
      ;
  originalNumLanguages = numLanguages;
  SetSection(tr("EPG"));
  SetHelp(tr("Button$Scan"));
  Setup();
}

void cMenuSetupEPG::Setup(void)
{
  int current = Current();

  Clear();

  Add(new cMenuEditIntItem( tr("Setup.EPG$EPG scan timeout (h)"),      &data.EPGScanTimeout));
  Add(new cMenuEditIntItem( tr("Setup.EPG$EPG bugfix level"),          &data.EPGBugfixLevel, 0, MAXEPGBUGFIXLEVEL));
  Add(new cMenuEditIntItem( tr("Setup.EPG$EPG linger time (min)"),     &data.EPGLinger, 0));
  Add(new cMenuEditBoolItem(tr("Setup.EPG$Show progress bar"),         &data.ShowProgressBar));
  Add(new cMenuEditBoolItem(tr("Setup.EPG$Set system time"),           &data.SetSystemTime));
  if (data.SetSystemTime)
     Add(new cMenuEditTranItem(tr("Setup.EPG$Use time from transponder"), &data.TimeTransponder, &data.TimeSource));
  Add(new cMenuEditIntItem( tr("Setup.EPG$Preferred languages"),       &numLanguages, 0, I18nNumLanguages));
  for (int i = 0; i < numLanguages; i++)
     Add(new cMenuEditStraItem(tr("Setup.EPG$Preferred language"),     &data.EPGLanguages[i], I18nNumLanguages, I18nLanguages()));

  epgModeStr[0] = tr("Setup.EPG$Full EPG");
  epgModeStr[1] = tr("Setup.EPG$Foreign EPG");
  epgModeStr[2] = tr("Setup.EPG$Do not save");
  epgModeStr[3] = tr("Setup.EPG$Now and Next only");
  epgModeStr[4] = tr("Setup.EPG$No EPG");

  Add(new cMenuEditStraItem(tr("Setup.EPG$Channel Default Mode"),
                             &data.EPGDefaultMode,
                             5, epgModeStr));
  Add(new cMenuEditStraItem(tr("Setup.EPG$Channel Inverse Mode"),
                             &data.EPGInvDefaultMode,
                             5, epgModeStr));
  Add(new cMenuEditIntItem(tr("Setup.EPG$Channel Limit"),&data.EPGChannelNoLimit));
  SetCurrent(Get(current));
  Display();
}
//M7X0 END AK
eOSState cMenuSetupEPG::ProcessKey(eKeys Key)
{
  if (Key == kOk) {
     bool Modified = numLanguages != originalNumLanguages;
     if (!Modified) {
        for (int i = 0; i < numLanguages; i++) {
            if (data.EPGLanguages[i] != ::Setup.EPGLanguages[i]) {
               Modified = true;
               break;
               }
            }
        }
     if (Modified)
        cSchedules::ResetVersions();
     }

  int oldnumLanguages = numLanguages;
  int oldSetSystemTime = data.SetSystemTime;

  eOSState state = cMenuSetupBase::ProcessKey(Key);
  if (Key != kNone) {
     if (numLanguages != oldnumLanguages || data.SetSystemTime != oldSetSystemTime) {
        for (int i = oldnumLanguages; i < numLanguages; i++) {
            data.EPGLanguages[i] = 0;
            for (int l = 0; l < I18nNumLanguages; l++) {
                int k;
                for (k = 0; k < oldnumLanguages; k++) {
                    if (data.EPGLanguages[k] == l)
                       break;
                    }
                if (k >= oldnumLanguages) {
                   data.EPGLanguages[i] = l;
                   break;
                   }
                }
            }
        data.EPGLanguages[numLanguages] = -1;
        Setup();
        }
     if (Key == kRed) {
        EITScanner.ForceScan();
        return osEnd;
        }
     }
  return state;
}

//M7X0 BEGIN AK
// --- cMenuSetupEPGMode -----------------------------------------------------

class cMenuSetupEPGMode : public cMenuSetupPage {
private:
  class clModeItem : public cListObject {
    public:
      eEpgMode epgVal;
      cEpgMode *epgMode;
  };
  cList<clModeItem> modes;
  void Set(void);
  const char *epgModeStr[7];
protected:
  virtual void Store(void);
public:
  cMenuSetupEPGMode(void) { Set(); }
};

void cMenuSetupEPGMode::Set(void)
{
  SetSection(tr("EPG Channel Modes"));

  epgModeStr[0] = tr("Setup.EPG$Default Mode");
  epgModeStr[1] = tr("Setup.EPG$Inverse Mode");
  epgModeStr[2] = tr("Setup.EPG$Full EPG");
  epgModeStr[3] = tr("Setup.EPG$Foreign EPG");
  epgModeStr[4] = tr("Setup.EPG$Do not save");
  epgModeStr[5] = tr("Setup.EPG$Now and Next only");
  epgModeStr[6] = tr("Setup.EPG$No EPG");

  char buffer[256];
  for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
      if (!channel->GroupSep()) {
         snprintf(buffer,256, "%d %s - %s", channel->Number(),
                  channel->Provider(), channel->Name());
         buffer[255] = 0;
         clModeItem *modeIt = new clModeItem;
         tChannelID chID = channel->GetChannelID();
         modeIt->epgMode = EpgModes.GetModeByChannelID(&chID);
         modeIt->epgVal = modeIt->epgMode->GetMode(false);
         modes.Add(modeIt);
         cMenuEditStraItem *item = new cMenuEditStraItem(buffer,
                                         (int *)&modeIt->epgVal,7, epgModeStr);
         Add(item);
         }
       else {
         snprintf(buffer,256, "---\t%s ----------------------------------------------------------------", channel->Name());
         buffer[255] = 0;
         Add(new cOsdItem(buffer, osUnknown, false));
         }
     }
  Display();
}

void cMenuSetupEPGMode::Store(void)
{
  bool changed = false;
  for (clModeItem *it = modes.First(); it; it = modes.Next(it)) {
      if (it->epgVal != it->epgMode->GetMode(false)) {
         changed = true;
         it->epgMode->SetMode(it->epgVal);
         }
      }
  if (changed)
     EpgModes.Save();
}
//M7X0 END AK

// --- cMenuSetupDVB ---------------------------------------------------------

class cMenuSetupDVB : public cMenuSetupBase {
private:
  int originalNumAudioLanguages;
  int numAudioLanguages;
  void Setup(void);
  const char *videoDisplayFormatTexts[2];
  const char *videoFormatTexts[3];
  const char *updateChannelsTexts[6];
public:
  cMenuSetupDVB(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuSetupDVB::cMenuSetupDVB(void)
{
  for (numAudioLanguages = 0; numAudioLanguages < I18nNumLanguages && data.AudioLanguages[numAudioLanguages] >= 0; numAudioLanguages++)
      ;
  originalNumAudioLanguages = numAudioLanguages;
  videoDisplayFormatTexts[0] = tr("pan&scan");
  videoDisplayFormatTexts[1] = tr("letterbox");
//M7X0 BEGIN GA

  //m7x0 auto aspect
  videoFormatTexts[0] = "4:3";
  videoFormatTexts[1] = "16:9";
  videoFormatTexts[2] = tr("auto");

  updateChannelsTexts[0] = tr("no");
  updateChannelsTexts[1] = tr("names only");
  updateChannelsTexts[2] = tr("PIDs only");
  updateChannelsTexts[3] = tr("names and PIDs");
  updateChannelsTexts[4] = tr("add new channels");
  updateChannelsTexts[5] = tr("add new transponders");

  SetSection(tr("DVB"));
  Setup();
}

void cMenuSetupDVB::Setup(void)
{
  int current = Current();

  Clear();

  Add(new cMenuEditIntItem( tr("Setup.DVB$Primary DVB interface"), &data.PrimaryDVB, 1, cDevice::NumDevices()));
  //m7x0 HotStandby
  Add(new cMenuEditBoolItem(tr("Setup.DVB$Hot Standby"),	   &data.HotStandby, tr("no"), tr("yes")));
  //m7x0 TvMode fbas svideo
  Add(new cMenuEditBoolItem(tr("Setup.DVB$TV mode"),		   &data.TvMode, "fbas", "svideo"));
  //m7x0 VCRMode fbas svideo
  Add(new cMenuEditBoolItem(tr("Setup.DVB$VCR mode"),		   &data.VCRMode, "fbas", "svideo"));
  //m7x0 auto aspect
  Add(new cMenuEditStraItem(tr("Setup.DVB$Video format"),          &data.VideoFormat, 3, videoFormatTexts));
//M7X0 BEGIN GA
  /*
   * we don't want to force 16/9 in letterbox as in orig code
   * try it this way
   * if (data.VideoFormat == 0)
   */

  Add(new cMenuEditStraItem(tr("Setup.DVB$Video display format"), &data.VideoDisplayFormat, 2, videoDisplayFormatTexts));
//M7X0 END GA
  Add(new cMenuEditBoolItem(tr("Setup.DVB$Enable video output without signal"), &data.VideoWithoutSignal, tr("no"), tr("yes")));
  Add(new cMenuEditBoolItem(tr("Setup.DVB$Use Dolby Digital"),     &data.UseDolbyDigital));
  Add(new cMenuEditStraItem(tr("Setup.DVB$Update channels"),       &data.UpdateChannels, 6, updateChannelsTexts));
  Add(new cMenuEditIntItem( tr("Setup.DVB$Audio languages"),       &numAudioLanguages, 0, I18nNumLanguages));
  for (int i = 0; i < numAudioLanguages; i++)
      Add(new cMenuEditStraItem(tr("Setup.DVB$Audio language"),    &data.AudioLanguages[i], I18nNumLanguages, I18nLanguages()));

  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupDVB::ProcessKey(eKeys Key)
{
  int oldVideoDisplayFormat = ::Setup.VideoDisplayFormat;
  int oldVideoFormat = ::Setup.VideoFormat;
  int newVideoFormat = data.VideoFormat;
  bool oldTvMode = ::Setup.TvMode;
  bool oldVCRMode = ::Setup.VCRMode;
  int oldnumAudioLanguages = numAudioLanguages;
  eOSState state = cMenuSetupBase::ProcessKey(Key);

  if (Key != kNone) {
     bool DoSetup = data.VideoFormat != newVideoFormat;
     if (numAudioLanguages != oldnumAudioLanguages) {
        for (int i = oldnumAudioLanguages; i < numAudioLanguages; i++) {
            data.AudioLanguages[i] = 0;
            for (int l = 0; l < I18nNumLanguages; l++) {
                int k;
                for (k = 0; k < oldnumAudioLanguages; k++) {
                    if (data.AudioLanguages[k] == l)
                       break;
                    }
                if (k >= oldnumAudioLanguages) {
                   data.AudioLanguages[i] = l;
                   break;
                   }
                }
            }
        data.AudioLanguages[numAudioLanguages] = -1;
        DoSetup = true;
        }
     if (DoSetup)
        Setup();
     }
  if (state == osBack && Key == kOk) {
     if (::Setup.VideoDisplayFormat != oldVideoDisplayFormat)
        cDevice::PrimaryDevice()->SetVideoDisplayFormat(eVideoDisplayFormat(::Setup.VideoDisplayFormat));
     if (::Setup.VideoFormat != oldVideoFormat)
        cDevice::PrimaryDevice()->SetVideoFormat(eVideoFormat(::Setup.VideoFormat));
     if (::Setup.TvMode != oldTvMode)
        cDevice::PrimaryDevice()->SetTvMode(::Setup.TvMode);
     if (::Setup.VCRMode != oldVCRMode)
        cDevice::PrimaryDevice()->SetVCRMode(::Setup.VCRMode);
     }
  return state;
}
#ifdef M750S
// --- cMenuSetupLNB ---------------------------------------------------------

class cMenuSetupLNB : public cMenuSetupBase {
private:
  void Setup(void);
public:
  cMenuSetupLNB(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuSetupLNB::cMenuSetupLNB(void)
{
  SetSection(tr("LNB"));
  Setup();
}

void cMenuSetupLNB::Setup(void)
{
  int current = Current();

  Clear();

  Add(new cMenuEditBoolItem(tr("Setup.LNB$Use DiSEqC"),               &data.DiSEqC));
  if (!data.DiSEqC) {
     Add(new cMenuEditIntItem( tr("Setup.LNB$SLOF (MHz)"),               &data.LnbSLOF));
     Add(new cMenuEditIntItem( tr("Setup.LNB$Low LNB frequency (MHz)"),  &data.LnbFrequLo));
     Add(new cMenuEditIntItem( tr("Setup.LNB$High LNB frequency (MHz)"), &data.LnbFrequHi));
     }

  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupLNB::ProcessKey(eKeys Key)
{
  int oldDiSEqC = data.DiSEqC;
  eOSState state = cMenuSetupBase::ProcessKey(Key);

  if (Key != kNone && data.DiSEqC != oldDiSEqC)
     Setup();
  return state;
}

// --- cMenuSetupCICAM -------------------------------------------------------

class cMenuSetupCICAMItem : public cOsdItem {
private:
  cCiHandler *ciHandler;
  int slot;
public:
  cMenuSetupCICAMItem(int Device, cCiHandler *CiHandler, int Slot);
  cCiHandler *CiHandler(void) { return ciHandler; }
  int Slot(void) { return slot; }
  };

cMenuSetupCICAMItem::cMenuSetupCICAMItem(int Device, cCiHandler *CiHandler, int Slot)
{
  ciHandler = CiHandler;
  slot = Slot;
  char buffer[32];
  const char *CamName = CiHandler->GetCamName(slot);
  snprintf(buffer, sizeof(buffer), "%s%d %d\t%s", tr("Setup.CICAM$CICAM DVB"), Device + 1, slot + 1, CamName ? CamName : "-");
  SetText(buffer);
}

class cMenuSetupCICAM : public cMenuSetupBase {
private:
  eOSState Menu(void);
  eOSState Reset(void);
public:
  cMenuSetupCICAM(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuSetupCICAM::cMenuSetupCICAM(void)
{
  SetSection(tr("CICAM"));
  for (int d = 0; d < cDevice::NumDevices(); d++) {
      cDevice *Device = cDevice::GetDevice(d);
      if (Device) {
         cCiHandler *CiHandler = Device->CiHandler();
         if (CiHandler) {
            for (int Slot = 0; Slot < CiHandler->NumSlots(); Slot++)
                Add(new cMenuSetupCICAMItem(Device->CardIndex(), CiHandler, Slot));
            }
         }
      }
  SetHelp(tr("Button$Menu"), tr("Button$Reset"));
}

eOSState cMenuSetupCICAM::Menu(void)
{
  cMenuSetupCICAMItem *item = (cMenuSetupCICAMItem *)Get(Current());
  if (item) {
     if (item->CiHandler()->EnterMenu(item->Slot())) {
        Skins.Message(mtWarning, tr("Opening CAM menu..."));
        time_t t = time(NULL);
        while (time(NULL) - t < MAXWAITFORCAMMENU && !item->CiHandler()->HasUserIO())
              item->CiHandler()->Process();
        return osEnd; // the CAM menu will be executed explicitly from the main loop
        }
     else
        Skins.Message(mtError, tr("Can't open CAM menu!"));
     }
  return osContinue;
}

eOSState cMenuSetupCICAM::Reset(void)
{
  cMenuSetupCICAMItem *item = (cMenuSetupCICAMItem *)Get(Current());
  if (item) {
     Skins.Message(mtWarning, tr("Resetting CAM..."));
     if (item->CiHandler()->Reset(item->Slot())) {
        Skins.Message(mtInfo, tr("CAM has been reset"));
        return osEnd;
        }
     else
        Skins.Message(mtError, tr("Can't reset CAM!"));
     }
  return osContinue;
}

eOSState cMenuSetupCICAM::ProcessKey(eKeys Key)
{
  eOSState state = cMenuSetupBase::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) {
       case kRed:    return Menu();
       case kGreen:  return Reset();
       default: break;
       }
     }
  return state;
}
#endif // M750S

// --- cMenuSetupRecord ------------------------------------------------------

class cMenuSetupRecord : public cMenuSetupBase {
private:
  const char *pauseKeyHandlingTexts[3];
  const char *delTimeshiftRecTexts[3];
  const char *RecordingsSortModeTexts[MAXSORTMODES];
  void Set(void);
  int tmpNVidPrefer,
      tmpUseVidPrefer;
public:
  cMenuSetupRecord(void);
  eOSState ProcessKey(eKeys key);
  };

cMenuSetupRecord::cMenuSetupRecord(void)
{
  Set();
}

eOSState cMenuSetupRecord::ProcessKey(eKeys key)
{
  eOSState s = cMenuSetupBase::ProcessKey(key);;

  if (key != kNone) {
     if (tmpNVidPrefer != data.nVidPrefer || tmpUseVidPrefer != data.UseVidPrefer) {
        int cur = Current();
        
        tmpNVidPrefer = data.nVidPrefer;
        tmpUseVidPrefer = data.UseVidPrefer;
        
        Clear();
        Set();
        SetCurrent(Get(cur));
        Display();
        cMenuSetupBase::ProcessKey(kNone);
        return osContinue;
        }
     }
  return s;
}

void cMenuSetupRecord::Set(void)
{
  pauseKeyHandlingTexts[0] = tr("do not pause live video");
  pauseKeyHandlingTexts[1] = tr("confirm pause live video");
  pauseKeyHandlingTexts[2] = tr("pause live video");
  delTimeshiftRecTexts[0] = tr("no");
  delTimeshiftRecTexts[1] = tr("confirm");
  delTimeshiftRecTexts[2] = tr("yes");
  RecordingsSortModeTexts[0] = tr("main dir alphabetically, subdirs flexible");
  RecordingsSortModeTexts[1] = tr("main dir by date, subdirs flexible");
  RecordingsSortModeTexts[2] = tr("all alphabetically");
  RecordingsSortModeTexts[3] = tr("all by date");

  SetSection(tr("Recording"));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Margin at start (min)"),     &data.MarginStart));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Margin at stop (min)"),      &data.MarginStop));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Primary limit"),             &data.PrimaryLimit, 0, MAXPRIORITY));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Default priority"),          &data.DefaultPriority, 0, MAXPRIORITY));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Default lifetime (d)"),      &data.DefaultLifetime, 0, MAXLIFETIME));
  Add(new cMenuEditStraItem(tr("Setup.Recording$Pause key handling"),        &data.PauseKeyHandling, 3, pauseKeyHandlingTexts));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Pause priority"),            &data.PausePriority, 0, MAXPRIORITY));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Pause lifetime (d)"),        &data.PauseLifetime, 0, MAXLIFETIME));
  tmpNVidPrefer = data.nVidPrefer;
  tmpUseVidPrefer = data.UseVidPrefer;

  Add(new cMenuEditBoolItem(tr("Setup.Recording$Video directory policy"),    &data.UseVidPrefer));
  if (data.UseVidPrefer != 0) {
     char tmp[ 64 ];
     Add(new cMenuEditIntItem(tr("Setup.Recording$ Number of video directories"), &data.nVidPrefer, 1, DVLVIDPREFER_MAX));
     for (int zz = 0; zz < data.nVidPrefer; zz++) {
         sprintf(tmp, tr("Setup.Recording$  Video %d - Priority"), zz);
         Add(new cMenuEditIntItem(tmp, &data.VidPreferPrio[ zz ], 0, 99));
         sprintf(tmp, tr("Setup.Recording$  Video %d - Min. free MB"), zz);
         Add(new cMenuEditIntItem(tmp, &data.VidPreferSize[ zz ], -1, 99999));
         }
     }
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Use episode name"),          &data.UseSubtitle));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Capitalize filenames"),      &data.CapitalizeFilenames));
  Add(new cMenuEditStraItem(tr("Setup.Recording$Delete timeshift recording"),&data.DelTimeshiftRec, 3, delTimeshiftRecTexts));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Use VPS"),                   &data.UseVps));
  Add(new cMenuEditIntItem( tr("Setup.Recording$VPS margin (s)"),            &data.VpsMargin, 0));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Record Dolby Digital"),      &data.UseDolbyInRecordings));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Record HD Video"),           &data.UseHDInRecordings));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Record HD in TS"),           &data.UseTSInHD));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Record SD in TS"),           &data.UseTSInSD));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Record Audio in TS"),        &data.UseTSInAudio));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Include Teletext in TS"),    &data.UseTeletextInTSRecordings));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Include Subtitles in TS"),   &data.UseSubtitlesInTSRecordings));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Mark instant recording"),    &data.MarkInstantRecord));
  Add(new cMenuEditStrItem( tr("Setup.Recording$Name instant recording"),     data.NameInstantRecord, sizeof(data.NameInstantRecord), tr(FileNameChars)));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Instant rec. time (min)"),   &data.InstantRecordTime, 1, MAXINSTANTRECTIME));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Max. video file size (MB)"), &data.MaxVideoFileSize, MINVIDEOFILESIZE, MAXVIDEOFILESIZETS));
  Add(new cMenuEditIntItem( tr("Setup.Recording$Max. recording size (GB)"),  &data.MaxRecordingSize, MINRECORDINGSIZE, MAXRECORDINGSIZE));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Show date"),                 &data.ShowRecDate));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Show time"),                 &data.ShowRecTime));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Show length"),               &data.ShowRecLength));
  Add(new cMenuEditStraItem(tr("Setup.Recording$Sort recordings by"),        &data.RecordingsSortMode, MAXSORTMODES, RecordingsSortModeTexts));
  Add(new cMenuEditBoolItem(tr("Setup.Recording$Sort directories before recordings"), &data.RecordingsSortDirsFirst));
}

// --- cMenuSetupCutter ------------------------------------------------------

class cMenuSetupCutter : public cMenuSetupBase {
public:
  cMenuSetupCutter(void);
  };

cMenuSetupCutter::cMenuSetupCutter(void)
{
  SetSection(tr("Cutter"));
  Add(new cMenuEditBoolItem(tr("Setup.Cutter$Split edited files"),           &data.SplitEditedFiles));
  Add(new cMenuEditBoolItem(tr("Setup.Cutter$Auto delete original record"),  &data.CutterAutoDelete));
  Add(new cMenuEditIntItem( tr("Setup.Cutter$Maximum MB per second"),        &data.CutterMaxBandwidth, 1, 60));
  Add(new cMenuEditIntItem( tr("Setup.Cutter$Maximum bandwidth used in %"),  &data.CutterRelBandwidth, 1, 100));
  Add(new cMenuEditBoolItem(tr("Setup.Cutter$Cutted recordings with marks"), &data.CutterWithMarks));
  Add(new cMenuEditBoolItem(tr("Setup.Cutter$Hard Link Cutter"),             &data.HardLinkCutter));
}

// --- cMenuSetupReplay ------------------------------------------------------

class cMenuSetupReplay : public cMenuSetupBase {
protected:
  virtual void Store(void);
public:
  cMenuSetupReplay(void);
  };

cMenuSetupReplay::cMenuSetupReplay(void)
{
  SetSection(tr("Replay"));
  Add(new cMenuEditBoolItem(tr("Setup.Replay$Multi speed mode"), &data.MultiSpeedMode));
  Add(new cMenuEditBoolItem(tr("Setup.Replay$Show replay mode"), &data.ShowReplayMode));
  Add(new cMenuEditIntItem(tr("Setup.Replay$Resume ID"), &data.ResumeID, 0, 99));
  Add(new cMenuEditIntItem( tr("Setup.Replay$Jump Seconds"), &data.JumpSeconds));
  Add(new cMenuEditIntItem( tr("Setup.Replay$Jump Seconds (repeated)"), &data.JumpSecondsRepeat));
  Add(new cMenuEditIntItem( tr("Setup.Replay$Jump Frames (repeated)"), &data.JumpFramesRepeat));
#ifdef USE_DIRECT_IO
  Add(new cMenuEditBoolItem(tr("Setup.Replay$Use Direct I/O"), &data.ReplayUseDirectIO));
#endif
}

void cMenuSetupReplay::Store(void)
{
  if (Setup.ResumeID != data.ResumeID)
     Recordings.ResetResume();
  cMenuSetupBase::Store();
}

// --- cMenuSetupMisc --------------------------------------------------------

class cMenuSetupMisc : public cMenuSetupBase {
private:
  const char *channelFilterTexts[3];
  const char *lrChannelGroupsTexts[3];
  const char *lrForwardRewindTexts[3];
public:
  cMenuSetupMisc(void);
  };

cMenuSetupMisc::cMenuSetupMisc(void)
{
  SetSection(tr("Miscellaneous"));
  channelFilterTexts[0] = tr("no");
  channelFilterTexts[1] = tr("Tv only");
  channelFilterTexts[2] = tr("Radio only");
  lrChannelGroupsTexts[0] = tr("no");
  lrChannelGroupsTexts[1] = tr("Setup.Miscellaneous$only in channelinfo");
  lrChannelGroupsTexts[2] = tr("yes");
  lrForwardRewindTexts[0] = tr("no");
  lrForwardRewindTexts[1] = tr("Setup.Miscellaneous$only in progress display");
  lrForwardRewindTexts[2] = tr("yes");
  Add(new cMenuEditIntItem( tr("Setup.Miscellaneous$Min. event timeout (min)"),   &data.MinEventTimeout));
  Add(new cMenuEditIntItem( tr("Setup.Miscellaneous$Min. user inactivity (min)"), &data.MinUserInactivity));
  Add(new cMenuEditIntItem( tr("Setup.Miscellaneous$SVDRP timeout (s)"),          &data.SVDRPTimeout));
  Add(new cMenuEditIntItem( tr("Setup.Miscellaneous$Zap timeout (s)"),            &data.ZapTimeout));
  Add(new cMenuEditIntItem( tr("Setup.Miscellaneous$Channel entry timeout (ms)"), &data.ChannelEntryTimeout, 0));
  Add(new cMenuEditStraItem(tr("Setup.Miscellaneous$Filter channels"),            &data.ChannelFilter, 3, channelFilterTexts));;
  Add(new cMenuEditChanItem(tr("Setup.Miscellaneous$Initial channel"),            &data.InitialChannel, tr("Setup.Miscellaneous$as before")));
  Add(new cMenuEditIntItem( tr("Setup.Miscellaneous$Initial volume"),             &data.InitialVolume, -1, 255, tr("Setup.Miscellaneous$as before")));
  Add(new cMenuEditStraItem(tr("Setup.Miscellaneous$Channelgroups with left/right"),   &data.LRChannelGroups, 3, lrChannelGroupsTexts));
  Add(new cMenuEditStraItem(tr("Setup.Miscellaneous$Search fwd/back with left/right"), &data.LRForwardRewind, 3, lrForwardRewindTexts));
  Add(new cMenuEditBoolItem(tr("Setup.Miscellaneous$Show cancel message at shutdown"), &data.ShutdownMessage, tr("no"), tr("yes")));
  Add(new cMenuEditBoolItem(tr("Setup.Miscellaneous$Abort when Plugin fails to load"), &data.AbortWhenPluginFails));
  Add(new cMenuEditBoolItem(tr("Setup.Miscellaneous$Emergency exit"),             &data.EmergencyExit));
  Add(new cMenuEditIntItem( tr("Setup.Miscellaneous$LIRC repeat delay"),          &data.LircRepeatDelay, 0, 1000));
  Add(new cMenuEditIntItem( tr("Setup.Miscellaneous$LIRC repeat freq"),           &data.LircRepeatFreq, 0, 1000));
  Add(new cMenuEditIntItem( tr("Setup.Miscellaneous$LIRC repeat timeout"),        &data.LircRepeatTimeout, 0, 5000));
}

// --- cMenuSetupTShift --------------------------------------------------

class cMenuSetupTShift : public cMenuSetupBase {
private:
  const char *modes[3];
  const char *delayed[3];
  void Setup();
public:
  eOSState ProcessKey(eKeys Key);
  cMenuSetupTShift(void);
  };

cMenuSetupTShift::cMenuSetupTShift(void)
{
  SetSection(tr("Delayed TV"));
  modes[0] = tr("never");
  modes[1] = tr("always");
  modes[2] = tr("using play");
  delayed[0] = tr("never");
  delayed[1] = tr("always");
  delayed[2] = tr("delayed");
  Setup();
}

void cMenuSetupTShift::Setup(void)
{
  int current=Current();
  Clear();
  Add(new cMenuEditStraItem(tr("Delayed TV"),&data.TShift,3,modes));
  Add(new cMenuEditIntItem(tr("Setup.TShift$Buffer size (min)"), &data.TShiftBufferSize, 10, 180));
  Add(new cMenuEditIntItem(tr("Setup.TShift$Start records after (s)"), &data.TShiftStartRecord, 0, 3600));
  Add(new cMenuEditIntItem(tr("Setup.TShift$Inactivity timeout (min)"),&data.TShiftTimeout, 0, 180));
  Add(new cMenuEditStraItem(tr("Setup.TShift$Keep recordings"),&data.TShiftDelayed,3,delayed));
  Add(new cMenuEditIntItem(tr("Setup.TShift$Delayed rec priority"),&data.TShiftPriority, 0, MAXPRIORITY));
  Add(new cMenuEditBoolItem(tr("Setup.TShift$Pause on channel switching"),&data.TShiftPause));
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupTShift::ProcessKey(eKeys Key)
{
  int oldTShift = data.TShift;
  int oldTShiftBufferSize = data.TShiftBufferSize;
  int oldTShiftStartRecord = data.TShiftStartRecord;
  int oldTShiftDelayed = data.TShiftDelayed;
  int oldTShiftPriority = data.TShiftPriority;
  int oldTShiftPause = data.TShiftPause;
  int oldTShiftTimeout = data.TShiftTimeout;
  eOSState state = cMenuSetupBase::ProcessKey(Key);

  if (Key != kNone && (data.TShift != oldTShift || data.TShiftBufferSize != oldTShiftBufferSize || data.TShiftStartRecord != oldTShiftStartRecord || data.TShiftDelayed != oldTShiftDelayed || data.TShiftPriority != oldTShiftPriority || data.TShiftPause != oldTShiftPause || data.TShiftTimeout != oldTShiftTimeout)) {
     Setup();
     if (!data.TShift != !oldTShift) {
       if (!data.TShift)
	 cTShiftControl::ShutdownTShift();
       else
	 cTShiftControl::StartTShift();
       }
     }
  return state;
}


// --- cMenuSetupPluginItem --------------------------------------------------

class cMenuSetupPluginItem : public cOsdItem {
private:
  int pluginIndex;
public:
  cMenuSetupPluginItem(const char *Name, int Index);
  int PluginIndex(void) { return pluginIndex; }
  };

cMenuSetupPluginItem::cMenuSetupPluginItem(const char *Name, int Index)
:cOsdItem(Name)
{
  pluginIndex = Index;
}

// --- cMenuSetupPlugins -----------------------------------------------------

class cMenuSetupPlugins : public cMenuSetupBase {
public:
  cMenuSetupPlugins(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuSetupPlugins::cMenuSetupPlugins(void)
{
  SetSection(tr("Plugins"));
  SetHasHotkeys();
  for (int i = 0; ; i++) {
      cPlugin *p = cPluginManager::GetPlugin(i);
      if (p)
         Add(new cMenuSetupPluginItem(hk(cString::sprintf("%s (%s) - %s", p->Name(), p->Version(), p->Description())), i));
      else
         break;
      }
}

eOSState cMenuSetupPlugins::ProcessKey(eKeys Key)
{
  eOSState state = HasSubMenu() ? cMenuSetupBase::ProcessKey(Key) : cOsdMenu::ProcessKey(Key);

  if (Key == kOk) {
     if (state == osUnknown) {
        cMenuSetupPluginItem *item = (cMenuSetupPluginItem *)Get(Current());
        if (item) {
           cPlugin *p = cPluginManager::GetPlugin(item->PluginIndex());
           if (p) {
              cMenuSetupPage *menu = p->SetupMenu();
              if (menu) {
                 menu->SetPlugin(p);
                 return AddSubMenu(menu);
                 }
              Skins.Message(mtInfo, tr("This plugin has no setup parameters!"));
              }
           }
        }
     else if (state == osContinue)
        Store();
     }
  return state;
}

// --- cMenuSetup ------------------------------------------------------------

class cMenuSetup : public cOsdMenu {
private:
  virtual void Set(void);
  eOSState Restart(void);
public:
  cMenuSetup(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuSetup::cMenuSetup(void)
:cOsdMenu("")
{
  Set();
}

void cMenuSetup::Set(void)
{
  Clear();
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "%s - VDR %s", tr("Setup"), VDRVERSION);
  SetTitle(buffer);
  SetHasHotkeys();
  Add(new cOsdItem(hk(tr("OSD")),           osUser1));
  Add(new cOsdItem(hk(tr("EPG")),           osUser2));
//M7X0 BEGIN AK
  Add(new cOsdItem(hk(tr("EPG Channel Modes")), osUser3));
  Add(new cOsdItem(hk(tr("DVB")),           osUser4));
#ifdef M750S
  Add(new cOsdItem(hk(tr("LNB")),           osUser5));
  Add(new cOsdItem(hk(tr("CICAM")),         osUser6));
#endif
  Add(new cOsdItem(hk(tr("Recording")),     osUser7));
  Add(new cOsdItem(hk(tr("Cutter")),        osUser8));
  Add(new cOsdItem(hk(tr("Replay")),        osUser9));
  Add(new cOsdItem(hk(tr("Miscellaneous")), osUser10));
  Add(new cOsdItem(hk(tr("Delayed TV")),osTShift));
  if (cPluginManager::HasPlugins())
  Add(new cOsdItem(hk(tr("Plugins")),       osUser11));
  Add(new cOsdItem(hk(tr("Restart")),       (eOSState) (osUser11 + 1)));
//M7X0 END AK
}

eOSState cMenuSetup::Restart(void)
{
  if (Interface->Confirm(tr("Really restart?")) && ShutdownHandler.ConfirmRestart(true)) {
     ShutdownHandler.Exit(1);
     return osEnd;
     }
  return osContinue;
}

eOSState cMenuSetup::ProcessKey(eKeys Key)
{
  int osdLanguage = Setup.OSDLanguage;
  eOSState state = cOsdMenu::ProcessKey(Key);

  switch (state) {
//M7X0 BEGIN AK
    case osUser1: return AddSubMenu(new cMenuSetupOSD);
    case osUser2: return AddSubMenu(new cMenuSetupEPG);
    case osUser3: return AddSubMenu(new cMenuSetupEPGMode);
    case osUser4: return AddSubMenu(new cMenuSetupDVB);
#ifdef M750S
    case osUser5: return AddSubMenu(new cMenuSetupLNB);
    case osUser6: return AddSubMenu(new cMenuSetupCICAM);
#endif
    case osUser7: return AddSubMenu(new cMenuSetupRecord);
    case osUser8: return AddSubMenu(new cMenuSetupCutter);
    case osUser9: return AddSubMenu(new cMenuSetupReplay);
    case osUser10: return AddSubMenu(new cMenuSetupMisc);
    case osUser11: return AddSubMenu(new cMenuSetupPlugins);
    case osUserRestart: return Restart();
    case osTShift: return AddSubMenu(new cMenuSetupTShift);
//M7X0 BEGIN AK
    default: ;
    }
  if (Setup.OSDLanguage != osdLanguage) {
     Set();
     if (!HasSubMenu())
        Display();
     }
  return state;
}

// --- cMenuPluginItem -------------------------------------------------------

class cMenuPluginItem : public cOsdItem {
private:
  int pluginIndex;
public:
  cMenuPluginItem(const char *Name, int Index);
  int PluginIndex(void) { return pluginIndex; }
  };

cMenuPluginItem::cMenuPluginItem(const char *Name, int Index)
:cOsdItem(Name, osPlugin)
{
  pluginIndex = Index;
}

// --- cMenuMain -------------------------------------------------------------

#define STOP_RECORDING tr(" Stop recording ")

cOsdObject *cMenuMain::pluginOsdObject = NULL;

cMenuMain::cMenuMain(eOSState State)
:cOsdMenu("")
{
  lastDiskSpaceCheck = 0;
  lastFreeMB = 0;
  replaying = false;
  stopReplayItem = NULL;
  cancelEditingItem = NULL;
  stopRecordingItem = NULL;
  recordControlsState = 0;
  Set();

  // Initial submenus:

  cOsdObject *menu = NULL;
  switch (State) {
    case osSchedule:
        if (!cPluginManager::CallFirstService("MainMenuHooksPatch-v1.0::osSchedule", &menu))
            menu = new cMenuSchedule;
        break;
    case osChannels:
        if (!cPluginManager::CallFirstService("MainMenuHooksPatch-v1.0::osChannels", &menu))
            menu = new cMenuChannels;
        break;
    case osTimers:
        if (!cPluginManager::CallFirstService("MainMenuHooksPatch-v1.0::osTimers", &menu))
            menu = new cMenuTimers;
        break;
    case osRecordings:
        if (!cPluginManager::CallFirstService("MainMenuHooksPatch-v1.0::osRecordings", &menu))
            menu = new cMenuRecordings(NULL, 0, true);
        break;
    case osSetup:      AddSubMenu(new cMenuSetup); break;
    case osCommands:   AddSubMenu(new cMenuCommands(tr("System Utilities"), &Commands)); break;
    default: break;
    }
  if (menu)
     if (menu->IsMenu())
        AddSubMenu((cOsdMenu *) menu);
}

cOsdObject *cMenuMain::PluginOsdObject(void)
{
  cOsdObject *o = pluginOsdObject;
  pluginOsdObject = NULL;
  return o;
}

void cMenuMain::Set(void)
{
  Clear();
  SetTitle("VDR");
  SetHasHotkeys();

  // PIN PATCH + SubMenu Patch
#ifdef SUBMENU_DEADCODE
  bool SubMenuActive=1;
  if (SubMenuActive)
#endif
  submenu.offset = Count();

#ifdef SUBMENU_DEADCODE
  if (SubMenuActive) // if ( SubmenuActive && !Plugin)
#endif
  {
  cPlugin *p;
  /*
  0::Schedule
  0::Channels
  0:1:Games
  1::games
  1::freecell
  1::solitaire
  */

  // Parse info of current Menu
  // add item of menu
  	for( int i = 0; i < submenu.nbSubMenuItem; i++)
  	{
  		if( submenu.subMenuItem[i].menuLevel == submenu.currentMenuLevel )
  		{
  			if( submenu.subMenuItem[i].kindOfItem == 0 )
  			{
  				// Basic menu items:
  				if( strcmp(submenu.subMenuItem[i].name,"Schedule") == 0 )
#ifdef USE_PINPLUGIN
  					if (!PinPatch::ChildLock::IsMenuHidden("Schedule"))
#endif
  					Add(new cOsdItem(hk(tr("Schedule")),   osSchedule));

  				if( strcmp(submenu.subMenuItem[i].name,"Channels") == 0 )
#ifdef USE_PINPLUGIN
  					if (!PinPatch::ChildLock::IsMenuHidden("Channels"))
#endif
  					Add(new cOsdItem(hk(tr("Channels setup")),   osChannels));

  				if( strcmp(submenu.subMenuItem[i].name,"Timers") == 0 )
#ifdef USE_PINPLUGIN
  					if (!PinPatch::ChildLock::IsMenuHidden("Timers"))
#endif
  					Add(new cOsdItem(hk(tr("Timers")),     osTimers));

  				if( strcmp(submenu.subMenuItem[i].name,"Recordings") == 0 )
#ifdef USE_PINPLUGIN
  					if (!PinPatch::ChildLock::IsMenuHidden("Recordings"))
#endif
  					Add(new cOsdItem(hk(tr("Recordings")), osRecordings));

  				if( strcmp(submenu.subMenuItem[i].name,"Setup") == 0 )
#ifdef USE_PINPLUGIN
  					if (!PinPatch::ChildLock::IsMenuHidden("Setup"))
#endif
  					Add(new cOsdItem(hk(tr("Setup")),      osSetup));

  				if( strcmp(submenu.subMenuItem[i].name,"Commands") == 0 )
  					if (Commands.Count())
#ifdef USE_PINPLUGIN
  						if (!PinPatch::ChildLock::IsMenuHidden("Commands"))
#endif
  						Add(new cOsdItem(hk(tr("System Utilities")),  osCommands));

  				// Plugins Item:
  				int j=0;
  				do
  				{
  					p = cPluginManager::GetPlugin(j);
  					if( p != NULL )
  					{
#ifdef USE_PINPLUGIN
  					    if (!PinPatch::ChildLock::IsPluginHidden(p))
#endif
  					    {
  						const char *item = p->MainMenuEntry();
  						if (item)
  						{
  							if( strcmp( submenu.subMenuItem[i].name , p->Name() ) == 0 )
  							Add(new cMenuPluginItem(hk(item), j));
  						}
  					    }
  				}
  				j++;
  		}
  		while( p != NULL);
  	}
          else
  	{
  		Add(new cOsdItem(hk(submenu.subMenuItem[i].name), osUser9));
  	}
      }
  }
        /* Add all plugin entry not listed in config file  */
  if ( submenu.currentMenuLevel == 0 )
  {
  	int j=0;
  	do
  	{
  		p = cPluginManager::GetPlugin(j);
  		if( p != NULL )
  		{
  			const char *item = p->MainMenuEntry();
  			if (item)
  			{
  				int trv = 0;
  				for( int i = 0; i < submenu.nbSubMenuItem; i++)
  				{
  					if( strcmp( submenu.subMenuItem[i].name , p->Name() ) == 0 )
  						trv = 1;
  				}
  				if(trv == 0)
  					Add(new cMenuPluginItem(hk(item), j));
  			}
  		}
  		j++;
  	}
  	while( p != NULL);
  	}

  }
#ifdef SUBMENU_DEADCODE
  else
  {
  // Basic menu items:
#ifdef USE_PINPLUGIN
  if (!PinPatch::ChildLock::IsMenuHidden("Schedule"))
#endif
  Add(new cOsdItem(hk(tr("Schedule")),   osSchedule));
#ifdef USE_PINPLUGIN
  if (!PinPatch::ChildLock::IsMenuHidden("Channels"))
#endif
  Add(new cOsdItem(hk(tr("Channels setup")),   osChannels));
#ifdef USE_PINPLUGIN
  if (!PinPatch::ChildLock::IsMenuHidden("Timers"))
#endif
  Add(new cOsdItem(hk(tr("Timers")),     osTimers));
#ifdef USE_PINPLUGIN
  if (!PinPatch::ChildLock::IsMenuHidden("Recordings"))
#endif
  Add(new cOsdItem(hk(tr("Recordings")), osRecordings));

  // Plugins:

  for (int i = 0; ; i++) {
      cPlugin *p = cPluginManager::GetPlugin(i);
      if (p) {
#ifdef USE_PINPLUGIN
         if (!PinPatch::ChildLock::IsPluginHidden(p))
#endif
         {
         const char *item = p->MainMenuEntry();
         if (item)
            Add(new cMenuPluginItem(hk(item), i));
         }
         }
      else
         break;
      }

  // More basic menu items:
#ifdef USE_PINPLUGIN
  if (!PinPatch::ChildLock::IsMenuHidden("Setup"))
#endif
  Add(new cOsdItem(hk(tr("Setup")),      osSetup));
  if (Commands.Count())
#ifdef USE_PINPLUGIN
     if (!PinPatch::ChildLock::IsMenuHidden("Commands"))
#endif
     Add(new cOsdItem(hk(tr("System Utilities")),  osCommands));
}
#endif // SUBMENU_DEADCODE

  Update(true);

  Display();
}

bool cMenuMain::Update(bool Force)
{
  bool result = false;
  bool SubMenuActive=1;
  cOsdItem* first = NULL;
  if (SubMenuActive)
     first = Get(submenu.offset);

  // Title with disk usage:
  if (Force || time(NULL) - lastDiskSpaceCheck > DISKSPACECHEK) {
     int FreeMB;
     int Percent = VideoDiskSpace(&FreeMB);
     if (Force || FreeMB != lastFreeMB) {
        int Minutes = int(double(FreeMB) / MB_PER_MINUTE);
        int Hours = Minutes / 60;
        Minutes %= 60;
        char buffer[40];
        snprintf(buffer, sizeof(buffer), "%s  -  %s %d%%  -  %2d:%02d %s", tr("VDR"), tr("Disk"), Percent, Hours, Minutes, tr("free"));
        //XXX -> skin function!!!
        SetTitle(buffer);
        result = true;
        }
     lastDiskSpaceCheck = time(NULL);
     }

  bool NewReplaying = cControl::Control() != NULL;
  if (Force || NewReplaying != replaying) {
     replaying = NewReplaying;
     // Replay control:
     if (replaying && !stopReplayItem)
        if (Setup.MenuCmdPosition) Ins(stopReplayItem = new cOsdItem(tr(" Stop replaying"), osStopReplay)); else
        Add(stopReplayItem = new cOsdItem(tr(" Stop replaying"), osStopReplay));
     else if (stopReplayItem && !replaying) {
        Del(stopReplayItem->Index());
        stopReplayItem = NULL;
        }
     // Color buttons:
     SetHelp(!replaying ? tr("Button$Record") : NULL, tr("Button$Audio"), replaying ? NULL : tr("Button$Pause"), replaying ? tr("Button$Stop") : cReplayControl::LastReplayed() ? tr("Button$Resume") : NULL);
     result = true;
     }

  // Editing control:
  bool CutterActive = cCutter::Active();
  if (CutterActive && !cancelEditingItem) {
     if (Setup.MenuCmdPosition) Ins(cancelEditingItem = new cOsdItem(tr(" Cancel editing"), osCancelEdit)); else
     Add(cancelEditingItem = new cOsdItem(tr(" Cancel editing"), osCancelEdit));
     result = true;
     }
  else if (cancelEditingItem && !CutterActive) {
     Del(cancelEditingItem->Index());
     cancelEditingItem = NULL;
     result = true;
     }

  // Record control:
  if (cRecordControls::StateChanged(recordControlsState)) {
     while (stopRecordingItem) {
           cOsdItem *it = Next(stopRecordingItem);
           Del(stopRecordingItem->Index());
           stopRecordingItem = it;
           }
     const char *s = NULL;
     while ((s = cRecordControls::GetInstantId(s)) != NULL) {
           cOsdItem *item = new cOsdItem(osStopRecord);
           item->SetText(cString::sprintf("%s%s", STOP_RECORDING, s));
           if (Setup.MenuCmdPosition) Ins(item); else
           Add(item);
           if (!stopRecordingItem)
              stopRecordingItem = item;
           }
     result = true;
     }

  if (SubMenuActive && first)
     submenu.offset = first->Index();

  return result;
}

eOSState cMenuMain::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();
  int osdLanguage = Setup.OSDLanguage;
  eOSState state = cOsdMenu::ProcessKey(Key);
  HadSubMenu |= HasSubMenu();

#ifdef USE_PINPLUGIN
  cOsdItem* item = Get(Current());
  if (item && item->Text() && state != osContinue && state != osUnknown && state != osBack)
     if (PinPatch::ChildLock::IsMenuProtected(item->Text()))
        return osContinue;
#endif

  cOsdObject *menu = NULL;
  switch (state) {
    case osSchedule:
        if (!cPluginManager::CallFirstService("MainMenuHooksPatch-v1.0::osSchedule", &menu))
            menu = new cMenuSchedule;
        else
            state = osContinue;
        break;
    case osChannels:
        if (!cPluginManager::CallFirstService("MainMenuHooksPatch-v1.0::osChannels", &menu))
            menu = new cMenuChannels;
        else
            state = osContinue;
        break;
    case osTimers:
        if (!cPluginManager::CallFirstService("MainMenuHooksPatch-v1.0::osTimers", &menu))
            menu = new cMenuTimers;
        else
            state = osContinue;
        break;
    case osRecordings:
        if (!cPluginManager::CallFirstService("MainMenuHooksPatch-v1.0::osRecordings", &menu))
            menu = new cMenuRecordings;
        else
            state = osContinue;
        break;
    case osSetup:      menu = new cMenuSetup; break;
    case osCommands:   menu = new cMenuCommands(tr("System Utilities"), &Commands); break;
    case osStopRecord: if (Interface->Confirm(tr("Stop recording?"))) {
                          cOsdItem *item = Get(Current());
                          if (item) {
                             cRecordControls::Stop(item->Text() + strlen(STOP_RECORDING));
                             return osEnd;
                             }
                          }
                       break;
    case osCancelEdit: if (Interface->Confirm(tr("Cancel editing?"))) {
                          cCutter::Stop();
                          return osEnd;
                          }
                       break;
    case osBack:
		       if (submenu.nbLevelMenu > 0)
		       {
			       submenu.nbLevelMenu--;
			       submenu.currentMenuLevel = submenu.oldNumberMenu[ submenu.nbLevelMenu ];
			       Set();
			       return osContinue;
		       }
		       else
			       return osEnd;
		       break;
    case osUser9:
		       submenu.oldNumberMenu[ submenu.nbLevelMenu] = submenu.currentMenuLevel;
		       submenu.nbLevelMenu++;
		       submenu.currentMenuLevel = submenu.GetByLevel(submenu.currentMenuLevel,Current()-submenu.offset);
		       Set();
		       return osContinue;
		       break;
    case osPlugin:     {
                         cMenuPluginItem *item = (cMenuPluginItem *)Get(Current());
                         if (item) {
                            cPlugin *p = cPluginManager::GetPlugin(item->PluginIndex());
                            if (p) {
#ifdef USE_PINPLUGIN
                               if (!PinPatch::ChildLock::IsPluginProtected(p))
#endif
                               {
                               cOsdObject *menu = p->MainMenuAction();
                               if (menu) {
                                  if (menu->IsMenu())
                                     return AddSubMenu((cOsdMenu *)menu);
                                  else {
                                     pluginOsdObject = menu;
                                     return osPlugin;
                                     }
                                  }
                               }
                               }
                            }
                         state = osEnd;
                       }
                       break;
    default: switch (Key) {
               case kRecord:
               case kRed:    if (!HadSubMenu)
                                state = replaying ? osContinue : osRecord;
                             break;
               case kGreen:  if (!HadSubMenu) {
                                cRemote::Put(kAudio, true);
                                state = osEnd;
                                }
                             break;
               case kYellow: if (!HadSubMenu)
                                state = replaying ? osContinue : osPause;
                             break;
               case kBlue:   if (!HadSubMenu)
                                state = replaying ? osStopReplay : cReplayControl::LastReplayed() ? osReplay : osContinue;
                             break;
               default:      break;
               }
    }
  if (menu) {
     if (menu->IsMenu())
        return AddSubMenu((cOsdMenu *) menu);
     pluginOsdObject = menu;
     return osPlugin;
  }
  if (!HasSubMenu() && Update(HadSubMenu))
     Display();
  if (Key != kNone) {
     if (Setup.OSDLanguage != osdLanguage) {
        Set();
        if (!HasSubMenu())
           Display();
        }
     }
  return state;
}

// --- SetTrackDescriptions --------------------------------------------------

static int trackDescriptionsLiveChannel = -1;

static void SetTrackDescriptions(int LiveChannel)
{
  cDevice::PrimaryDevice()->ClrAvailableTracks(true);
  const cComponents *Components = NULL;
  cSchedulesLock SchedulesLock(false, 200);
  trackDescriptionsLiveChannel = -1;
  if (LiveChannel) {
     trackDescriptionsLiveChannel = LiveChannel;
     cChannel *Channel = Channels.GetByNumber(LiveChannel);
     if (Channel) {
        const cSchedules *Schedules = cSchedules::Schedules(SchedulesLock);
        if (Schedules) {
           const cSchedule *Schedule = Schedules->GetSchedule(Channel);
           if (Schedule) {
              const cEvent *Present = Schedule->GetPresentEvent();
              if (Present)
                 Components = Present->Components();
              }
           }
        }
     }
  else if (cReplayControl::NowReplaying()) {
     cThreadLock RecordingsLock(&Recordings);
     cRecording *Recording = Recordings.GetByName(cReplayControl::NowReplaying());
     if (Recording)
        Components = Recording->Info()->Components();
     }
  if (Components) {
     trackDescriptionsLiveChannel = -1;
     int indexAudio = 0;
     int indexDolby = 0;
     for (int i = 0; i < Components->NumComponents(); i++) {
         const tComponent *p = Components->Component(i);
         if (p->stream == 2) {
            if (p->type == 0x05)
               cDevice::PrimaryDevice()->SetAvailableTrack(ttDolby, indexDolby++, 0, LiveChannel ? NULL : p->language, p->description);
            else
               cDevice::PrimaryDevice()->SetAvailableTrack(ttAudio, indexAudio++, 0, LiveChannel ? NULL : p->language, p->description);
            }
         }
     }
}

// --- cDisplayChannel -------------------------------------------------------

cDisplayChannel *cDisplayChannel::currentDisplayChannel = NULL;

cDisplayChannel::cDisplayChannel(int Number, bool Switched)
:cOsdObject(true)
{
  currentDisplayChannel = this;
  group = -1;
  withInfo = !Switched || Setup.ShowInfoOnChSwitch;
  displayChannel = Skins.Current()->DisplayChannel(withInfo);
  number = 0;
  timeout = Switched || Setup.TimeoutRequChInfo;
  channel = Channels.GetByNumber(Number);
  lastPresent = lastFollowing = NULL;
  if (channel) {
     DisplayChannel();
     DisplayInfo();
     displayChannel->Flush();
     }
  lastTime.Set();
}

cDisplayChannel::cDisplayChannel(eKeys FirstKey)
:cOsdObject(true)
{
  currentDisplayChannel = this;
  group = -1;
  number = 0;
  timeout = true;
  lastPresent = lastFollowing = NULL;
  lastTime.Set();
  withInfo = Setup.ShowInfoOnChSwitch;
  displayChannel = Skins.Current()->DisplayChannel(withInfo);
  channel = Channels.GetByNumber(cDevice::CurrentChannel());
  ProcessKey(FirstKey);
}

cDisplayChannel::~cDisplayChannel()
{
  delete displayChannel;
  currentDisplayChannel = NULL;
  cStatus::MsgOsdClear();
}

void cDisplayChannel::DisplayChannel(void)
{
  displayChannel->SetChannel(channel, number);
  cStatus::MsgOsdChannel(ChannelString(channel, number));
  lastPresent = lastFollowing = NULL;
  lastTime.Set();
}

void cDisplayChannel::DisplayInfo(void)
{
  if (withInfo && channel) {
     cSchedulesLock SchedulesLock(false,200);
     if (const cSchedules *Schedules = cSchedules::Schedules(SchedulesLock)) {
        if (const cSchedule *Schedule = Schedules->GetSchedule(channel)) {
           const cEvent *Present = Schedule->GetPresentEvent();
           const cEvent *Following = Schedule->GetFollowingEvent();
           if (Present != lastPresent || Following != lastFollowing) {
              SetTrackDescriptions(channel->Number());
              displayChannel->SetEvents(Present, Following);
              cStatus::MsgOsdProgramme(Present ? Present->StartTime() : 0, Present ? Present->Title() : NULL, Present ? Present->ShortText() : NULL, Following ? Following->StartTime() : 0, Following ? Following->Title() : NULL, Following ? Following->ShortText() : NULL);
              lastPresent = Present;
              lastFollowing = Following;
              lastTime.Set();
              }
           }
        }
     }
}

void cDisplayChannel::Refresh(void)
{
  DisplayChannel();
  displayChannel->SetEvents(NULL, NULL);
}

cChannel *cDisplayChannel::NextAvailableChannel(cChannel *Channel, int Direction)
{
//M7X0 BEGIN AK
  if (Direction) {
     cChannel *Current=Channel;
     while (Channel) {
           Channel = Direction > 0 ? Channels.Next(Channel) : Channels.Prev(Channel);
           if(Current) {
              if((!Channel)||(Channel&&(Channel->GroupSep()))) {
                 Current=Direction>0?Channels.Prev(Current):Channels.Next(Current);
                 if((!Current)||(Current&&(Current->GroupSep())))
                    return NULL;
                 while(Current&&(!Current->GroupSep())) {
                    Channel=Current;
                    Current=Direction>0?Channels.Prev(Current):Channels.Next(Current);
                 }
              Current=NULL;
           }
     }
#ifdef USE_PINPLUGIN
           if (!PinPatch::ChildLock::IsChannelProtected(Channel))
#endif
           if (Channel && Channel->Filtered() && !Channel->GroupSep() && (cDevice::PrimaryDevice()->ProvidesChannel(Channel, Setup.PrimaryLimit, NULL, true) || cDevice::GetDevice(Channel, 0, NULL, true)))
              return Channel;
           }
     }
//M7X0 END AK
  return NULL;
}

eOSState cDisplayChannel::ProcessKey(eKeys Key)
{
  Key=cTShiftControl::FilterKeyFromMenu(Key);
  cChannel *NewChannel = NULL;
  if (Key != kNone)
     lastTime.Set();
  switch (int(Key)) {
    case k0:
         if (number == 0) {
            // keep the "Toggle channels" function working
            cRemote::Put(Key);
            return osEnd;
            }
    case k1 ... k9:
         group = -1;
         if (number >= 0) {
            if (number > Channels.MaxNumber())
               number = Key - k0;
            else
               number = number * 10 + Key - k0;
            channel = Channels.GetByNumber(number);
            Refresh();
            withInfo = false;
            // Lets see if there can be any useful further input:
            int n = channel ? number * 10 : 0;
            int m = 10;
            cChannel *ch = channel;
            while (ch && (ch = Channels.Next(ch)) != NULL) {
                  if (!ch->GroupSep()) {
                     if (n <= ch->Number() && ch->Number() < n + m) {
                        n = 0;
                        break;
                        }
                     if (ch->Number() > n) {
                        n *= 10;
                        m *= 10;
                        }
                     }
                  }
            if (n > 0) {
               // This channel is the only one that fits the input, so let's take it right away:
               NewChannel = channel;
               withInfo = true;
               number = 0;
               Refresh();
               }
            }
         break;
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
         if (!Setup.LRChannelGroups) {
	    cRemote::Put(NORMALKEY(Key) == kLeft ? kVolDn : kVolUp, true);
	    break;
            }
         // else fall through
    case kNext|k_Repeat:
    case kNext:
    case kPrev|k_Repeat:
    case kPrev:
         withInfo = false;
         number = 0;
         if (group < 0) {
            cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
            if (channel)
            {
	            if(channel->Index()>0)
	                    group=Channels.GetPrevGroup(channel->Index());
                    if(group<0)
                            group = channel->Index();
            }
            }
         if (group >= 0) {
            int SaveGroup = group;
            if (NORMALKEY(Key) == kRight || NORMALKEY(Key) == kNext){
               group = Channels.GetNextGroup(group) ;
	       if(group<0)
		       group=Channels.GetNextGroup(-1);
            }
            else{
        	    if(group>0)
                    {
        	            group=Channels.GetPrevGroup(group);
                            if(group<0)
        	                    group=Channels.GetPrevGroup(Channels.Count());
                    }
                    else
          	          group=Channels.GetPrevGroup(Channels.Count());
            }
            if (group < 0)
               group = SaveGroup;
            channel = Channels.Get(group);
            if (channel) {
               Refresh();
               if (!channel->GroupSep())
                  group = -1;
               }
            }
         break;
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
    case kChanUp|k_Repeat:
    case kChanUp:
    case kChanDn|k_Repeat:
    case kChanDn: {
         eKeys k = NORMALKEY(Key);
         cChannel *ch = NextAvailableChannel(channel, (k == kUp || k == kChanUp) ? 1 : -1);
         if (ch)
            channel = ch;
         else if (channel && channel->Number() != cDevice::CurrentChannel())
            Key = k; // immediately switches channel when hitting the beginning/end of the channel list with k_Repeat
         }
         // no break here
    case kUp|k_Release:
    case kDown|k_Release:
    case kChanUp|k_Release:
    case kChanDn|k_Release:
    case kNext|k_Release:
    case kPrev|k_Release:
         if (!(Key & k_Repeat) && channel && channel->Number() != cDevice::CurrentChannel())
            NewChannel = channel;
         withInfo = true;
         group = -1;
         number = 0;
         Refresh();
         break;
    case kNone:
         if (number && Setup.ChannelEntryTimeout && int(lastTime.Elapsed()) > Setup.ChannelEntryTimeout) {
            channel = Channels.GetByNumber(number);
            if (channel)
               NewChannel = channel;
            withInfo = true;
            number = 0;
            Refresh();
            lastTime.Set();
            }
         break;
    //TODO
    //XXX case kGreen:  return osEventNow;
    //XXX case kYellow: return osEventNext;
    case kOk:
         if (group >= 0) {
            channel = Channels.Get(Channels.GetNextNormal(group));
            while(channel&&(!channel->GroupSep())){
	            if(cDevice::PrimaryDevice()->ProvidesChannel(channel,Setup.PrimaryLimit,NULL,true)||cDevice::GetDevice(channel,0,NULL,true))
                    {
	                    NewChannel = channel;
                            break;
                    }
                    channel=Channels.Next(channel);
            }
            withInfo = true;
            group = -1;
            Refresh();
            }
         else if (number > 0) {
            channel = Channels.GetByNumber(number);
            if (channel)
               NewChannel = channel;
            withInfo = true;
            number = 0;
            Refresh();
            }
         else
            return osEnd;
         break;
    default:
         if ((Key & (k_Repeat | k_Release)) == 0) {
            cRemote::Put(Key);
            return osEnd;
            }
    };
  if (!timeout || lastTime.Elapsed() < (uint64_t)(Setup.ChannelInfoTime * 1000)) {
     if (Key == kNone && !number && group < 0 && !NewChannel && channel && channel->Number() != cDevice::CurrentChannel()) {
        // makes sure a channel switch through the SVDRP CHAN command is displayed
        channel = Channels.GetByNumber(cDevice::CurrentChannel());
        Refresh();
        lastTime.Set();
        }
     DisplayInfo();
     displayChannel->Flush();
     if (NewChannel) {
        SetTrackDescriptions(NewChannel->Number()); // to make them immediately visible in the channel display
        Channels.SwitchTo(NewChannel->Number());
        SetTrackDescriptions(NewChannel->Number()); // switching the channel has cleared them
        channel = NewChannel;
        }
     return osContinue;
     }
  return osEnd;
}

// --- cDisplayVolume --------------------------------------------------------

#define VOLUMETIMEOUT 1000 //ms
#define MUTETIMEOUT   5000 //ms

cDisplayVolume *cDisplayVolume::currentDisplayVolume = NULL;

cDisplayVolume::cDisplayVolume(void)
:cOsdObject(true)
{
  currentDisplayVolume = this;
  timeout.Set(cDevice::PrimaryDevice()->IsMute() ? MUTETIMEOUT : VOLUMETIMEOUT);
  displayVolume = Skins.Current()->DisplayVolume();
  Show();
}

cDisplayVolume::~cDisplayVolume()
{
  delete displayVolume;
  currentDisplayVolume = NULL;
}

void cDisplayVolume::Show(void)
{
  displayVolume->SetVolume(cDevice::CurrentVolume(), MAXVOLUME, cDevice::PrimaryDevice()->IsMute());
}

cDisplayVolume *cDisplayVolume::Create(void)
{
  if (!currentDisplayVolume)
     new cDisplayVolume;
  return currentDisplayVolume;
}

void cDisplayVolume::Process(eKeys Key)
{
  if (currentDisplayVolume)
     currentDisplayVolume->ProcessKey(Key);
}

eOSState cDisplayVolume::ProcessKey(eKeys Key)
{
  switch (int(Key)) {
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight:
         cRemote::Put(NORMALKEY(Key) == kLeft ? kVolDn : kVolUp, true);
         break;
         // else fall through
    case kVolUp|k_Repeat:
    case kVolUp:
    case kVolDn|k_Repeat:
    case kVolDn:
         Show();
         timeout.Set(VOLUMETIMEOUT);
         break;
    case kMute:
         if (cDevice::PrimaryDevice()->IsMute()) {
            Show();
            timeout.Set(MUTETIMEOUT);
            }
         else
            timeout.Set();
         break;
    case kNone: break;
    default: if ((Key & k_Release) == 0) {
                cRemote::Put(Key);
                return osEnd;
                }
    }
  return timeout.TimedOut() ? osEnd : osContinue;
}

// --- cDisplayTracks --------------------------------------------------------

#define TRACKTIMEOUT 5000 //ms

cDisplayTracks *cDisplayTracks::currentDisplayTracks = NULL;

cDisplayTracks::cDisplayTracks(void)
:cOsdObject(true)
{
  if (trackDescriptionsLiveChannel != -1)
     SetTrackDescriptions(trackDescriptionsLiveChannel);
  cDevice::PrimaryDevice()->EnsureAudioTrack();
  SetTrackDescriptions(!cDevice::PrimaryDevice()->Replaying() || cDevice::PrimaryDevice()->Transferring() ? cDevice::CurrentChannel() : 0);
  currentDisplayTracks = this;
  numTracks = track = 0;
  audioChannel = cDevice::PrimaryDevice()->GetAudioChannel();
  eTrackType CurrentAudioTrack = cDevice::PrimaryDevice()->GetCurrentAudioTrack();
  for (int i = ttAudioFirst; i <= ttDolbyLast; i++) {
      const tTrackId *TrackId = cDevice::PrimaryDevice()->GetTrack(eTrackType(i));
      if (TrackId && TrackId->id) {
         types[numTracks] = eTrackType(i);
         descriptions[numTracks] = strdup(*TrackId->description ? TrackId->description : *TrackId->language ? TrackId->language : *itoa(i));
         if (i == CurrentAudioTrack)
            track = numTracks;
         numTracks++;
         }
      }
  timeout.Set(TRACKTIMEOUT);
  displayTracks = Skins.Current()->DisplayTracks(tr("Button$Audio"), numTracks, descriptions);
  Show();
}

cDisplayTracks::~cDisplayTracks()
{
  delete displayTracks;
  currentDisplayTracks = NULL;
  for (int i = 0; i < numTracks; i++)
      free(descriptions[i]);
  cStatus::MsgOsdClear();
}

void cDisplayTracks::Show(void)
{
  int ac = IS_AUDIO_TRACK(types[track]) ? audioChannel : -1;
  displayTracks->SetTrack(track, descriptions);
  displayTracks->SetAudioChannel(ac);
  displayTracks->Flush();
  cStatus::MsgSetAudioTrack(track, descriptions);
  cStatus::MsgSetAudioChannel(ac);
}

cDisplayTracks *cDisplayTracks::Create(void)
{
  if (cDevice::PrimaryDevice()->NumAudioTracks() > 0) {
     if (!currentDisplayTracks)
        new cDisplayTracks;
     return currentDisplayTracks;
     }
  Skins.Message(mtWarning, tr("No audio available!"));
  return NULL;
}

void cDisplayTracks::Process(eKeys Key)
{
  if (currentDisplayTracks)
     currentDisplayTracks->ProcessKey(Key);
}

eOSState cDisplayTracks::ProcessKey(eKeys Key)
{
  int oldTrack = track;
  int oldAudioChannel = audioChannel;
  switch (int(Key)) {
    case kUp|k_Repeat:
    case kUp:
    case kDown|k_Repeat:
    case kDown:
         if (NORMALKEY(Key) == kUp && track > 0)
            track--;
         else if (NORMALKEY(Key) == kDown && track < numTracks - 1)
            track++;
         timeout.Set(TRACKTIMEOUT);
         break;
    case kLeft|k_Repeat:
    case kLeft:
    case kRight|k_Repeat:
    case kRight: if (IS_AUDIO_TRACK(types[track])) {
                    static int ac[] = { 1, 0, 2 };
                    audioChannel = ac[cDevice::PrimaryDevice()->GetAudioChannel()];
                    if (NORMALKEY(Key) == kLeft && audioChannel > 0)
                       audioChannel--;
                    else if (NORMALKEY(Key) == kRight && audioChannel < 2)
                       audioChannel++;
                    audioChannel = ac[audioChannel];
                    timeout.Set(TRACKTIMEOUT);
                    }
         break;
    case kAudio|k_Repeat:
    case kAudio:
         if (++track >= numTracks)
            track = 0;
         timeout.Set(TRACKTIMEOUT);
         break;
    case kOk:
         if (types[track] != cDevice::PrimaryDevice()->GetCurrentAudioTrack())
            oldTrack = -1; // make sure we explicitly switch to that track
         timeout.Set();
         break;
    case kNone: break;
    default: if ((Key & k_Release) == 0)
                return osEnd;
    }
  if (track != oldTrack || audioChannel != oldAudioChannel)
     Show();
  if (track != oldTrack) {
     cDevice::PrimaryDevice()->SetCurrentAudioTrack(types[track]);
     Setup.CurrentDolby = IS_DOLBY_TRACK(types[track]);
     }
  if (audioChannel != oldAudioChannel)
     cDevice::PrimaryDevice()->SetAudioChannel(audioChannel);
  return timeout.TimedOut() ? osEnd : osContinue;
}

// --- cRecordControl --------------------------------------------------------

cRecordControl::cRecordControl(cDevice *Device, cTimer *Timer, bool Pause)
{
  // We're going to manipulate an event here, so we need to prevent
  // others from modifying any EPG data:
  cSchedulesLock SchedulesLock;
  /* coverity[side_effect_free] */
  cSchedules::Schedules(SchedulesLock);

  event = NULL;
  fileName = NULL;
  recorder = NULL;
  device = Device;
  if (!device) device = cDevice::PrimaryDevice();//XXX
  timer = Timer;
  if (!timer) {
     timer = new cTimer(true, Pause);
     Timers.Add(timer);
     Timers.SetModified();
     instantId = cString::sprintf(cDevice::NumDevices() > 1 ? "%s - %d" : "%s", timer->Channel()->Name(), device->CardIndex() + 1);
     }
  timer->SetPending(true);
  timer->SetRecording(true);
  event = timer->Event();

  if (event || GetEvent())
     dsyslog("Title: '%s' Subtitle: '%s'", event->Title(), event->ShortText());
  cRecording Recording(timer, event);
  fileName = strdup(Recording.FileName());

  // crude attempt to avoid duplicate recordings:
  if (cRecordControls::GetRecordControl(fileName)) {
     isyslog("already recording: '%s'", fileName);
     if (Timer) {
        timer->SetPending(false);
        timer->SetRecording(false);
        timer->OnOff();
        }
     else {
        Timers.Del(timer);
        Timers.SetModified();
        if (!cReplayControl::LastReplayed()) // an instant recording, maybe from cRecordControls::PauseLiveVideo()
           cReplayControl::SetRecording(fileName, Recording.Name());
        }
     timer = NULL;
     return;
     }

  cRecordingUserCommand::InvokeCommand(RUC_BEFORERECORDING, fileName);
  isyslog("record %s", fileName);
  if (MakeDirs(fileName, true)) {
     const cChannel *ch = timer->Channel();
     if (!Recording.IsPesRecording())
     {
        dsyslog("Starting TS record");
	recorder = new cTSRecorder(fileName, ch, timer->Priority(), false);
        }
     else
     recorder = new cRecorder(fileName, ch->Ca(), timer->Priority(), ch->Vpid(Setup.UseHDInRecordings), ch->Apids(), ch->Dpids(), ch->Spids(), false,  ch->Vtype(), ch->DPpids());
     if (device->AttachReceiver(recorder)) {
        Recording.WriteInfo();
        cStatus::MsgRecording(device, Recording.Name(), Recording.FileName(), true);
        if (!Timer && !cReplayControl::LastReplayed()) // an instant recording, maybe from cRecordControls::PauseLiveVideo()
           cReplayControl::SetRecording(fileName, Recording.Name());
        Recordings.AddByName(fileName);
        return;
        }
     else
        DELETENULL(recorder);
     }
  if (!Timer) {
     Timers.Del(timer);
     Timers.SetModified();
     timer = NULL;
     }
}

cRecordControl::~cRecordControl()
{
  Stop();
  free(fileName);
}

#define INSTANT_REC_EPG_LOOKAHEAD 300 // seconds to look into the EPG data for an instant recording

bool cRecordControl::GetEvent(void)
{
  const cChannel *channel = timer->Channel();
  time_t Time = timer->HasFlags(tfInstant) ? timer->StartTime() + INSTANT_REC_EPG_LOOKAHEAD : timer->StartTime() + (timer->StopTime() - timer->StartTime()) / 2;
  for (int seconds = 0; seconds <= MAXWAIT4EPGINFO; seconds++) {
      {
        cSchedulesLock SchedulesLock;
        if (const cSchedules *Schedules = cSchedules::Schedules(SchedulesLock)) {
           if (const cSchedule *Schedule = Schedules->GetSchedule(channel)) {
              event = Schedule->GetEventAround(Time);
              if (event) {
                 if (seconds > 0)
                    dsyslog("got EPG info after %d seconds", seconds);
                 return true;
                 }
              }
           }
      }
      if (seconds == 0)
         dsyslog("waiting for EPG info...");
      cCondWait::SleepMs(1000);
      }
  dsyslog("no EPG info available");
  return false;
}

void cRecordControl::Stop(bool ExecuteUserCommand)
{
  if (timer) {
     DELETENULL(recorder);
     timer->SetRecording(false);
     timer = NULL;
     cStatus::MsgRecording(device, NULL, fileName, false);
     if (ExecuteUserCommand)
        cRecordingUserCommand::InvokeCommand(RUC_AFTERRECORDING, fileName);
     }
}

bool cRecordControl::Process(time_t t)
{
//M7X0 BEGIN AK
  if (!recorder || !recorder->Activated() || !timer || !timer->Matches(t)) {
     if (timer)
        timer->SetPending(false);
     return false;
     }
  AssertFreeDiskSpace(timer->Priority());
  return true;
//M7X0 END AK
}

// --- cRecordControls -------------------------------------------------------

cRecordControl *cRecordControls::RecordControls[MAXRECORDCONTROLS] = { NULL };
int cRecordControls::state = 0;

bool cRecordControls::Start(cTimer *Timer, bool Pause)
{
  static time_t LastNoDiskSpaceMessage = 0;
  int FreeMB = 0;
  if (Timer) {
     AssertFreeDiskSpace(Timer->Priority(), !Timer->Pending());
     Timer->SetPending(true);
     }
  VideoDiskSpace(&FreeMB);
  if (FreeMB < MINFREEDISK) {
     if (!Timer || time(NULL) - LastNoDiskSpaceMessage > NODISKSPACEDELTA) {
        isyslog("not enough disk space to start recording%s%s", Timer ? " timer " : "", Timer ? *Timer->ToDescr() : "");
        Skins.Message(mtWarning, tr("Not enough disk space to start recording!"));
        LastNoDiskSpaceMessage = time(NULL);
        }
     return false;
     }
  LastNoDiskSpaceMessage = 0;

  ChangeState();
  int ch = Timer ? Timer->Channel()->Number() : cDevice::CurrentChannel();
  cChannel *channel = Channels.GetByNumber(ch);

  if (channel) {
     bool NeedsDetachReceivers = false;
     int Priority = Timer ? Timer->Priority() : Pause ? Setup.PausePriority : Setup.DefaultPriority;
     cDevice *device = cDevice::GetDevice(channel, Priority, &NeedsDetachReceivers);
     if (device) {
        if (NeedsDetachReceivers) {
           Stop(device);
           if (device == cTransferControl::ReceiverDevice())
              cControl::Shutdown(); // in case this device was used for Transfer Mode
           }
        dsyslog("switching device %d to channel %d", device->DeviceNumber() + 1, channel->Number());
        if (!device->SwitchChannel(channel, false)) {
           ShutdownHandler.RequestEmergencyExit();
           return false;
           }
        if (!Timer || Timer->Matches()) {
           for (int i = 0; i < MAXRECORDCONTROLS; i++) {
               if (!RecordControls[i]) {
                  RecordControls[i] = new cRecordControl(device, Timer, Pause);
#ifdef USE_PINPLUGIN
                  PinPatch::ChildLock::NotifyRecordingStart(RecordControls[i]->FileName());
#endif
                  return RecordControls[i]->Process(time(NULL));
                  }
               }
           }
        }
     else if (!Timer || (Timer->Priority() >= Setup.PrimaryLimit && !Timer->Pending())) {
        isyslog("no free DVB device to record channel %d!", ch);
        Skins.Message(mtError, tr("No free DVB device to record!"));
        }
     }
  else
     esyslog("ERROR: channel %d not defined!", ch);
  return false;
}

void cRecordControls::Stop(const char *InstantId)
{
  ChangeState();
  for (int i = 0; i < MAXRECORDCONTROLS; i++) {
      if (RecordControls[i]) {
         const char *id = RecordControls[i]->InstantId();
         if (id && strcmp(id, InstantId) == 0) {
            cTimer *timer = RecordControls[i]->Timer();
            RecordControls[i]->Stop();
            if (timer) {
               isyslog("deleting timer %s", *timer->ToDescr());
               Timers.Del(timer);
               Timers.SetModified();
               }
            break;
            }
         }
      }
}

void cRecordControls::Stop(cDevice *Device)
{
  ChangeState();
  for (int i = 0; i < MAXRECORDCONTROLS; i++) {
      if (RecordControls[i]) {
         if (RecordControls[i]->Device() == Device) {
            isyslog("stopping recording on DVB device %d due to higher priority", Device->CardIndex() + 1);
            RecordControls[i]->Stop();
            }
         }
      }
}

bool cRecordControls::PauseLiveVideo(void)
{
  Skins.Message(mtStatus, tr("Pausing live video..."));
  cReplayControl::SetRecording(NULL, NULL); // make sure the new cRecordControl will set cReplayControl::LastReplayed()
  if (Start(NULL, true)) {
     cCondWait::SleepMs(2000); // allow recorded file to fill up enough to start replaying
     cReplayControl *rc = new cReplayControl;
     cControl::Launch(rc);
     cControl::Attach();
     cCondWait::SleepMs(1000); // allow device to replay some frames, so we have a picture
     Skins.Message(mtStatus, NULL);
     rc->ProcessKey(kPause); // pause, allowing replay mode display
     return true;
     }
  Skins.Message(mtStatus, NULL);
  return false;
}

const char *cRecordControls::GetInstantId(const char *LastInstantId)
{
  for (int i = 0; i < MAXRECORDCONTROLS; i++) {
      if (RecordControls[i]) {
         if (!LastInstantId && RecordControls[i]->InstantId())
            return RecordControls[i]->InstantId();
         if (LastInstantId && LastInstantId == RecordControls[i]->InstantId())
            LastInstantId = NULL;
         }
      }
  return NULL;
}

cRecordControl *cRecordControls::GetRecordControl(const char *FileName)
{
  if (FileName) {
     for (int i = 0; i < MAXRECORDCONTROLS; i++) {
         if (RecordControls[i] && strcmp(RecordControls[i]->FileName(), FileName) == 0)
            return RecordControls[i];
         }
     }
  return NULL;
}

void cRecordControls::Process(time_t t)
{
  for (int i = 0; i < MAXRECORDCONTROLS; i++) {
      if (RecordControls[i]) {
         if (!RecordControls[i]->Process(t)) {
            DELETENULL(RecordControls[i]);
            ChangeState();
            }
         }
      }
}

void cRecordControls::ChannelDataModified(cChannel *Channel)
{
  for (int i = 0; i < MAXRECORDCONTROLS; i++) {
      if (RecordControls[i]) {
         if (RecordControls[i]->Timer() && RecordControls[i]->Timer()->Channel() == Channel) {
            if (RecordControls[i]->Device()->ProvidesTransponder(Channel)) { // avoids retune on devices that don't really access the transponder
               isyslog("stopping recording due to modification of channel %d", Channel->Number());
               RecordControls[i]->Stop();
               // This will restart the recording, maybe even from a different
               // device in case conditional access has changed.
               ChangeState();
               }
            }
         }
      }
}

bool cRecordControls::Active(void)
{
  for (int i = 0; i < MAXRECORDCONTROLS; i++) {
      if (RecordControls[i])
         return true;
      }
  return false;
}

void cRecordControls::Shutdown(void)
{
  for (int i = 0; i < MAXRECORDCONTROLS; i++)
      DELETENULL(RecordControls[i]);
  ChangeState();
}

bool cRecordControls::StateChanged(int &State)
{
  int NewState = state;
  bool Result = State != NewState;
  State = state;
  return Result;
}

// --- cReplayControl --------------------------------------------------------

cReplayControl *cReplayControl::currentReplayControl = NULL;
char *cReplayControl::fileName = NULL;
char *cReplayControl::title = NULL;

cReplayControl::cReplayControl(void)
:cDvbPlayerControl(fileName)
{
  currentReplayControl = this;
  displayReplay = NULL;
  visible = modeOnly = shown = displayFrames = false;
  lastCurrent = lastTotal = -1;
  lastPlay = lastForward = false;
  lastSpeed = -2; // an invalid value
  timeoutShow = 0;
  timeSearchActive = false;
  cRecording Recording(fileName);
  cStatus::MsgReplaying(this, Recording.Name(), Recording.FileName(), true);
  marks.Load(fileName, Recording.IsPesRecording());
  SetTrackDescriptions(false);
}

cReplayControl::~cReplayControl()
{
  Hide();
  cStatus::MsgReplaying(this, NULL, fileName, false);
  Stop();
  if (currentReplayControl == this)
     currentReplayControl = NULL;
}

void cReplayControl::Stop(void)
{
  if (Setup.DelTimeshiftRec && fileName) {
     cRecordControl* rc = cRecordControls::GetRecordControl(fileName);
     if (rc && rc->InstantId()) {
        if (Active()) {
           if (Setup.DelTimeshiftRec == 2 || Interface->Confirm(tr("Delete timeshift recording?"))) {
              cTimer *timer = rc->Timer();
              rc->Stop(false); // don't execute user command
              if (timer) {
                 isyslog("deleting timer %s", *timer->ToDescr());
                 Timers.Del(timer);
                 Timers.SetModified();
                 }
              cDvbPlayerControl::Stop();
              if (cRecording *recording = Recordings.GetByName(fileName)) {
                 if (recording->Delete()) {
                    Recordings.DelByName(fileName);
                    ClearLastReplayed(fileName);
                    }
                 else
                    Skins.Message(mtError, tr("Error while deleting recording!"));
                 }
              return;
              }
           }
        }
     }
  cDvbPlayerControl::Stop();
}

void cReplayControl::SetRecording(const char *FileName, const char *Title)
{
  free(fileName);
  free(title);
  fileName = FileName ? strdup(FileName) : NULL;
  title = Title ? strdup(Title) : NULL;
}

const char *cReplayControl::NowReplaying(void)
{
  return currentReplayControl ? fileName : NULL;
}

const char *cReplayControl::LastReplayed(void)
{
  return fileName;
}

void cReplayControl::ClearLastReplayed(const char *FileName)
{
  if (fileName && FileName && strcmp(fileName, FileName) == 0) {
     free(fileName);
     fileName = NULL;
     }
}

void cReplayControl::ShowTimed(int Seconds)
{
  if (modeOnly)
     Hide();
  if (!visible) {
     shown = ShowProgress(true);
     timeoutShow = (shown && Seconds > 0) ? time(NULL) + Seconds : 0;
     }
}

void cReplayControl::Show(void)
{
  ShowTimed();
}

void cReplayControl::Hide(void)
{
  if (visible) {
     delete displayReplay;
     displayReplay = NULL;
     needsFastResponse = visible = false;
     modeOnly = false;
     lastPlay = lastForward = false;
     lastSpeed = -2; // an invalid value
     timeSearchActive = false;
     }
}

void cReplayControl::ShowMode(void)
{
  if (visible || (Setup.ShowReplayMode && !cOsd::IsOpen())) {
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
           timeoutShow = time(NULL) + MODETIMEOUT;
        displayReplay->SetMode(Play, Forward, Speed);
        lastPlay = Play;
        lastForward = Forward;
        lastSpeed = Speed;
        }
     }
}

bool cReplayControl::ShowProgress(bool Initial)
{
  int Current, Total;
  if (!(Initial || updateTimer.TimedOut()))
     return visible;
  if (GetIndex(Current, Total) && Total > 0) {
     if (!visible) {
        displayReplay = Skins.Current()->DisplayReplay(modeOnly);
        displayReplay->SetMarks(&marks);
        needsFastResponse = visible = true;
        }
     if (Initial) {
        if (title)
           displayReplay->SetTitle(title);
        lastCurrent = lastTotal = -1;
        }
     if (Total != lastTotal) {
        displayReplay->SetTotal(IndexToHMSF(Total));
        if (!Initial)
           displayReplay->Flush();
        }
     if (Current != lastCurrent || Total != lastTotal) {
        displayReplay->SetProgress(Current, Total);
        if (!Initial)
           displayReplay->Flush();
        displayReplay->SetCurrent(IndexToHMSF(Current, displayFrames));
        displayReplay->Flush();
        lastCurrent = Current;
        }
     lastTotal = Total;
     ShowMode();
     updateTimer.Set(PROGRESSTIMEOUT);
     return true;
     }
  return false;
}

void cReplayControl::TimeSearchDisplay(void)
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

void cReplayControl::TimeSearchProcess(eKeys Key)
{
#define STAY_SECONDS_OFF_END 10
  int Seconds = (timeSearchTime >> 24) * 36000 + ((timeSearchTime & 0x00FF0000) >> 16) * 3600 + ((timeSearchTime & 0x0000FF00) >> 8) * 600 + (timeSearchTime & 0x000000FF) * 60;
  int Current = (lastCurrent / FRAMESPERSEC);
  int Total = (lastTotal / FRAMESPERSEC);
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
            Seconds = std::min(Total - Current - STAY_SECONDS_OFF_END, Seconds);
         SkipSeconds(Seconds * dir);
         timeSearchActive = false;
         }
         break;
    case kPlay:
    case kUp:
    case kPause:
    case kDown:
    case kOk:
         Seconds = std::min(Total - STAY_SECONDS_OFF_END, Seconds);
         Goto(Seconds * FRAMESPERSEC, Key == kDown || Key == kPause || Key == kOk);
         timeSearchActive = false;
         break;
    default:
         if (!(Key & k_Flags)) // ignore repeat/release keys
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

void cReplayControl::TimeSearch(void)
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

void cReplayControl::MarkToggle(void)
{
  int Current, Total;
  if (GetIndex(Current, Total, true)) {
     cMark *m = marks.Get(Current);
     lastCurrent = -1; // triggers redisplay
     if (m)
        marks.Del(m);
     else {
        marks.Add(Current);
        ShowTimed(2);
        bool Play, Forward;
        int Speed;
        if (GetReplayMode(Play, Forward, Speed) && !Play)
           Goto(Current, true);
        }
     marks.Save();
     }
}

void cReplayControl::MarkJump(bool Forward)
{
  if (marks.Count()) {
     int Current, Total;
     if (GetIndex(Current, Total)) {
        cMark *m = Forward ? marks.GetNext(Current) : marks.GetPrev(Current);
        if (m) {
           Goto(m->position, true);
           displayFrames = true;
           }
        }
     }
}

void cReplayControl::MarkMove(bool Forward)
{
  int Current, Total;
  if (GetIndex(Current, Total)) {
     cMark *m = marks.Get(Current);
     if (m) {
        displayFrames = true;
        int p = SkipFrames(Forward ? 1 : -1);
        cMark *m2;
        if (Forward) {
           if ((m2 = marks.Next(m)) != NULL && m2->position <= p)
              return;
           }
        else {
           if ((m2 = marks.Prev(m)) != NULL && m2->position >= p)
              return;
           }
        Goto(m->position = p, true);
        marks.Save();
        }
     }
}

void cReplayControl::EditCut(void)
{
  if (fileName) {
     Hide();
     bool CutterActive = cCutter::Active();
     if (!marks.Count())
        Skins.Message(mtError, tr("No editing marks defined!"));
     else if (!cCutter::Start(fileName))
        Skins.Message(mtError, tr("Can't start editing process!"));
     else if (!CutterActive)
        Skins.Message(mtInfo, tr("Editing process started"));
     else
        Skins.Message(mtInfo, tr("Adding to cutting queue"));
     ShowMode();
     }
}

void cReplayControl::EditTest(void)
{
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
}

cOsdObject *cReplayControl::GetInfo(void)
{
  cRecording *Recording = Recordings.GetByName(cReplayControl::LastReplayed());
  if (Recording)
     return new cMenuRecording(Recording, false);
  return NULL;
}

eOSState cReplayControl::ProcessKey(eKeys Key)
{
  static int jmpWidth = 1;
  int pkw = 0;

  if (!Active())
     return osEnd;
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
  bool DisplayedFrames = displayFrames;
  displayFrames = false;
  if (timeSearchActive && Key != kNone) {
     TimeSearchProcess(Key);
     return osContinue;
     }
  bool DoShowMode = true;
  if (!Setup.LRForwardRewind || (Setup.LRForwardRewind == 1 && !visible)) {
    switch (int(Key)) {
      // Left/Right volume control
      case kLeft|k_Repeat:
      case kLeft:
      case kRight|k_Repeat:
      case kRight:
	  cRemote::Put(NORMALKEY(Key) == kLeft ? kVolDn : kVolUp, true);
        return osContinue;
        break;
      default:
        break;
    }
  }
  switch (int(Key)) {
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
                   SkipSeconds(-(Setup.JumpSecondsRepeat)); break;
    case kGreen:   SkipSeconds(-(Setup.JumpSeconds)); break;
    case kYellow|k_Repeat:
                   SkipSeconds(Setup.JumpSecondsRepeat); break;
    case kYellow:  SkipSeconds(Setup.JumpSeconds); break;
    case kStop:
    case kBlue:    Hide();
                   Stop();
                   return osEnd;
//begin kw-Jumpseconds etc.
     case k1|k_Repeat:
            jmpWidth += Setup.JumpFramesRepeat;
                    displayFrames = false;
                    pkw = SkipFrames(-jmpWidth);
                    Goto(pkw, true);
                    break;
     case k1:
            jmpWidth = 1;
                    displayFrames = true;
                    pkw = SkipFrames(-1);
                    Goto(pkw, true);
                    break;
     case k3|k_Repeat:
            jmpWidth += Setup.JumpFramesRepeat;
                    displayFrames = false;
                    pkw = SkipFrames(jmpWidth);
                    Goto(pkw, true);
                    break;
     case k3:
            jmpWidth = 1;
                    displayFrames = true;
                    pkw = SkipFrames(1);
                    Goto(pkw, true);
                    break;
//begin kw-Jumpseconds etc.

    default: {
      DoShowMode = false;
      switch (int(Key)) {
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
          displayFrames = DisplayedFrames;
          switch (Key) {
            // Menu control:
            case kOk:      if (visible && !modeOnly) {
                              Hide();
                              DoShowMode = true;
                              }
                           else
                              Show();
                           break;
            case kBack:    if (Setup.DelTimeshiftRec) {
                              cRecordControl* rc = cRecordControls::GetRecordControl(fileName);
                              return rc && rc->InstantId() ? osEnd : osRecordings;
                              }
                           return osRecordings;
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
