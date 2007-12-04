/*
 * externalplayer-config.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "externalplayer-config.h"

sKeymap::sKeymap() {
  vdrKeyUp = new string("\e[A");
  vdrKeyDown = new string("\e[B");
  vdrKeyLeft = new string("\e[D");
  vdrKeyRight = new string("\e[C");
  vdrKeyOk = NULL;
  vdrKeyBack = NULL;
  vdrKeyRed = NULL;
  vdrKeyGreen = NULL;
  vdrKeyYellow = NULL;
  vdrKeyBlue = NULL;
  vdrKey0 = new string("0");
  vdrKey1 = new string("1");
  vdrKey2 = new string("2");
  vdrKey3 = new string("3");
  vdrKey4 = new string("4");
  vdrKey5 = new string("5");
  vdrKey6 = new string("6");
  vdrKey7 = new string("7");
  vdrKey8 = new string("8");
  vdrKey9 = new string("9");
  vdrKeyPlay = NULL;
  vdrKeyPause = NULL;
  vdrKeyStop = NULL;
  vdrKeyRecord = NULL;
  vdrKeyFastFwd = NULL;
  vdrKeyFaswRew = NULL;
  vdrKeyAudio = NULL;
  vdrKeySchedule = NULL;
  vdrKeyChannels = NULL;
  vdrKeyTimers = NULL;
  vdrKeyRecordings = NULL;
  vdrKeySetup = NULL;
  vdrKeyCommands = NULL;
  vdrKeyUser1 = NULL;
  vdrKeyUser2 = NULL;
  vdrKeyUser3 = NULL;
  vdrKeyUser4 = NULL;
  vdrKeyUser5 = NULL;
  vdrKeyUser6 = NULL;
  vdrKeyUser7 = NULL;
  vdrKeyUser8 = NULL;
  vdrKeyUser9 = NULL;
}

sKeymap::~sKeymap() {
  delete vdrKeyUp;
  delete vdrKeyDown;
  delete vdrKeyLeft;
  delete vdrKeyRight;
  delete vdrKeyOk;
  delete vdrKeyBack;
  delete vdrKeyRed;
  delete vdrKeyGreen;
  delete vdrKeyYellow;
  delete vdrKeyBlue;
  delete vdrKey0;
  delete vdrKey1;
  delete vdrKey2;
  delete vdrKey3;
  delete vdrKey4;
  delete vdrKey5;
  delete vdrKey6;
  delete vdrKey7;
  delete vdrKey8;
  delete vdrKey9;
  delete vdrKeyPlay;
  delete vdrKeyPause;
  delete vdrKeyStop;
  delete vdrKeyRecord;
  delete vdrKeyFastFwd;
  delete vdrKeyFaswRew;
  delete vdrKeyAudio;
  delete vdrKeySchedule;
  delete vdrKeyChannels;
  delete vdrKeyTimers;
  delete vdrKeyRecordings;
  delete vdrKeySetup;
  delete vdrKeyCommands;
  delete vdrKeyUser1;
  delete vdrKeyUser2;
  delete vdrKeyUser3;
  delete vdrKeyUser4;
  delete vdrKeyUser5;
  delete vdrKeyUser6;
  delete vdrKeyUser7;
  delete vdrKeyUser8;
  delete vdrKeyUser9;
}

// --- sPlayerArgs ----------------------------------------------------------

sPlayerArgs::sPlayerArgs() {
  menuEntry = "";
  playerCommand = "";
  slaveMode = false;
  playMode = pmExtern_THIS_SHOULD_BE_AVOIDED;
  deactivateRemotes = false;
  blockMenu = false;
  keys = new sKeymap();
}

sPlayerArgs::~sPlayerArgs() {
  delete keys;
}

// --- FileNotFoundException -------------------------------------------------

FileNotFoundException::FileNotFoundException(string nFilename) {
  filename = nFilename;
}

SyntaxErrorException::SyntaxErrorException(int nCharNumber, string * nConfigFileContent) {
  charNumber = nCharNumber;
  configFileContent = nConfigFileContent;
}

int SyntaxErrorException::getLineNumber() {
  int lineNumber = 0;
  for (int i = 0; i < charNumber; i++) {
    if ((*configFileContent)[i] == '\n') {
      lineNumber++;
    }
  }
  return lineNumber;
}

int SyntaxErrorException::getColumnNumber() {
  int columnNumber = 1;
  for (int i = 0; i < charNumber; i++) {
    if ((*configFileContent)[i] == '\n') {
      columnNumber = 1;
    }
    else {
      columnNumber++;
    }
  }
  return columnNumber;
}

// --- EntryMissingException -------------------------------------------------

EntryMissingException::EntryMissingException(string nPlayerCommand, string nMenuEntry, int nCharNumber, string * nConfigFileContent) {
  playerCommand = nPlayerCommand;
  menuEntry = nMenuEntry;
  charNumber = nCharNumber;
  configFileContent = nConfigFileContent;
}

int EntryMissingException::getLineNumber() {
  int lineNumber = 0;
  for (int i = 0; i < charNumber; i++) {
    if ((*configFileContent)[i] == '\n') {
      lineNumber++;
    }
  }
  return lineNumber;
}

// --- InvalidKeywordException -----------------------------------------------

InvalidKeywordException::InvalidKeywordException(string nKeyword, int nCharNumber, string * nConfigFileContent) {
  keyword = nKeyword;
  charNumber = nCharNumber;
  configFileContent = nConfigFileContent;
}

int InvalidKeywordException::getLineNumber() {
  int lineNumber = 0;
  for (int i = 0; i < charNumber; i++) {
    if ((*configFileContent)[i] == '\n') {
      lineNumber++;
    }
  }
  return lineNumber;
}

// --- cExternalplayerConfig -------------------------------------------------

cExternalplayerConfig::cExternalplayerConfig(string filename) {
  try {
    configFileContent = readConfigFile(filename);
    configuration = parseConfigFile();
  }
  catch (FileNotFoundException fnfEx) {
    configFileContent = NULL;
    isyslog("externalplayer-plugin: Configuration file \"%s\" not found!\n", fnfEx.getFilename().c_str());
  }
}

cExternalplayerConfig::~cExternalplayerConfig() {
  delete configFileContent;
  while (!configuration.empty()) {
    delete configuration.back();
    configuration.pop_back();
  }
}

string * cExternalplayerConfig::readConfigFile(string filename) {
  ifstream playerConfigStream;
  playerConfigStream.open(filename.c_str(), ios::in);

  if (playerConfigStream == NULL) {
    throw FileNotFoundException(filename);
  }

  string * configFileContent = new string();

  char buffer[256];

  while (!playerConfigStream.eof()) {
    playerConfigStream.getline(buffer, 256);
    (*configFileContent) = (*configFileContent) + '\n' + buffer;
  }
  (*configFileContent) = (*configFileContent) + '\n';

  playerConfigStream.close();

  return configFileContent;
}

list<sPlayerArgs *> cExternalplayerConfig::parseConfigFile() {
  list<sPlayerArgs *> configuration;
  sPlayerArgs * playerConfig = NULL;

  for (unsigned int i = 0; i < configFileContent->size(); i++) {
    switch ((*configFileContent)[i]) {
      case ' ':
      case '\n':
        break;
      case '#':
        while ((*configFileContent)[i] != '\n') {
          i++;
        }
        break;
      case '{':
        playerConfig = new sPlayerArgs();
        i++;
        try {
          playerConfig = getConfiguration(&i);
          configuration.push_back(playerConfig);
        }
        catch (EntryMissingException emEx) {
          if (emEx.getMenuEntry() == "") {
            isyslog("externalplayer-plugin: error in config file: \"MenuEntry\" missing or invalid, line %i",
                    emEx.getLineNumber());
          }
          if (emEx.getPlayerCommand() == "") {
            isyslog("externalplayer-plugin: error in config file: \"Command\" missing or invalid, line %i!\n",
                    emEx.getLineNumber());
          }
        }
        break;
      default:
        unsigned int errorPosition = i;
        while ((*configFileContent)[i] != '\n') {
          i++;
        }
        i++;
        isyslog("externalplayer-plugin: syntax error in config file: line %i, column %i! Ignoring rest of this line.",
                getLineNumberOfChar(errorPosition), getColumnNumberOfChar(errorPosition));
    }
  }

  return configuration;
}

sPlayerArgs * cExternalplayerConfig::getConfiguration(unsigned int * position) {
  sPlayerArgs * args = new sPlayerArgs();

  bool endOfFile = false;
  while ((*configFileContent)[*position] != '}' && !endOfFile) {
    switch ((*configFileContent)[*position]) {
      case ' ':
      case '\n':
        (*position)++;
        break;
      case '#':
        while ((*configFileContent)[*position] != '\n') {
          (*position)++;
        }
        (*position)++;
        break;
      default:
        if (*position >= configFileContent->size()) {
          isyslog("externalplayer-plugin: no \"}\" at end of file!");
          endOfFile = true;
        }
        else {
          try {
            sConfigEntry entry = getConfigEntry(position);
            processConfigEntry(args, entry, *position);
          }
          catch(SyntaxErrorException seEx) {
            isyslog("externalplayer-plugin: syntax error in config file: line %i, column %i! Ignoring entry.",
                    seEx.getLineNumber(), seEx.getColumnNumber());
          }
          catch (InvalidKeywordException ikEx) {
            isyslog("externalplayer-plugin: error in config file: invalig keyword \"%s\" line %i!",
                  ikEx.getKeyword().c_str(), ikEx.getLineNumber());
          }
        }
    }
  }

  if ((args->playerCommand == "") || (args->menuEntry == "")) {
    throw EntryMissingException(args->playerCommand, args->menuEntry, *position, configFileContent);
  }

  return args;
}

sConfigEntry cExternalplayerConfig::getConfigEntry(unsigned int * position) {
  sConfigEntry entry;

  while ((*configFileContent)[*position] != '=') {
    switch((*configFileContent)[*position]) {
      case '{':
      case '}':
      case '\n':
      case '\"':
      case ';':
        {
          unsigned int errorPosition = *position;
          (*position)++;
          while ((*configFileContent)[*position] != '\n' && (*configFileContent)[*position] != ';' \
                  && (*configFileContent)[*position] != '{' && (*configFileContent)[*position] != '}') {
            (*position)++;
          }
          if ((*configFileContent)[*position] != '{' && (*configFileContent)[*position] != '}') {
            (*position)++;
          }
          throw SyntaxErrorException(errorPosition, configFileContent);
        }
        break;
      case '#':
        {
          unsigned int errorPosition = *position;
          (*position)++;
          while ((*configFileContent)[*position] != '\n' && (*configFileContent)[*position] != ';' \
                  && (*configFileContent)[*position] != '{' && (*configFileContent)[*position] != '}') {
            (*position)++;
          }
          if ((*configFileContent)[*position] != '{' && (*configFileContent)[*position] != '}') {
            (*position)++;
          }
          throw SyntaxErrorException(errorPosition, configFileContent);
        }
        break;
      default:
        entry.key += (*configFileContent)[*position];
        (*position)++;
    }
  }
  (*position)++;

  while ((*configFileContent)[*position] != ';') {
    switch((*configFileContent)[*position]) {
      case '{':
      case '}':
      case '\n':
      case '=':
        {
          unsigned int errorPosition = *position;
          (*position)++;
          while ((*configFileContent)[*position] != '\n' && (*configFileContent)[*position] != ';' \
                  && (*configFileContent)[*position] != '{' && (*configFileContent)[*position] != '}') {
            (*position)++;
          }
          if ((*configFileContent)[*position] != '{' && (*configFileContent)[*position] != '}') {
            (*position)++;
          }
          throw SyntaxErrorException(errorPosition, configFileContent);
        }
      break;
      case '\"':
        while ((*configFileContent)[*position] != '\"') {
          (*position)++;
          if ((*configFileContent)[*position] == '\n') {
            throw SyntaxErrorException(*position, configFileContent);
          }
          entry.value += (*configFileContent)[*position];
        }
        (*position)++;
        break;
      case '#':
        {
          int errorPosition = *position;
          (*position)++;
          while ((*configFileContent)[*position] != '\n' && (*configFileContent)[*position] != ';' \
                  && (*configFileContent)[*position] != '{' && (*configFileContent)[*position] != '}') {
            (*position)++;
          }
          if ((*configFileContent)[*position] != '{' && (*configFileContent)[*position] != '}') {
            (*position)++;
          }
          throw SyntaxErrorException(errorPosition, configFileContent);
        }
        break;
      default:
        entry.value += (*configFileContent)[*position];
        (*position)++;
    }
  }
  (*position)++;

  removeUnnecessarySymbols(&(entry.key));
  removeUnnecessarySymbols(&(entry.value));

  return entry;
}

void cExternalplayerConfig::removeUnnecessarySymbols(string * stringPtr) {
  while ((*stringPtr)[0] == ' ') {
    stringPtr->erase(stringPtr->begin());
  }

  while ((*stringPtr)[stringPtr->size() - 1] == ' ') {
    stringPtr->erase((stringPtr->end() - 1), (stringPtr->end()));
  }

}

void cExternalplayerConfig::processConfigEntry(sPlayerArgs * args, sConfigEntry entry, int position) {
  if (entry.key == "Command") {
    args->playerCommand = entry.value;
  }

  else if (entry.key == "MenuEntry") {
    args->menuEntry = entry.value;
  }

  else if (entry.key == "InputMode") {
    if (entry.value == "deactivateRemotes") {
      args->deactivateRemotes = true;
      args->slaveMode = false;
    }
    else if (entry.value == "slave") {
      args->deactivateRemotes = false;
      args->slaveMode = true;
    }
    else if (entry.value == "normal" || entry.value == "default") {
      args->deactivateRemotes = false;
      args->slaveMode = false;
    }
    else {
      throw InvalidKeywordException(entry.value, position, configFileContent);
    }
  }

  else if (entry.key == "OutputMode") {
    if (entry.value == "extern") {
      args->playMode = pmExtern_THIS_SHOULD_BE_AVOIDED;
    }
    else if (entry.value == "none") {
      args->playMode = pmNone;
    }
    else if (entry.value == "audioOnly") {
      args->playMode = pmAudioOnly;
    }
    else if (entry.value == "audioOnlyBlack") {
      args->playMode = pmAudioOnlyBlack;
    }
  }

  else if (entry.key == "BlockMenu") {
    if ((entry.value == "true") || (entry.value == "1")) {
      args->blockMenu = true;
    }
    else if (entry.value == "false" || (entry.value == "0")) {
      args->blockMenu = false;
    }
    else {
      throw InvalidKeywordException(entry.value, position, configFileContent);
    }
  }

  else if (entry.key == "vdrKeyUp") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUp;
        args->keys->vdrKeyUp = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUp;
      args->keys->vdrKeyUp = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyDown") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyDown;
        args->keys->vdrKeyDown = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyDown;
      args->keys->vdrKeyDown = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyLeft") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyLeft;
        args->keys->vdrKeyLeft = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyLeft;
      args->keys->vdrKeyLeft = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyRight") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyRight;
        args->keys->vdrKeyRight = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyRight;
      args->keys->vdrKeyRight = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyOk") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyOk;
        args->keys->vdrKeyOk = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyOk;
      args->keys->vdrKeyOk = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyBack") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyBack;
        args->keys->vdrKeyBack = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyBack;
      args->keys->vdrKeyBack = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyRed") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyRed;
        args->keys->vdrKeyRed = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyRed;
      args->keys->vdrKeyRed = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyGreen") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyGreen;
        args->keys->vdrKeyGreen = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyGreen;
      args->keys->vdrKeyGreen = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyYellow") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyYellow;
        args->keys->vdrKeyYellow = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyYellow;
      args->keys->vdrKeyYellow = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyBlue") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyBlue;
        args->keys->vdrKeyBlue = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyBlue;
      args->keys->vdrKeyBlue = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey0") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey0;
        args->keys->vdrKey0 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey0;
      args->keys->vdrKey0 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey1") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey1;
        args->keys->vdrKey1 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey1;
      args->keys->vdrKey1 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey2") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey2;
        args->keys->vdrKey2 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey2;
      args->keys->vdrKey2 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey3") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey3;
        args->keys->vdrKey3 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey3;
      args->keys->vdrKey3 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey4") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey4;
        args->keys->vdrKey4 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey4;
      args->keys->vdrKey4 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey5") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey5;
        args->keys->vdrKey5 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey5;
      args->keys->vdrKey5 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey6") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey6;
        args->keys->vdrKey6 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey6;
      args->keys->vdrKey6 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey7") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey7;
        args->keys->vdrKey7 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey7;
      args->keys->vdrKey7 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey8") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey8;
        args->keys->vdrKey8 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey8;
      args->keys->vdrKey8 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKey9") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKey9;
        args->keys->vdrKey9 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKey9;
      args->keys->vdrKey9 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyPlay") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyPlay;
        args->keys->vdrKeyPlay = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyPlay;
      args->keys->vdrKeyPlay = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyPause") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyPause;
        args->keys->vdrKeyPause = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyPause;
      args->keys->vdrKeyPause = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyStop") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyStop;
        args->keys->vdrKeyStop = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyStop;
      args->keys->vdrKeyStop = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyRecord") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyRecord;
        args->keys->vdrKeyRecord = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyRecord;
      args->keys->vdrKeyRecord = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyFastFwd") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyFastFwd;
        args->keys->vdrKeyFastFwd = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyFastFwd;
      args->keys->vdrKeyFastFwd = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyFaswRew") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyFaswRew;
        args->keys->vdrKeyFaswRew = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyFaswRew;
      args->keys->vdrKeyFaswRew = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyAudio") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyAudio;
        args->keys->vdrKeyAudio = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyAudio;
      args->keys->vdrKeyAudio = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeySchedule") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeySchedule;
        args->keys->vdrKeySchedule = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeySchedule;
      args->keys->vdrKeySchedule = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyChannels") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyChannels;
        args->keys->vdrKeyChannels = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyChannels;
      args->keys->vdrKeyChannels = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyTimers") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyTimers;
        args->keys->vdrKeyTimers = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyTimers;
      args->keys->vdrKeyTimers = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyRecordings") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyRecordings;
        args->keys->vdrKeyRecordings = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyRecordings;
      args->keys->vdrKeyRecordings = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeySetup") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeySetup;
        args->keys->vdrKeySetup = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeySetup;
      args->keys->vdrKeySetup = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyCommands") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyCommands;
        args->keys->vdrKeyCommands = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyCommands;
      args->keys->vdrKeyCommands = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyUser1") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUser1;
        args->keys->vdrKeyUser1 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUser1;
      args->keys->vdrKeyUser1 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyUser2") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUser2;
        args->keys->vdrKeyUser2 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUser2;
      args->keys->vdrKeyUser2 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyUser3") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUser3;
        args->keys->vdrKeyUser3 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUser3;
      args->keys->vdrKeyUser3 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyUser4") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUser4;
        args->keys->vdrKeyUser4 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUser4;
      args->keys->vdrKeyUser4 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyUser5") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUser5;
        args->keys->vdrKeyUser5 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUser5;
      args->keys->vdrKeyUser5 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyUser6") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUser6;
        args->keys->vdrKeyUser6 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUser6;
      args->keys->vdrKeyUser6 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyUser7") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUser7;
        args->keys->vdrKeyUser7 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUser7;
      args->keys->vdrKeyUser7 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyUser8") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUser8;
        args->keys->vdrKeyUser8 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUser8;
      args->keys->vdrKeyUser8 = new string(entry.value);
    }
  }

  else if (entry.key == "vdrKeyUser9") {
    if (entry.value.size() > 1) {
      string * keyString = getCodeSpecialKey(entry.value);
      if (keyString != NULL) {
        delete args->keys->vdrKeyUser9;
        args->keys->vdrKeyUser9 = keyString;
      }
      else {
        throw InvalidKeywordException(entry.value, position, configFileContent);
      }
    }
    else {
      delete args->keys->vdrKeyUser9;
      args->keys->vdrKeyUser9 = new string(entry.value);
    }
  }

  else if (entry.key == "" || entry.value == "") {
    throw SyntaxErrorException(position, configFileContent);
  }

  else {
    throw InvalidKeywordException(entry.key, position, configFileContent);
  }
}

string * cExternalplayerConfig::getCodeSpecialKey(string name) {
  if (name == "noKey") {
    return new string("");
  }
  else if (name == "specialKeyUp") {
    return new string("\e[A");
  }
  else if (name == "specialKeyDown") {
    return new string("\e[B");
  }
  else if (name == "specialKeyRight") {
    return new string("\e[C");
  }
  else if (name == "specialKeyLeft") {
    return new string("\e[D");
  }
  else if (name == "specialKeyF1") {
    return new string("\eOP");
  }
  else if (name == "specialKeyF2") {
    return new string("\eOQ");
  }
  else if (name == "specialKeyF3") {
    return new string("\eOR");
  }
  else if (name == "specialKeyF4") {
    return new string("\eOS");
  }
  else if (name == "specialKeyF5") {
    return new string("\e[15~");
  }
  else if (name == "specialKeyF6") {
    return new string("\e[17~");
  }
  else if (name == "specialKeyF7") {
    return new string("\e[18~");
  }
  else if (name == "specialKeyF8") {
    return new string("\e[19~");
  }
  else if (name == "specialKeyF9") {
    return new string("\e[20~");
  }
  else if (name == "specialKeyF10") {
    return new string("\e[21~");
  }
  else if (name == "specialKeyF11") {
    return new string("\e[23~");
  }
  else if (name == "specialKeyF12") {
    return new string("\e[24~");
  }
  else if (name == "specialKeyIns") {
    return new string("\e[2~");
  }
  else if (name == "specialKeyDel") {
    return new string("\e[3~");
  }
  else if (name == "specialKeyHome") {
    return new string("\e[H");
  }
  else if (name == "specialKeyEnd") {
    return new string("\e[F");
  }
  else if (name == "specialKeyPageUp") {
    return new string("\e[5~");
  }
  else if (name == "specialKeyPageDown") {
    return new string("\e[6~");
  }
  else if (name == "specialKeySpace") {
    return new string(" ");
  }
  else if (name == "specialKeyReturn") {
    return new string("\n");
  }
  else {
    return NULL;
  }
}

unsigned int cExternalplayerConfig::getLineNumberOfChar(unsigned int charNumber) {
  unsigned int lineNumber = 0;

  for (unsigned int i = 0; i < charNumber; i++) {
    if ((*configFileContent)[i] == '\n') {
      lineNumber++;
    }
  }

  return lineNumber;
}

unsigned int cExternalplayerConfig::getColumnNumberOfChar(unsigned int charNumber) {
  unsigned int columnNumber = 1;

  for (unsigned int i = 0; i < charNumber; i++) {
    if ((*configFileContent)[i] == '\n') {
      columnNumber = 1;
    }
    else {
      columnNumber++;
    }
  }

  return columnNumber;
}
