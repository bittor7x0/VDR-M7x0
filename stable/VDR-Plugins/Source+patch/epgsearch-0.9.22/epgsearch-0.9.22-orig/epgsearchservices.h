#ifndef _EPGSEARCH_SERVICES_INC_
#define _EPGSEARCH_SERVICES_INC_

#include "services.h"
#include <vdr/tools.h>
#include "epgsearchext.h"

class cEpgsearchServiceHandler: public cServiceHandler
{
    virtual std::list<std::string> TranslateResults(cSearchResults* pCompleteSearchResults);
 public:
    virtual std::list<std::string> SearchTimerList();
    virtual int AddSearchTimer(const std::string&);
    virtual bool ModSearchTimer(const std::string&);
    virtual bool DelSearchTimer(int);
    virtual std::list<std::string> QuerySearchTimer(int);
    virtual std::list<std::string> QuerySearch(std::string);
    virtual std::list<std::string> ExtEPGInfoList();
    virtual std::list<std::string> ChanGrpList();
    virtual std::list<std::string> BlackList();
    virtual std::set<std::string> DirectoryList();
    virtual std::string ReadSetupValue(const std::string& entry);
    virtual bool WriteSetupValue(const std::string& entry, const std::string& value);
};

#endif
