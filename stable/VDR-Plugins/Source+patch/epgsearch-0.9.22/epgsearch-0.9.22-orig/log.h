#ifndef _LOG_INC_
#define _LOG_INC_

#include <stdarg.h>
#include <stdio.h>
#include <vdr/tools.h>

class cLogFile: public cFile
{
 public:
    static int loglevellimit;
    void Open(const char* filename, const char* version)
	{
	    if (loglevellimit == 0) return;
	    if (!cFile::Open(filename, O_CREAT|O_APPEND|O_WRONLY))
		esyslog("EPGSEARCH: could not open log file: %s", filename);
	    Log(1, "---------------------------------------", loglevellimit);
	    Log(1, "EPGSearch log started (verbose level %d, version %s)", loglevellimit, version);
	}
    void Log(int LogLevel, const char *text, ...)
	{
	    if (LogLevel > loglevellimit) return;
	    if(IsOpen()) 
	    {
		char* buffer = NULL;
		va_list Arg; 
		va_start(Arg,text); 
		vasprintf(&buffer, text, Arg); 
		va_end(Arg);
		time_t now = time(NULL);

		char datebuf[32];
		struct tm tm_r;
		tm *tm = localtime_r(&now, &tm_r);

		char *p = stpcpy(datebuf, WeekDayName(tm->tm_wday));
		*p++ = ' ';
		strftime(p, sizeof(datebuf) - (p - datebuf), "%d.%m.%Y", tm);
		
		char timebuf[25];
		strftime(timebuf, sizeof(timebuf), "%T", localtime_r(&now, &tm_r));

		char* log = NULL;
		asprintf(&log, "%s %s: %s\n", datebuf, timebuf, buffer);
		free(buffer);
		safe_write(*this, log, strlen(log));
		free(log);
	    }
	}
    void eSysLog(const char *text, ...)
	{
	    char* buffer = NULL;
	    va_list Arg; 
	    va_start(Arg,text); 
	    vasprintf(&buffer, text, Arg); 
	    va_end(Arg);
	    esyslog("EPGSearch: %s", buffer);
	    Log(1, "%s", buffer);
	    free(buffer);
	}
    void iSysLog(const char *text, ...)
	{
	    char* buffer = NULL;
	    va_list Arg; 
	    va_start(Arg,text); 
	    vasprintf(&buffer, text, Arg); 
	    va_end(Arg);
	    isyslog("EPGSearch: %s", buffer);
	    Log(1, "%s", buffer);
	    free(buffer);
	}
    int Level() { return loglevellimit; }

    static char *LogFileName;
};

extern cLogFile LogFile;

#endif

 
