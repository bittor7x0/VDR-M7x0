/*
 * fepg.c: An EPG plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/font.h>

#include <stdlib.h>
#include <time.h>

#include "fepg.h"
#include "menu.h"
#include "config.h"
#include "tools.h"
#include "screenMenu.h"


//////////////////////////////////////////////////////////////////////////////


static const char *VERSION        = "0.3.1";
static const char *DESCRIPTION    = "A Graphical EPG";
static const char *MAINMENUENTRY  = "fEPG";


//////////////////////////////////////////////////////////////////////////////


cFEpg::cFEpg(void)
{
  osd = NULL; 
  scrWidth = 719;
  scrHeight = (config.TVMode == NTSC) ? 479 : 575;
  width  = config.sX2 - config.sX1;
  height = config.sY2 - config.sY1 - TIME_H;  
  cWidth = (width - NAME_W)/NUM_COL;
  infoOpen = false;
  recOpen = false;
  curEvent = NULL;
  curRow = 1;
  curCol = 1;
  inputTime = 0;
  inputNum = 0;
     
  font = config.useSmallFont ? cFont::GetFont(fontSml) : cFont::GetFont(fontOsd);

  bgColor = toColor(config.clr1Red, config.clr1Green, config.clr1Blue, config.trans);
  green = toColor(config.clr2Red, config.clr2Green, config.clr2Blue, config.trans);
  lightgrey = toColor(170, 170, 170, config.trans);

}


//----------------------------------------------------------------------------

cFEpg::~cFEpg()
{
  delete osd;
  delete bitmap;
}


//----------------------------------------------------------------------------

void cFEpg::Show(void)
{
  // Mute
  if (config.mute)
  {
    if (!cDevice::PrimaryDevice()->IsMute()) {
      wasMuted = false;
      cDevice::PrimaryDevice()->ToggleMute();
    }
    else
      wasMuted = true;
  }
    
  osd = cOsdProvider::NewOsd(0, 0); // Will never return NULL

  tArea Area = { 0, 0, scrWidth, scrHeight,  2 };
  
  eOsdError e = osd->CanHandleAreas(&Area, 1);  
  if (e != oeOk) {     
    fprintf(stderr, "fEPG: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
    
    if (e == oeOutOfMemory && config.TVMode == PAL)
    {
      fprintf(stderr, "fEPG: Not enough OSD memory for PAL mode, using NTSC");
      scrHeight = 479;
      Area.y2 = scrHeight;
      eOsdError e = osd->CanHandleAreas(&Area, 1);  
      if (e != oeOk) { 
        fprintf(stderr, "fEPG: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
        return;
      } 
    }
    else return;
  }  
  osd->SetAreas(&Area, 1);

  bitmap = new cBitmap(scrWidth,scrHeight,2);
  Fill(0,0, scrWidth,scrHeight, clrBlack);
    
  currentTime = time(NULL);
  // Round down firstTime to nearest 30 min
  firstTime = currentTime - (currentTime % (30*60));

  oldMin = GET_MINUTES(currentTime);
     
  currentChannel = Channels.GetByNumber(cDevice::CurrentChannel());
  if (!currentChannel)
    currentChannel = Channels.First(); 
       
  Draw();
}


//----------------------------------------------------------------------------

void cFEpg::Draw()
{
  if (currentChannel == NULL)
  {
    fprintf(stderr, "fEPG: Trying to draw grid with NULL channel; Aborting.\n");
    return;
  }
  
  const int DX  = config.sX1;
  const int DY  = config.sY1;
  const int DYO = config.sY1 + TIME_H;
  const int NUM = config.numRows;
  
  time_t times[NUM_COL];

//XXX: will try to eliminate this... 
  // Clear the event matrix
  for (int i=0; i<NUM; i++)
    for (int j=0; j<3; j++)
    {
      eventMtx[i][j] = NULL;
    }


  currentTime = time(NULL);
     
  // Update firstTime - if necessary
  if (firstTime - currentTime <= -(30*60))
    firstTime += 30*60;
  
  //Fill(0,0,720,480,clrBlack);  
   
  Fill(DX+NAME_W, DYO, DX+width, DYO+height, bgColor);
  Fill(DX, DYO, DX+NAME_W, DYO+height, green);
  Fill(DX, DY, DX+width, DYO, green);
          
  // Draw current time
  char timeString[10];
  TimeToString(timeString, currentTime);         
  bitmap->DrawText(DX+brd, DY+brd, timeString, lightgrey, clrTransparent, font, NAME_W-1-brd, TIME_H-1-brd, taCenter); 
         
  // Draw the time for each column 
  for (int i=0; i<NUM_COL; i++)
  {
    times[i] = firstTime + 30*60 * i;
    TimeToString(timeString, times[i]);
    bitmap->DrawText(DX+NAME_W+cWidth*i+brd, DY+brd, timeString, lightgrey, clrTransparent, font, cWidth-1-brd, TIME_H-1-brd, taCenter);          
  } 


  const cSchedule* ScheduleList[NUM];
  cSchedulesLock SchedulesLock;
  const cSchedules* Schedules = cSchedules::Schedules(SchedulesLock);
     
  // Write the channel numbers & names 
  const cChannel* channel = currentChannel; 
  for (int i=0; i<NUM; i++)
  {
    displayedChannels[i] = channel->Number();
         
    char chanNum[8];
    snprintf(chanNum, 7, "%d", channel->Number());
    chanNum[7] = '\0';
      
    char chanName[8]; // Keep only 8 chars
    strncpy(chanName, channel->Name(), 7);
    chanName[7] = '\0';
    
    bitmap->DrawText(DX+brd, DYO+i*(height/NUM)+brd, chanNum, lightgrey, clrTransparent, font, NAME_W-1-brd, (height/NUM-1-brd)/2, taCenter|taBottom);
    bitmap->DrawText(DX+brd, DYO+i*(height/NUM)+brd+height/NUM/2, chanName, lightgrey, clrTransparent, font, NAME_W-1-brd, (height/NUM-1-brd)/2, taCenter|taTop);          
          
    ScheduleList[i] = Schedules->GetSchedule(channel->GetChannelID());   
             
    channel = getChannelDown(channel);
  }
  
  /*
  for (int i=0; i<NUM; i++)
  {
    const cEvent* event = NULL;
    time_t startTime = times[0];
    const cSchedule* schedule = ScheduleList[i];
    
    if (schedule) 
    {
      event = ScheduleList[i]->GetEventAround(startTime+10);
      if (!event) {
        event = schedule->Events()->First();
        if (event) {}
      }
      
      while (event->StartTime < times[NUM_COL-1])
      {
        
      }
    }
  }*/
     
//#if 0    
  curEvent = NULL;
    
//XXX: This loop needs to be cleened up, maybe scraped     
     //--- Put events in EPG -----
     for (int j=0; j<NUM; j++)
     {
       time_t stTime = times[0];
       long delTime = 3*30*60;
       int sec = 0;
       while (stTime <= times[2] && sec < 3)
       {
         sec++;
         int barX = width - NAME_W;  // place for the seperator: default value
         const cEvent* event = NULL;
         
         const char* title = "No Info";
         if (ScheduleList[j])
         {
         
           event = ScheduleList[j]->GetEventAround(stTime+10);
           
           if (event)
           {
             title = event->Title();
             delTime = event->EndTime() - stTime;          
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
         if (eventMtx[j][curCol-1] == event && j == curRow-1) 
         {    
           selected = true;
           curEvent = event;
           clr1 = bgColor;
           clr2 = lightgrey;
           
           //Fill 
           Fill(cellBuf+DX+NAME_W, DYO+j*(height/NUM), cellBuf+DX+NAME_W+barX, DYO+(j+1)*(height/NUM), lightgrey); 
         }  

         int availableWidth = barX;
         int offSet = 1;

         // Draw Arrow if event has already started or will not end
         if (event != NULL)
         {
           if (event->StartTime() < times[0]) 
           {
             bitmap->DrawSlope(DX+NAME_W+6, DYO+j*(height/NUM)+12 + 2, DX+NAME_W+16, DYO+(j*height/NUM+height/NUM/2)+1 -1 , clr1, 0);
             bitmap->DrawSlope(DX+NAME_W+6, DYO+(j*height/NUM+height/NUM/2)+1 -1, DX+NAME_W+16, DYO+(j+1)*(height/NUM)-9 -2, clr1, 3);
             offSet = 10;
             availableWidth -= offSet;
           }
           if (event->EndTime() > (times[NUM_COL-1]+60*30))
           {             
             bitmap->DrawSlope(DX+width-15 +2, DYO+j*(height/NUM)+12 + 2, DX+width-3, DYO+(j*height/NUM+height/NUM/2)+1, clr1, 2);
             bitmap->DrawSlope(DX+width-15 +2, DYO+(j*height/NUM+height/NUM/2)+1, DX+width-3, DYO+(j+1)*(height/NUM)-9 -2, clr1, 1);
             offSet = -1;
             availableWidth -= 16;
           }   
         }    
         
         char* newTitle = NULL;
         if (event != NULL && !(event->StartTime() < times[0]))
         {
           time_t min = GET_MINUTES( event->StartTime() );
           if ( min != 0 && min != 30 )
           {
             char timeBuf[10];
             TimeToString(timeBuf, event->StartTime());
             asprintf(&newTitle, "(%s) %s", timeBuf, title);
           }
         }
         
         cTextWrapper wrappedTitle((newTitle == NULL) ? title : newTitle, font, availableWidth - 15);
         
         // Draw name of event
         if (wrappedTitle.Lines() == 1) {
           bitmap->DrawText(cellBuf+DX+NAME_W+offSet+brd, DYO+j*(height/NUM)+brd, wrappedTitle.GetLine(0), clr1, clrTransparent, font, availableWidth-1-brd, height/NUM-1-brd, taCenter);
         }  
         else 
         {
           bitmap->DrawText(cellBuf+DX+NAME_W+offSet+brd, DYO+j*(height/NUM)+brd, wrappedTitle.GetLine(0), clr1, clrTransparent, font, availableWidth-1-brd, (height/NUM-1-brd)/2, taCenter|taBottom);
           bitmap->DrawText(cellBuf+DX+NAME_W+offSet+brd, DYO+j*(height/NUM)+brd+height/NUM/2, wrappedTitle.GetLine(1), clr1, clrTransparent, font, availableWidth-1-brd, (height/NUM-1-brd)/2, taCenter|taTop);          
         }

         if (selected) // Draw a box around selected event
         {      
           // TODO: Adjust using brd		       
           Fill(cellBuf+DX+NAME_W+3, DYO+j*(height/NUM), cellBuf+DX+NAME_W+4, DYO+(j+1)*(height/NUM), clrBlack);  //Left
           Fill(cellBuf+DX+NAME_W+barX-1, DYO+j*(height/NUM), cellBuf+DX+NAME_W+barX+1, DYO+(j+1)*(height/NUM), clrBlack);  //Right
           Fill(cellBuf+DX+NAME_W+3, DYO+j*(height/NUM)+3, cellBuf+DX+NAME_W+barX-1, DYO+j*(height/NUM)+4, clrBlack);   // Top
           Fill(cellBuf+DX+NAME_W+3, DYO+(j+1)*(height/NUM)-1, cellBuf+DX+NAME_W+barX-1, DYO+(j+1)*(height/NUM), clrBlack);  // Bottom 

         }
        
         // Draw seperator bar
         Fill(cellBuf+DX+NAME_W+barX, DYO+j*(height/NUM), cellBuf+DX+NAME_W+barX+brd-1, DYO+(j+1)*(height/NUM), lightgrey); 

//XXX: Redo this
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
     
     //------
//#endif

  // Vertical lines
  Fill(DX, DY, DX+brd-1, DYO+(height/NUM)*NUM, lightgrey); //Vert Left
  Fill(DX+width, DY, DX+width+brd-1, DYO+(height/NUM)*NUM, lightgrey); //Vert right
  Fill(DX+NAME_W, DY, DX+NAME_W+brd-1, DYO+(height/NUM)*NUM, lightgrey);
    
  for (int i=1; i<NUM_COL; i++)
  {
    Fill(DX+NAME_W+i*cWidth, DY, DX+NAME_W+i*cWidth+brd-1, DYO, lightgrey);
  }

  Fill(DX, DY, DX+width+brd-1, DY+brd-1, lightgrey);  // Top

  // Horizontal lines
  for (int k=0; k<=NUM; k++)
  {
    Fill(DX, DYO+(height/NUM)*k, DX+width+brd, DYO+(height/NUM)*k+brd-1, lightgrey);         
  }

  
  UpdateScreen();
}


//----------------------------------------------------------------------------

void cFEpg::ShowInfoWindow(const cEvent* e)
{
  if (e == NULL) return;
  
  DarkenBg();

  infoOpen = true;
  int W = (int)(0.8 * scrWidth);  // Width
  int H = (int)(0.7 * scrHeight); // Height
  int B = 10; // Border
  
  int Y = (scrHeight-H)/2; // Y Coord. of upper left corner
  int X = (scrWidth -W)/2;  // X Coord. of upper left corner
 
  // The Info Box 
  Fill(X, Y, X+W, Y+H, green);
  Fill(X, Y, X+W, Y+2, clrBlack);     //top
  Fill(X, Y+H-2, X+W, Y+H, clrBlack); //bottom
  Fill(X, Y, X+2, Y+H, clrBlack);     //left
  Fill(X+W-2, Y, X+W, Y+H, clrBlack); //right

  // The program name
  Fill(X+B,Y+B, X+W-B, Y+50, lightgrey);
  
  const char* name = e->Title() ? e->Title() : "";
  Text(X+(W-(Width(name)))/2, Y+15, name, clrBlack, lightgrey);

  // The description box
  int TOP = Y+2*B+50-10;
  Fill(X+B, TOP, X+W-B, Y+H-B, lightgrey);
  Fill(X+B, Y+H-B-60, X+W-B, Y+H-B-58, clrBlack);
 
  // Start and end times
  char start[40]; strcpy(start, "Start  : ");
  char end[40];   strcpy(end,   "End   : ");
  TimeToString(start + 9, e->StartTime());
  TimeToString(end + 8, e->EndTime()); 
  
  // Event duration
  char* dur = NULL;
  char* duration = NULL;

  if (e->StartTime() > currentTime || e->EndTime() < currentTime)
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
  asprintf(&channel, "Channel : %d", displayedChannels[curRow-1]); 

  Text(X+2*B, Y+H-B-55, start, clrBlack, lightgrey);
  Text(X+2*B, Y+H-B-30, end, clrBlack, lightgrey);
  
  int side = max(Width(duration), Width(channel));
  Text(X+W-2*B-side, Y+H-B-55, duration, clrBlack, lightgrey);
  Text(X+W-2*B-side, Y+H-B-30, channel, clrBlack, lightgrey);

  free(duration);
  free(channel);
  
  const char* description= e->Description() ? e->Description() : "No Info"; 
  
  cTextWrapper desc;
  
  // Search for certain terms that usually succeed an episode title 
  // and create a new line for that title
  if (!BEV_DN) 
    desc.Set(description, font, W-6*B);
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
    desc.Set(d, font, W-6*B);
    free(d); 
  }
  
  // Draw the description text
  for (int n=0; n < desc.Lines() && n < 7; n++)
  {
  	int pad = X+3*B;
  	if (config.centerInfo)
  		pad = X+(W - Width( desc.GetLine(n) ))/2;
  	int buf=0;	
    Text(pad, TOP+10+buf+n*27, desc.GetLine(n), clrBlack, lightgrey);     
  }
  
  UpdateScreen();

}

//----------------------------------------------------------------------------

void cFEpg::DarkenBg(void)
{
  for (int y=config.sY1; y<config.sY2; y++) 
  {
    int off = y%2;
    for (int x=config.sX1; x<config.sX2+3; x+=2)
      bitmap->DrawPixel(x+off, y, clrBlack);
  }
}


//----------------------------------------------------------------------------

eOSState cFEpg::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  
  if (state == osUnknown) 
  {  
    if (infoOpen) return InfoProcessKey(Key); // Must process info before rec  
    if (recOpen)  return RecProcessKey(Key);

    
    // Move up by one channel
    if (NORMALKEY(Key) == ekeys[config.keyUp])
    {
      if (curRow == 1)
        currentChannel = getChannelUp(currentChannel);
      else
        curRow--;

      Draw();
    }
         
    // Move down by one channel
    else if (NORMALKEY(Key) == ekeys[config.keyDown])
    { 
      if (curRow == config.numRows)
        currentChannel = getChannelDown(currentChannel);
      else
        curRow++;
          
      Draw();
    }


    // Scroll left by 0:30
    else if (NORMALKEY(Key) == ekeys[config.keyLeft])
    {  
      if (inputNum) {
        firstTime -= 60*60*inputNum; 
        time_t t = time(NULL);
        t = t - (t % (30*60));
        if (firstTime < t) firstTime = t;
        inputNum = 0;
      } 
      //Make sure firstTime doesn't get too small
      time_t ct = time(NULL);
      ct = ct - (ct%60); 
      if (!((firstTime - ct)/60 <= 0 && curCol == 1)) 
      {  
        if (curCol == 1) {
          firstTime -= 30*60;     
        }
        else // XXX: REDO
        {
          bool stop = false;
              
          const cEvent* e = eventMtx[curRow-1][curCol-1];
                          
          do
          { 
            if (curCol == 1)
            {
              ProcessKey((eKeys) config.keyLeft);
              curCol = 2;
              stop = true;
            }
            curCol--;
          } while(e == eventMtx[curRow-1][curCol-1] && !stop);
        }
      }
      Draw();
    }
       

    // Scroll right by 0:30
    else if (NORMALKEY(Key) == ekeys[config.keyRight])
    {
      if (inputNum) { // Jump forward by inputNum hours
        firstTime += 60*60*inputNum;
        inputNum = 0;
      }
      else if (curCol == NUM_COL) {
        firstTime += 30*60;   
      }
      else //XXX: REDO
      {
        bool stop = false;
              
        const cEvent* e = eventMtx[curRow-1][curCol-1];
            
        do
        {
          if (curCol == 3)
          {
            ProcessKey((eKeys)config.keyRight);
            curCol = 2;
            stop = true;
          }
          curCol++;
        } while(e == eventMtx[curRow-1][curCol-1] && !stop);
      }
      Draw();
    }

    // Open/close info window
    else if (Key == ekeys[config.keyInfo])
    {
      ShowInfoWindow(curEvent);
    }  

    // Page up
    else if (NORMALKEY(Key) == ekeys[config.keyPageUp])
    {
      for (int i=0; i<=config.numRows-curRow; i++)
        currentChannel = getChannelUp(currentChannel);
      curRow=1;  
      Draw();
    }
    
    // Page down
    else if (NORMALKEY(Key) == ekeys[config.keyPageDown])
    {
      for (int i=0; i<curRow; i++)
        currentChannel = getChannelDown(currentChannel);
      curRow=config.numRows;
      Draw();     
    }

    // Exit EPG, no channel switch
    else if (Key == ekeys[config.keyBack])
    {
      if (config.mute && !wasMuted && cDevice::PrimaryDevice()->IsMute())
        cDevice::PrimaryDevice()->ToggleMute();
               
      return osEnd;
    }    

    // Get numerical input
    else if (Key >= k0 && Key <= k9)
    {
      // Adapted from vdr's menu.c
      if (inputNum >= 0) {
        inputNum = inputNum * 10 + Key - k0;
        if (inputNum > 0 && inputNum < 10000000) {
          // Draw Input Number
          Fill(config.sX1+brd, config.sY1+brd, config.sX1+NAME_W-1, config.sY1+TIME_H-1, green);
               
          char inNum[10];
          snprintf(inNum, sizeof(inNum), "%d_", inputNum);
          bitmap->DrawText(config.sX1+brd, config.sY1+brd, inNum, lightgrey, clrTransparent, font, NAME_W-1, TIME_H-1, taCenter); 
          UpdateScreen();  
               
          cChannel* channel = Channels.GetByNumber(inputNum);
          inputTime = cTimeMs::Now();
          // Lets see if there can be any useful further input
          int n = channel ? inputNum * 10 : 0;
          cChannel* ch = channel;
          while (ch && (ch = Channels.Next(ch)) != NULL) {
            if (!ch->GroupSep()) 
            {
              if (n <= ch->Number() && ch->Number() <= n + 9) {
                n = 0;
                break;
              }
              if (ch->Number() > n)
                n *= 10;
            }
          }
          if (n > 0) {
            // This channel is the only one that fits the input, so let's take it right away:
            curRow = 1;
            currentChannel = channel;
            Draw();
            inputNum = 0; 
          }
        }
      }
    }
       
    // Record selected event
    else if (Key == ekeys[config.keyRecord])
    {
      if (!recOpen)
        ShowRecordDialog(curEvent); 
    }
           
    //Switch channel and exit
    else if (Key == ekeys[config.keyOk])
    {
      // Make sure we are not recording
      // TODO: what about multiple devices?
      //       maybe this check is no longer needed with newer VDR versions
      if (cRecordControls::Active()) {
        DarkenBg();
        int boxW = 350;
        int boxH = 110;
        int boxX = (scrWidth - boxW)/2;
        int boxY = (scrHeight - boxH)/2;
        Fill(boxX, boxY, boxX+boxW, boxY+boxH, clrBlack);
        Fill(boxX+3, boxY+3, boxX+boxW-3, boxY+boxH-3, green);
        Fill(boxX+10, boxY+10, boxX+boxW-10, boxY+boxH-10, lightgrey);
        const char* msg1 = "Cannot switch channels";
        const char* msg2 = "while recording";
        Text(boxX+(boxW-Width(msg1))/2, boxY+25, msg1, clrBlack, lightgrey);
        Text(boxX+(boxW-Width(msg2))/2, boxY+55, msg2, clrBlack, lightgrey);
        UpdateScreen();
      }
      else {
        if ( (curEvent ? curEvent->StartTime() : firstTime) <= time(NULL) )
        {
          if (cDevice::CurrentChannel() != displayedChannels[curRow-1]) {
            Fill(0,0,scrWidth,scrHeight,clrBlack);
            UpdateScreen();
            Channels.SwitchTo(displayedChannels[curRow-1]);
          }

          if (config.mute && !wasMuted && cDevice::PrimaryDevice()->IsMute())
            cDevice::PrimaryDevice()->ToggleMute();

          return osEnd;
        } 
        else  // Open info window
        {
          ShowInfoWindow(curEvent);
        }
      }   
    }
    
    else if (Key == kNone)
    {       		     		
      if (inputNum && cTimeMs::Now() - inputTime > (uint) config.timeout) 
      {
        cChannel* ch = Channels.GetByNumber(inputNum);
        if (ch) {
          currentChannel = ch;
          curRow = 1;
        }

        inputNum = 0; 
        Draw();           
      }  
          
      // Keep updating the clock
      int minNow = GET_MINUTES( time(NULL) );
      if (minNow != oldMin) {
        oldMin = minNow;
        Draw();
      }		 
    }    
        
    else 
      return state;

    state = osContinue;
  }
     
  return state;
}


//----------------------------------------------------------------------------

eOSState cFEpg::RecProcessKey(eKeys Key)
{
  // Back/Exit
  if (Key == ekeys[config.keyBack])
  {
    recOpen = false;
    Draw(); 
  }
  
  else if (Key == ekeys[config.keyOk])
  {
    recOpen = false;
    cTimer* timer = new cTimer(curEvent);
    Timers.Add(timer);           
    Timers.Save();
           
    Draw();
  }
    
  return osContinue;
}


//----------------------------------------------------------------------------

eOSState cFEpg::InfoProcessKey(eKeys Key)
{
  // Close info window
  if (Key == ekeys[config.keyInfo] && !recOpen)
  {
    infoOpen = false;
    Draw();
  }
  
  // Back/Exit
  else if (Key == ekeys[config.keyBack])
  {
    if (recOpen) {
      recOpen = false;                    
      ShowInfoWindow(curEvent);                  
    }
    else {
      infoOpen = false;
      Draw();
    }  
  }  
   
  else if (Key == ekeys[config.keyOk])
  {   
    // Set Timer 
    if (recOpen) { 
      recOpen = false;
      cTimer* timer = new cTimer(curEvent);
      Timers.Add(timer);           
      Timers.Save();
      ShowInfoWindow(curEvent);       
    }
    // Exit Info
    else {
      infoOpen = false;
      Draw();
    }
  }
  
  // Record event
  else if (Key == ekeys[config.keyRecord])
  {
    if (!recOpen)
      ShowRecordDialog(curEvent); 
  }
  
  return osContinue;
}


//----------------------------------------------------------------------------

inline void cFEpg::Text(int x, int y, const char* s, tColor c1, tColor c2)
{
   bitmap->DrawText(x, y, s, c1, c2, font);
}
  

//----------------------------------------------------------------------------
  
inline int cFEpg::Width(const char* s)
{
   return font->Width(s);
}
  

//----------------------------------------------------------------------------
  
inline void cFEpg::Fill(int x1, int y1, int x2, int y2, tColor c)
{
   bitmap->DrawRectangle(x1, y1, x2, y2, c);     
}
  

//----------------------------------------------------------------------------

inline void cFEpg::UpdateScreen(void)
{ 
  osd->DrawBitmap(0, 0, *bitmap);
  osd->Flush();
}


//----------------------------------------------------------------------------

void cFEpg::ShowRecordDialog(const cEvent* e)
{
  if (e)
  {
    recOpen = true;
    DarkenBg();
    
    int boxW = 350;
    int boxH = 110;
    int boxX = (scrWidth - boxW)/2;
    int boxY = (scrHeight- boxH)/2;
    Fill(boxX, boxY, boxX+boxW, boxY+boxH, clrBlack);
    Fill(boxX+3, boxY+3, boxX+boxW-3, boxY+boxH-3, green);
    Fill(boxX+10, boxY+10, boxX+boxW-10, boxY+boxH-10, lightgrey);
    const char* msg1 = "Really record this event?";
    Text(boxX+(boxW-Width(msg1))/2, boxY+40, msg1, clrBlack, lightgrey);
    UpdateScreen();
  }
}


//----------------------------------------------------------------------------

inline const cChannel* getChannelUp(const cChannel* c)
{
  if (config.chanOrder == DESCENDING)
    return getNextChannel(c);
  else
    return getPrevChannel(c); 
}


//----------------------------------------------------------------------------

inline const cChannel* getChannelDown(const cChannel* c)
{
  if (config.chanOrder == DESCENDING)
    return getPrevChannel(c);
  else
    return getNextChannel(c); 
}


//----------------------------------------------------------------------------

const cChannel* getNextChannel(const cChannel* c)
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

const cChannel* getPrevChannel(const cChannel* c)
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
  virtual const char *MainMenuEntry(void) { return config.hideMenuEntry ? NULL : MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
};
  
  
//////////////////////////////////////////////////////////////////////////////


cPluginFepg::cPluginFepg(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  
  // Default configuration values
  config.keyUp       = 0;
  config.keyDown     = 1;
  config.keyLeft     = 5;
  config.keyRight    = 6;
  config.keyInfo     = 8;
  config.keyPageUp   = 7;
  config.keyPageDown = 9;
  config.keyBack     = 4;
  config.keyOk       = 3;
  config.keyRecord   = 10;

  config.clr1Red   = 52;
  config.clr1Green = 79;
  config.clr1Blue  = 96;
  config.clr2Red   = 57;
  config.clr2Green = 96;
  config.clr2Blue  = 52;
  config.trans     = 255;
  
  config.mute = false;
  config.centerInfo = true;
  config.timeout = 1000; // 1 sec
  config.chanOrder = ASCENDING;
  config.timeFormat24h = true;
  config.hideMenuEntry = false;
  config.useSmallFont = false;
  config.TVMode = NTSC;
  config.sX1 = 60;
  config.sY1 = 20;
  config.sX2 = 660;  
  config.sY2 = 460;
  config.numRows = 6;
  
  requestScreenMenu = false;
}


//----------------------------------------------------------------------------

cPluginFepg::~cPluginFepg()
{
  // Clean up after yourself!
}


//----------------------------------------------------------------------------

const char *cPluginFepg::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}


//----------------------------------------------------------------------------

bool cPluginFepg::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}


//----------------------------------------------------------------------------

bool cPluginFepg::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}


//----------------------------------------------------------------------------

void cPluginFepg::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}


//----------------------------------------------------------------------------

cOsdObject *cPluginFepg::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  if (requestScreenMenu) return new cScreenMenu;
  
  return new cFEpg;
}


//----------------------------------------------------------------------------

cMenuSetupPage *cPluginFepg::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupFEpg;
}


//----------------------------------------------------------------------------

bool cPluginFepg::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "keyUp"))         config.keyUp      = atoi(Value);
  else if (!strcasecmp(Name, "keyDown"))       config.keyDown    = atoi(Value);
  else if (!strcasecmp(Name, "keyLeft"))       config.keyLeft    = atoi(Value);
  else if (!strcasecmp(Name, "keyRight"))      config.keyRight   = atoi(Value);
  else if (!strcasecmp(Name, "keyInfo"))       config.keyInfo    = atoi(Value);
  else if (!strcasecmp(Name, "keyPageUp"))     config.keyPageUp  = atoi(Value);
  else if (!strcasecmp(Name, "keyPageDown"))   config.keyPageDown = atoi(Value);
  else if (!strcasecmp(Name, "keyRecord"))     config.keyRecord  = atoi(Value);
  else if (!strcasecmp(Name, "keyBack"))       config.keyBack    = atoi(Value);
  else if (!strcasecmp(Name, "keyOk"))         config.keyOk      = atoi(Value);
  else if (!strcasecmp(Name, "mute"))          config.mute       = atoi(Value);
  else if (!strcasecmp(Name, "centerInfo"))    config.centerInfo = atoi(Value);
  else if (!strcasecmp(Name, "clr1Red"))       config.clr1Red    = atoi(Value);
  else if (!strcasecmp(Name, "clr1Green"))     config.clr1Green  = atoi(Value);
  else if (!strcasecmp(Name, "clr1Blue"))      config.clr1Blue   = atoi(Value);
  else if (!strcasecmp(Name, "clr2Red"))       config.clr2Red    = atoi(Value);
  else if (!strcasecmp(Name, "clr2Green"))     config.clr2Green  = atoi(Value);
  else if (!strcasecmp(Name, "clr2Blue"))      config.clr2Blue   = atoi(Value);
  else if (!strcasecmp(Name, "trans"))         config.trans      = atoi(Value);
  else if (!strcasecmp(Name, "timeout"))       config.timeout    = atoi(Value); 
  else if (!strcasecmp(Name, "chanOrder"))     config.chanOrder  = atoi(Value);
  else if (!strcasecmp(Name, "timeFormat24h")) config.timeFormat24h = atoi(Value);
  else if (!strcasecmp(Name, "hideMenuEntry")) config.hideMenuEntry = atoi(Value);
  else if (!strcasecmp(Name, "TVMode"))        config.TVMode = atoi(Value);
  else if (!strcasecmp(Name, "sX1"))           config.sX1 = atoi(Value);
  else if (!strcasecmp(Name, "sX2"))           config.sX2 = atoi(Value);
  else if (!strcasecmp(Name, "sY1"))           config.sY1 = atoi(Value);
  else if (!strcasecmp(Name, "sY2"))           config.sY2 = atoi(Value);
  else if (!strcasecmp(Name, "numRows"))       config.numRows = atoi(Value);
  else if (!strcasecmp(Name, "useSmallFont"))  config.useSmallFont = atoi(Value);
  else
     return false;
     
  return true;
}


//////////////////////////////////////////////////////////////////////////////


sFEpgConfig config;

bool requestScreenMenu;


//////////////////////////////////////////////////////////////////////////////


VDRPLUGINCREATOR(cPluginFepg); // Don't touch this!

