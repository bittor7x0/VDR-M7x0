/*
 * parser.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <stack>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <vdr/config.h>
#include "common.h"
#include "config.h"
#include "tools.h"
#include "parser.h"

#include <expat.h>
#define LIBEXPAT_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#if LIBEXPAT_VERSION(XML_MAJOR_VERSION, XML_MINOR_VERSION, XML_MICRO_VERSION) < LIBEXPAT_VERSION(1, 95, 8)
#warning "Expat XML parser library 1.95.8 or newer required!"
#endif

// --- Globals ----------------------------------------------------------

cParser Parser;

// --- cItem(s) ---------------------------------------------------------

cItem::cItem()
: conv("UTF-8", cCharSetConv::SystemCharacterTable())
{
  strcpy(date, "");
  strcpy(title, "");
  strcpy(link, "");
  strcpy(description, "");
}


void cItem::Clear(void)
{
  strcpy(date, "");
  strcpy(title, "");
  strcpy(link, "");
  strcpy(description, "");
}

void cItem::SetDate(const char *str)
{
  Utf8Strn0Cpy(date, str, sizeof(date));
  compactspace(date);
  Utf8Strn0Cpy(date, conv.Convert(date), sizeof(date));
}

void cItem::SetTitle(const char *str)
{
  Utf8Strn0Cpy(title, str, sizeof(title));
  compactspace(title);
  striphtml(title);
  Utf8Strn0Cpy(title, conv.Convert(title), sizeof(title));
}

void cItem::SetLink(const char *str)
{
  Utf8Strn0Cpy(link, str, sizeof(link));
  compactspace(link);
  Utf8Strn0Cpy(link, conv.Convert(link), sizeof(link));
}

void cItem::SetDescription(const char *str)
{
  Utf8Strn0Cpy(description, str, sizeof(description));
  compactspace(description);
  striphtml(description);
  Utf8Strn0Cpy(description, conv.Convert(description), sizeof(description));
}

// --- Parse RSS  -------------------------------------------------------

struct XmlNode {
  char nodename[SHORT_TEXT_LEN];
  int  depth;
};

cItem *item = NULL;
int depth = 0;
char data_string[LONG_TEXT_LEN];
std::stack<struct XmlNode> nodestack;

static int XMLCALL UnknownEncodingHandler(void *data,const XML_Char *encoding, XML_Encoding *info)
{
  if (strcmp(encoding, "iso-8859-15") == 0) {
     int i;
     for (i = 0; i < 256; ++i)
        info->map[i] = i;
     info->data = NULL;
     info->convert = NULL;
     info->release = NULL;
     return XML_STATUS_OK;
     }
  return XML_STATUS_ERROR;
}

static void XMLCALL StartHandler(void *data, const char *el, const char **attr)
{
  XmlNode node;

  strn0cpy(node.nodename, el, sizeof(node.nodename));
  node.depth = depth;
  nodestack.push(node);

  if (!strncmp(el, "item", 4)) {
     cItem *tmpitem = new cItem;
     item = tmpitem;
     item->Clear();
     }
  depth++;
}

static void XMLCALL EndHandler(void *data, const char *el)
{
  char parent[SHORT_TEXT_LEN];
  
  if (nodestack.size() > 1) {
     nodestack.pop();
     } 
  else {
     nodestack.pop();
     return;
     }
  strn0cpy(parent, (nodestack.top()).nodename, sizeof((nodestack.top()).nodename));
  // No need to free the node
  
  depth--;
  if (!strncmp(el, "item", 4)) {
     // End of the current item
     if (*item->GetTitle())
        Parser.Items.Add(item);
     }
  else if (!strncmp(el, "title", 5)) {
     if (!strncmp(parent, "item", 4)) {
        item->SetTitle(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("cParser::EndHandler(): RSS title '%s'", data_string);
        }
     }
  else if (!strncmp(el, "link", 4)) {
     if (!strncmp(parent, "item", 4)) {
        item->SetLink(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("cParser::EndHandler(): RSS link '%s'", data_string);
        }
     }
  else if (!strncmp(el, "pubDate", 7)) {
     if (!strncmp(parent, "item", 4)) {
        item->SetDate(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("cParser::EndHandler(): RSS date '%s'", data_string);
        }
     }
  else if (!strncmp(el, "description", 11)) {
     if (!strncmp(parent, "item", 4)) {
        item->SetDescription(data_string);
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("cParser::EndHandler(): RSS description '%s'", data_string);
        }
     }
  else if (!strncmp(el, "content:encoded", 15)) {
     if (!strncmp(parent, "item", 4)) {
        item->SetDescription(data_string); // overdrive description with content:encoded !
        }
     else if (!strncmp(parent, "channel", 7)) {
        debug("cParser::EndHandler(): RSS content '%s'", data_string);
        }
     }
  strcpy(data_string, "");
}

static void DataHandler(void *user_data, const XML_Char *s, int len)
{
  /* Only until the maximum size of the buffer */
  if (strlen(data_string) + len <= LONG_TEXT_LEN)
     strncat(data_string, s, len);
}

static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)data;

  mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) {
     memcpy(&(mem->memory[mem->size]), ptr, realsize);
     mem->size += realsize;
     mem->memory[mem->size] = 0;
     }
  return realsize;
}

cParser::cParser()
{
  data.memory = NULL;
  data.size = 0;
}

cParser::~cParser()
{
  if (data.memory) {
     free(data.memory);
     data.memory = NULL;
     data.size = 0;
     }
}

int cParser::DownloadAndParse(const char *url)
{
  CURL *curl_handle;

  // Clear Items list and initialize depth
  Items.Clear();
  depth = 0;
  if (data.memory)
     free(data.memory);
  data.memory = NULL;
  data.size = 0;

  // Init the curl session
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();

  // Specify URL to get
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);

  // Specify HTTP proxy: my.proxy.com:80
  if (RssConfig.useproxy) {
     curl_easy_setopt(curl_handle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
     curl_easy_setopt(curl_handle, CURLOPT_PROXY, RssConfig.httpproxy);
     }

  // Send all data to this function
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  // Set maximum file size to get (bytes)
  curl_easy_setopt(curl_handle, CURLOPT_MAXFILESIZE, 1048576);

  // No progress meter
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1);

  // No signaling
  curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);

  // Set timeout to 30 seconds
  curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30);

  // Pass our 'data' struct to the callback function
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&data);

  // Some servers don't like requests that are made without a user-agent field
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, RSSREADER_USERAGENT);

  // Get it!
  if (curl_easy_perform(curl_handle) != 0) {
     // Cleanup curl stuff
     curl_easy_cleanup(curl_handle);
     // Free allocated memory
     if (data.memory) {
        free(data.memory);
        data.memory = NULL;
        data.size = 0;
        }
     error("cParser::DownloadAndParse(): couldn't download the stream");
     return (RSS_DOWNLOAD_ERROR);
     }

  if (data.size) {
#ifdef DEBUG
     // Only for debug dump
     FILE *fp = fopen("/tmp/rssreader.vdr", "w");
     if (fp) {
        fwrite(data.memory, 1, data.size, fp);
        fclose(fp);
        }
#endif
     // Setup expat
     XML_Parser p = XML_ParserCreate(NULL);
     if (!p) {
        // Cleanup curl stuff
        curl_easy_cleanup(curl_handle);
        // Free allocated memory
        if (data.memory) {
           free(data.memory);
           data.memory = NULL;
           data.size = 0;
           }
        error("cParser::DownloadAndParse(): couldn't allocate memory for parser");
        return (RSS_UNKNOWN_ERROR);
        }
     XML_SetElementHandler(p, StartHandler, EndHandler);
     XML_SetCharacterDataHandler(p, DataHandler);
     XML_SetUnknownEncodingHandler(p, UnknownEncodingHandler, NULL);

     if (XML_Parse(p, data.memory, data.size, 1) == XML_STATUS_ERROR) {
        // Cleanup curl stuff
        curl_easy_cleanup(curl_handle);
        // Free allocated memory
        if (data.memory) {
           free(data.memory);
           data.memory = NULL;
           data.size = 0;
           }
        error("cParser::DownloadAndParse(): Parse error at line %ld:\n%s\n", XML_GetCurrentLineNumber(p), XML_ErrorString(XML_GetErrorCode(p)));
        return (RSS_PARSING_ERROR);
        }
     }

  // Cleanup curl stuff
  curl_easy_cleanup(curl_handle);
  // Free allocated memory
  if (data.memory) {
     free(data.memory);
     data.memory = NULL;
     data.size = 0;
     }

  return (RSS_PARSING_OK);
}

