#include <vdr/plugin.h>
#include <vdr/interface.h>
#include "menu.h"


eOSState cMenuSetupOsdadjust::cOpenScreenMenu::ProcessKey(eKeys Key)
{
  eOSState state = cOsdItem::ProcessKey(Key);

  if (state == osUnknown) {
     if (NORMALKEY(Key) == kOk || NORMALKEY(Key) == kRight) {
        fprintf(stderr, "osdadjust: Opening Screen Setup\n");
        cRemote::CallPlugin("osdadjust");
        parent->Store();
        return osEnd;
        }
     }

  return state;
}


cMenuSetupOsdadjust::cMenuSetupOsdadjust(void)
{
  aColors[0]  = tr("Transparent");
  aColors[1]  = tr("Transparent 50");
  aColors[2]  = tr("Black");
  aColors[3]  = tr("Red");
  aColors[4]  = tr("Green");
  aColors[5]  = tr("Yellow");
  aColors[6]  = tr("Magenta");
  aColors[7]  = tr("Blue");
  aColors[8]  = tr("Cyan");
  aColors[9]  = tr("White");
  aColors[10] = tr("Gray");

  aFormat[0] = "Auto";
  aFormat[1] = "PAL";
  aFormat[2] = "NTSC";

  aRatios[0] = "Auto";
  aRatios[1] = "4:3";
  aRatios[2] = "16:9";
  aRatios[3] = "1:1";

  newAspectRatio = config.AspectRatio;
  newVideoFormat = config.VideoFormat;
  newHideMenuEntry = config.HideMenuEntry;
  newHideHelpText = config.HideHelpText;
  newShowGrid = config.ShowGrid;
  newMute = config.Mute;
  newBgColor = config.BgColor;
  newFgColor = config.FgColor;
  newLineTn = config.LineTn;
  newMoveInc = config.MoveInc;

  AddCategory(tr("Size and Position"));
  Add(new cMenuEditStraItem(tr("Video Format"),         &newVideoFormat, 3, aFormat));
  Add(new cOpenScreenMenu(this));
  AddEmptyLine();

  AddCategory(tr("General Options"));
  Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &newHideMenuEntry,   trVDR("no"), trVDR("yes")));
  Add(new cMenuEditBoolItem(tr("Hide help text"),       &newHideHelpText,    trVDR("no"), trVDR("yes")));
  Add(new cMenuEditBoolItem(tr("Mute on open"),         &newMute,            trVDR("no"), trVDR("yes")));
  Add(new cMenuEditStraItem(tr("Background color"),     &newBgColor, 11, aColors));
  Add(new cMenuEditStraItem(tr("Foreground color"),     &newFgColor, 11, aColors));
  Add(new cMenuEditIntItem( tr("Line thickness"),       &newLineTn,  1, 10));
  Add(new cMenuEditIntItem( tr("Gauge"),                &newMoveInc, 1, 10));
  AddEmptyLine();

  AddCategory(trVDR("Miscellaneous"));
  Add(new cMenuEditStraItem(tr("Aspect Ratio"),         &newAspectRatio, 4, aRatios));
  Add(new cMenuEditBoolItem(tr("Show grid"),            &newShowGrid,    trVDR("no"), trVDR("yes")));
}


void cMenuSetupOsdadjust::Store(void)
{
  SetupStore("AspectRatio",     config.AspectRatio = newAspectRatio);
  SetupStore("VideoFormat",     config.VideoFormat = newVideoFormat);
  SetupStore("HideMenuEntry",   config.HideMenuEntry = newHideMenuEntry);
  SetupStore("HideHelpText",    config.HideHelpText = newHideHelpText);
  SetupStore("ShowGrid",        config.ShowGrid = newShowGrid);
  SetupStore("Mute",            config.Mute = newMute);
  SetupStore("BgColor",         config.BgColor = newBgColor);
  SetupStore("FgColor",         config.FgColor = newFgColor);
  SetupStore("LineTn",          config.LineTn = newLineTn);
  SetupStore("MoveInc",         config.MoveInc = newMoveInc);
}


void cMenuSetupOsdadjust::AddCategory(const char *Title) 
{
  cOsdItem *item = new cOsdItem(cString::sprintf("--- %s -------------------------------------------------------------------------------------", Title));
  item->SetSelectable(false);
  Add(item);
}

void cMenuSetupOsdadjust::AddEmptyLine(void)
{
  cOsdItem *item = new cOsdItem("");
  item->SetSelectable(false);
  Add(item);
}
