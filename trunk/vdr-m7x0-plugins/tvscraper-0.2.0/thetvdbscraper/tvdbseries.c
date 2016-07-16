#include <string>
#include <sstream>
#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "tvdbseries.h"

using namespace std;

cTVDBSeries::cTVDBSeries(string xml) {
    doc = NULL;
    SetXMLDoc(xml);
    seriesID = 0;
    name = "";
    banner = "";
    overview = "";
    imbdid = "";
}

cTVDBSeries::~cTVDBSeries() {
    xmlFreeDoc(doc);
}

void cTVDBSeries::SetXMLDoc(string xml) {
    xmlInitParser();
    doc = xmlReadMemory(xml.c_str(), strlen(xml.c_str()), "noname.xml", NULL, 0);
}

void cTVDBSeries::ParseXML(void) {
    if (doc == NULL)
        return;
    //Root Element has to be <Data>
    xmlNode *node = NULL;
    node = xmlDocGetRootElement(doc);
    if (!(node && !xmlStrcmp(node->name, (const xmlChar *)"Data")))
        return;
    //Searching for  <Series>
    node = node->children;
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if ((cur_node->type == XML_ELEMENT_NODE) && !xmlStrcmp(cur_node->name, (const xmlChar *)"Series")) {
            node = cur_node;
            break;
        } else {
            node = NULL;
        }
    }
    if (!node)
        return;
    //now read the first series    
    node = node->children;
    xmlChar *node_content;
    for (cur_node = node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            node_content = xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
            if (!node_content)
                continue;
            if (!xmlStrcmp(cur_node->name, (const xmlChar *)"seriesid")) {
                seriesID = atoi((const char *)node_content);
            } else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"SeriesName")) {
                name = (const char *)node_content;
            } else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"Overview")) {
                overview = (const char *)node_content;
            } else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"banner")) {
                banner = (const char *)node_content;
            } else if (!xmlStrcmp(cur_node->name, (const xmlChar *)"IMDB_ID")) {
                imbdid = (const char *)node_content;
            }
            xmlFree(node_content);
        }
    }
}

void cTVDBSeries::StoreDB(cTVScraperDB *db) {
    db->InsertSeries(seriesID, name, overview);
}

void cTVDBSeries::StoreBanner(string baseUrl, string destDir) {
    if (banner.size() == 0)
        return;
    stringstream strUrl;
    strUrl << baseUrl << banner;
    string url = strUrl.str();
    stringstream fullPath;
    fullPath << destDir << "banner.jpg";
    string path = fullPath.str();
    CurlGetUrlFile(url.c_str(), path.c_str());
}

void cTVDBSeries::Dump() {
    esyslog("tvscraper: series %s, id: %d, overview %s, imdb %s", name.c_str(), seriesID, overview.c_str(), imbdid.c_str());
}
