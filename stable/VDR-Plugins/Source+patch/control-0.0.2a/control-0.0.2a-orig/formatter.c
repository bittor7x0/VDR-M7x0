/*
 * formatter.c: A plugin for the Video Disk Recorder
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
#include "formatter.h"
#include "gateway.h"
#include "state.h"



cCtrlFormatterCol gl_CtrlFormatterCol;



cCtrlFormatter::cCtrlFormatter(cCtrlGateway* gateway)
: _gateway        (gateway),
  _width          (0),
  _height         (0),
  _textItemOffset (0)
{
  gl_CtrlFormatterCol.Add(this);

  // The correct size will be set throught the gateway as soon as possible
  //SetSize(80, 24);
}



cCtrlFormatter::~cCtrlFormatter() {
}



void cCtrlFormatter::Send(const char* Msg, ...) {

  va_list ap;
  va_start(ap, Msg);

  char *buffer = NULL;
  if (vasprintf(&buffer, Msg, ap) > 0) {

    // Send the message to all active gateways
    _gateway->SendMsg(buffer);
  }

  free(buffer);
  va_end(ap);
}



void cCtrlFormatter::SetSize(int width, int height) {

  if (width == _width && height == _height)
    return;

  _width = width; _height = height;


  // Clear OSD
  Send("\033[H\033[2J");


  // Recalculate view
  // ----------------

  switch (glCtrlState->mode()) {
  case cCtrlState::smMenu:

    OsdTitle(glCtrlState->title());
    OsdHelpKeys(glCtrlState->red(), glCtrlState->green(), glCtrlState->yellow(), glCtrlState->blue());

    if (glCtrlState->currentItem() >= 0) {
      OsdCurrentItem(glCtrlState->currentItem(), -1);
    } else if (glCtrlState->textItem()) {
      OsdTextItem(glCtrlState->textItem(), 0);
    }

    if (glCtrlState->message())
      OsdStatusMessage(glCtrlState->message());

    break;

  case cCtrlState::smMessage:

    OsdStatusMessage(glCtrlState->message());
    break;

  case cCtrlState::smInfo:

    OsdChannel(glCtrlState->channel());
    OsdProgramme(glCtrlState->presentTime(), glCtrlState->presentTitle(), glCtrlState->presentSubtitle(),
                 glCtrlState->followingTime(), glCtrlState->followingTitle(), glCtrlState->followingSubtitle());
    break;

  default:
    break;
  }
}



// Indicates a channel switch on the given DVB device.
// If ChannelNumber is 0, this is before the channel is being switched,
// otherwise ChannelNumber is the number of the channel that has been switched to.
//void cCtrlFormatter::ChannelSwitch(const cDevice *Device, int ChannelNumber) {
//}



// The given DVB device has started recording Name. Name is the name of the
// recording, without any directory path.
// If Name is NULL, the recording has ended.
//void cCtrlFormatter::Recording(const cDevice *Device, const char *Name) {
//printf("Recording %i, %s", Device->CardIndex(), Name );
//}



// The given player control has started replaying Name. Name is the name of the
// recording, without any directory path. In case of a player that can't provide
// a name, Name can be a string that identifies the player type (like, e.g., "DVD").
// If Name is NULL, the replay has ended.
//void cCtrlFormatter::Replaying(const cControl *Control, const char *Name) {
//printf("Replaying %s", Name );
//}



// The volume has been set to the given value, either
// absolutely or relative to the current volume.
//void cCtrlFormatter::SetVolume(int Volume, bool Absolute) {
//printf("Set Volume %i, %i\n", Volume, Absolute);
//}



// The OSD has been cleared.
void cCtrlFormatter::OsdClear(void) {

  Send("\033[H\033[2J");

  _textItem.Clear();
  _textItemOffset = 0;
}



// Title has been displayed in the title line of the menu.
void cCtrlFormatter::OsdTitle(const char *Title) {

  if (Title) {

    char* t = strchr(Title, '\t');
    if (t) {

      int size = t - Title;
      int width = _width - 2 - size;
      if (width < 0) width = 0;

      Send("\033[H\033[0;44m\033[K %-*.*s%*.*s\033[m", size, size, Title, width, width, t + 1);

    } else {

      int pos = (_width - 1 - (int)strlen(Title)) / 2;
      if (pos < 1)
        pos = 1;
      int width = _width - 2 - pos;
      if (width < 0) width = 0;

      Send("\033[H\033[0;44m\033[K%*s%.*s\033[m", pos, "", width, Title);
    }

  } else {
    Send("\033[H\033[0;44m\033[K\033[m");
  }
}



// Message has been displayed in the status line of the menu.
// If Message is NULL, the status line has been cleared.
void cCtrlFormatter::OsdStatusMessage(const char *Message) {

  if (Message) {

    int pos = (_width - 1 - (int)strlen(Message)) / 2;
    if (pos < 1) pos = 1;
    int width = _width - 1 - pos;
    if (width < 0) width = 0;

    Send("\033[%i;1f\033[30;43m\033[K%*s%.*s\033[m", _height - 1, pos, "", width, Message);

  } else {
    Send("\033[%i;1f\033[m\033[K", _height - 1);
  }
}



// The help keys have been set to the given values (may be NULL).
void cCtrlFormatter::OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue) {

  // Meassure text lengths
  int redLen    = Red    ? strlen(Red)    : 0;
  int greenLen  = Green  ? strlen(Green)  : 0;
  int yellowLen = Yellow ? strlen(Yellow) : 0;
  int blueLen   = Blue   ? strlen(Blue)   : 0;

  // Calculate some indicators
//  int buttonCount = Red ? 1 : 0 + Green ? 1 : 0 + Yellow ? 1 : 0 + Blue ? 1 : 0;
//  int neededSpace = redLen + greenLen + yellowLen + blueLen;
//  int averageButtonWidth = neededSpace / buttonCount;
  int optimalButtonWidth = (_width / 4) - 2;
  if (optimalButtonWidth < 0) optimalButtonWidth = 0;


  int redSpace, greenSpace, yellowSpace, blueSpace;
  int redWidth, greenWidth, yellowWidth, blueWidth;

//  if (averageButtonWidth <= optimalButtonWidth) {

    redWidth    = redLen    > optimalButtonWidth ? optimalButtonWidth : redLen;
    greenWidth  = greenLen  > optimalButtonWidth ? optimalButtonWidth : greenLen;
    yellowWidth = yellowLen > optimalButtonWidth ? optimalButtonWidth : yellowLen;
    blueWidth   = blueLen   > optimalButtonWidth ? optimalButtonWidth : blueLen;

    redSpace    = (optimalButtonWidth - redWidth) / 2;
    greenSpace  = (optimalButtonWidth - greenWidth) / 2;
    yellowSpace = (optimalButtonWidth - yellowWidth) / 2;
    blueSpace   = (optimalButtonWidth - blueWidth) / 2;

//  } else {
//  }

  if (redSpace < 0)      redSpace = 0;
  if (greenSpace < 0)    greenSpace = 0;
  if (yellowSpace < 0)   yellowSpace = 0;
  if (blueSpace < 0)     blueSpace = 0;


  Send("\033[%i;1f %s%*s%.*s%*s\033[m  %s%*s%.*s%*s\033[m  %s%*s%.*s%*s\033[m  %s%*s%.*s%*s\033[m",

    _height,
    Red    ? "\033[0;41m"  : "",
    redSpace, "", redWidth,
    Red    ? Red           : "",
    redSpace, "",

    Green  ? "\033[30;42m" : "",
    greenSpace, "", greenWidth,
    Green  ? Green         : "",
    greenSpace, "",

    Yellow ? "\033[30;43m" : "",
    yellowSpace, "", yellowWidth,
    Yellow ? Yellow        : "",
    yellowSpace, "",

    Blue   ? "\033[44m"    : "",
    blueSpace, "", blueWidth,
    Blue   ? Blue          : "",
    blueSpace, "" );
}



// The OSD displays the given single line Text as menu item at Index.
void cCtrlFormatter::OsdItem(const char *Text, int Index) {
}



int cCtrlFormatter::CalcMaxValidTextLength(const char* Text, int startPos, int endPos) {

  if (! Text)
    return 0;

  const char* text = Text;
  int stringLength = 0;		// Max. length of string that can be used
  int textPos = startPos;	// Resulting text position

  const int* tabs = glCtrlState->tabWidths();
  int tab = -1;
  int tabPos = 0;

  for (;;) {

    char* t = strchr(text, '\t');
    if (t) {
      // Tab found - check if the tab can be used

      // Calculate the next tabulator position
      if (++tab < cCtrlState::MaxTabs) {
        tabPos += tabs[tab] + 1;
      } else {
        ++tabPos;
      }

      if (tabPos <= endPos) {
        stringLength = t - Text + 1;
        textPos = tabPos;
      } else {
        // The tab is out of valid range - shorten the text
        int pendingWidth = t - text;
        if (textPos + pendingWidth > endPos) {
          stringLength += endPos - textPos + 1;
        } else {
          stringLength += pendingWidth;
        }
        // No need to calc the resulting textPos
        break;
      }

      text = t + 1;

    } else {
      // No further tab found
      int pendingWidth = strlen(text);
      if (textPos + pendingWidth > endPos) {
        stringLength += endPos - textPos + 1;
      } else {
        stringLength += pendingWidth;

        // The calculated string length and the really
        // string length should be the same.
        assert(stringLength == strlen(Text));
      }
      // No need to calc the resulting textPos
      break;
    }
  }

  return stringLength;
}



// The OSD displays the given single line Text as the current menu item.
//void cCtrlFormatter::OsdCurrentItem(const char *Text) {
void cCtrlFormatter::OsdCurrentItem(int currentItem, int oldItem) {

  const int height = _height - 4;

  long oldPage = oldItem < 0 ? -1 : oldItem / height;
  long newPage = currentItem < 0 ? -1 : currentItem / height;

  // Cursor switched to new page?
  if (oldPage != newPage) {

    // Setting tabs
    Send("\033[3g");
    int tabPos = 0;
    const int* tabs = glCtrlState->tabWidths();
    for (int i = 0; i < cCtrlState::MaxTabs && tabs[i]; ++i) {
      tabPos += tabs[i] + 1;
      Send("\033[1;%if\033H", tabPos);
    }

    // Display new page
    long lastEntry = (newPage + 1) * height;
    if (lastEntry > glCtrlState->menuItems().Count())
      lastEntry = glCtrlState->menuItems().Count();

    for (long i = newPage * height; i < lastEntry; ++i) {

      const char* text = glCtrlState->menuItems()[i];
      int maxWidth = CalcMaxValidTextLength(text, 1, _width);
      if (i != currentItem)
        Send("\033[%i;1f\033[K%.*s\033[m",           (i % height) + 3, maxWidth, text);
      else
        Send("\033[%i;1f\033[0;44m\033[K%.*s\033[m", (i % height) + 3, maxWidth, text);
    }

    // Clear the last rows.
    if (lastEntry % height != 0)
      for (long i = lastEntry % height; i < height; ++i)
        Send("\033[%i;1f\033[K", i + 3 );

  } else {

    // Incremental update of current page.
    if (oldItem >= 0) {

      const char* text = glCtrlState->menuItems()[oldItem];
      int maxWidth = CalcMaxValidTextLength(text, 1, _width);
      Send("\033[%i;1f\033[K%.*s\033m", (oldItem % height) + 3, maxWidth, text);
    }

    if (currentItem >= 0) {

      const char* text = glCtrlState->menuItems()[currentItem];
      int maxWidth = CalcMaxValidTextLength(text, 1, _width);
      Send("\033[%i;1f\033[0;44m\033[K%.*s\033[m", (currentItem % height) + 3, maxWidth, text);
    }
  }
}



// The OSD displays the given multi line text. If Text points to an
// actual string, that text shall be displayed and Scroll has no
// meaning. If Text is NULL, Scroll defines whether the previously
// received text shall be scrolled up (true) or down (false) and
// the text shall be redisplayed with the new offset.
void cCtrlFormatter::OsdTextItem(const char *Text, int Scroll) {

  const int height = _height - 4;

  if (Text) {

    _textItem.Clear();
    _textItemOffset = 0;

    long line = 0;

    for (;;) {

      const char* lastBreak = Text;
      const char* breakAt = Text;

      while (*breakAt != 0 && *breakAt != '\n' && breakAt - Text + 1 <= _width - 1) {

        lastBreak = breakAt;
        breakAt = strpbrk(breakAt + 1, " -.,:;!?_\n");

        // No breakable space found? -> take until end of string
        if (! breakAt)
          breakAt = lastBreak + strlen(lastBreak);
      }

      // If the last found breakable space is in the valid range
      // then take it insted of the previous found space.
      if (breakAt - Text + 1 <= _width - 1)
        lastBreak = breakAt;

      // Copy the relevant text into the string array.
      char*& text = _textItem[line++];
      text = (char*)malloc(lastBreak - Text + 2);
      strn0cpy(text, Text, lastBreak - Text + 1);

      if (! *lastBreak)
        break;

      Text = lastBreak + 1;
    }

  } else {

    if (Scroll > 0) {
      if (--_textItemOffset < 0)
        _textItemOffset = 0;

    } else if (Scroll < 0) {

      if (++_textItemOffset > _textItem.Count() - height)
        _textItemOffset = _textItem.Count() - height;
      if (_textItemOffset < 0)
        _textItemOffset = 0;
    }
  }


  int count = height;
  if (count > _textItem.Count())
    count = _textItem.Count();

  int i;
  for (i = 0; i < count; ++i) {

    const char* text = _textItem[_textItemOffset + i];
    Send("\033[%i;1f\033[K%.*s", i + 3, _width - 1, text ? text : "");
  }

  // Clear the lowest line in case we scrolled up
  if (i < height)
    Send("\033[%i;1f\033[K", i + 3);
}



// The OSD displays the single line Text with the current channel information.
void cCtrlFormatter::OsdChannel(const char *Text) {

  time_t timet;
  time(&timet);

  static char szActualTime[25];
  struct tm tm_r;
  strftime(szActualTime, sizeof(szActualTime), "%R", localtime_r(&timet, &tm_r));

  int size = Text ? strlen(Text) : 0;
  if (size > _width - 10)
    size = _width - 10;
  int width = _width - 10;
  if (width < 0)
    width = 0;
  int width2 = _width - 10 - size;
  if (width2 < 0)
     width2 = 0;

  Send("\033[%i;1f\033[30;44m  \033[30;47m%.*s\033[30;44m%*s\033[30;47m%s\033[30;44m\033[K\033[m",
    _height - 5, _width - 10,
    Text ? Text : "", _width - 10 - size, "", szActualTime);
}



// The OSD displays the given programme information.
void cCtrlFormatter::OsdProgramme(time_t PresentTime, const char *PresentTitle, const char *PresentSubtitle, time_t FollowingTime, const char *FollowingTitle, const char *FollowingSubtitle) {

  static char szPresentTime  [25];
  static char szFollowingTime[25];

  struct tm tm_r;
  strftime(szPresentTime, sizeof(szPresentTime), "%R", localtime_r(&PresentTime, &tm_r));
  strftime(szFollowingTime, sizeof(szFollowingTime), "%R", localtime_r(&FollowingTime, &tm_r));


  Send( "\033[%i;1f\033[41m %s \033[44m %.*s\033[K"
        "\033[%i;1f\033[41m       \033[33;44m %.*s\033[K"
        "\033[%i;1f\033[0;41m       \033[44m\033[K"
        "\033[%i;1f\033[41m %s \033[44m %.*s\033[K"
        "\033[%i;1f\033[41m       \033[33;44m %.*s\033[K\033[m",

    _height - 4, szPresentTime,
    _width - 10, PresentTitle ? PresentTitle : "",
    _height - 3, _width - 10, PresentSubtitle   ? PresentSubtitle   : "",

    _height - 2, 

    _height - 1, szFollowingTime,
    _width - 10, FollowingTitle ? FollowingTitle : "",
    _height - 0, _width - 10, FollowingSubtitle ? FollowingSubtitle : "");
}


