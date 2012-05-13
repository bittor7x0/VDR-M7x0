/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <u/libu.h>

#include "channels.h"
#include "i18n.h"
#include "misc.h"
#include "playlist.h"
#include "svdrp_comm.h"


//TODO gestionar como una clase de resource_t más

void initChannel(channel_t * const channel) {
    memset(channel, 0, sizeof (channel_t));
    channel->channelId = NULL;
    channel->channelName = NULL;
    channel->multiplexName = NULL;
    channel->parameter = NULL;
    channel->source = NULL;
}

void freeChannel(channel_t * const channel) {
    free(channel->channelId);
    free(channel->channelName);
    free(channel->multiplexName);
    free(channel->parameter);
    free(channel->source);
    initChannel(channel);
}

void initChannelList(channelList_t * const list) {
    list->length = 0;
    list->channel = NULL;
}

void freeChannelList(channelList_t * const list) {
    int i;
    channel_t *channel;
    for (i = 0, channel = list->channel; i < list->length; i++, channel++) {
        freeChannel(channel);
    }
    free(list->channel);
    list->length = 0;
    list->channel = NULL;
}

void parseChannel(char * line, channel_t * channel) {
    char *r;
    int k, l;

    r = line;
    channel->channelNum = strtol(r, &r, 10);
    r += strspn(r, " ");
    for (k = 0; k < 13 && r[0]; k++) {
        l = strcspn(r, ":");
        switch (k) {
            case 0:
            {
                int l2 = strcspn(r, ",;:");
                crit_goto_if((channel->channelName = strndup(r, l2)) == NULL, outOfMemory);
                if (r[l2] != ':') {
                    char *r2 = r + l2 + 1;
                    l2 = strcspn(r2, ":");
                    crit_goto_if((channel->multiplexName = strndup(r2, l2)) == NULL, outOfMemory);
                } else {
                    channel->multiplexName = strdup("");
                }
                break;
            }
            case 1: channel->frequency = strtol(r, NULL, 10);
                break;
            case 2: crit_goto_if((channel->parameter = strndup(r, l)) == NULL, outOfMemory);
                break;
            case 3: crit_goto_if((channel->source = strndup(r, l)) == NULL, outOfMemory);
                break;
            case 5: channel->vpid = strtol(r, NULL, 10);
                break;
            case 8: channel->caid = strtol(r, NULL, 10);
                break;
            case 9: channel->sid = strtol(r, NULL, 10);
                break;
            case 10: channel->nid = strtol(r, NULL, 10);
                break;
            case 11: channel->tid = strtol(r, NULL, 10);
                break;
            case 12: channel->rid = strtol(r, NULL, 10);
                break;
        }
        r += l;
        if (r[0] != ':') break;
        r++;
    }
    if (channel->rid) {
        crit_goto_if(asprintf(&channel->channelId, "%s-%d-%d-%d-%d", channel->source, channel->nid, channel->tid, channel->sid, channel->rid) < 0, outOfMemory);
    } else {
        crit_goto_if(asprintf(&channel->channelId, "%s-%d-%d-%d", channel->source, channel->nid, channel->tid, channel->sid) < 0, outOfMemory);
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

int compareChannels(const channel_t *ca, const channel_t *cb, sortField_t sortField, sortDirection_t sortDirection) {
    int result;
    switch (sortField) {
        case SF_MUX:
            result = strcasecmp(ca->multiplexName, cb->multiplexName) * sortDirection;
            if (result) return result;
            /* no break */
        case SF_CH_NAME:
            result = strcasecmp(ca->channelName, cb->channelName) * sortDirection;
            if (result) return result;
            /* no break */
        case SF_CH_NUMBER:
            return (ca->channelNum - cb->channelNum) * sortDirection;
        default:
            return 0;
    }
}

void sortChannelList(channelList_t * const list, const sortField_t sortField, const sortDirection_t sortDirection) {

    int compare(const void * a, const void * b) {
        return compareChannels((const channel_t*) a, (const channel_t*) b, sortField, sortDirection);
    }
    if (list->length > 0 && sortField != SF_NONE) {
        qsort(list->channel, list->length, sizeof (channel_t), compare);
    }
}

void getChannelList(hostConf_t *host, channelList_t * const list, sortField_t sortField, sortDirection_t sortDirection) {
    initChannelList(list);
    char *data = execSvdrp(host, "LSTC");
    if (data) {
        char *p;
        for (p = strtok(data, "\r\n"); p != 0; p = strtok(0, "\r\n")) {
            if (atoi(p) == SVDRP_CMD_OK) {
                list->channel = realloc(list->channel, (++list->length) * sizeof (channel_t));
                crit_goto_if(list->channel == NULL, outOfMemory);
                channel_t *channel = list->channel + list->length - 1;
                initChannel(channel);
                parseChannel(p + 4, channel);
            }
        }
        free(data);
        sortChannelList(list, sortField, sortDirection);
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool getChannel(hostConf_t *host, int channelNum, channel_t * const channel) {
    initChannel(channel);
    bool result = false;
    char *data = (channelNum < 1) ? execSvdrp(host, "CHAN") : execSvdrp(host, "CHAN %d", channelNum);
    if (data) {
        char *r;
        int code = strtol(data, &r, 10);
        if (code == SVDRP_CMD_OK) {
            r++;
            channel->channelNum = strtol(r, &r, 10);
            r++;
            int l = strcspn(r, "\r\n");
            channel->channelName = strndup(r, l);
            result = true;
        }
        free(data);
    }
    return result;
}

void printChannelListSelect(wcontext_t *wctx, const char *id, const char *name,
        const channelList_t * const channels, int channelNum, const char *onchange) {
    int i;
    channel_t *channel;
    WCTX_IPRINTFT("<select id=\"%s\" name=\"%s\" size=\"1\"", 0, 1, (id) ? id : name, name);
    if (onchange != NULL) {
        WCTX_PRINTF(" onchange=\"%s\"", onchange);
    }
    WCTX_PRINT(">\n");
    for (i = 0, channel = channels->channel; i < channels->length; i++, channel++) {
        WCTX_IPRINTF("<option value=\"%d\" %s>%d - %s</option>\n"
                , channel->channelNum, selected[boolean(channel->channelNum == channelNum)]
                , channel->channelNum, channel->channelName);
    }
    WCTX_IPRINTT("</select>\n", -1, 0);
}

void printChannelControls(wcontext_t *wctx, const channel_t *channel, const char *Epg, const char *LiveStream) {
    WCTX_IPRINT("<ul class=\"controls\">");
    if (Epg) {
        WCTX_PRINTT("<li class=\"control\">\n", 1);
        WCTX_IPRINTF("<a href=\"" URLWI_P AMP "channelNum=%d\" class=\"epg control ui-button ui-button-icon-only ui-state-default\" title=\"%s\">"
                "<span class=\"ui-button-icon-primary ui-icon ui-icon-clock\"></span>"
                "<span class=\"ui-button-text\">%s</span>"
                "</a>\n", PN_EPG, channel->channelNum, Epg, Epg);
        WCTX_IPRINTT("</li>", -1, 0);
    }
    if (LiveStream) {
        bool isTv = boolean(channel->vpid > 1);
        WCTX_PRINTT("<li class=\"control\">\n", 1);
        WCTX_IPRINTF("<a href=\"" URLWI_P AMP "channelNum=%d\" class=\"live control ui-button ui-button-icon-only ui-state-default\" title=\"%s\">"
                "<span class=\"ui-button-icon-primary ui-icon ui-icon-%s\"></span>"
                "<span class=\"ui-button-text\">%s</span>"
                "</a>\n"
                , PN_LIVE_STREAM, channel->channelNum, LiveStream, (isTv) ? "tv" : "radio", LiveStream);
        WCTX_IPRINTT("</li>", -1, 0);
    }
    WCTX_PRINT("</ul>\n");
}

char *wctxChannelDisplayName(wcontext_t *wctx, const channel_t *channel) {
    if (strchr(channel->channelName, '.')) {
        wctxBufferReserve(wctx, strlen(channel->channelName));
        strcpy(wctx->buffer, channel->channelName);
        char *c;
        while ((c = strchr(wctx->buffer, '.')) != NULL) *c = ' ';
        return wctx->buffer;
    } else {
        return channel->channelName;
    }
}

char *wctxChannelFilename(wcontext_t *wctx, const char *channelName, bool urlEncode) {
    int l;
    const char *s;
    char *d;
    l = strlen(channelName);
    wctxBufferReserve(wctx, l);
    for (s = channelName, d = wctx->buffer; *s; s++) {
        *d = (*s == ' ') ? '_' : *s;
        d++;
    }
    *d = 0;
    if (urlEncode) {
        char *se = strdup(wctx->buffer);
        wctxUrlEncodeN(wctx, se, l, NULL);
        free(se);
    }
    return wctx->buffer;
}

static void printHtmlHeadExtra(wcontext_t * wctx, const char *title) {
    printCssLinkTag(wctx, "channels.css", "screen");
    printJavascriptTag(wctx, "wi.channels.js");
    openJavascriptTag(wctx);
    WCTX_IPRINT("$(function(){webif.channelsPageInit();});\n");
    closeJavascriptTag(wctx);
}

static void printTopInnerHtml(wcontext_t * wctx, const char *title) {
    const char *PlaylistDownload = tr("channels.playlist.download");
    WCTX_IPRINTF("<a href=\"" URLWI_P "\" title=\"%s\">%s</a>\n"
            , PN_PLAYLIST_CHN, PlaylistDownload, title);
}

void printChannelsPage(wcontext_t *wctx) {
    wctxBufferReserve(wctx, 100);
    channelList_t channels;
    channel_t *channel;
    int i = 0;

    initHtmlPage(wctx, tr("channels"), printHtmlHeadExtra, printTopInnerHtml);

    hostConf_t *host = getFirstVdrHost();
    if (!host) {
        WCTX_MESSAGE(MT_ALERT, NULL, tr("errorNoVdrHost"));
    } else {
        getChannelList(host, &channels, wctx->sortField, wctx->sortDirection);
        bool isTv;
        const char *tableId[] = {"tvChannels", "radioChannels"};
        const char *ChannelDetails = tr("channelDetails");
        const char *Epg = tr("epg");
        const char *LiveStream = tr("liveStream");
        const char *Frequency = tr("frequency");
        const char *Parameters = tr("parameters");
        const char *Source = tr("source");
        const char *ConditionalAccessId = tr("conditionalAccessId");
        const char *ServiceId = tr("serviceId");
        const char *NetworkId = tr("networkId");
        const char *TransportStreamId = tr("transportStreamId");
        const char *RadioId = tr("radioId");
        int t;
        for (t = 0; t < 2; t++) {
            const char *Caption = tr(tableId[t]);
            WCTX_IPRINTFT("<table id=\"%s\" class=\"list channels\">\n", 0, 1, tableId[t]);
            WCTX_IPRINTF("<caption>%s</caption>\n", Caption);
            WCTX_IPRINT("<col class=\"number\"/>\n");
            WCTX_IPRINT("<col class=\"channel\"/>\n");
            WCTX_IPRINT("<col class=\"mux\"/>\n");
            WCTX_IPRINTT("<thead class=\"listHead\">\n", 0, 1);
            WCTX_IPRINTT("<tr>\n", 0, 1);
            printList1TH(wctx, SF_CH_NUMBER, "#");
            printList1TH(wctx, SF_CH_NAME, tr("channel"));
            printList1TH(wctx, SF_MUX, tr("channelMux"));
            WCTX_IPRINTT("</tr>\n", -1, 0);
            WCTX_IPRINTT("</thead>\n", -1, 0);
            for (i = 0, channel = channels.channel; i < channels.length; i++, channel++) {
                isTv = boolean(channel->vpid > 1);
                if ((t == 0 && isTv) || (t == 1 && !isTv)) {
                    WCTX_IPRINTT("<tbody class=\"listBody channelDiv itemDiv infoSet\">\n", 0, 1);
                    WCTX_IPRINTT("<tr class=\"channelHead itemHead\">\n", 0, 1);
                    WCTX_IPRINTF("<td class=\"number\" >%d</td>\n", channel->channelNum);
                    WCTX_IPRINTF(
                            "<td class=\"channel\">"
                            STATUS_ICON("expanded")
                            "<a class=\"channel title infoToggler\" href=\"#\">%s</a>"
                            "</td>\n", channel->channelName);
                    WCTX_IPRINTF("<td class=\"mux\">%s</td>\n", channel->multiplexName);
                    WCTX_IPRINTT("</tr>\n", -1, 0);
                    WCTX_IPRINTT("<tr class=\"channelBody itemBody infoExtra\">\n", 0, 1);
                    WCTX_IPRINTT("<td class=\"channelInfo\" colspan=\"3\">\n", 0, 1);
                    printChannelControls(wctx, channel, Epg, LiveStream);
                    if (!webifConf.noChannelLogos) {
                        WCTX_IPRINTF("<img id=\"logo_%s\" class=\"channelLogo\" alt=\"%s\" src=\"/www2/images/logos/%s.png\"/>\n"
                                , channel->channelId, channel->channelName, wctxChannelFilename(wctx, channel->channelName, true));
                    }
                    WCTX_IPRINTT("<table class=\"channelInfo\">\n", 0, 1);
                    WCTX_IPRINTF("<caption>%s</caption>\n", ChannelDetails);
                    WCTX_IPRINTF("<tr><th>Id</th><td>%s</td></tr>\n", channel->channelId);
                    WCTX_IPRINTF("<tr><th>%s</th><td>%d</td></tr>\n", Frequency, channel->frequency);
                    WCTX_IPRINTF("<tr><th>%s</th><td>%s</td></tr>\n", Parameters, channel->parameter);
                    WCTX_IPRINTF("<tr><th>%s</th><td>%d</td></tr>\n", Source, channel->source);
                    WCTX_IPRINTF("<tr><th>%s</th><td>%d</td></tr>\n", ConditionalAccessId, channel->caid);
                    WCTX_IPRINTF("<tr><th>%s</th><td>%d</td></tr>\n", ServiceId, channel->sid);
                    WCTX_IPRINTF("<tr><th>%s</th><td>%d</td></tr>\n", NetworkId, channel->nid);
                    WCTX_IPRINTF("<tr><th>%s</th><td>%d</td></tr>\n", TransportStreamId, channel->tid);
                    WCTX_IPRINTF("<tr><th>%s</th><td>%d</td></tr>\n", RadioId, channel->rid);
                    WCTX_IPRINTT("</table>\n", -1, 0);
                    WCTX_IPRINTT("</td>\n", -1, 0); //channelInfo
                    WCTX_IPRINTT("</tr>\n", -1, 0); //channelInfo
                    WCTX_IPRINTT("</tbody>" HTMLCOMENT("channelDiv") "\n", -1, 0);
                }
            }
            WCTX_IPRINTT("</table>" HTMLCOMENT("level3Div") "\n", -1, 0);
        }
        freeChannelList(&channels);
    }
    finishHtmlPage(wctx);
}

void printChannelPlaylistPage(wcontext_t *wctx) {
    vars_t *rqargs = request_get_args(wctx->request);
    hostConf_t *host = (vars_countn(rqargs, "hostId") > 0) ? getHost(vars_get_value_i(rqargs, "hostId")) : getFirstVdrHost();
    if (!host) {
        response_set_status(wctx->response, HTTP_STATUS_NO_CONTENT);
    } else {
        int nchannel = (vars_countn(rqargs, "channelnum") > 0)
                ? vars_get_value_i(rqargs, "channelnum")
                : -1;
        response_t *response = wctx->response;
        int i;
        channelList_t channels;
        channel_t *channel;
        getChannelList(host, &channels, SF_CH_NUMBER, SD_ASC);
        const char *hostAddr = getHostHttpAddr(host, wctx);
        const playlistGenerator_t *plg = plGenerator + wctx->conf.playlistType;

        plg->initPlaylist(plg, wctx, tr("channels.filename"));
        if (wctx->conf.playlistType == PL_XSPF) {
            WCTX_IPRINTT("<trackList>\n", 0, 1);
        }
        for (i = 0, channel = channels.channel; i < channels.length; i++, channel++) {
            if (nchannel == -1 || i == nchannel) {
                plg->printPlaylistItem(plg, wctx, channel->channelName
                        , wctxBufferPrintf(wctx, "http://%s:%d/%d", hostAddr, 3000, channel->channelNum), channel->channelNum);
            }
        };
        if (wctx->conf.playlistType == PL_XSPF) {
            WCTX_IPRINTT("</trackList>\n", -1, 0);
            WCTX_IPRINTT("<extension application=\"http://www.videolan.org/vlc/playlist/0\">\n", 0, 1);
            WCTX_IPRINTFT("<vlc:node title=\"%s\">\n", 0, 1, tr("channels"));
            for (i = 0; i < channels.length; i++) {
                if (nchannel == -1 || i == nchannel) {
                    WCTX_IPRINTF("<vlc:item tid=\"%d\" />\n", i);
                }
            }
            WCTX_IPRINTT("</vlc:node>\n", -1, 0);
            WCTX_IPRINTT("</extension>\n", -1, 0);
        }
        if (plg->finishPlaylist) plg->finishPlaylist(plg, wctx);
        freeChannelList(&channels);
    }

}
