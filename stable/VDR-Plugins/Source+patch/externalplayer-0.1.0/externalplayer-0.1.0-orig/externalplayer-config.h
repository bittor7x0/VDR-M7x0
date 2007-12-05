/*
 * externalplayer-config.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <string>
#include <list>
#include <fstream>
#include <cstring>

#include <vdr/device.h>
#include <vdr/tools.h>

#ifndef _EXTERNALPLAYER_CONFIG_H_
#define _EXTERNALPLAYER_CONFIG_H_

using namespace std;

struct sKeymap {
  string * vdrKeyUp;
  string * vdrKeyDown;
  string * vdrKeyLeft;
  string * vdrKeyRight;
  string * vdrKeyOk;
  string * vdrKeyBack;
  string * vdrKeyRed;
  string * vdrKeyGreen;
  string * vdrKeyYellow;
  string * vdrKeyBlue;
  string * vdrKey0;
  string * vdrKey1;
  string * vdrKey2;
  string * vdrKey3;
  string * vdrKey4;
  string * vdrKey5;
  string * vdrKey6;
  string * vdrKey7;
  string * vdrKey8;
  string * vdrKey9;
  string * vdrKeyPlay;
  string * vdrKeyPause;
  string * vdrKeyStop;
  string * vdrKeyRecord;
  string * vdrKeyFastFwd;
  string * vdrKeyFaswRew;
  string * vdrKeyChannelUp;
  string * vdrKeyChannelDown;
  string * vdrKeyAudio;
  string * vdrKeySchedule;
  string * vdrKeyChannels;
  string * vdrKeyTimers;
  string * vdrKeyRecordings;
  string * vdrKeySetup;
  string * vdrKeyCommands;
  string * vdrKeyUser1;
  string * vdrKeyUser2;
  string * vdrKeyUser3;
  string * vdrKeyUser4;
  string * vdrKeyUser5;
  string * vdrKeyUser6;
  string * vdrKeyUser7;
  string * vdrKeyUser8;
  string * vdrKeyUser9;
  sKeymap();
  ~sKeymap();
};

struct sPlayerArgs {
  string menuEntry;
  string playerCommand;
  ePlayMode playMode;
  bool slaveMode;
  bool deactivateRemotes;
  bool blockMenu;
  sKeymap * keys;
  sPlayerArgs();
  ~sPlayerArgs();
};

struct sConfigEntry {
  string key;
  string value;
};

class FileNotFoundException {
private:
  string filename;
public:
  FileNotFoundException(string nFilename);
  string getFilename() { return filename; }
};

class SyntaxErrorException {
private:
  int charNumber;
  string * configFileContent;
public:
  SyntaxErrorException(int nCharNumber, string * nConfigFileContent);
  int getLineNumber();
  int getColumnNumber();
};

class EntryMissingException {
private:
  string playerCommand;
  string menuEntry;
  int charNumber;
  string * configFileContent;
public:
  EntryMissingException(string nPlayerCommand, string nMenuEntry, int nCharNumber, string * nConfigFileContent);
  string getPlayerCommand() { return playerCommand; }
  string getMenuEntry() { return menuEntry; }
  int getLineNumber();
};

class InvalidKeywordException {
private:
  string keyword;
  int charNumber;
  string * configFileContent;
public:
  InvalidKeywordException(string nKeyword, int nCharNumber, string * nConfigFileContent);
  string getKeyword() { return keyword; }
  int getLineNumber();
};

class cExternalplayerConfig {
private:
  string * configFileContent;
  list<sPlayerArgs *> configuration;
  string * readConfigFile(string filename);
  list<sPlayerArgs *> parseConfigFile();
  sPlayerArgs * getConfiguration(unsigned int * position);
  sConfigEntry getConfigEntry(unsigned int * position);
  void removeUnnecessarySymbols(string * stringPtr);
  void processConfigEntry(sPlayerArgs * args, sConfigEntry entry, int position);
  string * getCodeSpecialKey(string name);
  unsigned int getLineNumberOfChar(unsigned int charNumber);
  unsigned int getColumnNumberOfChar(unsigned int charNumber);
public:
  cExternalplayerConfig(string filename);
  ~cExternalplayerConfig();
  list<sPlayerArgs *> getConfiguration() { return configuration; }
  int playerCount() { return configuration.size(); }
};

#endif /*_EXTERNALPLAYER_CONFIG_H_*/
