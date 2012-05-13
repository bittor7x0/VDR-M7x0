/*
 * Sudoku: A plug-in for the Video Disk Recorder
 *
 * Copyright (C) 2005-2008, Thomas Günther <tom@toms-cafe.de>
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
 * $Id: setup.cpp 151 2008-08-29 22:57:35Z tom $
 */

#include "setup.h"
#include "commands.h"
#include "i18n.h"
#include <strings.h>

using namespace SudokuPlugin;


//--- class SudokuPlugin::SetupData --------------------------------------------

/** Constructor
 *
 * Initialize the setup parameters of the plugin with standard values.
 */
SetupData::SetupData()
{
  givens_count = 36;
  symmetric = 1;
  mark_errors = 1;
  mark_ambiguous = 1;
  show_possibles_pattern = 0;
  show_possibles_digits = 0;
  clear_marks = 0;
  key_red = CommandList::key_red_default_index();
  key_green = CommandList::key_green_default_index();
  key_yellow = CommandList::key_yellow_default_index();
#if VDRVERSNUM >= 10504
  strcpy(maxi_font, DefaultFontOsd);
  maxi_font_height = 31;
  maxi_font_width = 42;
  strcpy(mini_font, DefaultFontOsd);
  mini_font_height = 10;
  mini_font_width = 14;
#endif
  transparency = 50;
}

/** Parse the setup parameters of the plugin.
 *
 * This method is called for each setup parameter the plugin has previously
 * stored in the global setup data.
 */
bool SetupData::parse(const char* name, const char* value)
{
  CommandList cl;
  if (!strcasecmp(name, "GivensCount"))
    givens_count = atoi(value);
  else if (!strcasecmp(name, "Symmetric"))
    symmetric = atoi(value);
  else if (!strcasecmp(name, "MarkErrors"))
    mark_errors = atoi(value);
  else if (!strcasecmp(name, "MarkAmbiguous"))
    mark_ambiguous = atoi(value);
  else if (!strcasecmp(name, "ShowPossiblesPattern"))
    show_possibles_pattern = atoi(value);
  else if (!strcasecmp(name, "ShowPossiblesDigits"))
    show_possibles_digits = atoi(value);
  else if (!strcasecmp(name, "ClearMarks"))
    clear_marks = atoi(value);
  else if (!strcasecmp(name, "KeyRed"))
    key_red = cl.id_to_index(atoi(value), cl.key_red_default_index());
  else if (!strcasecmp(name, "KeyGreen"))
    key_green = cl.id_to_index(atoi(value), cl.key_green_default_index());
  else if (!strcasecmp(name, "KeyYellow"))
    key_yellow = cl.id_to_index(atoi(value), cl.key_yellow_default_index());
#if VDRVERSNUM >= 10504
  else if (!strcasecmp(name, "MaxiFont"))
    Utf8Strn0Cpy(maxi_font, value, MAXFONTNAME);
  else if (!strcasecmp(name, "MaxiFontHeight"))
    maxi_font_height = atoi(value);
  else if (!strcasecmp(name, "MaxiFontWidth"))
    maxi_font_width = atoi(value);
  else if (!strcasecmp(name, "MiniFont"))
    Utf8Strn0Cpy(mini_font, value, MAXFONTNAME);
  else if (!strcasecmp(name, "MiniFontHeight"))
    mini_font_height = atoi(value);
  else if (!strcasecmp(name, "MiniFontWidth"))
    mini_font_width = atoi(value);
#endif
  else if (!strcasecmp(name, "Transparency"))
    transparency = atoi(value);
  else
    return false;
  return true;
}


//--- class SudokuPlugin::SetupPage --------------------------------------------

/** Constructor */
SetupPage::SetupPage(SetupData& setup) :
  setup(setup), data(setup)
{
#if VDRVERSNUM >= 10504
  cFont::GetAvailableFontNames(&maxi_font_names);
  cFont::GetAvailableFontNames(&mini_font_names);
  maxi_font_names.Insert(strdup(DefaultFontOsd));
  mini_font_names.Insert(strdup(DefaultFontOsd));
  maxi_font_index = max(0, maxi_font_names.Find(data.maxi_font));
  mini_font_index = max(0, mini_font_names.Find(data.mini_font));
#endif

  Add(new cMenuEditIntItem(tr("Givens count"), &data.givens_count, 26, 81));
  Add(new cMenuEditBoolItem(tr("Symmetric givens"), &data.symmetric));
  Add(new cMenuEditBoolItem(tr("Mark errors"), &data.mark_errors));
  Add(new cMenuEditBoolItem(tr("Mark ambiguous numbers"),
                            &data.mark_ambiguous));
  Add(new cMenuEditBoolItem(tr("Show possible numbers as pattern"),
                            &data.show_possibles_pattern));
#if VDRVERSNUM >= 10504
  Add(new cMenuEditBoolItem(tr("Show possible numbers as digits"),
                            &data.show_possibles_digits));
#endif
  Add(new cMenuEditBoolItem(tr("Clear marks on reset"), &data.clear_marks));
  Add(new cMenuEditStraItem(tr("Key Red"), &data.key_red,
                            CommandList::count(), CommandList::texts()));
  Add(new cMenuEditStraItem(tr("Key Green"), &data.key_green,
                            CommandList::count(), CommandList::texts()));
  Add(new cMenuEditStraItem(tr("Key Yellow"), &data.key_yellow,
                            CommandList::count(), CommandList::texts()));
#if VDRVERSNUM >= 10504
  Add(new cMenuEditStraItem(tr("Large font"), &maxi_font_index,
                            maxi_font_names.Size(), &maxi_font_names[0]));
  Add(new cMenuEditIntItem(tr("Large font height (pixel)"),
                           &data.maxi_font_height, 10, MAXFONTSIZE));
  Add(new cMenuEditIntItem(tr("Large font width (pixel)"),
                           &data.maxi_font_width, 10, MAXFONTSIZE));
  Add(new cMenuEditStraItem(tr("Small font"), &mini_font_index,
                            mini_font_names.Size(), &mini_font_names[0]));
  Add(new cMenuEditIntItem(tr("Small font height (pixel)"),
                           &data.mini_font_height, 10, MAXFONTSIZE));
  Add(new cMenuEditIntItem(tr("Small font width (pixel)"),
                           &data.mini_font_width, 10, MAXFONTSIZE));
#endif
  Add(new cMenuEditIntItem(tr("Transparency (%)"), &data.transparency, 0, 100));
}

/** Store the setup parameters of the plugin.
 *
 * The setup parameters of the plugin are stored into the global setup data
 * file.
 */
void SetupPage::Store()
{
#if VDRVERSNUM >= 10504
  Utf8Strn0Cpy(data.maxi_font, maxi_font_names[maxi_font_index],
               sizeof(data.maxi_font));
  Utf8Strn0Cpy(data.mini_font, mini_font_names[mini_font_index],
               sizeof(data.mini_font));
#endif

  setup = data;
  SetupStore("GivensCount", setup.givens_count);
  SetupStore("Symmetric", setup.symmetric);
  SetupStore("MarkErrors", setup.mark_errors);
  SetupStore("MarkAmbiguous", setup.mark_ambiguous);
  SetupStore("ShowPossiblesPattern", setup.show_possibles_pattern);
  SetupStore("ShowPossiblesDigits", setup.show_possibles_digits);
  SetupStore("ClearMarks", setup.clear_marks);
  SetupStore("KeyRed", CommandList::id(setup.key_red));
  SetupStore("KeyGreen", CommandList::id(setup.key_green));
  SetupStore("KeyYellow", CommandList::id(setup.key_yellow));
#if VDRVERSNUM >= 10504
  SetupStore("MaxiFont", setup.maxi_font);
  SetupStore("MaxiFontHeight", setup.maxi_font_height);
  SetupStore("MaxiFontWidth", setup.maxi_font_width);
  SetupStore("MiniFont", setup.mini_font);
  SetupStore("MiniFontHeight", setup.mini_font_height);
  SetupStore("MiniFontWidth", setup.mini_font_width);
#endif
  SetupStore("Transparency", setup.transparency);
}
