/********************************************************************************
 * transponders.c
 * supports transponder data.
 * writen by reel-multimedia
 *
 * Contact to mhahn@reel-multimedia.com
 ********************************************************************************/

#include "transponders.h"
#include <vdr/plugin.h>
#include <vdr/sources.h>
#include "m7x0_dvb/frontend.h"

#define SAT_TPL "Sat.tpl"

#ifdef DEBUG_CHANNELSCAN
#define DEBUG_TRANSPONDER(format, args...) printf (format, ## args)
#else
#define DEBUG_TRANSPONDER(format, args...)
#endif

using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::cout;

/* Notation for Europe (region 0)
   channel 1-4: VHF band 1
   5-13: VHF band 3
   21-69: UHF band 4/5
   101-110/111-120: cable frequencies (aka Sonderkanal/Midband/Superband S1-S20)
   121-141: cable frquencies (aka Hyperband, S21-S41)
   173/181: D73 and D81 in C-networks (Germany)
*/
   
int cTransponders::channel2Frequency(int region, int channel, int& bandwidth )
{
    bandwidth=BANDWIDTH_7_MHZ;

    if (region==0) {
        if (channel>=1 && channel <=4) {
            return 38000000 + (channel-1)*7000000; 
        }
        if (channel>=5 && channel<=13) {
            return 177500000 + 7000000*(channel-5);
        }
        if (channel>=21 && channel<=69) {
            bandwidth=BANDWIDTH_8_MHZ;
            return 474000000 + 8000000*(channel-21);
        }
        if (channel==101) 
            return 107500000;

        if (channel==102 || channel==103) {
            bandwidth=BANDWIDTH_8_MHZ;
            return 113000000+8000000*(channel-102);
        }
        if (channel>=104 && channel<=110) 
            return 128000000+7000000*(channel-104); // Fixme +500khz Offset?
            
        if (channel>=111 && channel<=120) 
            return 233000000+7000000*(channel-111); // Fixme +500khz Offset?

        if (channel>=121 && channel<=141) {
            bandwidth=BANDWIDTH_8_MHZ;
            return 306000000+8000000*(channel-121);
        }    
        if (channel==173) {
            bandwidth=BANDWIDTH_8_MHZ;
            return 73000000;
        }
        if (channel==181) {
            bandwidth=BANDWIDTH_8_MHZ;
            return 81000000;
        }
        
    }
    return 0;
}
//----------- Class Transponder -------------------------------


cTransponder::cTransponder(int Frequency)
:channelNum_(0), frequency_(Frequency), scanned_(false)
{
};

int cTransponder::IntToFec(int val)
{
   switch (val) {
     case 12: return FEC_1_2;
     case 23: return FEC_2_3;
     case 34: return FEC_3_4;
     case 45: return FEC_4_5;
     case 56: return FEC_5_6;
     case 67: return FEC_6_7;
     case 78: return FEC_7_8;
     default: return FEC_AUTO;
   }
   return FEC_NONE;
}

#ifdef M750S
//----------- Class cSatTransponder -------------------------------

cSatTransponder::cSatTransponder()
:cTransponder(0)
{
   symbolrate_ = 0;
   pol_ = ' ';
   fec_ = 0;
}

cSatTransponder::cSatTransponder(int Frequency, char Pol, int SymbolRate, int FEC)
:cTransponder(Frequency)        
{
   channelNum_ = 0;
   symbolrate_ = SymbolRate;
   pol_ = Pol;
   fec_ = FEC;
   DEBUG_TRANSPONDER(DBG " new cSatTransponder(f: %d,p: %c,sRate: %d,fec %d", frequency_, pol_, symbolrate_, fec_);
}

bool cSatTransponder::SetTransponderData(cChannel *c, int Code)
{
   DEBUG_TRANSPONDER(DBG " SetSatTransponderData(s:%d,f:%d,p:%c,sRate:%d,fec%d", Code, frequency_, pol_, symbolrate_, fec_);
   return c->SetSatTransponderData(Code, frequency_, pol_, symbolrate_, fec_);
}

 
bool cSatTransponder::Parse(const string& Line)
{
   string tpNumb = Line;

   int index = tpNumb.find_first_of('=');
   if (index == -1)
      return false;

   tpNumb =  tpNumb.erase(0,index+1);

    //   chop  string
   string tmp = Line.substr(index+1);

    // get Frequenz
   index = tmp.find_first_of(',');

   if (index == -1)
      return false;

   string freq = tmp;
   freq.erase(index);

    // get polarisation
   string polar =  tmp.substr(index+1);
   index = polar.find_first_of(',');
   if (index == -1)
       return false;

    // get symbol rate
   string symRate =  polar.substr(index+1);
   polar.erase(index);

   index = symRate.find_first_of(',');
   if (index == -1)
       return false;

   string sFec = symRate.substr(index+1);
   symRate.erase(index);

   channelNum_ = strToInt(tpNumb.c_str());
   frequency_ = strToInt(freq.c_str());
   if (frequency_ == -1) return false;
   pol_ = polar[0];
   symbolrate_ = strToInt(symRate.c_str());
   if (symbolrate_  == -1) return false;
   fec_ = IntToFec(strToInt(sFec.c_str()));

   dsyslog (" transp.c Parse()  return true f:%d p%c sRate %d fec %d ", frequency_,pol_,symbolrate_,fec_); 
   
   return true;
}

#endif // M750S

//----------- Class cTerrTransponder -------------------------------

cTerrTransponder::cTerrTransponder(int ChannelNr, int Frequency, int Bandwith)
:cTransponder(Frequency)
{
   channelNum_ = ChannelNr;
   symbolrate_ = 27500;
   bandwidth_ = Bandwith;
   // fec is called Srate in vdr 
   fec_h_ = FEC_AUTO;
   fec_l_ = FEC_AUTO;
   hierarchy_ = HIERARCHY_NONE;
   modulation_ = FE_OFDM;
   guard_ =  GUARD_INTERVAL_AUTO;
   transmission_ = TRANSMISSION_MODE_AUTO;
}

cTerrTransponder::~cTerrTransponder()
{
}

bool cTerrTransponder::SetTransponderData(cChannel *c, int Code)
{
   
   int type = cSource::stTerr;

   if (bandwidth_==0) 
   {
     if (frequency_>=306*1000*1000)
           bandwidth_=BANDWIDTH_8_MHZ;
     else
           bandwidth_=BANDWIDTH_7_MHZ;
   }   
   else if (bandwidth_==1)
       bandwidth_=BANDWIDTH_7_MHZ;
   else
       bandwidth_=BANDWIDTH_8_MHZ;
       
   return c->SetTerrTransponderData(type, frequency_, bandwidth_, modulation_, hierarchy_, fec_h_,
                                     fec_l_, guard_, transmission_);//
   
}
#ifdef M750S
//----------- Class cCableTransponder -------------------------------

cCableTransponder::cCableTransponder(int ChannelNr, int Frequency, int Bandwith, int sRate, int Mod)
:cTransponder(Frequency)
{
  int modTab[6]={QAM_64,QAM_256,QPSK,QAM_16,QAM_32,QAM_128};
   // QAM_64 is Auto QAM64/QAM256

  DEBUG_TRANSPONDER(DBG  " new cCableTransponder Channel: %d f: %d, sRate :%d  BW %d ",ChannelNr, Frequency, sRate, Bandwith); 
    
  channelNum_ = ChannelNr;
  symbolrate_ = sRate;
  bandwidth_ = Bandwith;

  fec_h_ = FEC_AUTO;
  hierarchy_ = HIERARCHY_NONE;

  modulation_ = modTab[Mod];
}

cCableTransponder::~cCableTransponder()
{
}

bool cCableTransponder::SetTransponderData(cChannel *c, int Code)
{
   int type = cSource::stCable;

   DEBUG_TRANSPONDER(DBG " SetCableTransponderData(f:%d, m :%d ,sRate: %d, fec %d", frequency_,modulation_, symbolrate_,fec_h_);
   return c->SetCableTransponderData(type, frequency_, modulation_, symbolrate_, fec_h_);
}
#endif

//----------- Class Transponders -------------------------------

cTransponders::cTransponders()
:sourceCode_(0)
{
} 

void cTransponders::Load(int Source, scanParameters *scp)
{

  DEBUG_TRANSPONDER(DBG "  %s \n", __PRETTY_FUNCTION__);
  Clear();

  sourceCode_ = Source;

#ifdef M750S
  if (cSource::IsSat(sourceCode_))
  {
     lockMs_ = 500;
     if (scp->frequency == 2 ) { // NIT
          LoadNitTransponder(sourceCode_);
      }
      else if (scp->frequency == 1) { //AUTO 
          fileName_ = TplFileName(Source);
          position_ = SetPosition(fileName_);
          LoadTpl(fileName_);
      }
      else {
          cSatTransponder *t = new cSatTransponder(scp->frequency, scp->polarization, scp->symbolrate, scp->fec);
          v_tp_.push_back(t);
      }
      
  }
  else if (cSource::IsTerr(sourceCode_))
  {
#endif
     position_ = "Terrestrial";
     //if (scp->frequency == 0 || scp->frequency == -1) 
     if (scp->frequency == 1)
          CalcTerrTpl();
     else {
          int channel=0;
          cTerrTransponder *t = new cTerrTransponder(channel, scp->frequency*1000,  scp->bandwidth);
          v_tp_.push_back(t);
      }
#ifdef M750S
  }
  else if (cSource::IsCable(sourceCode_))
  {
     position_ = "Cable";
     if (scp->frequency == 1)
          CalcCableTpl(0, scp);
      else {
          int channel=0;
          int sRate;
          if (scp->symbolrate_mode==2)
              sRate=scp->symbolrate;
          else
              sRate=6900;

          cCableTransponder *t = new cCableTransponder(channel, scp->frequency*1000, 
                                   scp->bandwidth?BANDWIDTH_7_MHZ:BANDWIDTH_8_MHZ, 
                                   sRate, scp->modulation);
          v_tp_.push_back(t);  
      }
  }
  else 
    esyslog(DBG "   Wrong  sourceCode %d",sourceCode_);
#endif
  DEBUG_TRANSPONDER(DBG "  %s end \n", __PRETTY_FUNCTION__);
}

#ifdef M750S
void cTransponders::Add(int Source, const scanParameters& scp)
{
  // type S/T/C
  sourceCode_ = Source;

  if (cSource::IsSat(sourceCode_))
  {
     cSatTransponder *t = new cSatTransponder(scp.frequency, scp.polarization, scp.symbolrate, scp.fec);
     v_tp_.push_back(t);
  }
      
}

bool cTransponders::LoadNitTransponder(int Source)
{

  DEBUG_TRANSPONDER(DBG " %s ... \"%d\" \n", __PRETTY_FUNCTION__, Source);
  //bool found = false;
  int found = 0;

  fileName_ = TplFileName(0);

#ifdef DEBUG_CHANNELSCAN
  printf (" load filename   %s  \n", fileName_.c_str()); 
#endif

  ifstream tpStream(fileName_.c_str());
  position_ = *cSource::ToString(Source);

  if (!tpStream || tpStream.bad() || tpStream.fail())
  {
    esyslog ("ERROR  in LoadSatTpls %s",fileName_.c_str());
    return false;
  }

  char line[256];
  int lc = 0;

  while (tpStream.getline(line,200))
  {

    // strip Space 
    uint k = 0;
    while (k<= strlen(line))
    {
       if (line[k] != ' ')
           break;
       k++;
    }
    string l = &line[k];

    // skip lines with #
    if (l.find('#') == 0)
    {
       continue;
    }

    if (l.find(position_) == 0)
    {
       // second entry  
       if (found == lc-1)
       {
#ifdef DEBUG_CHANNELSCAN
          printf (" \033[0;48m  found second entry at line %d: %s \033[0m\n",lc, l.c_str());
#endif
          cSatTransponder *t = new cSatTransponder();
          if (t->Parse(l)) {
#ifdef DEBUG_CHANNELSCAN
              printf ("\033[0;44m DEBUG [transponders]: add : %d  \033[0m \n", t->Frequency());
#endif
              v_tp_.push_back(t);
          }
       }
       else 
       {
          found = lc;
#ifdef DEBUG_CHANNELSCAN
          printf (" \033[0;43m  found first entry at line %d: %s\033[0m\n",lc, l.c_str());
#endif
          nitStartTransponder_.reset(new cSatTransponder);
          nitStartTransponder_->Parse(l);
       }
    }
    else 
    {
       if (found > 0 && found == lc-1)
          return true;
    }
    
    lc++;
  }
  
  if (!found);
  {
     esyslog ("ERROR: [channelscan] in  %s :  no values for \"%s\"",fileName_.c_str(), position_.c_str());
  }
  return found;

}


bool cTransponders::LoadTpl(const string& tplFileName)
{

  lockMs_ = 500;
  DEBUG_TRANSPONDER(DBG " in LoadSatTpls %s",tplFileName.c_str());

  sourceCode_ = cSource::FromString(position_.c_str());

  ifstream tpFH(tplFileName.c_str());

  if (!tpFH)
  {
    esyslog ("ERROR: [channelscan] in LoadSatTpls %s",tplFileName.c_str());
    return false;
  }

   char line[200];
   int lc = 0;
   while (tpFH.getline(line,200))
   {
     string::size_type  pos = 0;
     string l = line;
       // lookup for [
     pos =l.find('[');

     if (pos == l.npos)
     {
       cSatTransponder *t = new cSatTransponder();
       if (t->Parse(l))
          v_tp_.push_back(t);
     }
     lc++;
  }
  return true;
}

void cTransponders::CalcCableTpl(bool Complete, scanParameters *scp) 
{
   int bandwidth;
   int f, channel=0;
   int sRate, qam;

   Clear();

   if (scp->symbolrate_mode==2)
       sRate=scp->symbolrate;
   else
       sRate=6900;

   qam=scp->modulation;

   // Give the user the most popular cable channels first, speeds up SR auto detect
   for(channel=121;channel<200;channel++) {
       f=channel2Frequency(0,channel,bandwidth);
       if (f) {
           cCableTransponder *t = new cCableTransponder(channel, f, bandwidth, sRate , qam);
           v_tp_.push_back(t);
       }
   }

   for(channel=101;channel<121;channel++) {
       f=channel2Frequency(0,channel,bandwidth);
       if (f) {
           cCableTransponder *t = new cCableTransponder(channel, f, bandwidth, sRate , qam);
           v_tp_.push_back(t);
       }
   }

   for(channel=1;channel<100;channel++) {
       f=channel2Frequency(0,channel,bandwidth);
       if (f) {
           cCableTransponder *t = new cCableTransponder(channel, f, bandwidth, sRate , qam);
           v_tp_.push_back(t);
       }
   }

}
#endif // M750S

void cTransponders::CalcTerrTpl()
{
   Clear();
   int f; 
   int channel;
   int bandwidth;
   
   position_ = "Terrestrial";

   for (channel=5; channel <= 69; channel++) 
   {
       f = channel2Frequency(0,channel,bandwidth);
       if (f) {
           cTerrTransponder *t = new cTerrTransponder(channel, f, bandwidth);
           v_tp_.push_back(t);
       }
   } 
}

#ifdef M750S
string cTransponders::SetPosition(const string& tplPath)
{
  if(cSource::IsSat(sourceCode_))
  { 
     string tmp = fileName_.substr(tplPath.find_last_of('/')+1);
     int index = tmp.find_last_of('.');
     tmp.erase(index);
     return tmp;
  }
  if(cSource::IsTerr(sourceCode_))
     return "DVB-T";

  if(cSource::IsCable(sourceCode_))
     return "DVB-C";

  return "";
}

string cTransponders::TplFileName(int satCodec)
{
  string tmp = cPlugin::ConfigDirectory();
  tmp += "/transponders/";
  if (satCodec == 0)
  {
     tmp += "Sat";
  }
  else 
  {
     tmp += *cSource::ToString(satCodec);
  }
  tmp += ".tpl";
  return tmp;
}
#endif

void cTransponders::Clear()
{
   for (TpIter iter = v_tp_.begin(); iter != v_tp_.end(); ++iter)
   {
      delete *iter;
      *iter = NULL;
   }
   
    v_tp_.clear();
}

bool cTransponders::MissingTransponder(int Transponder)
{
   for (constTpIter iter = v_tp_.begin(); iter != v_tp_.end(); ++iter)
   { 
       if (Transponder == (*iter)->Frequency())
            return false;
   }
   return true;
}

