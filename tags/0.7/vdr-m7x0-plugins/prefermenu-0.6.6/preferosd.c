/*
 * preferosd.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "preferosd.h"

#include "config.h"
#include "preferchannel.h"
#include "i18n.h"

#include "symbols/up.xpm"
#include "symbols/down.xpm"
#include <vdr/device.h>

extern int number;
extern int current;
extern tChannelID preferchannels[CHANNELSMAX];

extern cPreferedChannels PreferedChannelsList;
extern cPreferedChannels PreferedChannelsListDisplay;

static const tColor clrBackground = clrGray50;

// Constructor and destructor

cPreferOsd::cPreferOsd(void)
{
  osd = NULL;
  lastChannel = tChannelID::InvalidID;
}
 
cPreferOsd::~cPreferOsd(void)
{
  delete osd;
}

// Virtual Methods

void cPreferOsd::Show()
{

  // Reading the preferList
 
  ReadPreferedChannels();

  // find the actual current
  cChannel *Channel = Channels.GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());
  if (Channel) {
    tChannelID mycurrent = Channel->GetChannelID();
    // Find the actually tuned channel
    int i=0;    
    while( (i < CHANNELSMAX) && (i < number) && !(preferchannels[i] == mycurrent) ) {
      i++;
    }                
    current = ( (number != 0) && (i != number) ) ? i+1 : 1;
  }
  osd=cOsdProvider::NewOsd(config.originx, config.originy);
  if (osd) {
    tArea Area = { 0, 0, config.width+LINEHEIGHT, config.height-1, 4};
    osd->SetAreas(&Area, 1);
    DrawPreferMenu(0,0);
    DisplayPreferMenu();
  }     
}

eOSState cPreferOsd::ProcessKey(eKeys Key)
{
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) 
  {
    //switch (Key & ~k_Repeat)
    switch (Key)
    { 
      case kUp:
      case kUp|k_Repeat:
        CursorUp();
        break; 
      case kDown:
      case kDown|k_Repeat:
        CursorDown();
        break; 
      case kLeft:
      case kLeft|k_Repeat:
        CursorLeft();
        break; 
      case kRight:
      case kRight|k_Repeat:
        CursorRight();
        break; 
      case kRed:
        RemoveChannel();
        break;
      case kGreen:
        AddChannel();
        break; 
      case kYellow:
        MoveChannel(-1);
        break;
      case kBlue:
        MoveChannel(1);
        break;
      case kOk:
        CursorOK();
		    if (config.closeonswitch)
          return osEnd;
		    else 
          break;
      case k0:
        LastChannel();
        break;
      case kBack:
        return osEnd;
      default:
        return state;
    }
    DisplayPreferMenu();
    DrawPreferMenu(0,0);
    DisplayPreferMenu();
    state = osContinue;
  }
  return state;
}


// Other public methods

void cPreferOsd::ReadPreferedChannels()
{
  int i=0;
  number = 0;
  cPreferedChannel *PreferedChannel;
  PreferedChannelsListDisplay.Sort();
  if (config.sortby == 0) 
    // If no sort, use the raw file
    PreferedChannel = PreferedChannelsList.GetPreferedChannelNumber(i);
  else
    // else use the sorted file
    PreferedChannel = PreferedChannelsListDisplay.GetPreferedChannelNumber(i);
  
  while ( PreferedChannel != NULL )
  {
    if ( (i < CHANNELSMAX) && (PreferedChannel != NULL)) {
      preferchannels[i] = PreferedChannel->GetIndex();

#ifdef DEBUG
      fprintf(stderr, "[%2.2d]->'%s'\n",\
          i, (Channels.GetByChannelID(preferchannels[i]))->Name());
#endif

    }
    number += 1;
    i++;    
    
    if (config.sortby == 0) 
      // If no sort, use the raw file
      PreferedChannel = PreferedChannelsList.GetPreferedChannelNumber(i);
    else
      // else use the sorted file
      PreferedChannel = PreferedChannelsListDisplay.GetPreferedChannelNumber(i);
  }
}

void cPreferOsd::CursorUp()
{
  if (current > 1) {
    current -= 1;
    DrawPreferMenu(-LINEHEIGHT,1);
    DisplayPreferMenu();
    //DrawPreferMneu(-10,1);
    //DisplayPreferMenu();
    //DrawPreferMenu(0,0);
    //DisplayPreferMenu();
  }
} 

void cPreferOsd::CursorDown()
{
  if (current < number) {
    current += 1;     
    DrawPreferMenu(+LINEHEIGHT,-1);
    DisplayPreferMenu();
    //DrawPreferMenu(+10,-1);
    //DisplayPreferMenu();
    //DrawPreferMenu(0,0);
    //DisplayPreferMenu();
  }
}

void cPreferOsd::CursorLeft()
{
  // Go one page left
  if (current > 7) {
    current -= 7;
  } else {
    current = 1;
  }
  DrawPreferMenu(0,0);
  DisplayPreferMenu();
}

void cPreferOsd::CursorRight()
{
  // Go one page left
  if (current < number-7) {
    current += 7;
  } else {
    current = number;
  }
  DrawPreferMenu(0,0);
  DisplayPreferMenu();
}

void cPreferOsd::CursorOK()
{
  DrawPreferMenu(0,2);
  DisplayPreferMenu();
  
  /*
   * If current channel is a preferred one we remember it as the lastChannel
   */
  lastChannel = PreferedChannelsList.GetCurrentPrefered();
  
  if ( preferchannels[current-1].Valid() ){
    cChannel *Channel =  Channels.GetByChannelID(preferchannels[current-1]);
    if (Channel)
      cDevice::PrimaryDevice()->SwitchChannel(Channel, true);
  }
}

void cPreferOsd::LastChannel()
{
  if (lastChannel.Valid())
  {
    /*
     * Before return to lastChannel we have to rememeber the actually tuned one
     */
    tChannelID oldLastChannel = lastChannel;
    lastChannel = PreferedChannelsList.GetCurrentPrefered();
    //Let's set the new current chennel and press OK :)
    for(int i=0; i<CHANNELSMAX; i++)
      if(preferchannels[i] == oldLastChannel){
        current = i+1;
        CursorOK();
        return;
      } 
  }
}

void cPreferOsd::AddChannel()
{
  if (number < CHANNELSMAX)
  {
    cChannel *Channel = Channels.GetByNumber(cDevice::PrimaryDevice()->CurrentChannel());
    if (Channel)
    {
      tChannelID current_channel = Channel->GetChannelID();
      // Test if channels is already prefered
      for (int i=0; i<number; i++)
      {
        if (current_channel == preferchannels[i]) {
          // Channel to add exist already -> go to it
          current = i+1;
          return;
        }
      }
      PreferedChannelsList.AddPreferedChannel(current_channel);
      PreferedChannelsList.Save();
      PreferedChannelsListDisplay.AddPreferedChannel(current_channel);
      ReadPreferedChannels();
      current=1;
      for (int i=0; i<number; i++)
        if (preferchannels[i] == current_channel)
          current = i+1;
    }
  }
}

void cPreferOsd::RemoveChannel()
{
  if (number > 0)
  {
    PreferedChannelsList.RemovePreferedChannel(preferchannels[current-1]);
    PreferedChannelsList.Save();
    PreferedChannelsListDisplay.RemovePreferedChannel(preferchannels[current-1]);
    if (current>0)
      current = current-1;
    else
      current = 1; 
    ReadPreferedChannels();
  }
}

void cPreferOsd::MoveChannel(int places)
{
  if (((current+places) >= 1) && ((current + places) <= number))
  {
    PreferedChannelsList.Move((current-1), ((current-1)+places));
    PreferedChannelsList.Save();
    PreferedChannelsListDisplay.Move((current-1), ((current-1)+places)); 
    current += places;
    ReadPreferedChannels();
  }
}

void cPreferOsd::DisplayPreferMenu()
{
  osd->Flush();
}

// Private Methods

void cPreferOsd::DrawPreferMenu(int delta, int highlight)
{
  
  int middle = int(config.height/2) + config.height%2;

  osd->DrawRectangle(0, 0, config.width+LINEHEIGHT, config.height, clrBackground);

  // Draw highlighted channel bar
  osd->DrawRectangle(0, middle-int(LINEHEIGHT/2),\
       config.width, middle-int(LINEHEIGHT/2)+LINEHEIGHT, clrCyan);

  // Draw Channel Names Now!
  DrawChannelsNames(delta);
 
  // Draw arrow symbols
  DrawSymbol(11, middle-int(LINEHEIGHT/2)-LINEHEIGHT-BORDERSIZE,\
      up_xpm, (highlight==1) ? clrBlack: clrWhite);
  DrawSymbol(11, middle+int(LINEHEIGHT/2)+(2*BORDERSIZE),\
      down_xpm, (highlight==-1) ? clrBlack: clrWhite);

  // Draw "OK" string
osd->DrawText(5, middle-int(LINEHEIGHT/2)+1, "OK",\
      (highlight==2) ? clrWhite : clrBlack, clrCyan, font);

  //Draw help bar
  
  // Help Colors
  osd->DrawRectangle(config.width+BORDERSIZE, BORDERSIZE,\
      config.width+LINEHEIGHT-BORDERSIZE, LINEHEIGHT+BORDERSIZE, clrRed);
  osd->DrawRectangle(config.width+BORDERSIZE, LINEHEIGHT+(2*BORDERSIZE),\
      config.width+LINEHEIGHT-BORDERSIZE, (2*LINEHEIGHT)+(2*BORDERSIZE), clrGreen);
  osd->DrawRectangle(config.width+BORDERSIZE, (2*LINEHEIGHT)+(3*BORDERSIZE),\
      config.width+LINEHEIGHT-BORDERSIZE, (3*LINEHEIGHT)+(3*BORDERSIZE), clrYellow);
  osd->DrawRectangle(config.width+BORDERSIZE, (3*LINEHEIGHT)+(4*BORDERSIZE),\
      config.width+LINEHEIGHT-BORDERSIZE, (4*LINEHEIGHT)+(4*BORDERSIZE), clrBlue);

  // Help Symbols (+/-)
  osd->DrawText(config.width+8, BORDERSIZE, "-", clrBlack, clrRed, font);
  osd->DrawText(config.width+8, LINEHEIGHT+(2*BORDERSIZE), "+", clrBlack, clrGreen, font);
  osd->DrawText(config.width+8, (2*LINEHEIGHT)+(3*BORDERSIZE), "^", clrBlack, clrYellow, font);
  osd->DrawText(config.width+8, (3*LINEHEIGHT)+(4*BORDERSIZE), "v", clrWhite, clrBlue, font);

  // Fill down with transparent
  osd->DrawRectangle(config.width+BORDERSIZE, (4*LINEHEIGHT)+(6*BORDERSIZE),\
       config.width+LINEHEIGHT+1, config.height, clrTransparent);

  // Draw Borders Here :)
  // eventually disabled with a config switch
  
  // Draw Main Frame Border
  // Vertical lines
  DrawVLine(0, 0, config.height, clrBlack);
  DrawVLine(config.width, 0, config.height, clrBlack);
  // Horizontal lines
  DrawHLine(0, 0, config.width+LINEHEIGHT, clrBlack);
  DrawHLine(0, config.height-BORDERSIZE, config.width, clrBlack);

  // Highlited channel borders
  DrawHLine(0, middle-int(LINEHEIGHT/2)-BORDERSIZE, config.width, clrBlack);
  DrawHLine(0, middle+int(LINEHEIGHT/2)+BORDERSIZE, config.width, clrBlack);
  
  // A vertical line to separate 'OK' from channel Names
  DrawVLine(int(LINEHEIGHT*1.5), 0, config.height, clrBlack);
   
  // Draw Help Frame Border
  // Vertical lines
  DrawVLine(config.width+LINEHEIGHT-BORDERSIZE, 0, (4*LINEHEIGHT)+(5*BORDERSIZE), clrBlack);
  // Horizontal lines
  DrawHLine(config.width, 0, config.width+LINEHEIGHT, clrBlack);
  DrawHLine(config.width, LINEHEIGHT+2*BORDERSIZE, config.width+LINEHEIGHT, clrBlack);
  DrawHLine(config.width, (2*LINEHEIGHT)+(3*BORDERSIZE), config.width+LINEHEIGHT, clrBlack);
  DrawHLine(config.width, (3*LINEHEIGHT)+(4*BORDERSIZE), config.width+LINEHEIGHT, clrBlack);
  DrawHLine(config.width, (4*LINEHEIGHT)+(5*BORDERSIZE), config.width+LINEHEIGHT, clrBlack);
}

void cPreferOsd::DrawChannelsNames(int delta)
{

  // XXX The margin is hardcoded for now
  // TODO: make it more flexible
  int margin=BORDERSIZE;
  
  if (config.lines==3)
    margin += (BORDERSIZE+2);
  else if (config.lines==5)
    margin+= (BORDERSIZE+3);
  else
    margin+= (BORDERSIZE+4);
  
  int d = int(LINEHEIGHT*1.7);

  if ( (number > 0) && (current > 0) ) {
    
#ifdef DEBUG
    fprintf(stderr, "lines=%d, current=%d number=%d\n",\
        config.lines, current, number); 
#endif
  
    // Channels before the channel to be tuned
    for(int i=3; i>0; i--)
      if( (current > i) && (config.lines >= 2*i))
        osd->DrawText(d, margin+(3+LINEHEIGHT)*(-i+int(config.lines/2))+delta,\
            (Channels.GetByChannelID(preferchannels[current-i-1]))->Name(), clrWhite, clrBackground, font);

    // Channel to be tuned :)
    osd->DrawText(d, margin+(3+LINEHEIGHT)*(0+int(config.lines/2))+delta,\
        (Channels.GetByChannelID(preferchannels[current-1]))->Name(), clrBlack, clrCyan, font);

    // Channels after the channel to be tuned
    for(int i=0; i<3; i++)
      if(current < number-i)
        osd->DrawText(d, margin+(3+LINEHEIGHT)*(i+1+int(config.lines/2))+delta,\
            (Channels.GetByChannelID(preferchannels[current+i]))->Name(), clrWhite, clrBackground, font);
  }
}

void cPreferOsd::DrawSymbol(int x, int y, char *SymbolName[], tColor ColorFg)
{
  cBitmap bm(SymbolName);
  osd->DrawBitmap(x, y, bm, ColorFg, clrBackground);
}

void cPreferOsd::DrawVLine(int xstart, int ystart, int height, tColor color)
{
  for(int i=xstart; i<xstart+BORDERSIZE; i++)
    for(int j=ystart; j<height; j++)
      osd->DrawPixel(i, j, clrBlack);
}

void cPreferOsd::DrawHLine(int xstart, int ystart, int width, tColor color)
{
  for(int i=xstart; i<width; i++)
    for(int j=ystart; j<ystart+BORDERSIZE; j++)
      osd->DrawPixel(i, j, clrBlack);
}
