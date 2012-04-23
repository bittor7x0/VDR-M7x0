/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifdef VDRWI_WEBDAV

#ifndef WI_WEBDAV_H
#define	WI_WEBDAV_H

#include <expat.h>
#include <stdbool.h>
#include <u/toolbox.h>

#include "misc.h"

#ifdef	__cplusplus
extern "C" {
#endif
    extern const char *PropName;
    extern const char *AllProps;
    extern const char *CreationDate;
    extern const char *GetContentLength;
    extern const char *CreationDate;
    extern const char *GetContentLength;
    extern const char *DisplayName;
    extern const char *Source;
    extern const char *GetContentLanguage;
    extern const char *GetContentType;
    extern const char *Executable;
    extern const char *GetLastModified;
    extern const char *GetETag;
    extern const char *SupportedLock;
    extern const char *LockDiscovery;
    extern const char *ResourceType;
    extern const char *WIRsType;

    typedef struct wdProps_s {
        bool creationDate;
        bool getContentLength;
        bool displayName;
        bool source;
        bool getContentLanguage;
        bool getContentType;
        bool executable;
        bool getLastModified;
        bool getETag;
        bool supportedLock;
        bool lockDiscovery;
        bool resourceType;
    } wdProps_t;

    enum wdPropFindStatus_e {
        PFS_error = -1,
        PFS_start,
        PFS_propfind,
        PFS_prop,
        PFS_propname = PFS_prop,
        PFS_propp1,
        PFS_propp2,
    };

    typedef struct wdPropFind_s {
        enum wdPropFindStatus_e status;
        wdProps_t props;
        bool allProps;
        bool propName;
        int depth;
    } wdPropFind_t;

    void propFind(wcontext_t * wctx);

#ifdef	__cplusplus
}
#endif

#endif	/* WI_WEBDAV_H */

#endif /* VDRWI_WEBDAV */