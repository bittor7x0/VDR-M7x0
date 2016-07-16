#ifndef __TVSCRAPER_TVDBSCRAPER_H
#define __TVSCRAPER_TVDBSCRAPER_H

using namespace std;

// --- cTVDBScraper -------------------------------------------------------------

class cTVDBScraper {
private:
    string apiKey;
    string baseURL;
    string baseDir;
    string language;
    cTVScraperDB *db;
    cOverRides *overrides;
    map<string, int> cache;
    cTVDBMirrors *mirrors;
    cTVDBSeries *ReadSeries(string seriesName);
    cTVDBSeriesMedia *ReadSeriesMedia(int seriesID);
    cTVDBActors *ReadSeriesActors(int seriesID);
    void StoreMedia(cTVDBSeries *series, cTVDBSeriesMedia *media, cTVDBActors *actors);
public:
    cTVDBScraper(string baseDir, cTVScraperDB *db, string language, cOverRides *overrides);
    virtual ~cTVDBScraper(void);
    bool Connect(void);
    void Scrap(const cEvent *event, int recordingID = 0);
};


#endif //__TVSCRAPER_TVDBSCRAPER_H
