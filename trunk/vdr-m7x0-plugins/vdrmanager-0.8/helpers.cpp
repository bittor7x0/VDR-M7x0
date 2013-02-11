/*
 * event und message handler
 */

#include <time.h>
#include <unistd.h>
#include <values.h>
#include <vdr/plugin.h>
#include <vdr/timers.h>
#include <vdr/recording.h>
#include <vdr/channels.h>
#include <vdr/epg.h>
#include <vdr/videodir.h>
#include <vdr/menu.h>
#include "helpers.h"
#include "vdrmanagerthread.h"
#include <memory>

#define TIMER_SEP		"#|#|#"

string cHelpers::GetRecordings(string args) {
	return SafeCall(GetRecordingsIntern);
}

string cHelpers::GetTimers(string args) {
	return SafeCall(GetTimersIntern);
}

string cHelpers::GetChannels(string args) {
	return SafeCall(GetChannelsIntern, args);
}

string cHelpers::GetChannelEvents(string args) {
	return SafeCall(GetEventsIntern, Trim(args), "");
}

string cHelpers::SetChannel(string args) {
	return SafeCall(SetChannelIntern, args);
}

string cHelpers::GetTimeEvents(string args) {

	args = Trim(args);

	size_t space = args.find(' ');
	if (space == string::npos) {
		return SafeCall(GetEventsIntern, "", args);
	}

	string when = args.substr(0, space);
	string wantedChannels = args.substr(space + 1);

	return SafeCall(GetEventsIntern, Trim(wantedChannels), Trim(when));
}

string cHelpers::SetTimer(string args) {
	return SafeCall(SetTimerIntern, args);
}

string cHelpers::DelRecording(string args) {
	return SafeCall(DelRecordingIntern, args);
}

string cHelpers::SearchEvents(string args) {

	args = Trim(args);

	string wantedChannels;
	string pattern;

	size_t space = args.find(':');
	if (space == string::npos) { //so only search term
		pattern = args;
		wantedChannels = "";
	} else {
		wantedChannels = args.substr(0, space);
		pattern = args.substr(space + 1);
	}
	pattern = UnMapSpecialChars(pattern);
	return SafeCall(SearchEventsIntern, Trim(wantedChannels), Trim(pattern));
}

string cHelpers::GetTimersIntern() {

	string result = "START\r\n";

	// iterate through all timers
	for (cTimer * timer = Timers.First(); timer; timer = Timers.Next(timer)) {
		result += ToText(timer);
	}

	return result + "END\r\n";
}

string cHelpers::GetRecordingsIntern() {

	string result = "START\r\n";
	//iterate through all recordings
	cRecording* recording = NULL;
	for (int i = 0; i < Recordings.Count(); i++) {
		recording = Recordings.Get(i);
		result += ToText(recording);
	}
	return result + "END\r\n";
}

string cHelpers::GetChannelsIntern(string wantedChannels) {

	string result = "START\r\n";
	string currentGroup = "";

	char number[10];
	for (cChannel * channel = Channels.First(); channel; channel =
			Channels.Next(channel)) {

		// channel group
		if (channel->GroupSep()) {
			currentGroup = channel->Name();
			continue;
		}

		// channel filtering
		if (IsWantedChannel(channel, wantedChannels)) {
			// current group
			if (currentGroup.length() > 0) {
				result += "C0:";
				result += currentGroup;
				result += "\r\n";
				currentGroup = "";
			}

			// channel
			snprintf(number, sizeof(number) - 1, "C%d", channel->Number());
			result += number;
			result += ":";
			result += channel->Name();
			result += ":";
			result += channel->Provider();
			result += ":";
			result += channel->GetChannelID().ToString();
			result += ":";
			result += GetAudioTracks(channel);
			result += ":";
			result += "\r\n";
		}
	}

	return result + "END\r\n";
}

string cHelpers::SetChannelIntern(const string args) {

	if (args.size() == 0) {
		return "!ERROR:SetChannel;empty args\r\n";
	}
	

	cChannel *channel;
	bool isnum = true;
	 for (int i = 0; i < (int)args.length(); i++) {
	       if (!std::isdigit(args[i])){
	    	   isnum = false;
	    	   break;
	       }

	   }

	if(isnum){
		int nr = atoi(args.c_str());
		channel = Channels.GetByNumber(nr);
	}  else {
		tChannelID chid = tChannelID::FromString(args.c_str());
		channel = Channels.GetByChannelID(chid);
	}


	if (!channel) {
		return "!ERROR:SetChannel;Unable to find channel " + args + "\r\n";
	}

	ostringstream outStream;

	if (!cDevice::PrimaryDevice()->SwitchChannel(channel, true)) {
	        outStream << channel->Number();
	        return "!ERROR:SetChannel;Error switching to channel " + outStream.str() + "\r\n";
	}
        
        return "START\r\n" + MapSpecialChars(channel->GetChannelID().ToString()) + "\r\nEND\r\n";
}

string cHelpers::GetAudioTracks(const cChannel* channel) {

	string result = "";
	int count = 0;
	for (int i = 0; channel->Apid(i) != 0; ++i, ++count)
		;
	for (int i = 0; channel->Dpid(i) != 0; ++i, ++count)
		;

	if (count > 1) {
		int index = 1;
		string sep = "";
		for (int i = 0; channel->Apid(i) != 0; ++i, ++index) {
			result += sep + "a," + (const char*) itoa(index) + ","
					+ channel->Alang(i);
			sep = "|";
		}
		for (int i = 0; channel->Dpid(i) != 0; ++i, ++index) {
			result += sep + "d," + (const char*) itoa(index) + ","
					+ channel->Dlang(i);
		}
		sep = "|";
	}
	return result;
}

string cHelpers::GetEventsIntern(string wantedChannels, string when) {

	when = ToUpper(when);
	time_t wantedTime;
	if (when == "NOW" || when == "NEXT") {
		wantedTime = time(0);
	} else {
		wantedTime = atol(when.c_str());
	}

	string result = "START\r\n";

	cSchedulesLock schedulesLock;
	const cSchedules * schedules = cSchedules::Schedules(schedulesLock);
	for (cSchedule * schedule = schedules->First(); schedule; schedule =
			schedules->Next(schedule)) {

		cChannel * channel = Channels.GetByChannelID(schedule->ChannelID());
		if (!IsWantedChannel(channel, wantedChannels)) {
			continue;
		}

		const cList<cEvent> * events = schedule->Events();
		for (cEvent * event = events->First(); event;
				event = events->Next(event)) {
			if (IsWantedTime(wantedTime, event)) {
				cEvent * match = event;
				if (when == "NEXT") {
					match = events->Next(match);
					if (!match) {
						break;
					}
				}

				result += ToText(match);

				if (when.length() > 0) {
					break;
				}
			}
		}
	}

	return result + "END\r\n";
}

string cHelpers::DelRecordingIntern(string args) {

	if (args.size() == 0) {
		return "!ERROR:DelRecording;empty args\r\n";
	}

	int index = atoi(args.c_str());

	cRecording * r;
	if (index < 0|| index >= Recordings.Count()
	|| (r = Recordings.Get(index)) == NULL) {return "!ERROR:DelRecording;Wrong recording index -> " + args + "\r\n";
}

	if (r->Delete()) {
		Recordings.DelByName(r->FileName());
		return "START\r\nEND\r\n";
	} else {
		return "!ERROR:Failed\r\n";
	}

}

string cHelpers::SetTimerIntern(char op, string param) {

	switch (op) {
	case 'C': // new timer
	case 'c': {
		auto_ptr<cTimer> newTimer(new cTimer);
		if (!newTimer->Parse(param.c_str())) {
			return Error("Error in timer settings");
		}

		cTimer* checkTimer = Timers.GetTimer(newTimer.get());
		if (checkTimer) {
			return Error("Timer already defined");
		}

		Timers.Add(newTimer.get());
		Timers.SetModified();
		dsyslog( "[vdrmanager] timer %s added", *newTimer->ToDescr());
		newTimer.release();
		break;
	}
	case 'D':
	case 'd': {
		if (Timers.BeingEdited()) {
			return Error("Timers are being edited - try again later");
		}

		dsyslog("[vdrmanager] try parse %s ", param.c_str());

		auto_ptr<cTimer> timer(new cTimer);
		if (!timer->Parse(param.c_str())) {
			return Error("Error in timer settings");
		}

		dsyslog("[vdrmanager] timer %s parsed", *timer->ToDescr());

		cTimer* oldTimer = Timers.GetTimer(timer.get());
		if (oldTimer == 0) {
			return Error("Timer not defined");
		}

		if (oldTimer->Recording()) {
			oldTimer->Skip();
			cRecordControls::Process(time(0));
		}
		Timers.Del(oldTimer);
		Timers.SetModified();
		dsyslog("[vdrmanager] timer %s deleted", *timer->ToDescr());
		break;
	}
	case 'M':
	case 'm': {

		if (Timers.BeingEdited()) {
			return Error("Timers are being edited - try again later");
		}

		string sep = string(TIMER_SEP);

		size_t idx = param.find(sep);
		if (idx == string::npos) {
			return Error("no separator found");
		}

		string newt = param.substr(0, idx);
		string oldt = param.substr(idx + sep.size());

		auto_ptr<cTimer> otimer(new cTimer);
		if (!otimer->Parse(oldt.c_str())) {
			return Error("Error in timer settings");
		}

		cTimer* oldTimer = Timers.GetTimer(otimer.get());
		if (oldTimer == 0) {
			return Error("Timer not defined");
		}

		cTimer copy = *oldTimer;
		if (!copy.Parse(newt.c_str())) {
			return Error("Error in timer settings");
		}

		*oldTimer = copy;
		Timers.SetModified();
		dsyslog(
				"[vdrmanager] timer %s modified (%s)", *oldTimer->ToDescr(), oldTimer->HasFlags(tfActive) ? "active" : "inactive");

		break;
	}
	case 'T':
	case 't': {
		if (Timers.BeingEdited()) {
			return Error("Timers are being edited - try again later");
		}

		auto_ptr<cTimer> timer(new cTimer);
		if (!timer->Parse(param.c_str())) {
			return Error("Error in timer settings");
		}

		cTimer* toggleTimer = Timers.GetTimer(timer.get());
		if (toggleTimer == 0) {
			return Error("Timer not defined");
		}

		toggleTimer->OnOff();
		Timers.SetModified();
		dsyslog(
				"[vdrmanager] timer %s toggled %s", *toggleTimer->ToDescr(), toggleTimer->HasFlags(tfActive) ? "on" : "off");
		break;
	}
	default:
		return Error("unknown timer command");
	}
	return "START\r\nEND\r\n";
}

string cHelpers::SetTimerIntern(string args) {
	//C dasdasda#|#|#
	//C 1 as:asd:SadA:sd  ada:a :ada
	// separete timer number
	//size_t sep = args.find('');
	//if (sep == string::npos) {
	//return "!ERROR:no separator found\r\n";
	//}

	char operation = args[0];

	args = Trim(args.substr(1));

	// Use StringReplace here because if ':' are characters in the
	// title or aux string it breaks parsing of timer definition
	// in VDRs cTimer::Parse method.  The '|' will be replaced
	// back to ':' by the cTimer::Parse() method.

	// Fix was submitted by rofafor: see
	// http://www.vdr-portal.de/board/thread.php?threadid=100398
	string params = replaceAll(args, "|##", "|");

	//replace also newlines
	params = replaceAll(params, "||#", "\n");

	string result = SetTimerIntern(operation, params);

	return result;
}

string cHelpers::Error(const string& msg) {
	return "START\r\n!ERROR:" + msg + "\r\nEND\r\n";
}

string cHelpers::SearchEventsIntern(string wantedChannels, string pattern) {

	string result = "START\r\n";

	cSchedulesLock schedulesLock;
	const cSchedules * schedules = cSchedules::Schedules(schedulesLock);
	for (cSchedule * schedule = schedules->First(); schedule; schedule =
			schedules->Next(schedule)) {

		cChannel * channel = Channels.GetByChannelID(schedule->ChannelID());
		if (!IsWantedChannel(channel, wantedChannels)) {
			continue;
		}

		const cList<cEvent> * events = schedule->Events();
		for (cEvent * event = events->First(); event;
				event = events->Next(event)) {

			if (IsWantedTime(0, event) && IsWantedEvent(event, pattern)) { //time must be ok, so stop > now
				result += ToText(event);
			}
		}
	}

	return result + "END\r\n";
}

string cHelpers::ToText(cRecording * recording) {
	const cRecordingInfo * info = recording->Info();
#if APIVERSNUM >= 10705
	const cEvent * event = info->GetEvent();
#endif
	/**
	 tChannelID ChannelID(void) const;
	 const cSchedule *Schedule(void) const { return schedule; }
	 tEventID EventID(void) const { return eventID; }
	 uchar TableID(void) const { return tableID; }
	 uchar Version(void) const { return version; }
	 int RunningStatus(void) const { return runningStatus; }
	 const char *Title(void) const { return title; }
	 const char *ShortText(void) const { return shortText; }
	 const char *Description(void) const { return description; }
	 const cComponents *Components(void) const { return components; }
	 uchar Contents(int i = 0) const { return (0 <= i && i < MaxEventContents) ? contents[i] : 0; }
	 int ParentalRating(void) const { return parentalRating; }
	 time_t StartTime(void) const { return startTime; }
	 time_t EndTime(void) const { return startTime + duration; }
	 int Duration(void) const { return duration; }
	 time_t Vps(void) const { return vps; }
	 time_t Seen(void) const { return seen; }
	 bool SeenWithin(int Seconds) const { return time(NULL) - seen < Seconds; }
	 bool HasTimer(void) const;
	 bool IsRunning(bool OrAboutToStart = false) const;
	 static const char *ContentToString(uchar Content);
	 cString GetParentalRatingString(void) const;
	 cString GetDateString(void) const;
	 cString GetTimeString(void) const;
	 cString GetEndTimeString(void) const;
	 cString GetVpsString(void) const;
	 */

	char buf[100];
	string result = "";

#if APIVERSNUM >= 10705
	time_t startTime = event->StartTime();
	time_t endTime = event->EndTime();
#else
	time_t startTime = 0L;
	time_t endTime = 1L;
#endif
	snprintf(buf, sizeof(buf) - 1, "%d", recording->Index());
	result = buf;
	result += ":";

	snprintf(buf, sizeof(buf) - 1, "%lu", startTime);
	result += buf;
	result += ":";

	snprintf(buf, sizeof(buf) - 1, "%lu", endTime);
	result += buf;
	result += ":";

	if (info->ChannelName()) {
		result += MapSpecialChars(info->ChannelName());
	} else {
		result += "<unknown>";
	}
	result += ":";

	if (info->Title()) {
		result += MapSpecialChars(info->Title());
#if APIVERSNUM >= 10705
	} else if (event->Title()) {
		result += MapSpecialChars(event->Title());
#endif
	} else {
		result += "<unknown>";
	}
	result += ":";

	result += MapSpecialChars(info->ShortText() ?  info->ShortText() : "");
	result += ":";

	result += MapSpecialChars(info->Description() ? info->Description() : "");
	result += ":";

	result += MapSpecialChars(recording->FileName());
	result += ":";

	snprintf(buf, sizeof(buf) - 1, "%d", DirSizeMB(recording->FileName()));
	result += buf;

	result += ":";
	result += MapSpecialChars(info->ChannelID().ToString());

	result += ":";

	int length = RecordingLengthInSeconds(recording);

	snprintf(buf, sizeof(buf) - 1, "%d", length);
	result += buf;

	result += "\r\n";
	return result;
}

string cHelpers::ToText(cTimer * timer) {

	const cChannel * channel = timer->Channel();
	const char * channelName = channel->Name();

	//cSchedulesLock schedulesLock;
	//  const cSchedules * schedules = cSchedules::Schedules(schedulesLock);

	//  const cSchedule * schedule = schedules->GetSchedule(channel->GetChannelID());

	//const cList<cEvent> * events = schedule->Events();
	//  cEvent * match = NULL;
	//  for(cEvent * event = events->First(); event; event = events->Next(event)) {
	//
	//time_t startTime = event->StartTime();
	//    time_t stopTime = startTime + event->Duration();
	//if(startTime <= timer->StartTime() && timer->StopTime() >= stopTime){
	//  match = event;
	//  break;
	//}
	//  }

	string result;
	char buf[100];
	snprintf(buf, sizeof(buf) - 1, "T%d", timer->Index());
	result = buf;
	result += ":";
	snprintf(buf, sizeof(buf) - 1, "%u", timer->Flags());
	result += buf;
	result += ":";
	snprintf(buf, sizeof(buf) - 1, "%d", timer->Channel()->Number());
	result += buf;
	result += ":";
	result += MapSpecialChars(channelName);
	result += ":";
	snprintf(buf, sizeof(buf) - 1, "%lu", timer->StartTime());
	result += buf;
	result += ":";
	snprintf(buf, sizeof(buf) - 1, "%lu", timer->StopTime());
	result += buf;
	result += ":";
	snprintf(buf, sizeof(buf) - 1, "%d", timer->Priority());
	result += buf;
	result += ":";
	snprintf(buf, sizeof(buf) - 1, "%d", timer->Lifetime());
	result += buf;
	result += ":";
	result += MapSpecialChars(timer->File());
	result += ":";
	result += MapSpecialChars(timer->Aux() ? timer->Aux() : "");
	const cEvent * event = timer->Event();
	dsyslog("[vdrmanager] timer's event is NULL. Try find it");
	if (!event) {
		cChannel * channel = Channels.GetByChannelID(
				timer->Channel()->GetChannelID());
		if (channel) {
			cSchedulesLock schedulesLock;
			const cSchedules * schedules = cSchedules::Schedules(schedulesLock);
			const cSchedule * schedule = schedules->GetSchedule(
					channel->GetChannelID());
			if (schedule) {
				const cList<cEvent> * events = schedule->Events();
				for (cEvent * ev = events->First(); event;
						ev = events->Next(ev)) {
					if (timer->StartTime() <= ev->StartTime()
							&& timer->StopTime()
									>= ev->StartTime() + ev->Duration()) {
						event = ev;
						dsyslog(
								"[vdrmanager] timer's event was NULL, but found via time!");
						break;
					}
				}
			}
		}
	}

	if (event) {
		result += ":";
		result += event->ShortText() ? MapSpecialChars(event->ShortText()) : "";
		result += ":";
		result +=
				event->Description() ?
						MapSpecialChars(event->Description()) : "";
	} else {
		result += "::";
	}

	result += ":";
	result += MapSpecialChars(timer->Channel()->GetChannelID().ToString());

	result += ":";

	result += ConvertWeekdays(timer->WeekDays());

	result += "\r\n";

	return result;
}

string cHelpers::ToText(const cEvent * event) {

	cChannel * channel = Channels.GetByChannelID(
			event->Schedule()->ChannelID());

	// search assigned timer
	cTimer * eventTimer = NULL;
	for (cTimer * timer = Timers.First(); timer; timer = Timers.Next(timer)) {
		if (timer->Channel() == channel
				&& timer->StartTime() <= event->StartTime()
				&& timer->StopTime()
						>= event->StartTime() + event->Duration()) {
			eventTimer = timer;
		}
	}

	char buf[100];
	string result;
	snprintf(buf, sizeof(buf) - 1, "E%d", channel->Number());
	result = buf;
	result += ":";
	result += MapSpecialChars(channel->Name());
	result += ":";
	snprintf(buf, sizeof(buf) - 1, "%lu", event->StartTime());
	result += buf;
	result += ":";
	snprintf(buf, sizeof(buf) - 1, "%lu",
			event->StartTime() + event->Duration());
	result += buf;
	result += ":";
	result += MapSpecialChars(event->Title());
	result += ":";
	result += MapSpecialChars(event->Description() ? event->Description() : "");
	result += ":";
	result += MapSpecialChars(event->ShortText() ? event->ShortText() : "");
	result += ":";
	result += MapSpecialChars(channel->GetChannelID().ToString());
	result += "\r\n";

	if (eventTimer) {
		result += ToText(eventTimer);
	}

	return result;
}

bool cHelpers::IsWantedEvent(cEvent * event, string pattern) {

	string text = event->Title();
	if (event->Description()) {
		text += event->Description();
	}

	return ToLower(text).find(ToLower(pattern)) != string::npos;
}

bool cHelpers::IsWantedChannel(cChannel * channel, string wantedChannels) {

	if (!channel) {
		return false;
	}

	if (wantedChannels.length() == 0) {
		return true;
	}

	int number = channel->Number();
	const char * delims = ",;";
	char * state;
	char * buffer = (char *) malloc(wantedChannels.size() + 1);
	strcpy(buffer, wantedChannels.c_str());

	bool found = false;
	for (char * token = strtok_r(buffer, delims, &state); token; token =
			strtok_r(NULL, delims, &state)) {
		const char * rangeSep = strchr(token, '-');
		if (rangeSep == NULL) {
			// single channel
			if (atoi(token) == number) {
				found = true;
			}
		} else {
			// channel range
			int start = atoi(token);
			while (*rangeSep && *rangeSep == '-')
				rangeSep++;
			int end = *rangeSep ? atoi(rangeSep) : INT_MAX;

			if (start <= number && number <= end) {
				found = true;
			}
		}
	}
	return found;
}

bool cHelpers::IsWantedTime(time_t when, cEvent * event) {

	time_t startTime = event->StartTime();
	time_t stopTime = startTime + event->Duration();

	if (when == 0) {
		return stopTime >= time(0);
	}

	return startTime <= when && when < stopTime;
}

string cHelpers::ToUpper(string text) {
	for (unsigned i = 0; i < text.length(); i++) {
		if (islower(text[i]))
			text[i] = toupper(text[i]);
	}

	return text;
}

string cHelpers::ToLower(string text) {
	for (unsigned i = 0; i < text.length(); i++) {
		if (isupper(text[i]))
			text[i] = tolower(text[i]);
	}

	return text;
}

string cHelpers::Trim(string str) {
	int a = str.find_first_not_of(" \t");
	int b = str.find_last_not_of(" \t");
	if (a == -1)
		a = 0;
	if (b == -1)
		b = str.length() - 1;
	return str.substr(a, (b - a) + 1);
}

string cHelpers::SafeCall(string (*f)()) {
	// loop, if vdr modified list and we crash
	for (int i = 0; i < 3; i++) {
		try {
			return f();

		} catch (exception &ex) {
			esyslog("[vdrmanager] catch an exception: %s", ex.what());
		} catch (...) {
			esyslog("[vdrmanager] catch an exception");
			usleep(100);
		}
	}

	return "";
}

string cHelpers::SafeCall(string (*f)(string arg), string arg) {
	// loop, if vdr modified list and we crash
	for (int i = 0; i < 3; i++) {
		try {
			return f(arg);
		} catch (const exception& ex) {
			esyslog("[vdrmanager] catch an exception 1: %s", ex.what());
		} catch (...) {
			esyslog("[vdrmanager] catch an exception 1");
			usleep(100);
		}
	}

	return "";
}

string cHelpers::SafeCall(string (*f)(string arg1, string arg2), string arg1,
		string arg2) {
	// loop, if vdr modified list and we crash
	for (int i = 0; i < 3; i++) {
		try {
			return f(arg1, arg2);
		} catch (exception &ex) {
			esyslog("[vdrmanager] catch an exception 2: %s", ex.what());
		} catch (...) {
			esyslog("[vdrmanager] catch an exception 2");
			usleep(100);
		}
	}

	return "";
}
string cHelpers::MapSpecialChars(cString text) {

	return MapSpecialChars((const char *) text);
}

string cHelpers::MapSpecialChars(const string text) {
	return MapSpecialChars(text.c_str());
}

string cHelpers::MapSpecialChars(const char * p) {

	//const char * p = text.c_str();
	string result = "";
	while (*p) {
		switch (*p) {
		case ':':
			result += "|##";
			break;
		case '\r':
			break;
		case '\n':
			result += "||#";
			break;
		default:
			result += *p;
			break;
		}
		p++;
	}
	return result;
}

//from live plugin StringReplace
string cHelpers::replaceAll(string const& text, string const& substring,
		string const& replacement) {
	string result = text;
	string::size_type pos = 0;
	while ((pos = result.find(substring, pos)) != string::npos) {
		result.replace(pos, substring.length(), replacement);
		pos += replacement.length();
	}
	return result;
}

string cHelpers::UnMapSpecialChars(string text) {

	string ntext = replaceAll(text, "|##", ":");
	ntext = replaceAll(ntext, "||#", "\n");

	return ntext;
}

/**
 * based on vdr-restfulapi's RecordingLengthInSeconds
 */
int cHelpers::RecordingLengthInSeconds(cRecording* recording) {

int nf = -1;
#if APIVERSNUM >= 10721
 nf = recording->NumFrames();
#endif

#if APIVERSNUM <= 10720
	struct tIndexTs {
	    uint64_t offset:40;
	    int reserved:7;
	    int independent:1;
	    uint16_t number:16;
	    tIndexTs(off_t Offset, bool Independent, uint16_t Number) {
		offset = Offset;
		reserved = 0;
		independent = Independent;
		number = Number;
	}
	};
 
	struct stat buf;
#if APIVERSNUM >= 10703
	cString fullname = cString::sprintf("%s%s", recording->FileName(), recording->IsPesRecording() ? "/index" ".vdr" : "/index");
#else
        cString fullname = cString::sprintf("%s%s", recording->FileName(), "/index" ".vdr");
#endif
        
	if (recording->FileName() && *fullname && access(fullname, R_OK) == 0 && stat(fullname, &buf) == 0)
	    nf = buf.st_size ? (buf.st_size - 1) / sizeof(tIndexTs) + 1 : 0;
#endif

if(nf == -1){
	return -1;
}

#if APIVERSNUM >= 10703
	return int(((double) nf / recording->FramesPerSecond()));
#else
	return int((double)nf / FRAMESPERSEC);
#endif

}

/** Compress a STL string using zlib with given compression level and return
 * the binary data. */
string cHelpers::compress_string(const string& str, int compressionlevel) {
	z_stream zs; // z_stream is zlib's control structure
	memset(&zs, 0, sizeof(zs));

	if (deflateInit(&zs, compressionlevel) != Z_OK)
		throw(runtime_error("deflateInit failed while compressing."));

	zs.next_in = (Bytef*) str.data();
	zs.avail_in = str.size(); // set the z_stream's input

	int ret;
	char outbuffer[32768];
	string outstring;

	// retrieve the compressed bytes blockwise
	do {
		zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
		zs.avail_out = sizeof(outbuffer);

		ret = deflate(&zs, Z_FINISH);

		if (outstring.size() < zs.total_out) {
			// append the block to the output string
			outstring.append(outbuffer, zs.total_out - outstring.size());
		}
	} while (ret == Z_OK);

	deflateEnd(&zs);

	if (ret != Z_STREAM_END) { // an error occurred that was not EOF
		ostringstream oss;
		oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
		throw(runtime_error(oss.str()));
	}

	return outstring;
}

/** Decompress an STL string using zlib and return the original data. */
string cHelpers::decompress_string(const string& str) {
	z_stream zs; // z_stream is zlib's control structure
	memset(&zs, 0, sizeof(zs));

	if (inflateInit(&zs) != Z_OK)
		throw(runtime_error("inflateInit failed while decompressing."));

	zs.next_in = (Bytef*) str.data();
	zs.avail_in = str.size();

	int ret;
	char outbuffer[32768];
	string outstring;

	// get the decompressed bytes blockwise using repeated calls to inflate
	do {
		zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
		zs.avail_out = sizeof(outbuffer);

		ret = inflate(&zs, 0);

		if (outstring.size() < zs.total_out) {
			outstring.append(outbuffer, zs.total_out - outstring.size());
		}

	} while (ret == Z_OK);

	inflateEnd(&zs);

	if (ret != Z_STREAM_END) { // an error occurred that was not EOF
		ostringstream oss;
		oss << "Exception during zlib decompression: (" << ret << ") "
				<< zs.msg;
		throw(runtime_error(oss.str()));
	}

	return outstring;
}

//These three methodes were stolen from vdr-restfulapi project. Thanks!
std::queue<int> cHelpers::ConvertToBinary(int v) {
	int b;
	std::queue<int> res;

	while (v != 0) {
		b = v % 2;
		res.push(b);
		v = (v - b) / 2;
	}
	return res;
}

std::string cHelpers::ConvertWeekdays(int v) {
	std::queue<int> b = cHelpers::ConvertToBinary(v);
	int counter = 0;
	std::ostringstream res;
	while (!b.empty() && counter < 7) {
		int val = b.front();
		switch (counter) {
		case 0:
			res << (val == 1 ? 'M' : '-');
			break;
		case 1:
			res << (val == 1 ? 'T' : '-');
			break;
		case 2:
			res << (val == 1 ? 'W' : '-');
			break;
		case 3:
			res << (val == 1 ? 'T' : '-');
			break;
		case 4:
			res << (val == 1 ? 'F' : '-');
			break;
		case 5:
			res << (val == 1 ? 'S' : '-');
			break;
		case 6:
			res << (val == 1 ? 'S' : '-');
			break;
		}
		b.pop();
		counter++;
	}
	while (counter < 7) {
		res << '-';
		counter++;
	}
	return res.str();
}

int cHelpers::ConvertWeekdays(std::string v) {
	const char* str = v.c_str();
	int res = 0;
	if (str[0] == 'M')
		res += 64;
	if (str[1] == 'T')
		res += 32;
	if (str[2] == 'W')
		res += 16;
	if (str[3] == 'T')
		res += 8;
	if (str[4] == 'F')
		res += 4;
	if (str[5] == 'S')
		res += 2;
	if (str[6] == 'S')
		res += 1;
	return res;
}
