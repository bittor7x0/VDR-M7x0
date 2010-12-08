/*
 * playlist: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: playlist.c 0.2 2004/10/08 02:34:00 hflor Exp $
 */

#include "playlist.h"
#include "menuplaylists.h"
#include "menucontrol.h"
#include "menusetup.h"
#include "i18n.h"
#include "vdrtools.h"
#include <getopt.h>
#include <ctype.h>
#include <vdr/recording.h>

static const char *VERSION           = "0.0.2";
static const char *DESCRIPTION       = "playlist for recordings";

/* TODO
option playonlynewrecords
*/
// Global variables that control the overall behaviour:

// var pos2
tParamFile  playlistconfigfile       = { false, false, false, "playlist.conf", "playlist.conf" };
#ifdef HAVE_ICONPATCH
tParamChar  charentry                = { false, false, false, 236 };
tParamChar  charfolder               = { false, false, false, 237 };
tParamChar  charrecord               = { false, false, false, 249 };
#else
tParamChar  charentry                = { false, false, false, 'e' };
tParamChar  charfolder               = { false, false, false, 'f' };
tParamChar  charrecord               = { false, false, false, 'r' };
#endif
tParamInt   commandline_preference   = { false, false, false, true };
tParamFile  lastplaylist             = { false, false, false, "" };
tParamFile  mainmenu_name            = { false, false, false, "Playlist" };
tParamInt   mainmenu_visible         = { false, false, false, true };
tParamInt   confirmdelplentry        = { false, false, false, true };
tParamInt   okstartplay              = { false, false, false, true };
tParamInt   deleterecords            = { false, false, false, true };
tParamFile  pathplaylists            = { false, false, false, "" };
tParamInt   startoption              = { false, false, false, 0 };
tParamInt   storeplaylist            = { false, false, false, 0 };
tParamInt   timeoutreadrecords       = { false, false, false, 60 };
tParamInt   verbose                  = { false, false, false, false, true };
tParamInt   PL_options[Option_max]   ={{ false, false, false, true },                  // confirmdeleterecord
                                       { false, false, false, false },                 // confirmstartnewrecord
                                       { false, false, false, true },                  // deletefromlist
                                       { false, false, false, false },                 // deleterecord
                                       { false, false, false, true },                  // jumpmark
                                       { false, false, false, PlayOptions_firstnew },  // playoption1
                                       { false, false, false, PlayOptions_selectpos }, // playoption2
                                       { false, false, false, false },                 // playonlynew
                                       { false, false, false, true },                  // searchnewrecord
                                       { false, false, false, true }};                 // searchrecordsub

char        plugin_name[MaxFileName]  = "Playlist";
char        DisplayStatus[MAXOSDTEXTWIDTH] = "";
const char  *NoYesDefault[NoYesDefault_max];
const char  *StartOptions[Start_max];
const char  *PlayOptions[PlayOptions_max];
char        *FileNameCharsAllowed = NULL;
char        *AllCharsAllowed = NULL;
char        *LastSelectedRecord = NULL;
bool        ControlMenuIsOpen = false;
#if VDRVERSNUM < 10311
cRecordings Recordings;
time_t      LoadTime_Recordings;
#endif

void DisplaySetings(void)
{
  #define WriteSource(T)   plugin_name, T.r ? 'r' : ' ', T.c ? 'c' : ' '
  #define IsDefault(T)     WriteSource(T), T.u == T.d ? '*' : ' '
  #define IsDefaultChar(T) WriteSource(T), strcmp(T.u, T.d) ? ' ' : '*'
  #define BoolValue(T)     IsDefault(T), T.u ? "yes" : "no"

  // var pos3
  ExpandEnvironment(&pathplaylists);
  if (verbose.u)
  {
    isyslog("%s: commandline_preference = [ %c%c%c ] %s", BoolValue(commandline_preference));
    isyslog("%s: mainmenu_visible       = [ %c%c%c ] %s", BoolValue(mainmenu_visible));
    if (mainmenu_visible.u)
      isyslog("%s: mainmenu_name          = [ %c%c%c ] %s", IsDefaultChar(mainmenu_name), mainmenu_name.u);
  }

#ifdef PL_Debug1
  if (!verbose.u)
  {
    dsyslog("%s: commandline_preference = [ %c%c%c ] %s", BoolValue(commandline_preference));
    dsyslog("%s: mainmenu_visible       = [ %c%c%c ] %s", BoolValue(mainmenu_visible));
    if (mainmenu_visible.u)
      dsyslog("%s: mainmenu_name          = [ %c%c%c ] %s", IsDefaultChar(mainmenu_name), mainmenu_name.u);
  }
#endif

  #undef WriteSource
  #undef IsDefault
  #undef IsDefaultChar
  #undef BoolValue
}

void ExpandEnvironment(tParamFile *FileStruc)
{
#ifdef PL_Debug2
  dsyslog("%s: ExpandEnvironment text=%s", plugin_name, FileStruc->u);
#endif
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
        FREENULL(buffer);
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
#ifdef PL_Debug2
  dsyslog("%s: ExpandEnvironment return=%s", plugin_name, FileStruc->e);
#endif
}

// --- cPluginPlaylist ----------------------------------------------------------

cPluginPlaylist *PluginPlaylist;

cPluginPlaylist::cPluginPlaylist(void)
{
#ifdef PL_Debug1
  dsyslog("%s: cPluginPlaylist::cPluginPlaylist", plugin_name);
#endif
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  OSDLanguage = -1;
  PluginPlaylist = this;
}

cPluginPlaylist::~cPluginPlaylist()
{
#ifdef PL_Debug1
  dsyslog("%s: cPluginPlaylist::~cPluginPlaylist", plugin_name);
#endif
  // Clean up after yourself!
  FREENULL(FileNameCharsAllowed);
  FREENULL(AllCharsAllowed);
  FREENULL(LastSelectedRecord);
  PluginPlaylist = NULL;
}

void cPluginPlaylist::TestAndSetOSDLanguage(void)
{
#ifdef PL_Debug2
  dsyslog("%s: cPluginPlaylist::TestAndSetOSDLanguage OSDLanguage=%d", plugin_name, Setup.OSDLanguage);
#endif
  if (OSDLanguage != Setup.OSDLanguage)
  {
    OSDLanguage = Setup.OSDLanguage;

    StartOptions[Start_DisplayPL] = tr("Setup$Display PL");
    StartOptions[Start_DisplayLast] = tr("Setup$Display last PL");
    StartOptions[Start_PlayLast] = tr("Setup$Play last PL");
    StartOptions[Start_NewEmpty] = tr("Setup$new empty PL");
    NoYesDefault[NoYesDefault_no] = tr("no");
    NoYesDefault[NoYesDefault_yes] = tr("yes");
    NoYesDefault[NoYesDefault_defaultPlugin] = tr("OptionPL$[from Setup]");
    NoYesDefault[NoYesDefault_defaultPlaylist] = tr("OptionPL$[from Playlist]");
    NoYesDefault[NoYesDefault_defaultRecordDir] = tr("OptionPL$[from Folder]");
    PlayOptions[PlayOptions_firstpos] = tr("OptionPL$first pos");
    PlayOptions[PlayOptions_firstnew] = tr("OptionPL$first new");
    PlayOptions[PlayOptions_lastplay] = tr("OptionPL$last played");
    PlayOptions[PlayOptions_selectpos] = tr("OptionPL$seletced pos");
    PlayOptions[PlayOptions_question] = tr("OptionPL$question");
    PlayOptions[PlayOptions_defaultPlugin] = tr("OptionPL$[from Setup]");
    FREENULL(FileNameCharsAllowed);
    asprintf(&FileNameCharsAllowed, "%s/$(){}!%%@", tr(FileNameChars));
    if (!AllCharsAllowed)
    {
      AllCharsAllowed = MALLOC(char, 193);
      char *next = AllCharsAllowed;
      for (int i = 32; i <= 127; i++)
        *next++ = i;
      for (int i = 160; i <= 255; i++)
        *next++ = i;
      *next = 0;
    }
  }
}

bool cPluginPlaylist::ProcessArg(int argc, char *argv[])
{
  int c;
  static struct option long_options[] = {
    { "delete_begin",              no_argument,       NULL, 'b' },
    { "nodelete_begin",            no_argument,       NULL, 'B' },
    { "min_entrys",                required_argument, NULL, 'd' },
    { "delete_end",                no_argument,       NULL, 'e' },
    { "nodelete_end",              no_argument,       NULL, 'E' },
    { "holdtime_history",          required_argument, NULL, 'h' },
    { "visible_in_mainmenu",       no_argument,       NULL, 'm' },
    { "hide_in_mainmenu",          no_argument,       NULL, 'M' },
    { "mainmenu_name",             required_argument, NULL, 'n' },
    { "holdtime_respones",         required_argument, NULL, 'r' },
    { "sort_ascending",            no_argument,       NULL, 's' },
    { "sort_descending",           no_argument,       NULL, 'S' },
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
  #define SetvalueChar(T) T.c = true; strn0cpy(T.u, optarg, sizeof(T.u))

  while ((c = getopt_long(argc, argv, "bBd:eEh:mMn:r:sSvV", long_options, NULL)) != -1)
  {
    // var pos4
    switch (c)
    {
      case 1:   commandline_preference.h = true;
                break;
/*      case 'b': Setvalue(delete_b_e) |= 0x1;
                break;
      case 'B': Setvalue(delete_b_e) &= ~0x1;
                break;
      case 'd': if (isnumber(optarg))
                {
                  Setvalue(num_entrys) = atoi(optarg);
                  Setvalue(delete_b_e) = 0x3;
                  if (num_entrys.u > 0) break;
                }
                esyslog("%s: invalid parameter for -d option: %s", plugin_name, optarg);
                return false;
      case 'e': Setvalue(delete_b_e) |= 0x2;
                break;
      case 'E': Setvalue(delete_b_e) &= ~0x2;
                break;
      case 'h': if (isnumber(optarg))
                {
                  Setvalue(message_in_queue) = atoi(optarg);
                  if (message_in_queue.u >=5 && message_in_queue.u <= 999) break;
                }
                esyslog("%s: invalid parameter for -h option: %s range 5-999", plugin_name, optarg);
                return false;
      case 'm': Setvalue(mainmenu_visible) = true;
                break;
      case 'M': Setvalue(mainmenu_visible) = false;
                break;
      case 'n': SetvalueChar(mainmenu_name);
                Setvalue(mainmenu_visible) = true;
                break;
      case 'r': if (isnumber(optarg))
                {
                  Setvalue(response_in_queue) = atoi(optarg);
                  if (response_in_queue.u >= 2 && response_in_queue.u <= 199) break;
                }
                esyslog("%s: invalid parameter for -r option: %s range 2-199", plugin_name, optarg);
                return false;
      case 's': Setvalue(sortdirection) = true;
                break;
      case 'S': Setvalue(sortdirection) = false;
                break;*/
      case 'v': Setvalue(verbose) = true;
                break;
      case 'V': Setvalue(verbose) = false;
                break;
      default:  return false;
    }
  }

  #undef Setvalue
  #undef SetvalueChar

  if (optind < argc && argv[optind][0] == '@')
  {
    strn0cpy(playlistconfigfile.u, &argv[optind][1], sizeof(playlistconfigfile.u));
    optind++;
  }
  return optind >= argc;
}

const char *cPluginPlaylist::Version(void)
{
  return VERSION;
}

const char *cPluginPlaylist::Description(void)
{
  return tr(DESCRIPTION);
}

const char *cPluginPlaylist::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return "  -m           --visible_in_mainmenu          Show the plugin in the mainmenu\n"
         "  -M           --hide_in_mainmenu             Hide the plugin in the mainmenu\n"
         "  -n Name      --mainmenu_name=Name           Select Name for entry in the mainmenu (set also -m)\n"
         "  -b           --delete_begin                 Show the delete all messages line at begin of messagelist\n"
         "  -B           --nodelete_begin               Hide the delete all messages line at begin of messagelist\n"
         "  -e           --delete_end                   Show the delete all messages line at end of messagelist\n"
         "  -E           --nodelete_end                 Hide the delete all messages line at end of messagelist\n"
         "  -d xx        --min_entrys=xx                minimum entrys for display delete all messages line at begin and end (include -b and -e)\n"
         "  -h xx        --holdtime_history=xx          minimum time (min) for message in historyqueue (OSD-list) 5-999\n"
         "  -r xx        --holdtime_respones=xx         minumum time (min) for responses in queue (readable by SAQRESP) 2-199\n"
         "  -s           --sort_ascending               sortoption for messagelist (OSD)\n"
         "  -S           --sort_descending              sortoption for messagelist (OSD)\n"
         "  -v           --verbose                      Enable more logging\n"
         "  -V           --noverbose                    Disable more loggig\n"
         "               --nosetup_commandline          Hide the Preferr Command Line Parameter form setup-menu\n"
         "               --ns_commandline               same as --nosetup_commandline";
         // free: a c f g i j k l o p q t u v w x y z
}

bool cPluginPlaylist::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return ProcessArg(argc, argv);
}

#ifdef PL_Debug1
void TestI18n(char *Test)
{
  Setup.OSDLanguage = 0;
  const char *eng = tr(Test);
  Setup.OSDLanguage = 1;
  const char *deu = tr(Test);
  if (eng && deu && strcmp(eng, deu) == 0)
    dsyslog("%s: missing: %s", plugin_name, Test);
}
#endif

bool cPluginPlaylist::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n(Phrases);
  TestAndSetOSDLanguage();

#ifdef PL_Debug1
  dsyslog("%s: cPluginPlaylist::Start playlistconfigfile=%s", plugin_name, playlistconfigfile.u);
#endif

  char *p;
  char *q = strdup(ConfigDirectory(""));
  asprintf(&p, "%s%s%s", q, *(q + strlen(q) - 1) == '/' ? "" : "/", playlistconfigfile.u);
  if (!access(playlistconfigfile.u, F_OK) && !access(playlistconfigfile.u, R_OK) || !access(p, F_OK) && !access(p, R_OK))
  {
    char *s;
    #define MAXARGS 100
    int fargc = 1;
    char *fargv[MAXARGS];
    cReadLine ReadLine;
    bool done;
    FILE *f;

    if (!access(playlistconfigfile.u, F_OK))
    {
      f = fopen(playlistconfigfile.u, "r");
#ifdef PL_Debug2
      dsyslog("%s: cPluginPlaylist::Start open playlistconfigfile=%s", plugin_name, playlistconfigfile.u);
#endif
    } else
    {
      f = fopen(p, "r");
#ifdef PL_Debug2
      dsyslog("%s: cPluginPlaylist::Start open playlistconfigfile=%s", plugin_name, p);
#endif
    }
    free(p);
    free(q);
    if (!f)
    {
      esyslog("%s: ERROR: cannot open config file: [%s]%s", plugin_name, ConfigDirectory(""), playlistconfigfile.u);
      return false;
    }
    while ((s = ReadLine.Read(f)) != NULL)
    {
      p = skipspace(stripspace(s));
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
        esyslog("%s: ERROR: cannot parse config file: [%s]%s", plugin_name, ConfigDirectory(""), playlistconfigfile.u);
        return false;
      }
      while(fargc) free(fargv[--fargc]);
  } else
  {
    free(p);
    free(q);
    if (strcmp(playlistconfigfile.u, playlistconfigfile.d))
    {
      esyslog("%s: ERROR: config file not found: [%s]%s", plugin_name, ConfigDirectory(""), playlistconfigfile.u);
      return false;
    } else if (verbose.u)
      isyslog("%s: INFO: config file not found: [%s]%s", plugin_name, ConfigDirectory(""), playlistconfigfile.u);
  }

  // default parameter                --> d_
  // read parameter from commandline  --> c_  (data in u_)
  // value parameter from config-file --> s_
  // read parameter from config-file  --> r_
  // paramater used                   --> u_

  #define SetParam(T) { if (!T.c || (!commandline_preference.u && T.r)) T.u = (!T.c && !T.r) ? T.d : T.s; }
  #define SetParamFile(T) { if (!T.c || (!commandline_preference.u && T.r)) strcpy(T.u, (!T.c && !T.r) ? T.d : T.s); }

  commandline_preference.u = commandline_preference.r ? commandline_preference.s : commandline_preference.d;

  // var pos5
  SetParamFile (playlistconfigfile);
  SetParam     (charentry);
  SetParam     (charfolder);
  SetParam     (charrecord);
  SetParamFile (lastplaylist);
  SetParamFile (mainmenu_name);
  SetParam     (mainmenu_visible);
  SetParam     (confirmdelplentry);
  SetParam     (okstartplay);
  SetParam     (deleterecords);
  SetParamFile (pathplaylists);
  SetParam     (startoption);
  SetParam     (storeplaylist);
  SetParam     (timeoutreadrecords);
  SetParam     (verbose);
  for (int i = Option_confirmdeleterecord; i < Option_max; i++)
    SetParam   (PL_options[i]);

  #undef SetParam
  #undef SetParamFile

  if (verbose.u)
    isyslog("%s: Start", plugin_name);
  DisplaySetings();
  strcpy(lastplaylist.o, lastplaylist.u);
  return true;
}

const char *cPluginPlaylist::MainMenuEntry(void)
{
  return mainmenu_visible.u ? mainmenu_name.u : NULL;
}

cOsdObject *cPluginPlaylist::MainMenuAction(void)
{
#ifdef PL_Debug1
  dsyslog("%s: cPluginPlaylist::MainMenuAction", plugin_name);
#endif
  TestAndSetOSDLanguage();
  return new cControlMenu;
}

cMenuSetupPage *cPluginPlaylist::SetupMenu(void)
{
  TestAndSetOSDLanguage();
  return new cMenuSetupPlayList;
}


bool cPluginPlaylist::SetupParse(const char *Name, const char *Value)
{
  #define SetParam(T) { T.r = true; T.s = strtol(Value, NULL, 0); }
  #define SetParamChar(T) { T.r = true; temp = strtol(Value, NULL, 0); T.s = temp > 255 ? temp - 256 : temp; }
  #define SetParamFile(T) { T.r = true; strn0cpy(T.s, Value, sizeof(T.s)); }

  // Parse your own setup parameters and store their values.
  dsyslog("%s: Setupparameter %s=%s", plugin_name, Name, Value);
  // var pos6
  int temp;

  if      (!strcasecmp(Name, "cahre"))       SetParamChar (charentry)
  else if (!strcasecmp(Name, "charf"))       SetParamChar (charfolder)
  else if (!strcasecmp(Name, "charr"))       SetParamChar (charrecord)
  else if (!strcasecmp(Name, "commandline")) SetParam     (commandline_preference)
  else if (!strcasecmp(Name, "lastpl"))      SetParamFile (lastplaylist)
  else if (!strcasecmp(Name, "name"))        SetParamFile (mainmenu_name)
  else if (!strcasecmp(Name, "visible"))     SetParam     (mainmenu_visible)
  else if (!strcasecmp(Name, "deletentry"))  SetParam     (confirmdelplentry)
  else if (!strcasecmp(Name, "okplay"))      SetParam     (okstartplay)
  else if (!strcasecmp(Name, "delete"))      SetParam     (deleterecords)
  else if (!strcasecmp(Name, "path"))        SetParamFile (pathplaylists)
  else if (!strcasecmp(Name, "start"))       SetParam     (startoption)
  else if (!strcasecmp(Name, "storepl"))     SetParam     (storeplaylist)
  else if (!strcasecmp(Name, "timerecords")) SetParam     (timeoutreadrecords)
  else if (!strcasecmp(Name, "verbose"))     SetParam     (verbose)
  else if (!strcasecmp(Name, "confdelete"))  SetParam     (PL_options[Option_confirmdeleterecord])
  else if (!strcasecmp(Name, "startnew"))    SetParam     (PL_options[Option_confirmstartnewrecord])
  else if (!strcasecmp(Name, "deletel"))     SetParam     (PL_options[Option_deletefromlist])
  else if (!strcasecmp(Name, "deleter"))     SetParam     (PL_options[Option_deleterecord])
  else if (!strcasecmp(Name, "jumpmark"))    SetParam     (PL_options[Option_jumpmark])
  else if (!strcasecmp(Name, "playoption1")) SetParam     (PL_options[Option_playoption1])
  else if (!strcasecmp(Name, "playoption2")) SetParam     (PL_options[Option_playoption2])
  else if (!strcasecmp(Name, "playonlynew")) SetParam     (PL_options[Option_playonlynew])
  else if (!strcasecmp(Name, "searchnew"))   SetParam     (PL_options[Option_searchnewrecord])
  else if (!strcasecmp(Name, "searchsub"))   SetParam     (PL_options[Option_searchrecordsub])
  else
    return false;
  return true;

  #undef SetParam
  #undef SetParamChar
  #undef SetParamFile
}

const char *cPluginPlaylist::ExpandPath(const char *Filename, bool CreateDir)
{
  static char path[MaxFileName];
  char *c = strdup(ConfigDirectory(""));
  char *p = NULL;

  ExpandEnvironment(&pathplaylists);
  if (pathplaylists.e[0] == '/')
    asprintf(&p, "%s%s%s", pathplaylists.e, *(pathplaylists.e + strlen(pathplaylists.e) - 1) == '/' ? "" : "/", Filename);
  else
    asprintf(&p, "%s%s%s%s%s", c, *(c + strlen(c) - 1) == '/' ? "" : "/", pathplaylists.e, strlen(pathplaylists.e) ? *(pathplaylists.e + strlen(pathplaylists.e) - 1) == '/' ? "" : "/" : "", Filename);
  if (strlen(p) >= MaxFileName)
    esyslog("%s: max length for filename is %d this name is %d [%s]", plugin_name, MaxFileName, strlen(p), p);
  strn0cpy(path, p, MaxFileName);
  if (CreateDir)
    MakeDirs(path, false);
  free(c);
  free(p);
#ifdef PL_Debug1
  dsyslog("%s: ExpandPath Filename=%s Return=%s", plugin_name, Filename, path);
#endif
  return &path[0];
}

VDRPLUGINCREATOR(cPluginPlaylist); // Don't touch this!
