/*
 * parser.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_PARSER_H
#define __RSSREADER_PARSER_H

#include "common.h"
#include "compatibility.h"
#include <string>

using namespace std;

// --- Defines ----------------------------------------------------------

#define SHORT_TEXT_LEN 2048
#define LONG_TEXT_LEN  16384

// --- cItem(s) ---------------------------------------------------------

class cItem : public cListObject {
	
private:
  cCharSetConv conv;
  string date;
  string title;
  string link;
  string description; 
public:
  cItem(const char *e);
//  ~cItem(){debug ("destruir citem [%s]",title.c_str());};
  void Clear(void);
  const char *GetDate(void) { return date.c_str(); }
  const char *GetTitle(void) { return title.c_str(); }
  const char *GetLink(void) { return link.c_str(); }
  const char *GetDescription(void)  { return description.c_str(); }
  void SetDate(const char *str);
  void SetTitle(const char *str);
  void SetLink(const char *str);
  void SetDescription(const char *str);  
};

class cItems : public cList<cItem> {
private:
  cItem *current;
};

// --- cParser ----------------------------------------------------------

struct MemoryStruct {
  char   *memory;
  size_t size;
};

class cParser {
private:
  struct MemoryStruct data;
  char encoding[SHORT_TEXT_LEN];
  void SetEncoding(const char *str);
public:
  enum {
    RSS_PARSING_OK     =  0,
    RSS_UNKNOWN_ERROR  = -1,
    RSS_DOWNLOAD_ERROR = -2,
    RSS_PARSING_ERROR  = -3
  } eRssError;
  cParser();
  ~cParser();
  int DownloadAndParse(const char *url);
  cItems Items;
  };

extern cParser Parser;

#endif // __RSSREADER_PARSER_H
