#ifndef __EPGSEARCH_TOOLS_INC__
#define __EPGSEARCH_TOOLS_INC__

#include <string>
#include <vdr/config.h> // For VDRVERSNUM only
#include <vdr/font.h>
#include "epgsearchext.h"
#include "recdone.h"

using std::string;

#define MAXPARSEBUFFER KILOBYTE(10)

#undef CHANNELNAME
#define CHANNELNAME(x) (x ? x->ShortName(true) : "")

#undef TIMESTRING
#define TIMESTRING(x) *(TimeString(x)) 

#undef DATESTRING
#define DATESTRING(x) *(DateString(x)) 

#undef GETDATESTRING
#define GETDATESTRING(x) *(x->GetDateString()) 

#undef GETTIMESTRING
#define GETTIMESTRING(x) *(x->GetTimeString()) 

#undef PRINTDAY
#define PRINTDAY *cTimer::PrintDay

#undef DAYDATETIME
#define DAYDATETIME(x) *DayDateTime(x) 

#undef CHANNELSTRING
#define CHANNELSTRING(x) (*x->GetChannelID().ToString()) 

#undef WEEKDAYNAME
#define WEEKDAYNAME(x) (*WeekDayName(x)) 

#undef ADDDIR
#define ADDDIR *AddDirectory 

#undef CONFIGDIR
#define CONFIGDIR (!ConfigDir?cPlugin::ConfigDirectory():ConfigDir)

#define CHNUMWIDTH  (numdigits(Channels.MaxNumber()) + 2)

#define SHORTTEXT(EVENT) \
  (EVENT && EPGSearchConfig.showShortText && !isempty((EVENT)->ShortText()))?" ~ ":"", \
  (EVENT && EPGSearchConfig.showShortText && !isempty((EVENT)->ShortText()))?(EVENT)->ShortText():""

#define ISRADIO(x) ((x)->Vpid()==0||(x)->Vpid()==1||(x)->Vpid()==0x1fff)

extern const char AllowedChars[];

extern char* ConfigDir;

// Helper functions
class cSearchExt;
class cSearchExtCat;
class cEvent;

char* IndentMenuItem(const char*);
void PatchFont(eDvbFont);
bool MatchesSearchMode(const char* test, const char* values, int searchmode, const char* delim, int tolerance);
char* GetExtEPGValue(const cEvent* e, cSearchExtCat* SearchExtCat);
char* GetExtEPGValue(const char* description, const char* catname);
char* GetAuxValue(const char* aux, const char* name);
char* GetAuxValue(const cRecording *recording, const char* name);
char* GetAuxValue(const cTimer* timer, const char* name);
string UpdateAuxValue(string aux, string section, string value);
string UpdateAuxValue(string aux, string section, long num);
void ToLower(char* szText);
char *strreplacei(char *s, const char *s1, const char *s2);
char *strreplaceall(char* source, char ch, char* replacement);
void sleepMSec(long ms);
void sleepSec(long s);
bool SendViaSVDRP(const char* SVDRPcmd);
int SendMsg(const char* Message, bool confirm = false, int seconds = 0);
bool InEditMode(const char* ItemText, const char* ItemName, const char* ItemValue);
cSearchExt* TriggeredFromSearchTimer(const cTimer* timer);
int TriggeredFromSearchTimerID(const cTimer* timer);
bool CopyFile(const char* file_path_from,  const char* file_path_to);
double FuzzyMatch(const char* s1, const char* s2, int maxLength);
bool DescriptionMatches(const char* eDescr, const char* rDescr);
const cEvent* GetEvent(cTimer* timer);
char* GetRawDescription(const char* descr);
void PrepareTimerFile(const cEvent* event, cTimer* timer);
int CompareEventTime(const void *p1, const void *p2);
int CompareEventChannel(const void *p1, const void *p2);
bool EventsMatch(const cEvent* event1, const cEvent* event2, bool compareTitle, bool compareSubtitle, bool compareSummary, unsigned long catvaluesAvoidRepeat);
int ChannelNrFromEvent(const cEvent* pEvent);
void DelTimer(int index);
char* FixSeparators(char* buffer, char sep);
cString DateTime(time_t t);
string NumToString(long l);
int FindIgnoreCase(const string& expr, const string& query);
bool EqualsNoCase(const string& a, const string& b);
string Strip(const string& input);
string ReplaceAll(const string& input, const string& what, const string& with);
string GetAlNum(const string& s);
string EscapeString(const string& S);
string QuoteApostroph(const string& S);
string MD5(const string& input);
bool movefile(const string& from, const string& to);
time_t GetDateTime(time_t day, int start);
void SetAux(cTimer* timer, string aux);

// --- cTimerObj --------------------------------------------------------
class cTimerObj : public cListObject {
public:
    cTimer* timer;
    cTimerObj(cTimer* Timer) : timer(Timer) {}
    ~cTimerObj() { timer = NULL;  } // do not delete anything!
};

// --- cTimerObjList --------------------------------------------------------
class cTimerObjList : public cList<cTimerObj> {
public:
    void DelTimer(cTimer* t)
	{
	    for (cTimerObj* pTObj = First(); pTObj; pTObj = Next(pTObj))
		if (pTObj->timer == t)
		{
		    Del(pTObj);
		    return;
		}
	}
};

#endif
