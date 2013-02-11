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
  static string decompress_string(const string& str);
  static string compress_string(const string& str, int compressionlevel = Z_BEST_COMPRESSION);
private:
  static string SafeCall(string (*)());
  static string SafeCall(string (*)(string), string arg);
  static string SafeCall(string (*)(string, string), string arg1, string arg2);
  static string GetTimersIntern();
  static string GetRecordingsIntern();
  static string GetChannelsIntern(string wantedChannels);
  static string SetChannelIntern(string wantedChannel);
  static string GetEventsIntern(string wantedChannels, string when);
  static string DelRecordingIntern(string index);
  static string SetTimerIntern(string args);
  static string SearchEventsIntern(string wantedChannels, string pattern);
  static bool IsWantedEvent(cEvent * event, string pattern);
  static bool IsWantedChannel(cChannel * channel, string wantedChannels);
  static bool IsWantedTime(time_t when, cEvent * event);
  static string MapSpecialChars(const char * text);
  static string MapSpecialChars(const cString text);
  static string MapSpecialChars(const string text);
  static string ToText(const cEvent * event);
  static string ToText(cTimer * timer);
  static string ToText(cRecording * recording);
  static string GetAudioTracks(const cChannel* channel);
  static string replaceAll(const string& where, const string& what, const string& replacement);
  static string UnMapSpecialChars(string text);
  static string Error(const string &error);
  static string SetTimerIntern(char c, string params);
  static int RecordingLengthInSeconds(cRecording* recording);
  static string ConvertWeekdays(int v);
  static int ConvertWeekdays(string v);
  static queue<int> ConvertToBinary(int v);
};
