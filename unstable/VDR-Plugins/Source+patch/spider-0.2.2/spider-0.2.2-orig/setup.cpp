/*
 * Spider-Arachnid: A plugin for the Video Disk Recorder
 *
 * Copyright (C) 2005-2007, Thomas Günther <tom@toms-cafe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id: setup.cpp 97 2007-09-24 22:29:48Z tom $
 */

#include "setup.h"
#include "i18n.h"
#include <strings.h>

using namespace SpiderPlugin;


//--- class SpiderPlugin::SetupData --------------------------------------------

/** Constructor
 *
 * Initialize the setup parameters of the plugin with standard values.
 */
SetupData::SetupData()
{
  variation = Normal;
  deck_count = 2;
  pile_count = 10;
  deal_count = 5;
  osd_left = 48;
  osd_top = 45;
  osd_width = 624;
  osd_height = 486;
  osd_error_compensation = ShrinkHeight;
  hide_toprow = 1;
}

/** Parse the setup parameters of the plugin.
 *
 * This method is called for each setup parameter the plugin has previously
 * stored in the global setup data.
 */
bool SetupData::parse(const char* name, const char* value)
{
  if (!strcasecmp(name, "Variation"))
    variation = atoi(value);
  else if (!strcasecmp(name, "CustomDeckCount"))
    deck_count = atoi(value);
  else if (!strcasecmp(name, "CustomPileCount"))
    pile_count = atoi(value);
  else if (!strcasecmp(name, "CustomDealCount"))
    deal_count = atoi(value);
  else if (!strcasecmp(name, "OSDLeft"))
    osd_left = atoi(value);
  else if (!strcasecmp(name, "OSDTop"))
    osd_top = atoi(value);
  else if (!strcasecmp(name, "OSDWidth"))
    osd_width = atoi(value);
  else if (!strcasecmp(name, "OSDHeight"))
    osd_height = atoi(value);
  else if (!strcasecmp(name, "OSDErrorCompensation"))
    osd_error_compensation = atoi(value);
  else if (!strcasecmp(name, "HideToprow"))
    hide_toprow = atoi(value);
  else
    return false;
  return true;
}


//--- class SpiderPlugin::SetupPage --------------------------------------------

/** Constructor */
SetupPage::SetupPage(SetupData& setup) :
  setup(setup), data(setup)
{
  variationTexts[0] = tr("Mini (one deck)");
  variationTexts[1] = tr("Normal");
  variationTexts[2] = tr("Custom");
  compensationTexts[0] = tr("Shrink height");
  compensationTexts[1] = tr("Shrink width");
  compensationTexts[2] = tr("Shrink width and height");
  compensationTexts[3] = tr("Reduce colors");
  SetHelp(tr("Reset"));
  Setup();
}

/** Set values into the menu page */
void SetupPage::Setup()
{
  int current = Current();
  Clear();

  Add(new cMenuEditStraItem(tr("Variation"), &data.variation,
                            3, variationTexts));

  if (data.variation == SetupData::Custom)
  {
    // TRANSLATORS: note the leading blank!
    Add(new cMenuEditIntItem(tr(" Deck count"), &data.deck_count, 1, 4));
    // TRANSLATORS: note the leading blank!
    Add(new cMenuEditIntItem(tr(" Pile count"), &data.pile_count, 1, 20));
    // TRANSLATORS: note the leading blank!
    Add(new cMenuEditIntItem(tr(" Deal count"), &data.deal_count, 1, 10));
  }

  Add(new cMenuEditIntItem(tr("OSD position left"), &data.osd_left, 0, 720));
  Add(new cMenuEditIntItem(tr("OSD position top"), &data.osd_top, 0, 576));
  Add(new cMenuEditIntItem(tr("OSD width"), &data.osd_width, 100, 720));
  Add(new cMenuEditIntItem(tr("OSD height"), &data.osd_height, 100, 576));
  Add(new cMenuEditStraItem(tr("OSD error compensation"),
                            &data.osd_error_compensation,
                            4, compensationTexts));
  Add(new cMenuEditBoolItem(tr("Hide top row"), &data.hide_toprow));

  SetCurrent(Get(current));
  Display();
}

/** Process user events */
eOSState SetupPage::ProcessKey(eKeys Key)
{
  int custom = (data.variation == SetupData::Custom);
  eOSState state = cMenuSetupPage::ProcessKey(Key);

  if (Key != kNone && custom != (data.variation == SetupData::Custom))
    Setup();

  if (state == osUnknown && Key == kRed)
  {
    data = SetupData();
    Setup();
    state = osContinue;
  }
  return state;
}

/** Store the setup parameters of the plugin.
 *
 * The setup parameters of the plugin are stored into the global setup data
 * file.
 */
void SetupPage::Store()
{
  setup = data;
  SetupStore("Variation", setup.variation);
  SetupStore("CustomDeckCount", setup.deck_count);
  SetupStore("CustomPileCount", setup.pile_count);
  SetupStore("CustomDealCount", setup.deal_count);
  SetupStore("OSDLeft", setup.osd_left);
  SetupStore("OSDTop", setup.osd_top);
  SetupStore("OSDWidth", setup.osd_width);
  SetupStore("OSDHeight", setup.osd_height);
  SetupStore("OSDErrorCompensation", setup.osd_error_compensation);
  SetupStore("HideToprow", setup.hide_toprow);
}
