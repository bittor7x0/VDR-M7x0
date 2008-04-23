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
      fprintf(stderr, "fEPG: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
      return;
    }  
  
    scrOsd->SetAreas(&Area, 1);
    scrOsd->DrawRectangle(0, 0, width, height, clrBlack);
    scrOsd->Flush();
  }
  
  Draw();
}


//----------------------------------------------------------------------------

void cScreenMenu::Draw(void)
{
  if (!scrOsd) return;
  
  // Erase Old Lines
  scrOsd->DrawRectangle(x1p,y1p, x1p+lt,y2p, clrBlack);
  scrOsd->DrawRectangle(x1p,y1p, x2p,y1p+lt, clrBlack);
  scrOsd->DrawRectangle(x1p,y2p, x2p,y2p+lt, clrBlack);
  scrOsd->DrawRectangle(x2p,y1p, x2p+lt,y2p, clrBlack);
  
  // Draw Lines
  scrOsd->DrawRectangle(x1,y1, x1+lt,y2, clrWhite);
  scrOsd->DrawRectangle(x1,y1, x2,y1+lt, clrWhite);
  scrOsd->DrawRectangle(x1,y2, x2,y2+lt, clrWhite);
  scrOsd->DrawRectangle(x2,y1, x2+lt,y2, clrWhite);
  
  //XXX: One line leaves a trail (why?) [on FF cards]
  
  scrOsd->DrawText(0, 0, "Screen Setup", clrWhite, clrTransparent, font, width, height, taCenter);
  
  scrOsd->Flush();
}


//----------------------------------------------------------------------------

eOSState cScreenMenu::ProcessKey(eKeys Key)
{
  x1p = x1; x2p = x2; y1p = y1; y2p = y2;
  
  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) {
    switch (Key & ~k_Repeat) {
      case kUp:    if (y1 >= inc)   y1 -= inc; break;
      case kDown:  if (y1 < y2-inc) y1 += inc; break;
      case kLeft:  if (x1 >= inc)   x1 -= inc; break;
      case kRight: if (x1 < x2-inc) x1 += inc; break;
      case k2:     if (y2 > y1+inc) y2 -= inc; break;
      case k4:     if (x2 > x1+inc) x2 -= inc; break;
      case k6:     if (x2 <= width-inc)  x2 += inc; break;
      case k8:     if (y2 <= height-inc) y2 += inc; break;
      case kOk:    Store(); return osEnd;
      case kBack:  return osEnd;
      default: return state;
    }
    Draw();
    state = osContinue;
  }
  return state;  
}


//----------------------------------------------------------------------------

void cScreenMenu::Store()
{
  cPlugin* plugin = cPluginManager::GetPlugin("fepg");
  if (plugin) {
    plugin->SetupStore("sX1", config.sX1 = x1);
    plugin->SetupStore("sX2", config.sX2 = x2);
    plugin->SetupStore("sY1", config.sY1 = y1);
    plugin->SetupStore("sY2", config.sY2 = y2);
  }
}


//////////////////////////////////////////////////////////////////////////////

