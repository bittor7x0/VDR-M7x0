/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 * Video or audio files.
 */

#include <klone/utils.h>
#include "resource.h"
#include "rspriv.h"

typedef struct mediaFile_s {
    resource_t r;
} mediaFile_t;

void addMediaFileFragments(void *this, fragmentList_t * const fragments, const char * const rsPath) {
    resource_t *resource = this;
    char * rsFullPath = getAbsolutePath(this, rsPath);
    if (rsFullPath) {
        fragments->entry = realloc(fragments->entry, (fragments->length + 1) * sizeof (fragment_t));
        crit_goto_if(fragments->entry == NULL, outOfMemory);
        fragment_t *f = fragments->entry + fragments->length;
        addFileFragment(f, fragments, rsFullPath, NULL, resource->mimeType);
        free(rsFullPath);
    }
    return;
outOfMemory:
    exit(EXIT_FAILURE);
}

static void printMediaFileControls(void *this, wcontext_t *wctx, const char *const path) {
    resource_t *resource = this;
    printResourceControls(resource, wctx, path, NULL, tr("play"), tr("rec.edit"), tr("rec.delete"));
}


static const char * mediaFileCssPfx = "mf";

mediaFile_t *initMediaFile(mediaFile_t *mediaFile, const int hostId, resource_t *parent, const char * name, const char *mimeType, time_t *pcreationDate) {
    resource_t *resource = &mediaFile->r;
    initResource(resource, hostId, RT_MEDIA_FILE, parent, name, mimeType, pcreationDate);
    resource->cssPfx = mediaFileCssPfx;
    resource->printHtmlHierarchy = &printMediaResourceHtmlHierarchy;
    resource->printHtmlList = &printMediaResourceHtmlList;
    resource->printControls = &printMediaFileControls;
#ifdef VDRWI_WEBDAV
    resource->printThisPropFind = &printMediaResourcePropFind;
#endif
    resource->addFragments = addMediaFileFragments;
    return mediaFile;
}

void *newMediaFile(const int hostId, resource_t *parent, const char * name, time_t *pcreationDate) {
    const char *mimeType = u_guess_mime_type(name);
    return initMediaFile(malloc(sizeof (mediaFile_t)), hostId, parent, name, mimeType, pcreationDate);
}