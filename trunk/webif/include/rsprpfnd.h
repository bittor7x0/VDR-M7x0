/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_RSPRPFND_H
#define WI_RSPRPFND_H

#ifdef VDRWI_WEBDAV

#include "conf.h"
#include "resource.h"
#include "webdav.h"

#ifdef __cplusplus
extern "C" {
#endif 

    void printProp(u_string_t *pstr, bool toutf8, const char *name, const char *value, ...) 
    __attribute__ ((format (printf, 4, 5)));

    void printPropStatus(u_string_t *pstr, int status);

    void printPropStat(u_string_t *pstr, int status, bool toUTF8, const char *name, const char *value, ...) 
    __attribute__ ((format (printf, 5, 6)));

    void printWIPropNames(u_string_t *pstr, resource_t *resource, const wdPropFind_t *propFind);

    void printWIPropStats(u_string_t *pstr, resource_t *resource, const wdPropFind_t *propFind);

    void printPropFindHref(u_string_t *pstr, wcontext_t *wctx, const char *const path, bool endSlash);

    void printRsCollectionPropFind(void *this, wcontext_t *wctx, const char *path, const wdPropFind_t *propFind, u_string_t * pstr, int depth);
    
    void printMediaResourcePropFind(void *this, wcontext_t *wctx, const char *path, const wdPropFind_t *propFind, u_string_t *pstr, int depth);

    void printVirtPlaylistPropFind(void *this, wcontext_t *wctx, const char *path, const wdPropFind_t *propFind, u_string_t *pstr, int depth);
    
#ifdef __cplusplus
}
#endif 

#endif

#endif
