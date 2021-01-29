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
#include <vdr/cutter.h>
#include <vdr/sources.h>
#include <fstream>
#include "epgsearch/services.h"

#define TIMER_SEP		"#|#|#"
// Taken from vdr-ive
#define INDEXFILESUFFIX   "/index.vdr"
#define LENGTHFILESUFFIX  "/length.vdr"

#ifdef DEF_LIST_LOCK

#define READ_LOCK_BASE(Class, Define) LOCK_##Define##_READ; const c##Class * the##Class = Class
#define WRITE_LOCK_BASE(Class, Define) LOCK_##Define##_WRITE; c##Class * the##Class = Class

#define READ_LOCK_TIMERS READ_LOCK_BASE(Timers, TIMERS)
#define WRITE_LOCK_TIMERS WRITE_LOCK_BASE(Timers, TIMERS)

#define READ_LOCK_RECORDINGS READ_LOCK_BASE(Recordings, RECORDINGS)
#define WRITE_LOCK_RECORDINGS WRITE_LOCK_BASE(Recordings, RECORDINGS)

#define READ_LOCK_CHANNELS READ_LOCK_BASE(Channels, CHANNELS)
#define WRITE_LOCK_CHANNELS WRITE_LOCK_BASE(Channels, CHANNELS)

#define READ_LOCK_SCHEDULES READ_LOCK_BASE(Schedules, SCHEDULES)
#define WRITE_LOCK_SCHEDULES WRITE_LOCK_BASE(Schedules, SCHEDULES)

#define RECORDING_CONTROLS_PROCESS cRecordControls::Process(theTimers, time(NULL))
#define LIST_ELEM_CONST const

#else

#define READ_LOCK_BASE(Class) c##Class * the##Class = &Class
#define WRITE_LOCK_BASE(Class) c##Class * the##Class = &Class

#define READ_LOCK_TIMERS READ_LOCK_BASE(Timers)
#define WRITE_LOCK_TIMERS WRITE_LOCK_BASE(Timers)

#define READ_LOCK_RECORDINGS READ_LOCK_BASE(Recordings)
#define WRITE_LOCK_RECORDINGS WRITE_LOCK_BASE(Recordings)

#define READ_LOCK_CHANNELS READ_LOCK_BASE(Channels)
#define WRITE_LOCK_CHANNELS WRITE_LOCK_BASE(Channels)

#define LOCKED_SCHEDULES cSchedulesLock schedulesLock; const cSchedules * theSchedules = cSchedules::Schedules(schedulesLock);
#define READ_LOCK_SCHEDULES  LOCKED_SCHEDULES
#define WRITE_LOCK_SCHEDULES LOCKED_SCHEDULES

#define RECORDING_CONTROLS_PROCESS cRecordControls::Process(time(NULL))
#define LIST_ELEM_CONST

#endif

static char ServiceInterface[] = "Epgsearch-services-v1.1";

string cHelpers::GetRecordings(string args) {
	return SafeCall(GetRecordingsIntern);
}

string cHelpers::GetTimers(string args) {
	return SafeCall(GetTimersIntern, args);
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

string cHelpers::GetTimersIntern(string options) {

	// timer conflicts wanted?
	set<string> conflicts;
	if (options.find("conflicts") != string::npos) {
		conflicts = GetTimerConflicts();
		if (conflicts.empty()) {
			conflicts.insert("");
		}
	}

	string result = "START\r\n";

	// iterate through all timers
	READ_LOCK_TIMERS;
	for (const cTimer * timer = theTimers->First(); timer; timer = theTimers->Next(timer)) {
		result += ToText(timer, conflicts);
	}

	return result + "END\r\n";
}

string cHelpers::GetRecordingsIntern() {

	int FreeMB, UsedMB;
#if APIVERSNUM < 20102
	int Percent = VideoDiskSpace(&FreeMB, &UsedMB);
#else
	int Percent = cVideoDirectory::VideoDiskSpace(&FreeMB, &UsedMB);
#endif

	std::stringstream sstm;

	sstm << "START|" << (FreeMB + UsedMB) << ":" << FreeMB << ":" << Percent
			<< "\r\n";

	string result = sstm.str();

	//iterate through all recordings
	READ_LOCK_RECORDINGS;
	const cRecording* recording = NULL;
	for (int i = 0; i < theRecordings->Count(); i++) {
		recording = theRecordings->Get(i);
		result += ToText(recording);
	}
	return result + "END\r\n";
}

string cHelpers::GetChannelsIntern(string wantedChannels) {

	string result = "START\r\n";
	string currentGroup = "";

	char number[10];
	READ_LOCK_CHANNELS;
	for (const cChannel * channel = theChannels->First(); channel; channel =
			theChannels->Next(channel)) {

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
			result += MapSpecialChars(channel->Name());
			result += ":";
			result += MapSpecialChars(channel->Provider());
			result += ":";
			result += channel->GetChannelID().ToString();
			result += ":";
			result += GetAudioTracks(channel);
			result += ":";
			result += MapSpecialChars(cSource::ToString(channel->Source()));
			result += "\r\n";
		}
	}

	return result + "END\r\n";
}

string cHelpers::SetChannelIntern(const string args) {

	if (args.size() == 0) {
		return "!ERROR:SetChannel;empty args\r\n";
	}

	const cChannel *channel;
	bool isnum = true;
	for (int i = 0; i < (int) args.length(); i++) {
		if (!std::isdigit(args[i])) {
			isnum = false;
			break;
		}

	}

	READ_LOCK_CHANNELS;
	if (isnum) {
		int nr = atoi(args.c_str());
		channel = theChannels->GetByNumber(nr);
	} else {
		tChannelID chid = tChannelID::FromString(args.c_str());
		channel = theChannels->GetByChannelID(chid);
	}

	if (!channel) {
		return "!ERROR:SetChannel;Unable to find channel " + args + "\r\n";
	}

	ostringstream outStream;

	if (!cDevice::PrimaryDevice()->SwitchChannel(channel, true)) {
		outStream << channel->Number();
		return "!ERROR:SetChannel;Error switching to channel " + outStream.str()
				+ "\r\n";
	}

	return "START\r\n" + MapSpecialChars(channel->GetChannelID().ToString())
			+ "\r\nEND\r\n";
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
			sep = "|";
		}

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

	READ_LOCK_SCHEDULES;
	READ_LOCK_CHANNELS;
	for (const cSchedule * schedule = theSchedules->First(); schedule; schedule =
			theSchedules->Next(schedule)) {

		const cChannel * channel = theChannels->GetByChannelID(schedule->ChannelID());
		if (!IsWantedChannel(channel, wantedChannels)) {
			continue;
		}

		const cList<cEvent> * events = schedule->Events();
		for (const cEvent * event = events->First(); event;
				event = events->Next(event)) {
			if (IsWantedTime(wantedTime, event)) {
				const cEvent * match = event;
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

string cHelpers::DelRecording(cRecording * recording) {

	cString FileName = recording->FileName();

#if VDRVERSNUM < 20102
	if (cCutter::Active(recording->FileName())) {
		cCutter::Stop();
		recording = Recordings.GetByName(FileName); // cCutter::Stop() might have deleted it if it was the edited version
		// we continue with the code below even if recording is NULL,
		// in order to have the menu updated etc.
	}
#else
	if (RecordingsHandler.GetUsage(FileName)) {
		RecordingsHandler.Del(FileName);
		WRITE_LOCK_RECORDINGS;
		recording = theRecordings->GetByName(FileName); // RecordingsHandler.Del() might have deleted it if it was the edited version
		// we continue with the code below even if recording is NULL,
		// in order to have the menu updated etc.
	}
#endif

	if (cReplayControl::NowReplaying()
			&& strcmp(cReplayControl::NowReplaying(), FileName) == 0) {
		cControl::Shutdown();
	}

	WRITE_LOCK_RECORDINGS;
	if (!recording || recording->Delete()) {
		cReplayControl::ClearLastReplayed(FileName);
		theRecordings->DelByName(FileName);
#if VDRVERSNUM > 10727
		cVideoDiskUsage::ForceCheck();
#endif
	}

	return "START\r\nEND\r\n";
}

string cHelpers::DelRecordingIntern(string args) {

	if (args.size() == 0) {
		return Error("empty args");
	}

	int index = atoi(args.c_str());

	cRecording *recording = NULL;
	{
		WRITE_LOCK_RECORDINGS;
		recording = theRecordings->Get(index);
	}
	if (!recording) {
		return Error("Recording not found");
	}

	cRecordControl *rc = cRecordControls::GetRecordControl(
			recording->FileName());

	if (!rc) {
		return cHelpers::DelRecording(recording);
	}

	/**
	 * this should come as a parameter later
	 */

	bool forceDelete = true;

	if (forceDelete == false) {
		return Error("Recording is in use by a timer");
	}

	cTimer *timer = rc->Timer();
	if (timer) {
		timer->Skip();
		WRITE_LOCK_TIMERS;
		RECORDING_CONTROLS_PROCESS;
		if (timer->IsSingleEvent()) {
			isyslog("deleting timer %s", *timer->ToDescr());
			theTimers->Del(timer);
		}
		theTimers->SetModified();
	}
	return cHelpers::DelRecording(recording);
}
//
//	return "START\r\nEND\r\n";
//	if (r->Delete()) {
//		cRecordControl* rc = cRecordControls::GetRecordControl(r->FileName());
//		//cTimer *timer = rc->Timer();
//		//if (timer) {
//		//isyslog("deleting timer %s", *timer->ToDescr());
//		//Timers.sel(timer);
//		Timers.SetModified();
//		//}
//		Recordings.DelByName(r->FileName());
//		return "START\r\nEND\r\n";
//	} else {
//		return "!ERROR:Failed\r\n";
//	}

string cHelpers::SetTimerIntern(char op, string param) {

	switch (op) {
	case 'C': // new timer
	case 'c': {
		auto_ptr<cTimer> newTimer(new cTimer);
		if (!newTimer->Parse(param.c_str())) {
			return Error("Error in timer settings");
		}

		WRITE_LOCK_TIMERS;
		cTimer* checkTimer = theTimers->GetTimer(newTimer.get());
		if (checkTimer) {
			return Error("Timer already defined");
		}

		theTimers->Add(newTimer.get());
		theTimers->SetModified();
		dsyslog("[vdrmanager] timer %s added", *newTimer->ToDescr());
		newTimer.release();
		break;
	}
	case 'D':
	case 'd': {
		dsyslog("[vdrmanager] try parse %s ", param.c_str());

		auto_ptr<cTimer> timer(new cTimer);
		if (!timer->Parse(param.c_str())) {
			return Error("Error in timer settings");
		}

		dsyslog("[vdrmanager] timer %s parsed", *timer->ToDescr());

		WRITE_LOCK_TIMERS;
		cTimer * t = theTimers->GetTimer(timer.get());

		if (!t) {
			return Error("Timer not defined");
		}

		/**
		 * this should come lates as a command line parameter
		 */
		bool forceDelete = true;

		if (t->Recording()) {
			if (forceDelete == true) {
				t->Skip();
				RECORDING_CONTROLS_PROCESS;
			} else {
				return Error("Timer  is recording");
			}
		}

		dsyslog("[vdrmanager] deleting timer %s", *t->ToDescr());
		theTimers->Del(t);
		theTimers->SetModified();
		break;
	}
	case 'M':
	case 'm': {

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

		WRITE_LOCK_TIMERS;
		cTimer* oldTimer = theTimers->GetTimer(otimer.get());
		if (oldTimer == 0) {
			return Error("Timer not defined");
		}

		cTimer copy = *oldTimer;
		if (!copy.Parse(newt.c_str())) {
			return Error("Error in timer settings");
		}

		*oldTimer = copy;
		theTimers->SetModified();
		dsyslog("[vdrmanager] timer %s modified (%s)", *oldTimer->ToDescr(),
				oldTimer->HasFlags(tfActive) ? "active" : "inactive");

		break;
	}
	case 'T':
	case 't': {
		auto_ptr<cTimer> timer(new cTimer);
		if (!timer->Parse(param.c_str())) {
			return Error("Error in timer settings");
		}

		WRITE_LOCK_TIMERS;
		cTimer* toggleTimer = theTimers->GetTimer(timer.get());
		if (toggleTimer == 0) {
			return Error("Timer not defined");
		}

		toggleTimer->OnOff();
		theTimers->SetModified();
		dsyslog("[vdrmanager] timer %s toggled %s", *toggleTimer->ToDescr(),
				toggleTimer->HasFlags(tfActive) ? "on" : "off");
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

	READ_LOCK_SCHEDULES;
	READ_LOCK_CHANNELS;
	for (const cSchedule * schedule = theSchedules->First(); schedule; schedule =
			theSchedules->Next(schedule)) {

		const cChannel * channel = theChannels->GetByChannelID(schedule->ChannelID());
		if (!IsWantedChannel(channel, wantedChannels)) {
			continue;
		}

		const cList<cEvent> * events = schedule->Events();
		for (const cEvent * event = events->First(); event;
				event = events->Next(event)) {

			if (IsWantedTime(0, event) && IsWantedEvent(event, pattern)) { //time must be ok, so stop > now
				result += ToText(event);
			}
		}
	}

	return result + "END\r\n";
}

//copied from vdr-live

long cHelpers::Duration(const cRecording* recording) {
	long RecLength = 0;
	if (!recording->FileName())
		return 0;
#if VDRVERSNUM < 10704
	cString filename = cString::sprintf("%s%s", recording->FileName(), INDEXFILESUFFIX);
	if (*filename) {
		if (access(filename, R_OK) == 0) {
			struct stat buf;
			if (stat(filename, &buf) == 0) {
				struct tIndex {int offset; uchar type; uchar number; short reserved;};
				int delta = buf.st_size % sizeof(tIndex);
				if (delta) {
					delta = sizeof(tIndex) - delta;
					esyslog("ERROR: invalid file size (%ld) in '%s'", buf.st_size, *filename);
				}
				RecLength = (buf.st_size + delta) / sizeof(tIndex) / SecondsToFrames(60);
			}
		}
	}
#elif VDRVERSNUM < 10721
	// open index file for reading only
	cIndexFile *index = new cIndexFile(recording->FileName(), false, recording->IsPesRecording());
	if (index && index->Ok()) {
		RecLength = (int) (index->Last() / SecondsToFrames(60, recording->FramesPerSecond()));
	}
	delete index;
#else
	return recording->LengthInSeconds() / 60;
#endif
	if (RecLength == 0) {
		cString lengthFile = cString::sprintf("%s%s", recording->FileName(),
		LENGTHFILESUFFIX);
		ifstream length(*lengthFile);
		if (length)
			length >> RecLength;
	}

	return RecLength;
}

string cHelpers::ToText(const cRecording * recording) {
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

#if VDRVERSNUM < 10726
	time_t startTime = recording->start;
#else
	time_t startTime = recording->Start();
#endif

	time_t endTime = startTime + RecordingLengthInSeconds(recording);

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

	if(info->Title()){
		result += MapSpecialChars(info->Title());
#if APIVERSNUM >= 10705
	} else if (event->Title()) {
		result += MapSpecialChars(event->Title());
#endif
	} else {
		result += MapSpecialChars(recording->Name());
	}
	result += ":";

	result += MapSpecialChars(info->ShortText() ? info->ShortText() : "");
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

	result += ":";

	struct stat st;
	if (stat(recording->FileName(), &st) == 0) {
		result += MapSpecialChars(
				cString::sprintf("%lu:%llu.rec", (unsigned long) st.st_dev,
						(unsigned long long) st.st_ino));
	} else {
		result += "";
	}

	result += ":";

	cRecordControl *rc = cRecordControls::GetRecordControl(
			recording->FileName());
	if (rc) {
		cTimer *timer = rc->Timer();
		if (timer) {
			char buf[100];
			snprintf(buf, sizeof(buf) - 1, "%lu", timer->StopTime());
			result += buf;
		}
	}

	result += ":";
	//Feature #1319
	result += MapSpecialChars(recording->Name());
	//Feature #1699
	result += ":";
	snprintf(buf, sizeof(buf) - 1, "%d", recording -> IsNew());
	result += buf;
	result += "\r\n";
	return result;
}

string cHelpers::ToText(const cTimer * timer, set<string> conflicts) {

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
	if (!event) {
		dsyslog("[vdrmanager] timer's event is NULL. Try find it");
		READ_LOCK_CHANNELS;
		const cChannel * channel = theChannels->GetByChannelID(
				timer->Channel()->GetChannelID());
		if (channel) {
			READ_LOCK_SCHEDULES;
			const cSchedule * schedule = theSchedules->GetSchedule(
					channel->GetChannelID());
			if (schedule) {
				const cList<cEvent> * events = schedule->Events();
				for (const cEvent * ev = events->First(); event;
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

	if (!conflicts.empty()) {
		ostringstream index;
		index << timer->Index() + 1;
		if (conflicts.find(index.str()) != conflicts.end()) {
			result += ":1";
		} else {
			result += ":0";
		}
	}

	result += ":";
	if (event && event->Vps()) {
		snprintf(buf, sizeof(buf) - 1, "%lu", event->Vps());
		result += buf;
	}

	result += "\r\n";

	return result;
}

string cHelpers::ToText(const cEvent * event) {

	READ_LOCK_CHANNELS;
	LIST_ELEM_CONST cChannel * channel = theChannels->GetByChannelID(
			event->Schedule()->ChannelID());

// search assigned timer

	//eTimerMatch TimerMatch = tmNone;

//	if(eventTimer){
//
//	for (cTimer * timer = Timers.First(); timer; timer = Timers.Next(timer)) {
//		if (timer->Channel() == channel
//				&& timer->StartTime() <= event->StartTime()
//				&& timer->StopTime()
//						>= event->StartTime() + event->Duration()) {
//			eventTimer = timer;
//			break;
//		}
//	}

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
	result += ":";
	result += GetAudioTracks(channel);
	result += ":";

	if (event->Contents(0)) {
		string sep = "";
		for (int i = 0; event->Contents(i); i++) {
			uchar c = event->Contents(i);
			result += sep;
			snprintf(buf, sizeof(buf) - 1, "%u", c);
			result += buf;
			sep = " ";
		}
	}

	result += ":";
	if (event->Vps()) {
		snprintf(buf, sizeof(buf) - 1, "%lu", event->Vps());
		result += buf;
	}

	result += "\r\n";

	READ_LOCK_TIMERS;
	LIST_ELEM_CONST cTimer * eventTimer = theTimers->GetMatch(event);

	if (eventTimer) {
		result += ToText(eventTimer, set<string>());
	}

	return result;
}

bool cHelpers::IsWantedEvent(const cEvent * event, string pattern) {

	string text = event->Title();
	if (event->Description()) {
		text += event->Description();
	}

	return ToLower(text).find(ToLower(pattern)) != string::npos;
}

bool cHelpers::IsWantedChannel(const cChannel * channel, string wantedChannels) {

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
//Bug #1236
	free(buffer);

	return found;
}

bool cHelpers::IsWantedTime(time_t when, const cEvent * event) {

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
int cHelpers::RecordingLengthInSeconds(const cRecording* recording) {
	return Duration(recording) * 60;
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

set<string> cHelpers::GetTimerConflicts() {

	Epgsearch_services_v1_1 service;
	set<string> timers;
	if (cPluginManager::CallFirstService(ServiceInterface, &service)) {
		cServiceHandler_v1_1* handler =
				dynamic_cast<cServiceHandler_v1_1*>(service.handler.get());
		if (handler) {
			list<string> conflicts = service.handler->TimerConflictList();
			for (list<string>::iterator it = conflicts.begin();
					it != conflicts.end(); it++) {
				string line = *it;
				size_t sep = line.find(':');
				line = line.substr(sep + 1);

				while (!line.empty()) {
					sep = line.find(':');
					string conflict;
					if (sep == string::npos) {
						conflict = line;
						line = "";
					} else {
						conflict = line.substr(0, sep);
						line = line.substr(sep + 1);
					}

					sep = conflict.rfind('|');
					conflict = conflict.substr(sep + 1);
					while (!conflict.empty()) {
						sep = conflict.find('#');
						string timer;
						if (sep == string::npos) {
							timer = conflict;
							conflict = "";
						} else {
							timer = conflict.substr(0, sep);
							conflict = conflict.substr(sep + 1);
						}
						timers.insert(timer);
					}
				}
			}
		}
	}

	return timers;
}
