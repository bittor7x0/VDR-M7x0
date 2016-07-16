#ifndef __TVSCRAPER_MOVIEDBMOVIE_H
#define __TVSCRAPER_MOVIEDBMOVIE_H

using namespace std;

struct searchResult {
    int id;
    int distance;
};

// --- cMovieDbMovie -------------------------------------------------------------

class cMovieDbMovie {
private:
    int id;
    string json;
    string title;
    string originalTitle;
    string overview;
    string backdropPath;
    string posterPath;
    vector<searchResult> resultSet;
    int FindBestResult(void);
public:
    cMovieDbMovie(string json);
    virtual ~cMovieDbMovie(void);
    int ParseJSONForMovieId(string movieSearchString);
    void ParseJSON(void);
    void SetID(int movieID) { id = movieID; };
    int ID(void) { return id; };
    void StoreDB(cTVScraperDB *db);
    void StoreMedia(string posterBaseUrl, string backdropBaseUrl, string destDir);
    void Dump();
};


#endif //__TVSCRAPER_TVDBSERIES_H
