/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <stddef.h>
#include <stdlib.h>
#include "fragment.h"

void initFragment(fragment_t * const entry) {
    entry->path = NULL;
    entry->size = 0;
    entry->start = 0;
    entry->end = 0;
}

void freeFragment(fragment_t * const entry) {
    free(entry->path);
    initFragment(entry);
}

void initFragmentList(fragmentList_t * const list) {
    list->resourceType = RT_UNKNOWN;
    list->mimeType = NULL;
    list->length = 0;
    list->fragNum = 0;
    list->fragPos = 0;
    list->entry = NULL;
    list->totalSize = 0;
}

void freeFragmentList(fragmentList_t * const list) {
    int i;
    for (i = 0; i < list->length; i++) {
        freeFragment(&list->entry[i]);
    }
    free(list->entry);
    initFragmentList(list);
}

