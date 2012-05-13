#include "screenMenu.h"
#include "tools.h"


//////////////////////////////////////////////////////////////////////////////


cScreenMenu::cScreenMenu(void)
{
  width = 719;
  height = (config.TVMode == NTSC) ? 479 : 575;
  
  scrOsd = NULL;
  font = cFont::GetFont(fontOsd);
  
  x1 = x1p = config.sX1;
  y1 = y1p = config.sY1; 
  x2 = x2p = config.sX2;
  y2 = y2p = config.sY2;
  chanWidth  = config.chanWidth;
  timeHeight = config.timeHeight;
  
  requestScreenMenu = false;
}


//----------------------------------------------------------------------------

cScreenMenu::~cScreenMenu()
{
  delete scrOsd;
}


//----------------------------------------------------------------------------

void cScreenMenu::Show(void)
{
  scrOsd = cOsdProvider::NewOsd(0, 0);

  if (scrOsd) {
    tArea Area = { 0, 0, width, height,  1 };

    eOsdError e = scrOsd->CanHandleAreas(&Area, 1);  
    if (e != oeOk) {     
      fprintf(stderr, "[fEPG] %s (#%d) %s\n", tr("OSD Error"), e, OsdErrorToString(e));
      return;
    }  
  
    scrOsd->SetAreas(&Area, 1);
  }
  
  Draw();
}


//----------------------------------------------------------------------------

void cScreenMenu::Draw(void)
{
  if (!scrOsd) return;
  
  scrOsd->DrawRectangle(0, 0, width, height, clrBlack);
  
  // Draw Lines
  scrOsd->DrawRectangle(x1,y1, x1+lt,y2, clrWhite);
  scrOsd->DrawRectangle(x1,y1, x2,y1+lt, clrWhite);
  scrOsd->DrawRectangle(x1,y2, x2,y2+lt, clrWhite);
  scrOsd->DrawRectangle(x2,y1, x2+lt,y2, clrWhite);
  
  scrOsd->DrawRectangle(x1+chanWidth, y1, x1+chanWidth+lt, y2, clrWhite);
  scrOsd->DrawRectangle(x1, y1+timeHeight, x2, y1+timeHeight+lt, clrWhite);
    
  scrOsd->DrawText(0, 0, tr("Display Setup"), clrWhite, clrTransparent, font, width, height, taCenter);
  
  scrOsd->Flush();
}


//----------------------------------------------------------------------------

eOSState cScreenMenu::ProcessKey(eKeys Key)
{
  x1p = x1; x2p = x2; y1p = y1; y2p = y2;
  
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) 
  {
    state = osContinue;
    switch (Key & ~k_Repeat) {
      case kUp:     if (y1 >= inc)   y1 -= inc; break;
      case kDown:   if (y1 < y2-inc) y1 += inc; break;
      case kLeft:   if (x1 >= inc)   x1 -= inc; break;
      case kRight:  if (x1 < x2-inc) x1 += inc; break;
      
      case k2:      if (y2 > y1+inc) y2 -= inc; break;
      case k4:      if (x2 > x1+inc) x2 -= inc; break;
      case k6:      if (x2 <= width-inc)  x2 += inc; break;
      case k8:      if (y2 <= height-inc) y2 += inc; break;

      case kGreen:  if (x1+chanWidth+inc < x2)  chanWidth += inc; break; 
      case kRed:    if (chanWidth >= inc)       chanWidth -= inc; break;
      case kYellow: if (timeHeight >= inc)      timeHeight -= inc; break; 
      case kBlue:   if (y1+timeHeight+inc < y2) timeHeight += inc; break;

      case kOk:    Store(); state = osEnd; break;
      case kBack:  state = osEnd; break;
    }
    Draw();
  }
  return state;  
}


//----------------------------------------------------------------------------

void cScreenMenu::Store()
{
  cPlugin* plugin = cPluginManager::GetPlugin("fepg");
  if (plugin) 
  {
    plugin->SetupStore("sX1", config.sX1 = x1);
    plugin->SetupStore("sX2", config.sX2 = x2);
    plugin->SetupStore("sY1", config.sY1 = y1);
    plugin->SetupStore("sY2", config.sY2 = y2);
    plugin->SetupStore("chanWidth",  config.chanWidth  = chanWidth);
    plugin->SetupStore("timeHeight", config.timeHeight = timeHeight);
  }
}


//////////////////////////////////////////////////////////////////////////////

