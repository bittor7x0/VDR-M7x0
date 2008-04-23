/*
 * state.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _CTRLSTATE_H_
#define _CTRLSTATE_H_


#include <vdr/status.h>

#include "StringArray.h"



class cCtrlState : public cStatus {

public:
  cCtrlState();
  virtual ~cCtrlState();

  enum {
    MaxTabs = 6
  };

  enum eStateMode {
    smEmpty,
    smMenu,
    smMessage,
    smInfo
  };

  eStateMode  mode()              {return _mode;}

  const StringArray& menuItems()  {return _menuItems;}
  const int*  tabWidths()         {return _tabWidths;}
  long        currentItem()       {return _oldItem;}

  const char* title()             {return _title;}

  const char* red()               {return _red;}
  const char* green()             {return _green;}
  const char* yellow()            {return _yellow;}
  const char* blue()              {return _blue;}

  const char* message()           {return _message;}
  const char* textItem()	  {return _textItem;}
  const char* channel()           {return _channel;}
  time_t      presentTime()       {return _presentTime;}
  const char* presentTitle()      {return _presentTitle;}
  const char* presentSubtitle()   {return _presentSubtitle;}
  time_t      followingTime()     {return _followingTime;}
  const char* followingTitle()    {return _followingTitle;}
  const char* followingSubtitle() {return _followingSubtitle;}


// cStatus
protected:
  virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
  virtual void Recording(const cDevice *Device, const char *Name);
  virtual void Replaying(const cControl *Control, const char *Name);
  virtual void SetVolume(int Volume, bool Absolute);
  virtual void OsdClear();
  virtual void OsdTitle(const char *Title);
  virtual void OsdStatusMessage(const char *Message);
  virtual void OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue);
  virtual void OsdItem(const char *Text, int Index);
  virtual void OsdCurrentItem(const char *Text);
  virtual void OsdTextItem(const char *Text, bool Scroll);
  virtual void OsdChannel(const char *Text);
  virtual void OsdProgramme(time_t PresentTime, const char *PresentTitle, const char *PresentSubtitle, time_t FollowingTime, const char *FollowingTitle, const char *FollowingSubtitle);

private:

  eStateMode  _mode;

  StringArray _menuItems;
  long        _oldItem;
  int         _tabWidths[MaxTabs];

  char*       _title;

  char*       _red;
  char*       _green;
  char*       _yellow;
  char*       _blue;

  char*       _message;

  char*       _textItem;
  char*       _channel;
  time_t      _presentTime;
  char*       _presentTitle;
  char*       _presentSubtitle;
  time_t      _followingTime;
  char*       _followingTitle;
  char*       _followingSubtitle;
};



extern cCtrlState* glCtrlState;



#endif // _CTRLSTATE_H_


