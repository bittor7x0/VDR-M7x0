/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include "channels.h"
#include "commands.h"
#include "i18n.h"
#include "vdrwi.h"
#include "resource.h"

/**
 * Controller of vdrwi.
 * Determines which page is sent.
 * 
 * @param request
 * @param response
 * @param session
 * @param in
 * @param out
 */
void serveWiPage(request_t *request, response_t *response, session_t *session, io_t* in, io_t *out) {
    wcontext_t vwctx;
    wcontext_t *wctx = &vwctx;
    if (initWCtx(wctx, request, response, session, out)) {
        switch (request_get_method(request)) {
            case HM_PROPFIND:
#ifdef VDRWI_WEBDAV
                propFind(wctx);
#else
                warn("PropFind no implementado");
                response_set_status(wctx->response, HTTP_STATUS_NOT_IMPLEMENTED);
#endif
                break;
            default:
                switch (wctx->page) {
                    case PN_EPG:
                        printEpgPage(wctx);
                        break;
                    case PN_CHANNELS:
                        printChannelsPage(wctx);
                        break;
                    case PN_LIVE_STREAM:
                        printLivePage(wctx);
                        break;
                    case PN_CURRENT_IMAGE:
                        currentImagePage();
                        break;
                    case PN_PLAYLIST_CHN:
                        printChannelPlaylistPage(wctx);
                        break;
                    case PN_TIMERS:
                        serveTimersPage(wctx);
                        break;
                    case PN_SEARCHES:
                        printSearchesPage(wctx);
                        break;
                    case PN_RESOURCES:
                        serveResourcePage(wctx);
                        break;
                    case PN_COMMANDS:
                        printCommandsPage(wctx);
                        break;
                    case PN_SETTINGS:
                        printSettingsPage(wctx);
                        break;
                    case PN_FILES:
                        printViewFilePage(wctx);
                        break;
                    default:
                        response_set_status(wctx->response, HTTP_STATUS_NOT_FOUND);
                        break;
                }
                break;
        }
    }
    freeWCtx(wctx);

}
