/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: menusetup.c 0.5 2005/11/17 21:05:05 hflor Exp $
 */

#include "menusetup.h"
#include "undelete.h"
#include "menuundelete.h"
#include "i18n.h"
#include "vdrtools.h"
#include "menueditkeys.h"
#include "menunoselectitem.h"
#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <typeinfo>

// --- cMenuSetupUndelete -------------------------------------------------------

cMenuSetupUndelete::cMenuSetupUndelete(int FunctionToChange)
{
#ifdef UND_Debug
  objID = ++cMenuSetupUndelete_nextID;
  d2syslog("cMenuSetupUndelete::cMenuSetupUndelete", "%s", "");
#endif
  store = false;

  #define StoreValue(T) T.o = T.u
  #define StoreValueFile(T) strcpy(T.o, T.u)

  // var pos7
  StoreValue     (commandline_preference);
  StoreValue     (confirmpurge);
  StoreValue     (confirmpurgedir);
  StoreValue     (confirmpurgeall);
  StoreValue     (confirmsalvage);
  StoreValue     (confirmsalvagedir);
  StoreValue     (confirmsalvageall);
  StoreValue     (firstsalvage);
  for (int i = 2; i < MaxFunctionName; i++)
    StoreValue   (functionkey[i]);
  StoreValueFile (mainmenu_name);
  StoreValue     (mainmenu_visible);
  StoreValue     (numkey);
  StoreValue     (okkey);
  for (int i = 0; i < 2; i++)
  {
    StoreValue   (functionline_b_e[i]);
    StoreValue   (functionline_num[i]);
  }
  StoreValue     (verbose);

  #undef StoreValue
  #undef StoreValueFile

  show_setupfunction = FunctionToChange > 0;
  Set(FunctionToChange);
}

cMenuSetupUndelete::~cMenuSetupUndelete(void)
{
  d2syslog("cMenuSetupUndelete::~cMenuSetupUndelete", "store=%s", store ? "true" : "false");
  if (!store)
  {

    #define RestoreValue(T) T.u = T.o
    #define RestoreValueFile(T) strcpy(T.u, T.o)

    // var pos8
    RestoreValue     (commandline_preference);
    RestoreValue     (confirmpurge);
    RestoreValue     (confirmpurgedir);
    RestoreValue     (confirmpurgeall);
    RestoreValue     (confirmsalvage);
    RestoreValue     (confirmsalvagedir);
    RestoreValue     (confirmsalvageall);
    RestoreValue     (firstsalvage);
    for (int i = 2; i < MaxFunctionName; i++)
      RestoreValue   (functionkey[i]);
    RestoreValueFile (mainmenu_name);
    RestoreValue     (mainmenu_visible);
    RestoreValue     (numkey);
    RestoreValue     (okkey);
    for (int i = 0; i < 2; i++)
    {
      RestoreValue   (functionline_b_e[i]);
      RestoreValue   (functionline_num[i]);
    }
    RestoreValue     (verbose);

    #undef RestoreValue
    #undef RestoreValueFile
  }
}

void cMenuSetupUndelete::Store(void)
{
  d2syslog("cMenuSetupUndelete::Store", "%s", "");

  #define SaveValue(T, N) if (T.u != T.o) SetupStore(N, T.u);
  #define SaveValueFile(T, N) if (strcmp(T.u, T.o)) SetupStore(N, T.u);

  // var pos9
  SaveValue     (commandline_preference, "commandline");
  SaveValue     (confirmpurge,           "confpurge1");
  SaveValue     (confirmpurgedir,        "confpurge2");
  SaveValue     (confirmpurgeall,        "confpurge3");
  SaveValue     (confirmsalvage,         "confsalvage1");
  SaveValue     (confirmsalvagedir,      "confsalvage2");
  SaveValue     (confirmsalvageall,      "confsalvage3");
  SaveValue     (firstsalvage,           "firstsalvage");
  SaveValue     (functionkey[2],         "functionkey02");
  SaveValue     (functionkey[3],         "functionkey03");
  SaveValue     (functionkey[4],         "functionkey04");
  SaveValue     (functionkey[5],         "functionkey05");
  SaveValue     (functionkey[6],         "functionkey06");
  SaveValue     (functionkey[7],         "functionkey07");
  SaveValue     (functionkey[8],         "functionkey08");
  SaveValue     (functionkey[9],         "functionkey09");
  SaveValue     (functionkey[10],        "functionkey10");
  SaveValueFile (mainmenu_name,          "name");
  SaveValue     (mainmenu_visible,       "visible");
  SaveValue     (numkey,                 "numkey");
  SaveValue     (okkey,                  "okkey");
  SaveValue     (functionline_b_e[0],    "purgebe");
  SaveValue     (functionline_num[0],    "purgenum");
  SaveValue     (functionline_b_e[1],    "salvagebe");
  SaveValue     (functionline_num[1],    "salvagenum");
  SaveValue     (verbose,                "verbose");

  #undef SaveValue
  #undef SaveValueFile

  store = true;
}

void cMenuSetupUndelete::Set(int FunctionToSelect)
{
  int current = Current();
  d2syslog("cMenuSetupUndelete::Set", "Current=%d", current);
  Clear();

  #define DisplayLine(T) !T.h && (!commandline_preference.u || !T.c)

  Add(new cOsdItem         (tr("Setup$Display deleted recordings now"),                  osUser1));
  if (DisplayLine(commandline_preference))
    Add(new cMenuEditBoolItem(tr("Setup$Preferr Command Line Parameter"),                &commandline_preference.u));
  if (DisplayLine(mainmenu_visible))
    Add(new cMenuEditBoolItem(tr("Setup$Visible in Mainmenu"),                           &mainmenu_visible.u));
  if (DisplayLine(mainmenu_name) && mainmenu_visible.u)
    Add(new cMenuEditStrItem (tr("Setup$  Name for Mainmenu"),                           mainmenu_name.u, sizeof(mainmenu_name.u), tr(FileNameChars)));
  if (DisplayLine(confirmpurge))
    Add(new cMenuEditBoolItem(tr("Setup$Confirm purge deleted recordings"),              &confirmpurge.u));
  if (DisplayLine(confirmpurgedir))
    Add(new cMenuEditBoolItem(tr("Setup$Confirm purge directories"),                     &confirmpurgedir.u));
  if (DisplayLine(confirmpurgeall))
    Add(new cMenuEditBoolItem(tr("Setup$Confirm purge all deleted recordings"),          &confirmpurgeall.u));
  if (DisplayLine(confirmsalvage))
    Add(new cMenuEditBoolItem(tr("Setup$Confirm salvage deleted recordings"),            &confirmsalvage.u));
  if (DisplayLine(confirmsalvagedir))
    Add(new cMenuEditBoolItem(tr("Setup$Confirm salvage directories"),                   &confirmsalvagedir.u));
  if (DisplayLine(confirmsalvageall))
    Add(new cMenuEditBoolItem(tr("Setup$Confirm salvage all deleted recordings"),        &confirmsalvageall.u));
  if (DisplayLine(functionline_b_e[0]))
    Add(new cMenuEditStraItem(tr("Setup$Purgeline position"),                            &functionline_b_e[0].u, 4, FunctionLine));
  if (DisplayLine(functionline_num[0]) && functionline_b_e[0].u == 0x3)
    Add(new cMenuEditIntItem (tr("Setup$  Display bottom purge ex lines"),               &functionline_num[0].u, 1, 200));
  if (DisplayLine(functionline_b_e[1]))
    Add(new cMenuEditStraItem(tr("Setup$Salvageline position"),                          &functionline_b_e[1].u, 4, FunctionLine));
  if (DisplayLine(functionline_num[1]) && functionline_b_e[1].u == 0x3)
    Add(new cMenuEditIntItem (tr("Setup$  Display bottom salvage ex lines"),             &functionline_num[1].u, 1, 200));
  if ((DisplayLine(functionline_b_e[0]) || DisplayLine(functionline_b_e[1])) && DisplayLine(firstsalvage) && ((functionline_b_e[0].u & functionline_b_e[1].u & 0x1) != 0x0 || (functionline_b_e[0].u & functionline_b_e[1].u & 0x2) != 0x0))
    Add(new cMenuEditBoolItem(tr("Setup$  inward lines"),                                &firstsalvage.u, tr("Choise$purge"), tr("Choise$salvage")));
  if (FunctionToSelect > 0)
    current = Count(); // pre select this line for display
  Add(new cMenuEditBoolItem(tr("Setup$Change assigned functionkeys"),                    &show_setupfunction));
  if (show_setupfunction)
  {
    for (int i = 2; i < MaxFunctionName; i++)
    {
      if (FunctionToSelect == i)
        current = Count();
      char *name = NULL;
      asprintf(&name, tr("Setup$  Key for function-%s"), FunctionName[i]);
      Add(new cMenuEditKeysItem(name,                                                    &functionkey[i].u));
      free(name);
    }
    Add(new cMenuOsdNoSelectItem(tr("Setup$  ** Spezial handled keys **")));
    if (FunctionToSelect == 1 && okkey.u == 1)
      current = Count();
    Add(new cMenuEditStraItem(tr("Setup$  OK-key function"),                             &okkey.u, MaxFunctionName, FunctionName));
    if (FunctionToSelect == 1 && numkey.u == 1)
      current = Count();
    Add(new cMenuEditStraItem(tr("Setup$  Numerickey select the entry and"),             &numkey.u, 4, FunctionName));
  }
  if (DisplayLine(verbose))
    Add(new cMenuEditBoolItem(tr("Setup$Verbose Log Mode"),                              &verbose.u));

  #undef DisplayLine

  d2syslog("cMenuSetupUndelete::Set", "Count=%d", Count());
  SetCurrent(Get(current));
  Display();
}

eOSState cMenuSetupUndelete::ProcessKey(eKeys Key)
{
  d3ProcessKey1("cMenuSetupUndelete::ProcessKey");
  static bool oldhelpkeys;
  bool helpkeys;

  int m_commandline_preference = commandline_preference.u;
  int m_mainmenu_visible = mainmenu_visible.u;
  int m_show_setupfunction = show_setupfunction;
  int m_purge_b_e = functionline_b_e[0].u;
  int m_salvage_b_e = functionline_b_e[1].u;

  eOSState state = osUnknown;
  
  if (Key == kOk)
  {
    int function1 = 0;
    int function2 = 0;
    for (int i = 2; i < MaxFunctionName; i++)
      if (functionkey[i].u)
        for (int j = i + 1; j < MaxFunctionName; j++)
          if (functionkey[j].u && functionkey[i].u == functionkey[j].u && (i != 6 || j != 7) && (i != 8 || j != 9))
          {
            function1 = i;
            function2 = j;
            i = j = MaxFunctionName;
          }
    if (function1)
    {
      show_setupfunction = true;
      Set(function2);
      char *info = NULL;
      asprintf(&info, tr("Error$Key for funtion %s and %s identical."), FunctionName[function1], FunctionName[function2]);
      INFO(info);
      free(info);
      state = osContinue;
    }
  }

  if (state == osUnknown)
    state = cMenuSetupPage::ProcessKey(Key);
  d3ProcessKey2("cMenuSetupUndelete::ProcessKey");

  if (m_commandline_preference != commandline_preference.u || m_mainmenu_visible != mainmenu_visible.u || m_show_setupfunction != show_setupfunction || m_purge_b_e != functionline_b_e[0].u || m_salvage_b_e != functionline_b_e[1].u)
    Set();

  helpkeys = typeid(*Get(Current())) == typeid(cMenuEditKeysItem);
  if (oldhelpkeys != helpkeys)
  {
    oldhelpkeys = helpkeys;
    if (helpkeys)
      SetHelp(tr("Setup$Select"), tr("Setup$Select"), tr("Setup$Select"), tr("Setup$Select"));
    else
      SetHelp(NULL, NULL, NULL, NULL);
  }

  switch (state)
  {
    case osUser1:   state = AddSubMenu(new cMenuUndelete);
                    break;
    default:        break;
  }

  d3ProcessKey3("cMenuSetupUndelete::ProcessKey");
  return state;
}
