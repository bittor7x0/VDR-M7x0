#ifndef _MEDIA_SERVICE_H
#define _MEDIA_SERVICE_H

#include <vdr/plugin.h>

/* flags for MediaManager_Status_v1_0 */
#define MEDIA_STATUS_REPLAY 	0x01
#define MEDIA_STATUS_EJECTDISC	0x02
#define MEDIA_STATUS_ACTIVE 	0x04
#define MEDIA_STATUS_CRASHED	0x08

#define MEDIA_MANAGER_REGISTER_ID	"MediaManager-Register-v1.0"
#define MEDIA_MANAGER_ACTIVATE_ID	"MediaManager-Activate-v1.0"
#define MEDIA_MANAGER_MAINMENU_ID	"MediaManager-Mainmenu-v1.0"
#define MEDIA_MANAGER_STATUS_ID 	"MediaManager-Status-v1.0"
#define MEDIA_MANAGER_PLUGIN		"mediamanager"

enum eMediaType {
	mtInvalid	= -1,
	mtNoDisc	= 0,
	mtBlank 	= 1,
	mtAudio 	= 2,
	mtVideoDvd	= 3,
	mtSvcd		= 4,
	mtVcd		= 5,
	mtData		= 6,
	mtVDRdata	= 7,
	mtAudioData	= 8,
	mtImageData = 9,
	mtExternUse = 10
};

struct MediaManager_Register_v1_0 {
	const char *PluginName;
	const char *MainMenuEntry;
	const char *Description;
	eMediaType mediatype;
	bool shouldmount;
};

struct MediaManager_Status_v1_0 {
	bool set;
	int flags;
	bool isreplaying;
	bool ejectdisc;
	bool active;		/* read only */
	bool crashed;
};

struct MediaManager_Activate_v1_0 {
	bool on;
	const char *device_file;
	const char *mount_point;
};

struct MediaManager_Mainmenu_v1_0 {
	cOsdObject *mainmenu;
};

class cMediaService {
  private:
	static cPlugin *manager;
	static bool active;
	static bool replaying;
  public:
	cMediaService(void);
	~cMediaService(void);

	void SetManager(cPlugin *Manager);

	static bool HaveManager(void);
	static void SetReplaying(bool Replaying);
	static void SetActive(bool Active);
	static bool IsReplaying(void);
	static bool IsActive(void);
	static void EjectDisc(void);
};
#endif
