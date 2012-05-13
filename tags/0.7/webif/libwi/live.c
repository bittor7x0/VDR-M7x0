/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Hans-Peter Jochmann
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include "channels.h"
#include "i18n.h"
#include "live.h"
#include "svdrp_comm.h"

void printLivePage(wcontext_t *wctx) {
    vars_t *rqargs = request_get_args(wctx->request);
    hostConf_t *host = (vars_countn(rqargs, "hostId") > 0) ? getHost(vars_get_value_i(rqargs, "hostId")) : getFirstVdrHost();

    void printHtmlHeadExtra(wcontext_t *wctx, const char *title) {
        // nested to use host.
        printCssLinkTag(wctx, "live.css", "screen");
        printJavascriptTag(wctx, "jquery.selectboxes.js");
        printJavascriptTag(wctx, "wi.live.js");
        if (true) {
            openCssStyleTag(wctx, "screen");
            WCTX_IPRINTF("#videoDiv { width: %dpx; }\n", wctx->conf.videoWidth + 10);
            closeCssStyleTag(wctx);
        }
        openJavascriptTag(wctx);
        WCTX_IPRINTF("$(function(){webif.livePageInit();});\n");

        WCTX_IPRINTT("$.extend(webif.pages,{\n", 0, 1);
        WCTX_IPRINTF("epg:%d\n", PN_EPG);
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.actions,{\n", 0, 1);
        WCTX_IPRINTF("getEpgNN:%d\n", PA_GET_EPG_NN);
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.state,{\n", 0, 1);
        if (host != NULL) {
            WCTX_IPRINTF("url_base:'http://%s',\n", getHostHttpAddr(host, wctx));
        }
        WCTX_IPRINTF("channelNum:'%d'\n", wctx->channelNum);
        WCTX_IPRINTT("});\n", -1, 0);

        closeJavascriptTag(wctx);
    }

    initHtmlPage(wctx, tr("liveStream"), printHtmlHeadExtra, NULL);

    if (host == NULL || !host->isVdr) {
        WCTX_MESSAGE(MT_ALERT, NULL, tr("errorNoVdrHost"));
    } else {
        channelList_t channels;
        getChannelList(host, &channels, SF_CH_NUMBER, SD_ASC);
        if (channels.length == 0) {
            WCTX_MESSAGE(MT_ALERT, NULL, tr("warnNoEPG"));
        } else {
            int channelNum = wctx->channelNum;
            if ((channelNum < 1) || (channelNum > channels.length)) {
                channelNum = 1;
            }

            WCTX_IPRINTT("<table class=\"layout\">\n", 0, 1);
            WCTX_IPRINTT("<tr>\n", 0, 1);
            WCTX_IPRINTT("<td id=\"videoDiv\">\n", 0, 1);
            if (wctx->conf.useHtml5VideoTag) {
                const char *hostAddr = getHostHttpAddr(host, wctx);
                WCTX_IPRINTFT("<video id=\"video\" src=\"http://%s:3000/%d\" width=\"%d\" height=\"%d\" controls=\"controls\">\n", 0, 1
                        , hostAddr, channelNum, wctx->conf.videoWidth, wctx->conf.videoHeight);
                WCTX_IPRINTT("</video>\n", -1, 0); //video
            } else { //vlc plugin
                WCTX_IPRINTT("<div id=\"video\">\n", 0, 1);
                WCTX_IPRINTF("<embed type=\"application/x-vlc-plugin\" "
                        "pluginspage=\"http://www.videolan.org\" "
                        "version=\"VideoLAN.VLCPlugin.2\" "
                        "id=\"vlc\" width=\"%d\" height=\"%d\">\n"
                        , wctx->conf.videoWidth, wctx->conf.videoHeight);
                WCTX_IPRINTF("</embed>")
            }
            WCTX_IPRINTT("</div>\n", -1, 0); //video
            WCTX_IPRINTT("</td>\n", -1, 0); //videoDiv

            WCTX_IPRINTT("<td id=\"control\">\n", 0, 1);
            //const char *Play = tr("play");
            const char *Pause = tr("pause");
            const char *Fullscreen = tr("fullScreen");
            const char *Mute = tr("mute");
            const char *VolumeDown = tr("volumeDown");
            const char *VolumeUp = tr("volumeUp");

            { //form
                WCTX_IPRINTT("<form action=\"" URLWI "\" method=\"post\" class=\"ui-helper-clearfix\">\n", 0, 1);
                WCTX_IPRINTF("<input type=\"hidden\" name=\"" ARG_PAGE "\" value=\"%d\"/>\n", PN_LIVE_STREAM);
                WCTX_IPRINTT("<ul class=\"controls\"><li class=\"control\">\n", 0, 1);
                printChannelListSelect(wctx, NULL, "channelNum", &channels, channelNum, NULL);
                WCTX_IPRINTT("</li><li class=\"control\">\n", -1, 1);
                WCTX_IPRINTF("<select id=\"audioTrack\" name=\"audioTrack\" size=\"1\" title=\"%s\"></select>\n", tr("audio.track"));
                WCTX_IPRINTFT("</li><li id=\"PlayOrPause\" class=\"control ui-button ui-button-icon-only ui-state-default\" title=\"%s\">\n", -1, 1, Pause);
                WCTX_IPRINTF("<span class=\"ui-button-icon-primary ui-icon ui-icon-pause\"></span><span class=\"ui-button-text\">%s</span>\n", Pause);
                WCTX_IPRINTFT("</li><li id=\"Fullscreen\" class=\"control ui-button ui-button-icon-only ui-state-default\" title=\"%s\">\n", -1, 1, Fullscreen);
                WCTX_IPRINTF("<span class=\"ui-button-icon-primary ui-icon ui-icon-arrow-4-diag\"></span><span class=\"ui-button-text\">%s</span>\n", Fullscreen);
                WCTX_IPRINTFT("</li><li id=\"Mute\" class=\"control ui-button ui-button-icon-only ui-state-default\" title=\"%s\">\n", -1, 1, Mute);
                WCTX_IPRINTF("<span class=\"ui-button-icon-primary ui-icon ui-icon-volume-off\"></span><span class=\"ui-button-text\">%s</span>\n", Mute);
                WCTX_IPRINTFT("</li><li id=\"VolumeDown\" class=\"control ui-button ui-button-icon-only ui-state-default\" title=\"%s\">\n", -1, 1, VolumeDown);
                WCTX_IPRINTF("<span class=\"ui-button-icon-primary ui-icon ui-icon-volume-down\"></span><span class=\"ui-button-text\">%s</span>\n", VolumeDown);
                WCTX_IPRINTFT("</li><li id=\"VolumeUp\" class=\"control ui-button ui-button-icon-only ui-state-default\" title=\"%s\">\n", -1, 1, VolumeUp);
                WCTX_IPRINTF("<span class=\"ui-button-icon-primary ui-icon ui-icon-volume-up\"></span><span class=\"ui-button-text\">%s</span>\n", VolumeUp);
                WCTX_IPRINTT("</li></ul>\n", -1, 0);
                WCTX_IPRINTT("</form>\n", -1, 0);
            }
            WCTX_IPRINT("<div id=\"info\"></div>\n");
            WCTX_IPRINTT("</td>\n", -1, 0); //control
            WCTX_IPRINTT("</tr>\n", -1, 0);
            WCTX_IPRINTT("</table>\n", -1, 0);
        }
        freeChannelList(&channels);
    }
    finishHtmlPage(wctx);
}

/**
 * Send snapshot of current channel in hostId.
 * Doesn't work in Gigaset's VDR.
 * 
 * @param wctx
 */
void currentImagePage(wcontext_t *wctx) {
    int imagesent = 0;
    vars_t *rqargs = request_get_args(wctx->request);
    int quality = (vars_countn(rqargs, "quality") > 0) ? vars_get_value_i(rqargs, "quality") : 80;
    int sizex = (vars_countn(rqargs, "sizex") > 0) ? vars_get_value_i(rqargs, "sizex") : 400;
    int sizey = (vars_countn(rqargs, "sizey") > 0) ? vars_get_value_i(rqargs, "sizey") : sizex * 3 / 4;
    int hostId = vars_get_value_i(rqargs, "hostId");
    char *data = execSvdrp(getHost(hostId), "GRAB .jpg %d %d %d", quality, sizex, sizey);
    if (data) {
        response_set_content_type(wctx->response, "image/jpeg");
        response_set_content_encoding(wctx->response, "base64");
        char *p = data;
        while (p && *p) {
            int code = strtol(p, &p, 10);
            if (code == SVDRP_IMG_DATA && p[0] == '-') {
                p++;
                int l = strcspn(p, "\n\r");
                io_write(wctx->out, p, l);
                p += l;
            } else {
                break;
            }
        }
        free(data);
        imagesent = ~0;
    }
    if (!imagesent) {
        response_set_status(wctx->response, HTTP_STATUS_NO_CONTENT);
    }

}