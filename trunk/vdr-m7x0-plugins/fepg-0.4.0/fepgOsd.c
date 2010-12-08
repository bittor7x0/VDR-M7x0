#include <vdr/plugin.h>
#include <vdr/interface.h>
#include <vdr/menu.h>
#include <vdr/font.h>

#include <stdlib.h>
#include <time.h>

#include "fepgOsd.h"
#include "menu.h"
#include "tools.h"
#include "screenMenu.h"
#include "colorMenu.h"
#include "i18n.h"
#include "textWrapper.h"


//////////////////////////////////////////////////////////////////////////////


static const char* const titleDelims[] = 
{ 
  " Series.", " Movie.", " News.", " Education.", " Sports.", 
  " Series/Special.", " Music/Art." 
};


static const eKeys keys[] = 
{
  kUp, kDown, kMenu, kOk, kBack, kLeft, kRight, kRed, kGreen, kYellow,
  kBlue, kPlay, kPause, kStop, kRecord, kFastFwd, kFastRew, kChanUp, kChanDn,
  kUser1, kUser2, kUser3, kUser4, kUser5, kUser6, kUser7, kUser8, kUser9
};


#ifdef VDR_RATINGS_PATCHED

static const char* const critiques[8] = 
{ 
  "", "*", "*+", "**", "**+", "***", "***+", "****" 
};

static const char* const ratings[8] = 
{
  "NR/AO", "G", "PG", "PG-13", "R", "NR/AO", "", "NC-17" 
};

#endif


#define NAME_W (config.chanWidth)
#define TIME_H (config.timeHeight)
#define DX     (config.sX1)
#define DY     (config.sY1)
#define DYO    (config.sY1 + TIME_H)  
 
 
//////////////////////////////////////////////////////////////////////////////


cFEpgOsd::cFEpgOsd(void)
{
  osd = NULL; 
  scrWidth = 719;
  scrHeight = (config.TVMode == NTSC) ? 479 : 575;
  width  = config.sX2 - config.sX1;
  height = config.sY2 - config.sY1 - TIME_H;  
  infoOpen = false;
  recOpen = false;
  warnOpen = false;
  curEvent = NULL;
  currentRow = 0;
  inputTime = 0;
  inputNum = 0;
  selChanNum = 0;
  
  rowHeight = height / config.numRows;
  colWidth  = (width - NAME_W) / config.numCols;

  font = config.useSmallFont ? cFont::GetFont(fontSml) : cFont::GetFont(fontOsd);
  
  gridClr = toColor(config.clr1Red, config.clr1Green, config.clr1Blue, config.trans);
  chanClr = toColor(config.clr2Red, config.clr2Green, config.clr2Blue, config.trans);
  timeClr = chanClr;
  textClr = toColor(170, 170, 170, config.trans);
  lineClr = textClr;
  infoClr = chanClr;
  selClr  = textClr;
  selTextClr = gridClr;

  times = new time_t[config.numCols + 1];
  timeWeight = 0;
}


//----------------------------------------------------------------------------

cFEpgOsd::~cFEpgOsd()
{
  delete osd;
  delete bitmap;
  delete[] times;
}


//----------------------------------------------------------------------------

void cFEpgOsd::Show(void)
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
    fprintf(stderr, "[fEPG] %s (#%d) %s\n", tr("OSD Error"), e, OsdErrorToString(e));
    
    if (e == oeOutOfMemory && config.TVMode == PAL)
    {
      fprintf(stderr, "[fEPG] %s\n", tr("Not enough OSD memory for PAL mode, using NTSC"));
      scrHeight = 479;
      Area.y2 = scrHeight;
      eOsdError e = osd->CanHandleAreas(&Area, 1);  
      if (e != oeOk) { 
        fprintf(stderr, "[fEPG] %s (#%d) %s\n", tr("OSD Error"), e, OsdErrorToString(e));
        return;
      } 
    }
    else return;
  }  
  osd->SetAreas(&Area, 1);

  bitmap = new cBitmap(scrWidth,scrHeight,2);
  Fill(0,0, scrWidth,scrHeight, clrBlack);
    
  currentTime = time(NULL);
  // Round down firstTime to nearest "timeInterval" min
  firstTime = currentTime - (currentTime % MIN_TO_SEC(config.timeInterval));

  curStartTime = curEndTime = firstTime;
  
  oldMin = GET_MINUTES(currentTime);
     
  currentChannel = Channels.GetByNumber(cDevice::CurrentChannel());
  if (!currentChannel)
    currentChannel = Channels.First(); 
       
  Draw();
}


//----------------------------------------------------------------------------

void cFEpgOsd::Draw()
{
  if (currentChannel == NULL)
  {
    fprintf(stderr, "[fEPG] %s\n", tr("Trying to draw grid with NULL channel. Aborting."));
    return;
  }
  
  const int numRows = config.numRows;
  const int numCols = config.numCols;
  
  currentTime = time(NULL);
     
  // Update firstTime - if necessary
  if (firstTime - currentTime <= -MIN_TO_SEC(config.timeInterval))
    firstTime += MIN_TO_SEC(config.timeInterval);
   
  Fill(DX+NAME_W, DYO, DX+width, DYO+height, gridClr);
  Fill(DX, DYO, DX+NAME_W, DYO+height, chanClr);
  
  Fill(DX, DY, DX+width, DYO, timeClr);
  if (config.darkenBg)
  {
    Darken(DX,DY, DX+width,DYO,2);
    Darken(DX,DY, DX+width,DYO,4,gridClr);
  }
          
  // Draw the current time
  char timeString[10];
  TimeToString(timeString, currentTime);         
  bitmap->DrawText(DX+brd, DY+brd, timeString, textClr, clrTransparent, font, NAME_W-1-brd, TIME_H-1-brd, taCenter); 
         
  // Draw the time for each column 
  for (int i=0; i<numCols; i++)
  {
    times[i] = firstTime + MIN_TO_SEC(config.timeInterval) * i;
    TimeToString(timeString, times[i]);
    bitmap->DrawText(DX+NAME_W+colWidth*i+brd, DY+brd, timeString, textClr, clrTransparent, font, colWidth-1-brd, TIME_H-1-brd, taCenter);          
  } 
  times[numCols] = times[numCols-1] + MIN_TO_SEC(config.timeInterval); // Not displayed

  const cSchedule* ScheduleList[numRows];
  cSchedulesLock SchedulesLock;
  const cSchedules* Schedules = cSchedules::Schedules(SchedulesLock);
     
  // Write the channel numbers & names 
  const cChannel* channel = currentChannel; 
  for (int i=0; i<numRows; i++)
  {
    if (i == currentRow) selChanNum = channel->Number();  
    
    char chanNum[8];
    snprintf(chanNum, 7, "%d", channel->Number());
    chanNum[7] = '\0';
      
    char chanName[8]; // Keep only 8 chars. TODO: calculate this based on NAME_W
    strncpy(chanName, channel->Name(), 7);
    chanName[7] = '\0';
    
    bitmap->DrawText(DX+brd, DYO+i*rowHeight+brd, chanNum, textClr, clrTransparent, font, NAME_W-1-brd, (rowHeight-1-brd)/2, taCenter|taBottom);
    bitmap->DrawText(DX+brd, DYO+i*rowHeight+brd+height/numRows/2, chanName, textClr, clrTransparent, font, NAME_W-1-brd, (rowHeight-1-brd)/2, taCenter|taTop);          
          
    ScheduleList[i] = Schedules->GetSchedule(channel->GetChannelID());   
             
    channel = getChannelDown(channel);
  }

  // Generate grid
  for (int row=0; row<numRows; row++)
  {
    if (ScheduleList[row] == NULL) { // No info for this entire row
      DrawEvent(NULL, times[0], times[numCols], row);
      continue;
    }
 
    time_t lastEndTime = times[0];

    while (lastEndTime < times[numCols])
    {
      const cEvent* event = ScheduleList[row]->GetEventAround(lastEndTime+10);
           
      // if event is null then there will be a 'no info' gap
      if (!event) {
        const cList<cEvent>* events = ScheduleList[row]->Events();
        cEvent* p;
        for (p = events->First(); p && p->StartTime()<times[numCols]; p = events->Next(p)) {
          if (p->StartTime() >= lastEndTime) {
            event = p;
            break;
          }
        }
      
        // if event is still null then we don't have any events for the remainder of this row
        if (!event) {
          DrawEvent(NULL, lastEndTime, times[numCols], row);
          lastEndTime = times[numCols];
        }
        else {
          DrawEvent(event, lastEndTime, event->StartTime(), row);
          lastEndTime = event->StartTime();
        }
      }
      
      else // Event around lastEndTime exists
      {        
        DrawEvent(event, event->StartTime(), event->EndTime(), row);
        lastEndTime = event->EndTime();
      }
    
    } // end: while (lastEndTime < times[numCols])  
    
  } // end: for (int row=0; row<numRows; row++)

 
  // Vertical lines
  Fill(DX, DY, DX+brd, DYO+rowHeight*numRows, lineClr); // Vert Left
  Fill(DX+width, DY, DX+width+brd, DYO+rowHeight*numRows, lineClr); // Vert right
  Fill(DX+NAME_W, DY, DX+NAME_W+brd, DYO+rowHeight*numRows, lineClr);
    
  for (int i=1; i<numCols; i++)
    Fill(DX+NAME_W+i*colWidth, DY, DX+NAME_W+i*colWidth+brd, DYO, lineClr);

  Fill(DX, DY, DX+width+brd, DY+brd, lineClr);  // Top

  // Horizontal lines
  for (int k=0; k<=numRows; k++)
    Fill(DX, DYO+rowHeight*k, DX+width+brd, DYO+rowHeight*k+brd, lineClr);         

  UpdateScreen();
}


//----------------------------------------------------------------------------

void cFEpgOsd::DrawEvent(const cEvent* event, time_t startTime, time_t endTime, int row)
{
  const char* title = event ? event->Title() : NULL;
  
  int gridStartTime = (startTime < times[0]) ? times[0] : startTime;
  int gridEndTime = (endTime > times[config.numCols]) ? times[config.numCols] : endTime ;
  
  int cellWidth = ((gridEndTime - gridStartTime) / (float)MIN_TO_SEC(config.timeInterval)) * colWidth;
  int sX =    ((gridStartTime - times[0])    / (float)MIN_TO_SEC(config.timeInterval)) * colWidth;
  
  // Draw RHS vertical separator if necessary
  if (gridEndTime != times[config.numCols]) 
  {
    Fill(DX+NAME_W+cellWidth+sX, DYO+rowHeight*row, DX+NAME_W+cellWidth+sX+brd, DYO+rowHeight*(row+1), lineClr); 
  }
  
  // Check if start time of event coincides with one of times[i]
  // if not append start time to the event title
  char* newTitle = NULL;
  if (title && startTime > times[0] && GET_MINUTES( startTime ) % config.timeInterval != 0)
  {
    char timeBuf[10];
    TimeToString(timeBuf, startTime);
    asprintf(&newTitle, "(%s) %s", timeBuf, title);
  } 
  
  if (title == NULL) title = tr("No Info");
    
  tColor clr1 = textClr;

  // TODO: Set some visual cue when an event has a timer
  //       if (event && event->HasTimer()) { ... }
  
  // Check to see if the current event is selected
  if (row == currentRow && startTime <= curStartTime && curStartTime < endTime)
  {    
    curStartTime = gridStartTime;
    curEndTime = gridEndTime;
    
    if (timeWeight == 0) { // Update
      timeWeight = curStartTime < times[0]+(times[config.numCols]-times[0])/2 ? curStartTime : curEndTime;
    }
    
    curEvent = event;
    
    clr1 = selTextClr;
    Fill(DX+NAME_W+sX, DYO+rowHeight*row, DX+NAME_W+cellWidth+sX, DYO+rowHeight*(row+1), selClr);
    
    // Draw black border around selected event. top and left are 2 pixel wide, bottom and right are 1 pixels wide.
    Fill(DX+NAME_W+sX+brd+1, DYO+rowHeight*row+brd+1, DX+NAME_W+cellWidth+sX-1, DYO+rowHeight*row+brd+2, clrBlack); // top
    Fill(DX+NAME_W+sX+brd+1, DYO+rowHeight*(row+1)-1, DX+NAME_W+cellWidth+sX-1, DYO+rowHeight*(row+1)-1, clrBlack); // bottom
    Fill(DX+NAME_W+sX+brd+1, DYO+rowHeight*row+brd+1, DX+NAME_W+sX+brd+2, DYO+rowHeight*(row+1)-2, clrBlack); // left
    Fill(DX+NAME_W+cellWidth+sX-1, DYO+rowHeight*row+brd+1, DX+NAME_W+cellWidth+sX-1, DYO+rowHeight*(row+1)-2, clrBlack); // right
  }
    
  int padLeft  = 10;
  int padRight = 10;
  
  if (cellWidth < 20) return;

  // Draw arrow if startTime is before times[0]
  if (startTime < times[0])
  {
    padLeft += 15;
    bitmap->DrawSlope(DX+NAME_W+sX+brd+5, DYO+row*rowHeight+brd+10, DX+NAME_W+sX+brd+15, DYO+row*rowHeight+brd + (rowHeight-brd)/2, clr1, 0);
    bitmap->DrawSlope(DX+NAME_W+sX+brd+5, DYO+row*rowHeight+brd + (rowHeight-brd)/2, DX+NAME_W+sX+brd+15, DYO+(row+1)*rowHeight-10, clr1, 3);
  }
  // Draw arrow if endTime is after times[config.numCols]
  if (endTime > times[config.numCols])
  {
    padRight += 15;
    bitmap->DrawSlope(DX+width-15, DYO+row*rowHeight+brd+10, DX+width-5, DYO+row*rowHeight+brd + (rowHeight-brd)/2, clr1, 2);
    bitmap->DrawSlope(DX+width-15, DYO+row*rowHeight+brd + (rowHeight-brd)/2, DX+width-5, DYO+(row+1)*rowHeight-10, clr1, 1);
  }
  
  int availableWidth = cellWidth - padLeft - padRight;   
  cExtTextWrapper wrappedTitle(newTitle ? newTitle : title, font, availableWidth-10, 2);
  free(newTitle);
  
  // Draw name of event
  if (wrappedTitle.Lines() == 1) 
  { 
    bitmap->DrawText(DX+NAME_W+sX+padLeft+brd, DYO+row*rowHeight+brd, wrappedTitle.GetLine(0), clr1, clrTransparent, font, availableWidth-brd, rowHeight-brd, taCenter); 
  }  
  else 
  {
    bitmap->DrawText(DX+NAME_W+sX+padLeft+brd, DYO+row*rowHeight+brd, wrappedTitle.GetLine(0), clr1, clrTransparent, font, availableWidth-brd, (rowHeight-brd)/2, taCenter|taBottom);
    bitmap->DrawText(DX+NAME_W+sX+padLeft+brd, DYO+row*rowHeight+brd + (rowHeight-brd)/2, wrappedTitle.GetLine(1), clr1, clrTransparent, font, availableWidth-brd, (rowHeight-brd)/2, taCenter|taTop);          
  }
}


//----------------------------------------------------------------------------

void cFEpgOsd::ShowInfoWindow(const cEvent* e)
{
  if (e == NULL) return;
  
  if (config.darkenBg) DarkenBg();

  infoOpen = true;
  int W = (int)(0.8 * scrWidth);  // Width
  int H = (int)(0.7 * scrHeight); // Height
  int B = 10; // Border
  
  int Y = (scrHeight-H)/2; // Y Coord. of upper left corner
  int X = (scrWidth -W)/2;  // X Coord. of upper left corner
 
  // The Info Box 
  Fill(X, Y, X+W, Y+H, infoClr);
  Fill(X, Y, X+W, Y+2, clrBlack);     //top
  Fill(X, Y+H-2, X+W, Y+H, clrBlack); //bottom
  Fill(X, Y, X+2, Y+H, clrBlack);     //left
  Fill(X+W-2, Y, X+W, Y+H, clrBlack); //right

  // The program name
  Fill(X+B,Y+B, X+W-B, Y+50, textClr);
  
  const char* name = e->Title() ? e->Title() : "";
  Text(X+(W-(Width(name)))/2, Y+15, name, clrBlack, clrTransparent);

  // The description box
  int TOP = Y+2*B+50-10;
  Fill(X+B, TOP, X+W-B, Y+H-B, lineClr);
  
#ifdef VDR_RATINGS_PATCHED
  Fill(X+B, Y+H-B-85, X+W-B, Y+H-B-83, clrBlack);
#else  
  Fill(X+B, Y+H-B-60, X+W-B, Y+H-B-58, clrBlack);
#endif
   
  // Event duration
  char* dur = NULL;
  char* duration = NULL;

  if (e->StartTime() > currentTime || e->EndTime() < currentTime)
  {
    dur = DurationToString( e->Duration() );
    asprintf(&duration, "%s : %s", tr("Duration"), dur); 
  }
  else
  {
    dur = DurationToString( e->EndTime() - currentTime + (currentTime%60) );
    asprintf(&duration, "%s : %s", tr("Time Left"), dur); 
  }
  free(dur);
  
  // Channel Number   
  char* channel = NULL;
  asprintf(&channel, "%s : %d", tr("Channel"), selChanNum); 

  // Start and end times
  const char* startStr = tr("Start : ");
  char start[40]; 
  strcpy(start, startStr);
  TimeToString(start + strlen(startStr), e->StartTime());
  
  const char* endStr = tr("End : ");
  char end[40];   
  strcpy(end, endStr);
  TimeToString(end + strlen(endStr), e->EndTime()); 
  
  Text(X+2*B, Y+H-B-55, start, clrBlack, clrTransparent);
  Text(X+2*B, Y+H-B-30, end, clrBlack, clrTransparent);
  
  int side = max(Width(duration), Width(channel));
  
#ifdef VDR_RATINGS_PATCHED

  uint16_t eventRating = e->Rating();

  char* starRating = NULL;
  char* parentalRating = NULL;

  char  detailRating[11]; // Maximal string: "[V,L,N,SC]" 10 chars
  detailRating[0] = '\0';

  if (eventRating & 0x35F) // Replace with 0x3FF when we know what bits 9 and 11 represent
  {
    strcpy(detailRating, "[");

    if (eventRating & (0x0200 | 0x0010)) // Bit 7  or 12: Violence (counting from MSB)
      strcat(detailRating, "V,");
    if (eventRating & (0x0008 | 0x0002)) // Bit 13 or 15: Language
      strcat(detailRating, "L,");
    if (eventRating & (0x0040 | 0x0004)) // Bit 10 or 14: Nudity
      strcat(detailRating, "N,");
    if (eventRating & (0x0100 | 0x0001)) // Bit 8  or 16: Sexual Content
      strcat(detailRating, "SC,");
                                         // Bits 9 and 11 have unknown meaning
    char* s = strrchr(detailRating, ',');
    if (s) s[0] = ']';
  }

  asprintf(&starRating, "%s: %s", tr("Critique"), critiques[eventRating>>13]);
  asprintf(&parentalRating, "%s: %s  %s", tr("Rating"), ratings[(eventRating>>10) & 0x07], detailRating);

  Text(X+2*B, Y+H-B-80, parentalRating, clrBlack, clrTransparent);
  
  side = max(side, Width(starRating));
  Text(X+W-2*B-side, Y+H-B-80, starRating, clrBlack, clrTransparent);
  
  free(starRating);
  free(parentalRating);
  
#endif
  
  Text(X+W-2*B-side, Y+H-B-55, duration, clrBlack, clrTransparent);
  Text(X+W-2*B-side, Y+H-B-30, channel, clrBlack, clrTransparent);

  free(duration);
  free(channel);

  const char* description = e->Description() ? e->Description() : tr("No Info"); 
  
  cTextWrapper desc;
  
  // Search for certain terms that usually succeed an episode title 
  // and create a new line for that title
  if (!BEV_DN) 
    desc.Set(description, font, W-6*B);
  else
  { 
    char* d = strdup(description);
    int size = sizeof(titleDelims) / sizeof(char*);  
    for (int i=0; i < size; i++)
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
  for (int n=0; n < desc.Lines() && n < 6; n++)
  {
  	int pad = X+3*B;
  	if (config.centerInfo)
  		pad = X+(W - Width( desc.GetLine(n) ))/2;
  	int buf=0;	
    Text(pad, TOP+10+buf+n*27, desc.GetLine(n), clrBlack, clrTransparent);     
  }
  
  UpdateScreen();
}


//----------------------------------------------------------------------------

void cFEpgOsd::Darken(int x1, int y1, int x2, int y2, int d, tColor c)
{
  for (int y=y1; y<=y2; y+=d/2) 
  {
    int offset = y%2;
    for (int x=x1; x<=x2; x+=d)
      bitmap->DrawPixel(x+offset, y, c);
  }
}


//----------------------------------------------------------------------------

inline void cFEpgOsd::DarkenBg(void)
{
  Darken(config.sX1, config.sY1, config.sX2+brd, config.sY2+brd);
}


//----------------------------------------------------------------------------

void cFEpgOsd::UpdateSelTime(void)
{
  curStartTime = curStartTime + (curEndTime - curStartTime) / 2;
  curStartTime += (timeWeight - curStartTime) / 2; // Take previous position into account
  if (curStartTime < times[0]) curStartTime = times[0];
  if (curStartTime > times[config.numCols]) curStartTime = times[config.numCols]-10;
}


//----------------------------------------------------------------------------

eOSState cFEpgOsd::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  
  if (state == osUnknown) 
  {  
    if (infoOpen) return InfoProcessKey(Key); // Must process info before rec  
    if (recOpen)  return RecProcessKey(Key);

    if (warnOpen)
    {
      if (NORMALKEY(Key) == keys[config.keyOk] || NORMALKEY(Key) == keys[config.keyBack]) {
        warnOpen = false;
        Draw();
      }
      return osContinue;
    }
    
    // Move up by one channel
    if (NORMALKEY(Key) == keys[config.keyUp])
    {
      if (currentRow == 0)
        currentChannel = getChannelUp(currentChannel);
      else
        currentRow--;
        
      UpdateSelTime();
      Draw();
    }
         
    // Move down by one channel
    else if (NORMALKEY(Key) == keys[config.keyDown])
    { 
      if (currentRow == config.numRows-1)
        currentChannel = getChannelDown(currentChannel);
      else
        currentRow++;
      
      UpdateSelTime();  
      Draw();
    }

    // Scroll left by 'config.timeInterval'
    else if (NORMALKEY(Key) == keys[config.keyLeft])
    { 
      timeWeight = 0;
      
      if (inputNum) {
        firstTime    -= HRS_TO_SEC(inputNum);
        curStartTime -= HRS_TO_SEC(inputNum);
        inputNum = 0;
      } 
      else {
        if (curStartTime <= times[0])
          firstTime -= MIN_TO_SEC(config.timeInterval);
        
        curStartTime -= 10;
      }
      
      // Make sure firstTime doesn't get too small
      time_t ct = time(NULL);
      ct -= (ct % MIN_TO_SEC(config.timeInterval));
      if (firstTime < ct) {
        firstTime = ct;
        curStartTime = firstTime;
      }  
      
      Draw();
    }
       

    // Scroll right by 'config.timeInterval'
    else if (NORMALKEY(Key) == keys[config.keyRight])
    {
      timeWeight = 0;
         
      if (inputNum) { // Jump forward by inputNum hours
        firstTime    += HRS_TO_SEC(inputNum);
        curStartTime += HRS_TO_SEC(inputNum);
        inputNum = 0;
      }
      else {
        if (curEndTime >= times[config.numCols]) 
          firstTime += MIN_TO_SEC(config.timeInterval);

        curStartTime = curEndTime;
      }

      Draw();
    }

    // Open/close info window
    else if (Key == keys[config.keyInfo])
    {
      ShowInfoWindow(curEvent);
    }  

    // Page up
    else if (NORMALKEY(Key) == keys[config.keyPageUp])
    {
      for (int i=0; i<config.numRows-currentRow; i++)
        currentChannel = getChannelUp(currentChannel);
      currentRow=0;  
      Draw();
    }
    
    // Page down
    else if (NORMALKEY(Key) == keys[config.keyPageDown])
    {
      for (int i=0; i<=currentRow; i++)
        currentChannel = getChannelDown(currentChannel);
      currentRow = config.numRows-1;
      Draw();     
    }

    // Exit EPG, no channel switch
    else if (Key == keys[config.keyBack])
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
          Fill(config.sX1+brd, config.sY1+brd, config.sX1+NAME_W-1, config.sY1+TIME_H-1, timeClr);
               
          char inNum[10];
          snprintf(inNum, sizeof(inNum), "%d_", inputNum);
          bitmap->DrawText(config.sX1+brd, config.sY1+brd, inNum, textClr, clrTransparent, font, NAME_W-1-brd, TIME_H-1-brd, taCenter); 
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
            currentRow = 0;
            currentChannel = channel;
            Draw();
            inputNum = 0; 
          }
        }
      }
    }
       
    // Record selected event
    else if (Key == keys[config.keyRecord])
    {
      if (!recOpen)
        ShowRecordDialog(curEvent); 
    }
           
    // Switch channel and exit
    else if (Key == keys[config.keyOk])
    {
      if ( (curEvent ? curEvent->StartTime() : firstTime) <= time(NULL) )
      {
        bool success = true;
        if (cDevice::CurrentChannel() != selChanNum)
          success = Channels.SwitchTo(selChanNum);

        if (success) 
          return osEnd;
        else 
        {
          warnOpen = true;
          if (config.darkenBg) DarkenBg();
          const char* msg1 = tr("Cannot switch channels");
          int boxW = 50 + Width(msg1);
          int boxH = 80;
          int boxX = (scrWidth - boxW)/2;
          int boxY = (scrHeight - boxH)/2;
          Fill(boxX, boxY, boxX+boxW, boxY+boxH, clrBlack);
          Fill(boxX+3, boxY+3, boxX+boxW-3, boxY+boxH-3, infoClr);
          Fill(boxX+10, boxY+10, boxX+boxW-10, boxY+boxH-10, textClr);

          Text(boxX+(boxW-Width(msg1))/2, boxY+25, msg1, clrBlack, clrTransparent);
          //Text(boxX+(boxW-Width(msg2))/2, boxY+55, msg2, clrBlack, clrTransparent);
          UpdateScreen();            
        }
      }
      else
      {
        ShowInfoWindow(curEvent);
      }             
    }
    
    else if (Key == kNone)
    {       		     		
      if (inputNum && cTimeMs::Now() - inputTime > (uint) config.timeout) 
      {
        cChannel* ch = Channels.GetByNumber(inputNum);
        if (ch) {
          currentChannel = ch;
          currentRow = 0;
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

eOSState cFEpgOsd::RecProcessKey(eKeys Key)
{
  // Back/Exit
  if (Key == keys[config.keyBack])
  {
    recOpen = false;
    Draw(); 
  }
  
  else if (Key == keys[config.keyOk])
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

eOSState cFEpgOsd::InfoProcessKey(eKeys Key)
{
  // Close info window
  if (Key == keys[config.keyInfo] && !recOpen)
  {
    infoOpen = false;
    Draw();
  }
  
  // Back/Exit
  else if (Key == keys[config.keyBack])
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
   
  else if (Key == keys[config.keyOk])
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
  else if (Key == keys[config.keyRecord])
  {
    if (!recOpen)
      ShowRecordDialog(curEvent); 
  }
  
  return osContinue;
}


//----------------------------------------------------------------------------

inline void cFEpgOsd::Text(int x, int y, const char* s, tColor c1, tColor c2)
{
  bitmap->DrawText(x, y, s, c1, c2, font);
}
  

//----------------------------------------------------------------------------
  
inline int cFEpgOsd::Width(const char* s) const
{
  return font->Width(s);
}
  

//----------------------------------------------------------------------------
  
inline void cFEpgOsd::Fill(int x1, int y1, int x2, int y2, tColor c)
{
  bitmap->DrawRectangle(x1, y1, x2, y2, c);     
}
  

//----------------------------------------------------------------------------

inline void cFEpgOsd::UpdateScreen(void)
{ 
  osd->DrawBitmap(0, 0, *bitmap);
  osd->Flush();
}


//----------------------------------------------------------------------------

void cFEpgOsd::ShowRecordDialog(const cEvent* e)
{
  if (e)
  {
    recOpen = true;
    if (config.darkenBg) DarkenBg();
    
    int boxW = 350;
    int boxH = 110;
    int boxX = (scrWidth - boxW)/2;
    int boxY = (scrHeight- boxH)/2;
    Fill(boxX, boxY, boxX+boxW, boxY+boxH, clrBlack);
    Fill(boxX+3, boxY+3, boxX+boxW-3, boxY+boxH-3, infoClr);
    Fill(boxX+10, boxY+10, boxX+boxW-10, boxY+boxH-10, textClr);
    const char* msg1 = tr("Really record this event?");
    Text(boxX+(boxW-Width(msg1))/2, boxY+40, msg1, clrBlack, clrTransparent);
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


