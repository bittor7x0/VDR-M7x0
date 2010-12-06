#ifndef __streamplayer_httpplayer_h__
#define __streamplayer_httpplayer_h__

#include "player.h"

//
// cFilterHTTPHeader
//

class cFilterHTTPHeader : public cDataStreamProcessor {
public:
	enum eState { esStart,esEatHeader,esFinished,esErrorHTTPNotFound,esErrorHTTPResponseFailed };

	cFilterHTTPHeader(cDataStreamSource *Src);
	eState GetState() { return State; }

protected:
	eState State;
	virtual void ProcessData(uchar *data, int Count, int &Forwardable, int &Deletable);
};

//
// cDataStreamSocketTCP
//

class cDataStreamSocketTCP : public cDataStreamSource {
public:
  	enum eState {stClosed,stConnecting,stRequest,stResponse,stReceive};

	cDataStreamSocketTCP(char *address, uint16_t port, char *Request);
	virtual ~cDataStreamSocketTCP();
	virtual uchar* Get(int &Count);
	virtual void Del(int Count);
	virtual void Reset();
	virtual int GetPollHandle();
	
	eState GetState() { return State; }
	int GetActivityCounter() { return activitycounter; }

protected:
	static const int CONNECT_WAIT=2;
	static const int TRANSMIT_TIMEOUT=10;

	char *Address;
	char *Request;
	uint16_t Port;

  	int Socket;
	uchar SrcBuffer[20480];
	uchar *pSrcBuffer;
	int nSrcBuffer;

	eState State;
	int activitycounter;
	time_t LastConn;

	bool Open();
	void Close();
};

//
// cStreamPlayerHTTP
//

class cStreamPlayerHTTP : public cStreamPlayer {
protected:
  uint16_t port;
  char address[40],path[128];
  char StatusString[80];
  cDataStreamSocketTCP *Src;
  cFilterHTTPHeader *http;

  virtual cDataStreamSource* NewSource();
  virtual void DeleteSource(cDataStreamSource *src);
  virtual cDataStreamSource* NewPostBufferFilters(cDataStreamSource* src);
  virtual void DeletePostBufferFilters(cDataStreamSource* filter);

  void ParseURL(char *url);
public:
  cStreamPlayerHTTP(char *url);
  virtual ~cStreamPlayerHTTP();
  virtual char* GetStatus();
};


#endif
