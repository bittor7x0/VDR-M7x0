#ifndef __TVSCRAPER_SETUP_H
#define __TVSCRAPER_SETUP_H

using namespace std;

class cTVScraperSetup : public cMenuSetupPage {
    public:
        cTVScraperSetup(cTVScraperWorker *workerThread);
        virtual ~cTVScraperSetup();      
    private:
        vector<int> channelsScrap;
        cTVScraperWorker *worker;
        void Setup(void);
    protected:
        virtual eOSState ProcessKey(eKeys Key);
        virtual void Store(void);
};

class cTVScraperChannelSetup : public cOsdMenu {
    public:
        cTVScraperChannelSetup(vector<int> *channelsScrap);
        virtual ~cTVScraperChannelSetup();       
    private:
        vector<int> *channelsScrap;
        void Setup(void);
    protected:
        virtual eOSState ProcessKey(eKeys Key);
};

#endif //__TVSCRAPER_SETUP_H