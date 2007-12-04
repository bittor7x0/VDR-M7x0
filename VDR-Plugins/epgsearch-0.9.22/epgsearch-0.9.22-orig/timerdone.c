#include <string>
#include "timerdone.h"
#include "epgsearchtools.h"

using std::string;

cTimersDone TimersDone;
char *cTimerDone::buffer = NULL;
// -- cTimerDone -----------------------------------------------------------------
cTimerDone::cTimerDone(void)
{
    start = stop = 0;
    buffer = NULL;
    searchID = -1;
}

cTimerDone::cTimerDone(const time_t Start, const time_t Stop, const cEvent* pEvent, const int SearchID)
{
    start = Start;
    stop = Stop;
    if (pEvent)
    {
	channelID = pEvent->ChannelID();
	title = pEvent->Title()?pEvent->Title():"";
	shorttext = pEvent->ShortText()?pEvent->ShortText():"";
    }
    searchID = SearchID;
    buffer = NULL;
}

cTimerDone::~cTimerDone(void)
{
    if (buffer) {
	free(buffer);
	buffer = NULL;
    }
}

bool cTimerDone::operator== (const cTimerDone &arg) const
{
    if (start == arg.start && stop == arg.stop && channelID == arg.channelID)
    {
	if (title != arg.title) return false;
	if (shorttext != "" && arg.shorttext != "" && shorttext != arg.shorttext) return false;
	 
	if (searchID > -1 && arg.searchID > -1)
	    return searchID == arg.searchID;
	else
	    return true;
    }
    else
	return false;
}

bool cTimerDone::Parse(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int parameter = 1;
    int valuelen;
    
#define MAXVALUELEN (10 * MaxFileName)
    
    char value[MAXVALUELEN];
    
    pos = line = strdup(s);
    pos_next = pos + strlen(pos);
    if (*pos_next == '\n') *pos_next = 0;
    while (*pos) {
	while (*pos == ' ') pos++;
	if (*pos) {
	    if (*pos != ':') {
		pos_next = strchr(pos, ':');
		if (!pos_next)
		    pos_next = pos + strlen(pos);
		valuelen = pos_next - pos + 1;
		if (valuelen > MAXVALUELEN) 
		{
		    LogFile.eSysLog("entry '%s' is too long. Will be truncated!", pos);  
		    valuelen = MAXVALUELEN;
		}
		strn0cpy(value, pos, valuelen);
		pos = pos_next;
		switch (parameter) {
		    case 1:  channelID = tChannelID::FromString(value);
			break;
		    case 2:  start = atol(value);
			break;
		    case 3:  stop = atol(value);
			break;
		    case 4:  searchID = atol(value);
			break;
		    case 5:  title = value;
			break;
		    case 6:  shorttext = value;
			break;
		} //switch
	    }
	    parameter++;
	}
	if (*pos) pos++;
    } //while
    
    
    title = ReplaceAll(title, "|", ":");
    shorttext = ReplaceAll(shorttext, "|", ":");

    free(line);
    return (parameter >= 6) ? true : false;
}

const char *cTimerDone::ToText(void) const
{
    free(buffer);
    cChannel *channel = Channels.GetByChannelID(channelID, true, true);
    string info = string(DAYDATETIME(start)) + " - " + string(channel?channel->Name():"");

    asprintf(&buffer, "%s:%ld:%ld:%d:%s:%s:%s", 
	     *channelID.ToString(),
	     start,
	     stop, 
	     searchID,
	     ReplaceAll(title, ":", "|").c_str(),
	     ReplaceAll(shorttext, ":", "|").c_str(),
	     ReplaceAll(info, ":", "|").c_str());
    return buffer;
}

bool cTimerDone::Save(FILE *f)
{
    return fprintf(f, "%s\n", ToText()) > 0;
}

// -- cTimerDones -----------------------------------------------------------------
cTimerDone* cTimersDone::InList(const time_t Start, const time_t Stop, const cEvent* pEvent, const int SearchID)
{
    cTimerDone td(Start, Stop, pEvent, SearchID);
    cTimerDone* timerdone = First();
    while (timerdone) 
    {
	if (*timerdone == td)
	    return timerdone;
	timerdone = Next(timerdone);
    }	
    return NULL;
}

void cTimersDone::ClearOutdated(void)
{
    // remove outdated items
    cTimerDone* timerdone = First();
    while (timerdone) 
    {
	cTimerDone* timerdoneNext = Next(timerdone);
	if (timerdone->stop < time(NULL))
	    Del(timerdone);
	timerdone = timerdoneNext;
    }	
}

void cTimersDone::Update(const time_t Start, const time_t Stop, const cEvent* pEvent, const int SearchID, cTimerDone* Timerdone)
{   
    cTimerDone* timerdone = InList(Start, Stop, pEvent, SearchID);
    if(timerdone) 
    {
	timerdone->start = Timerdone->start;
	timerdone->stop = Timerdone->stop;
	timerdone->channelID = Timerdone->channelID;
	timerdone->searchID = Timerdone->searchID;
	timerdone->title = Timerdone->title;
	timerdone->shorttext = Timerdone->shorttext;
	delete Timerdone;
    }
    else
	Add(Timerdone);
}
