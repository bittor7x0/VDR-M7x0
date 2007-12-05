/*
 * browse.c: An EPG browser plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <vdr/menu.h>

#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <time.h>


using namespace std;
// Set to true if you are using Bell ExpressVu, false otherwise.
static const bool BEV = true;

static const char *VERSION        = "0.0.2";
static const char *DESCRIPTION    = "An EPG browser";
static const char *MAINMENUENTRY  = "Browse";


//////////////////////////////////////////////////////////////////////////////

string to12time(string);
string toTimeString(long);

//----------------------------------------------------------------------------
template<class type>
inline string to_string(const type& value)
{
  ostringstream streamOut;
  streamOut << value;
  return streamOut.str();
}

//----------------------------------------------------------------------------
string toTimeString(struct tm* t)
{
  string mins = to_string((*t).tm_min);
  if (mins.length() == 1)
    mins = "0" + mins;
  int hrs = (*t).tm_hour;
  if (hrs > 12)
    hrs = hrs % 12;
  if (hrs == 0) hrs = 12;
  return to_string(hrs) + ":" + mins;
}

//////////////////////////////////////////////////////////////////////////////


class cBrowse : public cOsdObject 
{
private:
  void WrapText(string s, vector<string>&, int width, bool);
  void drawTitle(const char*);
  void draw();
  int jump(int, char);
  void info(const cEvent*);
  
  static const int SCR_WIDTH  = 720;
  static const int SCR_HEIGHT = 480;
  static const int HEIGHT = 120;
  
    static const tColor clrClear      = (tColor) 0x00000000;
  static const tColor green         = (tColor) 0xFF016E08; //0xDD016E08;
  static const tColor lightgrey     = (tColor) 0xFFAAAAAA; //0xDDAAAAAA;
  static const tColor clrBackground = (tColor) 0x7F000000;
  
  int width;
  int TOP_X;
  int TOP_Y;
  bool now;
  bool info_open;
  int currentChannel; 
  int inputNum; 
  long inputTime;
  cOsd* osd;
  const cFont* fontSmall;
  const cFont* fontBig;

  const cEvent* event;
  

public:
  cBrowse(void);
  ~cBrowse();
  virtual void Show(void);
  virtual eOSState ProcessKey(eKeys Key);
 };

//----------------------------------------------------------------------------

cBrowse::cBrowse(void)
{
     osd = NULL;
     width = static_cast<int> ((0.8) * SCR_WIDTH);
     TOP_X = (SCR_WIDTH-width)/2;
     TOP_Y = SCR_HEIGHT-HEIGHT-30;
     now = true;
     info_open = false;
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
  if (osd != NULL)
    delete osd;
}

//----------------------------------------------------------------------------

void cBrowse::draw()
{
  osd->DrawRectangle(TOP_X, TOP_Y, TOP_X+width-1, TOP_Y+HEIGHT-1, clrBackground);
  
  // Get current time
  time_t rawtime;
  time(&rawtime);
  struct tm* currentTime = localtime(&rawtime);
  string timeString = toTimeString(currentTime); 
  
  // TO DO: check validity of channel, is this necessary?
  cChannel* c = Channels.GetByNumber(currentChannel);
  
  string chanName(c->Name());
  string chanNum  = to_string(currentChannel);
  
  cSchedulesLock SchedulesLock;
  const cSchedules* Schedules = cSchedules::Schedules(SchedulesLock);
  const cSchedule* schedule = Schedules->GetSchedule(c->GetChannelID());
  
  event = NULL;
    
  const char* title = "No Info";
  string startTime("X:XX");
  string timeLeft("X:XX");
  if (schedule)
  {  
    if (now)
      event = schedule->GetPresentEvent();
    else
      event = schedule->GetFollowingEvent();
      
    if (event)
    {
      title = event->Title();
      
      startTime.assign(event->GetTimeString());
      if (startTime.at(0) == '0')
        startTime.at(0) = ' ';

      startTime = to12time(startTime);

      timeLeft = toTimeString((event->StartTime() + event->Duration()) - rawtime + (rawtime%60)); 
    }
  }
  
  drawTitle(title);
  

  // Draw Channel Number
  osd->DrawText(TOP_X+10, TOP_Y+3, chanNum.c_str(), clrWhite, clrBackground, fontSmall); 
  
  // Draw Channel Name
  osd->DrawText(TOP_X+10, TOP_Y+28, chanName.c_str(), clrWhite, clrBackground, fontSmall); 
  
  // Draw date & time
  int off = TOP_X+width-fontSmall->Width(timeString.c_str())-10;
  osd->DrawText(off, TOP_Y+28, timeString.c_str(), clrWhite, clrBackground, fontSmall); 
  
  string date(asctime(currentTime));
  date = date.substr(0, 10);
  off = TOP_X+width-fontSmall->Width(date.c_str())-10;
  osd->DrawText(off, TOP_Y+3, date.c_str(), clrWhite, clrBackground, fontSmall);
  
  if (now)
  {
    // Draw time left
    
    timeLeft = "Time Left: " + timeLeft;
    off = TOP_X+width-fontSmall->Width(timeLeft.c_str())-10;
    osd->DrawText(off, TOP_Y+HEIGHT-30, timeLeft.c_str(), clrWhite, clrBackground, fontSmall);
    
    // Draw Now
    osd->DrawText(TOP_X+10, TOP_Y+HEIGHT-30, "Now", clrWhite, clrBackground, fontSmall);
  }
  else
  {
    // Draw start time
    
    startTime = "Start Time: " + startTime;
    off = TOP_X+width-fontSmall->Width(startTime.c_str())-10; 
    osd->DrawText(off, TOP_Y+HEIGHT-30, startTime.c_str(), clrWhite, clrBackground, fontSmall);
    
    // Draw Next
    osd->DrawText(TOP_X+10, TOP_Y+HEIGHT-30, "Next", clrWhite, clrBackground, fontSmall);
  }
  
  osd->Flush();
}

//----------------------------------------------------------------------------

void cBrowse::drawTitle(const char* title)
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
     
     if (osd->CanHandleAreas(&Area, 1) != oeOk)
     {
      fprintf(stderr, "Browse: OSD Error #%d\n", osd->CanHandleAreas(&Area, 1));
     }
     osd->SetAreas(&Area, 1);
     
   
     draw();   
  }
  
  
}

//----------------------------------------------------------------------------

void cBrowse::info(const cEvent* e)
{
  if (e == NULL)
  {
    return;
  }
  
  osd->DrawRectangle(0, 0, SCR_WIDTH-1, SCR_HEIGHT-1, clrClear);
  
  info_open = true;
  int W = (int)(0.8 * SCR_WIDTH);  // Width
  int H = (int)(0.7 * SCR_HEIGHT); // Height
  int B = 10; // Border
  
  int Y = (SCR_HEIGHT-H)/2; // Y Coord. of upper left corner
  int X = (SCR_WIDTH-W)/2;  // X Coord. of upper left corner
 
  // The Info Box
  osd->DrawRectangle(X, Y, X+W, Y+H, green);
  osd->DrawRectangle(X, Y, X+W, Y+2, clrBlack);     //top
  osd->DrawRectangle(X, Y+H-2, X+W, Y+H, clrBlack); //bottom
  osd->DrawRectangle(X, Y, X+2, Y+H, clrBlack);     //left
  osd->DrawRectangle(X+W-2, Y, X+W, Y+H, clrBlack); //right

  // The program name
  osd->DrawRectangle(X+B,Y+B, X+W-B, Y+50, lightgrey);
  const char* name = e->Title() ? strdup(e->Title()) : NULL;
  osd->DrawText(X+(W-(fontBig->Width(name)))/2, Y+15, name, clrBlack, lightgrey, fontBig);

  // The description box
  int TOP = Y+2*B+50-10;
  osd->DrawRectangle(X+B, TOP, X+W-B, Y+H-B, lightgrey);
  osd->DrawRectangle(X+B, Y+H-B-60, X+W-B, Y+H-B-58, clrBlack);
  
  string start(e->GetTimeString());
  string end(e->GetEndTimeString());
  if (start.at(0) == '0')
    start.at(0) = ' ';
  if (end.at(0) == '0')
    end.at(0) = ' ';

  start = to12time(start);
  end   = to12time(end);


  long lDuration = e->Duration();
  start = "Start  : " + start;
  end = "End   : " + end;
  string duration = "Duration : " + toTimeString(lDuration);
 
  string channel = "Channel : " + to_string(currentChannel); 

  osd->DrawText(X+2*B, Y+H-B-55, start.c_str(), clrBlack, lightgrey, fontBig);
  osd->DrawText(X+2*B, Y+H-B-30, end.c_str(), clrBlack, lightgrey, fontBig);
  int side = fontBig->Width(duration.c_str());
  osd->DrawText(X+W-2*B-side, Y+H-B-55, duration.c_str(), clrBlack, lightgrey, fontBig);
  osd->DrawText(X+W-2*B-side, Y+H-B-30, channel.c_str(), clrBlack, lightgrey, fontBig);

  // The description text  
  vector<string> lines;
  string desc(e->Description());
  WrapText(desc, lines, W-6*B, true);

  int buf;
  unsigned int off;
  const char* subTitle;
  if (BEV)
  {
    subTitle = lines.at(0).c_str();
    off = 1;
  }
  else
  {
    subTitle = e->ShortText();
    off = 0;
  }

  if (strcmp(subTitle, "") == 0)
    buf = BEV ? -25 : 0;
  else
  {
    buf = BEV ? 0 : 25;
    osd->DrawText(X+(W-fontBig->Width(subTitle))/2, TOP+10, subTitle, clrBlack, lightgrey, fontBig);
  }

  // Check if text is contained in the box
  // TO DO: the number 6 is only ok for default size, should calculate this.
  if (lines.size() > 6)
    lines.at(5) = lines.at(5).substr(0, lines.at(5).length()-4) + "...";

  for (unsigned int n=off; n<lines.size() && n<6; n++)
  {
    osd->DrawText(X+3*B, TOP+10+buf+n*27, lines.at(n).c_str(), clrBlack, lightgrey, fontBig);
  }

  osd->Flush();

}

//----------------------------------------------------------------------------

eOSState cBrowse::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) {
     switch (Key & ~k_Repeat) {

      // Move up by one channel
      case kUp:
        if (!info_open)
        {      
          currentChannel = jump(currentChannel, 'U');
          draw();
        }
      break;
         
      // Move down by one channel
      case kDown:
        if (!info_open)
        {      
          currentChannel = jump(currentChannel, 'D');
          draw();
        }
      break;


      case kLeft:
        if (!info_open)
        { 
          now = true;
          draw();
        }   
      break;

      case kRight:
        if (!info_open)
        {      
          now = false;
          draw();
        }
      break;

      case kGreen:
        if (info_open)
        {
          delete osd;
          info_open = false;
          Show();   
        }
        else
        {
          if (event)
          {
            delete osd;
            
            osd = cOsdProvider::NewOsd(0, 0);

            if (osd) 
            {   
              tArea Area = {0, 0, SCR_WIDTH-1, SCR_HEIGHT-1, 2};
              
              if (osd->CanHandleAreas(&Area, 1) != oeOk)
              {
                fprintf(stderr, "fEPG: OSD Error #%d\n", osd->CanHandleAreas(&Area, 1));
              }
              osd->SetAreas(&Area, 1);
              //osd->Clear(); 
              
              //Show info window
              info(event);  
            }
          }
        }   
      break;


       
       case kBack:
        if (info_open)
        {
          delete osd;
          info_open = false;
          Show();   
        }
        else
         return osEnd;
       break;
      
      //Go to channel#
      case k0 ... k9:
        if (!info_open)
         {
          // Adapted from vdr's menu.c
          if (inputNum >= 0) 
          {
            inputNum = inputNum * 10 + Key - k0;
       
            
            if (inputNum > 0 && inputNum < 10000000) 
            {
               // Draw Input Number
               osd->DrawRectangle(TOP_X, TOP_Y, TOP_X+100, TOP_Y+50, clrBackground);
               string inNum = to_string(inputNum) + "_";
               osd->DrawText(TOP_X+10, TOP_Y+3, inNum.c_str(), clrWhite, clrBackground, fontSmall);
               osd->Flush();   
           
               cChannel* channel = Channels.GetByNumber(inputNum);
               inputTime = cTimeMs::Now();
               // Lets see if there can be any useful further input:
               int n = channel ? inputNum * 10 : 0;
               cChannel* ch = channel;
               while (ch && (ch = Channels.Next(ch)) != NULL) 
               {
                     if (!ch->GroupSep()) 
                     {
                        if (n <= ch->Number() && ch->Number() <= n + 9) 
                        {
                           n = 0;
                           break;
                        }
                        if (ch->Number() > n)
                           n *= 10;
                     }
               }
               if (n > 0) 
               {
                  // This channel is the only one that fits the input, so let's take it right away:
                  currentChannel = inputNum;
                  draw();
                  inputNum = 0; 
               }
            }
          }
         }
      break;

       
      case kOk:
        if (!info_open)
        {
          Channels.SwitchTo(currentChannel); 
          return osEnd; 
        } 
      break;
      
            case kNone:
        if (inputNum && cTimeMs::Now() - inputTime > 1000) 
        {
          if (Channels.GetByNumber(inputNum))
          {
            currentChannel = inputNum;
            draw();
          }

            inputNum = 0;
            draw();            
        }        
            break;
      
      
      default: return state;
      }

     
     state = osContinue;
     }
  return state;
}

//------------------------------------------------------------------
// TO DO: doesn't work with high values
int cBrowse::jump(int cc, char direction)
{
  if (direction == 'U')
    cc++;
  else
    cc--;
      bool ok = false;
    while(!ok)
    {
      if (cc < 0) direction = 'D';
      if (cc > Channels.MaxNumber()) direction = 'U';
      cChannel* check = Channels.GetByNumber(cc);
      if (check)
        ok=true;
      else
      {
        if (direction == 'U')
          cc++;
        else
          cc--;
      }    
    }
  return cc;  
}

//------------------------------------------------------------------
string to12time(string time)
{
  string min = time.substr(time.find_first_of(":")+1);
  string hr  = time.substr(0, time.find_first_of(":"));
  int iHr = atoi(hr.c_str());

  if (iHr > 12)
     iHr = iHr - 12;
 
  return to_string(iHr)+":"+min;

}

//----------------------------------------------------------------------------

string toTimeString(long time)
{
  //time = time - (time % 60);
  int mins = time / 60;
  int hrs  = mins / 60;
  mins = mins - hrs*60;
  string sMins = to_string(mins);
  if (sMins.length() == 1)
    sMins = "0" + sMins;

  return to_string(hrs) + ":" + sMins; 
}

//----------------------------------------------------------------------------
void cBrowse::WrapText(string s, vector<string>& lines, int width, bool title)
{
  // Tokenize
  vector<string> tokens;

  if (s == "")
  {
    lines.push_back("");
    return;
  }

  string word = "";
  for (unsigned int i = 0; i < s.length(); i++)
  {
    if (s.at(i) != ' ')
      word += s.at(i);
    else
    {
      tokens.push_back(word);
      word = "";
    }
  }

  tokens.push_back(word);


  int total = 0;
  string temp = "";
  bool foundTitle = false;
  for (unsigned int k = 0; k < tokens.size(); k++)
  {
    if (BEV && title && !foundTitle)
    {
      string st = tokens.at(k);
      if (st=="Series." || st=="Movie." || st=="News." || st=="Education." || st=="Sports.")
      {
        lines.push_back(temp);
        temp = "";
        total = 0;
        
        foundTitle = true;
      }
    }
    if (fontBig->Width(tokens.at(k).c_str()) + total >= width && temp != "")
    {
      lines.push_back(temp);
      temp = "";
      total = 0;
    }

    // Make sure the token is not too wide 
    while (fontBig->Width(tokens.at(k).c_str()) > width)
    {
      string split = "";
      unsigned int idx = 0;
      while (fontBig->Width(split.c_str()) <= width && idx < tokens.at(k).length())
      {
        split += tokens.at(k).at(idx++);
      }
      lines.push_back(split.substr(0, split.length()-2) + "-");
      tokens.at(k) = tokens.at(k).substr(idx-2);
    }

    temp += (tokens.at(k) + " ");
    total = fontBig->Width(temp.c_str());
  }
  
  if (temp != "")
    lines.push_back(temp);

}


//////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////
// --- cPluginBrowse --------------------------------------------------------

class cPluginBrowse : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginBrowse(void);
  virtual ~cPluginBrowse();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  };

cPluginBrowse::cPluginBrowse(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginBrowse::~cPluginBrowse()
{
  // Clean up after yourself!
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
  return NULL;
}

bool cPluginBrowse::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

VDRPLUGINCREATOR(cPluginBrowse); // Don't touch this!979gggfffggllfllgffd
