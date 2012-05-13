/* 
 * File:   playlist.h
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 *
 * Created on 12 de diciembre de 2011, 0:03
 */

#ifndef WI_PLAYLIST_H
#define	WI_PLAYLIST_H

#include "conf.h"

#ifdef	__cplusplus
extern "C" {
#endif

    extern const char *plMimeType[];
    extern const char *plExt[];
    extern const char *plVirtName[];

    typedef struct playlistGenerator_s playlistGenerator_t;

    struct resource_s;

    struct playlistGenerator_s {
        void (*initPlaylist)(const playlistGenerator_t * this, wcontext_t *wctx, const char *fileName);
        void (*finishPlaylist)(const playlistGenerator_t * this, wcontext_t * wctx);
        int (*printPlaylistItem)(const playlistGenerator_t * this, wcontext_t *wctx, const char * const displayName, const char *const url, int id);
        void (*printResourcePlaylist)(const playlistGenerator_t * this, wcontext_t *wctx, struct resource_s *resource, const char *const path);
    };
    void serveResourcePlaylist(wcontext_t *wctx, playlistType_t plType, int hostId, const char *const path, resourceType_t type);
    extern const playlistGenerator_t plGenerator[];

#ifdef	__cplusplus
}
#endif

#endif	/* WI_PLAYLIST_H */

