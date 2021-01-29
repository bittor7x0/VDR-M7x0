/*
 * helper tools
 */

#include <time.h>
#include <string>
#include <vdr/epg.h>
#include <zlib.h>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <queue>
#include <set>


using namespace std;

class cHelpers
{
public:
  static string GetTimers(string args);
  static string GetChannels(string args);
  static string GetChannelEvents(string args);
  static string SetChannel(string args);
  static string GetTimeEvents(string args);
  static string GetRecordings(string args);
  static string DelRecording(string args);
  static string SetTimer(string args);
  static string SearchEvents(string args);
  static string ToUpper(string text);
  static string ToLower(string text);
  static string Trim(string text);
  static long 	Duration(const cRecording* recording);
private:
  static string SafeCall(string (*)());
  static string SafeCall(string (*)(string), string arg);
  static string SafeCall(string (*)(string, string), string arg1, string arg2);
  static string GetTimersIntern(string options);
  static string GetRecordingsIntern();
  static string GetChannelsIntern(string wantedChannels);
  static string SetChannelIntern(string wantedChannel);
  static string GetEventsIntern(string wantedChannels, string when);
  static string DelRecordingIntern(string index);
  static string SetTimerIntern(string args);
  static string SearchEventsIntern(string wantedChannels, string pattern);
  static bool IsWantedEvent(const cEvent * event, string pattern);
  static bool IsWantedChannel(const cChannel * channel, string wantedChannels);
  static bool IsWantedTime(time_t when, const cEvent * event);
  static string MapSpecialChars(const char * text);
  static string MapSpecialChars(const cString text);
  static string MapSpecialChars(const string text);
  static string ToText(const cEvent * event);
  static string ToText(const cTimer * timer, set<string> conflicts);
  static string ToText(const cRecording * recording);
  static string GetAudioTracks(const cChannel* channel);
  static string replaceAll(const string& where, const string& what, const string& replacement);
  static string UnMapSpecialChars(string text);
  static string Error(const string &error);
  static string SetTimerIntern(char c, string params);
  static int RecordingLengthInSeconds(const cRecording* recording);
  static string ConvertWeekdays(int v);
  static int ConvertWeekdays(string v);
  static queue<int> ConvertToBinary(int v);
  static string DelRecording(cRecording * r);
  static set<string> GetTimerConflicts();
};
