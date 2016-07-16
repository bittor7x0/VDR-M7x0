#ifndef __TVSCRAPER_TVDBMEDIA_H
#define __TVSCRAPER_TVDBMEDIA_H

using namespace std; 

enum mediaType {
    mediaUnknown,
    mediaPoster,
    mediaFanart,
    mediaSeason,
    mediaBanner,
};

// --- cTVDBMedia -------------------------------------------------------------
class cTVDBMedia {
public:
    cTVDBMedia(void) {
        path = "";
        language = "";
        season = 0;
    };
    mediaType type;
    string path;
    string language;
    int season;
};

// --- cTVDBSeriesMedia --------------------------------------------------------

class cTVDBSeriesMedia {
private:
    xmlDoc *doc;
    string language;
    vector<cTVDBMedia*> medias;
    void SetXMLDoc(string xml);
    void ReadEntry(xmlNode *node);
public:
    cTVDBSeriesMedia(string xml, string language);
    virtual ~cTVDBSeriesMedia(void);
    void ParseXML(void);
    void Store(string baseUrl, string destDir);
    void Dump(bool verbose);
};

#endif //__TVSCRAPER_TVDBMEDIA_H
