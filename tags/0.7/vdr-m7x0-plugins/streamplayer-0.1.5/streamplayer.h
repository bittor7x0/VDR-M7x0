#ifndef __streamplayer_h__
#define __streamplayer_h__

#include <vdr/plugin.h>

class cBookmark : public cListObject {
private:
	char *title;
	char *url;
public:
	cBookmark();
	cBookmark(char *Title, char *Url);
	~cBookmark();
	bool Parse(const char *s);
	bool Save(FILE *f);
	void Set(char *Title, char *Url);
	char* GetTitle() { return title; }
	char* GetUrl() { return url; }
};

class cBookmarks : public cConfig<cBookmark> {

};

class cPluginStreamplayer : public cPlugin {
public:
  // Add any member variables or functions you may need here.

  char PlayURL[128];
  int BufferSize;
  int BufferPreFill;

  cBookmarks Bookmarks;
  
  static cPluginStreamplayer *Plugin;

  cPluginStreamplayer(void);
  virtual ~cPluginStreamplayer();
  virtual const char *Version(void);
  virtual const char *Description(void);
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  void Store();
  bool StartPlayer(char *url);
  static bool GetProtocol(char *dest, int len, char *&url);
};

#endif
