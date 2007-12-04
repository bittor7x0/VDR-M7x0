/*
 * state.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE


#include <stdarg.h>
#include <stdio.h>

#include "global.h"
#include "state.h"
#include "formatter.h"



cCtrlState* glCtrlState = 0;



cCtrlState::cCtrlState()
: _mode			(smEmpty),
  _oldItem		(-1),
  _title		(0),
  _red			(0),
  _green		(0),
  _yellow		(0),
  _blue			(0),
  _message		(0),
  _textItem		(0),
  _channel		(0),
  _presentTitle		(0),
  _presentSubtitle	(0),
  _followingTitle	(0),
  _followingSubtitle	(0)
{
  glCtrlState = this;

  for (int i = 0; i < MaxTabs; ++i)
    _tabWidths[i] = 0;
}



cCtrlState::~cCtrlState() {

  OsdClear();

  glCtrlState = 0;
}



// Indicates a channel switch on the given DVB device.
// If ChannelNumber is 0, this is before the channel is being switched,
// otherwise ChannelNumber is the number of the channel that has been switched to.
void cCtrlState::ChannelSwitch(const cDevice *Device, int ChannelNumber) {
}



// The given DVB device has started recording Name. Name is the name of the
// recording, without any directory path.
// If Name is NULL, the recording has ended.
void cCtrlState::Recording(const cDevice *Device, const char *Name) {
//printf("Recording %i, %s", Device->CardIndex(), Name );
}



// The given player control has started replaying Name. Name is the name of the
// recording, without any directory path. In case of a player that can't provide
// a name, Name can be a string that identifies the player type (like, e.g., "DVD").
// If Name is NULL, the replay has ended.
void cCtrlState::Replaying(const cControl *Control, const char *Name) {
//printf("Replaying %s", Name );
}



// The volume has been set to the given value, either
// absolutely or relative to the current volume.
void cCtrlState::SetVolume(int Volume, bool Absolute) {
//printf("Set Volume %i, %i\n", Volume, Absolute);
}



// The OSD has been cleared.
void cCtrlState::OsdClear() {

  _mode = smEmpty;

  _menuItems.Clear();
  _oldItem  = -1;

  free(_title);			_title = 0;
  free(_red);			_red = 0;
  free(_green);			_green = 0;
  free(_yellow);		_yellow = 0;
  free(_blue);			_blue = 0;
  free(_message);		_message = 0;
  free(_textItem);		_textItem = 0;
  free(_channel);		_channel = 0;
  free(_presentTitle);		_presentTitle = 0;
  free(_presentSubtitle);	_presentSubtitle = 0;
  free(_followingTitle);	_followingTitle = 0;
  free(_followingSubtitle);	_followingSubtitle = 0;

  for (int i = 0; i < MaxTabs; ++i)
    _tabWidths[i] = 0;

  for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
    Formatter->OsdClear();
  }
}



// Title has been displayed in the title line of the menu.
void cCtrlState::OsdTitle(const char *Title) {

  free(_title);
  _title = Title ? strdup(Title) : 0;

  _mode = smMenu;

  for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
    Formatter->OsdTitle(Title);
  }
}



// Message has been displayed in the status line of the menu.
// If Message is NULL, the status line has been cleared.
void cCtrlState::OsdStatusMessage(const char *Message) {

  free(_message);
  _message = Message ? strdup(Message) : 0;

  if (_mode == smEmpty)
    _mode = smMessage;

  for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
    Formatter->OsdStatusMessage(Message);
  }
}



// The help keys have been set to the given values (may be NULL).
void cCtrlState::OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue) {

  free(_red);
  free(_green);
  free(_yellow);
  free(_blue);

  _red    = Red    ? strdup(Red)    : 0;
  _green  = Green  ? strdup(Green)  : 0;
  _yellow = Yellow ? strdup(Yellow) : 0;
  _blue   = Blue   ? strdup(Blue)   : 0;

  _mode = smMenu;

  for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
    Formatter->OsdHelpKeys(Red, Green, Yellow, Blue);
  }
}



// The OSD displays the given single line Text as menu item at Index.
void cCtrlState::OsdItem(const char *Text, int Index) {

  char*& String = _menuItems[Index];

  free(String);
  String = Text ? strdup(Text) : 0;

  if (Text) {

    const char* found, *lastFound = Text;
    for (int i = 0; i < MaxTabs && (found = strchr(lastFound, '\t')); ++i) {

      int tabWidth = found - lastFound + 1;
      if (tabWidth > _tabWidths[i])
        _tabWidths[i] = tabWidth;

      lastFound = found + 1;
    }
  }

  _mode = smMenu;

//  for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
//    Formatter->OsdItem(Text, Index);
//  }
}



// The OSD displays the given single line Text as the current menu item.
void cCtrlState::OsdCurrentItem(const char *Text) {

  if (! Text)
    return;

  long currentItem = _menuItems.Search(Text);
  if (currentItem < 0) {
    currentItem = _oldItem;
    if (currentItem >= 0) {
      char*& String = _menuItems[currentItem];
      free(String);
      String = Text ? strdup(Text) : 0;

//      for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
//        Formatter->OsdItem(Text, currentItem);
//      }
    }
  } else {
    if (currentItem == _oldItem)
      return;
  }

  for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
    Formatter->OsdCurrentItem(currentItem, _oldItem);
  }

  _oldItem = currentItem;
}



// The OSD displays the given multi line text. If Text points to an
// actual string, that text shall be displayed and Scroll has no
// meaning. If Text is NULL, Scroll defines whether the previously
// received text shall be scrolled up (true) or down (false) and
// the text shall be redisplayed with the new offset.
void cCtrlState::OsdTextItem(const char *Text, bool Scroll) {

  if (Text) {
    free(_textItem);
    _textItem = strdup(Text);
  }

  for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
    Formatter->OsdTextItem(Text, Scroll ? 1 : -1);
  }

  _mode = smMenu;
}



// The OSD displays the single line Text with the current channel information.
void cCtrlState::OsdChannel(const char *Text) {

  free(_channel);
  _channel = Text ? strdup(Text) : 0;

  _mode = smInfo;

  for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
    Formatter->OsdChannel(Text);
  }
}



// The OSD displays the given programme information.
void cCtrlState::OsdProgramme(time_t PresentTime, const char *PresentTitle, const char *PresentSubtitle, time_t FollowingTime, const char *FollowingTitle, const char *FollowingSubtitle) {

  free(_presentTitle);
  free(_presentSubtitle);
  free(_followingTitle);
  free(_followingSubtitle);

  _presentTime = PresentTime;
  _presentTitle = PresentTitle ? strdup(PresentTitle) : 0;
  _presentSubtitle = PresentSubtitle ? strdup(PresentSubtitle) : 0;

  _followingTime = FollowingTime;
  _followingTitle = FollowingTitle ? strdup(FollowingTitle) : 0;
  _followingSubtitle = FollowingSubtitle ? strdup(FollowingSubtitle) : 0;

  _mode = smInfo;

  for (cCtrlFormatter *Formatter = gl_CtrlFormatterCol.First(); Formatter; Formatter = gl_CtrlFormatterCol.Next(Formatter)) {
    Formatter->OsdProgramme(PresentTime, PresentTitle, PresentSubtitle, FollowingTime, FollowingTitle, FollowingSubtitle);
  }
}


