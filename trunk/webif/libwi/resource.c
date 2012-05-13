/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <klone/io.h>
#include <klone/response.h>
#include <klone/utils.h>
#include <time.h>
#include <dirent.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <u/libu.h>

#include "conf.h"
#include "playlist.h"
#include "resource.h"
#include "rspriv.h"
#include "streaming.h"

bool isLocalResource(resource_t *resource) {
    hostConf_t *host = getHost(resource->hostId);
    return (host && host->video0 && host->video0[0]);
}

/**
 * If the video0 of the host to which the resource belongs, is accessible, converts
 * the path (relative to video0) in a full path.
 * 
 * @param resource
 * @param path path relative to video0 of the resource
 * @return absolute path of the resource
 */
char *getAbsolutePath(resource_t *resource, const char * const path) {
    hostConf_t *host = getHost(resource->hostId);
    warn_return_if(!host || !host->video0 || host->video0[0] != '/', NULL);
    return extendPath(host->video0, path);
}

static bool sameMimeType(fragmentList_t *fragments, const char *mimeType) {
    if (mimeType == NULL) return false;
    if (fragments->length > 0) {
        return sameString(fragments->mimeType, mimeType);
    } else {
        return true;
    }
}

bool addFileFragment(fragment_t * const f, fragmentList_t *fragments, const char * fullpath, const char * name, const char *mimeType) {
    if (!sameMimeType(fragments, mimeType)) {
        return false;
    }
    struct stat st;
    initFragment(f);
    f->path = extendPath(fullpath, name);
    if (stat(f->path, &st) == 0) {
        if (st.st_size > 0) {
            if (fragments->length == 0) {
                fragments->mimeType = mimeType;
            }
            f->size = st.st_size;
            f->start = fragments->totalSize;
            f->end = f->start + f->size - 1;
            fragments->totalSize = f->end + 1;
            fragments->length++;
            return true;
        } else {
            info("file %s is empty", f->path);
            freeFragment(f);
            return false;
        }
    } else {
        warn("file %s error %s", f->path, strerror(errno));
        freeFragment(f);
        return false;
    }
}

char *getResourceDisplayName(void *this, bool canDelegate) {
    resource_t *resource = this;
    return (resource->displayName != NULL) ? resource->displayName : resource->name;
}

/**
 * Returns the URL where a resource will be streamed or, if linkToStream is false, the URL of
 * a playlist which has the resource's URL as only member.
 * @param wctx
 * @param hostId
 * @param path
 * @param linkToStream
 * @return 
 */
const char *getResourceURL(resource_t *resource, wcontext_t *wctx, const char * const rsPath, pageAction_t action
        , bool urlEncode, bool htmlEncode) {
    const char *url = NULL;
    bool isLocal = true;
    char *epath = NULL;
    if (rsPath) {
        if (urlEncode) {
            crit_goto_if((epath = strdup(wctxUrlEncode(wctx, rsPath, NULL))) == NULL, outOfMemory);
        } else {
            epath = (char *) rsPath;
        }
    }
    const char *wiAddr = getHostHttpAddr(NULL, wctx); //request will come, by default, to this webif;
    if (action == PA_SERVE_RSR) {
        //request should go to a webif in the same host the resource is
        hostConf_t *wiHost = getHost(resource->hostId);
        if (!wiHost && resource->type != RT_VIRTUAL) {
            warn_goto_if(!wiHost, exit);
            warn_goto_if(wiHost->webifVersion < 1, exit);
        }
        wiAddr = getHostHttpAddr(wiHost, wctx);
        isLocal = isLocalhost(wiHost);
        if (!isLocal && wiHost->webifVersion < 2) {
            url = wctxBufferPrintf(wctx, "http://%s/streamrec.kl1?path=%s", wiAddr, (epath) ? epath : "");
        }
    }
    if (action == PA_GET_RSR_CONTENT) {
        //request should go to a webif in the same host the resource is, only if it has webif>1
        hostConf_t *wiHost = getHost(resource->hostId);
        if (wiHost && wiHost->webifVersion > 1) {
            wiAddr = getHostHttpAddr(wiHost, wctx);
            isLocal = isLocalhost(wiHost);
        }
    }
    if (!url) {
        url = wctxBufferPrintf(wctx, (htmlEncode)
                ? "http://%s/" URLWI_PA(AMP) AMP "path=%s" AMP "type=%d" AMP "hostId=%d"
                : "http://%s/" URLWI_PA("&") "&" "path=%s" "&" "type=%d" "&" "hostId=%d"
                , wiAddr, PN_RESOURCES, action, (epath) ? epath : "", resource->type, (isLocal) ? resource->hostId : HI_ANY_HOST);
    }
exit:
    if (epath != rsPath) free(epath);
    return url;
outOfMemory:
    exit(EXIT_FAILURE);
}

uint64_t getContentLength(void *this, const char * const path) {
    resource_t *resource = this;
    uint64_t contentLength = 0;
    if (resource) {
        fragmentList_t fragments;
        initFragmentList(&fragments);
        resource->addFragments(resource, &fragments, path);
        contentLength = fragments.totalSize;
        freeFragmentList(&fragments);
    }
    return contentLength;
}

void initResourceList(resourceList_t *resources) {
    resources->length = 0;
    resources->pentry = NULL;
}

void freeResourceList(resourceList_t *resources) {
    int i;
    for (i = 0; i < resources->length; i++) {
        resource_t *resource = resources->pentry[i];
        if (resource) resource->free(resource);
    }
    free(resources->pentry);
    initResourceList(resources);
}

resource_t *findOrAddNewResource(resource_t * const parent, const int hostId, const char *const childName
        , resourceType_t childType, time_t *pcreationDate) {
    //dbg("addResource(%s)", rsName);
    resource_t *resource = NULL;
    if (childType == RT_COLLECTION && parent) {
        int i;
        resource_t *child;
        for (i = 0; i < parent->children.length; i++) {
            child = parent->children.pentry[i];
            if (strcmp(childName, child->name) == 0) {
                warn_if(child->type != childType);
                if (child->type == RT_COLLECTION) {
                    resource = child;
                    switch (resource->hostId) {
                        case HI_NOT_ASSIGNED:
                            resource->hostId = hostId;
                            break;
                        case HI_ANY_HOST:
                            break;
                        default:
                            if (resource->hostId != hostId) {
                                resource->hostId = HI_ANY_HOST;
                            } else {
                                dbg("ya existe %s", childName);
                            }
                            break;
                    }
                }
                break;
            }
        }
    }
    if (!resource) {
        switch (childType) {
            case RT_COLLECTION:
                return newRsCollection(parent, hostId, childName, pcreationDate);
            case RT_VDR_REC:
                return newVdrRec(hostId, parent, childName);
            case RT_MEDIA_FILE:
                return newMediaFile(hostId, parent, childName, pcreationDate);
            default:
                break;
        }
    }
    return resource;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

resource_t *addVirtualResource(resource_t * const parent, const char *childName) {
    playlistType_t plType;
    if (isVirtualPlaylist(childName, &plType)) {
        return newVirtPlaylist(parent, plType);
    }
    return NULL;
}

static void addLocalResources(resource_t * const parent, hostConf_t *host, const char *thisFullPath, int depth);

static resource_t *addLocalResource(resource_t * const parent, hostConf_t *host, const char *const fullPathFE, const char *nameFE, int depth) {
    resource_t *resource = NULL;
    if (fullPathFE != NULL) {
        struct stat sstat;
        bool isFile = false;
        errno = 0;
        if (stat(fullPathFE, &sstat) == 0) {
            isFile = S_ISREG(sstat.st_mode);
            resourceType_t type = getResourceType(nameFE, isFile, isFile);
            if (type != RT_UNKNOWN) {
                char *name = (char *)nameFE;
                if (webifConf.filesystemEncoding != ISO885915) {
                    name = stringIconv(ISO885915, webifConf.filesystemEncoding, name, strlen(name));
                }

                resource = findOrAddNewResource(parent, host->id, name, type, &sstat.st_mtime);
                if (resource) {
                    if (resource->getContent && resource->type != RT_COLLECTION) {
                        int video0Len = strlen(host->video0) + 1;
                        resource->getContent(resource, fullPathFE + video0Len);
                    }
                    if (resource->acceptsLocalResource && fullPathFE && depth != 0) {
                        int depth2 = lowerDepth(depth);
                        addLocalResources(resource, host, fullPathFE, depth2);
                    }
                }
                if (name != nameFE) {
                    free(name);
                }
            }
        } else {
            warn_strerror(errno);
            errno = 0;
        }
    }
    return resource;
}

static void addLocalResources(resource_t * const parent, hostConf_t *host, const char *const parentFullPath, int depth) {
    warn_return_if(parent->acceptsLocalResource == NULL,);
    parent->isGetContentDone = true;
    DIR *dir = opendir(parentFullPath);
    warn_return_if(dir == 0,);
    struct dirent* dirE;
    while (0 != (dirE = readdir(dir))) {
        const char *childName = dirE->d_name;
        if (parent->acceptsLocalResource(parent, childName)) {
            char *childFullPath = extendPath(parentFullPath, childName);
            addLocalResource(parent, host, childFullPath, childName, depth);
            free(childFullPath);
        }
    }
    closedir(dir);
}

void addVirtualPlaylists(void *this, int depth) {
    newVirtPlaylist(this, PL_M3U);
    newVirtPlaylist(this, PL_XSPF);
    newDirectDownload(this);
}

resourceType_t getResourceType(const char *const rsName, bool isFile, bool lastSegment) {
    if (isFile) {
        const char *mimeType = u_guess_mime_type(rsName); //TODO devolver valor
        if (mimeType && (
                strncmp(mimeType, "audio", 5) == 0
                || strncmp(mimeType, "video", 5) == 0
                || (strncmp(mimeType, "application", 11) == 0
                && strcmp(mimeType, "application/octet-stream") != 0)
                ))
            return RT_MEDIA_FILE;
    } else {
        if (rsName) {
            const char *ext = strrchr(rsName, '.');
            if (boolean(ext && strcmp(ext, ".del") == 0)) {
                return RT_VDR_DELETED;
            }
            return (ext && strcmp(ext, ".rec") == 0) ? RT_VDR_REC : RT_COLLECTION;
        } else {
            return RT_COLLECTION;
        }
    }
    return RT_UNKNOWN;
}

void freeResource(void *this) {
    resource_t * resource = this;
    freeResourceList(&resource->children);
    free(resource->name);
    free(resource->displayName);
    free(resource->domId);
    free(this);
}

void freeResourceChain(resource_t *last) {
    resource_t *resource = last;
    while (resource->parent) resource = resource->parent;
    resource->free(resource);
}

static void streamResource(void *this, wcontext_t *wctx, const char *const path) {
    resource_t *resource = this;
    if (resource) {
        fragmentList_t fragments;
        initFragmentList(&fragments);
        resource->addFragments(resource, &fragments, path);
        streamFragments(wctx, &fragments);
        freeFragmentList(&fragments);
    }
}

static void prepareResource(void *this, int level) {
    resource_t *resource = this;
    resource->level = level;
}

resource_t *initResource(resource_t *resource, const int hostId, resourceType_t type
        , resource_t * parent, const char *name, const char *mimeType, time_t *pcreationDate) {
    crit_goto_if(resource == NULL, outOfMemory);
    resource->hostId = hostId;
    resource->type = type;
    initResourceList(&resource->children);
    if (name) {
        resource->name = strdup(name);
        crit_goto_if(resource->name == NULL, outOfMemory);
    } else {
        resource->name = NULL;
    }
    resource->mimeType = mimeType;
    resource->parent = parent;
    if (parent) {
        parent->children.pentry = realloc(parent->children.pentry, (++parent->children.length) * sizeof (resource_t *));
        crit_goto_if(parent->children.pentry == NULL, outOfMemory);
        parent->children.pentry[parent->children.length - 1] = resource;
    }
    resource->displayName = NULL;
    resource->prepare = &prepareResource;
    resource->isGetContentDone = false;
    resource->level = 0;
    resource->tcreationDate = (pcreationDate) ? *pcreationDate : time(NULL);
    resource->domId = NULL;
    resource->free = &freeResource;
    resource->renameResource = NULL;
    resource->deleteResource = NULL;
    resource->acceptsLocalResource = NULL;
    resource->addVirtualResources = &addVirtualPlaylists;
    resource->getContentLength = &getContentLength;
    resource->getDisplayName = &getResourceDisplayName;
    resource->serve = &streamResource;
    resource->printHtmlHierarchy = NULL;
    resource->printHtmlList = NULL;
    resource->printHtmlInsteadOfParent = NULL;
    resource->printControls = NULL;
    resource->printEditForm = NULL;
    resource->confirmEditForm = NULL;
    resource->getContent = NULL;
    resource->printContent = NULL;
    return resource;
outOfMemory:
    exit(EXIT_FAILURE);
}

static bool acceptsLocalResource(void *this, const char *childName) {
    return (childName[0] != '.');
}

void evaluateResourceChainHostId(resource_t * const resource, int hostId) {
    // assing unassigned resources in chain to this host
    resource_t *r = resource;
    while (r) {
        if (r->hostId == HI_NOT_ASSIGNED)
            r->hostId = hostId;
        else if (r->hostId >= 0 && hostId >= 0 && r->hostId != hostId)
            r->hostId = HI_ANY_HOST;
        r = r->parent;
    }
}

/**
 * If fullPath contains thisPath, this procedure creates a chain of resources (one per extra path segment)
 * adds the chain first resource as child to this, and returns the last resource in the chain.
 * If (a fraction of ) the chain is already present, it is reused.
 * If 'this' is NULL, the first resource in the chain will belong to the first segment in the path.
 * @param this The resource to which the chain will be added, or NULL.
 * @param hostId The id of the host to which the new resources will belong.
 * @param thisPath The path to 'this' resource
 * @param rsRelPath The path to the 'last' resource, relative to 'this' resource.
 * @paramm depth The maximum number of segments that will be added to the chain.
 * @return The 'last' resource.
 */
resource_t *addResourceChain(resource_t * const this, int hostId, const char *const rsRelPath, int depth) {
    warn_return_if(this == NULL || rsRelPath == NULL || rsRelPath[0] == 0, this);
    resource_t *resource = this;
    evaluateResourceChainHostId(resource, hostId);
    int depth2 = depth;
    char *d = (char *) rsRelPath, *nd;
    // add descendants
    while (d[0]) {
        while (d[0] == '/') d++;
        if (d[0]) {
            nd = strchr(d, '/');
            if (nd) nd[0] = 0;
            resourceType_t rsType = getResourceType(d, false, nd == NULL);
            resource = findOrAddNewResource(resource, hostId, d, rsType, NULL);
            if (resource == NULL || depth2 == 0) {
                return resource;
            }
            depth2 = lowerDepth(depth2);
            if (nd) {
                nd[0] = '/';
                d = nd + 1;
            } else {
                break;
            }
        }
    }
    return resource;
}

resource_t *addRemoteResources(resource_t * const root, resource_t *resource, const char *const rsPath, resourceType_t rsType, hostConf_t * const host, int depth) {
    return addRemoteVdrRecs(root, resource, rsPath, rsType, host, depth);
}

void addVirtualResources(resource_t * const resource, int depth) {
    if (resource) {
        if (resource->addVirtualResources) {
            resource->addVirtualResources(resource, depth);
        }
        depth = lowerDepth(depth);
        if (depth != 0) {
            int i;
            resource_t *child;
            for (i = 0; i < resource->children.length; i++) {
                child = resource->children.pentry[i];
                addVirtualResources(child, depth);
            }
        }
    }
}

/**
 * @param resource current value of resource or NULL
 * @param colPath
 * @param rsName
 * @param onlyLocal
 * @param depth: 0=resource, 1=resource+children, 2=resource+children+grandchildren, ..., -1=infinite
 * @return 
 */
static void *getRealResource(resource_t *resource, const int hostId, const char * rsPath, resourceType_t rsType, int depth, bool assumeExists, bool addVirtuals) {
    resource_t *root = newRsCollection(NULL, HI_ANY_HOST, NULL, NULL);
    root->displayName = strdup(tr("recordings"));
    int rootPathLength = 0;
    int nhosts = webifConf.hosts.length;
    if (nhosts == 0) return NULL;
    bool *hostParsed = calloc(nhosts, sizeof ( bool));
    crit_goto_if(hostParsed == NULL, outOfMemory);
    int h;
    hostConf_t * host;
    for (h = 0, host = webifConf.hosts.entry; h < nhosts; h++, host++) {
        if (!hostParsed[h] && host->video0[0] && (hostId == HI_ANY_HOST || hostId == host->id)) {
            char *rsPathFE; //filesystem encoded
            if (rsPath && isLocalhost(host) && webifConf.filesystemEncoding != ISO885915) {
                rsPathFE = stringIconv(webifConf.filesystemEncoding, ISO885915, rsPath, strlen(rsPath));
            } else {
                rsPathFE = (char *) rsPath;
            }
            char *rsFullPathFE = extendPath(host->video0, rsPathFE);
            resource_t *resourceInHost = root;
            if (rsPath) {
                char *d = rsFullPathFE + strlen(host->video0), *nd;
                while (resourceInHost) {
                    while (*d == '/') d++;
                    nd = strchr(d, '/');
                    if (nd) nd[0] = 0;
                    resourceInHost = addLocalResource(resourceInHost, host, rsFullPathFE, d, 0);
                    if (nd) {
                        nd[0] = '/';
                        d = nd + 1;
                    } else {
                        break;
                    }
                }
            }
            if (rsPathFE && rsPathFE != rsPath) {
                free(rsPathFE);
            }
            if (resourceInHost) {
                resource = resourceInHost;
                if (resource->type == RT_COLLECTION) {
                    int depth2 = (depth == -1) ? -1 : (depth > 3) ? lowerDepth(depth) : 2;
                    addLocalResources(resource, host, rsFullPathFE, depth2);
                }
            }
            free(rsFullPathFE);
            hostParsed[h] = true;
            int h2;
            hostConf_t *host2;
            for (h2 = h + 1, host2 = host + 1; h2 < webifConf.hosts.length; h2++, host2++) {
                if (!hostParsed[h2] && host2->video0[0] && strcmp(host->video0, host2->video0) == 0) {
                    hostParsed[h2] = true;
                }
            }
            if (resourceInHost && resourceInHost->type != RT_COLLECTION) { //TODO estudiar esto
                break;
            }
        }
    }
    if (!resource && (assumeExists || (rsType == RT_COLLECTION && depth == 0))) {
        resource = addResourceChain(root, (assumeExists) ? hostId : HI_ANY_HOST, rsPath + rootPathLength, -1);
    } else {
        if ((resource && resource->type == RT_COLLECTION && depth != 0) || !resource) {
            for (h = 0, host = webifConf.hosts.entry; h < nhosts; h++, host++) {
                if (!hostParsed[h] && !host->video0[0] && (hostId == HI_ANY_HOST || hostId == host->id)) {
                    resource = addRemoteResources(root, resource, rsPath, rsType, host, depth);
                }
            }
        }
    }
    if (!resource || resource->hostId == HI_NOT_ASSIGNED) {
        root->free(root);
        root = NULL;
        resource = NULL;
    }
    free(hostParsed);
    if (resource && depth != 0) {
        if (addVirtuals && resource->type != RT_VIRTUAL) {
            addVirtualResources(resource, lowerDepth(depth));
        }
    }
    if (root) {
        root->prepare(root, 0);
    }
    return resource;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void *getResource(const int hostId, const char * rsPath, resourceType_t rsType, int depth, bool assumeExists, bool addVirtuals) {
    while (rsPath && rsPath[0] == '/') rsPath++;
    if ((rsType == RT_VIRTUAL || rsType == RT_UNKNOWN) && rsPath) {
        resource_t *virtual = NULL;
        char *ls = strrchr((char *) rsPath, '/');
        if (ls) ls[0] = 0;
        const char *rsName = (ls) ? ls + 1 : rsPath;
        playlistType_t plType;
        if (isVirtualPlaylist(rsName, &plType)) {
            resource_t *parent = getRealResource(NULL, hostId, (ls) ? rsPath : NULL, RT_UNKNOWN, 0, true, false);
            virtual = (parent) ? newVirtPlaylist(parent, plType) : NULL;
        } else if (isDirectDownload(rsName)) {
            resource_t *parent = getRealResource(NULL, hostId, (ls) ? rsPath : NULL, RT_UNKNOWN, 0, true, false);
            virtual = (parent) ? newDirectDownload(parent) : NULL;
        }
        if (ls) ls[0] = '/';
        if (virtual) return virtual;
    }
    return getRealResource(NULL, hostId, rsPath, rsType, depth, assumeExists, addVirtuals);
}

static void addRsCollectionFragments(void *this, fragmentList_t * const fragments, const char *const path) {
    resource_t *resource = this;
    int i;
    for (i = 0; i < resource->children.length; i++) {
        resource_t *child = resource->children.pentry[i];
        if (child->type != RT_VIRTUAL && child->addFragments) {
            char *path2 = extendPath(path, child->name);
            child->addFragments(child, fragments, path2);
            free(path2);
        }
    }
}

static int comparePResources(const void * pa, const void * pb) {
    resource_t *ra = *(resource_t **) pa;
    resource_t *rb = *(resource_t **) pb;
    if (ra->type != rb->type) {
        return ra->type - rb->type;
    }
    const char *dna = ra->getDisplayName(ra, false);
    const char *dnb = rb->getDisplayName(rb, false);
    return strcmp(dna, dnb);
}

static void prepareRsCollection(void * this, int level) {
    resource_t *collection = this;
    collection->level = level;
    if (collection->children.length > 0) {
        if (!collection->isGetContentDone) {
            collection->isGetContentDone = true;
        }
        int i;
        for (i = 0; i < collection->children.length; i++) {
            resource_t *child = collection->children.pentry[i];
            if (child->prepare) child->prepare(child, level + 1);
        }
        qsort(collection->children.pentry, collection->children.length, sizeof (resource_t *), comparePResources);
    }
    if (collection->parent) {
        collection->parent->numVdrRecs += collection->numVdrRecs;
    }
}

static bool getRsCollectionContent(void *this, const char *const path) {
    resource_t *resource = this;
    getRealResource(resource, resource->hostId, path, RT_COLLECTION, 1, false, false);
    return true;
}

static void printResourceHostP(resource_t *resource, wcontext_t *wctx) {
    if (wctx->conf.displayHostId && webifConf.hosts.length > 1) {
        hostConf_t *host = getHost(resource->hostId);
        if (host && (host->name[0] || host->ip[0])) {
            WCTX_IPRINTF(
                    "<p class=\"host field\">"
                    "<span class=\"label\">Host:</span>&nbsp;<span class=\"value\">%s</span>"
                    "</p>\n", (host->name[0]) ? host->name : host->ip);
        }
    }
}

static void printRecursiveDisplayName(wcontext_t *wctx, resource_t *resource, bool displayCurrent) {
    if (resource->parent) {
        printRecursiveDisplayName(wctx, resource->parent, true);
        if (displayCurrent) {
            WCTX_PRINTF("%s<span class=\"dir\">%s</span>"
                    , (resource->parent->parent == NULL) ? "" : " / "
                    , wctxHtmlEncode(wctx, resource->getDisplayName(resource, false)));
        }
    }
}

static void printRecursivePathUL(wcontext_t *wctx, resource_t *resource, u_string_t* spath, bool current) {
    if (resource->parent) {
        printRecursivePathUL(wctx, resource->parent, spath, false);
        u_string_cat(spath, "/");
        u_string_cat(spath, wctxUrlEncode(wctx, resource->name, NULL));
    } else {
        u_string_clear(spath);
    }
    const char *epath = u_string_c(spath);

    if (epath[0] == '/') epath++;
    WCTX_PRINTFT("<li class=\"dir\">%s<a href=\"%s\"%s>", 1
            , (resource->parent) ? " / " : ""
            , getResourceURL(resource, wctx, epath, PA_DEFAULT, false, true)
            , classCurrent[current]);
    WCTX_PRINT(wctxHtmlEncode(wctx, resource->getDisplayName(resource, false)));
    WCTX_PRINT("</a>\n");
    WCTX_IPRINTT("</li>", -1, 0);
}

static void printPathUL(wcontext_t *wctx, resource_t *resource, const char *id) {
    if (id) {
        WCTX_IPRINTF("<ul id=\"%s\" class=\"path\">", id);
    } else {
        WCTX_IPRINT("<ul class=\"path\">");
    }
    u_string_t *path;
    warn_return_if(u_string_create(NULL, 0, &path),);
    printRecursivePathUL(wctx, resource, path, true);
    u_string_free(path);
    WCTX_PRINT("</ul>\n");

    return;
outOfMemory:
    exit(EXIT_FAILURE);
}

static void printResourceDeferredContent(resource_t *resource, wcontext_t *wctx, const char *const path, int level) {
    //info will be retrieved via ajax, don't loose time now
    WCTX_IPRINTF(
            "<div class=\"js-deferred\">"
            "<a class=\"remoteInfoUrl\" href=\"%s" AMP "level=%d\">%s</a>"
            "</div>\n"
            , getResourceURL(resource, wctx, path, PA_GET_RSR_CONTENT, true, true), level, tr("info"));
}

static void printMediaResourceHtmlTitle(resource_t *resource, wcontext_t *wctx, const char * const path, bool hierarchical) {
    WCTX_IPRINTF(STATUS_ICON("expanded") "<span class=\"title%s\">"
            , (resource->getContent && resource->printContent) ? " infoToggler" : "");
    const char *displayName;
    if (hierarchical) {
        WCTX_PRINT(wctxHtmlEncode(wctx, resource->getDisplayName(resource, true)));
    } else {
        bool displayCurrent = (resource->type != RT_VDR_REC || !resource->parent || resource->parent->children.length > 1);
        WCTX_PRINT("<span class=\"path\">");
        printRecursiveDisplayName(wctx, resource, displayCurrent);
        WCTX_PRINT("</span>");
    }
    if (webifConf.displayHostId) {
        WCTX_PRINTF(" <span class=\"host\">[h%d]</span>", resource->hostId);
    }
    WCTX_PRINT("</span>\n");
}

static void printRsCollectionContent(void *this, wcontext_t *wctx, const char *const path, int level, int depth) {
    resource_t *resource = this;
    if (resource->children.length > 0) {
        if (level == 0) {
            WCTX_IPRINTF("<ul id=\"%s\" class=\"coll\">", AJAX_REPLACE_PREFIX("folders"));
        } else {
            WCTX_IPRINT("<ul class=\"coll\">");
        }
        level++;
        int i;
        int depth2 = lowerDepth(depth);
        for (i = 0; i < resource->children.length; i++) {
            resource_t *child = resource->children.pentry[i];
            char *path2 = extendPath(path, child->name);
            child->printHtmlHierarchy(child, wctx, path2, level, depth2);
            free(path2);
        }
        WCTX_PRINT("</ul>\n");
    }
}

static void printResourceControlsForm(resource_t *resource, wcontext_t *wctx, const char * path) {
    if (resource->printControls) {
        warn_ifb(path[0] == '/') path++;
        WCTX_IPRINTT("<form action=\"" URLWI "\" method=\"get\" class=\"itemControls\" >\n", 0, 1);
        WCTX_IPRINTT("<fieldset class=\"hidden\">\n", 0, 1);
        WCTX_IPRINTF("<input type=\"hidden\" name=\"" ARG_PAGE "\" value=\"%d\" />\n", PN_RESOURCES);
        WCTX_IPRINTF("<input type=\"hidden\" name=\"path\" value=\"%s\" />\n", wctxHtmlEncode(wctx, path));
        WCTX_IPRINTF("<input type=\"hidden\" name=\"type\" value=\"%d\" />\n", resource->type);
        WCTX_IPRINTF("<input type=\"hidden\" name=\"hostId\" value=\"%d\" />\n", resource->hostId);
        WCTX_IPRINTF("<input type=\"hidden\" name=\"level\" value=\"%d\" />\n", resource->level);
        WCTX_IPRINTT("</fieldset>\n", -1, 0);
        WCTX_IPRINT("<ul class=\"controls\">");
        resource->printControls(resource, wctx, path);
        WCTX_PRINT("</ul>\n");
        WCTX_IPRINTT("</form>\n", -1, 0);
    }
}

void printResourceControls(resource_t *resource, wcontext_t *wctx, const char * const path, const char* Browse, const char *Play, const char *Edit, const char *Delete) {
    if (Delete && !webifConf.deletionDisabled && resource->deleteResource) {
        WCTX_PRINTT("<li class=\"control\">\n", 1);
        WCTX_IPRINTF(
                "<button type=\"submit\" class=\"delete control ui-button ui-button-icon-only ui-state-default\" name=\"" ARG_ACTION "\" value=\"%d\" title=\"%s\">"
                "<span class=\"ui-button-icon-primary ui-icon ui-icon-trash\"></span>"
                "<span class=\"ui-button-text\">%s</span>"
                "</button>\n", PA_DELETE, Delete, Delete);
        WCTX_IPRINTT("</li>", -1, 0);
    }
    if (Edit && resource->printEditForm) {
        WCTX_PRINTT("<li class=\"control\">\n", 1);
        WCTX_IPRINTF(
                "<button type=\"submit\" class=\"edit control ui-button ui-button-icon-only ui-state-default\" name=\"" ARG_ACTION "\" value=\"%d\" title=\"%s\">"
                "<span class=\"ui-button-icon-primary ui-icon ui-icon-pencil\"></span>"
                "<span class=\"ui-button-text\">%s</span>"
                "</button>\n", PA_EDIT_FORM, Edit, Edit);
        WCTX_IPRINTT("</li>", -1, 0);
    }
    if (Browse) {
        WCTX_PRINTT("<li class=\"control\">\n", 1);
        WCTX_IPRINTF(
                "<button type=\"submit\" class=\"js-no-ajax browse control ui-button ui-button-icon-only ui-state-default\" name=\"" ARG_ACTION "\" value=\"%d\" title=\"%s\">"
                "<span class=\"ui-button-icon-primary ui-icon ui-icon-folder-collapsed\"></span>"
                "<span class=\"ui-button-text\">%s</span>"
                "</button>\n", PA_DEFAULT, Browse, Browse);
        WCTX_IPRINTT("</li>", -1, 0);
    }
    if (Play) {
        WCTX_PRINTT("<li class=\"control\">\n", 1);
        WCTX_IPRINTF(
                "<button type=\"submit\" class=\"js-no-ajax play control ui-button ui-button-icon-only ui-state-default\" name=\"" ARG_ACTION "\" value=\"%d\" title=\"%s\">"
                "<span class=\"ui-button-icon-primary ui-icon ui-icon-play\"></span>"
                "<span class=\"ui-button-text\">%s</span>"
                "</button>\n", PA_SERVE_PLAYLIST, Play, Play);
        WCTX_IPRINTT("</li>", -1, 0);
    }
}

static void printCollectionControls(void *this, wcontext_t *wctx, const char *const path) {
    resource_t *resource = this;
    printResourceControls(resource, wctx, path, tr("browse"), tr("playAll"), NULL, NULL);
}

static void prinCollectionHtmlHierarchy(void *this, wcontext_t *wctx, const char *const path, int level, int depth) {
    resource_t *resource = this;
    dbg_return_if(resource->children.length == 0,);
    if (resource->children.length == 1) {
        resource_t *child = resource->children.pentry[0];
        if (child->printHtmlInsteadOfParent && child->printHtmlInsteadOfParent(child, wctx, true, path, level)) {
            return;
        }
    }
    WCTX_PRINTF("<li class=\"%sitemDiv %sDiv infoSet\">\n"
            , (level == 1) ? "level4Div " : "", resource->cssPfx);
    if (true) {//itemHead
        WCTX_IPRINTFT("<div class=\"%sitemHead %sHead\">\n", 1, 1
                , (level == 1) ? "level4Head " : "", resource->cssPfx);
        printResourceControlsForm(resource, wctx, path);
        if (true) {
            WCTX_IPRINT(STATUS_ICON("expanded") "<span class=\"title infoToggler\">");
            WCTX_PRINT(wctxHtmlEncode(wctx, resource->getDisplayName(resource, true)));
            if (wctx->conf.displayHostId && resource->hostId >= 0) {
                WCTX_PRINTF(" <span class=\"host\">[h%d]</span>", resource->hostId);
            }
            WCTX_PRINT("</span>\n");
        }
        WCTX_IPRINTT("</div>" HTMLCOMENT("itemHead") "\n", -1, 0);
    }
    bool isGetContentDone = resource->isGetContentDone;
    if (isGetContentDone && depth == 0) {
        int i;
        for (i = 0; i < resource->children.length; i++) {
            resource_t *child = resource->children.pentry[i];
            if (child->type == RT_COLLECTION && !child->isGetContentDone && child->children.length == 0) {
                //we don't know if we want to show the child or the grandchildren
                isGetContentDone = false;
                break;
            }
        }
    }
    WCTX_IPRINTFT("<div class=\"%sitem %s infoExtra\">\n", 0, 1
            , (level == 1) ? "level4 " : "", resource->cssPfx);
    if (!isGetContentDone) {
        printResourceDeferredContent(resource, wctx, path, level);
    } else {
        printRsCollectionContent(resource, wctx, path, level, depth);
    }
    printResourceHostP(resource, wctx);
    WCTX_IPRINTT("</div>" HTMLCOMENT("item") "\n", -1, 0);
    WCTX_IPRINTT("</li>" HTMLCOMENT("itemDiv"), -1, 0);

    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

static const char *rsCollectionCssPfx = "folder";

static resource_t * initRsCollection(resource_t *collection, const int hostId, resource_t *parent, const char *name, time_t *pcreationDate) {
    initResource(collection, hostId, RT_COLLECTION, parent, name, NULL, pcreationDate);
    bool isLocal = (pcreationDate != NULL);
    if (true) {
        int l = 0;
        char *s = collection->name;
        if (s) {
            if (s[0] == '%') {
                s++;
            }
            l = strlen(s);
        }
        if (l > 0) {
            //if (isLocal && webifConf.filesystemEncoding != ISO885915) {
            //    collection->displayName = stringIconv(ISO885915, webifConf.filesystemEncoding, s, l);
            //} else {
            collection->displayName = strndup(s, l);
            //}
            crit_goto_if(collection->displayName == NULL, outOfMemory);
            vdrDecode(collection->displayName);
            if (strcmp(collection->name, collection->displayName) == 0) {
                free(collection->displayName);
                collection->displayName = NULL;
            }
        }
    }

    collection->cssPfx = rsCollectionCssPfx;
    collection->prepare = &prepareRsCollection;
    collection->getContent = &getRsCollectionContent;
    collection->acceptsLocalResource = &acceptsLocalResource;
    collection->printHtmlHierarchy = &prinCollectionHtmlHierarchy;
    collection->printContent = &printRsCollectionContent;
    collection->printControls = &printCollectionControls;
    collection->addFragments = &addRsCollectionFragments;
#ifdef VDRWI_WEBDAV
    collection->printThisPropFind = &printRsCollectionPropFind;
#endif
    collection->numVdrRecs = 0;

    return collection;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

void *newRsCollection(resource_t *parent, const int hostId, const char *name, time_t *pcreationDate) {

    return initRsCollection(malloc(sizeof (resource_t)), hostId, parent, name, pcreationDate);
}

static void calcResourcePath(wcontext_t *wctx, resource_t *resource, u_string_t *spath, bool urlEncode) {
    if (resource->parent) {
        calcResourcePath(wctx, resource->parent, spath, urlEncode);
        u_string_cat(spath, "/");
        u_string_cat(spath, (urlEncode) ? wctxUrlEncode(wctx, resource->name, NULL) : resource->name);
    } else {

        u_string_clear(spath);
    }
}

static const char *getResourcePath(wcontext_t *wctx, resource_t *resource, u_string_t *spath, bool urlEncode) {
    calcResourcePath(wctx, resource, spath, urlEncode);
    const char *path = u_string_c(spath);
    if (path && path[0] == '/') path++;

    return path;
}

static void addLeavesRecursive(resource_t *resource, resourceList_t *list) {
    if (resource->type == RT_COLLECTION) {
        int i;
        for (i = 0; i < resource->children.length; i++) {
            resource_t *child = resource->children.pentry[i];
            addLeavesRecursive(child, list);
        }
    } else {

        list->pentry = realloc(list->pentry, (++list->length)*(sizeof (resource_t*)));
        crit_goto_if(!list->pentry, outOfMemory);
        list->pentry[list->length - 1] = resource;
    }
    return;
outOfMemory:
    exit(EXIT_FAILURE);
}

static int comparePath(const resource_t *ra, const resource_t *rb) {
    warn_return_if(!ra, -1);
    warn_return_if(!rb, 1);
    if (ra == rb) return 0;
    if (ra->level > rb->level) {
        return comparePath(ra->parent, rb);
    } else if (ra->level < rb->level) {
        return comparePath(ra, rb->parent);
    } else {
        int compareParents = (ra->parent && rb->parent) ? comparePath(ra->parent, rb->parent) : 0;
        if (compareParents)
            return compareParents;
        else {
            char *na = ra->name;
            char *nb = rb->name;
            if (na[0] == '%') na++;
            if (nb[0] == '%') nb++;

            return strcasecmp(na, nb);
        }
    }
}

static void sortResourceList(resourceList_t * const resources, sortField_t sortField, sortDirection_t sortDirection) {

    int compare(const void *a, const void *b) {
        resource_t *ra = *(resource_t **) a;
        resource_t *rb = *(resource_t **) b;
        int result;
        switch (sortField) {
            case SF_TITLE:
                if ((result = comparePath(ra, rb)) != 0) return sortDirection * result;
                /* nobreak */
            case SF_START:
                if ((result = ra->tcreationDate - rb->tcreationDate) != 0) return sortDirection * result;
                /* nobreak */
            default:
                return 0;
        }
    }
    if (resources->length > 0 && sortField != 0) {

        qsort(resources->pentry, resources->length, sizeof (resource_t *), compare);
    }
}

static void printHtmlList(wcontext_t * wctx) { //TODO incluir todos los tipos
    WCTX_IPRINTFT("<table id=\"%s\" class=\"list\">\n", 0, 1, AJAX_REPLACE_PREFIX("recordings"));
    WCTX_IPRINT("<col class=\"datetime\"/>\n");
    WCTX_IPRINT("<col class=\"content\"/>\n");
    WCTX_IPRINTT("<thead class=\"listHead\">\n", 0, 1);
    WCTX_IPRINTT("<tr>\n", 0, 1);
    printList1TH(wctx, SF_START, tr("date"));
    printList1TH(wctx, SF_TITLE, tr("title"));
    WCTX_IPRINTT("</tr>\n", -1, 0);
    WCTX_IPRINTT("</thead>\n", -1, 0);
    resource_t *root = getResource(HI_ANY_HOST, NULL, RT_UNKNOWN, -1, false, false);
    if (root) {
        u_string_t *spath;
        u_string_create(NULL, 0, &spath);
        resourceList_t list;
        initResourceList(&list);
        addLeavesRecursive(root, &list);
        sortResourceList(&list, wctx->sortField, wctx->sortDirection);
        resource_t *resource;
        int level = 2;
        int depth = -1;
        int i;
        if (list.length > 0) {
            WCTX_IPRINTT("<tbody class=\"listBody\">\n", 0, 1);
            for (i = 0; i < list.length; i++) {
                resource = list.pentry[i];
                if (resource->printHtmlList) {
                    resource->printHtmlList(resource, wctx, getResourcePath(wctx, resource, spath, false), level, depth);
                }
            }
            WCTX_IPRINTT("</tbody>\n", -1, 0);
        }
        free(list.pentry);
        freeResource(root);
        u_string_free(spath);
    }
    WCTX_IPRINTT("</table>" HTMLCOMENT("list") "\n", -1, 0);
}

void printMediaResourceHtmlHierarchy(void *this, wcontext_t *wctx, const char * const path, int level, int depth) {
    resource_t *resource = this;
    WCTX_PRINT("<li");
    if (resource->domId != NULL) {
        WCTX_PRINTF(" id=\"%s\"", resource->domId);
    }
    WCTX_PRINTF(" class=\"%sitemDiv %sDiv infoSet\">\n", (level == 1) ? "level4Div " : "", resource->cssPfx);
    if (true) {//itemHead
        WCTX_IPRINTFT("<div class=\"%sitemHead %sHead\">\n", 1, 1
                , (level == 1) ? "level4Head " : "", resource->cssPfx);
        printResourceControlsForm(resource, wctx, path);
        printMediaResourceHtmlTitle(resource, wctx, path, true);
        WCTX_IPRINTT("</div>" HTMLCOMENT("itemHead") "\n", -1, 0);
    }
    if (resource->printContent && resource->getContent) {
        WCTX_IPRINTFT("<div class=\"%sitem %s infoExtra\">\n", 0, 1
                , (level == 1) ? "level4 " : "", resource->cssPfx);
        if (!resource->isGetContentDone) {
            printResourceDeferredContent(resource, wctx, path, level);
        } else {
            resource->printContent(resource, wctx, path, level, depth);
        }
        printResourceHostP(resource, wctx);
        WCTX_IPRINTT("</div>" HTMLCOMENT("item") "\n", -1, 0);
    }
    WCTX_IPRINTT("</li>" HTMLCOMENT("itemDiv"), -1, 0);
}

void printMediaResourceHtmlList(void *this, wcontext_t *wctx, const char * const path, int level, int depth) {
    resource_t *resource = this;
    WCTX_IPRINT("<tr");
    if (resource->domId != NULL) {
        WCTX_PRINTF(" id=\"%s\"", resource->domId);
    }
    WCTX_PRINTF(" class=\"itemDiv %sDiv\">\n", resource->cssPfx);
    WCTX_INCTAB();
    struct tm screationDate = *localtime(&resource->tcreationDate);
    WCTX_IPRINTF("<td class=\"datetime\">%s %02d:%02d</td>\n"
            , formatDate(&screationDate, 0), screationDate.tm_hour, screationDate.tm_min);
    WCTX_IPRINTT("<td class=\"container infoSet\">\n", 0, 1);
    WCTX_IPRINTFT("<div class=\"itemHead %sHead\">\n", 0, 1, resource->cssPfx);
    printResourceControlsForm(resource, wctx, path);
    printMediaResourceHtmlTitle(resource, wctx, path, false);
    WCTX_IPRINTT("</div>" HTMLCOMENT("itemHead") "\n", -1, 0);
    if (resource->printContent && resource->getContent) {
        WCTX_IPRINTFT("<div class=\"item %s infoExtra\">\n", 0, 1, resource->cssPfx);
        if (!resource->isGetContentDone) {
            printResourceDeferredContent(resource, wctx, path, level);
        } else {
            resource->printContent(resource, wctx, path, level, depth);
        }
        printResourceHostP(resource, wctx);
        WCTX_IPRINTT("</div>" HTMLCOMENT("item") "\n", -1, 0);
    }
    WCTX_IPRINTT("</td>" HTMLCOMENT("itemHeadMain") "\n", -1, 0);
    WCTX_IPRINTT("</tr>" HTMLCOMENT("itemDiv") "\n", -1, 0);
}

const char *RootResourceTypeArg = "rootRsType";
const char *RootResourcePathLengthArg = "rootRsPathLen";
const char *RootResourceHostIdArg = "rootRsHostId";

static void printHtmlHeadExtra(wcontext_t *wctx, const char *title) {
    // Permitir acceso cross-site
    response_set_field(wctx->response, "Access-Control-Allow-Origin", "*");

    if (!wctx->isAjaxRequest) {

        printCssLinkTag(wctx, "recordings.css", "screen");
        printJavascriptTag(wctx, "jquery.form.js");
        printJavascriptTag(wctx, "wi.formHandler.js");
        printJavascriptTag(wctx, "wi.recordings.js");
        openJavascriptTag(wctx);
        WCTX_IPRINTT("$(function(){\n", 0, 1);
        WCTX_IPRINT("webif.recordingsPageInit();\n");
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.ajaxCommonData,{\n", 0, 1);
        WCTX_IPRINTF("%s:'%d',\n", RootResourcePathLengthArg, (wctx->rsPath) ? strlen(wctx->rsPath) : 0);
        WCTX_IPRINTF("%s:'%d',\n", RootResourceTypeArg, wctx->rsType);
        WCTX_IPRINTF("%s:'%d',\n", RootResourceHostIdArg, wctx->rsHostId);
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.messages,{\n", 0, 1);
        WCTX_IPRINTF("recDeleteConfirm:'%s',\n", tr("rec.delete.confirm"));
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.pages,{\n", 0, 1);
        WCTX_IPRINTF("recordings:%d,\n", PN_RESOURCES);
        WCTX_IPRINTT("});\n", -1, 0);

        WCTX_IPRINTT("$.extend(webif.actions,{\n", 0, 1);
        WCTX_IPRINTF("getRecInfo:%d,\n", PA_GET_RSR_CONTENT);
        WCTX_IPRINTT("});\n", -1, 0);

        closeJavascriptTag(wctx);
    }
}

void serveResourcePage(wcontext_t * wctx) {
    vars_t *rqargs = request_get_args(wctx->request);
    resource_t *resource = NULL;
    if (wctx->action == PA_SERVE_PLAYLIST) {
        playlistType_t plType = (vars_countn(rqargs, "plType") > 0) ? vars_get_value_i(rqargs, "plType") : wctx->conf.playlistType;
        serveResourcePlaylist(wctx, plType, wctx->rsHostId, wctx->rsPath, wctx->rsType);
        return;
    } else if (wctx->action == PA_SERVE_RSR) {
        wctx->action = PA_NO_ACTION;
        if (!wctx->rsPath) {
            response_set_status(wctx->response, HTTP_STATUS_BAD_REQUEST);
        } else {
            warn_if(wctx->rsHostId < 0);
            resource = getResource(wctx->rsHostId, wctx->rsPath, wctx->rsType, -1, false, false);
            if (resource && resource->serve) {
                char *pathFE = (char *)wctx->rsPath;
                if (webifConf.filesystemEncoding != ISO885915) {
                    pathFE = stringIconv(webifConf.filesystemEncoding, ISO885915, wctx->rsPath, strlen(wctx->rsPath));
                }
                resource->serve(resource, wctx, pathFE);
                if (pathFE != wctx->rsPath) {
                    free(pathFE);
                }
            } else {
                response_set_status(wctx->response, HTTP_STATUS_NOT_FOUND);
            }
        }
        goto freeResourceLabel;
    } else if (wctx->action == PA_GET_RSR_CONTENT) {
        wctx->decoratePage = false;
    }

    initHtmlPage(wctx, tr("recordings"), printHtmlHeadExtra, NULL);

    bool continueBaseAction = false;
    if (wctx->action == PA_EDIT_FORM) {
        resource = getResource(wctx->rsHostId, wctx->rsPath, wctx->rsType, 0, false, false);
        if (resource) {
            if (resource->confirmEditForm && resource->printEditForm) {
                if (resource->getContent) {
                    resource->getContent(resource, wctx->rsPath);
                }
                WCTX_IPRINTT("<form class=\"recEdit js-dialogContent\" action=\"" URLWI "\" method=\"post\">\n", 0, 1);
                WCTX_IPRINTT("<div class=\"hidden\">\n", 0, 1);
                WCTX_IPRINTF("<input type=\"hidden\" name=\"" ARG_PAGE "\" value=\"%d\" />\n", PN_RESOURCES);
                WCTX_IPRINTF("<input type=\"hidden\" name=\"path\" value=\"%s\"/>\n", wctx->rsPath);
                WCTX_IPRINTF("<input type=\"hidden\" name=\"type\" value=\"%d\"/>\n", resource->type);
                WCTX_IPRINTF("<input type=\"hidden\" name=\"hostId\" value=\"%d\" />\n", resource->hostId);
                WCTX_IPRINTT("</div>\n", -1, 0);
                resource->printEditForm(resource, wctx, wctx->rsPath);
                WCTX_IPRINTT("</form>\n", -1, 0);
            }
        }
        goto finishPageLabel;
    } else if (wctx->action == PA_EDIT_CONFIRM) {
        resource = getResource(wctx->rsHostId, wctx->rsPath, wctx->rsType, 0, true, false);
        char *newPath = NULL;
        if (resource && resource->confirmEditForm && resource->confirmEditForm(resource, wctx, wctx->rsPath, &newPath)) {
            /*
             * TODO 
             * 1-Si el recurso ha desaparecido de la carpeta actual o de cualquier
             * subcarpeta, basta con eliminar el item. Si la carpeta ha quedado 
             * vacía, habría que redirigir a la carpeta nueva.
             * 2-Si el recurso sólo se ha renombrado y sigue en la misma carpeta, 
             * basta generar el nuevo item y sustituir al actual.
             * 3-En otro caso, lo mas secillo es volver a generar la carpeta.
             */
            wctx->action = (vars_countn(rqargs, "baseAction") > 0) ? vars_get_value_i(rqargs, "baseAction") : PA_NO_ACTION;
            if (wctx->action == PA_LIST) {
                if (wctx->isAjaxRequest && vars_countn(rqargs, "itemDivId") == 1 && newPath != NULL) {
                    free(wctx->rsPath);
                    wctx->rsPath = newPath;
                    freeResourceChain(resource);
                    resource = getResource(wctx->rsHostId, wctx->rsPath, wctx->rsType, 0, true, false);
                    u_string_t *spath;
                    u_string_create(NULL, 0, &spath);
                    int level = 2;
                    int depth = -1;
                    crit_goto_if(asprintf(&(resource->domId), "replace_%s", vars_get_value(rqargs, "itemDivId")) == -1, outOfMemory);
                    resource->printHtmlList(resource, wctx, getResourcePath(wctx, resource, spath, false), level, depth);
                    u_string_free(spath);
                    goto freeResourceLabel;
                }
            }
            free(newPath);
            continueBaseAction = true;
            goto nextActionLabel;
        }
        goto finishPageLabel;
    } else if (wctx->action == PA_DELETE) {
        if (webifConf.deletionDisabled) {
            WCTX_MESSAGE(MT_ALERT, tr("rec.delete.err"), tr("deletionDisabled"));
        } else {
            resource = getResource(wctx->rsHostId, wctx->rsPath, wctx->rsType, 0, true, false);
            if (resource && resource->deleteResource && resource->deleteResource(resource, wctx, wctx->rsPath)) {
                if (wctx->isAjaxRequest && vars_countn(rqargs, "itemDivId") == 1) {
                    const char *buf = wctxBufferPrintf(wctx
                            , "<script type=\"text/javascript\">webif.deleteItemDiv($('#%s'));</script>\n"
                            , vars_get_value(rqargs, "itemDivId"));
                    int len = strlen(buf);
                    response_set_content_length(wctx->response, len);
                    io_write(wctx->out, buf, len);
                    goto freeResourceLabel;
                }
                wctx->action = (vars_countn(rqargs, "baseAction") > 0) ? vars_get_value_i(rqargs, "baseAction") : PA_NO_ACTION;
                continueBaseAction = true;
                goto nextActionLabel;
            }
        }
        goto finishPageLabel;
    }

nextActionLabel:
    if (continueBaseAction) {
        freeResourceChain(resource);
        resource = NULL;
        if (wctx->action != PA_NO_ACTION && vars_countn(rqargs, RootResourcePathLengthArg) > 0) {
            int rpl = vars_get_value_i(rqargs, RootResourcePathLengthArg); //resource path length, injected via ajax
            if (wctx->rsPath && rpl >= 0 && rpl < strlen(wctx->rsPath)) {

                warn_ifb(rpl > 0 && wctx->rsPath[rpl] != '/') {
                    goto finishPageLabel;
                }
                //restore base resource path, type and host
                wctx->rsPath[rpl] = 0;
                wctx->rsType = (vars_countn(rqargs, RootResourceTypeArg) == 1)
                        ? vars_get_value_i(rqargs, RootResourceTypeArg)
                        : RT_UNKNOWN;
                wctx->rsHostId = (vars_countn(rqargs, RootResourceHostIdArg) == 1)
                        ? vars_get_value_i(rqargs, RootResourceHostIdArg)
                        : HI_ANY_HOST;
            }
        }
    }
    if (wctx->action == PA_GET_RSR_CONTENT) {
        int depth = 1;
        resource = getResource(wctx->rsHostId, wctx->rsPath, wctx->rsType, depth, false, false);
        if (resource) {
            warn_if(!resource->isGetContentDone);
            int level = (vars_countn(rqargs, "level") > 0) ? vars_get_value_i(rqargs, "level") : 2;
            resource->printContent(resource, wctx, wctx->rsPath, level, depth);
        }
    } else if (wctx->action == PA_LIST) {
        printHtmlList(wctx);
    } else if (wctx->action == PA_DEFAULT) { //browse
        if (!wctx->isAjaxRequest) {
            WCTX_IPRINTT("<div class=\"level3Div\">\n", 0, 1);
        }
        int depth = wctx->conf.maxDepth;
        if (depth == 0) depth--; //infinite depth
        if (!resource) {
            resource = getResource(wctx->rsHostId, wctx->rsPath, wctx->rsType, depth, false, false);
        }
        if (resource) {
            if (!wctx->isAjaxRequest) {
                WCTX_IPRINTT("<div class=\"level3Head\">\n", 0, 1);
            }
            printPathUL(wctx, resource, AJAX_REPLACE_PREFIX("path"));
            if (!wctx->isAjaxRequest) {
                WCTX_IPRINTT("</div>" HTMLCOMENT("level3Head") "\n", -1, 0);
                WCTX_IPRINTT("<div class=\"level3\">\n", 0, 1);
            }
            resource->printContent(resource, wctx, wctx->rsPath, 0, depth);
        }
        if (!wctx->isAjaxRequest) {
            WCTX_IPRINTT("</div>" HTMLCOMENT("level3") "\n", -1, 0);
            WCTX_IPRINTT("</div>" HTMLCOMENT("level3Div") "\n", -1, 0);
        }
    }
finishPageLabel:
    finishHtmlPage(wctx);
freeResourceLabel:
    if (resource) {
        freeResourceChain(resource);
        resource = NULL;
    }
    return;
outOfMemory:
    exit(EXIT_FAILURE);
}
