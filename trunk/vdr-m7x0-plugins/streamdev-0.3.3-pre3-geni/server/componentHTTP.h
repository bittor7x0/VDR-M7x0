/*
 *  $Id: componentHTTP.h 17 2007-03-06 07:25:55Z andreas $
 */
 
#ifndef VDR_STREAMDEV_HTTPSERVER_H
#define VDR_STREAMDEV_HTTPSERVER_H

#include "server/component.h"

class cComponentHTTP: public cServerComponent {
protected:
	virtual cServerConnection *NewClient(void);

public:
	cComponentHTTP(void);
};

#endif // VDR_STREAMDEV_HTTPSERVER_H
