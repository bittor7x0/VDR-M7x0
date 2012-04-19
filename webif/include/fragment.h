/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_FRAGMENT_H
#define WI_FRAGMENT_H

#include <stdint.h>

typedef enum resourceType_e {
    RT_UNKNOWN,
    RT_COLLECTION,
    RT_VDR_REC,
    RT_VDR_DELETED,
    RT_MEDIA_FILE,
    RT_VIRTUAL
} resourceType_t;

typedef struct fragment_s {
    char * path;
    uint64_t start;
    uint64_t end;
    uint64_t size;
} fragment_t;

typedef struct fragmentList_s {
    resourceType_t resourceType;
    const char *mimeType;
    int length;
    int fragNum;
    uint64_t fragPos;
    uint64_t totalSize;
    fragment_t *entry;
} fragmentList_t;

void initFragment(fragment_t * const entry);
void freeFragment(fragment_t * const entry);
void initFragmentList(fragmentList_t * const list);
void freeFragmentList(fragmentList_t * const list);

#endif
