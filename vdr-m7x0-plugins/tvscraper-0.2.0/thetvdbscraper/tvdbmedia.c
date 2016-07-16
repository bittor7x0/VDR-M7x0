#include <string>
#include <sstream>
#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "tvdbmedia.h"

using namespace std;

cTVDBSeriesMedia::cTVDBSeriesMedia(string xml, string language) {
    doc = NULL;
    SetXMLDoc(xml);
    this->language = language;
}

cTVDBSeriesMedia::~cTVDBSeriesMedia() {
    xmlFreeDoc(doc);
    medias.clear();
}

void cTVDBSeriesMedia::SetXMLDoc(string xml) {
    xmlInitParser();
    doc = xmlReadMemory(xml.c_str(), strlen(xml.c_str()), "noname.xml", NULL, 0);
}

void cTVDBSeriesMedia::ParseXML(void) {
    if (doc == NULL)
        return;
    //Root Element has to be <Banners>
    xmlNode *node = NULL;
    node = xmlDocGetRootElement(doc);
    if (!(node && !xmlStrcmp(node->name, (const xmlChar *)"Banners")))
        return;
    //Looping through banners
    node = node->children;
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if ((cur_node->type == XML_ELEMENT_NODE) && !xmlStrcmp(cur_node->name, (const xmlChar *)"Banner")) {
            ReadEntry(cur_node->children);
        }
    }
}

void cTVDBSeriesMedia::ReadEntry(xmlNode *node) {
    xmlNode *cur_node = NULL;
    xmlChar *node_content;
    cTVDBMedia *media = new cTVDBMedia();
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            node_content = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
            if (!node_content)
                continue;
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"BannerPath")) {
                media->path = (const char *)node_content;
            } else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"BannerType")) {
                if (!xmlStrcmp(node_content, (const xmlChar *)"poster"))
                    media->type = mediaPoster;
                else if (!xmlStrcmp(node_content, (const xmlChar *)"fanart"))
                    media->type = mediaFanart;
                else if (!xmlStrcmp(node_content, (const xmlChar *)"banner"))
                    media->type = mediaBanner;
                else if (!xmlStrcmp(node_content, (const xmlChar *)"season"))
                    media->type = mediaSeason;
                else
                    media->type = mediaUnknown;
            } else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"Season")) {
                media->season = atoi((const char *)node_content);
            } else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"Language")) {
                media->language = (const char *)node_content;
            }
            xmlFree(node_content);
        }
    }
    if (media->type == mediaUnknown)
        delete media;
    else if ((media->type == mediaSeason) && (media->language).compare(language))
        delete media;
    else
        medias.push_back(media);
}

void cTVDBSeriesMedia::Store(string baseUrl, string destDir) {
    int size = medias.size();
    string path;
    string url;
    int maxPix = 3;
    int numFanart = 0;
    int numPoster = 0;
    int numSeason = 0;
    int numBanner = 0;
    for (int i=0; i<size; i++) {
        if (medias[i]->path.size() == 0)
            continue;
        stringstream strUrl;
        strUrl << baseUrl << medias[i]->path;
        url = strUrl.str();
        switch (medias[i]->type) {
            case mediaFanart: {
                if (numFanart >= maxPix)
                    continue;
                stringstream fullPath;
                fullPath << destDir << "fanart_" << numFanart << ".jpg";
                path = fullPath.str();
                numFanart++;
                break; }
            case mediaPoster: {
                if (numPoster >= maxPix)
                    continue;
                stringstream fullPath;
                fullPath << destDir << "poster_" << numPoster << ".jpg";
                path = fullPath.str();
                numPoster++;
                break; }
            case mediaSeason: {
                stringstream fullPath;
                if (medias[i]->season > 0) {
                    fullPath << destDir << "season_poster_" << medias[i]->season << ".jpg";
                } else {
                    if (numSeason >= maxPix)
                        continue;
                    fullPath << destDir << "season_" << numSeason << ".jpg";
                    numSeason++;
                }
                path = fullPath.str();
                break; }
            case mediaBanner: {
                if (numBanner > 9)
                    continue;
                stringstream fullPath;
                fullPath << destDir << "banner_" << numBanner << ".jpg";
                path = fullPath.str();
                numBanner++;
                break; }
            default:
                break;
        }
        CurlGetUrlFile(url.c_str(), path.c_str());
    }
}

void cTVDBSeriesMedia::Dump(bool verbose) {
    int size = medias.size();
    esyslog("tvscraper: read %d banner entries", size);
    if (!verbose)
        return;
    for (int i=0; i<size; i++) {
        esyslog("tvscraper: type %d, path %s, lang: %s, season %d", medias[i]->type, medias[i]->path.c_str(), medias[i]->language.c_str(), medias[i]->season);
    }
}