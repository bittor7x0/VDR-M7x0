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

// --- Defines ----------------------------------------------------------

#define SHORT_TEXT_LEN 2048
#define LONG_TEXT_LEN  16384

// --- cItem(s) ---------------------------------------------------------

class cItem : public cListObject {
private:
  cCharSetConv conv;
  char date[SHORT_TEXT_LEN];
  char title[SHORT_TEXT_LEN];
  char link[SHORT_TEXT_LEN];
  char description[LONG_TEXT_LEN];
public:
  cItem();
  void Clear(void);
  char *GetDate(void) { return date; }
  char *GetTitle(void) { return title; }
  char *GetLink(void) { return link; }
  char *GetDescription(void)  { return description; }
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
