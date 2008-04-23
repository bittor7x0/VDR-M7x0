#ifndef _SET_EGGTIMER__H
#define _SET_EGGTIMER__H

#include "eggtimer.h"

class cSetEggTimerMenu : public cOsdMenu {
    private:
	const char *actions[EGGTIMER_ACTION_COUNT];
	const char *timeModes[EGGTIMER_MODE_COUNT];
	const char **commandNames;
	
	char **templNames;
	
	cEggtimer *eggtimer;
	int tempTimeMode;
	int tempAction;
	int tempChannelNum;
	
	int tempTemplateIndex;
	int templateIndex;
	int tempCommandIndex;
	
	cEggtimerThread *bgThread;
		
	void Set();
	bool StartEggtimer();
	
    public:
	cSetEggTimerMenu(cEggtimerThread *thread);
	~cSetEggTimerMenu();
	virtual eOSState ProcessKey(eKeys Key);
	virtual void Display(void);
};

#endif
