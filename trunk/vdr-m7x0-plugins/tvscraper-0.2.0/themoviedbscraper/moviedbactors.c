#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <jansson.h>
#include "moviedbactors.h"

using namespace std;

cMovieDbActors::cMovieDbActors(string json) {
    this->json = json;
}

cMovieDbActors::~cMovieDbActors() {
    actors.clear();
}

void cMovieDbActors::ParseJSON(void) {
    json_t *jActors;
    json_error_t error;
    jActors = json_loads(json.c_str(), 0, &error);
    if (!jActors) {
        return;
    }
    if(!json_is_object(jActors)) {
        return;
    }
    json_t *cast = json_object_get(jActors, "cast");
    if(!json_is_array(cast)) {
        return;
    }
    size_t numActors = json_array_size(cast);
    for (size_t i = 0; i < numActors; i++) {
        json_t *jActor = json_array_get(cast, i);
        if (!json_is_object(jActor)) {
            return;
        }
        json_t *jId = json_object_get(jActor, "id");
        json_t *jName = json_object_get(jActor, "name");
        json_t *jRole = json_object_get(jActor, "character");
        json_t *jPath = json_object_get(jActor, "profile_path");
        if (!json_is_integer(jId) || !json_is_string(jName) || !json_is_string(jRole) || !json_is_string(jPath))
            return;
        cMovieDBActor *actor = new cMovieDBActor();
        actor->id = json_integer_value(jId);
        actor->name = json_string_value(jName);
        actor->role = json_string_value(jRole);
        actor->path = json_string_value(jPath);
        actors.push_back(actor);
    }
}

void cMovieDbActors::StoreDB(cTVScraperDB *db, int movieID) {
    int numActors = actors.size();
    for (int i=0; i<numActors; i++) {
        db->InsertMovieActor(movieID, actors[i]->id, actors[i]->name, actors[i]->role);
    }
}

void cMovieDbActors::Store(string baseUrl, string destDir) {
    int size = actors.size();
    string path;
    string url;
    for (int i=0; i<size; i++) {
        stringstream strUrl;
        strUrl << baseUrl << actors[i]->path;
        url = strUrl.str();
        stringstream fullPath;
        fullPath << destDir << "/actor_" << actors[i]->id << ".jpg";
        path = fullPath.str();
        if (!FileExists(path)) {
            CurlGetUrlFile(url.c_str(), path.c_str());
        }
    }
}

void cMovieDbActors::Dump(void) {
    int numActors = actors.size();
    esyslog("tvscraper: %d Actors:", numActors);
    for (int i=0; i<numActors; i++) {
        esyslog("tvscraper: id %d, name %s, role %s, path %s", actors[i]->id, actors[i]->name.c_str(), actors[i]->role.c_str(), actors[i]->path.c_str());
    }
}