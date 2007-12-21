/*
 * keyboard.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */


#ifndef _CTRLKEYBOARD_H_
#define _CTRLKEYBOARD_H_

#include <termios.h>

#include <vdr/remote.h>
#include <vdr/thread.h>



class cCtrlKeyboard
: public cKbdRemote
//: public cRemote
{

//private:
public:
  static cCtrlKeyboard* _pRemote;

//private:
  cCtrlKeyboard();
  virtual ~cCtrlKeyboard();

  bool intPut(uint64 Command, bool Repeat, bool Release);

public:
  static bool Put(uint64 Command, bool Repeat = false, bool Release = false);
  static int MapCodeToFunc(uint64 Code);
};



#endif // _CTRLKEYBOARD_H_


