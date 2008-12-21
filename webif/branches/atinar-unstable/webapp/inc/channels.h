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

//Channelliste kurz
typedef struct channelList {
  int  channel_num;       //Number to dial on remote control.
  char channel_id[50];    //ID of the channel, currently unset!
  char channelName[50];   //Name of the channel.
  char multiplexName[25]; //Name of the multiplex if avaible.
  int  frequency;         //Frequency (in Hz) of the multiplex.
  char parameter[25];     //Paramter of the Channel.
  char source[9];         //Source of the signal.
  int  srate;             //Symbol rate, currently unset.
  int  vpid;              //Video-PID, currently unset.
  int  apid;              //Audio-PID, currently unset.
  int  tpid;              //Teletext-PID, currently unset.
  int  caid;              //Conditional Access-ID.
  int  sid;               //Service-ID.
  int  nid;               //Network-ID.
  int  tid;               //Transport-stream-ID.
  int  rid;               //Radio-ID.
} channelList;

channelList * get_channel_list(int * max);
int getChannel(char channelName[30]);

#endif
