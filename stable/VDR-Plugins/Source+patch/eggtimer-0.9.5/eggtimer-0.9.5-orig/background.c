#include <unistd.h>

#include <vdr/channels.h>
#include <vdr/device.h>

#include "background.h"

cEggtimerThread::cEggtimerThread()
#if VDRVERSNUM > 10300 
:cThread("Eggtimer") 
#else
:cThread() 
#endif
{
#ifdef DEBUG
	printf("cEggtimerThread::cEggtimerThread\n" );
#endif
	lastInfo = 0;
}

/*
Returns the current eggtimer
*/
cEggtimer* cEggtimerThread::getEggtimer() {
	return eggtimer;
}

/*
sets the current eggtimer
*/
void cEggtimerThread::setEggtimer( cEggtimer* timer ) {
	eggtimer = timer;
	
	if (eggtimer != NULL)
		eggtimer->Set();
}

void cEggtimerThread::Stop() {
#ifdef DEBUG
    printf("cEggtimerThread::Stop\n" );
#endif
    Cancel(30);
}

/*
Displays a message in the OSD

Displaying messages from outside vdrs main-thread is somewhat tricky.
A real solution was introduced in vdr-1.3.37.

With VDR 1.4 release unstable support for VDR < 1.3.37 will be dropped 
*/
void cEggtimerThread::OsdMessage( const char *message, bool error ) {
    if (error) {
#if VDRVERSNUM < 10307
	// Use Interface->Message
	Interface->Error( tr(message) );
#elseif VDRVERSNUM < 10337
	// Use Skins.Message
	Skins.Message( mtError, tr(message) );
#else
	// Use Skins.QueueMessage
	Skins.QueueMessage( mtError, tr(message) );
#endif
    } else {
#if VDRVERSNUM < 10307
	// Use Interface->Message
	Interface->Status( tr(message) );
	usleep( Setup.OSDMessageTime * 1000000 );
	Interface->Status( NULL );	
#elseif VDRVERSNUM < 10337
	// Use Skins.Message
	Skins.Message( mtStatus, tr(message) );
#if VDRVERSNUM < 10314
	usleep( Setup.OSDMessageTime * 1000000 );
#else
	cCondWait::SleepMs( Setup.OSDMessageTime * 1000 );
#endif		
	Skins.Message( mtStatus, null );
#else
	// Use Skins.QueueMessage
	Skins.QueueMessage( mtInfo, tr(message), 0, -1 );
#endif
    }
}

void cEggtimerThread::Action(void) {
#ifdef DEBUG
    printf("cEggtimerThread::Action\n" );
#endif
    bool leaveLoop = false;    

    while ( Running() && !leaveLoop ) {
#ifdef DEBUG
	printf("cEggtimerThread::Idle loop\n" );
#endif

	// time is over ?
	if (eggtimer->TimeReached()) {
#ifdef DEBUG
	    printf("cEggtimerThread::Time reached\n" );
#endif
	    switch (eggtimer->action) {
		case EGGTIMER_ACTION_SWITCH: { // switch to channel
#ifdef DEBUG
		    printf("cEggtimerThread::Switch channel\n" );
#endif
		    cChannel *chan = Channels.GetByChannelID( eggtimer->channel );
		    cDevice *device = cDevice::ActualDevice();
				    
		    if ( chan == NULL || device == NULL || !device->SwitchChannel(chan , true) ) {
			OsdMessage( tr("Eggtimer: could not switch channel"), true );
		    }
		    
		    leaveLoop = true; // Stop eggtimer thread
		} break;
		
		case EGGTIMER_ACTION_MSG: { // OSD message
#ifdef DEBUG
		    printf("cEggtimerThread::Display message\n" );
#endif
		
		    time_t now = time(NULL); // for repeating messages we need
		    // current time

		    // If 2.5 * MessageTime (VDR Setup) has expired => display again
		    if (now - lastInfo >= (int) (2.5 * Setup.OSDMessageTime) ) {
			// Display the msg
			OsdMessage( eggtimer->msg );

			// remember time when message was displayed
			lastInfo = now;
		    }
		} break;
		
		case EGGTIMER_ACTION_COMMAND: { // Execute command
#ifdef DEBUG
			printf("cEggtimerThread::Executing command\n" );
#endif
			
			cCommand *command = eggtimer->GetVDRCommand();
		    
			if (command != NULL) {
			    command->Execute();
			} else {
			    OsdMessage( tr("Eggtimer: command not found"), true );
			}
			
			leaveLoop = true; // Stop eggtimer thread
		} break;
	    }
	}
	
	// Sleep 0.5 seconds	
#if VDRVERSNUM < 10314
	usleep(500000);
#else
	cCondWait::SleepMs(500);
#endif	
    }
    
#ifdef DEBUG
    printf("cEggtimerThread::Idle loop finished\n" );
#endif
    
    if (eggtimer != NULL)
	delete eggtimer;
}
