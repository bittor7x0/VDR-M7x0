#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <jansson.h>
#include "moviedbmovie.h"

using namespace std;

cMovieDbMovie::cMovieDbMovie(string json) {
    this->json = json;
    title = "";
    originalTitle = "";
    overview = "";
    backdropPath = "";
    posterPath = "";
}

cMovieDbMovie::~cMovieDbMovie() {
    
}

void cMovieDbMovie::ParseJSON(void) {
    json_t *movie;
    json_error_t error;
    movie = json_loads(json.c_str(), 0, &error);
    if (!movie) {
        return;
    }
    if(!json_is_object(movie)) {
        return;
    }
    json_t *jTitle = json_object_get(movie, "title");
    if(json_is_string(jTitle)) {
        title = json_string_value(jTitle);;
    }
    json_t *jOriginalTitle = json_object_get(movie, "original_title");
    if(json_is_string(jOriginalTitle)) {
        originalTitle = json_string_value(jOriginalTitle);
    }
    json_t *jOverview = json_object_get(movie, "overview");
    if(json_is_string(jOverview)) {
        overview = json_string_value(jOverview);
    }
    json_t *jBackdrop = json_object_get(movie, "backdrop_path");
    if(json_is_string(jBackdrop)) {
        backdropPath = json_string_value(jBackdrop);
    }
    json_t *jPoster = json_object_get(movie, "poster_path");
    if(json_is_string(jPoster)) {
        posterPath = json_string_value(jPoster);
    }
}

int cMovieDbMovie::ParseJSONForMovieId(string movieSearchString) {
    //convert searchstring to lower case
    transform(movieSearchString.begin(), movieSearchString.end(), movieSearchString.begin(), ::tolower);
    json_t *root;
    json_error_t error;
    root = json_loads(json.c_str(), 0, &error);
    if (!root) {
        return -1;
    }
    if(!json_is_object(root)) {
        return -1;
    }
    json_t *results = json_object_get(root, "results");
    if(!json_is_array(results)) {
        return -1;
    }
    size_t numResults = json_array_size(results);
    for (size_t res = 0; res < numResults; res++) {
        json_t *result = json_array_get(results, res);
        if (!json_is_object(result)) {
            return -1;
        }
        json_t *title = json_object_get(result, "title");
        if (!json_is_string(title)) {
            return -1;
        }
        string resultTitle = json_string_value(title);
        //convert result to lower case
        transform(resultTitle.begin(), resultTitle.end(), resultTitle.begin(), ::tolower);
        json_t *jId = json_object_get(result, "id");
        if (json_is_integer(jId)) {
            int id = (int)json_integer_value(jId);
            searchResult sRes;
            sRes.id = id;
            sRes.distance = sentence_distance(resultTitle, movieSearchString);
            resultSet.push_back(sRes);
        }
    }
    return FindBestResult();
}

int cMovieDbMovie::FindBestResult(void) {
    int resID = -1;
    int bestMatch = -1;
    int numResults = resultSet.size();
    for (int i=0; i<numResults; i++) {
        if (i == 0) {
            bestMatch = resultSet[i].distance;
            resID = resultSet[i].id;
        } else if (resultSet[i].distance < bestMatch) {
            bestMatch = resultSet[i].distance;
            resID = resultSet[i].id;
        }
    }
    return resID;
}

void cMovieDbMovie::StoreMedia(string posterBaseUrl, string backdropBaseUrl, string destDir) {
    stringstream pathPoster;
    pathPoster << destDir << id << "_poster.jpg";
    stringstream posterUrl;
    posterUrl << posterBaseUrl << posterPath;
    if (!FileExists(pathPoster.str())) {
        CurlGetUrlFile(posterUrl.str().c_str(), pathPoster.str().c_str());
    }
    stringstream pathBackdrop;
    pathBackdrop << destDir << id << "_backdrop.jpg";
    stringstream backdropUrl;
    backdropUrl << backdropBaseUrl << backdropPath;
    if (!FileExists(pathBackdrop.str())) {
        CurlGetUrlFile(backdropUrl.str().c_str(), pathBackdrop.str().c_str());
    }
}

void cMovieDbMovie::StoreDB(cTVScraperDB *db) {
    db->InsertMovie(id, title, originalTitle, overview);
}

void cMovieDbMovie::Dump(void) {
    esyslog("tvscraper: -------------- MOVIE DUMP ---------------");
    esyslog("tvscraper: title %s", title.c_str());
    esyslog("tvscraper: originalTitle %s", originalTitle.c_str());
    esyslog("tvscraper: overview %s", overview.c_str());
    esyslog("tvscraper: backdropPath %s", backdropPath.c_str());
    esyslog("tvscraper: posterPath %s", posterPath.c_str());
}