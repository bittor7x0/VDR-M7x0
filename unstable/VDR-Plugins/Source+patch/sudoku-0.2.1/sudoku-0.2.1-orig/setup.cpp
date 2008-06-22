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
 * $Id: setup.cpp 135 2008-04-05 20:23:44Z tom $
 */

#include "setup.h"
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
  transparency = 50;
}

/** Parse the setup parameters of the plugin.
 *
 * This method is called for each setup parameter the plugin has previously
 * stored in the global setup data.
 */
bool SetupData::parse(const char* name, const char* value)
{
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
  Add(new cMenuEditIntItem(tr("Transparency (%)"), &data.transparency, 0, 100));
}

/** Store the setup parameters of the plugin.
 *
 * The setup parameters of the plugin are stored into the global setup data
 * file.
 */
void SetupPage::Store()
{
  setup = data;
  SetupStore("GivensCount", setup.givens_count);
  SetupStore("Symmetric", setup.symmetric);
  SetupStore("MarkErrors", setup.mark_errors);
  SetupStore("MarkAmbiguous", setup.mark_ambiguous);
  SetupStore("ShowPossiblesPattern", setup.show_possibles_pattern);
  SetupStore("ShowPossiblesDigits", setup.show_possibles_digits);
  SetupStore("ClearMarks", setup.clear_marks);
  SetupStore("Transparency", setup.transparency);
}
