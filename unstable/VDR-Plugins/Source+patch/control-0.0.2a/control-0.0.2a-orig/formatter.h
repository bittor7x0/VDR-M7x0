/*
 * formatter.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _CTRLFORMATTER_H_
#define _CTRLFORMATTER_H_



// @@TODO replace with corresponding vdr includes
#include "state.h"


class cCtrlGateway;





class cCtrlFormatter
: public cListObject
{

public:
  cCtrlFormatter(cCtrlGateway* gateway);
  virtual ~cCtrlFormatter();

  virtual void SetSize(int width, int height);

//  virtual void ChannelSwitch(const cDevice *Device, int ChannelNumber);
//  virtual void Recording(const cDevice *Device, const char *Name);
//  virtual void Replaying(const cControl *Control, const char *Name);
//  virtual void SetVolume(int Volume, bool Absolute);
  virtual void OsdClear(void);
  virtual void OsdTitle(const char *Title);
  virtual void OsdStatusMessage(const char *Message);
  virtual void OsdHelpKeys(const char *Red, const char *Green, const char *Yellow, const char *Blue);
  virtual void OsdItem(const char *Text, int Index);
//  virtual void OsdCurrentItem(const char *Text);
  virtual void OsdCurrentItem(int currentItem, int oldItem);
  virtual void OsdTextItem(const char *Text, int Scroll);
  virtual void OsdChannel(const char *Text);
  virtual void OsdProgramme(time_t PresentTime, const char *PresentTitle, const char *PresentSubtitle, time_t FollowingTime, const char *FollowingTitle, const char *FollowingSubtitle);

protected:
  void Send(const char* Msg, ...);

  //! Calculates the max string length that can be send to the
  //! client.
  //! For this to work, the tabulator positions have to be
  //! included in the calculation because these a only a single
  //! character in the string but 1 to n characters for the
  //! client.
  //!
  //! \param Text The text to be meassured
  //! \param startPos X position at which the text starts
  //! \param endPos Max valid X position.
  //! \return The max. length of the string which sould be
  //!         send to the client.
  int CalcMaxValidTextLength(const char* Text, int startPos, int endPos);

private:
  cCtrlGateway* _gateway;
  int _width, _height;
  StringArray _textItem;
  int _textItemOffset;
};



class cCtrlFormatterCol : public cList<cCtrlFormatter> {};

extern cCtrlFormatterCol gl_CtrlFormatterCol;



#endif // _CTRLFORMATTER_H_


