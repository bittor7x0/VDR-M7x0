#include <vdr/plugin.h>
#include <vdr/epg.h>
#include <vdr/channels.h>
#include <vdr/skins.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "i18n.h"
#include "setup.h"
#include "thread.h"

#define EPGSYNC_SLEEPMS 30

void cEpgSyncThread::Action() {
	SetPriority(15);

	plugin = cPluginManager::GetPlugin("svdrpservice");
	if (!plugin) {
		esyslog("EpgSync: Plugin svdrpservice not available");
		return;
	}

	svdrp.handle = -1;

	for (int i = 0; svdrp.handle < 0 && i < EpgSyncSetup.connectAttempts && Running(); i++) {
		if (i > 0)
			cCondWait::SleepMs((1 << i) * 1000);
		svdrp.serverIp = EpgSyncSetup.serverIp;
		svdrp.serverPort = EpgSyncSetup.serverPort;
		svdrp.shared = EpgSyncSetup.channelByChannel;
		plugin->Service("SvdrpConnection-v1.0", &svdrp);
	}

	if (svdrp.handle < 0) {
		Skins.QueueMessage(mtError, tr("EpgSync: Unable to connect to Server"));
		return;
	}

	FILE *f = tmpfile();
	if (!f) {
		esyslog("EpgSync: Unable to open temporary file");
		plugin->Service("SvdrpConnection-v1.0", &svdrp);
		return;
	}

	// Get now and next
	if (EpgSyncSetup.nowNext && CmdLSTE(f, "now") && CmdLSTE(f, "next")) {
		AddSchedule(f);
	}

	if (EpgSyncSetup.channelByChannel) {
		// Get channel by channel
		cSchedulesLock *lock = NULL;
		for (cChannel *channel = Channels.First(); channel;
				channel = Channels.Next(channel)) {
			if (!lock)
				lock = new cSchedulesLock();
			if (cSchedules::Schedules(*lock)->GetSchedule(channel)) {
				DELETENULL(lock);
				if (CmdLSTE(f, *channel->GetChannelID().ToString())) {
					AddSchedule(f);
				}
				cCondWait::SleepMs(EPGSYNC_SLEEPMS);
			}
		}
		DELETENULL(lock);
	}
	else {
		// Get complete epg
		if (CmdLSTE(f))
			AddSchedule(f);
	}
	fclose(f);

	plugin->Service("SvdrpConnection-v1.0", &svdrp);
	cSchedules::Cleanup(true);
}

bool cEpgSyncThread::CmdLSTE(FILE *f, const char *Arg) {
	SvdrpCommand_v1_0 cmd;
	cmd.command = Arg ? cString::sprintf("LSTE %s\r\n", Arg) : "LSTE\r\n";
	cmd.handle = svdrp.handle;

	if (!Running())
		return false;

	plugin->Service("SvdrpCommand-v1.0", &cmd);
	cLine *line = cmd.reply.First();
	if (cmd.responseCode != 215 || !line) {
	        esyslog("EpgSync: LSTE error %hu %s", cmd.responseCode,
				line ? line->Text() : "");
		return false;
	}
	while (cmd.reply.Next(line)) {
		if (fputs(line->Text(), f) < 0 || fputs("\n", f) < 0) {
			LOG_ERROR;
			return false;
		};
		line = cmd.reply.Next(line);
	}
	return Running();
}

void cEpgSyncThread::AddSchedule(FILE *f) {
	rewind(f);

	if (!cSchedules::Read(f))
		esyslog("EpgSync: Error parsing EPG data");

	rewind(f);
	if (ftruncate(fileno(f), 0) < 0) {
		LOG_ERROR;
	}
}

cEpgSyncThread::cEpgSyncThread(): cThread("epgsync") {
	plugin = NULL;
}

cEpgSyncThread::~cEpgSyncThread() {
	Cancel(5);
}
