#ifndef __PREFERCHANNEL_H
#define __PREFERCHANNEL_H

#include <vdr/plugin.h>
#include <vdr/channels.h>

#define CHANNELSMAX 255

class cPreferedChannel : public cListObject {
private:
  tChannelID Index;
public:
  bool Parse(char *s);
  bool Save(FILE *f);
  const tChannelID GetIndex() { return Index; }
  void SetIndex(tChannelID index) { Index = index; }
  virtual int Compare(const cListObject &ListObject) const;
};

class cPreferedChannels : public cConfig<cPreferedChannel> {
private:
  const char *FileName_;
public:
  static tChannelID channel_to_remove;
  virtual bool Load(const char *filename, bool dummy=false);
  bool Save(void);
  cPreferedChannel *GetPreferedChannelNumber(int number);
  void AddPreferedChannel(const tChannelID Index);
  void RemovePreferedChannel(tChannelID number);
  tChannelID GetCurrentPrefered();
};

#endif //__PREFERCHANNEL_H

