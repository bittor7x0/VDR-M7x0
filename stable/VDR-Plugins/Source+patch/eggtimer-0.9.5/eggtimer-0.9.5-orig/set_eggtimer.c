#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "set_eggtimer.h"

#include <vdr/menuitems.h>

void cSetEggTimerMenu::Display(void) {
    SetHelp( tr("OK"), 0, 0, tr( "Button$Cancel") );
    
    cOsdMenu::Display();
}

cSetEggTimerMenu::cSetEggTimerMenu(cEggtimerThread *thread):cOsdMenu( tr("Set eggtimer"), 17) {
    actions[EGGTIMER_ACTION_MSG] = tr("OSD message");
    actions[EGGTIMER_ACTION_SWITCH] = tr("Switch channel");
    actions[EGGTIMER_ACTION_COMMAND] = tr("Execute command");
    
    timeModes[EGGTIMER_MODE_MINUTES] = tr("Minutes");
    timeModes[EGGTIMER_MODE_SECONDS] = tr("Seconds");    
    timeModes[EGGTIMER_MODE_TIME] = tr("Date/Time");
    
    commandNames = new const char*[Commands.Count()];
    int i=0;
    
    // Iterate VDRs commands
    for (cCommand *command = Commands.First(); command;  command = Commands.Next(command)) {
	commandNames[i] = command->Title();
	
	i++;
    }

    bgThread = thread;
    templateIndex = 0;
    
    tempTemplateIndex = templateIndex + 1;
    eggtimer = new cEggtimer();
    
    tempTimeMode = eggtimer->timeMode;
    tempAction = eggtimer->action;
    tempChannelNum = 1;
    tempCommandIndex = 0;
    
    cEggtimerTemplate *templ = NULL;
    templNames = new char*[templates.Count()];
    
    for (i=0; i<templates.Count(); i++) {
	templ = templates.Get(i);
	templNames[i] = templ->name;
    }
    
    Set();
    Display();
}

cSetEggTimerMenu::~cSetEggTimerMenu() {
	if (eggtimer != NULL)
	    delete eggtimer;
	
	if (commandNames != NULL)
	    delete commandNames;
}

void cSetEggTimerMenu::Set() {
    int current = Current();
    Clear();

    if (tempTemplateIndex != templateIndex) {
	cEggtimerTemplate *templ = templates.Get(templateIndex);
	
	eggtimer->timeMode = templ->timeMode;
	eggtimer->seconds = templ->seconds;
	eggtimer->minutes = templ->minutes;
	eggtimer->msg = templ->msg;
	eggtimer->action = templ->action;
	
	// if a command eggtimer template was selected
	if (eggtimer->action == EGGTIMER_ACTION_COMMAND) {
	    // get cCommand from template
	    cCommand* cmd = templ->GetVDRCommand();
	
	    if (cmd != NULL) { // if command was found
		tempCommandIndex = cmd->Index(); // set index of command
	    } else
		tempCommandIndex = 0;
	}
	
	// if a switch eggtimer template was selected	
	if (eggtimer->action == EGGTIMER_ACTION_SWITCH) {
	    // get cChannel by tChannelID from template
	    cChannel *chan = Channels.GetByChannelID( templ->channel );
	    if (chan != NULL)
		tempChannelNum = chan->Number();
	    else
	    	tempChannelNum = 1;
	}
	
	if (eggtimer->timeMode == EGGTIMER_MODE_TIME) { 
	    templ->SetTime();
	    eggtimer->SetTime(templ->endTime);    
	}
    }
    
    Add( new cMenuEditStraItem( tr("Template"), &templateIndex, templates.Count(), templNames));
    Add( new cMenuEditStraItem( tr("Action"), &eggtimer->action, EGGTIMER_ACTION_COUNT, actions));
    
    switch (eggtimer->action) {
	case EGGTIMER_ACTION_SWITCH:
	    Add( new cMenuEditChanItem( tr(" Channel"), &tempChannelNum ));
	    break;
	case EGGTIMER_ACTION_COMMAND:
	    Add( new cMenuEditStraItem( tr(" Command"), &tempCommandIndex, Commands.Count(), 
		commandNames));
	    break;
    }
    
    Add( new cMenuEditStraItem( tr("Timemode"), &eggtimer->timeMode, EGGTIMER_MODE_COUNT, timeModes));

    switch(eggtimer->timeMode) {
	case EGGTIMER_MODE_TIME:
	    Add( new cMenuEditDateItem( tr(" Date"), &eggtimer->s_date ));
	    Add( new cMenuEditTimeItem( tr(" Time"), &eggtimer->s_time ));
	    break;
	case EGGTIMER_MODE_MINUTES:
	    Add( new cMenuEditIntItem( tr(" Minutes"), &eggtimer->minutes, 1, 1000));
	    break;
	case EGGTIMER_MODE_SECONDS:
    	    Add( new cMenuEditIntItem( tr(" Seconds"), &eggtimer->seconds, 1, 3600));
	    break;
    }

    
    tempTimeMode = eggtimer->timeMode;
    tempAction = eggtimer->action;
    
    tempTemplateIndex = templateIndex;
    SetCurrent( Get(current) );
    Display();
}

bool cSetEggTimerMenu::StartEggtimer() {
    // If a Switch eggtimer
    if (eggtimer->action == EGGTIMER_ACTION_SWITCH) {
	// get cChannel instance for selected channel
	cChannel *chan = Channels.GetByNumber( tempChannelNum );
	
	if (chan != NULL)
	    // set tChannelID
	    eggtimer->channel = chan->GetChannelID();
	else {
	    // channel not found => return false
	    esyslog("Channel not found");
    	    return false;
	}
    }
	    
    // If a command eggtimer
    if (eggtimer->action == EGGTIMER_ACTION_COMMAND) {	    
	// get cCommand from selected index
    	cCommand* cmd = Commands.Get(tempCommandIndex);

	if (cmd != NULL) // copy command title to the eggtimer
	    eggtimer->SetCommand( cmd->Title() );
	else // command not found => return false
	    return false;
    }

    // pass eggtimer to background thread
    bgThread->setEggtimer( eggtimer );
    eggtimer = NULL;
    bgThread->Start();
    
    return true;
}

eOSState cSetEggTimerMenu::ProcessKey(eKeys Key) {
    eOSState state = osUnknown;

    state = cOsdMenu::ProcessKey(Key);
    
    if (tempTimeMode != eggtimer->timeMode || tempTemplateIndex != templateIndex ||
	tempAction != eggtimer->action) {
	Set();
	Display();
    }

    switch(Key) {
	case kOk:
	case kRed: // Start eggtimer
    	    if ( StartEggtimer() ) {
		// close OSD
		state = osEnd;
	    }
	    break;
	case kBlue:
	    // delete cEggtimer instance
	    delete eggtimer;
	    eggtimer = NULL;
	    // back to main menu
	    state = osBack; break;
	default: break;
    }
	
    if (state == osUnknown)
	state = osContinue;
    
    return state;
}
