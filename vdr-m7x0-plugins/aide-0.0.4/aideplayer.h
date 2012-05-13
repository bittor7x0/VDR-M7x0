#ifndef __AIDEPLAYER_H
#define __AIDEPLAYER_H
#include <vdr/player.h>

class cAidePlayer : public cPlayer {
	protected:
		virtual void Activate(bool On);
	public:
		cAidePlayer(void);
		virtual ~cAidePlayer(void);
		void send_pes_packet(unsigned char *data, int len, int timestamp);
		char *Image;
};

class cAidePlayerControl : public cControl {
	private :
		cAidePlayer *player;
	public : 
		cAidePlayerControl(const char *AideFile);
		virtual ~cAidePlayerControl(void);
		virtual void Hide(void);
		void Shutdown(void);
		virtual eOSState ProcessKey(eKeys Key);
};
#endif
