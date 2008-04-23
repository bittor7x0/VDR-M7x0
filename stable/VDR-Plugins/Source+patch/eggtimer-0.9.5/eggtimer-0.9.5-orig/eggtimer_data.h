#ifndef __EGGTIMER_DATA_H
#define __EGGTIMER_DATA_H

#include <time.h>

#include <vdr/config.h>

#define NAME_MAXLEN 20
#define MSG_MAXLEN 30

#define EGGTIMER_MODE_MINUTES 0
#define EGGTIMER_MODE_SECONDS 1
#define EGGTIMER_MODE_TIME 2

#define EGGTIMER_MODE_COUNT 3

#define EGGTIMER_ACTION_MSG 0
#define EGGTIMER_ACTION_SWITCH 1
#define EGGTIMER_ACTION_COMMAND 2

#define EGGTIMER_ACTION_COUNT 3

class cEggtimerData : public cListObject {
    public:
	cEggtimerData();
	~cEggtimerData();
	
	int action;
	int timeMode;
	
	time_t endTime;
	time_t s_date;
	int s_time;
	
	int minutes;
	int seconds;
	
	tChannelID channel;	
	char* command;
		
	void SetTime(long timestamp);
	void Reset();
	
	cCommand* GetVDRCommand();
};

class cEggtimer : public cEggtimerData {
    public:
	cEggtimer();
	void Set();
	char* msg;

	void SetCommand( const char* cmd );

	bool TimeReached();
};

class cEggtimerTemplate : public cEggtimerData {
    private:
	bool SetTime( char **timeBuffer );
	
	int hour;
	int minute;	
    public:
	cEggtimerTemplate();
	
	void SetTime();	
	char name[NAME_MAXLEN + 1];
	char msg[MSG_MAXLEN + 1];
	
	bool Parse(char *s);
};

class cEggtimerTemplates : public cConfig<cEggtimerTemplate> {
    private:
	// standard eggtimer template
	cEggtimerTemplate *fixed;
	
    public:
	cEggtimerTemplates();
	bool Load( const char* filename );
};

#endif
