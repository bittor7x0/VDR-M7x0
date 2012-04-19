/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_CHANNELS_H
#define WI_CHANNELS_H

#include <klone/io.h>
#include "conf.h"

typedef struct channel_s {
	int channelNum; //Number to dial on remote control.
	char *channelId; //Id of the channel
	char *channelName; //Name of the channel.
	char *multiplexName; //Name of the multiplex if avaible.
	int frequency; //Frequency (in Hz) of the multiplex.
	char *parameter; //Paramter of the Channel.
	char *source; //Source of the signal.
	int srate; //Symbol rate, currently unset.
	int vpid; //Video-PID, currently unset.
	int apid; //Audio-PID, currently unset.
	int tpid; //Teletext-PID, currently unset.
	int caid; //Conditional Access-ID.
	int sid; //Service-ID.
	int nid; //Network-ID.
	int tid; //Transport-stream-ID.
	int rid; //Radio-ID.
} channel_t;

typedef struct channelList_s {
	int length;
	channel_t *channel;
} channelList_t;

void initChannel(channel_t * const channel);
void freeChannel(channel_t * const channel);
void initChannelList(channelList_t * const list);
void freeChannelList(channelList_t * const list);
void getChannelList(hostConf_t *vdrHost, channelList_t * const list, sortField_t sortField, sortDirection_t sortDirection);
bool getChannel(hostConf_t *vdrHost, int channelNum, channel_t * const channel);
void printChannelListSelect(wcontext_t *wctx, const char * id, const char * name,
		const channelList_t * const channels, int channelNum, const char *onchange);
void printChannelControls(wcontext_t *wctx, const channel_t *channel, const char *Epg, const char *LiveStream);
char *wctxChannelDisplayName(wcontext_t *wctx, const channel_t *channel);
char *wctxChannelFilename(wcontext_t *wctx, const char *channelName, bool urlEncode);
void printChannelsPage(wcontext_t *wctx);
void printChannelPlaylistPage(wcontext_t *wctx);

#endif
