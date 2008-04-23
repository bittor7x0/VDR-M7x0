/*
 * telnet.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */



#ifndef _CTRL_TELNET_
#define _CTRL_TELNET_



#include "stack.h"



#define CHAR(x) ((uchar)x)


// Telnet control commands
// -----------------------

#define TELNET_SE 	CHAR(240)	// End of subnegotiation parameters.
#define TELNET_NOP	CHAR(241)	// No operation.
#define TELNET_DM	CHAR(242)	// Data Mark: The data stream portion of a Synch.
					// This should always be accompanied
					// by a TCP Urgent notification.
#define TELNET_BREAK	CHAR(243)	// Break: NVT character BRK.
#define TELNET_IP	CHAR(244)	// Interrupt Process
#define TELNET_AO	CHAR(245)	// Abort output
#define TELNET_AYT	CHAR(246)	// Are You There
#define TELNET_EC	CHAR(247)	// Erase character
#define TELNET_EL	CHAR(248)	// Erase Line
#define TELNET_GA	CHAR(249)	// Go ahead
#define TELNET_SB	CHAR(250)	// Indicates that what follows is
					// subnegotiation of the indicated option.


#define TELNET_WILL	CHAR(251)	// Indicates the desire to begin performing, or confirmation that
					// you are now performing, the indicated option.
#define TELNET_WONT	CHAR(252)	// Indicates the refusal to perform, or continue performing, the
                                	// indicated option.
#define TELNET_DO	CHAR(253)	// Indicates the request that the other party perform confirmation
					// that you are expecting the other party to perform, the indicated option.
#define TELNET_DONT	CHAR(254)	// Indicates the demand that the other party stop performing,
					// or confirmation that you are no longer expecting the other party
					// to perform, the indicated option.
#define TELNET_IAC	CHAR(255)	// IAC: Data Byte 255.





// Telnet options
// --------------

#define OPTION_ECHO	CHAR(1)		// Echo
#define OPTION_SGA	CHAR(3)		// Suppress Go Ahead
#define OPTION_TT	CHAR(24)	// Terminal Type
#define OPTION_NAWS	CHAR(31)	// Window Size





class cCtrlTelnet
: public cCtrlThier
{

public:
#define MAXTELNETOPTIONS 20

protected:

  enum eOptionMode {
    omInvalid,		// Only for initialization
    omLocal,
    omRemote
  };

  enum eAgreeState {
    asNotSupported,	// Default value
    asNotNegotiated,
    asPending,
    asRequested,
    asAgreed,
    asRejected
  };

  enum eTelnetState {
    teStNormal,
    teStInCommand,
    teStWill,
    teStWont,
    teStDo,
    teStDont,
    teStInSubCommand,
    teStEndSubCommand,
    teStReturn
  };

  struct sOptionEntry {
    sOptionEntry(uchar Option = TELNET_IAC)
    : option(Option), optionMode(omInvalid), agreeState(asNotSupported) {}

    uchar       option;
    eOptionMode optionMode;
    eAgreeState agreeState;
  };

  // Implemented options
  static const uchar localOptions[];
  static const uchar remoteOptions[];

public:

  cCtrlTelnet();

  bool useOption(uchar option, bool local, bool activate);

// cCtrlThier
  virtual int dataFromHigherThier(const uchar*, int size);
  virtual int dataFromLowerThier(const uchar*, int size);
  virtual int start();
  virtual void stop() {_started = false;}

  int width()  {return _width;}
  int height() {return _height;}


protected:

  bool setOption(uchar option, bool local, eAgreeState state);
  eAgreeState option(uchar option, bool local);

  void onCommandAreYouThere();
  void onCommandWill(uchar ch);
  void onCommandWont(uchar ch);
  void onCommandDo(uchar ch);
  void onCommandDont(uchar ch);

  void onSubCommand();

  void out(uchar ch);
  void flushOut();
  void accuSubCommand(uchar ch);

  int sendDataDown(const uchar* pData, int size) {
    return cCtrlThier::sendDataDown(pData, size);
  }

  int sendDataDown(uchar a, uchar b, uchar c) {
    uchar buf[3] = {a, b, c};
    return cCtrlThier::sendDataDown(buf, 3);
  }

  void activateOption(uchar option, eAgreeState state);
  eAgreeState isOptionActivated(uchar option);


protected:
  bool         _started;
  eTelnetState _state;
  sOptionEntry _optionModes[MAXTELNETOPTIONS];

  uchar        _output[256];
  uchar*       _outputPos;
  uchar*       _outputEnd;

  uchar        _subCommand[256];
  uchar*       _subCommandPos;
  uchar*       _subCommandEnd;

  // Option parameters
  int          _width, _height;
};



#endif


