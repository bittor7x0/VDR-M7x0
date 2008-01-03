/*
 * noepgmenu.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/device.h>
#include <vdr/i18n.h>

#if APIVERSNUM < 10507
#define trNOOP(s) (s)
#endif

#if APIVERSNUM < 10509
#define trVDR(s) tr(s)
#endif

static const char *VERSION        = "0.0.4";
static const char *DESCRIPTION    = trNOOP("a menu for noEPG patch");
static const char *MAINMENUENTRY  = trNOOP("noEPG channels");

#define CHNUMWIDTH  (numdigits(Channels.MaxNumber()) + 1)

#if APIVERSNUM < 10507
const tI18nPhrase Phrases[] = {
  { "a menu for noEPG patch",
    "ein Menü für den noEPG Patch",// Deutsch
    "",// TODO Slovenski
    "",// TODO Italiano
    "",
    "",// TODO Portugues
    "",// TODO Francais
    "",// TODO Norsk
    "",
    "",// TODO Polski
    "",// TODO Español
    "",// TODO ÅëëçíéêÜ (Greek)
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
    "",// TODO ÀãááÚØÙ (Russian)
    "",// TODO Hrvatski (Croatian)
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "noEPG channels",
    "noEPG Kanäle",// Deutsch
    "",// TODO Slovenski
    "",// TODO Italiano
    "",
    "",// TODO Portugues
    "",// TODO Francais
    "",// TODO Norsk
    "",
    "",// TODO Polski
    "",// TODO Español
    "",// TODO ÅëëçíéêÜ (Greek)
    "",// TODO Svenska
    "",// TODO Romaneste
    "",// TODO Magyar
    "",// TODO Català
    "",// TODO ÀãááÚØÙ (Russian)
    "",// TODO Hrvatski (Croatian)
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "Hide main menu entry",
    "Hauptmenüeintrag verstecken",
    "",// TODO
    "Nascondi voce nel menù principale",// Italiano
    "Verberg hoofd menu ingave",
    "",// TODO
    "Invisible dans le menu principal",
    "",// TODO
    "Piilota valinta päävalikosta",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { "too many channels!",
    "zu viele Kanäle!",
    "",// TODO
    "",// Italiano
    "",
    "",// TODO
    "",
    "",// TODO
    "",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO 
    "",// TODO
    "",// TODO
#if VDRVERSNUM && VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
  },
  { NULL }
  };
#endif

// --- cMenuChannelItemNoEPG ------------------------------------------------------

class cMenuChannelItemNoEPG : public cOsdItem {
private:
  cChannel *channel;
public:
  cMenuChannelItemNoEPG(cChannel *Channel);
  virtual int Compare(const cListObject &ListObject) const;
  virtual void Set(void);
  cChannel *Channel(void) { return channel; }
  };

cMenuChannelItemNoEPG::cMenuChannelItemNoEPG(cChannel *Channel)
{
  channel = Channel;
  if (channel->GroupSep())
     SetSelectable(false);
  Set();
}

int cMenuChannelItemNoEPG::Compare(const cListObject &ListObject) const
{
  cMenuChannelItemNoEPG *p = (cMenuChannelItemNoEPG *)&ListObject;
  return channel->Number() - p->channel->Number();
}

void cMenuChannelItemNoEPG::Set(void)
{
  char *buffer = NULL;
  if (!channel->GroupSep()) {
      tChannelID kanalID;
      kanalID=channel->GetChannelID();
      asprintf(&buffer, "%d\t%s\t%s", channel->Number(), channel->Name(), Channels.keinEPGScan(kanalID)?"noEPG":"EPG");
     }
  else
     asprintf(&buffer, "---\t%s ----------------------------------------------------------------", channel->Name());
  SetText(buffer, false);
}

// --- cMenuChannelsNoEPG ---------------------------------------------------------

class cMenuChannelsNoEPG : public cOsdMenu {
private:
  void Setup(void);
  cChannel *GetChannel(int Index);
protected:
  eOSState noEPGAdd(void);
  eOSState noEPGDel(void);
public:
  cMenuChannelsNoEPG(void);
  ~cMenuChannelsNoEPG();
  virtual eOSState ProcessKey(eKeys Key);
  };

cMenuChannelsNoEPG::cMenuChannelsNoEPG(void)
:cOsdMenu(tr("noEPG channels"), CHNUMWIDTH, 30)
{
  Setup();
}

cMenuChannelsNoEPG::~cMenuChannelsNoEPG()
{
}

void cMenuChannelsNoEPG::Setup(void)
{
  cChannel *currentChannel = GetChannel(Current());
  if (!currentChannel)
     currentChannel = Channels.GetByNumber(cDevice::CurrentChannel());
  cMenuChannelItemNoEPG *currentItem = NULL;
  Clear();
  for (cChannel *channel = Channels.First(); channel; channel = Channels.Next(channel)) {
      if ( *channel->Name()) {
         cMenuChannelItemNoEPG *item = new cMenuChannelItemNoEPG(channel);
         Add(item);
         if (channel == currentChannel)
            currentItem = item;
         }
      }
  SetCurrent(currentItem);
  Display();
}

cChannel *cMenuChannelsNoEPG::GetChannel(int Index)
{
  cMenuChannelItemNoEPG *p = (cMenuChannelItemNoEPG *)Get(Index);
  return p ? (cChannel *)p->Channel() : NULL;
}

//TK
eOSState cMenuChannelsNoEPG::noEPGAdd(void)
{
    if (!HasSubMenu() && Count() > 0) {
	cChannel *channel = GetChannel(Current());
	tChannelID kanalID;
	kanalID=channel->GetChannelID();
	
	if (strstr(::Setup.noEPG,kanalID.ToString())==NULL){
	    if (strlen(::Setup.noEPG) + strlen(*kanalID.ToString())+1 > 999)
	    {
		Skins.Message(mtError, tr("too many channels!"));
		return osContinue;
	    }
	    sprintf(::Setup.noEPG,"%s %s",::Setup.noEPG,*kanalID.ToString());
	    ::Setup.Save();
	}
    }
    RefreshCurrent();
    Display();
    return osContinue;
}

//TK
eOSState cMenuChannelsNoEPG::noEPGDel(void)
{
  char neu[999] = "";
  char tmp[999];
  char *sep = " ";
  char *word, *brkt;
  
  if (!HasSubMenu() && Count() > 0) {
    cChannel *channel = GetChannel(Current());
    tChannelID kanalID;
    kanalID=channel->GetChannelID();
    
    if (strstr(::Setup.noEPG,kanalID.ToString())!=NULL){
      
      strcpy(tmp,::Setup.noEPG);
      
      for (word = strtok_r(tmp, sep, &brkt);
	   word;
	   word = strtok_r(NULL, sep, &brkt))
	{
	  if(strcmp(word,kanalID.ToString())!=0){
	    strcat(neu," ");
	    strcat(neu,word);
	  }
	}
      
      strcpy(::Setup.noEPG,neu);
      ::Setup.Save();
    }
  }
  RefreshCurrent();
  Display();
  return osContinue;
}

eOSState cMenuChannelsNoEPG::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  cChannel *channel = GetChannel(Current());
  tChannelID kanalID;
  kanalID=channel->GetChannelID();

  switch (state) {
    default:
         if (state == osUnknown) {
            switch (Key) {
		case kOk: 
		    if (!Channels.keinEPGScan(kanalID))
			return noEPGAdd();
		    else
			return noEPGDel();
		    break;
              default: break;
              }
            }
    }
  return state;
}

struct cConfigNoEPG
{
public:
    int hidemenu;
    cConfigNoEPG(void) 
	{
	    hidemenu = 0;
	}
};
cConfigNoEPG ConfigNoEPG;


class cMenuSetupNoEPG : public cMenuSetupPage {
  private:
    virtual void Setup(void);
    cConfigNoEPG data;
  protected:
    virtual eOSState ProcessKey(eKeys Key);
    virtual void Store(void);
    void Set(void);
  public:
    cMenuSetupNoEPG(void);
};

cMenuSetupNoEPG::cMenuSetupNoEPG(void)
{
  Setup();
}

void cMenuSetupNoEPG::Setup(void)
{
  data = ConfigNoEPG;
  Set();
}

void cMenuSetupNoEPG::Set()
{
  int current = Current();
  Clear();
  Add(new cMenuEditBoolItem(tr("Hide main menu entry"),         &data.hidemenu,       trVDR("no"),      trVDR("yes")));
  SetCurrent(Get(current));
  Display();
}

void cMenuSetupNoEPG::Store(void)
{
    ConfigNoEPG = data;

    SetupStore("HideMenu",  ConfigNoEPG.hidemenu);
}

eOSState cMenuSetupNoEPG::ProcessKey(eKeys Key)
{
    return cMenuSetupPage::ProcessKey(Key);
}

class cPluginNoepgmenu : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginNoepgmenu(void);
  virtual ~cPluginNoepgmenu();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return (ConfigNoEPG.hidemenu ? NULL : tr(MAINMENUENTRY)); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginNoepgmenu::cPluginNoepgmenu(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginNoepgmenu::~cPluginNoepgmenu()
{
  // Clean up after yourself!
}

const char *cPluginNoepgmenu::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginNoepgmenu::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginNoepgmenu::Initialize(void)
{
#if APIVERSNUM < 10507
  RegisterI18n(Phrases);
#endif
  return true;
}

bool cPluginNoepgmenu::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginNoepgmenu::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginNoepgmenu::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginNoepgmenu::MainMenuAction(void)
{
    return new cMenuChannelsNoEPG;
}

cMenuSetupPage *cPluginNoepgmenu::SetupMenu(void)
{
  return new cMenuSetupNoEPG;
}

bool cPluginNoepgmenu::SetupParse(const char *Name, const char *Value)
{
    if      (!strcasecmp(Name, "HideMenu")) ConfigNoEPG.hidemenu = atoi(Value);
    return true;
}

bool cPluginNoepgmenu::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginNoepgmenu::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginNoepgmenu::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginNoepgmenu); // Don't touch this!
