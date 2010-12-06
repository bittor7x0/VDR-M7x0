#ifndef __streamplayer_datastream_h__
#define __streamplayer_datastream_h__

#include <vdr/tools.h>

class cDataStreamSource {
	// Abstract interface for the usual Get()/Del() concept
public:
	virtual uchar* Get(int &Count)=0;
	virtual void Del(int Count)=0;
	virtual void Reset()=0;
	virtual ~cDataStreamSource() {}
	
	virtual int GetPollHandle() { return 0; }
	// If no data is currently available, and new data depends on 
	// a file handle that can be poll()ed, return the handle.
	// Otherwise, return 0.
};


class cDataStreamFilter : public cDataStreamSource {
	// Source that depends on another source
private:
	cDataStreamSource *src;
protected:
	cDataStreamFilter(cDataStreamSource *Src) { src=Src; }
public:
	virtual uchar* Get(int &Count) { return src->Get(Count); }
	virtual void Del(int Count) { src->Del(Count); }
	virtual void Reset() { src->Reset(); }
};


class cDataStreamProcessor : public cDataStreamFilter {
	// Processing filter. All data will be passed to the ProcessData method.
	// The method can decide to drop data, pass through data, or hold back
	// data temporarily.
	
private:
	int ForwardableInBuffer;
protected:
	cDataStreamProcessor(cDataStreamSource *Src);
	virtual void ProcessData(uchar *data, int Count, int &Forwardable, int &Deletable)=0;
public:
	virtual uchar* Get(int &Count);
	virtual void Del(int Count);
	virtual void Reset() { cDataStreamFilter::Reset(); ForwardableInBuffer=0; }
};


#endif
