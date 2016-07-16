using namespace std;

// --- cImageServer --------------------------------------------------------

class cImageServer {
private:
    cTVScraperDB *db;
    cOverRides *overrides;
public:
    cImageServer(cTVScraperDB *db, cOverRides *overrides);
    virtual ~cImageServer(void);
    scrapType GetScrapType(const cEvent *event);
    int GetID(int eventID, scrapType type, bool isRecording);
    cTvMedia GetPosterOrBanner(int id, scrapType type);
    cTvMedia GetPoster(int id, scrapType type);
    cTvMedia GetBanner(int id);
    vector<cTvMedia> GetPosters(int id, scrapType type);
    vector<cTvMedia> GetSeriesFanarts(int id);
    cTvMedia GetMovieFanart(int id);
    vector<cActor> GetActors(int id, scrapType type);
    string GetDescription(int id, scrapType type);
};
