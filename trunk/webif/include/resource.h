/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_RESOURCE_H
#define WI_RESOURCE_H

#include "fragment.h"
#include "i18n.h"
#include "misc.h"
#ifdef VDRWI_WEBDAV
#include "webdav.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif 

    typedef struct resource_s resource_t;

    const char *getResourceURL(resource_t *resource, wcontext_t *wctx, const char * const path, pageAction_t action
            , bool urlEncode, bool htmlEncode);
    void printResourceControls(resource_t *resource, wcontext_t *wctx, const char * const path, const char* Browse, const char *Play, const char *Edit, const char *Delete);
#ifdef VDRWI_WEBDAV
    void printResourcePropFind(wcontext_t *wctx, const wdPropFind_t *propFind, u_string_t *pstr, int depth);
#endif
    void redirectToResource(wcontext_t *wctx);
    void serveResourcePage(wcontext_t *wctx);

#ifdef __cplusplus
}
#endif 

#endif
