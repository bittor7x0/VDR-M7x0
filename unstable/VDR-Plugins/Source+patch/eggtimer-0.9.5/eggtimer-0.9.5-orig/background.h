#ifndef __BACKGROUND_H
#define __BACKGROUND_H

#include <vdr/thread.h>

#include "eggtimer.h"
#include "eggtimer_data.h"

class cEggtimerThread : public cThread {

private:
	// running eggtimers
	cEggtimer* eggtimer;
	
	// timestamp when last info was displayed
	time_t lastInfo;
	
	void OsdMessage( const char* message, bool error = false );	
protected:
	virtual void Action(void);

public:
	cEggtimerThread();
	
	void Stop(void);
	
	cEggtimer* getEggtimer(); 
	
	void setEggtimer( cEggtimer* timer );
};

#endif
