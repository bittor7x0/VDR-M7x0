#include "imageserver.h"

using namespace std;

cImageServer::cImageServer(cTVScraperDB *db, cOverRides *overrides) {
    this->db = db;
    this->overrides = overrides;
}

cImageServer::~cImageServer() {
}

scrapType cImageServer::GetScrapType(const cEvent *event) {
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

int cImageServer::GetID(int eventID, scrapType type, bool isRecording) {
    int id = 0;
    if (type == scrapSeries) {
        id = db->GetSeriesID(eventID, isRecording);
    } else if (type == scrapMovie) {
        id = db->GetMovieID(eventID, isRecording);
    }
    return id;
}

cTvMedia cImageServer::GetPosterOrBanner(int id, scrapType type) {
    cTvMedia media;
    media.path = "";
    media.width = 0;
    media.height = 0;
    if (type == scrapSeries) {
        stringstream path;
        path << config.GetBaseDir() << "/series/" << id << "/banner.jpg";
        media.path = path.str();
        media.width = 758;
        media.height = 140;
    } else if (type == scrapMovie) {
        stringstream path;
        path << config.GetBaseDir() << "/movies/" << id << "_poster.jpg";
        media.path = path.str();
        media.width = 500;
        media.height = 750;
    }
    return media;
}

cTvMedia cImageServer::GetPoster(int id, scrapType type) {
    cTvMedia media;
    media.path = "";
    media.width = 0;
    media.height = 0;
    if (type == scrapSeries) {
        stringstream path;
        path << config.GetBaseDir() << "/series/" << id << "/poster_0.jpg";
        string filePoster = path.str();
        if (FileExists(filePoster)) {
            media.path = filePoster;
            media.width = 680;
            media.height = 1000;
        }
    } else if (type == scrapMovie) {
        stringstream path;
        path << config.GetBaseDir() << "/movies/" << id << "_poster.jpg";
        string filePoster = path.str();
        if (FileExists(filePoster)) {
            media.path = path.str();
            media.width = 500;
            media.height = 750;
        }
    }
    return media;
}

cTvMedia cImageServer::GetBanner(int id) {
    cTvMedia media;
    media.path = "";
    media.width = 0;
    media.height = 0;
    stringstream path;
    path << config.GetBaseDir() << "/series/" << id << "/banner.jpg";
    string fileBanner = path.str();
    if (FileExists(fileBanner)) {
        media.path = fileBanner;
        media.width = 758;
        media.height = 140;
    }
    return media;
}

vector<cTvMedia> cImageServer::GetPosters(int id, scrapType type) {
    vector<cTvMedia> posters;
    if (type == scrapSeries) {
        for (int i=0; i<3; i++) {
            stringstream path;
            path << config.GetBaseDir() << "/series/" << id << "/poster_" << i << ".jpg";
            string filePoster = path.str();
            if (FileExists(filePoster)) {
                cTvMedia media;
                media.path = filePoster;
                media.width = 680;
                media.height = 1000;
                posters.push_back(media);
            } else
                break;
        }
    } else if (type == scrapMovie) {
        stringstream path;
        path << config.GetBaseDir() << "/movies/" << id << "_poster.jpg";
        string filePoster = path.str();
        if (FileExists(filePoster)) {
            cTvMedia media;
            media.path = path.str();
            media.width = 500;
            media.height = 750;
            posters.push_back(media);
        }
    }
    return posters;
}

vector<cTvMedia> cImageServer::GetSeriesFanarts(int id) {
    vector<cTvMedia> fanart;
    for (int i=0; i<3; i++) {
        stringstream path;
        path << config.GetBaseDir() << "/series/" << id << "/fanart_" << i << ".jpg";
        string fileFanart = path.str();
        if (FileExists(fileFanart)) {
            cTvMedia media;
            media.path = fileFanart;
            media.width = 1920;
            media.height = 1080;
            fanart.push_back(media);
        } else
            break;
    }
    return fanart;
}

cTvMedia cImageServer::GetMovieFanart(int id) {
    cTvMedia fanart;
    stringstream path;
    path << config.GetBaseDir() << "/movies/" << id << "_backdrop.jpg";
    string fileFanart = path.str();
    if (FileExists(fileFanart)) {
        fanart.path = fileFanart;
        fanart.width = 1280;
        fanart.height = 720;
    }
    return fanart;
}


vector<cActor> cImageServer::GetActors(int id, scrapType type) {
    vector<cActor> actors;
    if (type == scrapSeries) {
        vector<vector<string> > actorsDB = db->GetActorsSeries(id);
        int numActors = actorsDB.size();
        for (int i=0; i < numActors; i++) {
            vector<string> row = actorsDB[i];
            if (row.size() == 3) {
                cActor actor;
                actor.name = row[0];
                actor.role = row[1];
                cTvMedia thumb;
                stringstream thumbPath;
                thumbPath << config.GetBaseDir() << "/series/" << id << "/" << row[2];
                thumb.path = thumbPath.str();
                thumb.width = 300;
                thumb.height = 450;
                actor.actorThumb = thumb;
                actors.push_back(actor);
            }
        }
    } else if (type == scrapMovie) {
        vector<vector<string> > actorsDB = db->GetActorsMovie(id);
        int numActors = actorsDB.size();
        for (int i=0; i < numActors; i++) {
            vector<string> row = actorsDB[i];
            if (row.size() == 3) {
                cActor actor;
                actor.name = row[1];
                actor.role = row[2];
                stringstream thumbPath;
                thumbPath << config.GetBaseDir() << "/movies/actors/actor_" << row[0] << ".jpg";
                cTvMedia thumb;
                thumb.path = thumbPath.str();
                thumb.width = 421;
                thumb.height = 632;
                actor.actorThumb = thumb;
                actors.push_back(actor);
            }
        }
    }
    return actors;
}

string cImageServer::GetDescription(int id, scrapType type) {
    string description;
    if (type == scrapSeries) {
        description = db->GetDescriptionSeries(id);
    } else if (type == scrapMovie) {
        description = db->GetDescriptionMovie(id);
    }
    return description;
}
