#ifndef __TVSCRAPER_OVERRIDES_H
#define __TVSCRAPER_OVERRIDES_H

using namespace std;

// --- cOverRides --------------------------------------------------------

class cOverRides {
private:
    vector<string> ignores;
    map<string,scrapType> searchTypes;
    map<string,string> substitutes;
    vector<string> ignorePath;
    void ReadConfigLine(string line);
public:
    cOverRides(void);
    virtual ~cOverRides(void);
    void ReadConfig(string confDir);
    bool Ignore(string title);
    string Substitute(string title);
    scrapType Type(string title);
    bool IgnorePath(string path);
    void Dump(void);
}; 
#endif //__TVSCRAPER_OVERRIDES_H