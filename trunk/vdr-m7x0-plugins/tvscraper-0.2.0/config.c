#include "config.h"

using namespace std;

cTVScraperConfig::cTVScraperConfig() {
    enableDebug = 0;
}

cTVScraperConfig::~cTVScraperConfig() {
}

void cTVScraperConfig::ClearChannels(void) {
    channels.clear();
}

void cTVScraperConfig::AddChannel(string channelID) {
    channels.push_back(channelID);   
}

bool cTVScraperConfig::ChannelActive(int channelNum) {
    cChannel *channel = Channels.GetByNumber(channelNum);
    if (channel) {
        string channelID = "";
        channelID = *(channel->GetChannelID().ToString());
        int numChannels = channels.size();
        for (int i=0; i<numChannels; i++) {
            if (!channels[i].compare(channelID)) {
                return true;
            }
        }
    }
    return false;
}

bool cTVScraperConfig::SetupParse(const char *Name, const char *Value) {
    if (strcmp(Name, "ScrapChannels") == 0) {
        splitstring s(Value);
        vector<string> flds = s.split(';');
        int numChannels = flds.size();
        for (int i=0; i<numChannels; i++) {
            channels.push_back(flds[i]);
        }
        return true;
    } else if (strcmp(Name, "enableDebug") == 0) {
        enableDebug = atoi(Value);
        return true;
    }
    return false;
}

void cTVScraperConfig::PrintChannels(void) {
    int numChannels = channels.size();
    esyslog("tvscraper: %d channel to be scrapped", numChannels);
    for (int i=0; i<numChannels; i++) {
        esyslog("tvscraper: channel to be scrapped: %s", channels[i].c_str());
    }
}
