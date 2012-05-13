#ifndef __streamplayer_fileplayer_h__
#define __streamplayer_fileplayer_h__

#include "player.h"

//
// File player, for debugging purposes only. Read a mpeg ts stream from disk.
//


//
// cDataStreamFile
//

class cDataStreamFile : public cDataStreamSource {
public:
	cDataStreamFile() { src=NULL; nSrcBuffer=0; }
	virtual ~cDataStreamFile() { Close(); }
	virtual uchar* Get(int &Count);
	virtual void Del(int Count);
	virtual void Reset();

protected:
	FILE *src;
	uchar SrcBuffer[20480];
	uchar *pSrcBuffer;
	int nSrcBuffer;

	bool Open();
	void Close();
};

//
// cStreamPlayerFile
//

class cStreamPlayerFile : public cStreamPlayer {

public:
  cStreamPlayerFile(char *url);
  virtual ~cStreamPlayerFile();
  virtual cDataStreamSource* NewSource();
  virtual void DeleteSource(cDataStreamSource *src);
  virtual char* GetStatus();
};



#endif
