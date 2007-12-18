/* * menu_reset.c: The actual menu implementations
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id:
 */

#include "menu_reset.h"
#include "zaplist.h"
#include "setup-zaphistory.h"

#include <vdr/menu.h>
#include <vdr/interface.h>

cMenuHistoryReset::cMenuHistoryReset(cZapHistoryChannel *zapChan)
:cOsdMenu(tr("Reset history") )
{
  zapChannel = zapChan;
  Setup();
}

void cMenuHistoryReset::Setup() {
  Clear(); // Remove old menuitems
  
  char *buffer = NULL;

  // Items to operate on the complete history  
  Add( new cOsdItem( tr("Delete all entries") ) );
  Add( new cOsdItem( tr("Reset all statistics") ) );
  
  // if a channel has been passed, add menuitems for operations on that channel
  if (zapChannel != NULL) {
    asprintf(&buffer, "%s%s %s", tr("Delete entry"), ":", zapChannel->GetChannel()->Name() );
    Add( new cOsdItem( buffer ) );
    free(buffer);

    asprintf(&buffer, "%s%s %s", tr("Reset statistics for"), ":", zapChannel->GetChannel()->Name() );
    Add( new cOsdItem( buffer ) );
    free(buffer);
  }

  // Buttons
  SetHelp(tr("Button$Ok"), 0,  0, tr("Button$Back") );  

  Display();
}

eOSState cMenuHistoryReset::ProcessKey(eKeys Key)
{
  eOSState state = cOsdMenu::ProcessKey(Key);

  if (state == osUnknown) {
    switch (Key) {
      case kBlue: // Close reset menu
	return osBack;
      case kRed:
      case kOk: // Execute operation
        // Validate index of selected item
	switch ( Current() ) {
	    case 0: // Delete all entries
		if (Interface->Confirm(tr("Delete complete history?") ) ) {
		    ZapHistory.Reset(NULL, false);
		    return osBack;
		}
		break;
	    case 1: // Reset complete statistisc
		if (Interface->Confirm(tr("Reset all?") ) ) {
		    ZapHistory.Reset(NULL);
		    return osBack;
		}
		break;
	    case 2: // Delete entry XYZ
		if (zapChannel != NULL && Interface->Confirm(tr("Delete entry?") ) ) {
		    ZapHistory.Reset(zapChannel, false);
		    return osBack;
		}
		break;
	    case 3: // Reset statistics for XYZ
		if (zapChannel != NULL && Interface->Confirm(tr("Reset entry?") ) ) {
		    ZapHistory.Reset(zapChannel);
		    return osBack;
		}
		break;
	}
      	return osContinue;
	
      default: return osContinue;  
    }
  }

  return state;
}
