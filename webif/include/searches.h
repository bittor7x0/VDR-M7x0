/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_SEARCHES_H
#define WI_SEARCHES_H

#include "channels.h"
#include "conf.h"

typedef enum eventCompareFlags_e {
	ECF_COMPARE_TITLE = 0x0001,
	ECF_COMPARE_SUBTITLE = 0x0002,
	ECF_COMPARE_DESCRIPTION = 0x0004,
} eventCompareFlags_t;

enum searchFlags_e { 
	SFL_NONE = 0x0000,
	SFL_USE_TIME = 0x0001,
	SFL_USE_CASE = 0x0002,
	SFL_USE_DURATION = 0x0004,
	SFL_USE_AS_SEARCH_TIMER = 0x0008,
	SFL_USE_WDAY = 0x0010,
	SFL_USE_EPISODE = 0x0020,
	SFL_USE_VPS = 0x0040,
	SFL_USE_EXT_EPG_INFO = 0x0080,
	SFL_AVOID_REPEATS = 0x0100,
	SFL_USE_IN_FAVORITES = 0x0200,
	SFL_IGNORE_MISSING_EPG_CATS = 0x0400,
	SFL_UNMUTE_SOUND_ON_SWITCH = 0x0800,
};

enum searchFields_e {
	SFI_ID=1,
	SFI_SEARCH_STR = 2,
	SFI_SEARCH = 4,
	SFI_CHANNEL_GROUP = 8,
	SFI_DIRECTORY = 16,
	SFI_EXT_EPG_VALUES = 32,
	SFI_BLACKLIST_IDS = 64,
};

typedef enum searchMode_e {
	SEARCH_MODE_SUBSTRING=0,
	SEARCH_MODE_SUBSTRING_AND=1,
	SEARCH_MODE_SUBSTRING_OR=2, 
	SEARCH_MODE_EQUAL=3, 
	SEARCH_MODE_REGEX=4,
	SEARCH_MODE_FUZZY=5,
	// numerical comparison:
	SEARCH_MODE_NUM_LT=10,  //less
	SEARCH_MODE_NUM_LET=11, //less equal
	SEARCH_MODE_NUM_GT=12,  // greater
	SEARCH_MODE_NUM_GET=13, // greater or equal
	SEARCH_MODE_NUM_EQ=14,  // equal
	SEARCH_MODE_NUM_NEQ=15, // not equal
} searchMode_t;

typedef enum blacklistMode_e {
	BLACKLIST_SELECTION,
} blacklistMode_t;

typedef struct channelGroup_s {
	int hostId;
	char *name;
	int length;
	channel_t **entry;
} channelGroup_t;

typedef struct channelGroupList_s {
	int length;
	channelGroup_t *entry;
} channelGroupList_t;

typedef struct search_s {
	int my;
	int id;
	char *searchStr;
	int hostId;
	char *search;
	searchMode_t searchMode;
	uint flags;
	uint compareFlags; //eventFields_e
 	int startTime;
	int stopTime;
	short useChannel;
	int channelMin;
	int channelMax;
	char *channelGroup;
	int minDuration;
	int maxDuration;
	int wday;
	char *directory;
	int priority;
	int lifetime;
	int marginStart;
	int marginStop;
	int action;
	char *extEpgValues;
	int allowedRepeats;
	uint repeatsCompareFlags; //eventFields_e
	int catvaluesAvoidRepeat;
	int repeatsWithinDays;
	int delAfterDays;
	int recordingsKeep;
	int switchMinsBefore;
	int pauseOnNrRecordings;
	blacklistMode_t blacklistMode;
	char *blacklistIds;
	int fuzzyTolerance;
	int menuTemplate;
	int delMode;
	int delAfterCountRecs;
	int delAfterDaysOfFirstRec;
	time_t useAsSearchTimerFrom;
	time_t useAsSearchTimerTil;
} search_t;

typedef struct searchList_s {
	int length;
	search_t *entry;
} searchList_t;

typedef struct searchCat_s {
	int id;
	int hostId;
	char* name;
	char* menuname;
	searchMode_t searchmode; 
	char** values;
	int nvalues;
	//char *currentvalue;
} searchCat_t;

typedef struct searchCatList_s {
	int length;
	searchCat_t *entry;
} searchCatList_t;


void initChannelGroupList(channelGroupList_t *const groups);
void freeChannelGroupList(channelGroupList_t *const groups);
void getChannelGroupList(hostConf_t *host, channelGroupList_t *const groups, channelList_t const *const channels);
void printChannelGroupListSelect(wcontext_t *wctx,const char * name,const channelGroupList_t *const groups,const char *groupName);

void initSearch(search_t *const entry);
bool initSearchFromEvent(wcontext_t *wctx, search_t *const search, hostConf_t *host, const int channelNum, const int eventId);
bool initSearchFromArgs(search_t *const search, vars_t *args, channelList_t *channels,searchCatList_t * const cats, wcontext_t *wctx);
void freeSearch(search_t *const entry);
void initSearchList(searchList_t *const searches);
void freeSearchList(searchList_t *const searches);
void getSearchList(searchList_t *const searchs, channelList_t const *const channels, const sortField_t sortField, const sortDirection_t sortDirection);
bool parseSearch(const char *line, search_t *const search, channelList_t const *const channels);
bool addSearch(wcontext_t *wctx, hostConf_t *host, const char *newSearchStr);
bool editSearch(wcontext_t *wctx, hostConf_t *host, int id, const char *oldSearchStr, const char *newSearchStr);
bool deleteSearch(wcontext_t *wctx, hostConf_t *host, int id, const char *oldSearchStr,bool andTimers);
bool updateSearches(wcontext_t *wctx, hostConf_t *host);
char *makeSearchStr(search_t *const search,const channelList_t *channels);
void printSearchForm(wcontext_t *wctx, search_t *const search, channelList_t const *const channels,searchCatList_t const *const cats,const char *cssLevel);

void initSearchCat(searchCat_t *const cat);
void freeSearchCat(searchCat_t *const cat);
void initSearchCatList(searchCatList_t *const cats);
void freeSearchCatList(searchCatList_t *const cats);
void getSearchCatList(searchCatList_t *const cats, hostConf_t *host);
bool printSearchCatListFilter(wcontext_t *wctx,const search_t *search,const searchCatList_t * const cats,const char *fieldsetId );
void printSearchesPage(wcontext_t *wctx);

#endif
