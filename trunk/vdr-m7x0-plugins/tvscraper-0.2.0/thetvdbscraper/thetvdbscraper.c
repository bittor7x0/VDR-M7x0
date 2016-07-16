#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "thetvdbscraper.h"

using namespace std;

cTVDBScraper::cTVDBScraper(string baseDir, cTVScraperDB *db, string language, cOverRides *overrides) {
    apiKey = "E9DBB94CA50832ED";
    baseURL = "thetvdb.com";
    this->baseDir = baseDir;
    this->language = language;
    this->db = db;
    this->overrides = overrides;
    mirrors = NULL;
}

cTVDBScraper::~cTVDBScraper() {
    if (mirrors)
        delete mirrors;
}

void cTVDBScraper::Scrap(const cEvent *event, int recordingID) {
    string seriesName = (event->Title())?event->Title():"";
    if (overrides->Ignore(seriesName)) {
        return;
    }
    seriesName = overrides->Substitute(seriesName);
    if (config.enableDebug)
        esyslog("tvscraper: scraping series \"%s\"", seriesName.c_str());
    int eventID = (int)event->EventID();

    map<string,int>::iterator cacheHit = cache.find(seriesName);
    if (cacheHit != cache.end()) {
        if (config.enableDebug)
            esyslog("tvscraper: found in cache %s => %d", ((string)cacheHit->first).c_str(), (int)cacheHit->second);
        int cachedSeriesID = (int)cacheHit->second;
        if (cachedSeriesID > 0) {
            if (!recordingID) {
                time_t validTill = event->EndTime();
                db->InsertEventSeries(eventID, validTill, cachedSeriesID);
            } else  {
                db->InsertRecording(recordingID, cachedSeriesID, 0);
            }
        }
        return;
    }
    cTVDBSeries *series = ReadSeries(seriesName);
    cTVDBSeriesMedia *media = NULL;
    cTVDBActors *actors = NULL;
    if (!series)
        return;
    cache.insert(pair<string, int>(seriesName, series->ID()));
    if (series->ID() < 1) {
        if (config.enableDebug)
            esyslog("tvscraper: nothing found for \"%s\"", seriesName.c_str());
        return;
    }
    if (!db->SeriesExists(series->ID())) {
        series->StoreDB(db);
        media = ReadSeriesMedia(series->ID());
        actors = ReadSeriesActors(series->ID());
        if (actors)
            actors->StoreDB(db, series->ID());
        StoreMedia(series, media, actors);
    }
    if (!recordingID) {
        time_t validTill = event->EndTime();
        db->InsertEventSeries(eventID, validTill, series->ID());
    } else  {
        db->InsertRecording(recordingID, series->ID(), 0);
    }
    if (config.enableDebug)
        esyslog("tvscraper: \"%s\" successfully scraped, id %d", seriesName.c_str(), series->ID());
    delete series;
    if (media)
        delete media;
    if (actors)
        delete actors;
}


bool cTVDBScraper::Connect(void) {
    stringstream url;
    url << baseURL << "/api/" << apiKey << "/mirrors.xml";
    string mirrorsXML;
    if (CurlGetUrl(url.str().c_str(), &mirrorsXML)) {
        mirrors = new cTVDBMirrors(mirrorsXML);
        return mirrors->ParseXML();
    }
    return false;
}

cTVDBSeries *cTVDBScraper::ReadSeries(string seriesName) {
    stringstream url;
    url << mirrors->GetMirrorXML() << "/api/GetSeries.php?seriesname=" << CurlEscape(seriesName.c_str()) << "&language=" << language.c_str();
    string seriesXML;
    cTVDBSeries *series = NULL;
    if (config.enableDebug)
        esyslog("tvscraper: calling %s", url.str().c_str());
    if (CurlGetUrl(url.str().c_str(), &seriesXML)) {
        series = new cTVDBSeries(seriesXML);
        series->ParseXML();
    }
    return series;
}

cTVDBSeriesMedia *cTVDBScraper::ReadSeriesMedia(int seriesID) {
    stringstream url;
    url << mirrors->GetMirrorXML() << "/api/" << apiKey << "/series/" << seriesID << "/banners.xml";
    string bannersXML;
    cTVDBSeriesMedia *media = NULL;
    if (CurlGetUrl(url.str().c_str(), &bannersXML)) {
        media = new cTVDBSeriesMedia(bannersXML, language);
        media->ParseXML();
    }
    return media;
}

cTVDBActors *cTVDBScraper::ReadSeriesActors(int seriesID) {
    stringstream url;
    url << mirrors->GetMirrorXML() << "/api/" << apiKey << "/series/" << seriesID << "/actors.xml";
    string actorsXML;
    cTVDBActors *actors = NULL;
    if (CurlGetUrl(url.str().c_str(), &actorsXML)) {
        actors = new cTVDBActors(actorsXML, language);
        actors->ParseXML();
    }
    return actors;
}

void cTVDBScraper::StoreMedia(cTVDBSeries *series, cTVDBSeriesMedia *media, cTVDBActors *actors) {
    stringstream baseUrl;
    baseUrl << mirrors->GetMirrorBanner() << "/banners/";
    stringstream destDir;
    destDir << baseDir << "/" << series->ID() << "/";
    bool ok = CreateDirectory(destDir.str().c_str());
    if (!ok)
        return;
    if (series) {
        series->StoreBanner(baseUrl.str(), destDir.str());
    }
    if (media) {
        media->Store(baseUrl.str(), destDir.str());
    }
    if (actors) {
        actors->Store(baseUrl.str(), destDir.str());
    }
}
