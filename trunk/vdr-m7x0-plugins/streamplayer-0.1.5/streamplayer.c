/*
 * streamplayer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: streamplayer.c,v 1.1 2008-11-22 19:55:02 dev Exp $
 */

#include "streamplayer.h"
#include "player.h"
#include "httpplayer.h"
#include "udpplayer.h"
#include "fileplayer.h"
#include "menu.h"
#include "compatibility.h"
#include "i18n.h"

static const char *VERSION        = "0.1.5";
static const char *DESCRIPTION    = tr("Stream video player");
static const char *MAINMENUENTRY  = tr("Streamplayer");


cBookmark::cBookmark() {
	title=NULL;
	url=NULL;
}

cBookmark::cBookmark(char *Title, char *Url) {
	title=NULL;
	url=NULL;
	Set(Title,Url);
}

void cBookmark::Set(char *Title, char *Url) {
	if (title) free(title);
	if (url) free(url);
	title=strdup(Title);
	url=strdup(Url);
}

cBookmark::~cBookmark() {
	if (title) free(title);
	if (url) free(url);
}

bool cBookmark::Parse(const char *s) {
	int i=0;
	while (s[i]!=':') {
		if (s[i]==0) return false;
		i++;
	}
	title=MALLOC(char,i+1);
	strn0cpy(title,s,i+1);
	stripspace(title);
	url=strdup(skipspace(s+i+1));
	stripspace(url);
	return true;
}

bool cBookmark::Save(FILE *f) {
	fprintf(f,"%s:%s\n",title,url);
	return true;
}



cPluginStreamplayer *cPluginStreamplayer::Plugin=NULL;

cPluginStreamplayer::cPluginStreamplayer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  strcpy(PlayURL,"http://localhost:3000/TS/1");
  BufferSize=2048;
  BufferPreFill=128;
  Plugin=this;
}

cPluginStreamplayer::~cPluginStreamplayer()
{
  // Clean up after yourself!
  Plugin=NULL;
}

const char *cPluginStreamplayer::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginStreamplayer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginStreamplayer::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
#if VDRVERSNUM < 10507
  RegisterI18n(StreamplayerPhrases);
#endif
  return true;
}

bool cPluginStreamplayer::Start(void)
{
  // Start any background activities the plugin shall perform.

  	Bookmarks.Load(AddDirectory(ConfigDirectory(),"streamplayer-bookmarks.conf"),true);
  	if (Bookmarks.Count()<1) {
	  	Bookmarks.Add(new cBookmark("Listen on UDP port 1234","udp:1234"));
		Bookmarks.Add(new cBookmark("Play channel 1 via Streamdev http","http://localhost:3000/TS/1"));
		Bookmarks.Save();
  	}

  	return true;
}

void cPluginStreamplayer::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginStreamplayer::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cStreamplayerMenu();
}

cMenuSetupPage *cPluginStreamplayer::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cStreamplayerSetupMenu();
}

bool cPluginStreamplayer::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.

  if      (!strcasecmp(Name, "PlayURL"))       strncpy(PlayURL,Value,sizeof PlayURL);
  else if (!strcasecmp(Name, "BufferSize"))    BufferSize=atoi(Value);
  else if (!strcasecmp(Name, "BufferPreFill")) BufferPreFill=atoi(Value);
  else return false;

  return true;
}

void cPluginStreamplayer::Store() {
	SetupStore("PlayURL",PlayURL);
}


bool cPluginStreamplayer::GetProtocol(char *dest, int len, char *&url) {

	while (*url==' ') url++;
	// skip spaces

	for (;;) {
		if (len==0) return false;
		// buffer overflow
		if (*url==0) return false;
		// unexpected end

		if (*url==':') break;
		// done parsing

		// copy char
		*dest=*url;
		dest++;
		url++;
		len--;
	}
	*dest=0;
	url++;
	return true;
}


bool cPluginStreamplayer::StartPlayer(char *url) {
	cStreamPlayer *Player;
	char proto[5];

	strncpy(PlayURL,url,sizeof PlayURL);
	Store();

	if (!GetProtocol(proto,sizeof proto,url)) {
		Interface->Error(tr("Protocol type scan error"));
		return false;
	}

	if (0==strcasecmp(proto,"http")) {
		Player=new cStreamPlayerHTTP(url);
	} else if (0==strcasecmp(proto,"udp")) {
		Player=new cStreamPlayerUDP(url);
	} else if (0==strcasecmp(proto,"file")) {
		Player=new cStreamPlayerFile(url);
	} else {
		Interface->Error(tr("Unknown protocol type"));
		return false;
	}

	cControl::Launch(new cStreamControl(Player));

	return true;
}

const char* cPluginStreamplayer::MainMenuEntry(void) {
	return MAINMENUENTRY;
}
const char* cPluginStreamplayer::Version(void) {
	return VERSION;
}
const char* cPluginStreamplayer::Description(void) {
	return DESCRIPTION;
}


VDRPLUGINCREATOR(cPluginStreamplayer); // Don't touch this!
