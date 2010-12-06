#include <stdio.h>
#include <stdarg.h>

#include "debug.h"

#include <vdr/thread.h>


struct structErrorList {
	structErrorList *Next;
	char *Text;	
};

static structErrorList *FirstError,*LastError;

static cMutex ErrorMutex;


void ReportError(const char *fmt, ...) {
	va_list ap;
	structErrorList *e = new structErrorList;
	e->Next=NULL;

	va_start(ap,fmt);
	vasprintf(&e->Text,fmt,ap);
	va_end(ap);
	
	_dprintf("%s\n",e->Text);
	
	cMutexLock(ErrorMutex);
	if (LastError) {
		LastError->Next=e;
	} else {
		FirstError=LastError=e;
	}
}

char* GetError() {
	char *Text=NULL;
	cMutexLock(ErrorMutex);

	if (FirstError) {
		Text=FirstError->Text;
		FirstError=FirstError->Next;
		if (!FirstError) LastError=NULL;
	}
	return Text;
}

