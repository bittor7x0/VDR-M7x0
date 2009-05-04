#include "aideplayer.h"
#include <linux/dvb/video.h>
#include <vdr/plugin.h>

cAidePlayer::cAidePlayer(void)
{
}

cAidePlayer::~cAidePlayer(void)
{
}

void cAidePlayer::Activate(bool On)
{
	if (On)
	{
		struct video_still_picture sp;
		int filefd;
		struct stat st;
		ssize_t nRead;
		if ( (filefd = open(Image,O_RDONLY)) >=0) {
			fstat(filefd, &st);
			if(st.st_size > 0){
				sp.iFrame = (char *)malloc(st.st_size);
				sp.size = st.st_size;	
				if(sp.iFrame) {
					nRead = read(filefd, sp.iFrame, sp.size);
					uchar *buffer;
					buffer = (uchar *) sp.iFrame;
					//for (int i = 1; i <= 25; ++i)
					//	  send_pes_packet(buffer, sp.size, i);
					DeviceStillPicture(buffer,sp.size);
				}
			}
		}
		else Detach();
	}
}


cAidePlayerControl::cAidePlayerControl(const char* AideFile) : cControl(player = new cAidePlayer)
{
	asprintf(&(player->Image),"%s",AideFile);
}

cAidePlayerControl::~cAidePlayerControl(void)
{
	delete player;
}

void cAidePlayerControl::Hide(void)
{
}

eOSState cAidePlayerControl::ProcessKey(eKeys Key)
{
	eOSState state = cControl::ProcessKey(Key);
	if (!player->IsAttached()) return osEnd;
	switch(Key)
	{
		case kBack : 
			     //delete player;
			     return osEnd;
			     break;
		default: break;
	}
	return state;
}

void cAidePlayerControl::Shutdown()
{
}
