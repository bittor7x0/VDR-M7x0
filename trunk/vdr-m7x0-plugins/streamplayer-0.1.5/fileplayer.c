#include "fileplayer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "debug.h"

//
// cDataStreamFile
//

bool cDataStreamFile::Open() {
    src=fopen("/critical.ts","r");
    if (!src) {
		ReportError("%s: fopen %s: %i %s\n",tr("Error"),tr("failed"),errno,strerror(errno));
		return false;
    }
    return true;
}

void cDataStreamFile::Close() {
    if (src) {
		fclose(src);
		src=NULL;
	}
}

uchar* cDataStreamFile::Get(int &Count) {
	if (nSrcBuffer>0) {
		// return remains
		Count=nSrcBuffer;
		return pSrcBuffer;
	}

	if (!src) {
		if (!Open()) return NULL;
	}

	int r=fread(SrcBuffer,1,sizeof SrcBuffer,src);
	if (r<0) return NULL;
	
	pSrcBuffer=SrcBuffer;
	nSrcBuffer=r;

	Count=nSrcBuffer;
	return pSrcBuffer;
}


void cDataStreamFile::Del(int Count) {
	nSrcBuffer-=Count;
	pSrcBuffer+=Count;
}

void cDataStreamFile::Reset() {
	nSrcBuffer=0;
}

//
// cStreamPlayerFile
//

cStreamPlayerFile::cStreamPlayerFile(char *url) {
}

cStreamPlayerFile::~cStreamPlayerFile() {
	// Kill thread before deepest nested destructor finishes
	// Not good style, but not to avoid.
	Detach();
	Stop();
}

cDataStreamSource* cStreamPlayerFile::NewSource() {
	return new cDataStreamFile();
}

void cStreamPlayerFile::DeleteSource(cDataStreamSource *src) {
	delete src;
}


char* cStreamPlayerFile::GetStatus() {
	return "";
}

