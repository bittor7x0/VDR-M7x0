#include "eggtimer.h"

#include <time.h>

//#define DEBUG 1

// cEggtimerData -----------------------------------------------------------------
cEggtimerData::cEggtimerData() {
    command = NULL;
    Reset();
}

cEggtimerData::~cEggtimerData() {
    // if command buffer has been allocated
    if (command != NULL) {
	free(command);
	command = NULL;
    }
}

void cEggtimerData::Reset() {
    action = EGGTIMER_ACTION_MSG;
    timeMode = EGGTIMER_MODE_MINUTES;
    
    minutes = 5;
    seconds = 30;
    s_date = 0;
    endTime = 0;
    // if command buffer has been allocated
    if (command != NULL) {
	free(command);
	command = NULL;
    }
}

void cEggtimerData::SetTime(time_t timestamp) {
    endTime = timestamp;
    
    struct tm tm_r;
    struct tm now = *localtime_r( &timestamp, &tm_r );
    s_time = now.tm_hour * 100 + now.tm_min;
    
    now.tm_hour = 0;
    now.tm_min = 0;
    now.tm_sec = 0;
    now.tm_isdst = -1;
    
    s_date = mktime( &now );
}

cCommand* cEggtimerData::GetVDRCommand() {
    if (command == NULL)
	return NULL;
	
    for (cCommand* cmd = Commands.First(); cmd; cmd = Commands.Next(cmd) ) {
	if (strcmp( command, cmd->Title() ) == 0)
	    return cmd;
    }
    
    return NULL;
}

// cEggtimer ------ -----------------------------------------------------------------

cEggtimer::cEggtimer():cEggtimerData() {
    SetTime( time(NULL) );
    msg = NULL;    
}

void cEggtimer::Set() {
    time_t now = time(NULL);
    
    switch(timeMode) {
	case EGGTIMER_MODE_MINUTES: // minutes countdown
	    SetTime( now + minutes * 60 );
	break;
	case EGGTIMER_MODE_SECONDS: // minutes countdown
	    SetTime( now + seconds );
	break;
	case EGGTIMER_MODE_TIME: // Date/Time
	    struct tm tm_r;
	    tm tm = *localtime_r( &s_date, &tm_r );    
	    tm.tm_hour = s_time / 100; 
	    tm.tm_min = s_time % 100;
	    tm.tm_sec = 0;
	    tm.tm_isdst = -1;
    	
	    SetTime( mktime(&tm) );
	break;    
    }
}

bool cEggtimer::TimeReached() {
    time_t now = time(NULL);
    
    if (now >= endTime)
	return true;
    
    return false;
}

void cEggtimer::SetCommand( const char* cmd ) {
    if (command != NULL) {
	free(command);
	command = NULL;
    }
    
    // Allocate memory and copy command string
    if (cmd != NULL) {
	command = strdup(cmd);
    }
}

// cEggtimerTemplate ---------------------------------------------------------------
cEggtimerTemplate::cEggtimerTemplate():cEggtimerData() {
    name[0] = 0;
    msg[0] = 0;
    
    hour = -1;
    minute = -1;
}

void cEggtimerTemplate::SetTime() {
    // We use today's date set hours and minutes from config
    time_t timestamp = time(NULL);
	    
    struct tm tm_r;
    struct tm now = *localtime_r( &timestamp, &tm_r );
    s_time = now.tm_hour * 100 + now.tm_min;
	    
    // If spepcified time (hh:mm) has already passed
    if ( hour != -1 && (hour < now.tm_hour || ( hour == now.tm_hour && minute <= now.tm_min )) ) {
	// Get tm_r for tomorrow
	timestamp += (24 * 60 * 60);
	now = *localtime_r( &timestamp, &tm_r );
    }
    
    if (hour != -1)
	now.tm_hour = hour;
    if (minute != -1)	
        now.tm_min = minute;
    now.tm_sec = 0;
    now.tm_isdst = -1;
	    	    
    endTime = mktime( &now );
}

bool cEggtimerTemplate::SetTime( char **timeBuffer ) {
    bool retVal = false;
    
    char timeMeasure[5];
    
    if (timeBuffer != NULL && *timeBuffer != NULL) {
	if ( (sscanf(*timeBuffer, "%d%4[^;]", &minutes, timeMeasure) == 2) && strncmp(timeMeasure, "min", 4) == 0) {
	    timeMode = EGGTIMER_MODE_MINUTES;
	    retVal = true;
	} else if ( (sscanf(*timeBuffer, "%d%4[^;]", &seconds, timeMeasure) == 2) && strncmp(timeMeasure, "sec", 4) == 0) {
	    minutes = 0;
	    timeMode = EGGTIMER_MODE_SECONDS;
	    retVal = true;	    
	} else if ( sscanf(*timeBuffer, "%d:%d", &hour, &minute) == 2) {
	    minutes = 0;
	    seconds = 0;
	    timeMode = EGGTIMER_MODE_TIME;
	    retVal = true;	    	    
	} else if ( strncmp( *timeBuffer, "now", 3 ) ==  0) {
	    minutes = 0;
	    seconds = 0;
	    timeMode = EGGTIMER_MODE_TIME;
	    hour = -1;
	    minute = -1;
	    retVal = true;	    	    
	}		
    }
    
    free(*timeBuffer);
    *timeBuffer = NULL;
    
    return retVal;
}

bool cEggtimerTemplate::Parse(char *s) {
    if (s == NULL)
	return false;

    Reset();

    char *buffer = NULL; // buffer for channel desc
    char *timeBuffer = NULL; // buffer for time

    
    // channel switch eggtimers template
    if ( sscanf(s, "SWITCH;%a[^;];%a[^;];%20[^;]", &timeBuffer, &buffer, name ) == 3) {
#ifdef DEBUG
	printf("SWITCH found\n");
#endif		        
	channel = tChannelID::FromString( buffer );
	free(buffer);
	buffer = NULL;
	
	action = EGGTIMER_ACTION_SWITCH;
	
	if (!SetTime(&timeBuffer))
	    return false;
	
	msg[0] = 0;  // Switch eggtimers don't have a message
#ifdef DEBUG
	printf("SWITCH: Time mode: %d\nMinutes: %d\nName: %s\nAction: %d\n", timeMode, minutes, name, action);
#endif		
    } else
    // command eggtimer template
    if ( sscanf(s, "COMMAND;%a[^;];%a[^;]", &timeBuffer, &command ) == 2) {
#ifdef DEBUG
	printf("COMMAND found\n");
#endif		        

	if (!SetTime(&timeBuffer))
	    return false;

	strncpy( name, command, NAME_MAXLEN );
	action = EGGTIMER_ACTION_COMMAND;

	msg[0] = 0;  // Command eggtimers don't have a message
#ifdef DEBUG
	printf("COMMAND: Time mode: %d\nMinutes: %d\nName: %s\nAction: %d\nCommand: %s\n", timeMode, minutes, name, action, command);
#endif	
    // Templates for action OSD message
    } else if ( sscanf(s, "MESSAGE;%a[^;];%20[^;];%30[^;]", &timeBuffer, name, msg ) == 3) {
#ifdef DEBUG
	printf("MESSAGE found\n");
#endif		            
	if ( !SetTime( &timeBuffer ))
	    return false;
	action = EGGTIMER_ACTION_MSG;
#ifdef DEBUG	
	printf("MESSAGE: Time mode: %d\nMinutes: %d\nName: %s\nMsg: %s\n", timeMode, minutes, name, msg);
#endif
    } else { // Unparsable
	return false;
    }
    
    return true;
} 

// cEggtimerTemplates ---------------------------------------------------------------

cEggtimerTemplates::cEggtimerTemplates() {
    fixed = NULL;
}

bool cEggtimerTemplates::Load( const char* filename ) {
    cConfig < cEggtimerTemplate > ::Load(filename, true);

    fixed = new cEggtimerTemplate();
    strncpy( fixed->name, tr("Eggtimer"), NAME_MAXLEN );
    strncpy( fixed->msg, tr("Eggtimer: time is over"), MSG_MAXLEN );    
    Add( fixed );

    return true;
}
