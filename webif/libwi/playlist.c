/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <stdio.h>
#include "misc.h"
#include "playlist.h"
#include "resource.h"
#include "rspriv.h"

const char *plMimeType[] = {
    "audio/mpegurl",
    "application/xspf+xml"
};

const char *plExt[] = {
    "m3u",
    "xspf"
};

const char *plVirtName[] = {
    "pl.m3u",
    "pl.xspf"
};

static void initPlaylist(const playlistGenerator_t *plg, wcontext_t *wctx, playlistType_t plType, const char *fileName) {
    wctxBufferReserve(wctx, 1024);
    const char *FileName = "pl";
    response_set_content_type(wctx->response, plMimeType[plType]);
    char *fn;
    if (!fileName) {
        fn = (char *) FileName;
    } else if (strchr(fileName, ' ')) {
        char *fn1 = strdup(fileName);
        char *s;
        for (s = fn1; *s; s++) if (*s == ' ') *s = '_';
        fn = strdup(wctxHtmlEncode(wctx, fn1));
        free(fn1);
    } else {
        fn = strdup(wctxHtmlEncode(wctx, fileName));
    }
    response_set_field(wctx->response,
            "Content-Disposition",
            wctxBufferPrintf(wctx, "inline; filename=\"%s.%s\"", fn, plExt[plType]));
    if (fn && fn != fileName && fn != FileName) {
        free(fn);
    }
}

static void initM3UPlaylist(const playlistGenerator_t *plg, wcontext_t *wctx, const char *fileName) {
    initPlaylist(plg, wctx, PL_M3U, fileName);
    WCTX_PRINT("#EXTM3U\n");
}

static int printM3UPlaylistItem(const playlistGenerator_t *plg, wcontext_t *wctx, const char *const displayName, const char *const url, int id) {
    WCTX_PRINTF("#EXTINF:0,%.2d-%s\n", ++id, displayName);
    WCTX_PRINTF("%s\n", url);
    return id;
}

static int addResourceToM3U(const playlistGenerator_t *plg, wcontext_t *wctx, struct resource_s *resource, const char *const path, int id) {
    bool isHomogeneousResource = false; //TODO
    if (resource->type == RT_COLLECTION && !isHomogeneousResource) {
        int i;
        for (i = 0; i < resource->children.length; i++) {
            resource_t *child = resource->children.pentry[i];
            char *path2 = extendPath(path, child->name);
            id = addResourceToM3U(plg, wctx, child, path2, id);
            free(path2);
        }
    } else {
        const char *dn = resource->getDisplayName(resource, true);
        const char *url = getResourceURL(resource, wctx, path, PA_SERVE_RSR, true, false);
        id = plg->printPlaylistItem(plg, wctx, dn, url, id);
    }
    return id;
}

void printRsM3UPlaylist(const playlistGenerator_t *plg, wcontext_t *wctx, struct resource_s *resource, const char *const path) {
    initM3UPlaylist(plg, wctx, resource->name);
    addResourceToM3U(plg, wctx, resource, path, 0);
}

static void initXSPFPlaylist(const playlistGenerator_t *plg, wcontext_t *wctx, const char *fileName) {
    initPlaylist(plg, wctx, PL_XSPF, fileName);
    WCTX_PRINT("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
    WCTX_PRINT("<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\" xmlns:vlc=\"http://www.videolan.org/vlc/playlist/ns/0/\">\n");
}

void finishXSPFPlaylist(const playlistGenerator_t *plg, wcontext_t *wctx) {
    WCTX_PRINT("</playlist>\n");
}

int printXSPFPlaylistItem(const playlistGenerator_t *plg, wcontext_t *wctx, const char *const displayName, const char *const eUrl, int id) {
    WCTX_PRINT("\t<track>\n");
    WCTX_PRINTF("\t\t<identifier>%d</identifier>\n", ++id);
    WCTX_PRINTF("\t\t<location>%s</location>\n", eUrl);
    WCTX_PRINTF("\t\t<title>%s</title>\n", wctxHtmlEncode(wctx, displayName));
    WCTX_PRINT("\t\t<extension application=\"http://www.videolan.org/vlc/playlist/0\">\n");
    WCTX_PRINTF("\t\t\t<vlc:id>%d</vlc:id>\n", id);
    WCTX_PRINT("\t\t</extension>\n");
    WCTX_PRINT("\t</track>\n");
    return id;
}

int printXSPFPlaylistItemExtra(const playlistGenerator_t *plg, wcontext_t *wctx, int id) {
    WCTX_IPRINTF("<vlc:item tid=\"%d\" />\n", ++id);
    return id;
}

static int addResourceToXSPFExtension(const playlistGenerator_t *plg, wcontext_t *wctx, resource_t *resource, int id) {
    if (resource->type == RT_COLLECTION && resource->children.length == 1 && resource->children.pentry[0]->type == RT_VDR_REC) {
        resource = resource->children.pentry[0];
    }
    if (resource->type == RT_COLLECTION) {
        if (resource->children.length > 0) {
            WCTX_IPRINTFT("<vlc:node title=\"%s\">\n", 0, 1, wctxHtmlEncode(wctx, resource->getDisplayName(resource, true)));
            int i;
            for (i = 0; i < resource->children.length; i++) {
                resource_t *child = resource->children.pentry[i];
                id = addResourceToXSPFExtension(plg, wctx, child, id);
            }
            WCTX_PRINT("</vlc:node>\n");
        }
    } else {
        WCTX_IPRINTF("<vlc:item tid=\"%d\" />\n", ++id);
    }
    return id;
}

static int addResourceToXSPFTrackList(const playlistGenerator_t *plg, wcontext_t *wctx, resource_t *resource, const char *const path, int id) {
    if (resource->type == RT_COLLECTION) {
        int i;
        for (i = 0; i < resource->children.length; i++) {
            resource_t *child = resource->children.pentry[i];
            char *path2 = extendPath(path, child->name);
            id = addResourceToXSPFTrackList(plg, wctx, child, path2, id);
            free(path2);
        }
    } else {
        const char *dn = resource->getDisplayName(resource, true);
        const char *url = getResourceURL(resource, wctx, path, PA_SERVE_RSR, true, true);
        id = plg->printPlaylistItem(plg, wctx, dn, url, id);
    }
    return id;
}

static void printRsXSPFPlaylist(const playlistGenerator_t *plg, wcontext_t *wctx, resource_t *resource, const char *const path) {
    int id = 0;
    initXSPFPlaylist(plg, wctx, resource->name);
    WCTX_PRINT("\t<trackList>\n");
    id = addResourceToXSPFTrackList(plg, wctx, resource, path, id);
    WCTX_PRINT("\t</trackList>\n");
    WCTX_PRINT("\t<extension application=\"http://www.videolan.org/vlc/playlist/0\">\n");
    addResourceToXSPFExtension(plg, wctx, resource, 0);
    WCTX_PRINT("\t</extension>\n");
    finishXSPFPlaylist(plg, wctx);
}

const playlistGenerator_t plGenerator[] = {
    {
        .initPlaylist = &initM3UPlaylist,
        .finishPlaylist = NULL,
        .printPlaylistItem = &printM3UPlaylistItem,
        .printResourcePlaylist = &printRsM3UPlaylist,
    },
    {
        .initPlaylist = &initXSPFPlaylist,
        .finishPlaylist = &finishXSPFPlaylist,
        .printPlaylistItem = &printXSPFPlaylistItem,
        .printResourcePlaylist = &printRsXSPFPlaylist,
    }
};

void serveResourcePlaylist(wcontext_t *wctx, playlistType_t plType, int hostId, const char *const path, resourceType_t type) {
    resource_t *resource = getResource(hostId, path, type, -1, false, false);
    if (resource) {
        if (plType >= PL_MIN && plType <= PL_MAX) {
            const playlistGenerator_t *plg = plGenerator + plType;
            plg->printResourcePlaylist(plg, wctx, resource, path);
        } else {
            response_set_status(wctx->response, HTTP_STATUS_BAD_REQUEST);
        }
        freeResourceChain(resource);
    } else {
        response_set_status(wctx->response, HTTP_STATUS_NOT_FOUND);
    }
}

