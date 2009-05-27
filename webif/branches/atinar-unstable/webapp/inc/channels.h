/*
* 
* This source-code is licensed under GPL v2.
* See ../../LICENSE for details
* 
* (c) Christian Kelinski <k@kille.cx>
* Please checkout the README file!
* 
* Originally written for the open7x0.org VDR-FW project:
* www.open7x0.org
* 
* Modified for http://vdr-m7x0.foroactivo.com.es by:
* atinar <atinar1@hotmail.com>
* 
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#ifndef __CHANNELS_H__
#define __CHANNELS_H__

#include <klone/io.h>
#include "conf.h"

typedef struct channel_s {
	int  channelNum;        //Number to dial on remote control.
	char *channelId;        //ID of the channel, currently unset!
	char *channelName;      //Name of the channel.
	char *multiplexName;    //Name of the multiplex if avaible.
	int  frequency;         //Frequency (in Hz) of the multiplex.
	char *parameter;        //Paramter of the Channel.
	char *source;           //Source of the signal.
	int  srate;             //Symbol rate, currently unset.
	int  vpid;              //Video-PID, currently unset.
	int  apid;              //Audio-PID, currently unset.
	int  tpid;              //Teletext-PID, currently unset.
	int  caid;              //Conditional Access-ID.
	int  sid;               //Service-ID.
	int  nid;               //Network-ID.
	int  tid;               //Transport-stream-ID.
	int  rid;               //Radio-ID.
} channel_t;

typedef struct channelList_s {
	int length;
	channel_t *channel;
} channelList_t;

void initChannel(channel_t * const channel);
void freeChannel(channel_t * const channel);
void initChannelList(channelList_t  * const list);
void freeChannelList(channelList_t  * const list);
void getChannelList(hostConf_t *vdrHost, channelList_t * const list, sortField_t sortBy, sortDirection_t sortDirection);
/*
 * Retrieve channelName.
 * If channelNum==0, retrieve current channel name.
 */
boolean_t getChannel(hostConf_t *vdrHost, int channelNum, channel_t * const channel);
void printChannelListSelect(io_t *out,int ntabs,const char * name,const channelList_t *const channels,int channelNum);

#endif
