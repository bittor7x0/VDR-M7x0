/*
 * channelscan.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */
/*
 * csmenu.c channelscan menu
 * written by reel-multimedia
 *
 * See README for copyright information and
 * how to reach the authors.
 *
 */

#include <iostream>

#include <linux/dvb/frontend.h> 
#include <vdr/interface.h>
#include <vdr/diseqc.h> 
#include <vdr/menu.h> 

#include "csmenu.h"
#include "filter.h"


#define CHNUMWIDTH 16
#define DVBFRONTEND "/dev/dvb/adapter%d/frontend0"
#define POLLDELAY 1
#define MENUDELAY 3

using std::endl;
using std::string;
using std::cout;
using std::cerr;
using std::vector;
using std::ofstream;

// XXX check all static vars!
typedef vector<int>::const_iterator iConstIter;

vector<string> tvChannelNames;
vector<string> radioChannelNames;
vector<string> dataChannelNames;

#ifdef DEBUG_CHANNELSCAN
vector<string> tvChannelList;
vector<string> radioChannelList;
vector<string> dataChannelList;
#endif 

cMutex mutexNames;
cMutex mutexScan;

volatile int cMenuChannelscan::scanState = ssInit;

int cMenuChannelscan::sourceStat = 0; // XXX
cTransponders *cTransponders::instance_ = NULL;

#define SAT   0
#define CABLE 1
#define TERR  2
 
int pollCount;

// --- Class cMenuChannelscan ------------------------------------------------

cMenuChannelscan::cMenuChannelscan(int CurrentChannelNr)
:cOsdMenu(tr("Channelscan"), CHNUMWIDTH)
{

#ifdef REELVDR
  ::Setup.LiveBuffer=0;
#endif
  
  data = Setup;
  cTransponders::Create();
  
  frequency = 12551;
  symbolrate = 22000;
  polarization = 'V';
  detailedScan = 0;

  scanState = 0;
  searchMode=0;
  expertSettings = 0;
  srcTuners=0;
  TunerDetection(); 
  // case TERR: DVB-T - Terrestrial"),tr("Tuner) .... break; 

  fecTexts[0] = tr("None");
  fecTexts[1] = "1/2";
  fecTexts[2] = "2/3";
  fecTexts[3] = "3/4";
  fecTexts[4] = "4/5";
  fecTexts[5] = "5/6";
  fecTexts[6] = "6/7";
  fecTexts[7] = "7/8";
  fecTexts[8] = "8/9";
  fecTexts[9] = tr("Auto");

  fecStat = 5;
  //modTexts[0] = "QAM-16";
  //modTexts[1] = "QAM-32";
  
  modTexts[0] = "Auto 64QAM/256QAM";
  modTexts[1] = "256QAM";
  modTexts[2] = "4QAM/QPSK";
  modTexts[3] = "16QAM";
  modTexts[4] = "32QAM";
  modTexts[5] = "128QAM";

  modStat = 0;

  searchTexts[0] = tr("Manual");
  searchTexts[1] = tr("SearchMode$Auto");
  searchTexts[2] = tr("SearchMode$Auto NIT");
   
  searchStat = 2;

  bandwidth = 0; // 0: 7MHz 1: 8MHz

  sRate6875 = 0;
  sRateItem[2] = "6111";

  sBwItem[0] = "Auto";
  sBwItem[1] = "7 MHz";
  sBwItem[2] = "8 MHz";

  sSRScanMode[0]= tr("Intelligent 6900/6875/6111");
  sSRScanMode[1]= tr("Try all 6900/6875/6111");
  sSRScanMode[2]= tr("Manual");

  srScanMode=0;
  
  // CurrentChannel has to be greater than 0!
  currentChannel = CurrentChannelNr == 0 ? 1 : CurrentChannelNr;
  cChannel *channel = Channels.GetByNumber(currentChannel);

  source = 0;
  if (channel) 
  {
      source = channel->Source();
  }

  if (source == 0)
  {
     if (Setup.DiSEqC > 0)
     {
       cDiseqc *d = Diseqcs.First();
       if (d) 
       { 
         source = d->Source();
       }
       else 
       {
         source = d->Source();
       }
     }
  }

  if (cPluginChannelscan::AutoScanStat != AssNone)
  {
      switch (cPluginChannelscan::AutoScanStat) 
      {
            case AssDvbS:
                 sourceType = SAT;
                 searchStat = 2; // 2: NIT scan, 1 OLD Atuo 
                 break;
            case AssDvbC: 
                 sourceType = CABLE;  
                 searchStat = 1; 
                 break;
            case AssDvbT: 
                 sourceType = TERR;
                 searchStat = 1;
                 break;
            default: 
                 cPluginChannelscan::AutoScanStat = AssNone;
                 esyslog ("Channelscan service handling error"); //??
                 break;
       }
       cRemote::Put(kRed);
  }
  Set();
}

void cMenuChannelscan::TunerDetection()
{

  int stp=0;
  for (int i=0;i<4;i++) 
  {
    cDevice *device = cDevice::GetDevice(i);
    srcTexts[i]=NULL; 
    if (device) {
      char *txt=NULL;
      if (device->ProvidesSource(cSource::stTerr)) {
         asprintf(&txt,"%s (%s %i)",tr("DVB-T - Terrestrial"),tr("Tuner"),i+1);
         stp=TERR;
      }
      else if (device->ProvidesSource(cSource::stCable)) {
         asprintf(&txt,"%s (%s %i)",tr("DVB-C - Cable"),tr("Tuner"),i+1);
         stp=CABLE;
      }
      else if (device->ProvidesSource(cSource::stSat)) {
         asprintf(&txt,"%s (%s %i)",tr("DVB-S - Satellite"),tr("Tuner"),i+1);
         stp=SAT;
      }
      if (txt) {
         srcTypes[srcTuners]=stp;
         srcTexts[srcTuners++]=txt;
      }
    }
  }            
}

void cMenuChannelscan::InitLnbs()
{
   lnbs = 0;
   for(cDiseqc *diseqc = Diseqcs.First(); diseqc; diseqc=Diseqcs.Next(diseqc))
   {
      if (diseqc != Diseqcs.First() && diseqc->Source() == Diseqcs.Prev(diseqc)->Source() )
          continue;
      
      printf (" --Menu --- Diseqc Sources  %d --- \n",   diseqc->Source());
      loopSources.push_back(diseqc->Source());
      lnbs++;
   }
}


void cMenuChannelscan::Set()
{    
  int current = Current();

  Clear();  

  sourceType=srcTypes[sourceStat];

  // avoid C/T-positions for SAT 
  // and take Astra as start position
  if (srcTypes[sourceStat]==SAT  && (source==cSource::FromString("C") || source==cSource::FromString("T")))
    source = cSource::FromString("S19.2E"); 

   int blankLines = 4;
  
   // for DVB-T or DVB-C   
   // we loop through all satalites
   /* TODO  aktivate loop mode again!
   if (loopMode && ::Setup.DiSEqC)
   {
      Add(new cMenuInfoItem("Scanning configured satellites"));
      AddBlankLineItem(4);
   }


   */
   Add(new cMenuEditBoolItem(tr("Setup.DVB$Update channels"),           &data.UpdateChannels, tr("no"), tr("yes") ));

#if 0
   Add(new cOsdItem(hk(tr("Select channel list")),   osUser1));
   // To Select Channel list 
   Add(new cMenuEditStraItem(tr("Setup.DVB$Update channels"),       &data.UpdateChannels, 6, updateChannelsTexts));
#endif

   Add(new cMenuEditStraItem(tr("Source"), &sourceStat, srcTuners, srcTexts));
      
   switch (sourceType) {
      case SAT:
         Add(new cMenuEditStraItem(tr("Search Mode"), &searchStat, 3, searchTexts)); 
         Add(new cMenuEditSrcItem(tr("Position"), &source)); //XXX all sources in Diseqc? 
         break;
      case TERR:
         Add(new cMenuEditBoolItem(tr("Detailed search"), &searchMode, tr("no"),tr("yes")));
         Add(new cMenuEditBoolItem(tr("Search Mode"), &searchStat, tr("Manual"),  tr("SearchMode$Auto") ));      
        break;
      case CABLE:
         Add(new cMenuEditBoolItem(tr("Search Mode"), &searchStat, tr("Manual"),  tr("SearchMode$Auto") ));
         break;
      }
      
    AddBlankLineItem(1);
      
    if (!searchStat)
    {
       switch (sourceType)
       { 
          case SAT:
                  frequency = 12551;
                  symbolrate = 22000;  // reset right default values
                  Add(new cMenuEditIntItem(tr("Frequency (MHz)"), &frequency));  
                  Add(new cMenuEditChrItem(tr("Polarization"),&polarization, "HVLR"));
                  Add(new cMenuEditIntItem(tr("Symbolrate"), &symbolrate));
                  Add(new cMenuEditStraItem("FEC", &fecStat, 10, fecTexts ));
                  break;          
          
          case TERR:
                  frequency = 212500; // reset right default values
                  Add(new cMenuEditIntItem(tr("Frequency (kHz)"), &frequency));  
                  Add(new cMenuEditStraItem(tr("Bandwidth"), &bandwidth, 3, sBwItem));

                  AddBlankLineItem(2);
                  break;
          case CABLE:
                 frequency = 113000; // reset right default values 
                 Add(new cMenuEditIntItem(tr("Frequency (kHz)"), &frequency));  
                 Add(new cMenuEditBoolItem(tr("Bandwidth"), &bandwidth, "7 MHz", "8 MHz"));
                 Add(new cMenuEditIntItem(tr("Symbolrate"), &symbolrate));
                 Add(new cMenuEditStraItem(tr("Modulation"), &modStat, 6, modTexts));
          default: esyslog ("ERROR in %s:%d\n", __FILE__, __LINE__);
                  break;
       }
    }
    else 
    {
       AddBlankLineItem(4);
    }
   
   // TODO  AllignButtom()
   if (::Setup.UseSmallFont || strcmp(::Setup.OSDSkin,"Reel") == 0)
   {
      blankLines -= 1;
   }

   AddBlankLineItem(blankLines);

   //Check this 
   SetInfoBar();

#ifdef REELVDR
   SetHelp(tr("Button$Start"), NULL, NULL, tr("Channel sel.") );
#else 
   SetHelp(tr("Button$Start"), NULL, NULL, NULL);
#endif
   
   SetCurrent(Get(current));
   Display();
}

void cMenuChannelscan::SetInfoBar() // Check with  cMenuScanActive
{

   printf (" Menus --- %s -- scanState %d ---  \n", __PRETTY_FUNCTION__, scanState);

   switch (scanState) 
   {
      case ssInit:
           if (sourceType==SAT) 
           {
             Add(new cMenuInfoItem(tr("DiSEqC"), static_cast<bool>(::Setup.DiSEqC))); 
             if (::Setup.DiSEqC) DiseqShow();  // TODO  two columns display
           }
           break;
      case ssInterrupted:
           Add(new cMenuInfoItem(tr("Scanning aborted")));
           break;
      case ssSuccess:
           Add(new cMenuInfoItem(tr("Scanning succed")));
           break;
      case ssNoTransponder:
           Add(new cMenuInfoItem(tr("Retrieving transponders failed!")));
           Add(new cMenuInfoItem(tr("Please check satellites.conf.")));
           break;
      case ssNoLock:
           Add(new cMenuInfoItem(tr("Tuner  preserved no lock!")));
           Add(new cMenuInfoItem(tr("Please check your comnections and satellites.conf.")));
           break;
      case ssFilterFailure:
           Add(new cMenuInfoItem(tr("Unpredictable error occured!")));
           Add(new cMenuInfoItem(tr("Please try again later")));
           break;
      default:  
           break;
   }
   Add(new cMenuInfoItem(tr("channels in current list"), Channels.MaxNumber()));
}

void cMenuChannelscan::Store()
{
  if (data.UpdateChannels>0)
     data.UpdateChannels = 5;

  Setup = data;
  Setup.Save();

} 

eOSState cMenuChannelscan::ProcessKey(eKeys Key)
{
  bool HadSubMenu = HasSubMenu();

  int oldDetailedScan = detailedScan;
  int oldSearchStat =searchStat;

  int oldSourceStat = sourceStat;
  int oldSRScanMode = srScanMode;
  bool oldExpertSettings = expertSettings;

  eOSState state = cOsdMenu::ProcessKey(Key);
  
  #ifdef REELVDR 
  if (state == osUser1) 
  {
     return AddSubMenu(new cMenuSelectChannelList); 
  }
  #endif // REELVDR 

  if (HadSubMenu && !HasSubMenu())
  {
     Set(); 
  }

  if (state == osUnknown && (!HadSubMenu || !HasSubMenu()))
  {
   
    sourceType=srcTypes[sourceStat];
  
    scp.card = sourceStat;
    scp.type=sourceType;
    scp.source = 
    scp.bandwidth = bandwidth;
    scp.polarization = polarization;
    scp.symbolrate = symbolrate;
    scp.fec = fecStat;
    scp.detail = searchStat | (searchMode<<1); // searchMode=1 -> DVB-T offset search
    scp.modulation = modStat;
    scp.symbolrate_mode = srScanMode;

    switch(Key)
    {
       case kOk:
            Store();            
            SwitchChannel();
            return osContinue;
       case kRed:
            source = sourceType==1?0x4000:sourceType==2?0xC000:source;
            // searchStat == 0: manual scan 
            // searchStat == 1: autoscan; take transponder list
            // searchStat == 2: autoscan fetch NIT from satellite

            scp.frequency = searchStat?searchStat:frequency;
            cTransponders::GetInstance().Load(source, &scp); 
            
            return AddSubMenu(new cMenuScanActive(&scp));     
       case kGreen: //kYellow:
            if (sourceType == 1)
            {
               expertSettings  = expertSettings?false:true;
               Set();
            }
            break;
#ifdef REELVDR
        case kBlue: 
             return AddSubMenu(new cMenuSelectChannelList);     
             break;
#endif 
        default: state = osContinue;
     }
  } 

  // forces setup if menu layout should be changed
  if (Key != kNone && !HadSubMenu)
  {
     if (oldDetailedScan != detailedScan ||
         oldSearchStat !=searchStat ||
         oldExpertSettings != expertSettings || 
         oldSourceStat != sourceStat ||
         oldSRScanMode != srScanMode)
     {
        if (sourceType == CABLE && symbolrate>8000)  // avoid DVB-S symbolrates in DVB-C
        {
           symbolrate=6900;
        }
        oldSourceStat = sourceStat;
        Set();
     }
  }
  return state;
}

void cMenuChannelscan::DiseqShow()
{
  for (iConstIter iter = loopSources.begin(); iter != loopSources.end(); ++iter)
  {
     char buffer[256];

     snprintf(buffer,sizeof(buffer),"LNB %c: %s", 'A'+ (int) (iter-loopSources.begin()), *cSource::ToString(*iter));
     //DLOG ("Show fetch Source [%d] %d %s ", (int) iter-loopSources.begin() , *iter, buffer);
     Add(new cMenuInfoItem(buffer));
  }
} 

cMenuChannelscan::~cMenuChannelscan()
{
  for(int i=0;i<srcTuners;i++) {
     if (srcTexts[i]) {
        free(srcTexts[i]);
    }
  }

  if (HasSubMenu()) {
    CloseSubMenu();
  }

  tvChannelNames.clear();
  radioChannelNames.clear();
  dataChannelNames.clear();
  cPluginChannelscan::AutoScanStat = AssNone;
  cTransponders::Destroy();
}

// taken fron vdr/menu.c
void cMenuChannelscan::AddBlankLineItem(int lines)
{
   for(int i = 0;i < lines;i++)
   {
      cOsdItem *item = new cOsdItem;
      item->SetSelectable(false);
      item->SetText(strndup(" ", 1), false);
      Add(item);
   }
}

void cMenuChannelscan::SwitchChannel()
{
   
   Channels.SwitchTo(currentChannel);
   /*
   cChannel *c = GetByNumber(currentChannel);
   if (c)
   {
       device->SwitchChannel(c,true);
   }
   */

}

// --- cMenuScanActive -------------------------------------------------------
#ifndef DBG  
#define DBG "DEBUG [cMenuScanActive]: "
#endif

cMenuScanActive::cMenuScanActive(scanParameters *sp)
:cOsdMenu(tr("TV CHANNELS                     RADIO"), 25)
{
 
 Channels.IncBeingEdited();

#ifdef DEBUG_CHANNELSCAN
  tvChannelList.clear();
  radioChannelList.clear();
  dataChannelList.clear();
#endif
  scp=sp;

  DLOG(DBG " cMenuScanActive Freq %d  --- ", scp->frequency);

  oldUpdateChannels = ::Setup.UpdateChannels;
  ::Setup.UpdateChannels = 0; // prevent  VDRs own update Channel  

  oldChannelNumbers = Channels.MaxNumber();

  // Make class 
  tvChannelNames.clear();
  radioChannelNames.clear();

  if (cTransponders::GetInstance().GetNITStartTransponder())
  {
    cMenuChannelscan::scanState=ssGetTransponders;
  }
  // auto_ptr
  Scan.reset(new cScan());
  
  isyslog (" start Scanning @ Card %d --- ", scp->card);
  if (!Scan->StartScanning(scp))
  {
     esyslog (ERR "  Tuner Error");
     cMenuChannelscan::scanState=ssInterrupted;
     //cMenuChannelscan::scanning == false;
  }
  Setup();
  Channels.Save();
}

void cMenuScanActive::Setup()
{
  int num_tv=0,num_radio=0;

  Clear();
  mutexNames.Lock();

  vector<string>::iterator tv;
  vector<string>::iterator radio;

  tv = tvChannelNames.begin();
  radio = radioChannelNames.begin();

  num_tv=tvChannelNames.size();
  num_radio=radioChannelNames.size();

  if (tvChannelNames.size() > 10)
     tv+=tvChannelNames.size()-10;

  if (radioChannelNames.size() > 10)
     radio+=radioChannelNames.size()-10;

  /// Display channel names 
  while (1)
  {
     if (tv == tvChannelNames.end() &&  radio == radioChannelNames.end()) break;

     cMenuScanActiveItem *Item = new cMenuScanActiveItem(tv == tvChannelNames.end()?"":tv->c_str(),
                               radio == radioChannelNames.end()?"":radio->c_str());
     Add(Item);

     if (tv != tvChannelNames.end()) {
      ++tv;
     }
     if (radio != radioChannelNames.end()) {
      ++radio;
     }
  }
  int nameLines = tvChannelNames.size() > radioChannelNames.size() ? tvChannelNames.size() : radioChannelNames.size();

  if (nameLines>10)
     nameLines=10;

  mutexNames.Unlock();

  AddBlankLineItem(11 - nameLines);

  char buffer[50];

  if (cMenuChannelscan::scanState == ssGetTransponders)
  {
     AddBlankLineItem(1);
  }
  else  
  {
     snprintf(buffer,50, "TV: %i \tRadio: %i ", num_tv,num_radio);
     Add(new cMenuInfoItem(buffer));
     transponderNum_ = cTransponders::GetInstance().v_tp_.size();
  }

  //If Scanning // TODO Make Switch Case  in Status() function

  if (cMenuChannelscan::scanState <= ssGetChannels)
  {
     char *txts[3]={"TV only","Radio only","Radio + TV"};

     if (cMenuChannelscan::scanState == ssGetTransponders)
     {
        snprintf(buffer,sizeof(buffer), tr("Retrieving transponder list from %s"), 
        cTransponders::GetInstance().Position().c_str());
        Add(new cMenuStatusBar(100, Scan->GetCurrentTransponderNr()*2, 0, true));
          //cMenuStatusBar(int Total, int Current, int Channel, bool BarOnly)
     }
     else if (cMenuChannelscan::scanState == ssGetChannels)
     {
       if (scp->type==SAT) 
       {
          snprintf(buffer,sizeof(buffer), tr("Scanning %s (%iMHz)\t%s"), cTransponders::GetInstance().Position().c_str(), 
             Scan->GetCurrentFrequency(),tr(txts[cMenuChannelscanSetup::SetupServiceType]));
          Add(new cMenuStatusBar(transponderNum_, Scan->GetCurrentTransponderNr(), Scan->GetCurrentChannelNumber()));
       }
       else
       {
          snprintf(buffer,sizeof(buffer), tr("Scanning %s (%.3fMHz)\t%s"), tr(cTransponders::GetInstance().Position().c_str()), 
             Scan->GetCurrentFrequency()/(1000.0*1000),tr(txts[cMenuChannelscanSetup::SetupServiceType]));
         Add(new cMenuStatusBar(transponderNum_, Scan->GetCurrentTransponderNr(), Scan->GetCurrentChannelNumber()));
       }
     }
     Add(new cMenuInfoItem(buffer));
  }
  else if (cMenuChannelscan::scanState >= ssInterrupted)
  {
    if (cMenuChannelscan::scanState == ssSuccess)
    { 
       if ((Channels.MaxNumber() > 1) && Channels.MaxNumber() == oldChannelNumbers)
       Add(new cMenuInfoItem(tr("No new channels found")));
    }
    else if (Channels.MaxNumber() > oldChannelNumbers) 
    {
       Add(new cMenuInfoItem(tr("Added new channels"), (Channels.MaxNumber() - oldChannelNumbers)));
    }
    Add(new cMenuInfoItem(""));
    if (cPluginChannelscan::AutoScanStat == AssNone)
       Add(new cMenuInfoItem(tr("Press OK to finish or Exit for new scan")));
    ErrorMessage();
  }
  Display();
}

// show this only cMenuChannelscan
void cMenuScanActive::ErrorMessage()
{
   if (cMenuChannelscan::scanState >= ssInterrupted)
   {
   
     /*
     if (cPluginChannelscan::AutoScanStat != AssNone)
     {
         cPluginChannelscan::AutoScanStat = AssNone;
         cRemote::CallPlugin("install");       
     }
     */
     switch (cMenuChannelscan::scanState)
     {
         case ssInterrupted: Skins.Message(mtInfo, tr("Scanning aborted")); break;
         // Scanning aborted by user
         /*
         case ssDeviceFailure: Skins.Message(mtError, tr("Tuner error! ")); break;
         // missing device file etc.
         case ssNoTransponder: Skins.Message(mtError, tr("Missing parameter")); break;
         // missing transponderlist due to wrong parameter 
         case ssNoLock: Skins.Message(mtError, tr("Tuner error!")); break;
         // reciver error 
         case ssFilterFailure: Skins.Message(mtError, tr("DVB services error!")); break;
         // DVB SI or filter error 
         */
         default: break;
     }
   }
}

void cMenuScanActive::DeleteDummy()
{
   printf (" --- %s --- %d -- \n", __PRETTY_FUNCTION__, Channels.Count());
   
   if (Channels.Count()< 3) return; 

   cChannel *channel = Channels.GetByNumber(1); 
   if (strcmp(channel->Name(),"ReelBox") == 0) {
       Channels.Del(channel);
   }
   Channels.ReNumber();
}

eOSState cMenuScanActive::ProcessKey(eKeys Key)
{

  //if (cPluginChannelscan::AutoScanStat != AssNone && !cMenuChannelscan::scanning)
  if (cPluginChannelscan::AutoScanStat != AssNone && cMenuChannelscan::scanState >= ssInterrupted)
  {
     cPluginChannelscan::AutoScanStat = AssNone;
     return osEnd;
  }

  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
     switch (Key) 
     {
        case kOk: 
             if (cMenuChannelscan::scanState >= ssInterrupted) 
             {
                cMenuChannelscan::scanState = ssInit; 
                Channels.Save();
                cRemote::Put(kChannels); 
                return osEnd; 
             }
             else 
             {
                state = osContinue;
             }
             break;
        case kBack: // 
             // shut down scanning 
             cMenuChannelscan::scanState = ssInterrupted; 
             //cMenuChannelscan::scanning = false; 
             return osBack;
        default: state = osContinue;
     }
  Setup();
  }
    
  return state;
}
void cMenuScanActive::AddBlankLineItem(int lines)
{
   for(int i = 0;i < lines;i++)
   {
      cOsdItem *item = new cOsdItem;
      item->SetSelectable(false);
      item->SetText(strndup(" ", 1), false);
      Add(item);
   }
}

cMenuScanActive::~cMenuScanActive()
{
  Scan->ShutDown();

  tvChannelNames.clear();
  radioChannelNames.clear();
  dataChannelNames.clear();
  // XXX
  //cMenuChannelscan::scanning = false;
  cMenuChannelscan::scanState = ssInterrupted; 
  scanning_on_receiving_device = false;
  ::Setup.UpdateChannels = oldUpdateChannels;

  // call cMenuChannels if kOk 
  DeleteDummy();

#ifdef DEBUG_CHANNELSCAN

   #include <sstream> 
   #include <fstream> 
   #include <vector> 
   #include <string> 

   typedef vector<string>::const_iterator constIter;
   std::ofstream outfile("/tmp/tv.txt");
   cout << " atempt to write " << dataChannelList.size() << " entries to /tmp/tv.txt \n";
   constIter it = tvChannelList.begin();
   while (it != tvChannelList.end())
   {
     outfile << it - tvChannelList.begin() << ": " << *it << endl;
     ++it;
   }

   outfile.close();
   outfile.open("/tmp/radio.txt");
   cout << " atempt to write " << dataChannelList.size() << " entries to /tmp/radio.txt \n";
   it = radioChannelList.begin();
   while (it != radioChannelList.end())
   {
     outfile << it - radioChannelList.begin() << ": " << *it << endl;
     ++it;
   }

   outfile.close();
   outfile.open("/tmp/data.txt");
   cout << " atempt to write " << dataChannelList.size() << " entries to /tmp/data.txt \n";
   it = dataChannelList.begin();
   while (it != dataChannelList.end())
   {
     outfile << it - dataChannelList.begin() << ": " << *it << endl;
     ++it;
   }
   outfile.close();
  
   tvChannelList.clear();
   radioChannelList.clear();
   dataChannelList.clear();
#endif

  Channels.DecBeingEdited();

  cChannel *channel = Channels.GetByNumber(1);
  if (channel) // && !scanning_on_receiving_device)
  {
     cDevice::PrimaryDevice()->SwitchChannel(channel,true);
     usleep(200*1000);
  }
}

// --- cMenuScanActiveItem ----------------------------------------------------
cMenuScanActiveItem::cMenuScanActiveItem(const char *TvChannel,const char *RadioChannel)
{
  tvChannel = strdup(TvChannel);
  radioChannel = strdup(RadioChannel);
  char *buffer = NULL;
  asprintf(&buffer, "%s \t%s", tvChannel, radioChannel);
  SetText(buffer, false);
  SetSelectable(false);
}

cMenuScanActiveItem::~cMenuScanActiveItem()
{
  free(tvChannel);
  free(radioChannel);
}

// cMenuEditSrcItem taken fron vdr/menu.c
// --- cMenuEditSrcItem ------------------------------------------------------

cMenuEditSrcItem::cMenuEditSrcItem(const char *Name, int *Value)
:cMenuEditIntItem(Name, Value, 0)
{
  source = Sources.Get(*Value);
  Set();
}

void cMenuEditSrcItem::Set(void)
{
  if (source) 
  {
    char *buffer = NULL;
    asprintf(&buffer, "%s - %s", *cSource::ToString(source->Code()), source->Description());
    SetValue(buffer);
    free(buffer);
  }
  else
  {
    cMenuEditIntItem::Set();
  }
}

eOSState cMenuEditSrcItem::ProcessKey(eKeys Key)
{
  eOSState state = cMenuEditItem::ProcessKey(Key);

  if (state == osUnknown) 
  {
    if (NORMALKEY(Key) == kLeft) // TODO might want to increase the delta if repeated quickly? 
    { 
       if (source && source->Prev()) 
       {
          source = (cSource *)source->Prev();
          *value = source->Code();
       }
    } 
    else if (NORMALKEY(Key) == kRight) 
    {
      if (source) 
      {
         if (source->Next()) 
           source = (cSource *)source->Next();
      }
      else
        source = Sources.First();

      if (source)
       *value = source->Code();
    }
    else
      return state; // we don't call cMenuEditIntItem::ProcessKey(Key) here since we don't accept numerical input

    Set();
    state = osContinue;
  }
  return state;
}

//taken from rotor rotor plugin

cMenuScanInfoItem::cMenuScanInfoItem(const string& Pos, int f, char pol, int sr, int fecIndex)
{

  const char *pos = Pos.c_str();
  char *buffer = NULL;
  asprintf(&buffer, "%s :\t%s %d %c %d %s",tr("Scanning on transponder"), pos, f, pol, sr, FECToStr(fecIndex));

  SetText(strdup(buffer), false);
  SetSelectable(false);
}
const char *cMenuScanInfoItem::FECToStr(int Index)
{
  switch (Index)
  {
     case 1: return "1/2";
     case 2: return "2/3";
     case 3: return "3/4";
     case 4: return "4/5";
     case 5: return "5/6";
     case 6: return "6/7";
     case 7: return "7/8";
     default : return tr("Auto");
  }
  return tr("Auto");
}

// --- cMenuStatusBar ----------------------------------------------------

cMenuStatusBar::cMenuStatusBar(int Total, int Current, int Channel, bool BarOnly)
{
   int barWidth = 50;
   int percent;
   Current +=1;

   if (Current > Total)
   {
     Current = Total;
   }
   if (Total < 1)
   {
     Total = 1;
   }

   // GetReal EditableWidth
   percent = static_cast<int>(((Current) * barWidth/(Total)));

   char buffer[barWidth+1];
   int i;

   buffer[0] = '[';
   for (i = 1;i < barWidth; i++)
       i < percent ?  buffer[i] = '|' : buffer[i] = ' ';

   buffer[i++] = ']';
   buffer[i] = '\0';

   char *tmp;
   int l = 0;
   if (BarOnly)
     l = asprintf(&tmp,"%s", buffer);
   /*
   else if (SAT)
     l = asprintf(&tmp,"%s\t  %d / %d", buffer, Current, Total);
   */
   else 
     l = asprintf(&tmp,"%s\t  %d / %d  (CH: %d)", buffer, Current, Total, Channel);

   SetText(strndup(tmp,l), false);
   SetSelectable(false);
   free(tmp);
}

// --- Class cMenuInfoItem -------------------------------------

cMenuInfoItem::cMenuInfoItem(const char *text, const char *textValue)
{
   char *buffer = NULL;
   asprintf(&buffer,"%s  %s",text, textValue? textValue:"");

   SetText(strdup(buffer), false);
   SetSelectable(false);
   free(buffer);
}

cMenuInfoItem::cMenuInfoItem(const char *text, int intValue)
{
   char *buffer = NULL;
   asprintf(&buffer,"%s: %d",text, intValue);

   SetText(strdup(buffer), false);
   SetSelectable(false);
   free(buffer);
}
cMenuInfoItem::cMenuInfoItem(const char *text, bool boolValue)
{
   char *buffer = NULL;
   asprintf(&buffer,"%s: %s",text, boolValue? tr("enabled"):tr("disabled"));

   SetText(strdup(buffer), false);
   SetSelectable(false);
   free(buffer);
}

#ifdef REELVDR 
// --- Class cMenuChannelsItem  ------------------------------------------------

cMenuChannelsItem::cMenuChannelsItem(cDirectoryEntry *Entry)
{
   entry_ = Entry;
   char *buffer = NULL;
   isDir_ = false;
   if (Entry->IsDirectory())
   {
      isDir_ = true;
      asprintf(&buffer, "\x82 %s", Entry->Title());
   }
   else 
   {
      asprintf(&buffer,"     %s", Entry->Title());
   }
   
   SetText(buffer, true);
   free(buffer);
}

cMenuChannelsItem::~cMenuChannelsItem()
{
  
}

//
// --- Class cMenuChannelscan ------------------------------------------------

cMenuSelectChannelList::cMenuSelectChannelList()
:cOsdMenu(tr("Channel lists"), 10)
{
  helpKeys_ = -1;
  level_ = current_ = 0;  
  DirectoryFiles.Load();
  // debug // 
  //DumpDir();
  Set();
  hasTimers_ = Timers.Count();
}

void cMenuSelectChannelList::Set()
{    
  if (Current() >= 0)
    current_ = Current();

  Clear();  
    
  for (cDirectoryEntry *d = DirectoryFiles.First(); d; d = DirectoryFiles.Next(d))
  {
     cMenuChannelsItem *item = new cMenuChannelsItem(d);
     Add(item);
  } 
  
  SetCurrent(Get(current_));

  SetHelp(NULL, NULL, NULL, NULL);
  Display();
}

cMenuSelectChannelList::~cMenuSelectChannelList()
{
}

bool cMenuSelectChannelList::ChangeChannelList()
{
  cMenuChannelsItem *ci = (cMenuChannelsItem *)Get(Current());

  if (ci && !ci->IsDirectory() && ci->FileName())
  {
     // new thread? 
     string buff = "cp -f ";
     buff += ci->FileName();
     buff += " ";
     buff +=  setup::FileNameFactory("link");

     system (buff.c_str()); 
     bool ret =  Channels.Reload(setup::FileNameFactory("link").c_str());
     return ret;

  }
  return false;
} 



eOSState cMenuSelectChannelList::ProcessKey(eKeys Key)
{
  
  if (Key == kBack)
  {
    Key = kNone;

    if (level_>0) 
    {
       Open(true); // back 
       Set();
       return osUnknown;
    }
    else 
    {
      return osBack;
    }
  }
  else 
  {
    current_ = 0;
  }
    
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) 
  {
     switch (Key) 
     {
       case kOk:     
       case kRed: if (DirState() == 1) // if no directory
                  {
                     if (ChangeChannelList())
                     {
                        Skins.Message(mtInfo, tr("Changes Done"));
                        if (hasTimers_ > 0)
                           Skins.Message(mtInfo, tr("Please check your Timers!"));
                         return osBack;
                     }
                     else 
                     {
                        Skins.Message(mtError, tr("Changes failed"));
                     }
                  }
                  else if (DirState() == 2)
                  { 
                     Open();
                     Set();
                  }
                  else if (DirState() == 0) // if directory 
                  {  
                     Open(true);  // back 
                     Set();
                  }
                  
            break;

       //case kYellow: state = Delete(); Set(); break;
       // case kBlue: return AddSubMenu(new cMenuBrowserCommands); ; Set(); break; /// make Commands Menu
       // Don`t forget Has || Had Submenus check 
       default: break;
     }
  }

  state = osUnknown;
  return state;
}

int cMenuSelectChannelList::DirState()
{
   cMenuChannelsItem *ci = (cMenuChannelsItem *)Get(Current());
   return  ci?ci->IsDirectory()?2:1:0;
}
eOSState cMenuSelectChannelList::Open(bool Back)
{
  if (!Back)
  {
    level_++;
  }
  else 
  {
    level_--;
    DirectoryFiles.Load(Back); 
    return osUnknown;
  }
    
  cMenuChannelsItem *ci = (cMenuChannelsItem *)Get(Current());

  if (ci && ci->FileName())
  {
     string fname = ci->FileName();
     if (Back)
     {
        DirectoryFiles.Load(Back); 
        return osUnknown;
     }
     else 
     {
        DirectoryFiles.Load(fname);
        return osUnknown;
     }
  }
  return osUnknown;
} 

eOSState cMenuSelectChannelList::Delete() // backup active list
{
  cMenuChannelsItem *ci = (cMenuChannelsItem *)Get(Current());
  if (ci) 
  {
    if (Interface->Confirm(tr("Delete channel list?")))
    {
      if (unlink(ci->FileName()) != 0)
      {
        int errnr = errno; 
        esyslog ("ERROR [channelscan]: can`t delete file %s errno: %d", ci->FileName(), errnr);
      }
    }
  }
  string tmp = "";
  DirectoryFiles.Load(tmp);  // reload current directory 
  return osUnknown;
}

void cMenuSelectChannelList::DumpDir()
{
   for (cDirectoryEntry *d = DirectoryFiles.First(); d; d = DirectoryFiles.Next(d))
   { 
      printf (" d->Entry: %s, Dir? %s %s  \n",d->Title(), d->IsDirectory()?"YES":"NO", d->FileName());
   }
}

#endif
