#include <stdio.h>
#include <stdlib.h>
#include <vdr/thread.h>
#include <vdr/remote.h>
#include <vdr/skins.h>
#include <vdr/themes.h>
#include <vdr/interface.h>
#include <vdr/status.h>
#include "i18n.h"
#include "menu.h"
#include "setup.h"

#define SVDRP_SERVER_TIMEOUT_MS 10
#define CMD_PLUG "PLUG svdrpext "
#define RC_OK 920
#define RC_NA 930

cRemoteOsdMenu::cRemoteOsdMenu(const char *Title): cOsdMenu(Title) {
	plugin = cPluginManager::GetPlugin("svdrpservice");
	title = message = text = NULL;
	red = green = yellow = blue = NULL;
	isEmpty = true;
	if (*RemoteOsdSetup.remoteTheme)
		cThemes::Load(Skins.Current()->Name(), RemoteOsdSetup.remoteTheme, Skins.Current()->Theme());
}

cRemoteOsdMenu::~cRemoteOsdMenu() {
	if (plugin && svdrp.handle >= 0) {
		// probably MENU key has been hit - close remote menu
		if (!isEmpty)
			CmdHITK("MENU");
		// close SVDRP connection
		plugin->Service("SvdrpConnection-v1.0", &svdrp);
	}
	free(title);
	free(message);
	free(text);
	free(red);
	free(green);
	free(yellow);
	free(blue);
	if (*RemoteOsdSetup.remoteTheme)
		cThemes::Load(Skins.Current()->Name(), Setup.OSDTheme, Skins.Current()->Theme());
}

bool cRemoteOsdMenu::Open(const char *ServerIp, unsigned short ServerPort, const char *Key) {
	if (!plugin)
		return false;

	// Get the connection
	svdrp.serverIp = ServerIp;
	svdrp.serverPort = ServerPort;
	svdrp.shared = true;
	svdrp.handle = -1;
	plugin->Service("SvdrpConnection-v1.0", &svdrp);
	if (svdrp.handle < 0)
		return false;

	// Close remote OSD if necessary
	int state;
	for (int i = 1; (state = CheckState()) != RC_NA; i++) {
		if (state != RC_OK) // error occured
			return false;
		if (i == 1 && !Interface->Confirm(tr("Remote OSD already in use. Proceed anyway?")))
			return false;
		else if (i > 2) {
			esyslog("RemoteOsdMenu: Cannot close remote menu");
			return false;
		}
		if (!CmdHITK("MENU"))
			return false;
	}

	if (RemoteOsdSetup.tuneServer) {
		cChannel *channel = Channels.GetByNumber(cDevice::CurrentChannel());
		CmdCHAN(channel);
	}

	// Now open the remote menu
	if (CmdHITK(Key) && CheckState() == RC_OK)
		return UpdateMenu();
	return false;
}

int cRemoteOsdMenu::CheckState() {
	SvdrpCommand_v1_0 cmd;
	// assume the OSD is empty if there is no title
	cmd.command = cString(CMD_PLUG "OSDT\r\n");
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);
	return cmd.responseCode;
}

bool cRemoteOsdMenu::CmdCHAN(cChannel *Channel) {
	SvdrpCommand_v1_0 cmd;
	cmd.command = cString::sprintf("CHAN %s\r\n", *Channel->GetChannelID().ToString());
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);
	return cmd.responseCode == 250;
}

bool cRemoteOsdMenu::CmdHITK(const char* Key) {
	SvdrpCommand_v1_0 cmd;
	cmd.command = cString::sprintf("HITK %s\r\n", Key);
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);

	// A timeout in cFile::AnyFileReady defines how long the
	// server will now wait for more SVDRP commands before
	// it actually processes our HITK command
	cCondWait::SleepMs(SVDRP_SERVER_TIMEOUT_MS);

	return cmd.responseCode == 250;
}

bool cRemoteOsdMenu::CmdOSDT() {
	SvdrpCommand_v1_0 cmd;
	cLine *line;

	cmd.command = cString(CMD_PLUG "OSDT\r\n");
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);

	switch (cmd.responseCode) {
		case RC_OK:
			line = cmd.reply.First();
			asprintf(&title, "%s: %s", *svdrp.serverIp, line->Text());
			isEmpty = false;
			break;
		case RC_NA:
			title = strdup(svdrp.serverIp);
			break;
		default:
			return false;
	}
	SetTitle(title);
	return true;
}

bool cRemoteOsdMenu::CmdOSDI() {
	int maxItems = RemoteOsdSetup.maxItems;
	// special value 0: use local maxItems
	if (maxItems == 0)
		maxItems = DisplayMenu()->MaxItems();

	SvdrpCommand_v1_0 cmd;
	cmd.command = cString::sprintf(CMD_PLUG "OSDI %d\r\n", maxItems);
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);

	int cols[cSkinDisplayMenu::MaxTabs];
	int currentCol = 0;
	memset(&cols, 0, sizeof(cols));

	switch (cmd.responseCode) {
		case RC_OK:
			for (cLine *line = cmd.reply.First(); line; line = cmd.reply.Next(line)) {
				const char *s = line->Text();
				if (strlen(s) > 2) {
					switch (*s) {
						case 'C': if (currentCol < cSkinDisplayMenu::MaxTabs)
								  cols[currentCol++] = strtol(s + 2, NULL, 10);
							  break;
						case 'I': Add(new cOsdItem(s + 2)); break;
						case 'S': Add(new cOsdItem(s + 2), true); break;
					}
				}
			}
			SetCols(cols[0], cols[1], cols[2], cols[3], cols[4]);
			break;
		case RC_NA:
			break;
		default:
			return false;
	}
	return true;
}

bool cRemoteOsdMenu::CmdOSDH() {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString(CMD_PLUG "OSDH\r\n");
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);

	switch (cmd.responseCode) {
		case RC_OK:
			for (cLine *line = cmd.reply.First(); line; line = cmd.reply.Next(line)) {
				const char *s = line->Text();
				if (strlen(s) > 2) {
					char *text = strdup(s + 2);
					switch (*s) {
						case 'R': red = text; break;
						case 'G': green = text; break;
						case 'Y': yellow = text; break;
						case 'B': blue = text; break;
					}
				}
			}
			break;
		case RC_NA:
			break;
		default:
			return false;
	}
	SetHelp(red, green, yellow, blue);
	return true;
}

bool cRemoteOsdMenu::CmdOSDM() {
	SvdrpCommand_v1_0 cmd;
	cLine *line;

	cmd.command = cString(CMD_PLUG "OSDM\r\n");
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);

	switch (cmd.responseCode) {
		case RC_OK:
			line = cmd.reply.First();
			asprintf(&message, "%s: %s", *svdrp.serverIp, line->Text());
			break;
		case RC_NA:
			break;
		default:
			return false;
	}
	SetStatus(message);
	return true;
}

bool cRemoteOsdMenu::CmdOSDX() {
	SvdrpCommand_v1_0 cmd;
	cLine *line;

	cmd.command = cString(CMD_PLUG "OSDX\r\n");
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);

	switch (cmd.responseCode) {
		case RC_OK:
			line = cmd.reply.First();
			text = strdup(line->Text());
			strreplace(text, '|', '\n');
			break;
		case RC_NA:
			break;
		default:
			return false;
	}
	return true;
}

bool cRemoteOsdMenu::UpdateMenu() {
	Clear();

	if (!CmdOSDT() || !CmdOSDM() || !CmdOSDH() || !CmdOSDI())
		return false;
	// Check for text only if no item is shown
	if (Count() == 0 && !CmdOSDX())
		return false;

	Display();
	return true;
}

void cRemoteOsdMenu::Clear() {
	cOsdMenu::Clear();
	free(title);
	free(message);
	free(text);
	free(red);
	free(green);
	free(yellow);
	free(blue);
	title = message = text = NULL;
	red = green = yellow = blue = NULL;
	isEmpty = true;
}

void cRemoteOsdMenu::Display(void)
{
	cOsdMenu::Display();
	if (text) {
		DisplayMenu()->SetText(text, true);
		cStatus::MsgOsdTextItem(text);
	}
}

eOSState cRemoteOsdMenu::ProcessKey(eKeys Key) {
	if (Key == kNone)
		return osContinue;	//TODO: Update menu e.g. once per second

	if (Key & k_Release)
		return osContinue;

	if (text) {
		switch (Key) {
			case kUp|k_Repeat:
			case kUp:
			case kDown|k_Repeat:
			case kDown:
			case kLeft|k_Repeat:
			case kLeft:
			case kRight|k_Repeat:
			case kRight:
				DisplayMenu()->Scroll(NORMALKEY(Key) == kUp || NORMALKEY(Key) == kLeft, NORMALKEY(Key) == kLeft || NORMALKEY(Key) == kRight);
				cStatus::MsgOsdTextItem(NULL, NORMALKEY(Key) == kUp);
				return osContinue;
			default: break;
		}
	}
	if (CmdHITK(cKey::ToString(RAWKEY(Key))) && UpdateMenu())
		return isEmpty ? osBack : osContinue;
	return osBack;
}
