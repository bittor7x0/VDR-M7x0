/*
 *  $Id: componentVTP.c 17 2007-03-06 07:25:55Z andreas $
 */
 
#include "server/componentVTP.h"
#include "server/connectionVTP.h"
#include "server/setup.h"

cComponentVTP::cComponentVTP(void): 
		cServerComponent("VTP", StreamdevServerSetup.VTPBindIP, 
		                 StreamdevServerSetup.VTPServerPort)
{
}

cServerConnection *cComponentVTP::NewClient(void)
{
	return new cConnectionVTP;
}
