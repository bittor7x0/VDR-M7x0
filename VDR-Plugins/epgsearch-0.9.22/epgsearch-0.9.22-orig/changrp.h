#ifndef __EPGSEARCHCHANGRP_H
#define __EPGSEARCHCHANGRP_H

#include <vdr/status.h>
#include <vdr/plugin.h>
#include <vdr/tools.h>

class cSearchExt;

// --- cChannelGroupItem --------------------------------------------------------
class cChannelGroupItem : public cListObject {
public:
    cChannel* channel;
public:
  cChannelGroupItem(cChannel* ch) : channel(ch) {}
};

// --- cChannelGroup --------------------------------------------------------
class cChannelGroup : public cListObject {
public:
    char name[MaxFileName];
    cList<cChannelGroupItem> channels;
public:
  cChannelGroup(void);
  virtual ~cChannelGroup(void);

  bool Parse(const char *s);
  const char *ToText(void);
  bool Save(FILE *f);
  int* CreateChannelSel();
  void CreateChannelList(int*);
  bool ChannelInGroup(cChannel*);
};

// --- cChannelGroups --------------------------------------------------------
class cChannelGroups : public cConfig<cChannelGroup> {
private:
public:
    cChannelGroups(void) {}
    ~cChannelGroups(void) {}
    int GetIndex(char* channelGroup);
    cChannelGroup* GetGroupByName(const char* channelGroup);
    cSearchExt* Used(cChannelGroup*);
    char** CreateMenuitemsList();
};

extern cChannelGroups ChannelGroups;

// --- cMenuChannelGroupItem ----------------------------------------------------------
class cMenuChannelGroupItem : public cOsdItem {
  private:
public:
    cChannelGroup* group;
    cMenuChannelGroupItem(cChannelGroup*);
    void Set(void);
  };

// --- cMenuChannelGroups --------------------------------------------------------
class cMenuChannelGroups : public cOsdMenu {
  private:
    cChannelGroup *CurrentGroup(void);
    eOSState New(void);
    eOSState Delete(void);
    int groupSel;
    char** groupName;

 protected:
    virtual eOSState ProcessKey(eKeys Key);
 public:
    cMenuChannelGroups(char** groupName = NULL);
};

// --- cMenuEditChannelGroup --------------------------------------------------------
class cMenuEditChannelGroup : public cOsdMenu {
private:
  cChannelGroup *group;
  bool addIfConfirmed;
  char name[MaxFileName];
  int* channelSel;
public:
  cMenuEditChannelGroup(cChannelGroup *group, bool New = false);
  ~cMenuEditChannelGroup();
  void Set();
  virtual eOSState ProcessKey(eKeys Key);
};

#endif
