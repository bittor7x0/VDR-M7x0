#include <vdr/osd.h>
#include <vdr/plugin.h>
#include <vdr/device.h>
#include <vdr/remote.h>


#include "radioInfoOsd.h"
#include "radioinfo.h"
#include "sRadioInfo.h"
#include "tools.h"
#include "bg.h"
#include "config.h"


#define HEIGHT 300


cPluginRadioinfo* currentRadioInfo;



cRadioInfoOsd::cRadioInfoOsd(cRadioInfoOsd** Rid, sRadioInfo* Ri)
{ 
  rOsd = NULL;
  radioInfo = Ri;
  radioInfoOsd = Rid; //Used for clean-up
  

  header = new cBitmap(radioinfobg_xpm); 
  canvas = new cBitmap(520, HEIGHT, 2);

  
  canvas->SetColor(0, clrTransparent);
  canvas->SetColor(1, (tColor) 0xFF6F649A);
  canvas->SetColor(2, (tColor) 0xFF625584);
  canvas->SetColor(3, clrWhite);
     
  header->SetColor(3, clrWhite);
  header->SetColor(1, (tColor) 0xFF6F649A);
  header->SetColor(0, clrTransparent);
  header->SetColor(2, (tColor) 0xFF625584);
  
  


  offsetY = 50;
  offsetX = 100;
  
  dirY = 1;
  dirX = 0;
  
  name = NULL;
  getEventName(); 

}


cRadioInfoOsd::~cRadioInfoOsd()
{
  if (rOsd) delete rOsd;
  if (header) delete header;
  if (canvas) delete canvas;
  if (name) free(name);
  
  (*radioInfoOsd) = NULL; // If VDR decides to kill the osd, the plugin's pointer
                          // to the osd needs to be set to NULL
}


void cRadioInfoOsd::Show(void)
{ 
  rOsd = cOsdProvider::NewOsd(0, 0);
  tArea Area = { 0, 0, 719, 479,  2 };
  
  
  if (rOsd)
  {
    if (rOsd->CanHandleAreas(&Area, 1) == oeOk)
      rOsd->SetAreas(&Area, 1);
    else
      fprintf(stderr,"RadioInfo: OSD Error %d\n", rOsd->CanHandleAreas(&Area, 1));
  }
  else
    fprintf(stderr,"RadioInfo: Could not create OSD\n");
 
     
  draw();
}


eOSState cRadioInfoOsd::ProcessKey(eKeys Key) { return ProcessKey(Key, true); }


eOSState cRadioInfoOsd::ProcessKey(eKeys Key, bool wantsOsd)
{
  if ((Key & ~k_Repeat) == kNone)
  {
    if (time(NULL) - lastUpdate >= config.DELAY)
    {
      if (offsetY + HEIGHT == 479)  dirY = -1;
      if (offsetY == 50)  dirY = 1;
      offsetY += dirY;
      draw();
      lastUpdate = time(NULL);
    }
    return osContinue;
  }
  
  //if (Key == kBlue) cDevice::PrimaryDevice()->GrabImageFile("radioInfoScreen.jpg");

  // Let VDR handle input key normally, then return.
  currentRadioInfo->wantsOsd = wantsOsd;
  cRemote::Put(Key);
  return osEnd;
}



void cRadioInfoOsd::draw(void)
{
  if (!rOsd) return;
  
  rOsd->DrawRectangle(0, 0, 719, 479, clrTransparent);
  canvas->DrawRectangle(0, 0, 520, HEIGHT, clrTransparent);
  /*
  
  canvas->DrawRectangle(0, 0, 520, 50, clrWhite);
  canvas->DrawRectangle(75, 2, 518, 48, clrTransparent);

  canvas->DrawBitmap(10, 0, (*notes));
  
  canvas->DrawRectangle(70, 0, 100, 50, clrWhite);
  canvas->DrawRectangle(490, 0, 520, 50, clrWhite);
  canvas->DrawEllipse(71, 2, 100, 48, clrTransparent, 7);
  canvas->DrawEllipse(490, 2, 518, 48, clrTransparent, 5);
  */
  canvas->DrawBitmap(0, 0, (*header));
  
  //canvas->DrawText(70, 0, name, clrWhite, clrTransparent, cFont::GetFont(fontOsd), 450, 50, taCenter);
  canvas->DrawText(52, 0, name, clrWhite, clrTransparent, cFont::GetFont(fontOsd), 447, 51, taCenter);
  
  if (radioInfo && !radioInfo->updating)
  {
    canvas->DrawText(70, 80,  radioInfo->title,  clrWhite, clrTransparent, cFont::GetFont(fontOsd));
    canvas->DrawText(70, 110, radioInfo->artist, clrWhite, clrTransparent, cFont::GetFont(fontOsd));
    canvas->DrawText(70, 140, radioInfo->extra1, clrWhite, clrTransparent, cFont::GetFont(fontOsd));
    canvas->DrawText(70, 170, radioInfo->extra2, clrWhite, clrTransparent, cFont::GetFont(fontOsd));
    canvas->DrawText(70, 200, radioInfo->extra3, clrWhite, clrTransparent, cFont::GetFont(fontOsd));
  }
  
  
  
  rOsd->DrawBitmap(offsetX, offsetY, (*canvas));
  rOsd->Flush();
}


void cRadioInfoOsd::getEventName(void)
{
  cChannel* c = Channels.GetByNumber(cDevice::CurrentChannel());
  cSchedulesLock SchedulesLock;
  const cSchedules* Schedules = cSchedules::Schedules(SchedulesLock);
  const cSchedule* schedule = Schedules->GetSchedule(c->GetChannelID());
  
  const cEvent* event = NULL;
    
  if (schedule)
    event = schedule->GetPresentEvent();
    
  if (name) free(name);
  name = event ? strdup(event->Title()) : strdup("No Title");

}



