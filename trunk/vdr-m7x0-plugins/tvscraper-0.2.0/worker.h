#ifndef __TVSCRAPER_WORKER_H
#define __TVSCRAPER_WORKER_H

// --- cTVScraperWorker -------------------------------------------------------------

class cTVScraperWorker : public cThread {
private:
    bool startLoop;
    bool scanVideoDir;
    bool manualScan;
    string language;
    string plgBaseDir;
    string plgConfDir;
    string seriesDir;
    string movieDir;
    cOverRides *overrides;
    int initSleep;
    int loopSleep;
    cCondVar waitCondition;
    cMutex mutex;
    cTVScraperDB *db;
    cMovieDBScraper *moviedbScraper;
    cTVDBScraper *tvdbScraper;
    scrapType GetScrapType(const cEvent *event);
    bool ConnectScrapers(void);
    void DisconnectScrapers(void);
    void CheckRunningTimers(void);
    void ScrapEPG(void);
    void ScrapRecordings(void);
    bool StartScrapping(void);
public:
    cTVScraperWorker(cTVScraperDB *db, cOverRides *overrides);
    virtual ~cTVScraperWorker(void);
    void SetLanguage(void);
    void SetDirectories(void);
    void InitVideoDirScan(void);
    void InitManualScan(void);
    virtual void Action(void);
    void Stop(void);
};


#endif //__TVSCRAPER_WORKER_H
