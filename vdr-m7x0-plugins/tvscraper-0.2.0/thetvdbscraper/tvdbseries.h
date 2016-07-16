#ifndef __TVSCRAPER_TVDBSERIES_H
#define __TVSCRAPER_TVDBSERIES_H

using namespace std;

// --- cTVDBSeries -------------------------------------------------------------

class cTVDBSeries {
private:
    xmlDoc *doc;
    int seriesID;
    string name;
    string banner;
    string overview;
    string imbdid;
    void SetXMLDoc(string xml);
public:
    cTVDBSeries(string xml);
    virtual ~cTVDBSeries(void);
    void ParseXML(void);
    int ID(void) { return seriesID; };
    const char *Name(void) { return name.c_str(); };
    void StoreDB(cTVScraperDB *db);
    void StoreBanner(string baseUrl, string destDir);
    void Dump();
};


#endif //__TVSCRAPER_TVDBSERIES_H
