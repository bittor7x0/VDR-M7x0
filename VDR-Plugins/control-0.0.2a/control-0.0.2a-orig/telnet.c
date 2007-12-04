/*
 * telnet.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE



#include <malloc.h>
#include <string.h>
//#include <sys/socket.h>

#include <vdr/remote.h>
#include <vdr/tools.h>

#include "global.h"
#include "telnet.h"



const uchar cCtrlTelnet::localOptions[] = {OPTION_ECHO, OPTION_SGA, 255};
const uchar cCtrlTelnet::remoteOptions[] = {OPTION_SGA, OPTION_NAWS, 255};



cCtrlTelnet::cCtrlTelnet()
: _started       (false),
  _state         (teStNormal),
  _outputPos     (_output),
  _outputEnd     (_output + 255),
  _subCommandPos (_subCommand),
  _subCommandEnd (_subCommand + 255),
  _width         (80),
  _height        (24)
{
  // Initialize all implemented options
  int i = 0;
  const uchar* pOptions = localOptions;
  while (*pOptions != 255) {

    _optionModes[i].option     = *pOptions;
    _optionModes[i].optionMode = omLocal;
    _optionModes[i].agreeState = asNotNegotiated;
    ++i; ++pOptions;
  }

  pOptions = remoteOptions;
  while (*pOptions != 255) {

    _optionModes[i].option     = *pOptions;
    _optionModes[i].optionMode = omRemote;
    _optionModes[i].agreeState = asNotNegotiated;
    ++i; ++pOptions;
  }
}



int cCtrlTelnet::start() {

  // The protocol stack is up - now try to get options

conLOG_TELNET1("cCtrlTelnet::start: negotiating required options %d", MAXTELNETOPTIONS);

  for (int i = 0; i < MAXTELNETOPTIONS; ++i) {

    if (_optionModes[i].agreeState == asPending) {

      _optionModes[i].agreeState = asRequested;

      if (_optionModes[i].optionMode == omLocal) {

conLOG_TELNET1("cCtrlTelnet::start: sending WILL (%d)", _optionModes[i].option);
        sendDataDown(TELNET_IAC, TELNET_WILL, _optionModes[i].option);

      } else {

conLOG_TELNET1("cCtrlTelnet::start: sending DO (%d)", _optionModes[i].option);
        sendDataDown(TELNET_IAC, TELNET_DO, _optionModes[i].option);
      }
    }
  }

  _started = true;
  return 0;
}



bool cCtrlTelnet::useOption(uchar option, bool local, bool activate) {

  if (! _started) {

    // Not running...
    if (activate) {
      // ... mark option to be activated if telnet starts

      // search for option
      bool found = false;
      for (int i = 0; i < MAXTELNETOPTIONS; ++i) {

        if (_optionModes[i].option == option &&
            (_optionModes[i].optionMode == (local ? omLocal : omRemote))) {

          _optionModes[i].agreeState = asPending;
          found = true;
          break;
        }
      }

      if (! found)
        conESYSLOG("useOption: option not implemented %d (local=%s, activate=%s)", option, local?"true":"false", activate?"true":"false");

      return found;
    }
  } else {
    // Already running - activate at once
    // not yet implemented :-(
  }
  return false;
}



int cCtrlTelnet::dataFromLowerThier(const uchar* pData, int size) {

  for (int i = 0; i < size; ++i) {

    switch (_state) {

    case teStNormal:

      switch (*pData) {
      case TELNET_IAC:	_state = teStInCommand; break;
      case ((char)8):   out(((char)0x7f)); break;  // Translate to backspace
      case 0xD:         out(*pData); _state = teStReturn; break; // Translate different return types
      default:		out(*pData); break;
      }
      break;

    case teStInCommand:

      switch (*pData) {

      // Repeated IAC -> output one
      case TELNET_IAC:  out(*pData); _state = teStNormal; break;
      case TELNET_SB:   _state = teStInSubCommand; break;

      case TELNET_AYT:  onCommandAreYouThere(); _state = teStNormal; break;

      // Options
      case TELNET_WILL: _state = teStWill; break;
      case TELNET_WONT: _state = teStWont; break;
      case TELNET_DO:   _state = teStDo;   break;
      case TELNET_DONT: _state = teStDont; break;

      // Unknown command -> ignore it
      default: _state = teStNormal; break;
      }
      break;

    case teStWill: onCommandWill(*pData); _state = teStNormal; break;
    case teStWont: onCommandWont(*pData); _state = teStNormal; break;
    case teStDo:   onCommandDo  (*pData); _state = teStNormal; break;
    case teStDont: onCommandDont(*pData); _state = teStNormal; break;

    case teStInSubCommand:

      switch (*pData) {
      case TELNET_IAC: _state = teStEndSubCommand; break;
      default:         accuSubCommand(*pData); break;
      }
      break;

    case teStEndSubCommand:

      switch (*pData) {
      case TELNET_IAC: accuSubCommand(*pData); _state = teStInSubCommand; break;
      case TELNET_SE:  onSubCommand(); _state = teStNormal; break;
      default:
        conESYSLOG("Invalid subnogitation received (%d)", *_subCommand);
        _subCommandPos = _subCommand;
        _state = teStNormal;
        break;
      }
      break;

    case teStReturn:

      switch (*pData) {
      case 0x0: _state = teStNormal; break;
      case 0xA: _state = teStNormal; break;
      default:
        _state = teStNormal;
        dataFromLowerThier(pData, 1); // Retry the actual char
        break;
      }
      break;

    // Unknown state???
    default: _state = teStNormal; break;
    }

    ++pData;
  }

  // Are there any pending data?
  flushOut();

  return 0;
}



void cCtrlTelnet::flushOut() {

  // Are there any pending data?
  if (_outputPos > _output) {

    // Then... send
    sendDataUp(_output, _outputPos - _output);
    _outputPos = _output;
  }
}



void cCtrlTelnet::onCommandAreYouThere() {

  static const uchar* msg = (const uchar*)"\n\r[vdr : yes]\n\r";

  for (int i = 0; i < (int)sizeof(msg); ++i)
    out(msg[i]);
  flushOut();
//  sendDataDown(msg, sizeof(msg)-1);
}



void cCtrlTelnet::onCommandWill(uchar ch) {

conLOG_TELNET2("cCtrlTelnet::onCommandWill %d", ch);

  eAgreeState state = option(ch, false);

  switch (state) {

  case asNotSupported:

conLOG_TELNET1("cCtrlTelnet::onCommandWill: DONT");
    sendDataDown(TELNET_IAC, TELNET_DONT, ch);
    break;

  case asNotNegotiated:
  case asPending:
  case asRejected:

conLOG_TELNET1("cCtrlTelnet::onCommandWill: AGREED");
    sendDataDown(TELNET_IAC, TELNET_DO, ch);

  case asRequested:
    setOption(ch, false, asAgreed);
    break;

  case asAgreed:
    // nothing to do
conLOG_TELNET1("cCtrlTelnet::onCommandWill: IGNORED");
    break;

  default:
conLOG_TELNET1("cCtrlTelnet::onCommandWill: OOPS");
    assert(NULL);
  }
}



void cCtrlTelnet::onCommandWont(uchar ch) {

conLOG_TELNET1("cCtrlTelnet::onCommandWont %d", ch);

  eAgreeState state = option(ch, false);

  switch (state) {

  case asNotSupported:

conLOG_TELNET1("cCtrlTelnet::onCommandWont: REJECTED");
    sendDataDown(TELNET_IAC, TELNET_DONT, ch);
    break;

  case asNotNegotiated:
  case asPending:
  case asAgreed:

conLOG_TELNET1("cCtrlTelnet::onCommandWont: REJECTED");
    sendDataDown(TELNET_IAC, TELNET_DONT, ch);

  case asRequested:
    setOption(ch, false, asRejected);
    break;

  case asRejected:
    // nothing to do
conLOG_TELNET1("cCtrlTelnet::onCommandWont: IGNORED");
    break;

  default:
conLOG_TELNET1("cCtrlTelnet::onCommandWont: OOPS");
    assert(null);
  }
}



void cCtrlTelnet::onCommandDo(uchar ch) {

conLOG_TELNET1("cCtrlTelnet::onCommandDo %d", ch);

  eAgreeState state = option(ch, true);

  switch (state) {

  case asNotSupported:

conLOG_TELNET1("cCtrlTelnet::onCommandDo: WONT");
    sendDataDown(TELNET_IAC, TELNET_WONT, ch);
    break;

  case asNotNegotiated:
  case asPending:
  case asRejected:

conLOG_TELNET1("cCtrlTelnet::onCommandDo: WILL");
    sendDataDown(TELNET_IAC, TELNET_WILL, ch);

  case asRequested:
    setOption(ch, true, asAgreed);
    break;

  case asAgreed:
    // nothing to do
conLOG_TELNET1("cCtrlTelnet::onCommandDo: IGNORED");
    break;

  default:
conLOG_TELNET1("cCtrlTelnet::onCommandDo: OOPS");
    assert(null);
  }
}



void cCtrlTelnet::onCommandDont(uchar ch) {

conLOG_TELNET1("cCtrlTelnet::onCommandDont %d", ch);

  eAgreeState state = option(ch, true);

  switch (state) {

  case asNotSupported:

conLOG_TELNET1("cCtrlTelnet::onCommandDont: WONT");
    sendDataDown(TELNET_IAC, TELNET_WONT, ch);
    break;

  case asNotNegotiated:
  case asPending:
  case asAgreed:

conLOG_TELNET1("cCtrlTelnet::onCommandDont: WONT");
    sendDataDown(TELNET_IAC, TELNET_WONT, ch);

  case asRequested:
    setOption(ch, true, asRejected);
    break;

  case asRejected:
    // nothing to do
conLOG_TELNET1("cCtrlTelnet::onCommandDont: IGNORED");
    break;

  default:
conLOG_TELNET1("cCtrlTelnet::onCommandDont: OOPS");
    assert(null);
  }
}



void cCtrlTelnet::onSubCommand() {

  switch (*_subCommand) {
  case OPTION_NAWS:

    // Consistency check
    if (_subCommandPos - _subCommand == 5) {

      short width, height;
      ((uchar*)&width)[1]  = _subCommand[1];
      ((uchar*)&width)[0]  = _subCommand[2];
      ((uchar*)&height)[1] = _subCommand[3];
      ((uchar*)&height)[0] = _subCommand[4];

      // New width or height reported?
      if (width >= 0)  _width = width;
      if (height >= 0) _height = height;

      //@@MISSING send event
conLOG_TELNET1("cCtrlTelnet::onSubCommand NAWS %d, %d", _width, _height);
    } else {
      conESYSLOG("subcommand received NAWS - rejected because of invalid size (%d)", _subCommandPos - _subCommand);
    }
    break;

  default:
conLOG_TELNET2("cCtrlTelnet::onSubCommand %d", *_subCommand);
  }

  _subCommandPos = _subCommand;
}



inline void cCtrlTelnet::out(uchar ch) {

  *_outputPos++ = ch;

  // If the buffer is full...
  if (_outputPos >= _outputEnd) {

    // ... send pending data out
    flushOut();
  }
}



inline void cCtrlTelnet::accuSubCommand(uchar ch) {

  // If the buffer is not full...
  if (_subCommandPos < _subCommandEnd) {
    // accumulate the new char
    *_subCommandPos++ = ch;
  }
}



bool cCtrlTelnet::setOption(uchar Option, bool local, eAgreeState State) {

  // search for option
  for (int i = 0; i < MAXTELNETOPTIONS; ++i) {

    if (_optionModes[i].option == Option &&
       (_optionModes[i].optionMode == (local ? omLocal : omRemote))) {

      _optionModes[i].agreeState = State;
      return true;
    }
  }

  // option not found
  return false;
}



cCtrlTelnet::eAgreeState cCtrlTelnet::option(uchar Option, bool local) {

conLOG_TELNET1("cCtrlTelnet::option: searching option (%d)", Option);

  for (int i = 0; i < MAXTELNETOPTIONS; ++i) {

conLOG_TELNET1("cCtrlTelnet::option: test option (%d)", _optionModes[i].option);

    if (_optionModes[i].option == Option &&
       (_optionModes[i].optionMode == (local ? omLocal : omRemote))) {

conLOG_TELNET1("cCtrlTelnet::option: returning agree state (%d)", _optionModes[i].agreeState);
      return _optionModes[i].agreeState;
    }
  }

  // Option unknown
conLOG_TELNET1("cCtrlTelnet::option: returning agree state: asNotSupported");
  return asNotSupported;
}



int cCtrlTelnet::dataFromHigherThier(const uchar* pData, int size) {
  return sendDataDown(pData, size);
}


