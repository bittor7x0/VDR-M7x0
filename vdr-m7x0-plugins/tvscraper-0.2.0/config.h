#ifndef __TVSCRAPER_CONFIG_H
#define __TVSCRAPER_CONFIG_H

using namespace std;

enum scrapType {
    scrapSeries,
    scrapMovie,
    scrapNone
};

class cTVScraperConfig {
    private:
        string baseDir;
        vector<string> channels; 
    public:
        cTVScraperConfig();
        ~cTVScraperConfig();
        int enableDebug;
        void SetBaseDir(string dir) { baseDir = dir; };
        string GetBaseDir(void) { return baseDir; };
        void ClearChannels(void);
        void AddChannel(string channelID);
        bool ChannelActive(int channelNum);
        bool SetupParse(const char *Name, const char *Value);
        vector<string> GetChannels(void) { return channels; };
        void PrintChannels(void);
};

#endif //__TVSCRAPER_CONFIG_H