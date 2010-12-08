#include "colorMenu.h"
#include "tools.h"


#define SCREEN_WIDTH 720
#define WIDTH        500
#define HEIGHT       160


//////////////////////////////////////////////////////////////////////////////


cColorMenu::cColorMenu(void)
{
  screenHeight = (config.TVMode == NTSC) ? 480 : 576;
  
  scrOsd = NULL;
  font = cFont::GetFont(fontOsd);

  sel = 0;
  colors[0] = config.clr1Red; 
  colors[1] = config.clr1Green; 
  colors[2] = config.clr1Blue;
  colors[3] = config.clr2Red; 
  colors[4] = config.clr2Green; 
  colors[5] = config.clr2Blue;
  
  color1 = toColor(colors[0], colors[1], colors[2], config.trans);
  color2 = toColor(colors[3], colors[4], colors[5], config.trans);
  
  requestColorMenu = false;
  inputNum = 0;
  inputTime = 0;
}


//----------------------------------------------------------------------------

cColorMenu::~cColorMenu()
{
  delete scrOsd;
  scrOsd = NULL;
}


//----------------------------------------------------------------------------

void cColorMenu::Show(void)
{
  int osdWidth = (WIDTH - (WIDTH & 3))+3;
  int sX = (SCREEN_WIDTH - osdWidth) / 2; 
  int sY = (screenHeight - HEIGHT) / 2;
  
  scrOsd = cOsdProvider::NewOsd(sX, sY);

  if (scrOsd) {   
    tArea Area = { 0, 0, osdWidth, HEIGHT,  2 };

    eOsdError e = scrOsd->CanHandleAreas(&Area, 1);  
    if (e != oeOk) {     
      fprintf(stderr, "[fEPG] %s (#%d) %s\n", tr("OSD Error"), e, OsdErrorToString(e));
      return;
    }
  
    scrOsd->SetAreas(&Area, 1);
    
    scrOsd->GetBitmap(0)->SetColor(0, clrBlack);
    scrOsd->GetBitmap(0)->SetColor(1, clrWhite);
    scrOsd->GetBitmap(0)->SetColor(2, color1);
    scrOsd->GetBitmap(0)->SetColor(3, color2); 
  }
  
  Draw();
}


//----------------------------------------------------------------------------

void cColorMenu::Draw(void)
{
  if (!scrOsd) return;
  
  color1 = toColor(colors[0], colors[1], colors[2], config.trans);
  color2 = toColor(colors[3], colors[4], colors[5], config.trans);
  scrOsd->GetBitmap(0)->SetColor(2, color1);
  scrOsd->GetBitmap(0)->SetColor(3, color2);
   
  scrOsd->DrawRectangle(0, 0, WIDTH, HEIGHT, clrBlack);
  
  scrOsd->DrawRectangle(0,0, WIDTH,lt, clrWhite);
  scrOsd->DrawRectangle(0,0, lt,HEIGHT, clrWhite);
  scrOsd->DrawRectangle(0,HEIGHT-lt, WIDTH,HEIGHT, clrWhite);
  scrOsd->DrawRectangle(WIDTH-lt,0, WIDTH,HEIGHT, clrWhite);
  
  int x = 20, y=10;
  scrOsd->DrawText(x, y, tr("Grid Color"), clrWhite, clrTransparent, font);
  scrOsd->DrawRectangle(x,y+30, x+100,y+130, color1);
    
  DrawColor(x+130, y+35,  "R", colors[0], 0);
  DrawColor(x+130, y+70,  "G", colors[1], 1);
  DrawColor(x+130, y+105, "B", colors[2], 2);
   
  scrOsd->DrawRectangle(249, 0, 249+lt, HEIGHT, clrWhite);
    
  x = 270, y=10; 
  scrOsd->DrawText(x, y, tr("Channels Color"), clrWhite, clrTransparent, font);
  scrOsd->DrawRectangle(x,y+30, x+100,y+130, color2);

  DrawColor(x+130, y+35,  "R", colors[3], 3);
  DrawColor(x+130, y+70,  "G", colors[4], 4);
  DrawColor(x+130, y+105, "B", colors[5], 5);
  
  scrOsd->Flush();
}


//----------------------------------------------------------------------------

eOSState cColorMenu::ProcessKey(eKeys Key)
{  
  int newInputNum = 0;
  eOSState state = cOsdObject::ProcessKey(Key);

  if (state == osUnknown) {
    state = osContinue;
    switch (Key & ~k_Repeat) {
      case kDown:  
        sel = (sel==5) ? 0 : sel+1; 
        inputNum = 0; 
        break;
      case kUp:    
        sel = (sel==0) ? 5 : sel-1; 
        inputNum = 0; 
        break;
      case kLeft:  
        colors[sel] = (colors[sel]==0) ? 255 : colors[sel]-1; 
        inputNum = 0; 
        break;
      case kRight: 
        colors[sel] = (colors[sel]==255) ? 0 : colors[sel]+1; 
        inputNum = 0; 
        break;
      case kOk:
        Store(); 
        state = osEnd; 
        break;
      case kBack:  
        state = osEnd; 
        break;
      case k0...k9:
        newInputNum = inputNum * 10 + Key - k0;
        if (newInputNum < 256) inputNum = newInputNum;
        inputTime = cTimeMs::Now();
        break;
      case kNone:
        if (inputNum && cTimeMs::Now() - inputTime > (uint) config.timeout) {
          if (inputNum < 256) colors[sel] = inputNum;
          inputNum = 0;
        }
        break;        
    }
    
    if (state != osEnd) Draw();
  }

  return state;  
}


//----------------------------------------------------------------------------

void cColorMenu::DrawColor(int x, int y, const char* s, int val, int i)
{
  char buf[16];
  
  if (i != sel) 
  {
    snprintf(buf, 16, "%s: %d%c", s, val, '\0');
    scrOsd->DrawText(x, y, buf, clrWhite, clrTransparent, font);
  }
  else 
  {
    if (inputNum)
      snprintf(buf, 16, "%s: %d_%c", s, inputNum, '\0');
    else  
      snprintf(buf, 16, "%s: %d%c", s, val, '\0'); 
       
    scrOsd->DrawText(x, y, buf, clrTransparent, clrWhite, font);
  }
}


//----------------------------------------------------------------------------

void cColorMenu::Store()
{
  cPlugin* plugin = cPluginManager::GetPlugin("fepg");
  if (plugin) {
    plugin->SetupStore("clr1Red",   config.clr1Red   = colors[0]);
    plugin->SetupStore("clr1Green", config.clr1Green = colors[1]);
    plugin->SetupStore("clr1Blue",  config.clr1Blue  = colors[2]);
    plugin->SetupStore("clr2Red",   config.clr2Red   = colors[3]);
    plugin->SetupStore("clr2Green", config.clr2Green = colors[4]);
    plugin->SetupStore("clr2Blue",  config.clr2Blue  = colors[5]);
  }
}


//////////////////////////////////////////////////////////////////////////////

