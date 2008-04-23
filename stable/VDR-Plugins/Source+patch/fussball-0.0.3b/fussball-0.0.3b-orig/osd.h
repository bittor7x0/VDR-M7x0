#ifndef TABELLEOSD_H_
#define TABELLEOSD_H_

#include <vdr/menu.h>
#include <fstream>
#include <iostream>


class cOSD:public cMenuText
{
public:
	cOSD();
	virtual ~cOSD();
	void loadFile(const char* file, const char* titel);
    virtual eOSState ProcessKey(eKeys Key);	
};

#endif /*TABELLEOSD_H_*/
