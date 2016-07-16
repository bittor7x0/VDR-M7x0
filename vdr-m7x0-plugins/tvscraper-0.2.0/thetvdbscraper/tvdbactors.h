#ifndef __TVSCRAPER_TVDBACTORS_H
#define __TVSCRAPER_TVDBACTORS_H

using namespace std; 

// --- cTVDBActor -------------------------------------------------------------
class cTVDBActor {
public:
    cTVDBActor(void) {
        path = "";
        name = "";
        role = "";
    };
    string path;
    string name;
    string role;
};

// --- cTVDBActors --------------------------------------------------------

class cTVDBActors {
private:
    xmlDoc *doc;
    string language;
    vector<cTVDBActor*> actors;
    void SetXMLDoc(string xml);
    void ReadEntry(xmlNode *node);
public:
    cTVDBActors(string xml, string language);
    virtual ~cTVDBActors(void);
    void ParseXML(void);
    void StoreDB(cTVScraperDB *db, int series_id);
    void Store(string baseUrl, string destDir);
    void Dump(bool verbose);
};

#endif //__TVSCRAPER_TVDBACTORS_H
