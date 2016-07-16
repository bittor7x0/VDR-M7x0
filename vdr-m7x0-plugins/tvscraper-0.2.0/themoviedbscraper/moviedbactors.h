#ifndef __TVSCRAPER_MOVIEDBACTORS_H
#define __TVSCRAPER_MOVIEDBACTORS_H

using namespace std;

// --- cMovieDBActor -------------------------------------------------------------
class cMovieDBActor {
public:
    cMovieDBActor(void) {
        id = 0;
        path = "";
        name = "";
        role = "";
    };
    int id;
    string path;
    string name;
    string role;
};

// --- cMovieDBActors -------------------------------------------------------------

class cMovieDbActors {
private:
    string json;
    vector<cMovieDBActor*> actors;
public:
    cMovieDbActors(string json);
    virtual ~cMovieDbActors(void);
    void ParseJSON(void);
    void StoreDB(cTVScraperDB *db, int movieID);
    void Store(string baseUrl, string destDir);
    void Dump(void);
};


#endif //__TVSCRAPER_MOVIEDBACTORS_H
