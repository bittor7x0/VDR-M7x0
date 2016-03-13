#ifndef __streamplayer_udpplayer_h__
#define __streamplayer_udpplayer_h__

#include "player.h"

//
// cDataStreamSocketUDP
//

class cDataStreamSocketUDP : public cDataStreamSource {
public:
	cDataStreamSocketUDP(char *address, uint16_t port);
	virtual ~cDataStreamSocketUDP();
	virtual uchar* Get(int &Count);
	virtual void Del(int Count);
	virtual void Reset();
	virtual int GetPollHandle();
	
protected:
	char *Address;
	uint16_t Port;

	int sock;
	uchar SrcBuffer[20480];
	uchar *pSrcBuffer;
	int nSrcBuffer;

	bool Open();
	void Close();
};

//
// cStreamPlayerUDP
//

class cStreamPlayerUDP : public cStreamPlayer {
protected:
  uint16_t port;
  char *address;
  char StatusString[80];

public:
  cStreamPlayerUDP(char *url);
  virtual ~cStreamPlayerUDP();
  void ParseURL(char *url);
  virtual cDataStreamSource* NewSource();
  virtual void DeleteSource(cDataStreamSource *src);
  virtual char* GetStatus();
};



#endif
