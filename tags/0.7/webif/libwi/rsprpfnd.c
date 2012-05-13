/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifdef VDRWI_WEBDAV

#include <klone/utils.h>
#include <time.h>
#include <dirent.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <u/libu.h>

#include "conf.h"
#include "resource.h"
#include "rspriv.h"

static void printPropVL(u_string_t *pstr, bool toUTF8, const char *name, const char *value, va_list ap) {
    u_string_t *ptmp;
    if (toUTF8) {
        u_string_create(NULL, 0, &ptmp);
    } else {
        ptmp = pstr;
    }
    u_string_aprintf(ptmp, "<prop><%s>", name);
    if (value) {
        u_string_vaprintf(ptmp, value, ap);
    }
    u_string_aprintf(ptmp, "</%s></prop>\n", name);
    if (toUTF8) {
        u_string_aiconv(pstr, charset[UTF8], charset[ISO885915], u_string_c(ptmp), u_string_len(ptmp));
        u_string_free(ptmp);
    }
}

void printProp(u_string_t *pstr, bool toUTF8, const char *name, const char *value, ...) {
    va_list ap;
    va_start(ap, value);
    printPropVL(pstr, toUTF8, name, value, ap);
    va_end(ap);
}

void printPropStatus(u_string_t *pstr, int status) {
    u_string_aprintf(pstr, "<status>HTTP/1.0 %d %s</status>\n", status, http_get_status_desc(status));
}

void printPropStat(u_string_t *pstr, int status, bool toUTF8, const char *name, const char *value, ...) {
    u_string_cat(pstr, "<propstat>\n");
    va_list ap;
    va_start(ap, value);
    printPropVL(pstr, toUTF8, name, value, ap);
    va_end(ap);
    printPropStatus(pstr, status);
    u_string_cat(pstr, "</propstat>\n");
}

void printWIPropNames(u_string_t *pstr, resource_t *resource, const wdPropFind_t *propFind) {
    u_string_aprintf(pstr, "<%s/>", WIRsType);
}

void printWIPropStats(u_string_t *pstr, resource_t *resource, const wdPropFind_t *propFind) {
    printPropStat(pstr, HTTP_STATUS_OK, false, WIRsType, "%d", resource->type);
}

void printPropFindHref(u_string_t *pstr, wcontext_t *wctx, const char * const path, bool endSlash) {
    u_string_cat(pstr, "<href>" URLWI "/webdav/");
    if (path) {
        u_string_t *ptmp;
        u_string_create(NULL, 0, &ptmp);
        u_string_aiconv(ptmp, charset[UTF8], charset[ISO885915], path, strlen(path));
        if (endSlash) u_string_cat(ptmp, "/");
        u_string_cat(pstr, wctxUrlEncode(wctx, u_string_c(ptmp), "/"));
        u_string_free(ptmp);
    }
    u_string_cat(pstr, "</href>\n");
}

void printMediaResourcePropFind(void *this, wcontext_t *wctx, const char *path, const wdPropFind_t *propFind, u_string_t *pstr, int depth) {
    resource_t *resource = this;
    u_string_cat(pstr, "<response>\n");
    printPropFindHref(pstr, wctx, path, true);
    if (propFind->propName) {
        u_string_cat(pstr, "<propstat>\n");
        u_string_cat(pstr, "<prop>\n");
        u_string_aprintf(pstr, "<%s/>", GetContentLength);
        u_string_aprintf(pstr, "<%s/>", DisplayName);
        u_string_aprintf(pstr, "<%s/>", GetContentType);
        u_string_aprintf(pstr, "<%s/>", ResourceType);
        u_string_aprintf(pstr, "<%s/>", WIRsType);
        u_string_cat(pstr, "</prop>\n");
        printPropStatus(pstr, HTTP_STATUS_OK);
        u_string_cat(pstr, "</propstat>\n");
    } else {
        if (propFind->allProps || propFind->props.displayName) {
            printPropStat(pstr, HTTP_STATUS_OK, true, DisplayName, "%s.%s", resource->getDisplayName(resource, false));
        }
        printWIPropStats(pstr, resource, propFind);
        if (propFind->allProps || propFind->props.resourceType) {
            //Aunque sean archivos individuales se representan mediante una coleccion de recursos
            printPropStat(pstr, HTTP_STATUS_OK, false, ResourceType, "<collection/>");
        }
    }
    u_string_cat(pstr, "</response>\n");
}

void printResourcePropFind(wcontext_t *wctx, const wdPropFind_t *propFind, u_string_t *pstr, int depth) {
    int hostId = wctx->rsHostId;
    const char *path = wctx->rsPath;
    resource_t *resource = getResource(hostId, path, wctx->rsType, depth, false, true);
    if (resource) {
        if (resource->printThisPropFind) {
            resource->printThisPropFind(resource, wctx, path, propFind, pstr, depth);
            if (resource->children.length > 0) {
                int i;
                for (i = 0; i < resource->children.length; i++) {
                    resource_t *child = resource->children.pentry[i];
                    char *path2 = extendPath(path, child->name);
                    child->printThisPropFind(child, wctx, path2, propFind, pstr, lowerDepth(depth));
                    free(path2);
                }
            }
        } else {
            warn("printPropFind indefinido");
        }
        freeResourceChain(resource);
    }
}

void printRsCollectionPropFind(void *this, wcontext_t *wctx, const char *path, const wdPropFind_t *propFind, u_string_t * pstr, int depth) {
    resource_t *resource = this;
    u_string_cat(pstr, "<response>\n");
    printPropFindHref(pstr, wctx, path, true);
    if (propFind->propName) {
        u_string_cat(pstr, "<propstat>\n");
        u_string_cat(pstr, "<prop>\n");
        u_string_aprintf(pstr, "<%s/>", DisplayName);
        u_string_aprintf(pstr, "<%s/>", ResourceType);
        printWIPropNames(pstr, resource, propFind);
        u_string_cat(pstr, "</prop>\n");
        printPropStatus(pstr, HTTP_STATUS_OK);
        u_string_cat(pstr, "</propstat>\n");
    } else {
        if (propFind->allProps || propFind->props.displayName) {
            printPropStat(pstr, HTTP_STATUS_OK, true, DisplayName, resource->getDisplayName(resource, false));
        }
        printWIPropStats(pstr, resource, propFind);
        if (propFind->allProps || propFind->props.resourceType) {

            printPropStat(pstr, HTTP_STATUS_OK, false, ResourceType, "<collection/>");
        }
    }
    u_string_cat(pstr, "</response>\n");
}

#endif