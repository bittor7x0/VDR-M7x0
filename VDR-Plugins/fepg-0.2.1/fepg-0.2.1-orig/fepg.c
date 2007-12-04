/*
 * fepg.c: An EPG plugin for the Video Disk Recorder
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

#include "fepg.h"
#include "menu.h"
#include "config.h"
#include "tools.h"


using namespace std;


static const char *VERSION        = "0.2.1";
static const char *DESCRIPTION    = "A Graphical EPG";
static const char *MAINMENUENTRY  = "fEPG";


//////////////////////////////////////////////////////////////////////////////


cFEpg::cFEpg(void)
{
     osd = NULL;
     width = SCR_WIDTH - 2*DX;
     height = SCR_HEIGHT - DY - DYO;   
     cWidth = (width - NAME_W)/3;
     info_open = false;
     rec_open = false;
     firstTime = new struct tm;
     curEvent = NULL;
     curCellh = 1;
     curCellv = 1;
     inputTime = 0;
     inputNum = 0;
     
     font = cFont::GetFont(fontOsd);

     bgColor = toColor(config.clr1Red, config.clr1Green, config.clr1Blue, config.trans);
     green = toColor(config.clr2Red, config.clr2Green, config.clr2Blue, config.trans);
     lightgrey = toColor(170, 170, 170, config.trans);

}


//----------------------------------------------------------------------------

cFEpg::~cFEpg()
{
  if (osd != NULL)
    delete osd;
  if (bitmap != NULL)
    delete bitmap;
  if (firstTime != NULL)
    delete firstTime;
}


//----------------------------------------------------------------------------

void cFEpg::draw(int start_sid, char direction = 'D')
{
     time_t times[3];

     // Clear the event matrix
     for (int i=0; i<NUM; i++)
       for (int j=0; j<3; j++)
       {
         eventMtx[i][j] = NULL;
       }

     vector<const cSchedule*> ScheduleList;
     
     cSchedulesLock SchedulesLock;
     const cSchedules* Schedules = cSchedules::Schedules(SchedulesLock);


     // Check if channel is valid
     bool ok = false;
     while(!ok)
     {
       if (start_sid < 0) direction = 'D';
       if (start_sid > Channels.MaxNumber()) direction = 'U';
       cChannel* check = Channels.GetByNumber(start_sid);
       if (check)
         ok=true;
       else
       {
         if (direction == 'U')
           start_sid--;
         else
           start_sid++;
       }    
     }  
     
     currentChannel = start_sid;

     // Get current time
     time_t rawtime;
     time(&rawtime);
     currentTime = localtime(&rawtime);
     
     // Update firstTime - if necessary
     if (mktime(firstTime) - mktime(currentTime) <= -(30*60))
     {
       if (firstTime->tm_min == 30)
       {
         firstTime->tm_hour = firstTime->tm_hour + 1;
         firstTime->tm_min = 0;
       } 
       else
       {
         firstTime->tm_min = 30;
       }
     }
   
     Fill(0,0,720,480,clrBlack);   
     Fill(DX+NAME_W, DYO, DX+width, DYO+height, bgColor);
     Fill(DX, DYO, DX+NAME_W, DYO+height, green);
     Fill(DX, DY, DX+width, DYO, green);
     
     // Draw current time
     const char* s = toTimeString(currentTime).c_str();
     Text(DX+(NAME_W-(Width(s)))/2, DY+10, s, lightgrey, green);

     // Draw the other times
     times[0] = mktime(firstTime);
     s = toTimeString(firstTime).c_str();
     Text(DX+NAME_W+(cWidth-Width(s))/2, DY+10, s, lightgrey, green);


     struct tm* t = new struct tm;
     (*t) = (*firstTime);

     if (firstTime->tm_min == 30)
     {

       t->tm_hour = firstTime->tm_hour + 1;
       t->tm_min = 0;
       s = toTimeString(t).c_str();
       
       Text(DX+NAME_W+cWidth+(cWidth-Width(s))/2, DY+10, s, lightgrey, green);


       times[1] = mktime(t);

       t->tm_min = 30;
       s = toTimeString(t).c_str();
       Text(DX+NAME_W+2*cWidth+(cWidth-Width(s))/2, DY+10, s, lightgrey, green);

       times[2] = mktime(t); 
     }
     else
     {

       t->tm_hour = firstTime->tm_hour;
       t->tm_min = 30;
       s = toTimeString(t).c_str();
       Text(DX+NAME_W+cWidth+(cWidth-Width(s))/2, DY+10, s, lightgrey, green);
       times[1] = mktime(t);

       t->tm_hour = firstTime->tm_hour + 1;
       t->tm_min = 0;
       s = toTimeString(t).c_str();
       Text(DX+NAME_W+2*cWidth+(cWidth-Width(s))/2, DY+10, s, lightgrey, green);
       times[2] = mktime(t);       
     }
     delete t;

     // Write the channel numbers & names
     int numberOfChans = 0;
     int n = 0;
     while(numberOfChans < NUM)
     {
        // Make sure channels don't get too big...
        if (start_sid + n > Channels.MaxNumber())
        {
          draw(--currentChannel, 'U');
          return;
        }

        string chanNum = to_string(start_sid + n);
        cChannel* c = Channels.GetByNumber(start_sid + n);
        
        // Make sure this is a valid channel
        if (c)
        {
          displayedChannels[numberOfChans] = start_sid + n;
          string chanName(c->Name());
          if ((int)chanName.find_first_of(";") != -1)
            chanName = chanName.substr(0, chanName.find_first_of(";"));
          
          if (chanName.length() > 8) // Keep only 8 characters
            chanName = chanName.substr(0,8);
            
          Text(DX+(NAME_W-(Width(chanNum.c_str())))/2, DYO+5+numberOfChans*(height/NUM), chanNum.c_str(), lightgrey, green);
          Text(DX+(NAME_W-(Width(chanName.c_str())))/2, DYO+32+numberOfChans*(height/NUM), chanName.c_str(), lightgrey, green);
          numberOfChans++;

          // Get Schedule for this channel
          ScheduleList.push_back(Schedules->GetSchedule(c->GetChannelID()));
        }
           
        n++;
     }

     curEvent = NULL;
    
     
     //--- Put events in EPG -----
     for (int j=0; j<NUM; j++)
     {
       time_t stTime = times[0];
       long delTime = 3*30*60;
       int sec = 0; // Just in case...
       while (stTime <= times[2] && sec < 3)
       {
         sec++;
         int barX = width - NAME_W;  // place for the seperator: default value
         const cEvent* event = NULL;
         
         const char* title = "No Info";
         if (ScheduleList.at(j))
         {
         
           event = ScheduleList.at(j)->GetEventAround(stTime+10);
           
           if (event)
           {
             title = event->Title();
             delTime = event->StartTime() + event->Duration() - stTime;
                       
             // Calculate place for the seperator
             barX = static_cast<int>((static_cast<float>(delTime/60))/30.0 * cWidth);
           }
         }
       
         int cellBuf = 0; // How far away (in X direction) to start drawing event


         if (stTime == times[0])
         {
           if (barX <= cWidth)
           {             
             barX = cWidth;
             eventMtx[j][0] = event;
           }
           else if (barX <= 2*cWidth)
           {
             barX = 2*cWidth;
             eventMtx[j][0] = event;
             eventMtx[j][1] = event;
           }
           else 
           {            
             barX = width - NAME_W;
             eventMtx[j][0] = event;
             eventMtx[j][1] = event;
             eventMtx[j][2] = event;
           }
         }
         else if (stTime == times[1])
         {
           cellBuf = cWidth;
           if (barX <= cWidth)
           {
             barX = cWidth; 
             eventMtx[j][1] = event;
           }
           else
           {
             barX = 2*cWidth;
             eventMtx[j][1] = event;
             eventMtx[j][2] = event;
           }
         }
         else if (stTime == times[2])
         {
           cellBuf = 2*cWidth;
           barX = cWidth;
           eventMtx[j][2] = event;
         }
         
         tColor clr1 = lightgrey;
         tColor clr2 = bgColor;
              
         bool selected = false;
         // Check if this cell is selected
         if (eventMtx[j][curCellv-1] == event && j == curCellh-1) 
         {    
           selected = true;
           curEvent = event;
           clr1 = bgColor;
           clr2 = lightgrey;
           
           //Fill 
           Fill(cellBuf+DX+NAME_W, DYO+j*(height/NUM), cellBuf+DX+NAME_W+barX, DYO+(j+1)*(height/NUM), lightgrey); 
    
         }  

         int availableWidth = barX;
         int offSet = 7;

         // Draw Arrow if event has already started or will not end
         if (event != NULL)
         {
           if (event->StartTime() < times[0]) 
           {
             Text(cellBuf+DX+NAME_W+6, DYO+20+j*(height/NUM), "<", clr1, clr2);
             offSet = Width("<") - 3;
             availableWidth -= offSet;
           }
           if (event->StartTime() + event->Duration() > (times[2]+60*30))
           {
             Text(DX+width-15, DYO+20+j*(height/NUM), ">", clr1, clr2);
             availableWidth -= Width(">");
             offSet = 0;
           }   
         }    

         vector<string> wrappedTitle;
         string t(title);
         if (event != NULL && !(event->StartTime() < times[0]))
         {
	         // If event doesn't start at :00 or :30, add start time as prefix
		       string st(event->GetTimeString());
		       string st2 = st.substr(st.find_first_of(":")+1);
	         if (st2 != "00" && st2 != "30")
           {
             st = to12time(st);
	           t = "("+ st +") "+ t;
           }
         }
         WrapText(t, wrappedTitle, availableWidth - 15, false);
        

         // Draw name of event
         if (wrappedTitle.size() == 1)
         {
           Text(cellBuf+DX+NAME_W+offSet+(barX-Width(wrappedTitle.at(0).c_str()))/2, DYO+20+j*(height/NUM), wrappedTitle.at(0).c_str(), clr1, clr2);
         }
         else if (wrappedTitle.at(0) == "" && wrappedTitle.size() > 1)
         {
           if (wrappedTitle.size() == 2)
             Text(cellBuf+DX+NAME_W+offSet+(barX-Width(wrappedTitle.at(1).c_str()))/2, DYO+20+j*(height/NUM), wrappedTitle.at(0).c_str(), clr1, clr2);
           else
           {
             Text(cellBuf+DX+NAME_W+offSet+(barX-Width(wrappedTitle.at(1).c_str()))/2, DYO+5+j*(height/NUM), wrappedTitle.at(0).c_str(), clr1, clr2);
             Text(cellBuf+DX+NAME_W+offSet+(barX-Width(wrappedTitle.at(2).c_str()))/2, DYO+32+j*(height/NUM), wrappedTitle.at(1).c_str(), clr1, clr2);
           }
         }
         else //TO DO: expand to cover more cases
         {
           Text(cellBuf+DX+NAME_W+offSet+(barX-Width(wrappedTitle.at(0).c_str()))/2, DYO+5+j*(height/NUM), wrappedTitle.at(0).c_str(), clr1, clr2);
           Text(cellBuf+DX+NAME_W+offSet+(barX-Width(wrappedTitle.at(1).c_str()))/2, DYO+32+j*(height/NUM), wrappedTitle.at(1).c_str(), clr1, clr2);
         }

         if (selected) // Draw a box around selected event
         {      		       
           Fill(cellBuf+DX+NAME_W+3, DYO+j*(height/NUM), cellBuf+DX+NAME_W+4, DYO+(j+1)*(height/NUM), clrBlack);  //Left
           Fill(cellBuf+DX+NAME_W+barX-1, DYO+j*(height/NUM), cellBuf+DX+NAME_W+barX+1, DYO+(j+1)*(height/NUM), clrBlack);  //Right
           Fill(cellBuf+DX+NAME_W+3, DYO+j*(height/NUM)+3, cellBuf+DX+NAME_W+barX-1, DYO+j*(height/NUM)+4, clrBlack);   // Top
           Fill(cellBuf+DX+NAME_W+3, DYO+(j+1)*(height/NUM)-1, cellBuf+DX+NAME_W+barX-1, DYO+(j+1)*(height/NUM), clrBlack);  // Bottom 

         }
        
         // Draw seperator bar
         Fill(cellBuf+DX+NAME_W+barX, DYO+j*(height/NUM), cellBuf+DX+NAME_W+barX+3, DYO+(j+1)*(height/NUM), lightgrey); 


         if (event)
         {
           stTime += delTime;

           if (stTime > times[0] && stTime <= times[1]) stTime = times[1];
           else if (stTime > times[1] && stTime <= times[2]) stTime = times[2];
           else if (stTime > times[2]) stTime = times[2] + 1000;
         }
         else // TO DO: What if there is no info now but there is some later
         {
           stTime = times[2] + 1000;
         }

       }
     }
     
     //---------------------------

     // Vertical lines
     Fill(DX, DY, DX+3, DYO+(height/NUM)*NUM, lightgrey); //Vert Left
     Fill(DX+width, DY, DX+width+3, DYO+(height/NUM)*NUM, lightgrey); //Vert right
     Fill(DX+NAME_W, DY, DX+NAME_W+3, DYO+(height/NUM)*NUM, lightgrey);
    
     for (int i=1; i<3; i++)
     {
       Fill(DX+NAME_W+i*cWidth, DY, DX+NAME_W+i*cWidth+3, DYO, lightgrey);
     }

     Fill(DX, DY, DX+width+3, DY+3, lightgrey);  // Top

     // Horizontal lines
     for (int k=0; k<=NUM; k++)
     {
       Fill(DX, DYO+(height/NUM)*k, DX+width+3, DYO+(height/NUM)*k+3, lightgrey);         
     }

  
  updateScreen();
}

//----------------------------------------------------------------------------

void cFEpg::info(const cEvent* e)
{
  if (e == NULL)
  {
    return;
  }

  info_open = true;
  int W = (int)(0.8 * SCR_WIDTH);  // Width
  int H = (int)(0.7 * SCR_HEIGHT); // Height
  int B = 10; // Border
  
  int Y = (SCR_HEIGHT-H)/2; // Y Coord. of upper left corner
  int X = (SCR_WIDTH-W)/2;  // X Coord. of upper left corner
 
  // The Info Box 
  Fill(X, Y, X+W, Y+H, green);
  Fill(X, Y, X+W, Y+2, clrBlack);     //top
  Fill(X, Y+H-2, X+W, Y+H, clrBlack); //bottom
  Fill(X, Y, X+2, Y+H, clrBlack);     //left
  Fill(X+W-2, Y, X+W, Y+H, clrBlack); //right

  // The program name
  Fill(X+B,Y+B, X+W-B, Y+50, lightgrey);
  const char* name = e->Title() ? strdup(e->Title()) : NULL;
  Text(X+(W-(Width(name)))/2, Y+15, name, clrBlack, lightgrey);

  // The description box
  int TOP = Y+2*B+50-10;
  Fill(X+B, TOP, X+W-B, Y+H-B, lightgrey);
  Fill(X+B, Y+H-B-60, X+W-B, Y+H-B-58, clrBlack);
  
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
  int num = displayedChannels[curCellh-1];
  string channel = "Channel : " + to_string(num); 

  Text(X+2*B, Y+H-B-55, start.c_str(), clrBlack, lightgrey);
  Text(X+2*B, Y+H-B-30, end.c_str(), clrBlack, lightgrey);
  
  int side = max(Width(duration.c_str()), Width(channel.c_str()));
  Text(X+W-2*B-side, Y+H-B-55, duration.c_str(), clrBlack, lightgrey);
  Text(X+W-2*B-side, Y+H-B-30, channel.c_str(), clrBlack, lightgrey);

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
    Text(X+(W-Width(subTitle))/2, TOP+10, subTitle, clrBlack, lightgrey);
  }

  // Check if text is contained in the box
  // TO DO: the number 6 is only ok for default size, should calculate this.
  if (lines.size() > 6)
    lines.at(5) = lines.at(5).substr(0, lines.at(5).length()-4) + "...";

	
  for (unsigned int n=off; n<lines.size() && n<6; n++)
  {
  	int pad = X+3*B;
  	if (config.centerInfo)
  		pad = X+(W - Width(lines.at(n).c_str()))/2;
  		
    Text(pad, TOP+10+buf+n*27, lines.at(n).c_str(), clrBlack, lightgrey); 
  }

  updateScreen();

}

//----------------------------------------------------------------------------


void cFEpg::Show(void)
{
  // Mute
  if (config.mute)
  {
    if (!cDevice::PrimaryDevice()->IsMute())
    {
      wasMuted = false;
      cDevice::PrimaryDevice()->ToggleMute();
    }
    else
      wasMuted = true;
  }
  
   
  osd = cOsdProvider::NewOsd(0, 0);


  if (osd) 
  {
     tArea Area = { 0, 0, SCR_WIDTH, SCR_HEIGHT,  2 };
     
     if (osd->CanHandleAreas(&Area, 1) != oeOk)
     {
      fprintf(stderr, "fEPG: OSD Error #%d\n", osd->CanHandleAreas(&Area, 1));
     }
     osd->SetAreas(&Area, 1);

     bitmap = new cBitmap(720,480,2);  

     // Get current time
     time_t rawtime;
     time(&rawtime);
     currentTime = localtime(&rawtime);
     
     (*firstTime) = (*currentTime);
     //Round down firstTime to nearest 30 min
     if ((*firstTime).tm_min >= 30)     
       (*firstTime).tm_min = 30;
     else
       (*firstTime).tm_min = 0;
       
     firstTime->tm_sec = 0;
     oldMin = currentTime->tm_min;
     
     currentChannel = cDevice::CurrentChannel();   
     draw(currentChannel);

  }
  else
  {
    fprintf(stderr, "fEPG: Failed to create OSD");  
  }
  
}

//----------------------------------------------------------------------------

eOSState cFEpg::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) 
  {
     
       // Move up by one channel
       if (NORMALKEY(Key) == ekeys[config.keyUp])
       {
         if (!info_open && !rec_open)
         {
           if (curCellh == 1)
             draw(--currentChannel, 'U');
           else
           {
             curCellh--;
             draw(currentChannel);
           }
         }
       }
         
       // Move down by one channel
       else if (NORMALKEY(Key) == ekeys[config.keyDown])
       {
         if (!info_open && !rec_open)
         {
           if (curCellh == NUM)
             draw(++currentChannel);
           else
           {
             curCellh++;
             draw(currentChannel);
           }
         }
       }

       // Scroll left by 0:30
       else if (NORMALKEY(Key) == ekeys[config.keyLeft])
       {   
         if (!info_open && !rec_open)
         {
           //Make sure firstTime doesn't get too small
           long ct = mktime(currentTime);
           ct = ct - (ct%60); 
           if (!((mktime(firstTime) - ct)/60 <= 0 && curCellv == 1))
           {  

            if (curCellv == 1)
            {
              if (firstTime->tm_min == 30)
                firstTime->tm_min = 0;
              else
              {  
               firstTime->tm_hour -= 1;
               firstTime->tm_min = 30;
              } 
            }
            else
            {
              bool stop = false;
              
              const cEvent* e = eventMtx[curCellh-1][curCellv-1];
                          
              do
              { 
                if (curCellv == 1)
                {
                  ProcessKey((eKeys) config.keyLeft);
                  curCellv = 2;
                  stop = true;
                }
                curCellv--;
              } while(e == eventMtx[curCellh-1][curCellv-1] && !stop);
            }

             draw(currentChannel);
          }
         }
       }

       // Scroll right by 0:30
       else if (NORMALKEY(Key) == ekeys[config.keyRight])
       {
         if (!info_open && !rec_open)
         {   
           if (curCellv == 3)
           {
             if (firstTime->tm_min == 30)
             { 
               firstTime->tm_min = 0;
               firstTime->tm_hour +=1;
             }
             else
               firstTime->tm_min = 30;
           }
           else
           {
              bool stop = false;
              
              const cEvent* e = eventMtx[curCellh-1][curCellv-1];
            
              do
              {
                if (curCellv == 3)
                {
                  ProcessKey((eKeys)config.keyRight);
                  curCellv = 2;
                  stop = true;
                }
                curCellv++;
              } while(e == eventMtx[curCellh-1][curCellv-1] && !stop);
              
           }
             draw(currentChannel);
         }
       }

       // Open/close info window
       else if (Key == ekeys[config.keyInfo])
       {
         if (info_open)
         {
           info_open = false;
           draw(currentChannel);
         }
         else if (!rec_open)
         {
           info(curEvent);  
         }   
       }  

       // Page up
       else if (NORMALKEY(Key) == ekeys[config.keyPageUp])
       {
         if (!info_open && !rec_open)
         {
           draw(jumpTo(currentChannel, NUM, 'U'));
         }
       }

       // Page down
       else if (NORMALKEY(Key) == ekeys[config.keyPageDown])
       {
         if (!info_open && !rec_open)
         {
           draw(jumpTo(currentChannel, NUM, 'D'));
         }
       }

       // Take screenshot
       else if (Key == ekeys[config.keyScreenShot])
       {
         //cDevice::PrimaryDevice()->GrabImage("img.jpg", true, 256, -1, -1);
       }

       // Exit EPG, no channel switch
       else if (Key == ekeys[config.keyBack])
       {
         if (!info_open && !rec_open)
         {
           if (config.mute)
             if (!wasMuted && cDevice::PrimaryDevice()->IsMute())
               cDevice::PrimaryDevice()->ToggleMute();
               
           return osEnd;
         }
         else if (info_open && rec_open)
         {
           rec_open = false;                    
           info(curEvent);                  
         }
         else if (info_open)
         {
           info_open = false;
           draw(currentChannel);
         }
         else if (rec_open)
         {
           rec_open = false;
           draw(currentChannel);                  
         }
       }
      
       // Get numerical input
       else if (Key >= k0 && Key <= k9)
       {
         if (!info_open && !rec_open)
         {
          // Adapted from vdr's menu.c
          if (inputNum >= 0) 
          {
            inputNum = inputNum * 10 + Key - k0;
            if (inputNum > 0 && inputNum < 10000000) 
            {
               // Draw Input Number
               Fill(DX+4, DY+4, DX+NAME_W-4, DY+32, green);
               string inNum = to_string(inputNum) + "_";
               Text(DX+(NAME_W-(Width(inNum.c_str())-Width("_")))/2, DY+10, inNum.c_str(), lightgrey, green);
               updateScreen();  
               
               
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
                  curCellh = 1;
                  draw(inputNum);
                  inputNum = 0; 
               }
            }
          }
         }
       }
       
       // Record selected event
       else if (Key == ekeys[config.keyRecord])
       {
         if (!rec_open)
         {
           record(eventMtx[curCellh-1][curCellv-1]); 
         }           
       }
       
       //Switch channel and exit or close info window
       else if (Key == ekeys[config.keyOk])
       {
       if (!info_open && !rec_open)
       {
        if (cRecordControls::Active()) // Make sure we are not recording
        {
          int boxW = 350;
          int boxH = 110;
          int boxX = (SCR_WIDTH - boxW)/2;
          int boxY = (SCR_HEIGHT - boxH)/2;
          Fill(boxX, boxY, boxX+boxW, boxY+boxH, clrBlack);
          Fill(boxX+3, boxY+3, boxX+boxW-3, boxY+boxH-3, green);
          Fill(boxX+10, boxY+10, boxX+boxW-10, boxY+boxH-10, lightgrey);
          const char* msg1 = "Cannot switch channels";
          const char* msg2 = "while recording";
          Text(boxX+(boxW-Width(msg1))/2, boxY+25, msg1, clrBlack, lightgrey);
          Text(boxX+(boxW-Width(msg2))/2, boxY+55, msg2, clrBlack, lightgrey);
          updateScreen();
        }
        else
        {
          // If the first event is selected, then switch
          const cEvent* ev = eventMtx[curCellh-1][0];
          if (ev == NULL)
          {
           //TO DO: switch to this channel if this is the first event
          }
          else if (eventMtx[curCellh-1][curCellv-1] == ev
               && ev->StartTime() < mktime(currentTime))
          {
           if (cDevice::CurrentChannel() != displayedChannels[curCellh-1])
           {
             Fill(0,0,SCR_WIDTH,SCR_HEIGHT,clrBlack);

             updateScreen();

             Channels.SwitchTo(displayedChannels[curCellh-1]);
           }

            if (config.mute)
              if (!wasMuted && cDevice::PrimaryDevice()->IsMute())
               cDevice::PrimaryDevice()->ToggleMute();

              return osEnd;
            } 
            else  // Open info window
            {
              info(curEvent);
            }
          }
         }
         else if (info_open && rec_open)
         {
           rec_open = false;
           cTimer* timer = new cTimer(eventMtx[curCellh-1][curCellv-1]);
           Timers.Add(timer);           Timers.Save();
           
           info(curEvent);       
         }
         else if (info_open)
         {
          info_open = false;
          draw(currentChannel);
         }
         else if (rec_open)
         {
           rec_open = false;
           cTimer* timer = new cTimer(eventMtx[curCellh-1][curCellv-1]);
           Timers.Add(timer);           Timers.Save();
           
           draw(currentChannel);                 
         }
       }
       else if (Key == kNone)
       {       		     		
          if (inputNum && cTimeMs::Now() - inputTime > config.timeout) 
          {
            if (Channels.GetByNumber(inputNum))
            {
                  curCellh = 1;
                  draw(inputNum);
            }

            inputNum = 0; 
            draw(currentChannel);           
          }  
          
         // Keep updating the clock
         time_t rawtime;
         time(&rawtime);
         struct tm* now = localtime(&rawtime);
         
         if (now->tm_min != oldMin && !info_open && !rec_open)
         {
           oldMin = now->tm_min;
           draw(currentChannel);          
         }			 
       }    
       else 
        return state;
       

     
      state = osContinue;
     }
     
  return state;
}


//////////////////////////////////////////////////////////////////////////////


void cFEpg::WrapText(string s, vector<string>& lines, int width, bool title)
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
      if (st=="Series." || st=="Movie." || st=="News." || st=="Education." || st=="Sports." || st=="Series/Special.") 
      {
        lines.push_back(temp);
        temp = "";
        total = 0;
        
        foundTitle = true;
      }
    }
    if (Width(tokens.at(k).c_str()) + total >= width && temp != "")
    {
      lines.push_back(temp);
      temp = "";
      total = 0;
    }

    // Make sure the token is not too wide 
    while (Width(tokens.at(k).c_str()) > width)
    {
      string split = "";
      unsigned int idx = 0;
      while (Width(split.c_str()) <= width && idx < tokens.at(k).length())
      {
        split += tokens.at(k).at(idx++);
      }
      lines.push_back(split.substr(0, split.length()-2) + "-");
      tokens.at(k) = tokens.at(k).substr(idx-2);
    }

    temp += (tokens.at(k) + " ");
    total = Width(temp.c_str());
  }
  
  if (temp != "")
    lines.push_back(temp);

}

//----------------------------------------------------------------------------

 
inline void cFEpg::Text(int x, int y, const char* s, tColor c1, tColor c2)
{
   bitmap->DrawText(x, y, s, c1, c2, font);
}
  
  
inline int cFEpg::Width(const char* s)
{
   return font->Width(s);
}
  
  
inline void cFEpg::Fill(int x1, int y1, int x2, int y2, tColor c)
{
   bitmap->DrawRectangle(x1, y1, x2, y2, c);     
}
  


//----------------------------------------------------------------------------

inline void cFEpg::updateScreen(void)
{ 
  osd->DrawBitmap(0, 0, *bitmap);
  osd->Flush();
}


//----------------------------------------------------------------------------

void cFEpg::record(const cEvent* e)
{
  if (e)
  {
    rec_open = true;
    
    int boxW = 350;
    int boxH = 110;
    int boxX = (SCR_WIDTH - boxW)/2;
    int boxY = (SCR_HEIGHT - boxH)/2;
    Fill(boxX, boxY, boxX+boxW, boxY+boxH, clrBlack);
    Fill(boxX+3, boxY+3, boxX+boxW-3, boxY+boxH-3, green);
    Fill(boxX+10, boxY+10, boxX+boxW-10, boxY+boxH-10, lightgrey);
    const char* msg1 = "Really record this event?";
    Text(boxX+(boxW-Width(msg1))/2, boxY+40, msg1, clrBlack, lightgrey);
    updateScreen();
    
  }
}


/// cPluginFepg //////////////////////////////////////////////////////////////

class cPluginFepg : public cPlugin 
{
private:
  // Add any member variables or functions you may need here.
public:
  cPluginFepg(void);
  virtual ~cPluginFepg();
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
  
  

cPluginFepg::cPluginFepg(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  config.keyUp = 0;
  config.keyDown = 1;
  config.keyLeft = 5;
  config.keyRight = 6;
  config.keyInfo = 8;
  config.keyPageUp = 7;
  config.keyPageDown = 9;
  config.keyScreenShot = 20;
  config.keyBack = 4;
  config.keyOk = 3;
  config.keyRecord = 10;
  config.mute = 1;
  config.centerInfo = 0;
  config.clr1Red = 52;
  config.clr1Green = 80;
  config.clr1Blue = 97;
  config.clr2Red = 30;
  config.clr2Green = 110;
  config.clr2Blue = 23;
  
  config.trans = 255;
  config.timeout = 1000;
}

cPluginFepg::~cPluginFepg()
{
  // Clean up after yourself!
}

const char *cPluginFepg::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginFepg::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginFepg::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginFepg::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginFepg::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cFEpg;
}

cMenuSetupPage *cPluginFepg::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupFEpg;
}

bool cPluginFepg::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "keyUp"))         config.keyUp = atoi(Value);
  else if (!strcasecmp(Name, "keyDown"))       config.keyDown = atoi(Value);
  else if (!strcasecmp(Name, "keyLeft"))       config.keyLeft = atoi(Value);
  else if (!strcasecmp(Name, "keyRight"))      config.keyRight = atoi(Value);
  else if (!strcasecmp(Name, "keyInfo"))       config.keyInfo = atoi(Value);
  else if (!strcasecmp(Name, "keyPageUp"))     config.keyPageUp = atoi(Value);
  else if (!strcasecmp(Name, "keyPageDown"))   config.keyPageDown = atoi(Value);
  else if (!strcasecmp(Name, "keyScreenShot")) config.keyScreenShot = atoi(Value);
  else if (!strcasecmp(Name, "keyRecord"))     config.keyRecord = atoi(Value);
  else if (!strcasecmp(Name, "keyBack"))       config.keyBack = atoi(Value);
  else if (!strcasecmp(Name, "keyOk"))         config.keyOk = atoi(Value);
  else if (!strcasecmp(Name, "mute"))          config.mute = atoi(Value);
  else if (!strcasecmp(Name, "centerInfo"))    config.centerInfo = atoi(Value);
  else if (!strcasecmp(Name, "clr1Red"))       config.clr1Red = atoi(Value);
  else if (!strcasecmp(Name, "clr1Green"))     config.clr1Green = atoi(Value);
  else if (!strcasecmp(Name, "clr1Blue"))      config.clr1Blue = atoi(Value);
  else if (!strcasecmp(Name, "clr2Red"))       config.clr2Red = atoi(Value);
  else if (!strcasecmp(Name, "clr2Green"))     config.clr2Green = atoi(Value);
  else if (!strcasecmp(Name, "clr2Blue"))      config.clr2Blue = atoi(Value);
  else if (!strcasecmp(Name, "trans"))         config.trans = atoi(Value);
  else if (!strcasecmp(Name, "timeout"))       config.timeout = atoi(Value); 
  
  else
     return false;
     
  return true;
}


sFEpgConfig config;



VDRPLUGINCREATOR(cPluginFepg); // Don't touch this!
