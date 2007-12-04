/********************************************************************************
 * transponders.h 
 * supports transponder data. 
 * writen by reel-multimedia
 *  
 * Contact to mhahn@reel-multimedia.com
 * 
 ********************************************************************************/


#ifndef _TRANSPONDERS__H
#define _TRANSPONDERS__H

#define DBG "DEBUG [transponders]: "


#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <vector>

#include <assert.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <vdr/channels.h>

#include "debug.h"

#ifndef REELVDR

namespace setup
{
  std::string FileNameFactory(std::string Filename);
}

#endif

typedef struct {
    int card;
    int type;
    int source;
    int frequency;     // 0: Full band scan
    int symbolrate;
    int bandwidth;
    int polarization;
    int fec;
    int detail;        // DVB-T: Scan +-166kHz offsets
    int modulation;    // DVB-C: auto, ...
    int symbolrate_mode;  // DVB-C: 0/1/2 intelligent/dumb/fixed symbolrate

} scanParameters;


class cTransponder;

typedef std::vector<cTransponder *>::const_iterator constTpIter;
typedef std::vector<cTransponder *>::iterator TpIter;

// the source below ist taken from http://www.parashift.com/c++-faq-lite/

inline int strToInt(const std::string& s)
{
   std::istringstream i(s);
   int x = -1;
   if (!(i >> x))
     std::cerr << " error in strToInt(): " << s << std::endl;
   return x;
}
// --- Class cTransponder ------------------------------------------------------
   ///< base class which actual transponders can be derived

class cTransponder {
public:
   virtual ~cTransponder() {}
   int ChannelNum() const; 
   int Frequency() const;
   int Modulation() {return modulation_;};
   int Symbolrate() {return symbolrate_;};
   void SetFrequency(int f) ;
   void SetSymbolrate(int sr) {symbolrate_=sr;}
   void SetModulation(int mod) {modulation_=mod;}
   ///<  set transponder Data to channel
   virtual bool SetTransponderData(cChannel *c, int Code=0) = 0;
   bool Scanned();
   void SetScanned();


   std::string IntToStr(int num);
   int StrToInt(std::string str);
   int IntToFec(int val);
protected:
   cTransponder(int Frequency);
   int channelNum_;
   int frequency_;
   int symbolrate_;
   int modulation_;
   int bandwidth_;
   bool scanned_;
private:
   cTransponder(const cTransponder&);
   cTransponder &operator=(const cTransponder&);
   std::string service_; 
};

inline int cTransponder::ChannelNum() const
{
   return channelNum_;
}
inline int cTransponder::Frequency() const
{
   return frequency_;
}
inline void cTransponder::SetFrequency(int f) 
{
   frequency_=f;
}
inline void cTransponder::SetScanned() 
{
   scanned_ =true;
}
inline bool cTransponder::Scanned() 
{
   return scanned_;
}

// --- class cSatTransponder ---------------------------------------------------

class cSatTransponder : public cTransponder {
private:
   char pol_;
   int fec_;
public:
   cSatTransponder(void);
   cSatTransponder(int Frequency, char Pol, int SymbolRate, int FEC);
   bool SetTransponderData(cChannel *c, int Code=0);
   ///<  set transponder Data to channel
   bool Parse(const std::string& line);
};


// --- class cTerrTransponder --------------------------------------------------

class cTerrTransponder : public cTransponder {
public:
   cTerrTransponder(int ChannelNr, int Frequency, int Bandwidth);
   ///< ChannelNr is only for debunging purposes
   cTerrTransponder();
   ~cTerrTransponder();
   bool SetTransponderData(cChannel *c, int Code=0); 
   ///<  set transponder Data to channel
private:
   int fec_l_;
   int fec_h_;
   int hierarchy_;
   int transmission_;
   int guard_;
};


// --- class cCableTransponder -------------------------------------------------

class cCableTransponder : public cTransponder {
public:
   cCableTransponder(int ChannelNr, int Frequency, int Bandwidth, int sRate, int Mod);
   ~cCableTransponder();
   bool SetTransponderData(cChannel *c, int Code=0); 
private:
   int fec_h_;
   int hierarchy_;
   int transmission_;
   int guard_;
};

// ---- Class cTransponders --------------------------------------
///< Container class for cTransponder class
///< can be createt only once, and can hold
///< only one type of cTransponder 

class cTransponders 
{
public:
   static void Create();
   ///< call Constructor and instance_
   static void Destroy();
   ///<  put this to your destructor of class calling Create()
   static cTransponders &GetInstance();
   ///<  gives you reference to existing cTransponders
   void Clear();
   ///< Clears all transponders in vector. deletes all allocatet memmory 
   ///< don`t  use v_tp_.clear() !
   std::vector<cTransponder *> v_tp_;

   void Load(int source, scanParameters *scp);
   ///< loads complete transponder lists or single data for given source

   bool LoadNitTransponder(int Source);
   bool LoadTpl(const std::string& tpList);
   void Add(int Source, const scanParameters& scp);
   void CalcTerrTpl(); 
   ///< Calculates terrestrial transponder lists and load them to cTransponder 
   void CalcCableTpl(bool Complete, scanParameters *scp);
   ///< Calculates cable Transponder lists and load them to cTransponder 

   static int channel2Frequency(int region, int channel, int& bandwidth );
   ///< Common data of signal source:

   int SourceCode() const { return sourceCode_; }
   ///< returns vdr internal code of signal source
   std::string TplFileName(int source);
   ///< returns complete path and filename to transponder list
   std::string Position() const { return position_; }
   ///< returns abrev. of sources eg. S19.2E
   std::string Description() const { return description_; }
   ///< returns Descr. of sources eg. ASTRA ... 
   int LockMs() const { return lockMs_; }
   cTransponder *GetNITStartTransponder();
   //void ResetNITStartTransponder();
   bool MissingTransponder(int Transponder);
private:
   cTransponders();
   ~cTransponders();
   cTransponders(const cTransponders&);
   cTransponders &operator=(const cTransponders&);
   std::auto_ptr<cSatTransponder> nitStartTransponder_;
   //bool SetNITStartTransponder();
   static cTransponders *instance_;
   int sourceCode_; 

   std::string SetPosition(const std::string& tplFileName);
   std::string SetDescription();

   std::string position_;
   std::string description_;
   std::string fileName_;
   int lockMs_;
};


// --- Class cTransponders 
inline cTransponders::~cTransponders()
{
  Clear();
}
inline cTransponders &cTransponders::GetInstance()
{
  assert(instance_);
  return *instance_;
}

inline void cTransponders::Create()
{
  if (!instance_)
  {
    DLOG(DBG " Create Transponders");
    instance_ = new cTransponders();
  }
   else 
     DLOG(DBG " Transponders have been Created already");
}
inline void cTransponders::Destroy()
{
  printf (" DEBUG [transponders] ---------- delete instance_ ----- \n");
  delete instance_;
  instance_ = NULL;
}

inline cTransponder *cTransponders::GetNITStartTransponder()
{
  return nitStartTransponder_.get();

}

/*
inline void cTransponders::ResetNITStartTransponder()
{
   printf("DEBUG [channelscan]: delete nitStartTransponder_ \n");
   delete nitStartTransponder_;
   nitStartTransponder_ = NULL;
}
*/

#endif //_TRANSPONDERS__H
