#include "datastreams.h"


cDataStreamProcessor::cDataStreamProcessor(cDataStreamSource *Src)
	:cDataStreamFilter(Src) {

	ForwardableInBuffer=0;
};

uchar* cDataStreamProcessor::Get(int &Count) {
	uchar *Data;
	int avail;
	int forward=0,deletable=0;

	// See whats available from source:
	Data=cDataStreamFilter::Get(avail);
	if (Data==NULL) return NULL;

	if (avail>ForwardableInBuffer) {
		// New data in buffer that is not yet processed by ProcessData.

		ProcessData(Data+ForwardableInBuffer,avail-ForwardableInBuffer,forward,deletable);

		if (deletable>0) {
			// Delete first bytes from buffer
			// plus (unfortunately) all not yet used data.

			cDataStreamFilter::Del(ForwardableInBuffer+deletable);
			ForwardableInBuffer=0;
			Data=cDataStreamFilter::Get(avail);
		}

		ForwardableInBuffer+=forward;
		// Remember how much data is ready
	}

	// Return all processed data as available.
	Count=ForwardableInBuffer;
	if (Count>0) return Data;
	return NULL;
}

void cDataStreamProcessor::Del(int Count) {
	ForwardableInBuffer-=Count;
	cDataStreamFilter::Del(Count);
}
