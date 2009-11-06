//#include <vdr/plugin.h>
#include <vdr/dvbdevice.h>
#include <vdr/interface.h>
//#include <vdr/osd.h>
#include <vdr/menu.h>
#include "i18n.h"
#include "screenmenu.h"

int DEBUG_OSD = 1;  // 0, 1, 2, 3

//----------------------------------------------------------------------------

const char* OsdErrorToString(eOsdError err)
{
  const char* e = "Unknown";
  switch (err)
  {
    case oeOk:              e = "Ok"; break;
    case oeTooManyAreas:    e = "Too Many Areas"; break;
    case oeTooManyColors:   e = "Too Many Colors"; break;
    case oeBppNotSupported: e = "Bpp Not Supported"; break;
    case oeAreasOverlap:    e = "Areas Overlap"; break;
    case oeWrongAlignment:  e = "Wrong Alignment"; break;
    case oeOutOfMemory:     e = "Out Of Memory"; break;
    case oeWrongAreaSize:   e = "Wrong Area Size"; break;
    case oeUnknown: break;
  }
  return e;
}


//----------------------------------------------------------------------------

cScreenMenu::cScreenMenu()
{
  osd = NULL;
  resetOsd = false;
  stateOsd = 0;
  toggleOsd = 0;

  offset = 0;

  lt = config.LineTn;
  inc = config.MoveInc;

  // Size & Position
  width = 720;
  if (config.VideoFormat == 0)
     height = (eVideoSystem() == vsNTSC) ? 480 : 576;
  else
     height = (config.VideoFormat == 2) ? 480 : 576;

/*
  if (Setup.OSDWidth + 2 * Setup.OSDLeft > width) {
     Setup.OSDLeft = 20;
     Setup.OSDWidth = width - 40;
     }

  if (Setup.OSDHeight + 2 * Setup.OSDTop > height) {
     Setup.OSDTop = 20;
     Setup.OSDHeight = height - 40;
     }
*/

  x0 = x0p = Setup.OSDLeft;
  y0 = y0p = Setup.OSDTop;
  x1 = x1p = Setup.OSDWidth + Setup.OSDLeft;
  y1 = y1p = Setup.OSDHeight + Setup.OSDTop;

  // Font
  if (Setup.UseSmallFont == 2)
     font = cFont::GetFont(fontSml);
  else
     font = cFont::GetFont(fontOsd);

  // background color
  if      (config.BgColor ==  0) myBgColor =  myTransparent;
  else if (config.BgColor ==  1) myBgColor =  myTransparent50;
  else if (config.BgColor ==  2) myBgColor =  myBlack;
  else if (config.BgColor ==  3) myBgColor =  myRed;
  else if (config.BgColor ==  4) myBgColor =  myGreen;
  else if (config.BgColor ==  5) myBgColor =  myYellow;
  else if (config.BgColor ==  6) myBgColor =  myMagenta;
  else if (config.BgColor ==  7) myBgColor =  myBlue;
  else if (config.BgColor ==  8) myBgColor =  myCyan;
  else if (config.BgColor ==  9) myBgColor =  myWhite;
  else if (config.BgColor == 10) myBgColor =  myGray;
  else                           myBgColor =  myBlack;

  // foreground color
  if      (config.FgColor ==  0) myFgColor =  myTransparent;
  else if (config.FgColor ==  1) myFgColor =  myTransparent50;
  else if (config.FgColor ==  2) myFgColor =  myBlack;
  else if (config.FgColor ==  3) myFgColor =  myRed;
  else if (config.FgColor ==  4) myFgColor =  myGreen;
  else if (config.FgColor ==  5) myFgColor =  myYellow;
  else if (config.FgColor ==  6) myFgColor =  myMagenta;
  else if (config.FgColor ==  7) myFgColor =  myBlue;
  else if (config.FgColor ==  8) myFgColor =  myCyan;
  else if (config.FgColor ==  9) myFgColor =  myWhite;
  else if (config.FgColor == 10) myFgColor =  myGray;
  else                           myFgColor =  myWhite;

  // Mute
  if (config.Mute) {
     if (!cDevice::PrimaryDevice()->IsMute()) {
        wasMuted = false;
        cDevice::PrimaryDevice()->ToggleMute();
        }
     else
        wasMuted = true;
     }
}


//----------------------------------------------------------------------------

cScreenMenu::~cScreenMenu()
{
  if (osd)
     delete osd;
  if (config.Mute && !wasMuted && cDevice::PrimaryDevice()->IsMute())
     cDevice::PrimaryDevice()->ToggleMute();
}


//----------------------------------------------------------------------------

void cScreenMenu::Show()
{
  if (osd) delete osd;
  osd = NULL;
  osd = cOsdProvider::NewOsd(0, 0);
  if (!osd) return;
  toggleOsd = 0;

  tArea Areas[] = { { 0, 0, width - 1, height - 1, 8 } };
  if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
     osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     if (DEBUG_OSD > 0) dsyslog("osdadjust: using 8 bit color depth \n");
     stateOsd = 8;
     }
  else {
     tArea Areas[] = { { 0, 0, width - 1, height - 1, 4 } };
     eOsdError e = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
     if (e == oeOk) {
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (DEBUG_OSD > 0) dsyslog("osdadjust: using 4 bit color depth \n");
        stateOsd = 4;
        }
     else {
        if (DEBUG_OSD > 2) dsyslog("osdadjust: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
        tArea Areas[] = { { 0, 0, width - 1, height - 1, 2 } };
        eOsdError e = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (e == oeOk) {
           osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
           if (DEBUG_OSD > 0) dsyslog("osdadjust: using 2 bit color depth \n");
           stateOsd = 2;
           }
        else {
           if (DEBUG_OSD > 2) dsyslog("osdadjust: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
           tArea Areas[] = { { 0, 0, width - 1, height - 1, 1 } };
           eOsdError e = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
           if (e != oeOk) {
              esyslog("osdadjust: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
              return;
              }
           osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
           if (DEBUG_OSD > 0) dsyslog("osdadjust: using 1 bit color depth \n");
           stateOsd = 1;
           }
        }
     }
  osd->DrawRectangle(0, 0, width - 1, height - 1, myBgColor);

  if (!resetOsd)
     Draw();
  else {
     osd->Flush();
     resetOsd = false;
     }
}


//----------------------------------------------------------------------------

void cScreenMenu::Osdtest256()
{
  resetOsd = true;
  toggleOsd = 4;

  if (stateOsd != 8) {
     if (osd) delete osd;
     osd = NULL;
     osd = cOsdProvider::NewOsd(0, 0);
     if (!osd) return;

     tArea Area = { 0, 0, width - 1, height - 1, 8 };
     if (osd->CanHandleAreas(&Area, 1) == oeOk) {
        osd->SetAreas(&Area, 1);
        if (DEBUG_OSD > 0) dsyslog("osdadjust: using 8 bit color depth \n");
        stateOsd = 8;
        }
     else {
        DrawColor();
//        osd->DrawText(0, 0, "Can not handle 8 bit color depth!", myFgColor, myBgColor, font, width, height, taCenter);
//        osd->Flush();
        if (DEBUG_OSD > 0) dsyslog("osdadjust: can not handle 8 bit color depth \n");
        return;
        }
     }

  if (!osd) return;

  int x = 60;
  int y = 40;

  osd->DrawRectangle(0, 0, width - 1, height - 1, myTransparent50);

  for (int i = 0; i < 60; i++)
      osd->DrawRectangle(10*i+x,  0+y,10*(i+1)+x, 99+y, 0xff000000+0x040404*(60-i));
  for (int i = 0; i < 60; i++)
      osd->DrawRectangle(10*i+x,100+y,10*(i+1)+x,199+y, 0xff000000+((15+i*4)<<16));
  for (int i = 0; i < 60; i++)
      osd->DrawRectangle(10*i+x,200+y,10*(i+1)+x,299+y, 0xff000000+((15+(60-i)*4)<<8));
  for (int i = 0; i < 60; i++)
      osd->DrawRectangle(10*i+x,300+y,10*(i+1)+x,399+y, 0xff000000+(15+i*4));
  for (int i = 0; i < 60; i++)
      osd->DrawRectangle(10*i+x,400+y,10*(i+1)+x,499+y, 0xff000000+0x040404*(60-i));

  osd->Flush();
}

//----------------------------------------------------------------------------

void cScreenMenu::DrawColor()
{
  if (!osd) return;
  if (resetOsd) Show();

  toggleOsd = 0;
  osd->DrawRectangle(0, 0, width - 1, height - 1, myBgColor);
  if (config.ShowGrid && stateOsd == 6) DrawCycle(myFgColor, myBgColor);
  if (config.ShowGrid) DrawGrid(myFgColor, myBgColor);
  osd->Flush();
}

//----------------------------------------------------------------------------

void cScreenMenu::DrawCycle(int tmpFgColor, int tmpBgColor)
{
  if (!osd) return;

  int d = 400;
  int xm = width / 2 - d / 2;
  int ym = height / 2 - d / 2;
  offset = ((768 - 720) / 2) / 2;  // 4:3

  if (config.AspectRatio == 1)
     offset = ((768 - 720) / 2 + (1024 - 768) / 2) / 2;   // 16:9

  if (config.AspectRatio == 2)
     offset = 0;    // 1:1

  osd->DrawEllipse(xm, ym - offset, xm + d, ym + d + offset, tmpFgColor, 0);
  osd->DrawEllipse(xm + 1, ym + 1 - offset, xm - 1 + d, ym - 1 + d + offset, tmpBgColor, 0);
//  osd->Flush();
}

//----------------------------------------------------------------------------

void cScreenMenu::DrawGrid(int tmpFgColor, int tmpBgColor)
{
  if (!osd) return;

  if (stateOsd != 6) DrawCycle(tmpFgColor, tmpBgColor);

  int x = 38;
  int y = 40;

  if (config.AspectRatio == 1) x = 32;    // 16:9
  if (config.AspectRatio == 2) x = 40;    // 1:1

  offset = 12;

  for (int i = 1; i * y < height; i++)
      osd->DrawRectangle(0,  i * y - offset, width - 1,  i * y - offset, tmpFgColor);

  offset = 1;    // 4:3
  if (config.AspectRatio == 1) offset = 8;    // 16:9
  if (config.AspectRatio == 2) offset = 0;    // 1:1

  for (int i = 1; i * x < width; i++)
      osd->DrawRectangle( i * x - offset, 0,  i * x - offset, height - 1, tmpFgColor);

//  osd->Flush();
}

//----------------------------------------------------------------------------

void cScreenMenu::Bluge()
{
  toggleOsd = 3;
  barHeight = height / 6;
  barWidth = width / 4;

  if (stateOsd < 3 || stateOsd == 6) {
     if (osd) delete osd;
     osd = NULL;
     osd = cOsdProvider::NewOsd(0, 0);
     if (!osd) return;
     resetOsd = true;

     tArea Areas[] = { { 0, 0, width - 1, height - 1, 8 } };
     if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (DEBUG_OSD > 0) dsyslog("osdadjust: using 8 bit color depth \n");
        stateOsd = 8;
        }
     else {
        tArea Areas[] = { { 0, 0, width - 1, height - 1, 4 } };
        eOsdError e = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (e == oeOk) {
           osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
           if (DEBUG_OSD > 0) dsyslog("osdadjust: using 4 bit color depth \n");
           stateOsd = 4;
           }
        else {
           if (DEBUG_OSD > 0) dsyslog("osdadjust: splitting areas\n");
           tArea Areas[] = { { 0, 0,             width - 1, 1 * barHeight - 1, 1 },
                             { 0, 1 * barHeight, width - 1, 2 * barHeight - 1, 2 },
                             { 0, 2 * barHeight, width - 1, 3 * barHeight - 1, 2 },
                             { 0, 3 * barHeight, width - 1, 4 * barHeight - 1, 2 },
                             { 0, 4 * barHeight, width - 1, 5 * barHeight - 1, 2 },
                             { 0, 5 * barHeight, width - 1,        height - 1, 1 } };
           eOsdError e = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
           if (e != oeOk) {
              esyslog("osdadjust: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
              return;
              }
           else {
              osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
              stateOsd = 3;
              }
           }
        }
     }
  osd->DrawRectangle(0, 0, width - 1, height - 1, myBlack);

  osd->DrawRectangle(barWidth,      barHeight, barWidth + 20, 5 * barHeight - 1, myGray20);
  osd->DrawRectangle(barWidth + 41, barHeight, barWidth + 60, 5 * barHeight - 1, myGray10);

//  osd->DrawRectangle(2 * barWidth, 0,             3 * barWidth - 1, 1 * barHeight - 1, myBlack);
  osd->DrawRectangle(2 * barWidth,     barHeight, 3 * barWidth - 1, 2 * barHeight - 1, myWhite);
  osd->DrawRectangle(2 * barWidth, 2 * barHeight, 3 * barWidth - 1, 3 * barHeight - 1, myGray75);
  osd->DrawRectangle(2 * barWidth, 3 * barHeight, 3 * barWidth - 1, 4 * barHeight - 1, myGray50);
  osd->DrawRectangle(2 * barWidth, 4 * barHeight, 3 * barWidth - 1, 5 * barHeight - 1, myGray25);
//  osd->DrawRectangle(2 * barWidth, 5 * barHeight,        width - 1,        height - 1, myBlack);

  osd->Flush();
}

//----------------------------------------------------------------------------

void cScreenMenu::ColorBars()
{
  toggleOsd = 1;
  barWidth = width / 10;
  barHeight = 120;
  int cinebar = (height - barHeight) / 2;

  if (stateOsd == 8) {
     barHeight = 240;
     cinebar = (height - barHeight) / 2;
     }

  if (stateOsd < 4) {
     if (osd) delete osd;
     osd = NULL;
     osd = cOsdProvider::NewOsd(0, 0);
     if (!osd) return;
     resetOsd = true;
     tArea Areas[] = { { 0, 0, width - 1, height - 1, 8 } };
     if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (DEBUG_OSD > 0) dsyslog("osdadjust: using 8 bit color depth \n");
        stateOsd = 8;
        barHeight = 240;
        cinebar = (height - barHeight) / 2;
        }
     else {
        tArea Areas[] = { { 0, 0, width - 1, height - 1, 4 } };
        eOsdError e = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (e == oeOk) {
           osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
           if (DEBUG_OSD > 0) dsyslog("osdadjust: using 4 bit color depth \n");
           stateOsd = 4;
           }
        else {
           if (DEBUG_OSD > 0) dsyslog("osdadjust: splitting areas\n");
           tArea Areas[] = { { 0, 0, width - 1, cinebar - 1, 1 },
                             { 0, cinebar, width - 1, cinebar + barHeight, 4 },
                             { 0, cinebar + barHeight + 1, width - 1, height - 1, 1 } };
           eOsdError e = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
           if (e != oeOk) {
              esyslog("osdadjust: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
              return;
              }
           else {
              osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
              stateOsd = 6;
              }
           }
        }
     }

  osd->DrawRectangle(0, 0, width - 1, height - 1, myGray);
  if (config.ShowGrid)
     DrawGrid(myWhite, myGray);

  offset = barWidth;
//  osd->DrawRectangle(0,                     cinebar, offset - 1,                cinebar + barHeight, myBgColor);
  osd->DrawRectangle(offset,                cinebar, offset +     barWidth - 1, cinebar + barHeight, myWhite);
  osd->DrawRectangle(offset + 1 * barWidth, cinebar, offset + 2 * barWidth - 1, cinebar + barHeight, myYellow);
  osd->DrawRectangle(offset + 2 * barWidth, cinebar, offset + 3 * barWidth - 1, cinebar + barHeight, myCyan);
  osd->DrawRectangle(offset + 3 * barWidth, cinebar, offset + 4 * barWidth - 1, cinebar + barHeight, myGreen);
  osd->DrawRectangle(offset + 4 * barWidth, cinebar, offset + 5 * barWidth - 1, cinebar + barHeight, myMagenta);
  osd->DrawRectangle(offset + 5 * barWidth, cinebar, offset + 6 * barWidth - 1, cinebar + barHeight, myRed);
  osd->DrawRectangle(offset + 6 * barWidth, cinebar, offset + 7 * barWidth - 1, cinebar + barHeight, myBlue);
  osd->DrawRectangle(offset + 7 * barWidth, cinebar, offset + 8 * barWidth - 1, cinebar + barHeight, myBlack);
//  osd->DrawRectangle(offset + 8 * barWidth, cinebar, width - 1,                 cinebar + barHeight, myBgColor);
  osd->Flush();
}

//----------------------------------------------------------------------------

void cScreenMenu::GrayBars()
{
  toggleOsd = 2;
  barWidth = width / 12;
  barHeight = 120;
  int cinebar = (height - barHeight) / 2;

  if (stateOsd == 8) {
     barHeight = 240;
     cinebar = (height - barHeight) / 2;
     }

  if (stateOsd < 4) {
     if (osd) delete osd;
     osd = NULL;
     osd = cOsdProvider::NewOsd(0, 0);
     if (!osd) return;
     resetOsd = true;
     tArea Areas[] = { { 0, 0, width - 1, height - 1, 8 } };
     if (osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
        osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (DEBUG_OSD > 0) dsyslog("osdadjust: using 8 bit color depth \n");
        stateOsd = 8;
        barHeight = 240;
        cinebar = (height - barHeight) / 2;
        }
     else {
        tArea Areas[] = { { 0, 0, width - 1, height - 1, 4 } };
        eOsdError e = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
        if (e == oeOk) {
           osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
           if (DEBUG_OSD > 0) dsyslog("osdadjust: using 4 bit color depth \n");
           stateOsd = 4;
           }
        else {
           if (DEBUG_OSD > 0) dsyslog("osdadjust: splitting areas\n");
           tArea Areas[] = { { 0, 0, width - 1, cinebar - 1, 1 },
                             { 0, cinebar, width - 1, cinebar + barHeight, 4 },
                             { 0, cinebar + barHeight + 1, width - 1, height - 1, 1 } };
           eOsdError e = osd->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea));
           if (e != oeOk) {
              esyslog("osdadjust: OSD Error (#%d) %s\n", e, OsdErrorToString(e));
              return;
              }
           else {
              osd->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
              stateOsd = 6;
              }
           }
        }
     }

  osd->DrawRectangle(0, 0, width - 1, height - 1, myBlack);

  DrawGrid(myWhite, myBlack);
  if (!config.ShowGrid) // workaround for ff-cards
     osd->DrawRectangle(0, 0, width - 1, height - 1, myBlack);

  offset = barWidth;
//  osd->DrawRectangle(0,                     cinebar, offset - 1,                cinebar + barHeight, myBlack);
  osd->DrawRectangle(offset,                cinebar, offset +     barWidth - 1, cinebar + barHeight, myWhite);
  osd->DrawRectangle(offset + 1 * barWidth, cinebar, offset + 2 * barWidth - 1, cinebar + barHeight, myGray90);
  osd->DrawRectangle(offset + 2 * barWidth, cinebar, offset + 3 * barWidth - 1, cinebar + barHeight, myGray80);
  osd->DrawRectangle(offset + 3 * barWidth, cinebar, offset + 4 * barWidth - 1, cinebar + barHeight, myGray70);
  osd->DrawRectangle(offset + 4 * barWidth, cinebar, offset + 5 * barWidth - 1, cinebar + barHeight, myGray60);
  osd->DrawRectangle(offset + 5 * barWidth, cinebar, offset + 6 * barWidth - 1, cinebar + barHeight, myGray50);
  osd->DrawRectangle(offset + 6 * barWidth, cinebar, offset + 7 * barWidth - 1, cinebar + barHeight, myGray40);
  osd->DrawRectangle(offset + 7 * barWidth, cinebar, offset + 8 * barWidth - 1, cinebar + barHeight, myGray30);
  osd->DrawRectangle(offset + 8 * barWidth, cinebar, offset + 9 * barWidth - 1, cinebar + barHeight, myGray20);
  osd->DrawRectangle(offset + 9 * barWidth, cinebar, offset + 10* barWidth - 1, cinebar + barHeight, myGray10);
//  osd->DrawRectangle(offset + 10 * barWidth, cinebar, width - 1,                 cinebar + barHeight, myBlack);
  osd->Flush();
}

//----------------------------------------------------------------------------

void cScreenMenu::Draw()
{
  if (stateOsd == 0 || stateOsd == 6 || resetOsd) Show();
  if (!osd) return;

  toggleOsd = 0;
  char buffer[32];
  int lineHeight = font->Height();
//  int currentRow = height / 2;    // horizont
  int myTab = 6 * font->Height();   // workaround

/* old style
  // Erase old lines
  osd->DrawRectangle(x0p,y0p, x0p+lt,y1p, myBgColor);
  osd->DrawRectangle(x0p,y0p, x1p,y0p+lt, myBgColor);
  osd->DrawRectangle(x0p,y1p-lt, x1p,y1p, myBgColor);
  osd->DrawRectangle(x1p-lt,y0p, x1p,y1p, myBgColor);

  // Erase old text
  if (config.HideHelpText == false) {
     osd->DrawText(x0p + 50, y1p - 50 - 3*lineHeight, tr("Press 1-9 to move,"), myBgColor, myTransparent, font);
     osd->DrawText(x0p + 50, y1p - 50 - 2*lineHeight, tr("Up/Down & Left/Right to resize,"), myBgColor, myTransparent, font);
     osd->DrawText(x0p + 50, y1p - 50 - 1*lineHeight, tr("Ok to save, Back to cancel."), myBgColor, myTransparent, font);
     }

  // Erase old info
  snprintf(buffer, sizeof(buffer), "%s: %d  %s: %d", trVDR("Setup.OSD$Top"), y0p, tr("Bottom"), height - y1p);
  osd->DrawText(x0p + 50, y0p + 50, buffer, myBgColor, myTransparent, font);

  snprintf(buffer, sizeof(buffer), "%s: %d  %s: %d", trVDR("Setup.OSD$Left"), x0p, tr("Right"), width - x1p);
  osd->DrawText(x0p + 50, y0p + 50 + lineHeight, buffer, myBgColor, myTransparent, font);

  snprintf(buffer, sizeof(buffer), "%s: %d  %s: %d", trVDR("Setup.OSD$Width"), x1p - x0p, trVDR("Setup.OSD$Height"), y1p - y0p);
  osd->DrawText(x0p + 50, y0p + 50 + 2*lineHeight, buffer, myBgColor, myTransparent, font);

  // Erase old warning
  osd->DrawText(0, 0, tr("Width must be a multiple of 8!"), myBgColor, myTransparent, font, width, height, taCenter);
*/

  // New style (maybe faster?)
  osd->DrawRectangle(0, 0, width-1, height-1, myBgColor);

  // Draw lines
  osd->DrawRectangle(x0,y0, x0+lt,y1, myFgColor);
  osd->DrawRectangle(x0,y0, x1,y0+lt, myFgColor);
  osd->DrawRectangle(x0,y1-lt, x1,y1, myFgColor);
  osd->DrawRectangle(x1-lt,y0, x1,y1, myFgColor);

  // Draw text
  if (config.HideHelpText == false) {
     osd->DrawText(x0 + 50, y1 - 50 - 3*lineHeight, tr("Press 1-9 to move,"), myFgColor, myTransparent, font);
     osd->DrawText(x0 + 50, y1 - 50 - 2*lineHeight, tr("Up/Down & Left/Right to resize,"), myFgColor, myTransparent, font);
     osd->DrawText(x0 + 50, y1 - 50 - 1*lineHeight, tr("Ok to save, Back to cancel."), myFgColor, myTransparent, font);
     }

  // Draw info
  snprintf(buffer, sizeof(buffer), "%s: %d", trVDR("Setup.OSD$Top"), y0);
  osd->DrawText(x0 + 50, y0 + 50, buffer, myFgColor, myTransparent, font);

  snprintf(buffer, sizeof(buffer), "%s: %d", tr("Bottom"), height - y1);
  osd->DrawText(x0 + 50 + myTab, y0 + 50, buffer, myFgColor, myTransparent, font);

  snprintf(buffer, sizeof(buffer), "%s: %d", trVDR("Setup.OSD$Left"), x0);
  osd->DrawText(x0 + 50, y0 + 50 + lineHeight, buffer, myFgColor, myTransparent, font);

  snprintf(buffer, sizeof(buffer), "%s: %d", tr("Right"), width - x1);
  osd->DrawText(x0 + 50 + myTab, y0 + 50 + lineHeight, buffer, myFgColor, myTransparent, font);

  snprintf(buffer, sizeof(buffer), "%s: %d", trVDR("Setup.OSD$Width"), x1 - x0);
  osd->DrawText(x0 + 50, y0 + 50 + 2*lineHeight, buffer, myFgColor, myTransparent, font);

  snprintf(buffer, sizeof(buffer), "%s: %d", trVDR("Setup.OSD$Height"), y1 - y0);
  osd->DrawText(x0 + 50 + myTab, y0 + 50 + 2*lineHeight, buffer, myFgColor, myTransparent, font);

  // Draw warning if width is not a multiple of 8
  int w = x1 - x0;
  w &= ~0x07;
  if (w != x1 -x0)
     osd->DrawText(0, 0, tr("Width must be a multiple of 8!"), myFgColor, myTransparent, font, width, height, taCenter);

  osd->Flush();
}


//----------------------------------------------------------------------------

eOSState cScreenMenu::ProcessKey(eKeys Key)
{
//  x0p = x0; x1p = x1; y0p = y0; y1p = y1;

  eOSState state = cOsdObject::ProcessKey(Key);
  if (state == osUnknown) {
/*   old style
     switch (Key & ~k_Repeat) {
        case kUp:    if (y0 >= inc)   y0 -= inc; break;
        case kDown:  if (y0 < y1-inc) y0 += inc; break;
        case kLeft:  if (x0 >= inc)   x0 -= inc; break;
        case kRight: if (x0 < x1-inc) x0 += inc; break;
        case k2:     if (y1 > y0+inc) y1 -= inc; break;
        case k4:     if (x1 > x0+inc) x1 -= inc; break;
        case k6:     if (x1 <= width-inc)  x1 += inc; break;
        case k8:     if (y1 <= height-inc) y1 += inc; break;
        case kOk:    Store(); return osEnd;
        case kBack:  return osEnd;
        default:     return state;
        }
*/
     // new "monitor" style
     switch (Key & ~k_Repeat) {
        // move
        case k1:     if (y0 >= inc && x0 >= inc)              { y0 -= inc; y1 -= inc; x0 -= inc; x1 -= inc; }; break; // 2 + 4
        case k2:     if (y0 >= inc)                           { y0 -= inc; y1 -= inc; }; break;                       // up
        case k3:     if (y0 >= inc && x1 <= width-inc)        { y0 -= inc; y1 -= inc; x0 += inc; x1 += inc; }; break; // 2 + 6
        case k4:     if (x0 >= inc)                           { x0 -= inc; x1 -= inc; }; break;                       // left
        case k5:     x0 = (width-x1+x0)/2; y0 = (height-y1+y0)/2; x1 = width-x0; y1 = height-y0; break;           // center
        case k6:     if (x1 <= width-inc)                     { x0 += inc; x1 += inc; }; break;                       // right
        case k7:     if (y1 <= height-inc && x0 >= inc)       { y0 += inc; y1 += inc; x0 -= inc; x1 -= inc; }; break; // 8 + 4
        case k8:     if (y1 <= height-inc)                    { y0 += inc; y1 += inc; }; break;                       // down
        case k9:     if (y1 <= height-inc && x1 <= width-inc) { y0 += inc; y1 += inc; x0 += inc; x1 += inc; }; break; // 8 + 6
        case k0:     x0 = x0p; x1 = x1p; y0 = y0p; y1 = y1p; break; // reset
        // zoom - width must be a multiple of 8
        case kUp:    {    if (y0 >= inc && y1 <= height-inc) { y0 -= inc; y1 += inc; }
                     else if (y0 == 0   && y1 <= height-inc) { y0  = 0;   y1 += inc; }
                     else if (y0 >= inc && y1 == height)     { y0 -= inc; y1  = height; }
                     else if (y0 >= inc && y1 >= height-inc) { y0 -= (inc+y1-height); y1 = height; }
                     else if (y0 <= inc && y1 >= height-inc) { y0  = 0;   y1 = height; } ; break;
                     }
        case kLeft:       if (x0 + 4 < x1 - 4) { x0 += 4;   x1 -= 4; }; break;

        case kRight: {    if (x0 >= 4 && x1 <= width-4) { x0 -= 4; x1 += 4; }
                     else if (x0 == 0 && x1 <= width-8) { x0  = 0; x1 += 8; }
                     else if (x0 >= 8 && x1 == width)   { x0 -= 8; x1 = width; }
                     else if (x0 >= 8 && x1 >= width-8) { x0 -= (8+x1-width); x1 = width; }
                     else if (x0 <= 8 && x1 >= width-8) { x0  = 0; x1 = width; }
                     else if (x0 <  8 && x1 <= width-8) { x0  = 0; x1 += 8; x1 &= ~0x07; }; break;
                     }
        case kDown:       if (y0 + inc < y1 - inc) { y0 += inc; y1 -= inc; }; break; // horizontal down

        case kOk:    Store(); return osEnd;
        case kBack:  return osEnd;
        // Fun
        case kRed:    {    if (toggleOsd == 1) GrayBars();
                      else if (toggleOsd == 2) Bluge();
                      else if (toggleOsd == 3) Osdtest256();
                      else if (toggleOsd == 4) DrawColor();
                      else ColorBars();
                      return osContinue;
                      }
        case kGreen:  {
                      if (config.ShowGrid) (config.ShowGrid = 0);
                      else (config.ShowGrid = 1);
//                      Setup.Save();
                      if (toggleOsd == 1) ColorBars();
                      else if (toggleOsd == 2) GrayBars();
                      else if (toggleOsd == 4) DrawColor();
                      else DrawColor();
                      return osContinue;
                      }
        case kYellow: {
                      int tmpColor = myFgColor;
                      myFgColor = myBgColor;
                      myBgColor = tmpColor;
                      tmpColor = config.FgColor;
                      config.FgColor = config.BgColor;
                      config.BgColor = tmpColor;
//                      Setup.Save();
                      if (toggleOsd == 4) { if (stateOsd == 1) resetOsd = 1; DrawColor(); } // workaround for FF Cards
                      else Draw();
                      return osContinue;
                      }
        case kBlue:   Draw(); return osContinue;

        default:     return state;
        }

     Draw();
     state = osContinue;
     }

  return state;
}


//----------------------------------------------------------------------------

void cScreenMenu::Store()
{
  if (Setup.OSDLeft != x0 || Setup.OSDTop != y0 || Setup.OSDWidth != x1 - x0 || Setup.OSDHeight != y1 - y0) {
     Setup.OSDLeft   = x0;
     Setup.OSDTop    = y0;
     Setup.OSDWidth  = x1 - x0;
     if (Setup.OSDWidth < 100) Setup.OSDWidth *= 12;
     Setup.OSDWidth &= ~0x07; // OSD width must be a multiple of 8
     Setup.OSDHeight = y1 - y0;
     if (Setup.OSDHeight < 100) Setup.OSDHeight *= 27;

     Setup.Save();
     }
}
