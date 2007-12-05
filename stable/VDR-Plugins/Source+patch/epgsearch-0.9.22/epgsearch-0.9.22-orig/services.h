#ifndef EPGSEARCHSERVICES_INC
#define EPGSEARCHSERVICES_INC

#include <string>
#include <list>
#include <memory>
#include <set>
#include <vdr/osdbase.h>

// Data structure for service "Epgsearch-search-v1.0"
struct Epgsearch_search_v1_0
{
// in
      char* query;               // search term
      int mode;                  // search mode (0=phrase, 1=and, 2=or, 3=regular expression)
      int channelNr;             // channel number to search in (0=any)
      bool useTitle;             // search in title
      bool useSubTitle;          // search in subtitle
      bool useDescription;       // search in description
// out
      cOsdMenu* pResultMenu;   // pointer to the menu of results
};

// Data structure for service "Epgsearch-exttimeredit-v1.0"
struct Epgsearch_exttimeredit_v1_0
{
// in
      cTimer* timer;             // pointer to the timer to edit
      bool bNew;                 // flag that indicates, if this is a new timer or an existing one
      const cEvent* event;             // pointer to the event corresponding to this timer (may be NULL)
// out
      cOsdMenu* pTimerMenu;   // pointer to the menu of results
};

// Data structure for service "Epgsearch-updatesearchtimers-v1.0"
struct Epgsearch_updatesearchtimers_v1_0
{
// in
      bool showMessage;           // inform via osd when finished?
};

// Data structure for service "Epgsearch-osdmessage-v1.0"
struct Epgsearch_osdmessage_v1_0
{
// in
      char* message;             // the message to display
      eMessageType type;
};

// Data structure for service "EpgsearchMenu-v1.0"
struct EpgSearchMenu_v1_0
{
// in
// out
      cOsdMenu* Menu;   // pointer to the menu
};

// Data structure for service "Epgsearch-lastconflictinfo-v1.0"
struct Epgsearch_lastconflictinfo_v1_0
{
// in
// out
      time_t nextConflict;       // next conflict date, 0 if none
      int relevantConflicts;     // number of relevant conflicts
      int totalConflicts;        // total number of conflicts
};

// Data structure for service "Epgsearch-searchresults-v1.0"
struct Epgsearch_searchresults_v1_0
{
// in
      char* query;               // search term
      int mode;                  // search mode (0=phrase, 1=and, 2=or, 3=regular expression)
      int channelNr;             // channel number to search in (0=any)
      bool useTitle;             // search in title
      bool useSubTitle;          // search in subtitle
      bool useDescription;       // search in description
// out

      class cServiceSearchResult : public cListObject 
      {
        public:
         const cEvent* event;
         cServiceSearchResult(const cEvent* Event) : event(Event) {}
      };

      cList<cServiceSearchResult>* pResultList;   // pointer to the results
};

// Data structure for service "Epgsearch-switchtimer-v1.0"
struct Epgsearch_switchtimer_v1_0
{
// in
      const cEvent* event;
      int mode;                  // mode (0=query existance, 1=add/modify, 2=delete)
// in/out
      int switchMinsBefore;
      int announceOnly;
// out   		
      bool success;              // result
};

// Data structures for service "Epgsearch-services-v1.0"
class cServiceHandler
{
  public:
   virtual std::list<std::string> SearchTimerList() = 0;
   // returns a list of search timer entries in the same format as used in epgsearch.conf 
   virtual int AddSearchTimer(const std::string&) = 0;
   // adds a new search timer and returns its ID (-1 on error)
   virtual bool ModSearchTimer(const std::string&) = 0;
   // edits an existing search timer and returns success
   virtual bool DelSearchTimer(int) = 0;
   // deletes search timer with given ID and returns success
   virtual std::list<std::string> QuerySearchTimer(int) = 0;
   // returns the search result of the searchtimer with given ID in the same format as used in SVDRP command 'QRYS' (->MANUAL)        
   virtual std::list<std::string> QuerySearch(std::string) = 0;
   // returns the search result of the searchtimer with given settings in the same format as used in SVDRP command 'QRYS' (->MANUAL)        
   virtual std::list<std::string> ExtEPGInfoList() = 0;
   // returns a list of extended EPG categories in the same format as used in epgsearchcats.conf 
   virtual std::list<std::string> ChanGrpList() = 0;
   // returns a list of channel groups maintained by epgsearch
   virtual std::list<std::string> BlackList() = 0;
   // returns a list of blacklists in the same format as used in epgsearchblacklists.conf
   virtual std::set<std::string> DirectoryList() = 0;
   // List of all recording directories used in recordings, timers, search timers or in epgsearchdirs.conf
   virtual ~cServiceHandler() {}
   // Read a setup value
   virtual std::string ReadSetupValue(const std::string& entry) = 0;
   // Write a setup value
   virtual bool WriteSetupValue(const std::string& entry, const std::string& value) = 0;
};

struct Epgsearch_services_v1_0
{
// in/out
      std::auto_ptr<cServiceHandler> handler;
};

#endif
