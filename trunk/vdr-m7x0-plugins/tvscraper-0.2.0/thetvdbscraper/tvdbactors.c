#include <string>
#include <sstream>
#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "tvdbactors.h"

using namespace std;

cTVDBActors::cTVDBActors(string xml, string language) {
    doc = NULL;
    SetXMLDoc(xml);
    this->language = language;
}

cTVDBActors::~cTVDBActors() {
    xmlFreeDoc(doc);
    actors.clear();
}

void cTVDBActors::SetXMLDoc(string xml) {
    xmlInitParser();
    doc = xmlReadMemory(xml.c_str(), strlen(xml.c_str()), "noname.xml", NULL, 0);
}

void cTVDBActors::ParseXML(void) {
    if (doc == NULL)
        return;
    //Root Element has to be <Actors>
    xmlNode *node = NULL;
    node = xmlDocGetRootElement(doc);
    if (!(node && !xmlStrcmp(node->name, (const xmlChar *)"Actors")))
        return;
    //Looping through actors
    node = node->children;
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if ((cur_node->type == XML_ELEMENT_NODE) && !xmlStrcmp(cur_node->name, (const xmlChar *)"Actor")) {
            ReadEntry(cur_node->children);
        }
    }
}

void cTVDBActors::ReadEntry(xmlNode *node) {
    xmlNode *cur_node = NULL;
    xmlChar *node_content;
    cTVDBActor *actor = new cTVDBActor();
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            node_content = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
            if (!node_content)
                continue;
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"Image")) {
                actor->path = (const char *)node_content;
            } else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"Name")) {
                actor->name = (const char *)node_content;
            } else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"Role")) {
                actor->role = (const char *)node_content;
            }
            xmlFree(node_content);
        }
    }
    actors.push_back(actor);
}

void cTVDBActors::StoreDB(cTVScraperDB *db, int series_id) {
    int size = actors.size();
    for (int i=0; i<size; i++) {
        stringstream strThumb;
        strThumb << "actor_" << i << ".jpg";
        db->InsertActor(series_id, actors[i]->name, actors[i]->role, strThumb.str());
    }
}

void cTVDBActors::Store(string baseUrl, string destDir) {
    int size = actors.size();
    string path;
    string url;
    for (int i=0; i<size; i++) {
        stringstream strUrl;
        strUrl << baseUrl << actors[i]->path;
        url = strUrl.str();
        stringstream fullPath;
        fullPath << destDir << "actor_" << i << ".jpg";
        path = fullPath.str();
        if (actors[i]->path.size() > 0) {
            CurlGetUrlFile(url.c_str(), path.c_str());
        }
    }
}

void cTVDBActors::Dump(bool verbose) {
    int size = actors.size();
    esyslog("tvscraper: read %d actor entries", size);
    if (!verbose)
        return;
    for (int i=0; i<size; i++) {
        esyslog("tvscraper: path %s, name: %s, role %s", actors[i]->path.c_str(), actors[i]->name.c_str(), actors[i]->role.c_str());
    }
}