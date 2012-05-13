/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <dirent.h>
#include <errno.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "resource.h"
#include "streaming.h"
#include "svdrp_comm.h"

bool seekFragment(fragmentList_t * const list, uint64_t pos) {
    fragment_t * f;
    int i;
    for (i = 0, f = list->entry; i < list->length; i++, f++) {
        if (pos >= f->start && pos <= f->end) {
            list->fragPos = pos - f->start;
            list->fragNum = i;
            return true;
        }
    }
    return false;
}

void streamFragments(wcontext_t *wctx, fragmentList_t *fragments) {
    session_t *session = wctx->session;
    request_t *request = wctx->request;
    response_t *response = wctx->response;
    io_t *out = wctx->out;

    uint64_t range_start = 0;
    uint64_t range_end = 0;
    bool use_range = false;

    const char *icyMetadata = request_get_field_value(request, "Icy-Metadata");
    if (icyMetadata && sameString(icyMetadata, "1")) {
        dbg("Icy-Metadata request");
        //response_set_field(response, "Icy-Metaint", "0");
        //response_print_header(response);
    }
    const char * sessionId = session_get_id(session);
    dbg("Sesion id: %s", sessionId);

    if (fragments->length > 0 && fragments->totalSize > 0 && fragments->mimeType) {
        field_t * range = request_get_field(request, "Range");
        if (range) {
            dbg("Rango solicitado \"%s\"", range->value);
            if (strncmp(range->value, "bytes=", 6) == 0) {
                char *r = range->value + 6;
                use_range = true;
                errno = 0;
                range_start = (uint64_t) strtoll(r, &r, 10);
                if (errno || range_start > fragments->totalSize) {
                    warn("Invalid range start %s, range_start=%lld, total_size=%lld, errno=%d", r, range_start, fragments->totalSize, errno);
                    use_range = false;
                } else {
                    r += strspn(r, "- ");
                    if (strlen(r) > 0) {
                        errno = 0;
                        range_end = (uint64_t) strtoll((const char *) r, &r, 10);
                        if (errno) {
                            warn("Invalid range end %s, range_end=%lld, total_size=%lld, errno=%d", r, range_end, fragments->totalSize, errno);
                            range_end = 0;
                        }
                    }
                }
            } else {
                warn("Invalid range %s", range->value);
                use_range = 0;
            }
        }
        if (!seekFragment(fragments, range_start)) {
            warn("No fragment found for range_start=%lld", range_start);
            response_set_status(wctx->response, HTTP_STATUS_NO_CONTENT);
        } else {
            dbg("length[%d],fragNum[%d],fragPos[%lld],fragSize[%lld],totalSize[%lld]", fragments->length, fragments->fragNum, fragments->fragPos, fragments->entry[fragments->fragNum].size, fragments->totalSize);
            response_set_field(response, "Accept-Ranges", "bytes");
            if (use_range) {
                if (!range_end) {
                    range_end = fragments->totalSize - 1;
                }
                response_set_status(response, HTTP_STATUS_PARTIAL_CONTENT); //=206 requires klone-core patched
                char *contentRange;
                if (asprintf(&contentRange, "bytes %lld-%lld/%lld", range_start, range_end, fragments->totalSize) != -1) {
                    response_set_field(response, "Content-Range", contentRange);
                    free(contentRange);
                }
            } else {
                response_set_status(response, HTTP_STATUS_OK);
                range_end = fragments->totalSize - 1;
            }
            uint64_t remaining = (range_end - range_start + 1);
            response_set_content_length(response, remaining);
            response_set_content_type(response, fragments->mimeType);

            io_codecs_remove(response_io(response)); //avoids extra buffering
            response_print_header(response);
            io_flush(out); //TODO ¿necesario?

            // Start of streaming

            bool cancel = false;
            int len, br, bw;
            char buffer[4096];
            while (fragments->fragNum < fragments->length && remaining > 0 && !cancel) {
                fragment_t *f = fragments->entry + fragments->fragNum;
                if (f->path != NULL && f->size > 0) {
                    dbg("fragNum[%d],fragPos[%lld],fragSize[%lld],totalSize[%lld]", fragments->fragNum, fragments->fragPos, f->size, fragments->totalSize);
                    FILE *fi = fopen(f->path, "rb");
                    if (fi) {
                        errno = 0;
                        if (fseek(fi, fragments->fragPos, SEEK_SET) != 0) {
                            warn("Wrong position %lld in fragment[&d].Error:%d", fragments->fragPos, fragments->fragNum, errno);
                        }
                        while (remaining > 0 && !cancel && !feof(fi) && !ferror(fi)) {
                            len = sizeof (buffer);
                            if (remaining < len) len = (int) remaining;
                            br = fread(buffer, 1, len, fi);
                            while (br > 0 && !cancel) {
                                bw = io_write(out, buffer, br);
                                if (bw < 0) {
                                    cancel = true;
                                } else {
                                    br -= bw;
                                    remaining -= bw;
                                }
                            }
                        }
                        if (errno) {
                            if (errno != ECONNRESET) warn("%d %s", errno, strerror(errno));
                            errno = 0;
                        }
                        fclose(fi);
                    }
                }
                fragments->fragNum++;
                fragments->fragPos = 0;
            }
        }
    } else {
        response_set_status(wctx->response, HTTP_STATUS_NO_CONTENT);
    }
}

