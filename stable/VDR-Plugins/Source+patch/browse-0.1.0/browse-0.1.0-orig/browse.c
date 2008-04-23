/*
 * browse.c: An EPG browser plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */


#include "menu.h"
#include "browse.h"


const char* cPluginBrowse::VERSION        = "0.1.0";
const char* cPluginBrowse::DESCRIPTION    = "An EPG browser";
const char* cPluginBrowse::MAINMENUENTRY  = "Browse";

//////////////////////////////////////////////////////////////////////////////


cBrowse::cBrowse(void)
{
  osd = NULL;
  width = static_cast<int> ((0.8) * SCR_WIDTH);
  screenHeight = config.NTSC ? 480:576;
  TOP_X = (SCR_WIDTH-width)/2;
  TOP_Y = screenHeight-HEIGHT-30;
  now = true;
  infoOpen = false;
  currentChannel = cDevice::CurrentChannel();
  event = NULL;
  fontSmall = cFont::GetFont(fontSml);
  fontBig = cFont::GetFont(fontOsd);
  inputNum = 0;
  inputTime = 0;
}


//----------------------------------------------------------------------------

cBrowse::~cBrowse()
{
  delete osd;
}


//----------------------------------------------------------------------------

void cBrowse::Draw()
{
  osd->DrawRectangle(TOP_X, TOP_Y, TOP_X+width-1, TOP_Y+HEIGHT-1, clrBackground);
  
  time_t currentTime = time(NULL);
  
  cChannel* c = Channels.GetByNumber(currentChannel);
  
  if (c == NULL) { 
    fprintf(stderr, "Browse: Trying to draw on null channel. Aborting.\n");
    return;
  }
  
  cSchedulesLock SchedulesLock;
  const cSchedules* Schedules = cSchedules::Schedules(SchedulesLock);
  const cSchedule* schedule = Schedules->GetSchedule(c->GetChannelID());
  
  event = NULL;
    
  const char* title = "No Info";
  char* timeLeft  = NULL;
  char startTime[10];
  
  if (schedule)
  {  
    if (now)
      event = schedule->GetPresentEvent();
    else
      event = schedule->GetFollowingEvent();
      
    if (event)
    {
      title = event->Title();
      TimeToString(startTime, event->StartTime()); 
      timeLeft = DurationToString(event->EndTime() - currentTime + (currentTime%60));
    }
  }
  
  if (!event || !schedule)
  {
    sprintf(startTime, "N/A");
    timeLeft  = strdup("N/A");
  }
  
  DrawTitle(title);
  
  char chanNum[8];
  snprintf(chanNum, 7, "%d", c->Number());
  chanNum[7] = '\0';
      
  char chanName[8]; // Keep only 8 chars
  strncpy(chanName, c->Name(), 7);
  chanName[7] = '\0';

  // Draw Channel Number & Name
  osd->DrawText(TOP_X+10, TOP_Y+3, chanNum, clrWhite, clrBackground, fontSmall); 
  osd->DrawText(TOP_X+10, TOP_Y+28, chanName, clrWhite, clrBackground, fontSmall); 
  
  // Draw date & time
  char timeString[10];
  TimeToString(timeString, currentTime);
  int off = TOP_X+width-fontSmall->Width(timeString)-10;
  osd->DrawText(off, TOP_Y+28, timeString, clrWhite, clrBackground, fontSmall); 
  
  struct tm tm_r;
  localtime_r(&currentTime, &tm_r);
  char date[10];
  strftime(date, 9, "%b %d", &tm_r); date[9] = '\0';
  off = TOP_X+width-fontSmall->Width(date)-10;
  osd->DrawText(off, TOP_Y+3, date, clrWhite, clrBackground, fontSmall);
  
  if (now)
  {
    // Draw time left
    char* tl = NULL;
    asprintf(&tl, "Time Left: %s", timeLeft);
    off = TOP_X+width-fontSmall->Width(tl)-10;
    osd->DrawText(off, TOP_Y+HEIGHT-30, tl, clrWhite, clrBackground, fontSmall);
    free(tl);
    
    // Draw 'Now'
    osd->DrawText(TOP_X+10, TOP_Y+HEIGHT-30, "Now", clrWhite, clrBackground, fontSmall);
  }
  else
  {
    // Draw start time
    char* st = NULL;
    asprintf(&st, "Start Time: %s", startTime);
    off = TOP_X+width-fontSmall->Width(st)-10; 
    osd->DrawText(off, TOP_Y+HEIGHT-30, st, clrWhite, clrBackground, fontSmall);
    free(st);
    
    // Draw 'Next'
    osd->DrawText(TOP_X+10, TOP_Y+HEIGHT-30, "Next", clrWhite, clrBackground, fontSmall);
  }
  free(timeLeft);
  
  osd->Flush();
}


//----------------------------------------------------------------------------

inline void cBrowse::DrawTitle(const char* title)
{
  int x = (width - fontBig->Width(title))/2; 
  osd->DrawText(TOP_X+x, TOP_Y+50, title, clrWhite, clrBackground, fontBig); }


//----------------------------------------------------------------------------

void cBrowse::Show(void)
{ 
  osd = cOsdProvider::NewOsd(0, 0);

  if (osd) 
  {   
    width = width - (width % 8);
    tArea Area = {TOP_X, TOP_Y, TOP_X+width-1, TOP_Y+HEIGHT-1,  2 };
     
    eOsdError e = osd->CanHandleAreas(&Area, 1);  
    if (e != oeOk) {
      fprintf(stderr, "Browse: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
    }
    osd->SetAreas(&Area, 1);
    Draw();   
  } 
}


//----------------------------------------------------------------------------
void cBrowse::ShowInfoWindow(const cEvent* e)
{
  if (e == NULL) return;
  
  osd->DrawRectangle(0, 0, SCR_WIDTH-1, screenHeight-1, clrClear);

  int W = (int)(0.8 * SCR_WIDTH) - 1;  // Width
  int H = (int)(0.7 * screenHeight); // Height
  int B = 10; // Border
  
  int Y = (screenHeight-H)/2; // Y Coord. of upper left corner
  int X = (SCR_WIDTH-W)/2;    // X Coord. of upper left corner
  
  // The Info Box 
  osd->DrawRectangle(X, Y, X+W, Y+H, green);
  osd->DrawRectangle(X, Y, X+W, Y+2, clrBlack);     //top
  osd->DrawRectangle(X, Y+H-2, X+W, Y+H, clrBlack); //bottom
  osd->DrawRectangle(X, Y, X+2, Y+H, clrBlack);     //left
  osd->DrawRectangle(X+W-2, Y, X+W, Y+H, clrBlack); //right

  // The program name
  osd->DrawRectangle(X+B,Y+B, X+W-B, Y+50, lightgrey);
  
  const char* name = e->Title() ? e->Title() : "";
  osd->DrawText(X+(W-(fontBig->Width(name)))/2, Y+15, name, clrBlack, lightgrey, fontBig);

  // The description box
  int TOP = Y+2*B+50-10;
  osd->DrawRectangle(X+B, TOP, X+W-B, Y+H-B, lightgrey);
  osd->DrawRectangle(X+B, Y+H-B-60, X+W-B, Y+H-B-58, clrBlack);
 
  // Start and end times
  char start[40]; strcpy(start, "Start  : ");
  char end[40];   strcpy(end,   "End   : ");
  TimeToString(start + 9, e->StartTime());
  TimeToString(end + 8, e->EndTime());
  
  // Event duration
  char* dur = NULL;
  char* duration = NULL;
  time_t currentTime = time(NULL);
  
  if (e->StartTime() > currentTime)
  {
    dur = DurationToString( e->Duration() );
    asprintf(&duration, "Duration : %s", dur); 
  }
  else
  {
    dur = DurationToString( e->EndTime() - currentTime + (currentTime%60) );
    asprintf(&duration, "Time Left : %s", dur); 
  }
  free(dur);
  
  // Channel Number   
  char* channel = NULL;
  asprintf(&channel, "Channel : %d", currentChannel); 

  osd->DrawText(X+2*B, Y+H-B-55, start, clrBlack, lightgrey, fontBig);
  osd->DrawText(X+2*B, Y+H-B-30, end, clrBlack, lightgrey, fontBig);
  
  int side = max(fontBig->Width(duration), fontBig->Width(channel));
  osd->DrawText(X+W-2*B-side, Y+H-B-55, duration, clrBlack, lightgrey, fontBig);
  osd->DrawText(X+W-2*B-side, Y+H-B-30, channel, clrBlack, lightgrey, fontBig);

  free(duration);
  free(channel);
  
  const char* description= e->Description() ? e->Description() : "No Info"; 
  
  cTextWrapper desc;
  
  // Search for certain terms that usually succeed an episode title 
  // and create a new line for that title
  if (!BEV_DN) 
    desc.Set(description, fontBig, W-6*B);
  else
  { 
    char* d = strdup(description);  
    for (int i=0; i < EXPSIZE; i++)
    {
      char* loc = strstr(d, titleDelims[i]);
      if (loc) { 
        *loc = '\n';
        break;
      }
    }
    desc.Set(d, fontBig, W-6*B);
    free(d); 
  }
  
  // Draw the description text
  for (int n=0; n < desc.Lines() && n < 7; n++)
  {
  	int pad = X+3*B;
  	if (config.centerInfo)
  		pad = X+(W - fontBig->Width( desc.GetLine(n) ))/2;
  	int buf=0;	
    osd->DrawText(pad, TOP+10+buf+n*27, desc.GetLine(n), clrBlack, lightgrey, fontBig);     
  }
  
  osd->Flush();
}

//----------------------------------------------------------------------------

eOSState cBrowse::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state != osUnknown) return state;
  
  if (infoOpen) {
    switch (Key & ~k_Repeat) 
    {
      case kGreen:
      case kBack:
      case kOk:
        delete osd; osd=NULL;
        infoOpen = false;
        Show(); 
        return osContinue;
      break;
    }
  }
    
  if (!infoOpen) {
    switch (Key & ~k_Repeat) 
    {
      // Move up by one channel
      case kUp:     
        currentChannel = GetNextChannel(Channels.GetByNumber(currentChannel))->Number();
        Draw();
      break;
         
      // Move down by one channel
      case kDown:
        currentChannel = GetPrevChannel(Channels.GetByNumber(currentChannel))->Number();
        Draw();
      break;

      case kLeft:
          now = true;
          Draw();   
      break;

      case kRight:   
          now = false;
          Draw();
      break;

      case kGreen:
        if (event) {
          delete osd;
          osd = cOsdProvider::NewOsd(0, 0);

          if (osd) {    
            int W = (int)(0.8 * SCR_WIDTH) - 1;  // Width
            int H = (int)(0.7 * screenHeight); // Height

            int Y = (screenHeight-H)/2; // Y Coord. of upper left corner
            int X = (SCR_WIDTH-W)/2;  // X Coord. of upper left corner
            
            tArea Area = {X, Y, X+W, Y+H, 2}; 

            eOsdError e = osd->CanHandleAreas(&Area, 1);  
            if (e != oeOk) {
                fprintf(stderr, "Browse: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
            }
            osd->SetAreas(&Area, 1); 
              
            infoOpen = true;  
            ShowInfoWindow(event);  
          }
        }   
      break;

      case kBack:
        return osEnd;
      break;
      
      // Go to channel #
      case k0 ... k9:
        // Adapted from vdr's menu.c
        if (inputNum >= 0) {
          inputNum = inputNum * 10 + Key - k0;
       
          if (inputNum > 0 && inputNum < 10000000) 
          {
            // Draw Input Number
            osd->DrawRectangle(TOP_X, TOP_Y, TOP_X+150, TOP_Y+50, clrBackground);
            char* inNum = NULL;
            asprintf(&inNum, "%d_", inputNum);
            osd->DrawText(TOP_X+10, TOP_Y+3, inNum, clrWhite, clrBackground, fontSmall);
            osd->Flush();   
           
            cChannel* channel = Channels.GetByNumber(inputNum);
            inputTime = cTimeMs::Now();
            // Lets see if there can be any useful further input:
            int n = channel ? inputNum * 10 : 0;
            cChannel* ch = channel;
            while (ch && (ch = Channels.Next(ch)) != NULL) {
              if (!ch->GroupSep()) {
                if (n <= ch->Number() && ch->Number() <= n + 9) 
                {
                  n = 0;
                  break;
                }
                if (ch->Number() > n)
                  n *= 10;
              }
            }
            if (n > 0) {
              // This channel is the only one that fits the input, so let's take it right away:
              currentChannel = inputNum;
              Draw();
              inputNum = 0; 
            }
          }
        }
      break;

       
      case kOk:
        if (cDevice::CurrentChannel() != currentChannel)
          Channels.SwitchTo(currentChannel); 
        return osEnd;  
      break;
      
            case kNone:
        if (inputNum && cTimeMs::Now() - inputTime > 1000) {
          if (Channels.GetByNumber(inputNum))
          {
            currentChannel = inputNum;
            Draw();
          }

          inputNum = 0;
          Draw();            
        }        
            break;
      
      
      default: return state;
    }
  }
     
  return osContinue;
}


//////////////////////////////////////////////////////////////////////////////


const cChannel* GetNextChannel(const cChannel* c)
{    
  const cChannel* newCh = c;
  do {
    if (newCh == Channels.Last()) 
      newCh = Channels.First();
    else
      newCh = Channels.Next(newCh);     
  } while (newCh->GroupSep());
     
  return newCh;  
}


//----------------------------------------------------------------------------

const cChannel* GetPrevChannel(const cChannel* c)
{  
  const cChannel* newCh = c;
  do {
    if (newCh == Channels.First())
      newCh = Channels.Last(); 
    else
      newCh = Channels.Prev(newCh); 
  } while (newCh->GroupSep());
        
  return newCh;  
}


//////////////////////////////////////////////////////////////////////////////


cPluginBrowse::cPluginBrowse(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  config.centerInfo = true;
  config.timeFormat24h = true;
  config.hideMenuEntry = false;
  config.NTSC = true;
}

cPluginBrowse::~cPluginBrowse()
{
  // Clean up after yourself!
}

const char *cPluginBrowse::MainMenuEntry(void) 
{ 
  return config.hideMenuEntry ? NULL : MAINMENUENTRY;
}

const char *cPluginBrowse::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginBrowse::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginBrowse::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginBrowse::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginBrowse::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cBrowse;
}

cMenuSetupPage *cPluginBrowse::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupBrowse;
}

bool cPluginBrowse::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "timeFormat24h")) config.timeFormat24h = atoi(Value);
  else if (!strcasecmp(Name, "hideMenuEntry")) config.hideMenuEntry = atoi(Value);
  else if (!strcasecmp(Name, "centerInfo"))    config.centerInfo    = atoi(Value);
  else if (!strcasecmp(Name, "NTSC"))          config.NTSC          = atoi(Value);
  else
    return false;
     
  return true;
}


sBrowseConfig config;


VDRPLUGINCREATOR(cPluginBrowse); // Don't touch this!
