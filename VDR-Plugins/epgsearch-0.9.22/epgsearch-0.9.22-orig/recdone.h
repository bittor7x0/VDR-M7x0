#ifndef __RECDONE_H
#define __RECDONE_H

#include <vdr/status.h>
#include <vdr/plugin.h>
#include <vdr/recording.h>
#include "epgsearchext.h"

class cSearchExt;

// --- cRecDone --------------------------------------------------------
class cRecDone : public cListObject {
public:
    char *title;             // Title of this event
    char *shortText;         // Short description of this event
    char *description;       // Description of this event
    char *aux;               // Aux info
    time_t startTime;        // Start time of the timer
    int duration; 
    int searchID;          // ID of the search, that triggered this recording
    tChannelID channelID;    
    
    static char *buffer;

    cRecDone();
    cRecDone(cTimer*, const cEvent* event, cSearchExt* search);
    ~cRecDone();

    static bool Read(FILE *f);
    bool Parse(char *s);
    const char *ToText(void);
    bool Save(FILE *f);
    int ChannelNr();
};

class cRecsDone : public cList<cRecDone>, public cMutex {
 private:
    char *fileName;
 public:
    void Clear(void)
	{
	    free(fileName);
	    fileName = NULL;
	    cList<cRecDone>::Clear();
	}
    cRecsDone(void) {}
    int GetCountRecordings(const cEvent* event, cSearchExt* search, cRecDone** first = NULL);
    int GetCountRecordings(const cEvent*, bool compareTitle, bool compareSubtitle, bool compareSummary, unsigned long, cRecDone** first = NULL);
    void RemoveSearchID(int ID);
    bool Load(const char *FileName = NULL);
    bool Save(void);
};

extern cRecsDone RecsDone;

#endif
