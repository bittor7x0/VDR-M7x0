#include "setup.h"

using namespace std;

/* cTVScraperSetup */

cTVScraperSetup::cTVScraperSetup(cTVScraperWorker *workerThread) {
    worker = workerThread;
    int numChannels = Channels.Count();
    for (int i=0; i<numChannels; i++) {
        int akt = 0;
        if (config.ChannelActive(i+1))
            akt = 1;
        channelsScrap.push_back(akt);
    }
    Setup();
}

cTVScraperSetup::~cTVScraperSetup() {
}


void cTVScraperSetup::Setup(void) {
    int currentItem = Current();
    Clear();
    Add(new cOsdItem(tr("Configure channels to be scraped")));
    Add(new cOsdItem(tr("Trigger scraping Video Directory")));
    Add(new cOsdItem(tr("Trigger EPG scraping")));
    Add(new cMenuEditBoolItem(tr("Enable Debug Logging"), &config.enableDebug));
    
    SetCurrent(Get(currentItem));
    Display();
}

eOSState cTVScraperSetup::ProcessKey(eKeys Key) {
    bool hadSubMenu = HasSubMenu();
    if (hadSubMenu && Key == kOk)
        Store();
    eOSState state = cMenuSetupPage::ProcessKey(Key);
    if (!hadSubMenu && (Key == kOk)) {
        const char* ItemText = Get(Current())->Text();
        if (strcmp(ItemText, tr("Configure channels to be scraped")) == 0)
            state = AddSubMenu(new cTVScraperChannelSetup(&channelsScrap));
        else if (strcmp(ItemText, tr("Trigger scraping Video Directory")) == 0) {
            Skins.Message(mtInfo, "Scraping Video Directory started");
            worker->InitVideoDirScan();
            state = osContinue;
        } else if (strcmp(ItemText, tr("Trigger EPG scraping")) == 0) {
            Skins.Message(mtInfo, "EPG Scraping started");
            worker->InitManualScan();
            state = osContinue;
        }
    }   
    return state;
}

void cTVScraperSetup::Store(void) {
    config.ClearChannels();
    stringstream channelsToScrap;
    int numChannels = channelsScrap.size();
    for (int i=0; i<numChannels; i++) {
        if (channelsScrap[i] == 1) {
            cChannel *channel = Channels.GetByNumber(i+1);
            if (channel) {
                string channelID = *(channel->GetChannelID().ToString());
                channelsToScrap << channelID << ";";
                config.AddChannel(channelID);
            }
        }
    }
    SetupStore("ScrapChannels", channelsToScrap.str().c_str());
    SetupStore("enableDebug", config.enableDebug);
}


/* cTVScraperChannelSetup */

cTVScraperChannelSetup ::cTVScraperChannelSetup (vector<int> *channelsScrap) : cOsdMenu(tr("Configure channels to be scraped"), 30) {
    this->channelsScrap = channelsScrap;
    SetMenuCategory(mcSetupPlugins);
    Setup();
}

cTVScraperChannelSetup ::~cTVScraperChannelSetup () {
}


void cTVScraperChannelSetup ::Setup(void) {
    int currentItem = Current();
    Clear();
    int i=0;
    for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
        if (!channel->GroupSep()) {
            Add(new cMenuEditBoolItem(channel->Name(), &channelsScrap->at(i), tr("don't scrap"), tr("scrap")));
            i++;
        }
    }
    SetCurrent(Get(currentItem));
    Display();
}

eOSState cTVScraperChannelSetup ::ProcessKey(eKeys Key) {
    eOSState state = cOsdMenu::ProcessKey(Key);
    switch (Key) {
        case kOk:
            return osBack;
        default:
            break;
    }
    return state;
}