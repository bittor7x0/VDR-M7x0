#ifndef __OSDSERVER_CONNECTION_H
#define __OSDSERVER_CONNECTION_H

#include "tools.h"

class cConnectionBase {
public:
    cConnectionBase() { }
    virtual ~cConnectionBase() {}

    virtual bool Send(const char *s, int length=-1)=0;
    virtual char* ReadLine()=0;
    virtual void DelLine()=0;
    virtual void AddSelect(cSelect &Selector)=0;

    virtual bool IsConnected()=0;

protected:
    void ReplyPlain(int Code, char *buffer);
    void Reply(int Code, const char *fmt, ...) __attribute__((format(printf,3,4)));

public:
    void ReplyDebug(int index, const char *fmt, ...) __attribute__((format(printf,3,4)));

};

#endif
