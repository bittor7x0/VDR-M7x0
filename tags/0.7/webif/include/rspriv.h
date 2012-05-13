/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_RESOURCE_PRIV_H
#define WI_RESOURCE_PRIV_H

#include "conf.h"
#ifdef VDRWI_WEBDAV
#include "rsprpfnd.h"
#endif

#define lowerDepth(depth) ((depth > 0) ? depth - 1 : depth)

#ifdef __cplusplus
extern "C" {
#endif 

    typedef void freeFunction_t(void *this);
    typedef uint64_t getContentLengthFunction_t(void *this, const char *const path);
    typedef char *getDisplayNameFunction_t(void *this, bool canDelegate);
    typedef bool getContentFunction_t(void * const this, const char *const path);
    typedef void prepareFunction_t(void *this, int level);
    typedef bool renameResourceFunction_t(void *this, wcontext_t *wctx, const char * const path);
    typedef bool deleteResourceFunction_t(void *this, wcontext_t *wctx, const char * const path);
    typedef void serveResourceFunction_t(void *this, wcontext_t *wctx, const char *const path);
    typedef void printHtmlHierarchyFunction_t(void *this, wcontext_t *wctx, const char *const path, int level, int depth);
    typedef void printHtmlListFunction_t(void *this, wcontext_t *wctx, const char *const path, int level, int depth);
    typedef bool printHtmlInsteadOfParentFunction_t(void *this, wcontext_t *wctx, bool hierarchical, const char *const parentPath, int level);
    typedef void printControlsFunction_t(void *this, wcontext_t *wctx, const char *const path);
    typedef void printContentFuncion_t(void *this, wcontext_t *wctx, const char *const path, int level, int depth);
    typedef void printEditFormFunction_t(void *this, wcontext_t *wctx, const char *const path);
    typedef bool confirmEditFormFunction_t(void *this, wcontext_t *wctx, const char * const path, char **newPath);
    typedef bool acceptsLocalResourceFunction_t(void *this, const char *childName);
    typedef void addLocalResourcesFunction_t(void *this, const int hostId, const char *const thisFullPath, int depth);
    typedef resource_t *addVirtualResourceFunction_t(void *this, const char *rsName);
    typedef void addVirtualResourcesFunction_t(void *this, int depth);
    typedef void addFragmentsFunction_t(void *this, fragmentList_t *fragments, const char *const path);
#ifdef VDRWI_WEBDAV
    typedef void printPropFindFunction_t(void *this, wcontext_t *wctx, const char *const path, const wdPropFind_t *propFind, u_string_t *pstr, int depth);
#endif

    typedef struct resourceList_s {
        resource_t **pentry;
        int length;
    }
    resourceList_t;

    struct resource_s {
        int hostId;
        resource_t *parent;
        resourceList_t children;
        const char *mimeType;
        const char *cssPfx;
        resourceType_t type;
        char *name;
        char *displayName;
        int numVdrRecs;
        bool isGetContentDone;
        int level;
        time_t tcreationDate;
        char *domId;
        freeFunction_t *free;
        renameResourceFunction_t *renameResource;
        deleteResourceFunction_t *deleteResource;
        getContentLengthFunction_t *getContentLength;
        getDisplayNameFunction_t *getDisplayName;
        prepareFunction_t *prepare;
        serveResourceFunction_t *serve;
        printHtmlHierarchyFunction_t *printHtmlHierarchy;
        printHtmlListFunction_t *printHtmlList;
        printHtmlInsteadOfParentFunction_t *printHtmlInsteadOfParent;
        printControlsFunction_t *printControls;
        printEditFormFunction_t *printEditForm;
        confirmEditFormFunction_t *confirmEditForm;
        getContentFunction_t *getContent;
        printContentFuncion_t *printContent;
        acceptsLocalResourceFunction_t *acceptsLocalResource;
        addVirtualResourcesFunction_t *addVirtualResources;
        addFragmentsFunction_t *addFragments;
#ifdef VDRWI_WEBDAV
        printPropFindFunction_t *printThisPropFind;
#endif
    };

    /**
     * If the video0 of the host to which the resource belongs, is accessible, converts
     * the path (relative to video0) in a full path.
     * 
     * @param resource
     * @param path path relative to video0 of the resource
     * @return absolute path of the resource
     */
    char *getAbsolutePath(resource_t *resource, const char *const path);

    bool addFileFragment(fragment_t * const f, fragmentList_t *fragments, const char * fullpath
            , const char * name, const char *mimeType);

    char *getResourceDisplayName(void *resource, bool canDelegate);

    void printMediaResourceHtmlHierarchy(void *this, wcontext_t *wctx, const char * const path, int level, int depth);

    void printMediaResourceHtmlList(void *this, wcontext_t *wctx, const char * const path, int level, int depth);

    void initResourceList(resourceList_t *resources);

    void freeResourceList(resourceList_t *resources);

    resource_t *initResource(resource_t *resource, int hostId, resourceType_t type, resource_t *parent, const char *name
            , const char * mimeType, time_t *pcreationDate);

    void *getResource(const int hostId, const char * rsPath, resourceType_t rsType, int depth, bool assumeExists, bool addVirtuals);

    resourceType_t getResourceType(const char *const rsName, bool isFile, bool lastSegment);

    resource_t *findOrAddNewResource(resource_t * const parent, const int hostId, const char *const childName
            , resourceType_t childType, time_t *pcreationDate);

    bool isVirtualPlaylist(const char *path, playlistType_t *pplType);

    bool isDirectDownload(const char *path);

    resource_t *addVirtualResource(resource_t * const parent, const char *childName);

    void evaluateResourceChainHostId(resource_t * const resource, int hostId);

    resource_t *addResourceChain(resource_t * const resource, int hostId, const char *const rsRelPath, int depth);

    resource_t *addRemoteVdrRecs(resource_t * const root, resource_t *resource, const char *const rsPath, resourceType_t rsType, hostConf_t * const host, int depth);

    void freeResource(void *resource);

    void freeResourceChain(resource_t *last);

    void *newVdrRec(const int hostId, resource_t *parent, const char * name);

    void *newMediaFile(const int hostId, resource_t *parent, const char * name, time_t *pcreationDate);

    void *newRsCollection(resource_t *parent, const int hostId, const char * name, time_t *pcreationDate);

    void *newVirtPlaylist(resource_t *parent, const playlistType_t plType);

    void *newDirectDownload(resource_t *parent);

#ifdef __cplusplus
}
#endif 

#endif
