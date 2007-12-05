#include "ircviewer.h"

//-- cIrcNet -----------------------------------------------------------------

cIrcNet::cIrcNet( void )
:cThread( "ircviewer" )
{
  if( IrcConfig.ConnectToStart == true )
  {
    IrcConfig.Active = true;
  }
  Start();
}

cIrcNet::~cIrcNet()
{
  Cancel( 1 );
  IrcConfig.Failed = false;
}

void cIrcNet::Disconnect( void )
{
  if( IrcConfig.Active == true && IrcConfig.Connected == true )
  {
    send( Sock, "QUIT\r\n", 6, 0 );
    close( Sock );
    IrcConfig.Active = false;
    IrcConfig.Connected = false;
    ClearOsdBuffer();
  }
}

void cIrcNet::Action()
{
  while( 1 )
  {
    if( IrcConfig.Active == true && IrcConfig.Failed == false )
    {
      Font = NULL;
      if( IrcConfig.SmallFont == true )
      {
        Font = cFont::GetFont( fontSml );
      }
      else
      {
        Font = cFont::GetFont( fontOsd );
      }
      bzero( IrcClientName, sizeof( IrcClientName ) );
      bzero( IrcServerName, sizeof( IrcServerName ) );
      IrcServerPort = IRC_DEFAULT_SERVER_PORT;
      bzero( IrcNickname, sizeof( IrcNickname ) );
      bzero( IrcChannelName, sizeof( IrcChannelName ) );
      gethostname( IrcClientName, sizeof( IrcClientName ) );
      snprintf( IrcServerName, sizeof( IrcServerName ), "%s", IrcConfig.IrcServerName[IrcConfig.DefaultConnection-1] );
      IrcServerPort = IrcConfig.IrcServerPort[IrcConfig.DefaultConnection-1];
      snprintf( IrcNickname, sizeof( IrcNickname ), "%s", IrcConfig.IrcNickname[IrcConfig.DefaultConnection-1] );
      snprintf( IrcChannelName, sizeof( IrcChannelName ), "#%s", IrcConfig.IrcChannelName[IrcConfig.DefaultConnection-1] );
      if( IrcConfig.Connected == false )
      {
        fprintf( stderr, "net: connect to host \"%s\"\n", IrcServerName );
	HostEnt = gethostbyname( IrcServerName );
	if( HostEnt )
	{
	  bzero( ( struct sockaddr_in * ) &SockAddrIn, sizeof( SockAddrIn ) );
	  SockAddrIn.sin_family = AF_INET;
	  SockAddrIn.sin_port = htons( IrcServerPort );
	  SockAddrIn.sin_addr.s_addr = ( ( struct in_addr * ) ( HostEnt->h_addr ) )->s_addr;
	  if( ( Sock = socket( AF_INET, SOCK_STREAM, 0 ) ) >= 0 )
	  {
	    if( ( connect( Sock, ( struct sockaddr * ) &SockAddrIn, sizeof( SockAddrIn ) ) ) >= 0 )
	    {
	      bzero( Command, sizeof( Command ) );
	      snprintf( Command, sizeof( Command ), COMMAND_NICK, IrcNickname );
	      send( Sock, Command, strlen( Command ), 0 );
	      sleep( 1 );
	      bzero( Command, sizeof( Command ) );
	      snprintf( Command, sizeof( Command ), COMMAND_USER, IrcNickname, IrcClientName, IrcServerName, IrcNickname );
	      send( Sock, Command, strlen( Command ), 0 );
	      sleep( 1 );
	      bzero( Command, sizeof( Command ) );
	      snprintf( Command, sizeof( Command ), COMMAND_JOIN, IrcChannelName );
	      send( Sock, Command, strlen( Command ), 0 );
	      while( 1 )
	      {
	        bzero( Buffer, sizeof( Buffer ) );
		if( ( recv( Sock, Buffer, sizeof( Buffer ), 0 ) ) == 0 )
		{
		  close( Sock );
		  IrcConfig.Connected = false;
		  IrcConfig.OsdChanged = true;
		  fprintf( stderr, "net: disconnect\n" );
		  break;
		}
		if( DEBUG == 2 )
		{
		  fprintf( stderr, "%s\n", Buffer );
		}
		ParseBuffer( Buffer );
	      }
	      // Disconnect
	    }
	    else
	    {
	      close( Sock );
	      IrcConfig.Failed = true;
	      bzero( IrcConfig.ErrorMessage, sizeof( IrcConfig.ErrorMessage ) );
	      snprintf( IrcConfig.ErrorMessage, sizeof( IrcConfig.ErrorMessage ), "connecting to host %s failed", IrcServerName );
	      IrcConfig.OsdChanged = true;
	      fprintf( stderr, "net: connecting to host \"%s\" failed\n", IrcServerName );
	    }
	  }
	  else
	  {
	    IrcConfig.Failed = true;
	    bzero( IrcConfig.ErrorMessage, sizeof( IrcConfig.ErrorMessage ) );
	    snprintf( IrcConfig.ErrorMessage, sizeof( IrcConfig.ErrorMessage ), "creating socket failed" );
	    IrcConfig.OsdChanged = true;
	    fprintf( stderr, "net: creating socket failed\n" );
	  }
	}
	else
	{
	  IrcConfig.Failed = true;
          bzero( IrcConfig.ErrorMessage, sizeof( IrcConfig.ErrorMessage ) );
	  snprintf( IrcConfig.ErrorMessage, sizeof( IrcConfig.ErrorMessage ), "unknown host  %s", IrcServerName );
	  IrcConfig.OsdChanged = true;
	  fprintf( stderr, "net: unknown host \"%s\"\n", IrcServerName );
	}
      }
      else
      {
        fprintf( stderr, "net: ircviewer is already connected\n" );
	Cancel( 1 );
      }
    }
    sleep( 1 );
    if( DEBUG == 2 )
    {
      fprintf( stderr, "net: sleep\n" );
    }
  }
}

void cIrcNet::ParseBuffer( const char *Buffer )
{
  int c, b, m, u;
  c = 0;
  bzero( Command, sizeof( Command ) );
  if( IrcConfig.Connected == false )
  {
    if( strstr( Buffer, "MOTD" ) != NULL )
    {
      IrcConfig.Connected = true;
      IrcConfig.OsdChanged = true;
      return;
    }
    else
    {
      return;
    }
  }
  for( b = 0; b < strlen( Buffer ); b ++ )
  {
    if( Buffer[b] == '\r' || Buffer[b] == '\n' || Buffer[b] == '\0' )
    {
      Command[c] = '\0';
      if( strlen( Command ) > 0 )
      {
        // PING - PONG
	if( ! strncmp( Command, "PING", 4 ) )
	{
	  for( c = 0; c < strlen( Command ); c ++ )
	  {
	    if( Command[c] == ':' )
	    {
	      snprintf( Command, sizeof( Command ), COMMAND_PONG, &Command[c+1] );
	      if( DEBUG == 1 )
	      {
	        fprintf( stderr, "%s\n", Command );
	      }
	      break;
	    }
	  }
	  send( Sock, Command, strlen( Command ), 0 );
	}
	
	// PRIVMSG
	u = 0;
	bzero( Username, sizeof( Username ) );
	m = 0;
	bzero( Message, sizeof( Message ) );
	if( strstr( Command, "PRIVMSG" ) != NULL )
	{
	  if( Command[0] == ':' )
	  {
	    // PRIVMSG: Username
	    for( c = 1; c < ( USERNAME_SIZE - 1 ); c ++ )
	    {
	      if( Command[c] == '!' )
	      {
	        c ++;
		break;
	      }
	      Username[u] = Command[c];
	      u ++;
	    }
	    
	    // PRIVMSG: Message
	    for( c = c; c < ( MESSAGE_SIZE - 1 ); c ++ )
	    {
	      if( Command[c] == 'P' )
	      {
	        if( memcmp( &Command[c], "PRIVMSG", 7 ) == 0 )
		{
		  c ++;
		  break;
		}
	      }
	    }
	    for( c = c; c < ( MESSAGE_SIZE - 1 ); c ++ )
	    {
	      if( Command[c] == ':' )
	      {
		c ++;
		break;
	      }
	    }
	    for( c = c; c < ( MESSAGE_SIZE - 1 ); c ++ )
	    {
	      if( ( unsigned char ) Command[c] > 0x7e && ( unsigned char ) Command[c] < 0xa1 )
	      {
	        Command[c] = ' ';
	      }
	      if( ( unsigned char ) Command[c] < 0x20 )
	      {
	        Message[m] = '\0';
		break;
	      }
	      if( ! ( Command[c] == ' ' && m == 0 ) )
	      {
	        Message[m] = Command[c];
		m ++;
	      }
	    }
	    c = 0;
	    if( strlen( Username ) > 0 && strlen( Message ) > 0 )
	    {
	      AddMessage( Username, Message );
	    }
	    if( DEBUG == 1 )
	    {
	      fprintf( stderr, "%s: %s\n", Username, Message );
	    }
	  }
	}
	
	// OTHER
      }
      if( Buffer[b] == '\0' )
      {
        break;
      }
      else
      {
        c = 0;
	bzero( Command, sizeof( Command ) );
      }
    }
    else
    {
      Command[c] = Buffer[b];
      c ++;
    }
  }
}

void cIrcNet::AddMessage( const char *Username, const char *Message )
{
  int m, w;
  ScrollOsdBuffer();
  OsdRowWidth = Font->Width( Username );
  memcpy( ( OsdBuffer + MESSAGES - 1 )->Username, Username, strlen( Username ) );
  w = 0;
  bzero( Word, sizeof( Word ) );
  MessageLength = 0;
  for( m = 0; m <= strlen( Message ); m ++ )
  {
    Word[w] = Message[m];
    w ++;
    if( Message[m] == ' ' || Message[m] == '\0' )
    {
      WordWidth = Font->Width( Word );
      if( ( OsdRowWidth + WordWidth + ( 2 * Font->Width( ' ' ) ) ) > IrcConfig.OsdWidth )
      {
        ScrollOsdBuffer();
	OsdRowWidth = WordWidth;
	MessageLength = 0;
      }
      else
      {
        OsdRowWidth = OsdRowWidth + WordWidth;
      }
      memcpy( &( OsdBuffer + MESSAGES - 1 )->Message[MessageLength], Word, strlen( Word ) );
      MessageLength = MessageLength + w;
      bzero( Word, sizeof( Word ) );
      w = 0;
    }
  }
  IrcConfig.OsdChanged = true;
}

void cIrcNet::ScrollOsdBuffer( void )
{
  int r;
  for( r = 0; r < ( MESSAGES - 1 ); r ++ )
  {
    memcpy( ( OsdBuffer + r )->Username, ( OsdBuffer + r + 1 )->Username, USERNAME_SIZE );
    memcpy( ( OsdBuffer + r )->Message, ( OsdBuffer + r + 1 )->Message, MESSAGE_SIZE );
  }
  bzero( ( OsdBuffer + MESSAGES - 1 )->Username, sizeof( ( OsdBuffer + MESSAGES - 1 )->Username ) );
  bzero( ( OsdBuffer + MESSAGES - 1 )->Message, sizeof( ( OsdBuffer + MESSAGES - 1 )->Message ) );
}

void cIrcNet::ClearOsdBuffer( void )
{
  bzero( OsdBuffer, sizeof( OsdBuffer ) );
}

//----------------------------------------------------------------------------

//-- cIrcOsd -----------------------------------------------------------------

cIrcOsd::cIrcOsd( void )
{
  Osd = NULL;
  IrcConfig.OsdChanged = true;
}

cIrcOsd::~cIrcOsd( void )
{
  delete Osd;
}

void cIrcOsd::Show( void )
{
  if( IrcConfig.SmallFont == true )
  {
    Font = cFont::GetFont( fontSml );
  }
  else
  {
    Font = cFont::GetFont( fontOsd );
  }
  LineHeight = Font->Height();
  OsdMarginLeft = ( ( SCREEN_WIDTH - IrcConfig.OsdWidth ) / 2 ) - OSD_MARGIN_LEFT + IrcConfig.OsdMarginLeft;
  OsdMarginTop = SCREEN_HEIGHT - IrcConfig.OsdMarginBottom - 450;
  OsdWidth = IrcConfig.OsdWidth - 1;
  OsdHeight = 449;
  Osd = cOsdProvider::NewOsd( OsdMarginLeft, OsdMarginTop );
  if( Osd )
  {
    tArea Area = { 0, 0, OsdWidth, OsdHeight, 2 };
    Osd->SetAreas( &Area, 1 );
    ClearOsd();
    DrawOsd();
  }
  else
  {
    fprintf( stderr, "osd too large\n" );
  }
}

eOSState cIrcOsd::ProcessKey( eKeys Key )
{
  eOSState State = cOsdObject::ProcessKey( Key );
  if( State == osUnknown )
  {
    switch( Key )
    {
      case kOk:
        if( IrcConfig.Failed == false )
	{
          if( IrcConfig.Active == true && IrcConfig.Connected == true )
	  {
	    IrcConfig.Active = false;
	    IrcConfig.OsdChanged = true;
	  }
	  else if( IrcConfig.Active == false && IrcConfig.Connected == true )
	  {
	    IrcConfig.Active = true;
	    IrcNet->Disconnect();
	    return osEnd;
	  }
          else if( IrcConfig.Active == false && IrcConfig.Connected == false )
	  {
	    IrcConfig.Active = true;
	    IrcConfig.OsdChanged = true;
	  }
	}
	else
	{
	  IrcConfig.Active = false;
	  IrcConfig.Connected = false;
	  IrcConfig.Failed = false;
	  return osEnd;
	}
        break;
      case kDown:
        if( IrcConfig.Failed == false )
	{
          if( IrcConfig.Active == false && IrcConfig.Connected == false )
	  {
	    IrcConfig.DefaultConnection ++;
	    if( IrcConfig.DefaultConnection > IRC_CONNECTIONS )
	    {
	      IrcConfig.DefaultConnection = IRC_CONNECTIONS;
	    }
	    IrcConfig.OsdChanged = true;
	  }
	}
        break;
      case kUp:
        if( IrcConfig.Failed == false )
	{
          if( IrcConfig.Active == false && IrcConfig.Connected == false )
	  {
	    IrcConfig.DefaultConnection --;
	    if( IrcConfig.DefaultConnection < 1 )
	    {
	      IrcConfig.DefaultConnection = 1;
	    }
	    IrcConfig.OsdChanged = true;
	  }
	}
        break;
      case kBack:
        if( IrcConfig.Failed == false )
	{
          if( IrcConfig.Active == true && IrcConfig.Connected == true )
	  {
	    return osEnd;
	  }
	  else if( IrcConfig.Active == true && IrcConfig.Connected == false )
	  {
	    return osEnd;
	  }
	  else if( IrcConfig.Active == false && IrcConfig.Connected == true )
	  {
	    IrcConfig.Active = true;
	    IrcConfig.OsdChanged = true;
	  }
	  else if( IrcConfig.Active == false && IrcConfig.Connected == false )
	  {
            return osEnd;
	  }
	}
	else
	{
	  IrcConfig.Active = false;
	  IrcConfig.Connected = false;
	  IrcConfig.Failed = false;
	  return osEnd;
	}
	break;
      default:
        break;
    }
    State = osContinue;
    DrawOsd();
  }
  return State;
}

void cIrcOsd::DrawOsd( void )
{
  int r;
  if( IrcConfig.OsdChanged == true )
  {
    if( IrcConfig.Failed == false )
    {
      if( IrcConfig.Active == true && IrcConfig.Connected == true )
      {
        if( DEBUG == 1 )
        {
          fprintf( stderr, "draw osd Active= true Connected=true\n" );
        }
        ClearOsd();
        X1 = 0;
        Y1 = OsdHeight - ( IrcConfig.OsdRows * LineHeight );
        X2 = X1 + OsdWidth;
        Y2 = OsdHeight;
        Osd->DrawRectangle( X1, Y1, X2, Y2, clrGray50 );
        for( r = 1; r <= IrcConfig.OsdRows; r ++ )
        {
          Y1 = OsdHeight - ( r * LineHeight );
	  bzero( Username, sizeof( Username ) );
	  bzero( Message, sizeof( Message ) );
	  if( strlen( ( OsdBuffer + MESSAGES - r )->Username ) > 0 )
	  {
	    snprintf( Username, sizeof( Username ), " %s:", ( OsdBuffer + MESSAGES - r )->Username );
	  }
	  if( strlen( ( OsdBuffer + MESSAGES - r )->Message ) > 0 )
	  {
	    snprintf( Message, sizeof( Message ), " %s", ( OsdBuffer + MESSAGES - r )->Message );
	  }
          Osd->DrawText( 0, Y1, Username, clrCyan, clrGray50, Font );
	  Osd->DrawText( Font->Width( Username ), Y1, Message, clrWhite, clrGray50, Font );
        }
        Osd->Flush();
      }
      else if( IrcConfig.Active == false && IrcConfig.Connected == true )
      {
        if( DEBUG == 1 )
        {
          fprintf( stderr, "draw osd Active=false Connected = true\n" );
        }
        ClearOsd();
        X1 = ( OsdWidth - 500 ) / 2;
        Y1 = 450 - LineHeight - 20;
        X2 = X1 + 500;
        Y2 = Y1 + LineHeight + 14;
        Osd->DrawRectangle( X1, Y1, X2, Y2, clrGray50 );
        Osd->DrawText( X1 + ( ( 500 - Font->Width( tr( "Disconnect ?" ) ) ) / 2 ), Y1 + 9, tr( "Disconnect ?" ), clrWhite, clrGray50, Font );
        Osd->Flush();
      }
      else if( IrcConfig.Active == true && IrcConfig.Connected == false )
      {
        if( DEBUG == 1 )
        {
          fprintf( stderr, "draw osd Active=true Connected = false\n" );
        }
        ClearOsd();
        X1 = ( OsdWidth - 500 ) / 2;
        Y1 = 450 - LineHeight - 20;
        X2 = X1 + 500;
        Y2 = Y1 + LineHeight + 14;
        Osd->DrawRectangle( X1, Y1, X2, Y2, clrGray50 );
        Osd->DrawText( X1 + ( ( 500 - Font->Width( tr( "Wait connecting" ) ) ) / 2 ), Y1 + 9, tr( "Wait connecting" ), clrWhite, clrGray50, Font );
        Osd->Flush();
      }
      else if( IrcConfig.Active == false && IrcConfig.Connected == false )
      {
        if( DEBUG == 1 )
        {
          fprintf( stderr, "draw osd Active= false Connected=false\n" );
        }
        ClearOsd();
        X1 = ( OsdWidth - 500 ) / 2;
        Y1 = 0;
        X2 = X1 + 500;
        Y2 = Y1 + ( ( IRC_CONNECTIONS + 1 ) * ( LineHeight + 4 ) ) + 6;
        Osd->DrawRectangle( X1, Y1, X2, Y2, clrGray50 );
        Osd->DrawText( X1 + 2 , Y1 + 2, tr( "Select a connection" ), clrWhite, clrGray50, Font );
        Osd->DrawRectangle( X1, Y1 + LineHeight + 4, X2, Y1 + LineHeight + 5, clrWhite );
        Y1 = Y1 + LineHeight + 10;
        Y2 = Y1 + LineHeight + 4;
        for( r = 1; r <= IRC_CONNECTIONS; r ++ )
        {
	  if( IrcConfig.DefaultConnection == r )
	  {
	    Osd->DrawRectangle( X1 + 2, Y1 + 1, X2 - 2, Y2 - 2, clrCyan );
	    Osd->DrawText( X1 + 4, Y1 + 1, IrcConfig.IrcServerName[r-1], clrGray50, clrCyan, Font );
	    Osd->DrawText( X1 + 254, Y1 + 1, IrcConfig.IrcChannelName[r-1], clrGray50, clrCyan, Font );
	  }
	  else
	  {
            Osd->DrawText( X1 + 4, Y1 + 1, IrcConfig.IrcServerName[r-1], clrWhite, clrGray50, Font );
            Osd->DrawText( X1 + 254, Y1 + 1, IrcConfig.IrcChannelName[r-1], clrWhite, clrGray50, Font );
	  }
	  Y1 = Y1 + LineHeight + 4;
	  Y2 = Y1 + LineHeight + 4;
        }
        Osd->Flush();
      }
    }
    else
    {
      if( DEBUG == 1 )
      {
        fprintf( stderr, "error message\n" );
      }
      ClearOsd();
      X1 = ( OsdWidth - 500 ) / 2;
      Y1 = 450 - LineHeight - 20;
      X2 = X1 + 500;
      Y2 = Y1 + LineHeight + 14;
      Osd->DrawRectangle( X1, Y1, X2, Y2, clrGray50 );
      Osd->DrawText( X1 + ( ( 500 - Font->Width( tr( IrcConfig.ErrorMessage ) ) ) / 2 ), Y1 + 9, IrcConfig.ErrorMessage, clrWhite, clrGray50, Font );
      Osd->Flush();
    }
  }    
  IrcConfig.OsdChanged = false;
}

void cIrcOsd::ClearOsd( void )
{
  Osd->DrawRectangle( 0, 0, OsdWidth, OsdHeight, clrTransparent );
}

//----------------------------------------------------------------------------

//-- cIrcSetup ---------------------------------------------------------------

cIrcSetup::cIrcSetup( void )
{
  Clear();
  Add( new cMenuEditBoolItem( tr( "Use small font" ), &IrcConfig.SmallFont, tr( "No" ), tr( "Yes" ) ) );
  Add( new cMenuEditBoolItem( tr( "Connect to the start" ), &IrcConfig.ConnectToStart, tr( "No" ), tr( "Yes" ) ) );
  Add( new cMenuEditIntItem( tr( "Default connection" ), &IrcConfig.DefaultConnection, 1, IRC_CONNECTIONS ) );
  Add( new cMenuEditIntItem( tr( "Number rows" ), &IrcConfig.OsdRows, 1, 10 ) );
  Add( new cMenuEditIntItem( tr( "Osd width" ), &IrcConfig.OsdWidth, 500, 600 ) );
  Add( new cMenuEditIntItem( tr( "Margin left" ), &IrcConfig.OsdMarginLeft, 0, 80 ) );
  Add( new cMenuEditIntItem( tr( "Margin bottom" ), &IrcConfig.OsdMarginBottom, 0, 80 ) );
  for( IrcChannelId = 0; IrcChannelId < IRC_CONNECTIONS; IrcChannelId ++ )
  {
    snprintf( Field, sizeof( Field ), "%s(%i)", tr( "Server name" ), IrcChannelId + 1 );
    Add( new cMenuEditStrItem( Field, IrcConfig.IrcServerName[IrcChannelId], sizeof( IrcConfig.IrcServerName[IrcChannelId] ), tr( Characters ) ) );
    snprintf( Field, sizeof( Field ), "%s(%i)", tr( "Server port" ), IrcChannelId + 1 );
    Add( new cMenuEditIntItem( Field, &IrcConfig.IrcServerPort[IrcChannelId], 1024, 65536 ) );
    snprintf( Field, sizeof( Field ), "%s(%i)", tr( "Nickname" ), IrcChannelId + 1 );
    Add( new cMenuEditStrItem( Field, IrcConfig.IrcNickname[IrcChannelId], sizeof( IrcConfig.IrcNickname[IrcChannelId] ), tr( Characters ) ) );
    snprintf( Field, sizeof( Field ), "%s(%i)", tr( "Channel name" ), IrcChannelId + 1 );
    Add( new cMenuEditStrItem( Field, IrcConfig.IrcChannelName[IrcChannelId], sizeof( IrcConfig.IrcChannelName[IrcChannelId] ), tr( Characters ) ) );
  }
  Display();
}

void cIrcSetup::Store( void )
{
  SetupStore( "SmallFont", IrcConfig.SmallFont );
  SetupStore( "ConnectToStart", IrcConfig.ConnectToStart );
  SetupStore( "DefaultConnection", IrcConfig.DefaultConnection );
  SetupStore( "OsdRows", IrcConfig.OsdRows );
  SetupStore( "OsdWidth", IrcConfig.OsdWidth );
  SetupStore( "OsdMarginLeft", IrcConfig.OsdMarginLeft );
  SetupStore( "OsdMarginBottom", IrcConfig.OsdMarginBottom );
  for( IrcChannelId = 0; IrcChannelId < IRC_CONNECTIONS; IrcChannelId ++ )
  {
    snprintf( Field, sizeof( Field ), "IrcServerName%i", IrcChannelId + 1 );
    SetupStore( Field, IrcConfig.IrcServerName[IrcChannelId] );
    snprintf( Field, sizeof( Field ), "IrcServerPort%i", IrcChannelId + 1 );
    SetupStore( Field, IrcConfig.IrcServerPort[IrcChannelId] );
    snprintf( Field, sizeof( Field ), "IrcNickname%i", IrcChannelId + 1 );
    SetupStore( Field, IrcConfig.IrcNickname[IrcChannelId] );
    snprintf( Field, sizeof( Field ), "IrcChannelName%i", IrcChannelId + 1 );
    SetupStore( Field, IrcConfig.IrcChannelName[IrcChannelId] );
  }
}

//----------------------------------------------------------------------------


//-- cPluginIrcViewer --------------------------------------------------------

cPluginIrcViewer::cPluginIrcViewer(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  bzero( &IrcConfig, sizeof( IrcConfig ) );
  IrcConfig.Connected = false;
  IrcConfig.Active = false;
  IrcConfig.Failed = false;
  IrcConfig.OsdChanged = true;
  IrcConfig.Timeout = 5;
  IrcConfig.SmallFont = SMALL_FONT;
  IrcConfig.ConnectToStart = CONNECT_TO_START;
  IrcConfig.DefaultConnection = IRC_DEFAULT_CHANNEL;
  IrcConfig.OsdRows = OSD_ROWS;
  IrcConfig.OsdWidth = OSD_WIDTH;
  IrcConfig.OsdMarginLeft = OSD_MARGIN_LEFT;
  IrcConfig.OsdMarginBottom = OSD_MARGIN_BOTTOM;
  for( IrcChannelId = 0; IrcChannelId < IRC_CONNECTIONS; IrcChannelId ++ )
  {
    snprintf( IrcConfig.IrcServerName[IrcChannelId], sizeof( IrcConfig.IrcServerName[IrcChannelId] ), "%s", IRC_DEFAULT_SERVER_NAME );
    IrcConfig.IrcServerPort[IrcChannelId] = IRC_DEFAULT_SERVER_PORT;
    snprintf( IrcConfig.IrcNickname[IrcChannelId], sizeof( IrcConfig.IrcNickname[IrcChannelId] ), "%s", IRC_DEFAULT_NICKNAME );
    snprintf( IrcConfig.IrcChannelName[IrcChannelId], sizeof( IrcConfig.IrcChannelName[IrcChannelId] ), "%s", IRC_DEFAULT_CHANNEL_NAME );
  }
}

cPluginIrcViewer::~cPluginIrcViewer()
{
  // Clean up after yourself!
}

const char *cPluginIrcViewer::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginIrcViewer::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginIrcViewer::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginIrcViewer::Start(void)
{
  // Start any background activities the plugin shall perform.
  RegisterI18n( IrcViewerPhrases );
  IrcNet = new cIrcNet();
  return true;
}

void cPluginIrcViewer::Stop(void)
{
  // Stop any background activities the plugin shall perform.
}

void cPluginIrcViewer::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginIrcViewer::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  IrcOsd = new cIrcOsd();
  IrcOsd->IrcNet = IrcNet;
  return IrcOsd;
}

cMenuSetupPage *cPluginIrcViewer::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cIrcSetup();
}

bool cPluginIrcViewer::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if( ! strcasecmp( Name, "SmallFont" ) )
  {
    IrcConfig.SmallFont = atoi( Value );
    return true;
  }
  else if( ! strcasecmp( Name, "ConnectToStart" ) )
  {
    IrcConfig.ConnectToStart = atoi( Value );
    return true;
  }
  else if( ! strcasecmp( Name, "DefaultConnection" ) )
  {
    IrcConfig.DefaultConnection = atoi( Value );
    return true;
  }
  else if( ! strcasecmp( Name, "OsdRows" ) )
  {
    IrcConfig.OsdRows = atoi( Value );
    return true;
  }
  else if( ! strcasecmp( Name, "OsdWidth" ) )
  {
    IrcConfig.OsdWidth = atoi( Value );
    return true;
  }
  else if( ! strcasecmp( Name, "OsdMarginLeft" ) )
  {
    IrcConfig.OsdMarginLeft = atoi( Value );
    return true;
  }
  else if( ! strcasecmp( Name, "OsdMarginBottom" ) )
  {
    IrcConfig.OsdMarginBottom = atoi( Value );
    return true;
  }
  else
  {
    for( IrcChannelId = 0; IrcChannelId < IRC_CONNECTIONS; IrcChannelId ++ )
    {
      snprintf( Field, sizeof( Field ), "IrcServerName%i", IrcChannelId + 1 );
      if( ! strcasecmp( Name, Field ) )
      {
        snprintf( IrcConfig.IrcServerName[IrcChannelId], sizeof( IrcConfig.IrcServerName[IrcChannelId]), "%s", Value );
	return true;
      }
      snprintf( Field, sizeof( Field ), "IrcServerPort%i", IrcChannelId + 1 );
      if( ! strcasecmp( Name, Field ) )
      {
        IrcConfig.IrcServerPort[IrcChannelId] = atoi( Value );
	return true;
      }
      snprintf( Field, sizeof( Field ), "IrcNickname%i", IrcChannelId + 1 );
      if( ! strcasecmp( Name, Field ) )
      {
        snprintf( IrcConfig.IrcNickname[IrcChannelId], sizeof( IrcConfig.IrcNickname[IrcChannelId]), "%s", Value );
	return true;
      }
      snprintf( Field, sizeof( Field ), "IrcChannelName%i", IrcChannelId + 1 );
      if( ! strcasecmp( Name, Field ) )
      {
        snprintf( IrcConfig.IrcChannelName[IrcChannelId], sizeof( IrcConfig.IrcChannelName[IrcChannelId]), "%s", Value );
	return true;
      }
    }
  }
  return false;
}

bool cPluginIrcViewer::Service(const char *Id, void *Data)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginIrcViewer::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginIrcViewer::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginIrcViewer); // Don't touch this!

//----------------------------------------------------------------------------
