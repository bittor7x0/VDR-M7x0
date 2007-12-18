#ifndef MENU_CHANNEL_H
#define MENU_CHANNEL_H

#include <vdr/menu.h>
#include <vdr/channels.h>

#define CHNUMWIDTH (numdigits(Channels.MaxNumber()) + 1)

class cMenuChannelItem : public cOsdItem {
    public:
	enum eChannelSortMode { csmNumber, csmName, csmProvider };
    private:
	static eChannelSortMode sortMode;
	cChannel *channel;
    public:
	cMenuChannelItem(cChannel *Channel);
	static void SetSortMode(eChannelSortMode SortMode) { sortMode = SortMode; }
	static void IncSortMode(void) { sortMode = eChannelSortMode((sortMode == csmProvider) ? csmNumber : sortMode + 1); }
	static eChannelSortMode SortMode(void) { return sortMode; }
	virtual int Compare(const cListObject &ListObject) const;
	virtual void Set(void);
	cChannel *Channel(void) { return channel; }
};

class cMenuChannels : public cOsdMenu {
protected:
  int number;
  cTimeMs numberTimer;
  void Setup(void);
  cChannel *GetChannel(int Index);
  void Propagate(void);
protected:
  eOSState Number(eKeys Key);
  eOSState Switch(void);
  eOSState Edit(void);
  eOSState New(void);
  eOSState Delete(void);
  virtual void Move(int From, int To);
public:
  cMenuChannels(void);
  ~cMenuChannels();
  virtual eOSState ProcessKey(eKeys Key);
  };


class cRadioListMenu : public cMenuChannels {

private:
    void Setup(void);
    
public:
    cRadioListMenu(void);
};

#endif
