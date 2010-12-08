/*
 * menu.h: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#ifndef __RSSREADER_MENU_H
#define __RSSREADER_MENU_H

#include <vdr/osdbase.h>
#include <vdr/tools.h>

// --- cRssItem(s) ------------------------------------------------------

class cRssItem : public cListObject {
private:
  char *title;
  char *url;
public:
  cRssItem(void);
  virtual ~cRssItem();
  bool Parse(const char *s);
  const char *Title(void) { return title; }
  const char *Url(void) { return url; }
};

class cRssItems : public cConfig<cRssItem> {
public:
  virtual bool Load(const char *filename);
};

extern cRssItems RssItems;

// --- cRssMenuItem --------------------------------------------------------

class cRssMenuItem : public cOsdMenu {
private:
  char *text;
public:
  cRssMenuItem(const char *Date, const char *Title, const char *Link, const char *Description);
  virtual ~cRssMenuItem();
  virtual void Display(void);
  virtual eOSState ProcessKey(eKeys Key);
  };

// --- cRssItemsMenu -------------------------------------------------------

class cRssItemsMenu: public cOsdMenu {
private:
  eOSState ShowDetails(void);
public:
  cRssItemsMenu(void);
  virtual eOSState ProcessKey(eKeys Key);
};

// --- cRssStreamsMenu -----------------------------------------------------

class cRssStreamsMenu: public cOsdMenu {
public:
  cRssStreamsMenu();
  virtual eOSState ProcessKey(eKeys Key);
  virtual eOSState Select(void);
};

#endif // __RSSREADER_MENU_H
