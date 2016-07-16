#ifndef __TVSCRAPER_TVDBMIRRORS_H
#define __TVSCRAPER_TVDBMIRRORS_H

using namespace std;

// --- cTVDBMirrors -------------------------------------------------------------

class cTVDBMirrors {
private:
    xmlDoc *doc;
    vector<string> xmlmirrors;
    vector<string> bannermirrors;
    vector<string> zipmirrors;
    void SetXMLDoc(string xml);
    bool ReadEntry(xmlNode *node);
    bool CreateMirror(string path, int typemask);
public:
    cTVDBMirrors(string xml);
    virtual ~cTVDBMirrors(void);
    bool ParseXML(void);
    string GetMirrorXML(void);
    string GetMirrorBanner(void);
    string GetMirrorZip(void);
};


#endif //__TVSCRAPER_TVDBMIRRORS_H
