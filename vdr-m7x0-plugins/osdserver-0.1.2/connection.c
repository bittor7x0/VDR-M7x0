

#include <stdarg.h>

#include "connection.h"

void cConnectionBase::ReplyPlain(int Code, char *buffer) {
    if (!IsConnected()) return;

    char *buffer1=NULL,*buffer2;

    if (500<=Code && Code<600) {
        // quote string
        int chars=0;
        char *p=buffer;
        while (*p!=0) {
            if (*p=='\r' || *p=='\n' || *p=='\t' || *p=='"' || *p=='\\' || *p=='\'') chars++;
            chars++;
            p++;
        }
        buffer1=(char*)malloc(chars+1);
        p=buffer;
        char *q=buffer1;
        while (*p!=0) {
            if (*p=='\r') {
                *q++='\\'; *q++='r';
            } else if (*p=='\n') {
                *q++='\\'; *q++='n';
            } else if (*p=='\t') {
                *q++='\\'; *q++='t';
            } else if (*p=='"') {
                *q++='\\'; *q++='"';
            } else if (*p=='\\') {
                *q++='\\'; *q++='\\';
            } else if (*p=='\'') {
                *q++='\\'; *q++='\'';
            } else {
                *q++=*p;
            }
            p++;
        }
        *q=0;
        buffer=buffer1;
    }

    char *p = buffer;
    while (*p) {
        char *n = strchr(p, '\n');
        if (n) *n=0;
        int len=asprintf(&buffer2,"%03d %s\r\n",Code,p);
        if (len<0) {
            LOG_ERROR;
            break;
        }
        Send(buffer2,len);
        free(buffer2);

        if (!n) break;
        p=n+1;
    }
    free(buffer1);
}

void cConnectionBase::Reply(int Code, const char *fmt, ...) {
    char *buffer;
    va_list ap;

    va_start(ap, fmt);
    int len=vasprintf(&buffer, fmt, ap);
    va_end(ap);
    if (len<0) {
        LOG_ERROR;
        return;
    }
    ReplyPlain(Code,buffer);
    free(buffer);
}

void cConnectionBase::ReplyDebug(int index, const char *fmt, ...) {
    char *buffer;
    va_list ap;

    index = (index%100)+100;

    va_start(ap, fmt);
    int len=vasprintf(&buffer, fmt, ap);
    va_end(ap);
    if (len<0) {
        LOG_ERROR;
        return;
    }
    ReplyPlain(index,buffer);
    free(buffer);
}




