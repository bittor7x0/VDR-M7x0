#include <vdr/plugin.h>
#include <vdr/device.h>
#include <vdr/remote.h>


#include "radioInfoOsd.h"
#include "radioinfo.h"
#include "tools.h"
#include "bg.h"
#include "config.h"


///////////////////////////////////////////////////////////////////////////////


#define HEIGHT 300


///////////////////////////////////////////////////////////////////////////////


cRadioInfoOsd::cRadioInfoOsd(cRadioInfoData* Ri)
             : cThread("RadioInfo OSD"), header(radioinfobg_xpm), canvas(520, HEIGHT, 2)
{ 
  DEBUG_MSG("Creating OSD");
  rOsd = NULL;
  radioInfoData = Ri;

  canvas.SetColor(0, clrTransparent);
  canvas.SetColor(1, (tColor) 0xFF6F649A);
  canvas.SetColor(2, (tColor) 0xFF625584);
  canvas.SetColor(3, clrWhite);
     
  header.SetColor(0, clrTransparent);
  header.SetColor(1, (tColor) 0xFF6F649A);
  header.SetColor(2, (tColor) 0xFF625584);
  header.SetColor(3, clrWhite);
 
  offsetY = 50;
  offsetX = 100;
  
  dirY = 1;
  
  eventName = NULL;
  GetEventName(); //TODO: Event name should be updated...

  Show();
  Start();
}


//-----------------------------------------------------------------------------

cRadioInfoOsd::~cRadioInfoOsd()
{
  DEBUG_MSG("Killing OSD");
  Cancel();
  
  delete rOsd;
}


//-----------------------------------------------------------------------------

void cRadioInfoOsd::Show(void)
{ 
  rOsd = cOsdProvider::NewOsd(0, 0, 15);
  tArea Area = { 0, 0, 719, 479,  2 };
  
  if (rOsd)
  {
    eOsdError e = rOsd->CanHandleAreas(&Area, 1);  
    if (e != oeOk) {
      fprintf(stderr, "RadioInfo: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
      return;
    }
    
    rOsd->SetAreas(&Area, 1);
    
    Draw();    
  }
  else
    fprintf(stderr,"RadioInfo: Could not create OSD\n");
}


//-----------------------------------------------------------------------------

void cRadioInfoOsd::Action(void)
{
  while( Running() )
  {
    cCondWait::SleepMs(1000*config.osdDelay);  
    if (offsetY + HEIGHT == 479)  dirY = -1;
    if (offsetY == 50)  dirY = 1;
    offsetY += dirY;     
    Draw();
  }
}


//-----------------------------------------------------------------------------

void cRadioInfoOsd::Draw(void)
{
  if (!rOsd) return;
  
  rOsd->DrawRectangle(0, 0, 719, 479, clrTransparent);
  canvas.DrawRectangle(0, 0, 520, HEIGHT, clrTransparent);

  canvas.DrawBitmap(0, 0, header);
  
  const cFont* font = cFont::GetFont(fontOsd);
  
  canvas.DrawText(52, 0, eventName, clrWhite, clrTransparent, font, 447, 51, taCenter);
  
  radioInfoData->Lock();
  
  canvas.DrawText(70, 80,  radioInfoData->title,  clrWhite, clrTransparent, font);
  canvas.DrawText(70, 110, radioInfoData->artist, clrWhite, clrTransparent, font);
  canvas.DrawText(70, 140, radioInfoData->extra1, clrWhite, clrTransparent, font);
  canvas.DrawText(70, 170, radioInfoData->extra2, clrWhite, clrTransparent, font);
  canvas.DrawText(70, 200, radioInfoData->extra3, clrWhite, clrTransparent, font);
  
  radioInfoData->Unlock();

  rOsd->DrawBitmap(offsetX, offsetY, canvas);
  rOsd->Flush();
}


//-----------------------------------------------------------------------------

void cRadioInfoOsd::GetEventName(void)
{
  cChannel* c = Channels.GetByNumber(cDevice::CurrentChannel());
  cSchedulesLock SchedulesLock;
  const cSchedules* Schedules = cSchedules::Schedules(SchedulesLock);
  const cSchedule*  schedule  = Schedules->GetSchedule(c->GetChannelID());
  
  const cEvent* event = NULL;
    
  if (schedule)
    event = schedule->GetPresentEvent();
    
  eventName = event ? event->Title() : "No Title";
}


///////////////////////////////////////////////////////////////////////////////

