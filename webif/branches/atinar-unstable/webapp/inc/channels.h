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
* You will need the KLONE web application development framework
* from www.koanlogic.com Version 2.
* 
*/

#ifndef __CHANNELS_H__
#define __CHANNELS_H__

typedef struct channelEntry_s {
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
} channelEntry;

typedef struct channelList_s {
	int length;
	channelEntry *entry;
} channelList;

void initCE(channelEntry * const entry);
void freeCE(channelEntry * const entry);
void initCL(channelList  * const list);
void freeCL(channelList  * const list);
void getChannelList(channelList * const list);
/*
 * Retrieve channelName.
 * If channelNum==0, retrieve current channel name.
 */
int getChannel(int channelNum, channelEntry * const channel);

#endif
