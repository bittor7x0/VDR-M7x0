/*
 * ircviewer.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <vdr/device.h>
#include <vdr/thread.h>
#include <vdr/osd.h>
#include <vdr/tools.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "i18n.h"

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "IRC Viewer client";
static const char *MAINMENUENTRY  = "IRC Viewer";

#define SMALL_FONT 0
#define CONNECT_TO_START 0
#define IRC_DEFAULT_CHANNEL 1
#define IRC_DEFAULT_SERVER_NAME "irc.server.name"
#define IRC_DEFAULT_SERVER_PORT 6667
#define IRC_DEFAULT_NICKNAME "nickname"
#define IRC_DEFAULT_CHANNEL_NAME "ircchannelname"
#define IRC_CONNECTIONS 3
#define IRC_CLIENT_NAME_SIZE 256
#define IRC_SERVER_NAME_SIZE 256
#define IRC_NICKNAME_SIZE 32
#define IRC_CHANNEL_NAME_SIZE 256

#define COMMAND_SIZE 512
#define USERNAME_SIZE 32
#define MESSAGE_SIZE 512
#define BUFFER_SIZE 4096
#define COMMAND_NICK "NICK %s\r\n"
#define COMMAND_USER "USER %s %s %s %s\r\n"
#define COMMAND_JOIN "JOIN %s\r\n"
#define COMMAND_PONG "PONG %s\r\n"

#define MESSAGES 32

#define OSD_ROWS 5
#define OSD_WIDTH 600
#define OSD_MARGIN_LEFT 60
#define OSD_MARGIN_BOTTOM 40
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 576

#define ERROR_MESSAGE_SIZE 42

#define DEBUG 0

const char *Characters = "abcdefghijklmnopqrstuvwxyz0123456789~._-|";

struct IrcConfig
{
  bool Connected;
  bool Active;
  bool Failed;
  int Timeout;
  bool OsdChanged;
  int SmallFont;
  int ConnectToStart;
  int DefaultConnection;
  int OsdRows;
  int OsdWidth;
  int OsdMarginLeft;
  int OsdMarginBottom;
  char IrcServerName[IRC_CONNECTIONS][IRC_SERVER_NAME_SIZE];
  int IrcServerPort[IRC_CONNECTIONS];
  char IrcNickname[IRC_CONNECTIONS][IRC_NICKNAME_SIZE];
  char IrcChannelName[IRC_CONNECTIONS][IRC_CHANNEL_NAME_SIZE];
  char ErrorMessage[ERROR_MESSAGE_SIZE];
};

static struct IrcConfig IrcConfig;

struct OsdRow
{
  char Username[USERNAME_SIZE];
  char Message[MESSAGE_SIZE];
};

static struct OsdRow OsdBuffer[MESSAGES];

class cIrcNet : public cThread, cOsdObject
{
  private:
    char IrcClientName[IRC_CLIENT_NAME_SIZE];
    char IrcServerName[IRC_SERVER_NAME_SIZE];
    int IrcServerPort;
    char IrcNickname[IRC_NICKNAME_SIZE];
    char IrcChannelName[IRC_CHANNEL_NAME_SIZE];
    int Sock;
    struct hostent *HostEnt;
    struct sockaddr_in SockAddrIn;
    char Command[COMMAND_SIZE];
    char Buffer[BUFFER_SIZE];
    char Username[USERNAME_SIZE];
    char Message[MESSAGE_SIZE];
    const cFont *Font;
    int OsdRowWidth;
    char Word[MESSAGE_SIZE];
    int WordWidth;
    int MessageLength;
  protected:
    virtual void Action( void );
  public:
    cIrcNet( void );
    ~cIrcNet();
    void Disconnect( void );
    void ParseBuffer( const char *Buffer );
    void AddMessage( const char *Username, const char *Message );
    void ScrollOsdBuffer( void );
    void ClearOsdBuffer( void );
};

class cIrcOsd : public cOsdObject
{
  private:
    cOsd *Osd;
    const cFont *Font;
    int LineHeight;
    int OsdMarginTop;
    int OsdMarginLeft;
    int OsdWidth;
    int OsdHeight;
    int X1;
    int Y1;
    int X2;
    int Y2;
    char Username[USERNAME_SIZE];
    char Message[MESSAGE_SIZE];
  protected:
  public:
    cIrcOsd( void );
    ~cIrcOsd();
    virtual void Show( void );
    virtual eOSState ProcessKey( eKeys Key );
    void DrawOsd( void );
    void ClearOsd( void );
    cIrcNet *IrcNet;
};

class cIrcSetup : public cMenuSetupPage
{
  private:
    int IrcChannelId;
    char Field[256];
  protected:
    virtual void Store( void );
  public:
    cIrcSetup( void );
};

class cPluginIrcViewer : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  cIrcOsd *IrcOsd;
  int IrcChannelId;
  char Field[256];
  cIrcNet *IrcNet;
public:
  cPluginIrcViewer(void);
  virtual ~cPluginIrcViewer();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr( DESCRIPTION ); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return tr( MAINMENUENTRY ); }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };
