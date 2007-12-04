#ifndef CFILTER_H
#define CFILTER_H

#include <map>
#include <set>
#include <vector>
#include <algorithm>

#include <stdint.h>

#include <vdr/eit.h>
#include <vdr/filter.h>
#include <vdr/channels.h>
#include <libsi/section.h>
#include <libsi/descriptor.h>

#include "transponders.h"

#define CMAXPMTENTRIES 256
#define CMAXSIDENTRIES 256
#ifdef REELVDR
#define MAXFILTERS 10 // FPGA has enough PID filters
#else
#define MAXFILTERS 6
#endif
#define FILTERTIMEOUT 10

extern std::map<int, cTransponder *> transponderMap;
extern std::map<int, int> TblVersions;

typedef std::map<int, cTransponder *>::const_iterator tpMapIter;
typedef std::map<int, cTransponder *>::iterator tpMapItr;
typedef std::pair<tpMapItr, bool> mapRet;

class SdtFilter;


class PatFilter : public cFilter {
private:
  int pSid[CMAXSIDENTRIES];
  int pSidCnt;

  time_t lastPmtScan[MAXFILTERS];
  int pmtIndex;
  int pmtPid[MAXFILTERS];

  int Sids[MAXFILTERS];
  uint64_t pmtVersion[CMAXPMTENTRIES];
  int numPmtEntries;
  bool PmtVersionChanged(int PmtPid, int Sid, int Version);
  int num,pit,pnum,numRunning;
  SdtFilter *sdtFilter;
  volatile bool endofScan;
  bool SidinSdt(int Sid);
  bool sdtfinished;
  time_t lastFound;
  int waitingForGodot;
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char *Data, int Length);
public:
  PatFilter(void);
  void SetSdtFilter(SdtFilter* SdtFilter);
  virtual void SetStatus(bool On);
  bool EndOfScan() {return endofScan;};
  void Trigger(void);
  void SdtFinished(void) {sdtfinished=true;};
  time_t LastFoundTime(void){return lastFound;};
  void GetFoundNum(int &current, int &total);
  };

int GetCaDescriptors(int Source, int Transponder, int ServiceId, const unsigned short *CaSystemIds, int BufSize, uchar *Data, bool &StreamFlag);

class SdtFilter : public cFilter {
friend class PatFilter;
private:
  int numSid,sid[CMAXSIDENTRIES];
  int usefulSid[CMAXSIDENTRIES];
  int numUsefulSid;
  cSectionSyncer sectionSyncer;
  PatFilter *patFilter;
  int AddServiceType;
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char *Data, int Length);
public:
  SdtFilter(PatFilter *PatFilter);
  virtual void SetStatus(bool On);
  };


#ifdef REELVDR 
#define MAXNITS 256
#endif
#define MAXNETWORKNAME 256


class NitFilter : public cFilter {
private:
  class cNit {
  public:
    u_short networkId;
    char name[MAXNETWORKNAME];
    bool hasTransponder;
    };

  cSectionSyncer sectionSyncer;
  cNit nits[MAXNITS];
  u_short networkId;
  int numNits;
  unsigned int lastCount;
  volatile bool endofScan;
  volatile bool found_;
  std::vector<int> sectionSeen_;
protected:
  virtual void Process(u_short Pid, u_char Tid, const u_char *Data, int Length);
public:
  NitFilter(void);
  ~NitFilter(void);
  virtual void SetStatus(bool On);
  bool EndOfScan() {return endofScan;};
  void Dump();
  //void Copy();
  bool Found();
  };


#endif
