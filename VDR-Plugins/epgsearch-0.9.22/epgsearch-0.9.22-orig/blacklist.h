#ifndef __EPGSEARCHBL_H
#define __EPGSEARCHBL_H

#include <vdr/plugin.h>

class cSearchExt;
class cSearchResults;

class cBlacklist : public cListObject {
  friend class cMenuEditSearchExt;
public:
  int      ID;
  char     search[MaxFileName];
  int      options;
  int      useTime;
  int      startTime;
  int      stopTime;
  int      useChannel;
  cChannel *channelMin;
  cChannel *channelMax;
  char*    channelGroup;
  int      useCase;
  int      mode;
  int      useTitle;
  int      useSubtitle;
  int      useDescription;
  int      useDuration;
  int      minDuration;
  int      maxDuration;
  int      useDayOfWeek;
  int      DayOfWeek;
  int      useEpisode;
  int      useExtEPGInfo;
  char**   catvalues;
  int      fuzzyTolerance;
  static char *buffer;
public:
  cBlacklist(void);
  virtual ~cBlacklist(void);
  cBlacklist& operator= (const cBlacklist&);
  virtual bool operator< (const cListObject &ListObject);

  const char *Search(void) { return search; }
  int Options(void) { return options; }
  int StartTime(void) { return startTime; }
  int StopTime(void) { return stopTime; }
  int UseChannel(void) { return useChannel; }
  cChannel *ChannelMin(void) { return channelMin; }
  cChannel *ChannelMax(void) { return channelMax; }
  cEvent * GetEventByBlacklist(const cSchedule *schedules, const cEvent *Start, int MarginStop = 0);
  bool MatchesExtEPGInfo(const cEvent* e);
  const char *ToText(void);
  bool Parse(const char *s);
  bool ParseExtEPGValues(const char *s);
  bool ParseExtEPGEntry(const char *s);
  bool Save(FILE *f);
  cSearchResults* Run(cSearchResults* pSearchResults = NULL, int MarginStop = 0);  
  void CopyFromTemplate(const cSearchExt* templ);
};

class cBlacklists : public cConfig<cBlacklist>, public cMutex {
public:
    int GetNewID(void);
    cBlacklist* GetBlacklistFromID(int ID);
    bool Exists(const cBlacklist* Blacklist);
};

class cBlacklistObject: public cListObject {
 public:
    cBlacklist* blacklist;
    cBlacklistObject(cBlacklist* Blacklist) : blacklist(Blacklist) {}
};

extern cBlacklists Blacklists;

#endif
