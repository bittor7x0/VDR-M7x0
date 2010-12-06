/*
 *  $Id: componentVTP.h 17 2007-03-06 07:25:55Z andreas $
 */
 
#ifndef VDR_STREAMDEV_SERVERS_SERVERVTP_H
#define VDR_STREAMDEV_SERVERS_SERVERVTP_H

#include "server/component.h"

class cComponentVTP: public cServerComponent {
protected:
	virtual cServerConnection *NewClient(void);

public:
	cComponentVTP(void);
};

#endif // VDR_STREAMDEV_SERVERS_SERVERVTP_H
