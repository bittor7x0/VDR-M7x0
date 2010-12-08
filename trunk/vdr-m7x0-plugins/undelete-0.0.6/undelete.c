/*
 * undelete: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: undelete.c 0.6 2006/03/15 22:12:06 hflor Exp $
 */

#include "undelete.h"
#include "menuundelete.h"
#include "menusetup.h"
#include "i18n.h"
#include "vdrtools.h"
#include <getopt.h>
#include <ctype.h>
#include <vdr/recording.h>
#include <vdr/videodir.h>

static const char *VERSION           = "0.0.6";
static const char *DESCRIPTION       = "undelete for recordings";

// Global variables that control the overall behaviour:

// var pos2
tParamFile  undeleteconfigfile     = { false, false, false, "undelete.conf", "undelete.conf" };
tParamInt   commandline_preference = { false, false, false, true };
tParamInt   confirmpurge           = { false, false, false, true };
tParamInt   confirmpurgedir        = { false, false, false, true };
tParamInt   confirmpurgeall        = { false, false, false, true };
tParamInt   confirmsalvage         = { false, false, false, true };
tParamInt   confirmsalvagedir      = { false, false, false, true };
tParamInt   confirmsalvageall      = { false, false, false, true };
tParamInt   firstsalvage           = { false, false, false, true };
tParamInt   functionkey[MaxFunctionName] = {
                                     { false, false, false },
                                     { false, false, false },
                                     { false, false, false, 5 },
                                     { false, false, false, 3 },
                                     { false, false, false, 0 },
                                     { false, false, false, 0 },
                                     { false, false, false, 1 },
                                     { false, false, false, 7 },
                                     { false, false, false, 0 },
                                     { false, false, false, 0 },
                                     { false, false, false, 9 }};
tParamFile  mainmenu_name          = { false, false, false };
tParamInt   mainmenu_visible       = { false, false, false, false };
tParamInt   okkey                  = { false, false, false, 1 };
tParamInt   numkey                 = { false, false, false, 0 };
tParamInt   functionline_b_e[2]    ={{ false, false, false, false },
                                     { false, false, false, false }};
tParamInt   functionline_num[2]    ={{ false, false, false, 14 },
                                     { false, false, false, 14 }};
tParamInt   verbose                = { false, false, false, false, true };

char        plugin_name[MaxFileName]  = "Undelete";
cPlugin     *plugin = NULL;
bool        PurgeRecording = false;
bool        SalvageRecording = false;
#if VDRVERSNUM >= 10311
cRecordings DeletedRecordings(true);
#else
cRecordings DeletedRecordings;
#endif
bool        MenuIsOpen = false;
char        *SVDRP_Process = NULL;
char        *WorkFilename = NULL;
time_t      NextRemoveThreadStart = 0;
int         KeyState = 0;

const char  *FunctionLine[4];
const char  *FunctionName[MaxFunctionName];
const char  *FunctionHotKey[MaxFunctionName];
char        *KeysName[MaxKeysName];

cRemoveThread oRemoveThread;

#ifdef UND_Debug
int cMenuRecordingSelectItem_nextID = 0;
int cMenuRecordingSelect_nextID = 0;
int cMenuUndelete_nextID = 0;
int cMenuSetupUndelete_nextID = 0;
int cMenuDispKey_nextID = 0;
#endif

void DisplaySetings(void)
{
  #define WriteSource(T)   plugin_name, T.r ? 'r' : ' ', T.c ? 'c' : ' '
  #define IsDefault(T)     WriteSource(T), T.u == T.d ? '*' : ' '
  #define IsDefaultS(T)    T.r ? 'r' : ' ', T.c ? 'c' : ' ', T.u == T.d ? '*' : ' '
  #define IsDefaultFile(T) WriteSource(T), strcmp(T.u, T.d) ? ' ' : '*'
  #define BoolValue(T)     IsDefault(T), T.u ? "yes" : "no"

  // var pos3
  if (verbose.u)
  {
    isyslog("%s: commandline_preference       = [ %c%c%c ] %s", BoolValue(commandline_preference));
    isyslog("%s: mainmenu_visible             = [ %c%c%c ] %s", BoolValue(mainmenu_visible));
    if (mainmenu_visible.u)
      isyslog("%s: mainmenu_name                = [ %c%c%c ] %s", IsDefaultFile(mainmenu_name), mainmenu_name.u);
    isyslog("%s: confirm purge                = [ %c%c%c ] %s", BoolValue(confirmpurge));
    isyslog("%s: confirm purge directory      = [ %c%c%c ] %s", BoolValue(confirmpurgedir));
    isyslog("%s: confirm purge all            = [ %c%c%c ] %s", BoolValue(confirmpurgeall));
    isyslog("%s: confirm salvage              = [ %c%c%c ] %s", BoolValue(confirmsalvage));
    isyslog("%s: confirm salvage directory    = [ %c%c%c ] %s", BoolValue(confirmsalvagedir));
    isyslog("%s: confirm salvage all          = [ %c%c%c ] %s", BoolValue(confirmsalvageall));
    isyslog("%s: purge lines in list          = [ %c%c%c ] %s", IsDefault(functionline_b_e[0]), FunctionLine[functionline_b_e[0].u]);
    if (functionline_b_e[0].u == 0x3)
      isyslog("%s: num entry for display        = [ %c%c%c ] %d", IsDefault(functionline_num[0]), functionline_num[0].u);
    isyslog("%s: salvage lines in list        = [ %c%c%c ] %s", IsDefault(functionline_b_e[1]), FunctionLine[functionline_b_e[1].u]);
    if (functionline_b_e[1].u == 0x3)
      isyslog("%s: num entry for display        = [ %c%c%c ] %d", IsDefault(functionline_num[1]), functionline_num[1].u);
    if ((functionline_b_e[0].u & functionline_b_e[1].u & 0x1) != 0x0 || (functionline_b_e[0].u & functionline_b_e[1].u & 0x2) != 0x0)
      isyslog("%s: inward lines                 = [ %c%c%c ] %s", IsDefault(firstsalvage), firstsalvage.u ? tr("Choise$salvage") : tr("Choise$purge"));
    for (int i = 2; i < MaxFunctionName; i++)
      isyslog("%s: key for %-20s = [ %c%c%c ] %s", plugin_name, FunctionName[i], IsDefaultS(functionkey[i]), KeysName[functionkey[i].u]);
    isyslog("%s: spec. function for OK-key    = [ %c%c%c ] %s", IsDefault(okkey), FunctionName[okkey.u]);
    isyslog("%s: spec. function for numerics  = [ %c%c%c ] %s", IsDefault(numkey), FunctionName[numkey.u]);
    isyslog("%s: verbose                      = [ %c%c%c ] %s", BoolValue(verbose));
  }

#ifdef UND_Debug1
  if (!verbose.u)
  {
    dsyslog("%s: commandline_preference       = [ %c%c%c ] %s", BoolValue(commandline_preference));
    dsyslog("%s: mainmenu_visible             = [ %c%c%c ] %s", BoolValue(mainmenu_visible));
    if (mainmenu_visible.u)
      dsyslog("%s: mainmenu_name                = [ %c%c%c ] %s", IsDefaultFile(mainmenu_name), mainmenu_name.u);
    dsyslog("%s: confirm purge                = [ %c%c%c ] %s", BoolValue(confirmpurge));
    dsyslog("%s: confirm purge directory      = [ %c%c%c ] %s", BoolValue(confirmpurgedir));
    dsyslog("%s: confirm purge all            = [ %c%c%c ] %s", BoolValue(confirmpurgeall));
    dsyslog("%s: confirm salvage              = [ %c%c%c ] %s", BoolValue(confirmsalvage));
    dsyslog("%s: confirm salvage directory    = [ %c%c%c ] %s", BoolValue(confirmsalvagedir));
    dsyslog("%s: confirm salvage all          = [ %c%c%c ] %s", BoolValue(confirmsalvageall));
    dsyslog("%s: purge lines in list          = [ %c%c%c ] %s", IsDefault(functionline_b_e[0]), FunctionLine[functionline_b_e[0].u]);
    if (functionline_b_e[0].u == 0x3)
      dsyslog("%s: num entry for display        = [ %c%c%c ] %d", IsDefault(functionline_num[0]), functionline_num[0].u);
    dsyslog("%s: salvage lines in list        = [ %c%c%c ] %s", IsDefault(functionline_b_e[1]), FunctionLine[functionline_b_e[1].u]);
    if (functionline_b_e[1].u == 0x3)
      dsyslog("%s: num entry for display        = [ %c%c%c ] %d", IsDefault(functionline_num[1]), functionline_num[1].u);
    if ((functionline_b_e[0].u & functionline_b_e[1].u & 0x1) != 0x0 || (functionline_b_e[0].u & functionline_b_e[1].u & 0x2) != 0x0)
      dsyslog("%s: inward lines                 = [ %c%c%c ] %s", IsDefault(firstsalvage), firstsalvage.u ? tr("Choise$salvage") : tr("Choise$purge"));
    for (int i = 2; i < MaxFunctionName; i++)
      dsyslog("%s: key for %-20s = [ %c%c%c ] %s", plugin_name, FunctionName[i], IsDefaultS(functionkey[i]), KeysName[functionkey[i].u]);
    dsyslog("%s: spec. function for OK-key    = [ %c%c%c ] %s", IsDefault(okkey), FunctionName[okkey.u]);
    dsyslog("%s: spec. function for numerics  = [ %c%c%c ] %s", IsDefault(numkey), FunctionName[numkey.u]);
    dsyslog("%s: verbose                      = [ %c%c%c ] %s", BoolValue(verbose));
  }
#endif

  #undef WriteSource
  #undef IsDefault
  #undef IsDefaultFile
  #undef BoolValue
}

void ExpandEnvironment(tParamFile *FileStruc)
{
  d2syslogi("ExpandEnvironment", "text=%s", FileStruc->u);
  char *s;
  char *p;
  strn0cpy(FileStruc->e, FileStruc->u, sizeof(FileStruc->e));
  while ((s = strstr(FileStruc->e, "$(")) || (s = strstr(FileStruc->e, "${")))
  {
    char c = *(s + 1) == '(' ? ')' : '}';
    p = strchr(s, c); // search closing ) or }
    if (p)
    {
      *p++ = 0;
      *s = 0;
      s += 2;
      char *e = getenv(s);
      if (e)
      {
        char *buffer = NULL;
        asprintf(&buffer, "%s%s%s", FileStruc->e, e, p);
        strn0cpy(FileStruc->e, buffer, sizeof(FileStruc->e));
        freenull(buffer);
      } else
      {
        esyslog("%s: environmentvariable '%s' not found path=%s", plugin_name, s, FileStruc->u);
        FileStruc->e[0] = 0;
      }
    } else
    {
      esyslog("%s: missing ')' after '$(' path=%s", plugin_name, FileStruc->u);
      FileStruc->e[0] = 0;
    }
  }
  while ((p = strstr(FileStruc->e, "//")))
    strcpy(p, p + 1);
  d2syslogi("ExpandEnvironment", "return=%s", FileStruc->e);
}

// --- cPluginUndelete ----------------------------------------------------------

class cPluginUndelete : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  void FreeKeyNames(void);
  int OSDLanguage;
  void TestAndSetOSDLanguage(void);
  bool ProcessArg(int argc, char *argv[]);
#ifdef UND_Debug
  int objID;  
#endif

public:
  cPluginUndelete(void);
  virtual ~cPluginUndelete();
  virtual const char *Version(void)                             { return VERSION; }
  virtual const char *Description(void)                         { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);

  virtual const char *MainMenuEntry(void)                       { return (mainmenu_visible.u) ? mainmenu_name.u : NULL; }
  virtual cOsdObject *MainMenuAction(void)                      { TestAndSetOSDLanguage(); return new cMenuUndelete; }

  virtual cMenuSetupPage *SetupMenu(void)                       { TestAndSetOSDLanguage(); return new cMenuSetupUndelete; }
  virtual bool SetupParse(const char *Name, const char *Value);

#ifdef HAVE_SVDRP
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
#endif
  };

cPluginUndelete::cPluginUndelete(void)
{
#ifdef UND_Debug
  objID = 1;
  d2syslog("cPluginUndelete::cPluginUndelete", "%s", "");
#endif
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  plugin = this;
  memset(KeysName, sizeof(KeysName), 0);
}

cPluginUndelete::~cPluginUndelete()
{
  d2syslog("cPluginUndelete::~cPluginUndelete", "%s", "");
  plugin = NULL;
  FreeKeyNames();
  freenull(SVDRP_Process);
  freenull(WorkFilename);
  // Clean up after yourself!
}

void cPluginUndelete::FreeKeyNames(void)
{
  for (int i = 0; i < MaxKeysName; i++)
    freenull(KeysName[i]);
}

void cPluginUndelete::TestAndSetOSDLanguage(void)
{
  d2syslog("cPluginUndelete::TestAndSetOSDLanguage", "OSDLanguage=%d", Setup.OSDLanguage);
  if (OSDLanguage != Setup.OSDLanguage) {
    FreeKeyNames();
    OSDLanguage = Setup.OSDLanguage;
    FunctionLine[0] = tr("Choise$none");
    FunctionLine[1] = tr("Choise$top");
    FunctionLine[2] = tr("Choise$bottom");
    FunctionLine[3] = tr("Choise$top and bottom");
    FunctionName[0] = tr("Choise$none");
    FunctionName[1] = tr("Choise$open/summary");
    FunctionName[2] = tr("Choise$purge");
    FunctionName[3] = tr("Choise$salvage");
    FunctionName[4] = tr("Choise$purge all");
    FunctionName[5] = tr("Choise$salvage all");
    FunctionName[6] = tr("Choise$open");
    FunctionName[7] = tr("Choise$summary");
    FunctionName[8] = tr("Choise$switch hotkeys to 1");
    FunctionName[9] = tr("Choise$switch hotkeys to 2");
    FunctionName[10] = tr("Choise$Display keys");
    FunctionHotKey[2] = tr("Display$purge");
    FunctionHotKey[3] = tr("Display$salvage");
    FunctionHotKey[4] = tr("Display$purge all");
    FunctionHotKey[5] = tr("Display$salvage all");
    FunctionHotKey[6] = tr("Open");
    FunctionHotKey[7] = tr("Summary");
    FunctionHotKey[8] = tr("Display$<--1");
    FunctionHotKey[9] = tr("Display$2-->");
    FunctionHotKey[10] = tr("Display$disp. keys");
    KeysName[0] = strdup(tr("Choise$none"));
    for (int i = 1; i <= 2; i++) {
      asprintf(&KeysName[i], "%s (%d)", tr("Red"), i);
      asprintf(&KeysName[i+2], "%s (%d)", tr("Green"), i);
      asprintf(&KeysName[i+4], "%s (%d)", tr("Yellow"), i);
      asprintf(&KeysName[i+6], "%s (%d)", tr("Blue"), i);
    }
    for (int i = 0; i <= 9; i++)
      asprintf(&KeysName[i+9], "%d", i);
    for (int i = 1; i <= 9; i++)
      asprintf(&KeysName[i+18], "%s %d", tr("Choise$User"), i);
  }
}

bool cPluginUndelete::ProcessArg(int argc, char *argv[])
{
  int c;
  static struct option long_options[] = {
    { "confirm_purge_rec",         no_argument,       NULL, 'c' },
    { "not_confirm_purge_rec",     no_argument,       NULL, 'C' },
    { "confirm_purge_dir",         no_argument,       NULL, 'd' },
    { "not_confirm_purge_dir",     no_argument,       NULL, 'D' },
    { "confirm_purge_all",         no_argument,       NULL, 'e' },
    { "not_confirm_purge_all",     no_argument,       NULL, 'E' },
    { "visible_in_mainmenu",       no_argument,       NULL, 'm' },
    { "hide_in_mainmenu",          no_argument,       NULL, 'M' },
    { "mainmenu_name",             required_argument, NULL, 'n' },
    { "confirm_salvage_rec",       no_argument,       NULL, 's' },
    { "not_confirm_salvage_rec",   no_argument,       NULL, 'S' },
    { "confirm_salvage_dir",       no_argument,       NULL, 't' },
    { "not_confirm_salvage_dir",   no_argument,       NULL, 'T' },
    { "confirm_salvage_all",       no_argument,       NULL, 'u' },
    { "not_confirm_salvage_all",   no_argument,       NULL, 'U' },
    { "verbose",                   no_argument,       NULL, 'v' },
    { "noverbose",                 no_argument,       NULL, 'V' },
    { "nosetup_commandline",       no_argument,       NULL, 1   },
    { "ns_commandline",            no_argument,       NULL, 1   },
    { NULL }
  };

  if (argc >= 1)
    strn0cpy(plugin_name, argv[0], sizeof(plugin_name));

  for (c = 1; c < argc; c++)
    dsyslog("%s: parameter%d=%s", plugin_name, c, argv[c]);

  #define Setvalue(T) T.c = true; T.u
  #define SetvalueFile(T) T.c = true; strn0cpy(T.u, optarg, sizeof(T.u))

  while ((c = getopt_long(argc, argv, "cCdDmMn:sStTvV", long_options, NULL)) != -1)
  {
    // var pos4
    switch (c)
    {
      case 1:   commandline_preference.h = true;
                break;
      case 'c': Setvalue(confirmpurge) = true;
                break;
      case 'C': Setvalue(confirmpurge) = false;
                break;
      case 'd': Setvalue(confirmpurgedir) = true;
                break;
      case 'D': Setvalue(confirmpurgedir) = false;
                break;
      case 'e': Setvalue(confirmpurgeall) = true;
                break;
      case 'E': Setvalue(confirmpurgeall) = false;
                break;
      case 'm': Setvalue(mainmenu_visible) = true;
                break;
      case 'M': Setvalue(mainmenu_visible) = false;
                break;
      case 'n': SetvalueFile(mainmenu_name);
                Setvalue(mainmenu_visible) = true;
                break;
      case 's': Setvalue(confirmsalvage) = true;
                break;
      case 'S': Setvalue(confirmsalvage) = false;
                break;
      case 't': Setvalue(confirmsalvagedir) = true;
                break;
      case 'T': Setvalue(confirmsalvagedir) = false;
                break;
      case 'u': Setvalue(confirmsalvageall) = true;
                break;
      case 'U': Setvalue(confirmsalvageall) = false;
                break;
      case 'v': Setvalue(verbose) = true;
                break;
      case 'V': Setvalue(verbose) = false;
                break;
      default:  return false;
    }
  }

  #undef Setvalue
  #undef SetvalueFile

  if (optind < argc && argv[optind][0] == '@')
  {
    strn0cpy(undeleteconfigfile.u, &argv[optind][1], sizeof(undeleteconfigfile.u));
    optind++;
  }
  return optind >= argc;
}

const char *cPluginUndelete::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.            column 80 --> |
  return tr("  -m        --visible_in_mainmenu     Show the plugin in the mainmenu\n"
            "  -M        --hide_in_mainmenu        Hide the plugin in the mainmenu\n"
            "                                      You can start the Plugin in the setupmenu\n"
            "  -n Name   --mainmenu_name=Name      Select Name for entry in the mainmenu\n"
            "                                      (set also -m)\n"
            "  -c        --confirm_purge_rec       show confirmation for purge recording\n"
            "  -C        --not_confirm_purge_rec   purge recording without confirmation\n"
            "  -d        --confirm_purge_dir       show confirmation for purge directory\n"
            "  -D        --not_confirm_purge_dir   purge directory without confirmation\n"
            "  -e        --confirm_purge_all       show confirmation for purge all function\n"
            "  -E        --not_confirm_purge_all   purge all function without confirmation\n"
            "  -s        --confirm_salvage_rec     show confirmation for salvage recording\n"
            "  -S        --not_confirm_salvage_rec salvage recording without confirmation\n"
            "  -t        --confirm_salvage_dir     show confirmation for salvage directory\n"
            "  -T        --not_confirm_salvage_dir salvage directory without confirmation\n"
            "  -u        --confirm_salvage_all     show confirmation for salvage all function\n"
            "  -U        --not_confirm_salvage_all salvage all function without confirmation\n"
            "  -v        --verbose                 Enable more logging\n"
            "  -V        --noverbose               Disable more loggig\n"
            "            --nosetup_commandline     Hide the 'Preferr Command Line Parameter'\n"
            "                                      form setup-menu\n"
            "  @config-file                        Read also parameters from this file"
            "\n"
            "  note: recording means deleted recordings (there not show in VDR)");

/* only for translation:
            "  -m        --visible_in_mainmenu     Plugin wird im Hautmenü angezeigt\n"
            "  -M        --hide_in_mainmenu        kein Eintrag im Hauptmenü für das Plugin\n"
            "                                      es kann über das Setup-Menü gestartet werden\n"
            "  -n Name   --mainmenu_name=Name      Name für den Eintrag im Hauptmenü\n"
            "                                      (diese Option setzt auch -m)\n"
            "  -c        --confirm_purge_rec       Bestätigung für das Löschen von Aufnahmen\n"
            "  -C        --not_confirm_purge_rec   keine Bestätigung (löschen Aufnahmen)\n"
            "  -d        --confirm_purge_dir       Bestätigung beim Löschen von Verzeichnissen\n"
            "  -D        --not_confirm_purge_dir   keine Bestätigung (löschen Verzeichnissen)\n"
            "  -e        --confirm_purge_all       Bestätigung beim "alles Löschen"\n"
            "  -E        --not_confirm_purge_all   "alles Löschen" ohne Bestätigung\n"
            "  -s        --confirm_salvage_rec     Bestätigung Wiederherstellung von Aufnahmen\n"
            "  -S        --not_confirm_salvage_rec keine Best. Wiederherstellung von Aufnahmen\n"
            "  -t        --confirm_salvage_dir     Bestätigung Wiederherstellung von Verzei.\n"
            "  -T        --not_confirm_salvage_dir keine Best. Wiederherstellung von Verzei.\n"
            "  -u        --confirm_salvage_all     Bestätigung beim "alles Wiederherstellen"\n"
            "  -U        --not_confirm_salvage_all "alles Wiederherstellen" ohne Bestätigung\n"
            "  -v        --verbose                 erweiterte Protokollierung aktivieren\n"
            "  -V        --noverbose               keine erweiterte Protokollierung\n"
            "            --nosetup_commandline     Option 'Kommandozeile hat Vorrang'\n"
            "                                      im Setup-Menü verbergen\n"
            "  @config-file                        Komandozeilenoptionen auch aus der angegebenen\n"
            "                                      Datei lesen\n"
            "\n"
            "  Hinweis: Mit Aufnahmen sind hier die gelöschten Aufnahmen gemeint"
*/
         // free: a b e f g h i j k l o p q r u w x y z
}

bool cPluginUndelete::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return ProcessArg(argc, argv);
}

bool cPluginUndelete::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  TestAndSetOSDLanguage();

  d1syslog("cPluginUndelete::Start", "undeleteconfigfile=%s", undeleteconfigfile.u);

  char *p;
  char *q = strdup(ConfigDirectory());
  asprintf(&p, "%s%s%s", q, *(q + strlen(q) - 1) == '/' ? "" : "/", undeleteconfigfile.u);
  if (!access(undeleteconfigfile.u, F_OK) && !access(undeleteconfigfile.u, R_OK) || !access(p, F_OK) && !access(p, R_OK))
  {
    #define MAXARGS 100
    int fargc = 1;
    char *fargv[MAXARGS];
    cReadLine ReadLine;
    bool done;
    FILE *f;

    if (!access(undeleteconfigfile.u, F_OK))
    {
      f = fopen(undeleteconfigfile.u, "r");
      d1syslog("cPluginUndelete::Start", "open undeleteconfigfile=%s", undeleteconfigfile.u);
    } else
    {
      f = fopen(p, "r");
      d1syslog("cPluginUndelete::Start", "open undeleteconfigfile=%s", p);
    }
    free(p);
    free(q);
    if (!f)
    {
      esyslog("%s: ERROR: cannot open config file: [%s]%s", plugin_name, ConfigDirectory(), undeleteconfigfile.u);
      return false;
    }
    while ((p = compactspace(ReadLine.Read(f))) != NULL)
    {
      q = NULL;
      done = false;
      while (!done)
      {
        if (!q)
          q = p;
        switch (*p)
        {
          case '\\': strcpy(p, p + 1);
                     if (*p)
                       p++;
                     else
                     {
                       esyslog("%s: ERROR: missing character after \\", plugin_name);
                       return false;
                     }
                     break;
          case '"':
          case '\'': if ((p = SkipQuote(p)) == NULL)
                       return false;
                     break;
          default:   if (!*p || isspace(*p))
                     {
                       done = !*p;
                       *p = 0;
                       if (q)
                       {
                         if (fargc < MAXARGS - 1)
                         {
                           if (*q != '#')
                             fargv[fargc++] = strdup(q);
                         } else
                         {
                           esyslog("%s: ERROR: plugin argument list too long", plugin_name);
                           return false;
                         }
                         q = NULL;
                       }
                     }
                     if (!done)
                       p = *p ? p + 1 : skipspace(p + 1);
        }
      }
    }
    fclose(f);
    fargv[0] = strdup(plugin_name);
    fargv[fargc] = NULL;
    optind = 0; // to reset the getopt() data
    if (fargc > 1)
      if (!ProcessArg(fargc, fargv))
      {
        esyslog("%s: ERROR: cannot parse config file: [%s]%s", plugin_name, ConfigDirectory(), undeleteconfigfile.u);
        return false;
      }
      while(fargc) free(fargv[--fargc]);
  } else
  {
    free(p);
    free(q);
    if (strcmp(undeleteconfigfile.u, undeleteconfigfile.d))
    {
      esyslog("%s: ERROR: config file not found: [%s]%s", plugin_name, ConfigDirectory(), undeleteconfigfile.u);
      return false;
    } else if (verbose.u)
      isyslog("%s: INFO: config file not found: [%s]%s", plugin_name, ConfigDirectory(), undeleteconfigfile.u);
  }

  strn0cpy(mainmenu_name.d, tr("Menu$Undelete recordings"), sizeof(mainmenu_name.d));
  
  // default parameter                --> d_
  // read parameter from commandline  --> c_  (data in u_)
  // value parameter from config-file --> s_
  // read parameter from config-file  --> r_
  // paramater used                   --> u_

  #define SetParam(T) { if (!T.c || (!commandline_preference.u && T.r)) T.u = (!T.c && !T.r) ? T.d : T.s; }
  #define SetParamFile(T) { if (!T.c || (!commandline_preference.u && T.r)) strcpy(T.u, (!T.c && !T.r) ? T.d : T.s); }

  commandline_preference.u = commandline_preference.r ? commandline_preference.s : commandline_preference.d;

  // var pos5
  SetParamFile (undeleteconfigfile);
  SetParam     (confirmpurge);
  SetParam     (confirmpurgedir);
  SetParam     (confirmpurgeall);
  SetParam     (confirmsalvage);
  SetParam     (confirmsalvagedir);
  SetParam     (confirmsalvageall);
  SetParam     (firstsalvage);
  for (int i = 2; i < MaxFunctionName; i++)
    SetParam   (functionkey[i]);
  SetParamFile (mainmenu_name);
  SetParam     (mainmenu_visible);
  SetParam     (numkey);
  SetParam     (okkey);
  for (int i = 0; i < 2; i++)
  {
    SetParam   (functionline_b_e[i]);
    SetParam   (functionline_num[i]);
  }
  SetParam     (verbose);

  #undef SetParam
  #undef SetParamFile

  if (verbose.u)
    isyslog("%s: Start", plugin_name);
  DisplaySetings();
  return true;
}

void cPluginUndelete::Housekeeping(void)
{
  d2syslog("cPluginUndelete::Housekeeping", "NextRemoveThreadStart=%d (%d)", (int)NextRemoveThreadStart, (int)(NextRemoveThreadStart > 0 ? NextRemoveThreadStart - time(NULL) : 0));
  if (NextRemoveThreadStart > 0 && NextRemoveThreadStart <= time(NULL))
  {
    if (oRemoveThread.ActiveWithCancel())
      oRemoveThread.Start();
    NextRemoveThreadStart = 0;
  }
}

bool cPluginUndelete::SetupParse(const char *Name, const char *Value)
{
  #define SetParam(T) { T.r = true; T.s = strtol(Value, NULL, 0); }
  #define SetParamChar(T) { T.r = true; strn0cpy(T.s, Value, sizeof(T.s)); }

  // Parse your own setup parameters and store their values.
  dsyslog("%s: Setupparameter %s=%s", plugin_name, Name, Value);
  // var pos6
  if      (!strcasecmp(Name, "commandline"))   SetParam     (commandline_preference)
  else if (!strcasecmp(Name, "confpurge1"))    SetParam     (confirmpurge)
  else if (!strcasecmp(Name, "confpurge2"))    SetParam     (confirmpurgedir)
  else if (!strcasecmp(Name, "confpurge3"))    SetParam     (confirmpurgeall)
  else if (!strcasecmp(Name, "confsalvage1"))  SetParam     (confirmsalvage)
  else if (!strcasecmp(Name, "confsalvage2"))  SetParam     (confirmsalvagedir)
  else if (!strcasecmp(Name, "confsalvage3"))  SetParam     (confirmsalvageall)
  else if (!strcasecmp(Name, "firstsalvage"))  SetParam     (firstsalvage)
  else if (!strcasecmp(Name, "functionkey02")) SetParam     (functionkey[2])
  else if (!strcasecmp(Name, "functionkey03")) SetParam     (functionkey[3])
  else if (!strcasecmp(Name, "functionkey04")) SetParam     (functionkey[4])
  else if (!strcasecmp(Name, "functionkey05")) SetParam     (functionkey[5])
  else if (!strcasecmp(Name, "functionkey06")) SetParam     (functionkey[6])
  else if (!strcasecmp(Name, "functionkey07")) SetParam     (functionkey[7])
  else if (!strcasecmp(Name, "functionkey08")) SetParam     (functionkey[8])
  else if (!strcasecmp(Name, "functionkey09")) SetParam     (functionkey[9])
  else if (!strcasecmp(Name, "functionkey10")) SetParam     (functionkey[10])
  else if (!strcasecmp(Name, "name"))          SetParamChar (mainmenu_name)
  else if (!strcasecmp(Name, "visible"))       SetParam     (mainmenu_visible)
  else if (!strcasecmp(Name, "numkey"))        SetParam     (numkey)
  else if (!strcasecmp(Name, "okkey"))         SetParam     (okkey)
  else if (!strcasecmp(Name, "purgebe"))       SetParam     (functionline_b_e[0])
  else if (!strcasecmp(Name, "purgenum"))      SetParam     (functionline_num[0])
  else if (!strcasecmp(Name, "salvagebe"))     SetParam     (functionline_b_e[1])
  else if (!strcasecmp(Name, "salvagenum"))    SetParam     (functionline_num[1])
  else if (!strcasecmp(Name, "verbose"))       SetParam     (verbose)
  else
    return false;
  return true;

  #undef SetParam
  #undef SetParamChar
}

#ifdef HAVE_SVDRP
const char **cPluginUndelete::SVDRPHelpPages(void)
{
  static const char *HelpPages[] = {
  "LSTD    [ <number> ]\n"
  "        List deleted recordings. Without option, all deleted recordings are\n"
  "        listed.\n"
  "        Otherwise the information for the given deleted recording is listed.",
  "SIZED   [ -h ]\n"
  "        Caclulate the size of deleted recordings. Result is in Bytes or\n"
  "        with option -h in human readable format.",
  "RMDIR\n"
  "        Start remove empty directory in the background",
  "PURGE   ALL | <number>\n"
  "        purge the selected deleted recording (number see LSTD)",
  "SALVAGE ALL | <number>\n"
  "        salvage the selected deleted recording (number see LSTD)",
  NULL
  };
  return HelpPages;
}

cString cPluginUndelete::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
#define CMD(c) (strcasecmp(Command, c) == 0)

  d1syslog("cPluginUndelete::SVDRPCommand", "Command=%s Option=%s ReplyCode=%d MenuIsOpen=%s", Command ? Command : "(NULL)", Option ? Option : "(NULL)", ReplyCode, MenuIsOpen ? "yes" : "no");
  if (MenuIsOpen)
  {
    ReplyCode = 554;
    return "OSD-Menu is currently open";
  }
  if        CMD("LSTD")
  {
    ReplyCode = 550;
    if (*Option)
    {
      if (isnumber(Option))
      {
        cRecording *recording = DeletedRecordings.Get(atoi(Option) - 1);
        if (recording)
        {
#if VDRVERSNUM >= 10325
          const char *summary = NULL;
          if (recording->Info())
            summary = recording->Info()->Description();
#else 
          const char *summary = recording->Summary();
#endif
          if (summary && *summary)
          {
            ReplyCode = 902;
            return summary;
          } else
            return "no summary availabe";
        } else
          return cString::sprintf("deleted recording \"%s\" not found%s", Option, DeletedRecordings.Count() ? "" : " (use LSTD before get summary)");
      } else
      {
        ReplyCode = 501;
        return cString::sprintf("Error in deleted recording number \"%s\"", Option);
      }
    } else
    {
      // list deleted recordings
#if VDRVERSNUM >= 10311
      DeletedRecordings.Load();
#else
      DeletedRecordings.Load(true);
#endif
      DeletedRecordings.Sort();
      freenull(SVDRP_Process);
      char *list = NULL;
      for (cRecording *recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording))
      {
        char *newline;
	asprintf(&newline, "%s%d %s\n", list ? list : "", recording->Index() + 1, recording->Title(' ', true, -1));
        freenull(list);
        list = newline;
      }
      if (list)
      {
        ReplyCode = 901;
        return cString(list, true);
      } else
        return "No deleted recordings available";
    }
  } else if CMD("SIZED")
  {
    long long llsize = 0;
    if (DeletedRecordings.Count())
    {
      for (cRecording *recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording))
      {
        char *process = NULL;
        if (SVDRP_Process)
        {
          char *temp;
          asprintf(&temp, "#%d#", recording->Index() + 1);
          process = strstr(SVDRP_Process, temp);
          free(temp);
        }
        if (!process)
          GetVDRSize(recording->FileName(), llsize);
      }
    } else
    {
#if VDRVERSNUM >= 10311
      DeletedRecordings.Load();
#else
      DeletedRecordings.Load(true);
#endif
      for (cRecording *recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording))
        GetVDRSize(recording->FileName(), llsize);
      DeletedRecordings.Clear();
    }
    if (*Option)
      if (strcasecmp(Option, "-h"))
      {
        ReplyCode = 501;
        return cString::sprintf("Error in parameter \"%s\" (only -h allow)", Option);
      } else
      {
        float llsizetmp = llsize;
        const char* strUnit[5] = { tr("Bytes"), "KB", "MB", "GB", "TB" };
        int Unit;
        for(Unit = 0; (llsizetmp > 1024) && (Unit < 5); Unit++)
          llsizetmp /= 1024;
        int decimalplaces = Unit < 2 ? 0 : Unit > 2 ? 2 : 1;
        ReplyCode = 904;
        return cString::sprintf("size of deleted recordings are %1.*f %s", decimalplaces, llsizetmp, strUnit[Unit]);
      }
    else
    {
      ReplyCode = 903;
      return cString::sprintf("%lld bytes in deleted recordings", llsize);
    }
  } else if CMD("RMDIR")
  {
    NextRemoveThreadStart = 0;
    if (oRemoveThread.ActiveWithCancel())
    {
      ReplyCode = 554;
      return "thread for remove empty directory alredy active";
    } else
    {
      oRemoveThread.Start();
      ReplyCode = 905;
      return "thread for remove empty directory is started";
    }
  } else if CMD("PURGE")
  {
    ReplyCode = 550;
    if (*Option)
    {
      if (isnumber(Option))
      {
        int recnumber = atoi(Option);
        char *process = NULL;
        if (SVDRP_Process)
        {
          char *temp;
          asprintf(&temp, "#%d#", recnumber);
          process = strstr(SVDRP_Process, temp);
          free(temp);
        }
        if (process)
          return cString::sprintf("deleted recording \"%s\" is already %s", Option, *--process == 'P' ? "purged" : "salvaged");
        else
        {
          cRecording *recording = DeletedRecordings.Get(recnumber - 1);
          if (recording)
          {
            if (verbose.u)
              isyslog("%s: purge deleted recording (%s)", plugin_name, recording->FileName());
            if (!RemoveVideoFile(recording->FileName()))
            {
              esyslog("%s: Error while remove deleted recording (%s)", plugin_name, recording->FileName());
              return cString::sprintf("error while purge the deleted recording \"%s\"  [%s]", Option, recording->Title());
            }
            NextRemoveThreadStart = time(NULL) + 600;
            char *temp;
            asprintf(&temp, "%sP#%d#", SVDRP_Process ? SVDRP_Process : "", recnumber);
            freenull(SVDRP_Process);
            SVDRP_Process = temp;
            ReplyCode = 906;
            return cString::sprintf("purge of deleted recording \"%s\" [%s] is successful", Option, recording->Title());
          } else
            return cString::sprintf("deleted recording \"%s\" not found%s", Option, DeletedRecordings.Count() ? "" : " (use LSTD before purge)");
        }
      } else if (!strcasecmp(Option, "ALL"))
      {
        if (!DeletedRecordings.Count())
        {
#if VDRVERSNUM >= 10311
          DeletedRecordings.Load();
#else
          DeletedRecordings.Load(true);
#endif
          DeletedRecordings.Sort();
          freenull(SVDRP_Process);
          if (!DeletedRecordings.Count())
            return "No deleted recordings available";
        }
        int replycode = 0;
        char *list = NULL;
        for (cRecording *recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording))
        {
          replycode = 0;
          cString reply = SVDRPCommand("purge", (const char *)cString::sprintf("%d", recording->Index() + 1), replycode);
          if (replycode < 900)
          {
            char *newline;
            asprintf(&newline, "%s%s\n", list ? list : "", (const char *)reply);
            freenull(list);
            list = newline;
          }
        }
        if (!oRemoveThread.ActiveWithCancel())
          oRemoveThread.Start();
        DeletedRecordings.Clear();
        freenull(SVDRP_Process);
#if VDRVERSNUM >= 10311
        DeletedRecordings.Load();
#else
        DeletedRecordings.Load(true);
#endif
        bool recordingsavailable = DeletedRecordings.Count();
        DeletedRecordings.Clear();
        if (recordingsavailable)
          return cString::sprintf("%snot all deleted recordings are purged (see lstd)", list ? (const char *)cString(list, true) : "");
        else if (list)
        {
          ReplyCode = 907;
          return cString::sprintf("%swarings while purge deleted recordings", (const char *)cString(list, true));
        } else
        {
          ReplyCode = 908;
          return "all deleted recordings are purged and thread for remove empty directory is started";
        }
      } else
      {
        ReplyCode = 501;
        return cString::sprintf("Error in deleted recording number \"%s\"", Option);
      }
    } else
    {
      ReplyCode = 501;
      return "number of deleted recoding for purge missing";
    }
  } else if CMD("SALVAGE")
  {
    ReplyCode = 550;
    if (*Option)
    {
      if (isnumber(Option))
      {
        int recnumber = atoi(Option);
        char *process = NULL;
        if (SVDRP_Process)
        {
          char *temp;
          asprintf(&temp, "#%d#", recnumber);
          process = strstr(SVDRP_Process, temp);
          free(temp);
        }
        if (process)
          return cString::sprintf("deleted recording \"%s\" is already %s", Option, *--process == 'P' ? "purged" : "salvaged");
        else
        {
          cRecording *recording = DeletedRecordings.Get(recnumber - 1);
          if (recording)
          {
            if (verbose.u)
              isyslog("%s: undelete recording (%s)", plugin_name, recording->FileName());
            cString NewName = recording->FileName();
            char *ext = strrchr(NewName, '.');
            if (!strcmp(ext, DELEXT))
            {
              strncpy(ext, RECEXT, strlen(ext));
              if (!access(NewName, F_OK))
              {
                if (verbose.u)
                  isyslog("%s: Recording with the same name exists (%s)", plugin_name, (const char *)NewName);
                return cString::sprintf("recording with the same name exists \"%s\" [%s]", Option, recording->Title());
              } else
              {
                if (!RenameVideoFile(recording->FileName(), (const char *)NewName))
                {
                  esyslog("%s: Error while rename deleted recording (%s) to (%s)", plugin_name, recording->FileName(), (const char *)NewName);
                  return cString::sprintf("error while rename deleted recording \"%s\" [%s]", Option, recording->Title());
                }
              }
            }
            char *temp;
            asprintf(&temp, "%sS#%d#", SVDRP_Process ? SVDRP_Process : "", recnumber);
            free(SVDRP_Process);
            SVDRP_Process = temp;
            cIndexFile *index = new cIndexFile(NewName, false);
            int LastFrame = index->Last() - 1;
            if (LastFrame > 0) {
              uchar FileNumber = 0;
              int FileOffset = 0;
              index->Get(LastFrame, &FileNumber, &FileOffset);
              delete index;
              if (FileNumber == 0)
                return cString::sprintf("error while read last filenumber for \"%s\" [%s]", Option, recording->Title());
              for (int i = 1; i <= FileNumber; i++) {
                asprintf(&temp, "%s/%03d.vdr", (const char *)NewName, i);
                if (access(temp, R_OK) != 0) {
                  free(temp);
                  return cString::sprintf("error accessing vdrfile %03d for \"%s\" [%s]", i, Option, recording->Title());
                }
                free(temp);
              }
            } else {
              delete index;
              return cString::sprintf("error accessing indexfile for \"%s\" [%s]", Option, recording->Title());
            }
            ReplyCode = 909;
            return cString::sprintf("salvage of deleted recording \"%s\" [%s] is successful", Option, recording->Title());
          } else
            return cString::sprintf("deleted recording \"%s\" not found%s", Option, DeletedRecordings.Count() ? "" : " (use LSTD before salvage)");
        }
      } else if (!strcasecmp(Option, "ALL"))
      {
        if (!DeletedRecordings.Count())
        {
#if VDRVERSNUM >= 10311
          DeletedRecordings.Load();
#else
          DeletedRecordings.Load(true);
#endif
          DeletedRecordings.Sort();
          freenull(SVDRP_Process);
          if (!DeletedRecordings.Count())
            return "No deleted recordings available";
        }
        int replycode = 0;
        char *list = NULL;
        for (cRecording *recording = DeletedRecordings.First(); recording; recording = DeletedRecordings.Next(recording))
        {
          replycode = 0;
          cString reply = SVDRPCommand("salvage", (const char *)cString::sprintf("%d", recording->Index() + 1), replycode);
          if (replycode < 900)
          {
            char *newline;
            asprintf(&newline, "%s%s\n", list ? list : "", (const char *)reply);
            freenull(list);
            list = newline;
          }
        }
        DeletedRecordings.Clear();
        freenull(SVDRP_Process);
#if VDRVERSNUM >= 10311
        DeletedRecordings.Load();
#else
        DeletedRecordings.Load(true);
#endif
        bool recordingsavailable = DeletedRecordings.Count();
        DeletedRecordings.Clear();
        if (recordingsavailable)
          return cString::sprintf("%snot all deleted recordings are salvaged (see lstd)", list ? (const char *)cString(list, true) : "");
        else if (list)
        {
          ReplyCode = 910;
          return cString::sprintf("%swarings while salvage deleted recordings", (const char *)cString(list, true));
        } else
        {
          ReplyCode = 911;
          return "all deleted recordings are salvaged";
        }
      } else
      {
        ReplyCode = 501;
        return cString::sprintf("Error in deleted recording number \"%s\"", Option);
      }
    } else
    {
      ReplyCode = 501;
      return "number of deleted recoding for purge missing";
    }
  }
  return NULL;
#undef CMD
}
#endif

void cRemoveThread::Action(void)
{
  d1syslogi("cRemoveThread::Action", "RemoveEmptyVideoDirectories thread started (pid=%d)", getpid());
  RemoveEmptyVideoDirectories();
  d1syslogi("cRemoveThread::Action", "RemoveEmptyVideoDirectories thread ended (pid=%d)", getpid());
}

bool cRemoveThread::ActiveWithCancel(void)
{
#if VDRVERSNUM >= 10318
  return Active();
#else
  if (Active())
    return true;
  Cancel(1); // reset the var "running" to restart the thread, fixed in 1.3.18
  return false;
#endif
}

VDRPLUGINCREATOR(cPluginUndelete); // Don't touch this!
