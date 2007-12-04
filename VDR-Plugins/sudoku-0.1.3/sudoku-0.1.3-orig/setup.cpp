/*
 * Sudoku: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: setup.cpp 19 2005-11-03 21:54:19Z tom $
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
  SetupStore("ClearMarks", setup.clear_marks);
  SetupStore("Transparency", setup.transparency);
}
