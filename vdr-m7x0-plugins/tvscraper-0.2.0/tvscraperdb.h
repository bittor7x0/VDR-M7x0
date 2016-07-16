#ifndef __TVSCRAPER_TVSCRAPERDB_H
#define __TVSCRAPER_TVSCRAPERDB_H

using namespace std; 

// --- cTVScraperDB --------------------------------------------------------

class cTVScraperDB {
private:
    sqlite3 *db;
    string dbPathPhys;
    string dbPathMem;
    bool inMem;
    vector<vector<string> > Query(string query);
    vector<vector<string> > QueryEscaped(string query, string where);
    int LoadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave);
    bool CreateTables(void);
public:
    cTVScraperDB(void);
    virtual ~cTVScraperDB(void);
    bool Connect(void);
    void BackupToDisc(void);
    void ClearOutdated(string movieDir);
    bool CheckMovieOutdatedEvents(int movieID);
    bool CheckMovieOutdatedRecordings(int movieID);
    void DeleteMovie(int movieID, string movieDir);
    void InsertSeries(int seriesID, string name, string overview);
    void InsertEventSeries(int eventID, time_t validTill, int seriesID);
    void InsertActor(int seriesID, string name, string role, string thumb);
    void InsertMovie(int movieID, string title, string originalTitle, string overview);
    void InsertEventMovie(int eventID, time_t validTill, int movieID);
    void InsertMovieActor(int movieID, int actorID, string name, string role);
    bool MovieExists(int movieID);
    bool SeriesExists(int seriesID);
    int SearchMovie(string movieTitle);
    int SearchSeries(string seriesTitle);
    void InsertRecording(int recEventID, int seriesID, int movieID);
    bool SetRecordingSeries(int eventID); 
    bool SetRecordingMovie(int eventID);
    void ClearRecordings(void);
    bool CheckScrap(time_t timeStamp, string channelID);
    bool CheckStartScrapping(int minimumDistance);
    int GetSeriesID(int eventID, bool isRecording);
    int GetMovieID(int eventID, bool isRecording);
    vector<vector<string> > GetActorsSeries(int seriesID);
    vector<vector<string> > GetActorsMovie(int movieID);
    string GetDescriptionSeries(int seriesID);
    string GetDescriptionMovie(int movieID);
};

#endif //__TVSCRAPER_TVSCRAPPERDB_H
