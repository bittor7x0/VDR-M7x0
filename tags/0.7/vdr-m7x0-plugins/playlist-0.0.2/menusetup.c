/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menusetup.c 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#include "menusetup.h"
#include "playlist.h"
#include "menuplaylists.h"
#include "menucontrol.h"
#include "i18n.h"
#include "vdrtools.h"
#include <vdr/plugin.h>

// --- cMenuSetupPlayList -------------------------------------------------------

cMenuSetupPlayList::cMenuSetupPlayList(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuSetupPlayList::cMenuSetupPlayList", plugin_name);
#endif
  store = false;
  changedefault = false;

  #define StoreValue(T) T.o = T.u
  #define StoreValueFile(T) strcpy(T.o, T.u)

  // var pos7
  StoreValue     (charentry);
  StoreValue     (charfolder);
  StoreValue     (charrecord);
  StoreValue     (commandline_preference);
  StoreValueFile (lastplaylist);
  StoreValueFile (mainmenu_name);
  StoreValue     (mainmenu_visible);
  StoreValue     (confirmdelplentry);
  StoreValue     (okstartplay);
  StoreValue     (deleterecords);
  StoreValueFile (pathplaylists);
  StoreValue     (startoption);
  StoreValue     (storeplaylist);
  StoreValue     (timeoutreadrecords);
  StoreValue     (verbose);
  for (int i = Option_confirmdeleterecord; i < Option_max; i++)
    StoreValue   (PL_options[i]);

  #undef StoreValue
  #undef StoreValueFile

  Set();
}

cMenuSetupPlayList::~cMenuSetupPlayList(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuSetupPlayList::~cMenuSetupPlayList store=%s", plugin_name, store ? "true" : "false");
#endif
  if (!store)
  {

    #define RestoreValue(T) T.u = T.o
    #define RestoreValueFile(T) strcpy(T.u, T.o)

    // var pos8
    RestoreValue     (charentry);
    RestoreValue     (charfolder);
    RestoreValue     (charrecord);
    RestoreValue     (commandline_preference);
    RestoreValueFile (lastplaylist);
    RestoreValueFile (mainmenu_name);
    RestoreValue     (mainmenu_visible);
    RestoreValue     (confirmdelplentry);
    RestoreValue     (okstartplay);
    RestoreValue     (deleterecords);
    RestoreValueFile (pathplaylists);
    RestoreValue     (startoption);
    RestoreValue     (storeplaylist);
    RestoreValue     (timeoutreadrecords);
    RestoreValue     (verbose);
    for (int i = Option_confirmdeleterecord; i < Option_max; i++)
      RestoreValue   (PL_options[i]);

    #undef RestoreValue
    #undef RestoreValueFile
  }
}

void cMenuSetupPlayList::Store(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuSetupPlayList::Store", plugin_name);
#endif

  #define SaveValue(T, N) if (T.u != T.o) SetupStore(N, T.u);
  #define SaveValueChar(T, N) if (T.u != T.o) SetupStore(N, T.u + 256);
  #define SaveValueFile(T, N) if (strcmp(T.u, T.o)) SetupStore(N, T.u);

  // var pos9
  SaveValueChar (charentry,                                "cahre");
  SaveValueChar (charfolder,                               "charf");
  SaveValueChar (charrecord,                               "charr");
  SaveValue     (commandline_preference,                   "commandline");
  SaveValueFile (lastplaylist,                             "lastpl");
  SaveValueFile (mainmenu_name,                            "name");
  SaveValue     (mainmenu_visible,                         "visible");
  SaveValue     (confirmdelplentry,                        "deletentry");
  SaveValue     (okstartplay,                              "okplay");
  SaveValue     (deleterecords,                            "delete");
  SaveValueFile (pathplaylists,                            "path");
  SaveValue     (startoption,                              "start");
  SaveValue     (storeplaylist,                            "storepl");
  SaveValue     (timeoutreadrecords,                       "timerecords");
  SaveValue     (verbose,                                  "verbose");
  SaveValue     (PL_options[Option_confirmdeleterecord],   "confdelete");
  SaveValue     (PL_options[Option_confirmstartnewrecord], "startnew");
  SaveValue     (PL_options[Option_deletefromlist],        "deletel");
  SaveValue     (PL_options[Option_deleterecord],          "deleter");
  SaveValue     (PL_options[Option_jumpmark],              "jumpmark");
  SaveValue     (PL_options[Option_playoption1],           "playoption1");
  SaveValue     (PL_options[Option_playoption2],           "playoption2");
  SaveValue     (PL_options[Option_playonlynew],           "playonlynew");
  SaveValue     (PL_options[Option_searchnewrecord],       "searchnew");
  SaveValue     (PL_options[Option_searchrecordsub],       "searchsub");

  #undef SaveValue
  #undef SaveValueChar
  #undef SaveValueFile

  store = true;
}

void cMenuSetupPlayList::Set(bool sel)
{
#ifdef PL_Debug2
  dsyslog("%s: cMenuSetupPlayList::Set Current=%d", plugin_name, Current());
#endif
  int current = Current();
  Clear();
  fileitempos = -1;

  #define DisplayLine(T) !T.h && (!commandline_preference.u || !T.c)

  Add(new cOsdItem         (tr("Setup$Display playlists now"),                           osUser6));
  Add(new cOsdItem         (tr("Setup$Play last playlist now"),                          osUser7));
  if (DisplayLine(commandline_preference))
    Add(new cMenuEditBoolItem(tr("Setup$Preferr Command Line Parameter"),                &commandline_preference.u));
  if (DisplayLine(mainmenu_visible))
    Add(new cMenuEditBoolItem(tr("Setup$Visible in Mainmenu"),                           &mainmenu_visible.u));
  if (DisplayLine(mainmenu_name) && mainmenu_visible.u)
    Add(new cMenuEditStrItem (tr("Setup$  Name for Mainmenu"),                           mainmenu_name.u, sizeof(mainmenu_name.u), tr(FileNameChars)));
  if (DisplayLine(startoption))
    Add(new cMenuEditStraItem(tr("Setup$Startoptions for plugin"),                       &startoption.u, 4, StartOptions));
  if (DisplayLine(okstartplay))
    Add(new cMenuEditBoolItem(tr("Setup$Press OK in Playlistview starts"),               &okstartplay.u, tr("Setup$Edit"), tr("Setup$Play")));
  if (DisplayLine(storeplaylist))
    Add(new cMenuEditBoolItem(tr("Setup$Store playlist after change"),                   &storeplaylist.u, tr("Setup$automatically"), tr("Setup$after question")));
  if (DisplayLine(deleterecords))
    Add(new cMenuEditBoolItem(tr("Setup$Allow delete Records"),                          &deleterecords.u));
  if (DisplayLine(confirmdelplentry))
    Add(new cMenuEditBoolItem(tr("Setup$Confirm delete playlistentries"),                &confirmdelplentry.u));
  if (DisplayLine(pathplaylists))
  {
    fileitempos = Count();
    Add(new cMenuEditStrItem (tr("Setup$Path for store playlists"),                      pathplaylists.u, sizeof(pathplaylists.u), FileNameCharsAllowed));
  }
  Add(new cMenuEditBoolItem(tr("Setup$Change default options for playlist"),             &changedefault));
  if (changedefault)
  {
    if (DisplayLine(PL_options[Option_playoption1]))
      Add(new cMenuEditStraItem(tr("OptionPL$  In playlistview starts play at"),         &PL_options[Option_playoption1].u, PlayOptions_max - 1, PlayOptions));
    if (DisplayLine(PL_options[Option_playoption2]))
      Add(new cMenuEditStraItem(tr("OptionPL$  In detailview starts play at"),           &PL_options[Option_playoption2].u, PlayOptions_max - 1, PlayOptions));
    if (DisplayLine(PL_options[Option_searchnewrecord]))
      Add(new cMenuEditBoolItem(tr("OptionPL$  Search new records on folders"),          &PL_options[Option_searchnewrecord].u));
    if (DisplayLine(PL_options[Option_searchrecordsub]))
      Add(new cMenuEditBoolItem(tr("OptionPL$  Search new records on subfolders"),       &PL_options[Option_searchrecordsub].u));
    if (DisplayLine(PL_options[Option_playonlynew]))
      Add(new cMenuEditBoolItem(tr("OptionPL$  Play only new Records"),                  &PL_options[Option_playonlynew].u));
    if (DisplayLine(PL_options[Option_jumpmark]))
      Add(new cMenuEditBoolItem(tr("OptionPL$  Jump to first segment of movie"),         &PL_options[Option_jumpmark].u));
    if (DisplayLine(PL_options[Option_confirmstartnewrecord]))
      Add(new cMenuEditBoolItem(tr("OptionPL$  Confirm start new record"),               &PL_options[Option_confirmstartnewrecord].u));
    if (DisplayLine(PL_options[Option_deletefromlist]))
      Add(new cMenuEditBoolItem(tr("OptionPL$  Remove records after play from list"),    &PL_options[Option_deletefromlist].u));
    if (DisplayLine(PL_options[Option_deleterecord]) && deleterecords.u)
      Add(new cMenuEditBoolItem(tr("OptionPL$  Delete records after play"),              &PL_options[Option_deleterecord].u));
    if (DisplayLine(PL_options[Option_confirmdeleterecord]) && PL_options[Option_deleterecord].u && deleterecords.u)
      Add(new cMenuEditBoolItem(tr("OptionPL$    Confirm delete records"),               &PL_options[Option_confirmdeleterecord].u));
  }
  Add(new cMenuEditBoolItem(tr("Setup$Change Character for identification"),             &changechar));
  if (changechar)
  {
    if (DisplayLine(charfolder))
      Add(new cMenuEditChrItem(tr("OptionPL$  Character for folders"),                   &charfolder.u, AllCharsAllowed));
    if (DisplayLine(charentry))
      Add(new cMenuEditChrItem(tr("OptionPL$  Character for records in folders"),        &charentry.u, AllCharsAllowed));
    if (DisplayLine(charrecord))
      Add(new cMenuEditChrItem(tr("OptionPL$  Character for normal records"),            &charrecord.u, AllCharsAllowed));
  }
  if (DisplayLine(verbose))
    Add(new cMenuEditBoolItem(tr("Setup$Verbose Log Mode"),                              &verbose.u));
  if (sel && changechar)
    SetCurrent(Get(Count() - 1));

  #undef DisplayLine

#ifdef PL_Debug2
  dsyslog("%s: Count=%d", plugin_name, Count());
#endif
  SetCurrent(Get(current));
}

eOSState cMenuSetupPlayList::ProcessKey(eKeys Key)
{
#ifdef PL_Debug3
  bool noneKey = Key == kNone;
  if (!noneKey)
    dsyslog("%s: cMenuSetupPlayList::ProcessKey Key=%s", plugin_name, KeyName(Key));
#endif
  static char oldstatus[MAXOSDTEXTWIDTH];
  char status[MAXOSDTEXTWIDTH];
  int pathlen;

  int m_commandline_preference = commandline_preference.u;
  int m_mainmenu_visible = mainmenu_visible.u;
  int m_deleterecords = deleterecords.u;
  int m_changedefault = changedefault;
  int m_deleterecord = PL_options[Option_deleterecord].u;
  int m_changechar = changechar;
  int m_playoption = PL_options[Option_playoption1].u;

  eOSState state = cMenuSetupPage::ProcessKey(Key);
#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
  {
    dsyslog("%s: cMenuSetupPlayList::ProcessKey OSState=%s", plugin_name, OSStateName(state));
    noneKey = false;
  }
#endif

  if (m_commandline_preference != commandline_preference.u || m_mainmenu_visible != mainmenu_visible.u || m_deleterecords != deleterecords.u || m_changedefault != changedefault || m_deleterecord != PL_options[Option_deleterecord].u || m_changechar != changechar)
  {
    Set(m_changechar != changechar);
    Display();
    if (!m_changedefault && changedefault)
    {
      for (int i = 0; i < 5; i++)
        CursorDown();
      for (int i = 0; i < 5; i++)
        CursorUp();
    }
  } else if (m_playoption != PL_options[Option_playoption1].u && PL_options[Option_playoption1].u == PlayOptions_selectpos)
  {
    PL_options[Option_playoption1].u = m_playoption < PlayOptions_selectpos ? PlayOptions_question : PlayOptions_lastplay;
    RefreshCurrent();
  }
  if (Current() == fileitempos)
  {
    const char *temp = PluginPlaylist->ExpandPath("", false);
    pathlen = strlen(temp);
    if (pathlen > AKTOSDTEXTWIDTH - 10)
      sprintf(status, "...%s", temp + pathlen - AKTOSDTEXTWIDTH + 10);
    else
      strcpy(status, temp);
    if (oldstatus != status)
    {
      SetStatus(status);
      strcpy(oldstatus, status);
    }
  } else if (strlen(oldstatus))
  {
    SetStatus(NULL);
    oldstatus[0] = 0;
  }
  switch (state)
  {
    case osUser6:    state = AddSubMenu(new cMenuPlaylists);
                     break;
    case osUser7:    {
                       cMenuPlaylists *menu = new cMenuPlaylists;
                       if (SelectPLR)
                         menu->ProcessKey(kUser2);
                       state = AddSubMenu(menu);
                     }
                     break;
    case osUser9:    if (SelectPLR)
                     {
                       cControl::Shutdown();  // Start play (SelectPLR)
                       cControl::Launch(new cControlPlaylist);
                     }
                     state = osEnd;
                     break;
    default:         break;
  }

#ifdef PL_Debug3
  if (!noneKey || (state != osUnknown && state != osContinue))
    dsyslog("%s: cMenuSetupPlayList::ProcessKey returned OSState=%s", plugin_name, OSStateName(state));
#endif
  return state;
}
