#include <locale.h>
#include "worker.h"

using namespace std;

cTVScraperWorker::cTVScraperWorker(cTVScraperDB *db, cOverRides *overrides) : cThread("tvscraper", true) {
    startLoop = true;
    scanVideoDir = false;
    manualScan = false;
    this->db = db;
    this->overrides = overrides;
    moviedbScraper = NULL;
    tvdbScraper = NULL;
    initSleep = 2 * 60 * 1000;
    loopSleep = 5 * 60 * 1000;
    language = "";
}

cTVScraperWorker::~cTVScraperWorker() {
    if (moviedbScraper)
        delete moviedbScraper;
    if (tvdbScraper)
        delete tvdbScraper;
}

void cTVScraperWorker::SetLanguage(void) {
    string loc = setlocale(LC_NAME, NULL);
    size_t index = loc.find_first_of("_");
    string langISO = "";
    if (index > 0) {
        langISO = loc.substr(0, index);
    }
    if (langISO.size() == 2) {
        language = langISO.c_str();
        dsyslog("tvscraper: using language %s", language.c_str());
        return;
    }
    language = "en";
    dsyslog("tvscraper: using fallback language %s", language.c_str());
}

void cTVScraperWorker::Stop(void) {
    waitCondition.Broadcast();    // wakeup the thread
    Cancel(5);                    // wait up to 5 seconds for thread was stopping
    db->BackupToDisc();
}

void cTVScraperWorker::InitVideoDirScan(void) {
    scanVideoDir = true;
    waitCondition.Broadcast();
}

void cTVScraperWorker::InitManualScan(void) {
    manualScan = true;
    waitCondition.Broadcast();
}

void cTVScraperWorker::SetDirectories(void) {
    plgBaseDir = config.GetBaseDir();
    stringstream strSeriesDir;
    strSeriesDir << plgBaseDir << "/series";
    seriesDir = strSeriesDir.str();
    stringstream strMovieDir;
    strMovieDir << plgBaseDir << "/movies";
    movieDir = strMovieDir.str();
    bool ok = false;
    ok = CreateDirectory(plgBaseDir);
    if (ok)
        ok = CreateDirectory(seriesDir);
    if (ok)
        ok = CreateDirectory(movieDir);
    if (!ok) {
        esyslog("tvscraper: ERROR: check %s for write permissions", plgBaseDir.c_str());
        startLoop = false;
    } else {
        dsyslog("tvscraper: using base directory %s", plgBaseDir.c_str());
    }
}

scrapType cTVScraperWorker::GetScrapType(const cEvent *event) {
    scrapType type = scrapNone;
    string title = (event->Title())?event->Title():"";
    type = overrides->Type(title);
    if (type != scrapNone)
        return type;
    int duration = event->Duration() / 60;
    if ((duration > 9) && (duration <= 75)) {
        type = scrapSeries;
    } else if (duration > 75) {
        type = scrapMovie;
    }
    return type;
}

bool cTVScraperWorker::ConnectScrapers(void) {
    if (!moviedbScraper) {
        moviedbScraper = new cMovieDBScraper(movieDir, db, language, overrides);
        if (!moviedbScraper->Connect()) {
            esyslog("tvscraper: ERROR, connection to TheMovieDB failed");
            delete moviedbScraper;
            moviedbScraper = NULL;
            return false;
        }
    }
    if (!tvdbScraper) {
        tvdbScraper = new cTVDBScraper(seriesDir, db, language, overrides);
        if (!tvdbScraper->Connect()) {
            esyslog("tvscraper: ERROR, connection to TheTVDB failed");
            delete tvdbScraper;
            tvdbScraper = NULL;
            return false;
        }
    }
    return true;
}

void cTVScraperWorker::DisconnectScrapers(void) {
    if (moviedbScraper) {
        delete moviedbScraper;
        moviedbScraper = NULL;
    }
    if (tvdbScraper) {
        delete tvdbScraper;
        tvdbScraper = NULL;
    }
}

void cTVScraperWorker::ScrapEPG(void) {
    vector<string> channels = config.GetChannels();
    int numChannels = channels.size();
    for (int i=0; i<numChannels; i++) {
        string channelID = channels[i];
        const cChannel *channel = Channels.GetByChannelID(tChannelID::FromString(channelID.c_str()));
        if (!channel)
            continue;
        dsyslog("tvscraper: scraping Channel %s %s", channel->Name(), channelID.c_str());
        cSchedulesLock schedulesLock;
        const cSchedules *schedules = cSchedules::Schedules(schedulesLock);
        const cSchedule *Schedule = schedules->GetSchedule(channel);
        if (Schedule) {
            const cEvent *event = NULL;
            for (event = Schedule->Events()->First(); event; event =  Schedule->Events()->Next(event)) {
                if (!Running())
                    return;
                scrapType type = GetScrapType(event);
                if (type != scrapNone) {
                    if (!db->CheckScrap(event->StartTime(), channelID))
                        continue;
                    if (type == scrapSeries) {
                        tvdbScraper->Scrap(event);
                    } else if (type == scrapMovie) {
                        moviedbScraper->Scrap(event);
                    }
                    waitCondition.TimedWait(mutex, 100);
                }
            }
        }
    }
}

void cTVScraperWorker::ScrapRecordings(void) {
    db->ClearRecordings();
    for (cRecording *rec = Recordings.First(); rec; rec = Recordings.Next(rec)) {
        if (overrides->IgnorePath(rec->FileName()))
            continue;
        const cRecordingInfo *recInfo = rec->Info();
        const cEvent *recEvent = recInfo->GetEvent();
        if (recEvent) {
            tEventID recEventID = recEvent->EventID();
            scrapType type = GetScrapType(recEvent);
            if (type == scrapSeries) {
                int seriesID = db->SearchSeries(recEvent->Title());
                if (seriesID) {
                    db->InsertRecording((int)recEventID, seriesID, 0);
                } else {
                    tvdbScraper->Scrap(recEvent, (int)recEventID);
                }
            } else if (type == scrapMovie) {
                int movieID = db->SearchMovie(recEvent->Title());
                if (movieID) {
                    db->InsertRecording((int)recEventID, 0, movieID);
                } else {
                    moviedbScraper->Scrap(recEvent, (int)recEventID);
                }
            }
        }
    }
}

void cTVScraperWorker::CheckRunningTimers(void) {
    for (cTimer *timer = Timers.First(); timer; timer = Timers.Next(timer)) {
        if (timer->Recording()) {
            const cEvent *event = timer->Event();
            if (!event)
                continue;
            scrapType type = GetScrapType(event);
            if (type == scrapSeries) {
                if (!db->SetRecordingSeries((int)event->EventID())) {
                    if (ConnectScrapers()) {
                        tvdbScraper->Scrap(event, (int)event->EventID());
                    }
                }
            } else if (type == scrapMovie) {
                if (!db->SetRecordingMovie((int)event->EventID())) {
                    if (ConnectScrapers()) {
                        moviedbScraper->Scrap(event, (int)event->EventID());
                    }
                }
            }
        }
    }
    DisconnectScrapers();
}
 
bool cTVScraperWorker::StartScrapping(void) {
    if (manualScan) {
        manualScan = false;
        return true;
    }
    //wait at least one day from last scrapping to scrap again
    int minTime = 24 * 60 * 60;
    return db->CheckStartScrapping(minTime);
}


void cTVScraperWorker::Action(void) {
    if (!startLoop)
        return;

    mutex.Lock();    
    dsyslog("tvscraper: waiting %d minutes to start main loop", initSleep / 1000 / 60);
    waitCondition.TimedWait(mutex, initSleep);

    while (Running()) {
        if (scanVideoDir) {
            scanVideoDir = false;
            dsyslog("tvscraper: scanning video dir");
            if (ConnectScrapers()) {
                ScrapRecordings();
            }
            DisconnectScrapers();
            continue;
        }
        CheckRunningTimers();
        if (StartScrapping()) {
            dsyslog("tvscraper: start scraping epg");
            db->ClearOutdated(movieDir);
            if (ConnectScrapers()) {
                ScrapEPG();
            }
            DisconnectScrapers();
            db->BackupToDisc();
            dsyslog("tvscraper: epg scraping done");
        }
        waitCondition.TimedWait(mutex, loopSleep);    
    }
}
