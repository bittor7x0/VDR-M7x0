#ifndef __EPGSEARCHEXT_H
#define __EPGSEARCHEXT_H

#include <vdr/menu.h>
#include <vdr/menuitems.h>
#include <vdr/status.h>
#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <vdr/skins.h>

#include "log.h"
#include "epgsearchtools.h"

#define MAXOSDTEXTWIDTH 45
#define ERROR(T) Skins.Message(mtError, T)
#define INFO(I)  Skins.Message(mtInfo, I)

typedef enum
{
    blacklistsNo = 0,
    blacklistsSelection,
    blacklistsAll
} blacklistModes;

typedef enum
{
    searchTimerActionRecord=0,
    searchTimerActionAnnounceOnly,
    searchTimerActionSwitchOnly
} searchTimerAction;

class cSearchExt;
class cBlacklist;

class cSearchResult : public cListObject {
 public:
    const cEvent* event;
    const cSearchExt* search;
    const cBlacklist* blacklist;
    bool needsTimer;
    cSearchResult(const cEvent* Event, const cSearchExt* Search) : event(Event), search(Search), blacklist(NULL), needsTimer(true) {}
    cSearchResult(const cEvent* Event, const cBlacklist* Blacklist) : event(Event), search(NULL), blacklist(Blacklist), needsTimer(true) {}
};

class cSearchResults : public cList<cSearchResult> {
 public:

    void SortBy(int(*compar)(const void *, const void *))
	{
	    int n = Count();
	    cListObject *a[n];
	    cListObject *object = objects;
	    int i = 0;
	    while (object && i < n) {
		a[i++] = object;
		object = object->Next();
	    }
	    qsort(a, n, sizeof(cListObject *), compar);
	    objects = lastObject = NULL;
	    for (i = 0; i < n; i++) {
		a[i]->Unlink();
		count--;
		Add(a[i]);
	    }
	}
    bool Lookup(const cEvent* Event)
	{
	    cSearchResult* r = First();
	    while(r)
	    {
		if (r->event == Event)
		    return true;
		r = Next(r);
	    }
	    return false;
	}

};

class cBlacklistObject;
class cTimerObj;
class cTimerObjList;

class cSearchExt : public cListObject {
  friend class cMenuEditSearchExt;
  friend class cMenuEditTemplate;
public:
  int      ID;
  char     search[MaxFileName];
  int      options;
  int      useTime;
  int      startTime;
  int      stopTime;
  int      useChannel;
  int      useCase;
  int      mode;
  int      useTitle;
  int      useSubtitle;
  int      useDescription;
  int      useDuration;
  int      minDuration;
  int      maxDuration;
  int      useAsSearchTimer;
  int      useDayOfWeek;
  int      DayOfWeek;
  int      useEpisode;
  char     directory[MaxFileName];
  int      Priority;
  int      Lifetime;
  int      MarginStart;
  int      MarginStop;
  int      useVPS;
  int      action;
  int      useExtEPGInfo;
  char**   catvalues;
  cChannel *channelMin;
  cChannel *channelMax;
  char*    channelGroup;
  int      avoidRepeats;
  int      compareTitle;
  int      compareSubtitle;
  int      compareSummary;
  int      allowedRepeats;
  unsigned long catvaluesAvoidRepeat;
  int      repeatsWithinDays;
  int      delAfterDays;
  int      recordingsKeep;
  int      switchMinsBefore;
  int      pauseOnNrRecordings;
  int      blacklistMode;
  cList<cBlacklistObject> blacklists;
  int      fuzzyTolerance;
  int      useInFavorites;
  int      menuTemplate;

  static char *buffer;
public:
  cSearchExt(void);
  virtual ~cSearchExt(void);
  cSearchExt& operator= (const cSearchExt &SearchExt);
  virtual bool operator< (const cListObject &ListObject);

  const char *Search(void) { return search; }
  int Options(void) { return options; }
  int StartTime(void) { return startTime; }
  int StopTime(void) { return stopTime; }
  int UseChannel(void) { return useChannel; }
  cChannel *ChannelMin(void) { return channelMin; }
  cChannel *ChannelMax(void) { return channelMax; }
  cEvent * GetEventBySearchExt(const cSchedule *schedules, const cEvent *Start, bool inspectTimerMargin = false);
  bool MatchesExtEPGInfo(const cEvent* e);
  const char *ToText();
  bool Parse(const char *s);
  bool ParseExtEPGValues(const char *s);
  bool ParseExtEPGEntry(const char *s);
  bool ParseBlacklistIDs(const char *s);
  bool Save(FILE *f);
  char* BuildFile(const cEvent* pEvent) const;
  cSearchResults* Run(int PayTVMode = -1, bool inspectTimerMargin = false, int evalLimitMins = 0, cSearchResults* pPrevResults = NULL, bool suppressRepeatCheck = false); 
  void CheckRepeatTimers(cSearchResults* pResults);
  void CheckExistingRecordings(cSearchResults* pResults);
  void CopyFromTemplate(const cSearchExt* templ);
  cSearchResults* GetBlacklistEvents(int MarginStop = 0);
  void OnOffTimers(bool);
  void DeleteAllTimers();
  cTimerObjList* GetTimerList(cTimerObjList* timerList);
  int GetCountRecordings();
};

class cSearchExts : public cList<cSearchExt>, public cMutex {
 private:
    char *fileName;
    bool allowComments;
    void Clear(void)
	{
	    cMutexLock SearchExtsLock(this);
	    free(fileName);
	    fileName = NULL;
	    cList<cSearchExt>::Clear();
	}
 public:
  cSearchExts(void) { fileName = NULL; }
  virtual ~cSearchExts() { free(fileName); }

public:
    bool Load(const char *FileName = NULL);
    bool Save(void);
    void Update(void);
    int GetNewID(void);
    cSearchExt* GetSearchFromID(int ID);
    void RemoveBlacklistID(int ID);
    bool Exists(const cSearchExt* SearchExt);
    cSearchExts* Clone();
};

extern cSearchExts SearchExts;
extern cSearchExts SearchTemplates;

#endif
