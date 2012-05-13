/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 * Virtual children (playlist, direct download...) of real resources
 */

#include <stddef.h>
#include "playlist.h"
#include "resource.h"
#include "rspriv.h"

typedef struct rsVirtPlaylist_s {
    resource_t r;
    playlistType_t plType;
} rsVirtPlaylist_t;

void serveVirtPlaylist(void *this, wcontext_t *wctx, const char *path) {
    rsVirtPlaylist_t *vpl = this;
    if (vpl) {
        char *ls = strrchr(path, '/');
        if (!ls) {
            path = NULL;
        }
        if (ls) ls[0] = 0;
        serveResourcePlaylist(wctx, vpl->plType, vpl->r.hostId, path, RT_UNKNOWN);
        if (ls) ls[0] = '/';
    }
}

#ifdef VDRWI_WEBDAV

void printVirtPlaylistPropFind(void *this, wcontext_t *wctx, const char *path, const wdPropFind_t *propFind, u_string_t *pstr, int depth) {
    rsVirtPlaylist_t *vpl = this;
    u_string_cat(pstr, "<response>\n");
    printPropFindHref(pstr, wctx, path, false);
    if (propFind->propName) {
        u_string_cat(pstr, "<propstat>\n");
        u_string_cat(pstr, "<prop>\n");
        u_string_aprintf(pstr, "<%s/>", DisplayName);
        u_string_aprintf(pstr, "<%s/>", GetContentType);
        u_string_cat(pstr, "</prop>\n");
        printPropStatus(pstr, HTTP_STATUS_OK);
        u_string_cat(pstr, "</propstat>\n");
    } else {
        if (propFind->allProps || propFind->props.displayName) {
            printPropStat(pstr, HTTP_STATUS_OK, true, DisplayName, vpl->r.getDisplayName(vpl, false));
        }
        if (propFind->allProps || propFind->props.getContentType) {
            printPropStat(pstr, HTTP_STATUS_OK, false, GetContentType, plMimeType[vpl->plType]);
        }
    }
    u_string_cat(pstr, "</response>\n");
}
#endif

const char *VirtPlaylistCssPfx = "pl";

rsVirtPlaylist_t *initVirtPlaylist(rsVirtPlaylist_t *virtPlaylist, resource_t *parent, const playlistType_t plType) {
    initResource(&virtPlaylist->r, parent->hostId, RT_VIRTUAL, parent, plVirtName[plType], plMimeType[plType], &parent->tcreationDate);
    virtPlaylist->plType = plType;
    virtPlaylist->r.serve = &serveVirtPlaylist;
#ifdef VDRWI_WEBDAV
    virtPlaylist->r.printThisPropFind = &printVirtPlaylistPropFind;
#endif
    virtPlaylist->r.addFragments = NULL;
    virtPlaylist->r.addVirtualResources = NULL;
    return virtPlaylist;
}

void *newVirtPlaylist(resource_t *parent, const playlistType_t plType) {
    return initVirtPlaylist(malloc(sizeof (rsVirtPlaylist_t)), parent, plType);
}

bool isVirtualPlaylist(const char *path, playlistType_t *pplType) {
    warn_return_if(!path, false);
    const char *ls = strrchr(path, '/');
    if (!ls) ls = path;
    else ls++;
    int plType;
    for (plType = PL_MIN; plType <= PL_MAX; plType++) {
        if (strcmp(ls, plVirtName[plType]) == 0) {
            *pplType = plType;
            return true;
        }
    }
    return false;
}

/**
 * Direct download virtual resource
 */

typedef struct rsDirectDownload_s {
    resource_t r;
} rsDirectDownload_t;

void serveDirectDownload(void *this, wcontext_t *wctx, const char *path) {
    rsDirectDownload_t *dd = this;
    if (dd && dd->r.parent) {
        resource_t *parent = dd->r.parent;
        char *ls = strrchr(path, '/');
        if (!ls) {
            path = NULL;
        }
        if (ls) ls[0] = 0;
        parent->serve(parent, wctx, path);
        if (ls) ls[0] = '/';
    }
}

#ifdef VDRWI_WEBDAV

void printDirectDownloadPropFind(void *this, wcontext_t *wctx, const char *path, const wdPropFind_t *propFind, u_string_t *pstr, int depth) {
    rsDirectDownload_t *dd = this;
    resource_t *parent = dd->r.parent;
    u_string_cat(pstr, "<response>\n");
    printPropFindHref(pstr, wctx, path, false);
    if (propFind->propName) {
        u_string_cat(pstr, "<propstat>\n");
        u_string_cat(pstr, "<prop>\n");
        u_string_aprintf(pstr, "<%s/>", DisplayName);
        u_string_aprintf(pstr, "<%s/>", GetContentType);
        u_string_cat(pstr, "</prop>\n");
        printPropStatus(pstr, HTTP_STATUS_OK);
        u_string_cat(pstr, "</propstat>\n");
    } else {
        if (propFind->allProps || propFind->props.displayName) {
            printPropStat(pstr, HTTP_STATUS_OK, true, DisplayName, dd->r.getDisplayName(dd, false));
        }
        if (propFind->allProps || propFind->props.getContentType) {
            printPropStat(pstr, HTTP_STATUS_OK, false, GetContentType, parent->mimeType);
        }
        if (propFind->allProps || propFind->props.getContentLength) {
            if (parent->getContentLength && isLocalResource(parent)) {
                char *ls = strrchr(path, '/');
                if (ls) {
                    ls[0] = 0;
                    uint64_t cl = parent->getContentLength(parent, path);
                    printPropStat(pstr, HTTP_STATUS_OK, false, GetContentLength, "%lld", cl);
                    ls[0] = '/';
                }
            }
        }
    }
    u_string_cat(pstr, "</response>\n");
}
#endif

const char *DirectDownloadCssPfx = "dd";

rsDirectDownload_t *initDirectDownload(rsDirectDownload_t *dd, resource_t *parent) {
    initResource(&dd->r, parent->hostId, RT_VIRTUAL, parent, tr("download"), parent->mimeType, &parent->tcreationDate);
    dd->r.serve = &serveDirectDownload;
#ifdef VDRWI_WEBDAV
    dd->r.printThisPropFind = &printDirectDownloadPropFind;
#endif
    dd->r.addFragments = NULL;
    dd->r.addVirtualResources = NULL;
    return dd;
}

void *newDirectDownload(resource_t *parent) {
    return initDirectDownload(malloc(sizeof (rsDirectDownload_t)), parent);
}

bool isDirectDownload(const char *path) {
    warn_return_if(!path, false);
    const char *ls = strrchr(path, '/');
    if (!ls) ls = path;
    else ls++;
    if (strcmp(ls, tr("download")) == 0) {
        return true;
    }
    return false;
}

