/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 * VDR recording resource
 */

#include <klone/utils.h>
#include <time.h>
#include <dirent.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <u/libu.h>
#include <u/toolbox.h>

#include "conf.h"
#include "epg.h"
#include "resource.h"
#include "rspriv.h"
#include "svdrp_comm.h"

typedef struct vdrRec_s {
    resource_t r;
    int vdrId;
    char *path;
    char *vdrName;
    bool isTS;
    bool edited;
    epgEvent_t event;
} vdrRec_t;

static const char *vdrRecCssPfx = "rec";

static const char *vdrRecMimeType = "video/mpeg";

static int isVdrPesFragment(const struct dirent * ent) {
    char *c;
    char *c0 = (char *) ent->d_name;
    for (c = c0; c < c0 + 3; c++) if (!isdigit(*c)) return 0;
    return (strcmp(c, ".vdr") == 0);
}

static int isVdrTsFragment(const struct dirent * ent) {
    char *c;
    char *c0 = (char *) ent->d_name;
    for (c = c0; c < c0 + 5; c++) if (!isdigit(*c)) return 0;
    return (strcmp(c, ".ts") == 0);
}

static void addVdrRecFragments(void * this, fragmentList_t * const fragments, const char * const path) {
    vdrRec_t *vdrRec = this;
    resource_t *resource = this;
    char * fullpath = getAbsolutePath(resource, path);
    if (fullpath) {
        struct stat st;
        if ((stat(fullpath, &st) == 0) && S_ISDIR(st.st_mode)) {
            fragment_t *f;
            int i;
            struct dirent **namelist;
            int n = scandir(fullpath, &namelist, (vdrRec->isTS) ? isVdrTsFragment : isVdrPesFragment, alphasort);
            if (n > 0) {
                fragments->entry = realloc(fragments->entry, (fragments->length + n) * sizeof (fragment_t));
                crit_goto_if(fragments->entry == NULL, outOfMemory);
                for (i = 0, f = fragments->entry + fragments->length; i < n; i++) {
                    if (addFileFragment(f, fragments, fullpath, namelist[i]->d_name, resource->mimeType)) {
                        f++;
                    }
                }
                for (i = 0; i < n; i++) free(namelist[i]);
                free(namelist);
            }
        }
        free(fullpath);
    }
    return;
outOfMemory:
    exit(EXIT_FAILURE);
}

static char *getVdrRecDisplayName(void *this, bool canDelegate) {
    resource_t *resource = this;
    char *inheritedDisplayName = NULL;
    if (canDelegate && resource->parent && resource->parent->children.length == 1) {
        inheritedDisplayName = resource->parent->getDisplayName(resource->parent, false);
        if (inheritedDisplayName && inheritedDisplayName[0] != ' ' && inheritedDisplayName[0] != 0) {
            return inheritedDisplayName;
        }
    }
    return getResourceDisplayName(this, false);
}

static void prepareVdrRec(void * this, int level) {
    resource_t *resource = this;
    resource->level = level;
    vdrRec_t *vdrRec = this;
    if (resource->parent) {
        if (resource->parent->name[0] == '%') {
            vdrRec->edited = true;
        }
        resource->parent->numVdrRecs++;
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

static bool renameVdrRec(void *this, wcontext_t *wctx, const char * const path) {
    resource_t *resource = this;
    vdrRec_t *vdrRec = this;
    warn_return_if(!vdrRec->vdrName, false);
    bool result = false;
    if (!path) {
        WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), tr("recErrorNoPath"));
        return false;
    }
    hostConf_t *host = getHost(resource->hostId);
    if (!host) {
        WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), tr("recErrorWrongHost"));
        return false;
    }
    char *data = execSvdrp(host, "RENR %s %s", path, vdrRec->vdrName);
    if (data != NULL) {
        char * p = data;
        int code = strtol(p, &p, 10);
        result = boolean(code == SVDRP_CMD_OK);
        if (wctx && *p && *(++p)) {
            message_t *msg = (result)
                    ? WCTX_MESSAGE(MT_INFO, tr("rec.update.ok"), "%s", p)
                    : WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), "%s", p);
            if (msg) msg->requieresHtmlEncoding = true;
        }
        free(data);
    } else {
        WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), tr("warnSvdrpConnection"));
    }
    return result;
}

static bool deleteVdrRec(void *this, wcontext_t *wctx, const char * const path) {
    resource_t *resource = this;
    vdrRec_t *vdrRec = this;
    if (!path) {
        WCTX_MESSAGE(MT_ALERT, tr("rec.delete.err"), tr("recErrorNoPath"));
        return false;
    }
    hostConf_t *host = getHost(resource->hostId);
    if (!host) {
        WCTX_MESSAGE(MT_ALERT, tr("rec.delete.err"), tr("recErrorWrongHost"));
        return false;
    }
    bool result = false;
    char *data = execSvdrp(host, "DELR %s", path);
    if (data != NULL) {
        char * p = data;
        int code = strtol(p, &p, 10);
        result = boolean(code == SVDRP_CMD_OK);
        if (wctx && *p && *(++p)) {
            message_t *msg = (result)
                    ? WCTX_MESSAGE(MT_INFO, tr("rec.delete.ok"), "%s", p)
                    : WCTX_MESSAGE(MT_ALERT, tr("rec.delete.err"), "%s", p);
            if (msg) msg->requieresHtmlEncoding = true;
        }
        free(data);
    } else {
        WCTX_MESSAGE(MT_ALERT, tr("rec.delete.err"), tr("warnSvdrpConnection"));
    }
    return result;
}

/**
 * This is used both to read information both from the info.vdr file, and from the 
 * response to a "LSTR recId" SVDRP command.
 * @param vdrRec VDR recording which info we want
 * @param data A line of text from the file or from the SVDRP response
 * @param fromFile Indicates data procendence.
 */
static void parseVdrRecInfo(vdrRec_t * const vdrRec, const char *const path, char * const data, bool fromFile) {
    char *s;
    for (s = strtok(data, "\r\n"); s != NULL; s = strtok(0, "\r\n")) {
        if (!fromFile) { //skip response code
            int code = strtol(s, &s, 10);
            if (code == SVDRP_RCRD) {
                if (s[0] == ' ') {
                    break; //End of recording info
                }
                s++;
            } else {
                continue;
            }
        }
        switch (s[0]) {
            case 'P':
                //We already know the path 
                break;
            default:
                parseEventLine(s[0], s + 2, &vdrRec->event);
                break;
        }
    }
    if (vdrRec->event.start == 0 && path && path[0]) {
        //obtener fecha a partir de ruta
        char *r = strrchr(path, '/');
        if (r) {
            r++;
            struct tm sstart = *localtime(&vdrRec->event.start);
            if (strptime(r, "%Y-%m-%d.%H.%M", &sstart) != NULL) {
                sstart.tm_sec = 0;
                sstart.tm_isdst = -1;
                vdrRec->event.start = mktime(&sstart);
            }
        }
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

static void parseVdrRecLine(vdrRec_t * const vdrRec, char * line, bool withPath, bool storePath) {
    char * r = line;
    int l;
    struct tm timeptr;

    vdrRec->vdrId = strtol(r, &r, 10);
    warn_return_if(r == NULL,);
    while (r[0] == ' ') r++;
    if (withPath) {
        l = strcspn(r, " ");
        if (storePath) {
            int l2 = l;
            while (r[l2 - 1] == '/') l2--;
            warn_if(vdrRec->path != NULL);
            free(vdrRec->path);
            vdrRec->path = (l2 > 0) ? strndup(r, l2) : NULL;
        }
        r += l;
        while (r[0] == ' ') r++;
    }
    r = strptime(r, "%d.%m.%y %H:%M", &timeptr);
    warn_if(r == NULL);
    if (r) {
        timeptr.tm_sec = 0;
        timeptr.tm_isdst = -1;
        vdrRec->event.start = mktime(&timeptr);
        r++;
        while (r[0] == ' ') r++;
        l = strcspn(r, "/\n\r");
        vdrRec->vdrName = strndup(r, l);
    }
    return;
}

static char *setVdrRecName(vdrRec_t *vdrRec, const char *const path) {
    if (!vdrRec->vdrName) {
        warn_return_if(!path, NULL);
        char *ls = strrchr(path, '/');
        warn_return_if(!ls, NULL);
        crit_goto_if((vdrRec->vdrName = stringIconv(ISO885915, UTF8, path, ls - path)) == NULL, outOfMemory);
    }
    return vdrRec->vdrName;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

static bool readVdrRecInfo(vdrRec_t * const vdrRec, const char *const path) {
    resource_t *resource = &vdrRec->r;
    dbg("readVdrRecInfo hostId: %d path: %s", resource->hostId, path);
    warn_return_if(resource->hostId < 0 || resource->hostId >= webifConf.hosts.length || !path, false);
    bool ok = true;
    u_buf_t *ubuf;
    crit_goto_if(u_buf_create(&ubuf), outOfMemory);
    hostConf_t *host = getHost(resource->hostId);
    char *fileName = NULL;
    crit_goto_if(asprintf(&fileName, "%s/%s/info%s", host->video0, path, (vdrRec->isTS) ? "" : ".vdr") < 0, outOfMemory);

    warn_ifb(u_buf_load(ubuf, fileName)) {
        warn("%s no se ha leído", fileName);
        ok = false;
    }
    free(fileName);
    if (ok && u_buf_size(ubuf) > 0) {
        parseVdrRecInfo(vdrRec, path, u_buf_ptr(ubuf), true);
    }
    u_buf_free(ubuf);
    return resource->isGetContentDone = ok;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

static bool getRemoteVdrRecInfo(vdrRec_t *vdrRec, const char *const path, hostConf_t *host) {
    char *data = execSvdrp(host, "LSTR %s", path);
    if (data != NULL) {
        parseVdrRecInfo(vdrRec, path, data, false);
        free(data);
        return vdrRec->r.isGetContentDone = true;
    }
    return false;
}

static bool getVdrRecContent(void * const this, const char *const path) {
    resource_t *resource = this;
    dbg_return_if(resource->isGetContentDone, true);
    vdrRec_t *vdrRec = this;
    warn_return_if(!path, false);
    hostConf_t *host = getHost(resource->hostId);
    if (host && host->video0[0]) { //recordins are locally accessible
        return readVdrRecInfo(vdrRec, path);
    } else if (host->isVdr) {
        return getRemoteVdrRecInfo(vdrRec, path, host);
    }
    warn("No valid host defined for rec %s", path);
    return false;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

static void printVdrRecContent(void *this, wcontext_t *wctx, const char *const path, int level, int depth) {
    vdrRec_t *vdrRec = this;
    WCTX_IPRINTT("<div class=\"recDate\">\n", 0, 1);
    WCTX_IPRINTF(
            "<span class=\"recStart field\">"
            "<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%s</span>"
            "</span>\n", tr("date"), formatDate(localtime(&vdrRec->event.start), 1));
    if (vdrRec->event.duration > 0) {
        WCTX_IPRINTF(
                "<span class=\"recDuration field\">"
                "<span class=\"label\">%s:</span>&nbsp;<span class=\"value\">%d&#39;</span>"
                "</span>\n", tr("runtime"), vdrRec->event.duration / 60);
    }
    WCTX_IPRINTT("</div>" HTMLCOMENT("recDate") "\n", -1, 0);
    if (vdrRec->event.title && strlen(vdrRec->event.title) > 0) {
        WCTX_IPRINTF("<div class=\"title\">%s</div>\n", wctxHtmlEncode(wctx, vdrRec->event.title));
    }
    if (vdrRec->event.shortdesc && vdrRec->event.shortdesc[0]) {
        WCTX_IPRINTF("<div class=\"shortdesc\">%s</div>\n", wctxHtmlEncode(wctx, vdrRec->event.shortdesc));
    }
    if (vdrRec->event.desc && vdrRec->event.desc[0]) {
        WCTX_IPRINTT("<div class=\"desc\">\n", 0, 1);
        printEventDesc(wctx, vdrRec->event.desc);
        WCTX_IPRINTT("</div>" HTMLCOMENT("desc") "\n", -1, 0);
    }
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

static bool printVdrRecHtmlInsteadOfParent(void *this, wcontext_t *wctx, bool hierarchical, const char *const parentPath, int level) {
    resource_t *resource = this;
    char * path = extendPath(parentPath, resource->name);
    if (hierarchical) {
        resource->printHtmlHierarchy(resource, wctx, path, level, -1);
    } else {
        resource->printHtmlList(resource, wctx, path, level, -1);
    }
    free(path);
    return true;
}

static void printVdrRecEditForm(void *this, wcontext_t *wctx, const char *const path) {
    resource_t *resource = this;
    vdrRec_t *vdrRec = this;
    int len;
    WCTX_IPRINTT("<table class=\"form1\">\n", 0, 1);
    WCTX_IPRINTF("<caption class=\"js-dialogTitle\">%s</caption>\n", tr("rec.edit"));
    WCTX_IPRINTT("<tfoot class=\"form1Foot\">\n", 0, 1);
    {
        WCTX_IPRINTT("<tr class=\"buttons\">\n", 0, 1);
        WCTX_IPRINTT("<td class=\"buttons js-dialogButtons\" colspan=\"2\">\n", 0, 1);
        { //accept button
            WCTX_IPRINTF(
                    "<button type=\"submit\" name=\"" ARG_ACTION "\" value=\"%d\" class=\"confirm ui-state-default ui-button ui-button-text-icon-primary\" >"
                    "<span class=\"ui-button-icon-primary ui-icon ui-icon-check\"></span>"
                    "<span class=\"ui-button-text\">%s</span>"
                    "</button>\n"
                    , PA_EDIT_CONFIRM, tr("accept"));
        }
        { //play button
            const char *Play = tr("play");
            int level = 0;
            WCTX_IPRINTF(
                    "<a href=\"%s\" class=\"play ui-state-default ui-button ui-button-text-icon-primary\">"
                    "<span class=\"ui-button-icon-primary ui-icon ui-icon-play\"></span>"
                    "<span class=\"ui-button-text\">%s</span>"
                    "</a>\n"
                    , getResourceURL(resource, wctx, path, PA_SERVE_PLAYLIST, true, true), Play);
        }
        if (!webifConf.deletionDisabled && (path)) { //delete button
            WCTX_IPRINTF(
                    "<button type=\"submit\" name=\"" ARG_ACTION "\" value=\"%d\" class=\"delete ui-state-default ui-button ui-button-text-icon-primary\" >"
                    "<span class=\"ui-button-icon-primary ui-icon ui-icon-trash\"></span>"
                    "<span class=\"ui-button-text\">%s</span>"
                    "</button>\n", PA_DELETE, tr("rec.delete"));
        }
        WCTX_IPRINTT("</td>\n", -1, 0);
        WCTX_IPRINTT("</tr>\n", -1, 0);
    }
    WCTX_IPRINTT("</tfoot>\n", -1, 0);

    WCTX_IPRINTT("<tbody class=\"form1Body\">\n", 0, 1);
    warn_if(!setVdrRecName(vdrRec, path));
    if (vdrRec->vdrName) { //vdrName
        WCTX_IPRINTT("<tr>\n", 0, 1);
        WCTX_IPRINTF("<th>%s</th>\n", tr("name"));
        WCTX_IPRINTT("<td>\n", 0, 1);
        vdrDecode(vdrRec->vdrName);
        len = strlen(vdrRec->vdrName);
        const char *edn = wctxHtmlEncodeN(wctx, vdrRec->vdrName, len);
        WCTX_IPRINTF("<input type=\"text\" name=\"name\" value=\"%s\" size=\"%d\"/>\n", edn, 5 + (len > 55) ? 55 : len);
        WCTX_IPRINTF("<input type=\"hidden\" name=\"oldName\" value=\"%s\"/>\n", edn);
        WCTX_IPRINTT("</td>\n", -1, 0);
        WCTX_IPRINTT("</tr>\n", -1, 0);
    }
    { //TODO printEventForm
        if (vdrRec->event.title != NULL) {
            WCTX_IPRINTT("<tr>\n", 0, 1);
            WCTX_IPRINTF("<th>%s</th>\n", tr("title"));
            WCTX_IPRINTT("<td>\n", 0, 1);
            //wctx_printf0("<textarea name=\"title\" cols=\"%d\" rows=\"%d\">",(l>70)?70:l,(l/70)+2);
            WCTX_PRINT(wctxHtmlEncode(wctx, vdrRec->event.title));
            //wctx_printf("</textarea>\n");
            WCTX_IPRINTT("</td>\n", -1, 0);
            WCTX_IPRINTT("</tr>\n", -1, 0);
        }
        if (vdrRec->event.shortdesc != NULL) {
            WCTX_IPRINTT("<tr>\n", 0, 1);
            WCTX_IPRINTF("<th>%s</th>\n", tr("shortdesc"));
            WCTX_IPRINTT("<td>\n", 0, 1);
            //wctx_printf0("<textarea name=\"shortdesc\" cols=\"%d\" rows=\"%d\">",(l>70)?70:l,(l/70)+2);
            WCTX_PRINT(wctxHtmlEncode(wctx, vdrRec->event.shortdesc));
            //wctx_printf("</textarea>\n");
            WCTX_IPRINTT("</td>\n", -1, 0);
            WCTX_IPRINTT("</tr>\n", -1, 0);
        }
        if (vdrRec->event.desc != NULL) {
            WCTX_IPRINTT("<tr>\n", 0, 1);
            WCTX_IPRINTF("<th>%s</th>\n", tr("desc"));
            WCTX_IPRINTT("<td>\n", 0, 1);
            //l=strlen(rec->event.desc);
            //wctx_printfn("<textarea name=\"desc\" cols=\"%d\" rows=\"%d\">\n",0,1,(l>70)?70:l,(l/70)+2);
            printEventDesc(wctx, vdrRec->event.desc);
            //wctx_printfn("</textarea>\n",-1,0);
            WCTX_IPRINTT("</td>\n", -1, 0);
            WCTX_IPRINTT("</tr>\n", -1, 0);
        }
    }
    WCTX_IPRINTT("</tbody>\n", -1, 0);

    WCTX_IPRINTT("</table>\n", -1, 0);
    return;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

static void printVdrRecControls(void *this, wcontext_t *wctx, const char *const path) {
    resource_t *resource = this;
    vdrRec_t *vdrRec = this;
    if (vdrRec->edited) {
        const char *Edited = tr("rec.edited");
        WCTX_PRINTT("<li class=\"control\">\n", 1);
        WCTX_IPRINTF(
                "<span class=\"cut control ui-button ui-button-icon-only\" title=\"%s\">"
                "<span class=\"ui-button-icon-primary ui-icon ui-icon-scissors\"></span>"
                "<span class=\"ui-button-text\">%s</span>"
                "</span>\n", Edited, Edited);
        WCTX_IPRINTT("</li>", -1, 0);
    }
    printResourceControls(resource, wctx, path, NULL, tr("play"), tr("rec.edit"), tr("rec.delete"));
}

static bool confirmVdrRecEditForm(void *this, wcontext_t *wctx, const char *oldPath, char **pNewPath) {
    resource_t *resource = this;
    vdrRec_t *vdrRec = this;
    if (!oldPath) {
        WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), tr("recErrorNoPath"));
        return false;
    }
    hostConf_t *host = getHost(resource->hostId);
    bool result = false;
    if (!host) {
        WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), tr("recErrorWrongHost"));
        return false;
    }
    vars_t *rqargs = request_get_args(wctx->request);
    bool newNameIsACopy, oldNameIsACopy;
    char *newName = getRequestFieldValue(wctx, rqargs, "name", &newNameIsACopy);
    if (!newName || !newName[0]) {
        WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), "Es necesario especificar un nombre"); //TODO i18n
        goto exit;
    }
    vdrEncode(newName);

    warn_ifb(vdrRec->vdrName) {
        free(vdrRec->vdrName);
        vdrRec->vdrName = NULL;
    }
    vdrRec->vdrName = getRequestFieldValue(wctx, rqargs, "oldName", &oldNameIsACopy);
    if (!oldNameIsACopy) {
        vdrRec->vdrName = strdup(vdrRec->vdrName);
    }
    vdrEncode(vdrRec->vdrName);
    if (sameString(vdrRec->vdrName, newName)) {
        WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), "No se ha cambiado el nombre"); //TODO i18n
        goto exit;
    }
    char *data = execSvdrp(host, "RENR %s %s", oldPath, newName);
    if (data != NULL) {
        char * p = data;
        int code = strtol(p, &p, 10);
        result = boolean(code == SVDRP_CMD_OK);
        if (result) {
            if (pNewPath) {
                //try to guess new path
                //would be better if RENR provides it
                asprintf(pNewPath, "%s%s", newName, rindex(oldPath, '/'));
                vdrDecode(*pNewPath);
            }
            free(vdrRec->vdrName);
            vdrRec->vdrName = (newNameIsACopy) ? newName : strdup(newName);
            newName = NULL;
        }
        if (wctx && *p && *(++p)) {
            message_t *msg = (result)
                    ? WCTX_MESSAGE(MT_INFO, tr("rec.update.ok"), "%s", p)
                    : WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), "%s", p);
            if (msg) msg->requieresHtmlEncoding = true;
        }
        free(data);
    } else {
        WCTX_MESSAGE(MT_ALERT, tr("rec.update.err"), tr("warnSvdrpConnection"));
    }
exit:
    if (newName && newNameIsACopy) free(newName);
    return result;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

resource_t *addRemoteVdrRecs(resource_t * const root, resource_t *resource, const char *const rsPath, resourceType_t rsType, hostConf_t * const host, int depth) {
    dbg("addRemoteResources(%s)", rsPath);
    int rootPathLength = 0;
    int rsPathLength = (rsPath) ? strlen(rsPath) : rootPathLength;
    if (rsType == RT_VDR_REC) {
        warn_return_if(!rsPathLength, NULL);
        warn_return_if(getResourceType(rsPath, false, true) != RT_VDR_REC, NULL);
        resource = addResourceChain(root, host->id, rsPath + rootPathLength, -1);
        if (resource) {
            warn_if(resource->type != RT_VDR_REC);
            if (resource->type == RT_VDR_REC) {
                getRemoteVdrRecInfo((vdrRec_t*) resource, rsPath, host);
            } else {
                resource->free(resource);
                resource = NULL;
            }
        }
        return resource;
    }
    char *data = execSvdrp(host, "LSTR path");
    if (data) {
        char *r, *sr; //row
        for (r = strtok_r(data, "\r\n", &sr); r != 0; r = strtok_r(0, "\r\n", &sr)) {
            int code = strtol(r, &r, 10);
            if (code == SVDRP_CMD_OK) {
                char *line = r + 1;
                //while (line[0] == ' ') line++;
                char *remoteRsPath = (line[0]) ? strchr(line, ' ') : NULL;
                bool resourceChainHostIdEvaluated = false;
                if (remoteRsPath) {
                    while (*remoteRsPath == ' ') remoteRsPath++;
                    char *e = strchr(remoteRsPath, ' ');
                    if (e) {
                        *e = 0;
                        if (strchr(remoteRsPath, '/') == NULL) {
                            crit("VDR should be patched to return path!!");
                        } else if (strncmp(remoteRsPath, rsPath, rsPathLength) == 0) {
                            vdrRec_t *vdrRec = NULL;
                            if (!resource) {
                                resource = (rsPath) ? addResourceChain(root, host->id, rsPath + rootPathLength, -1) : root;
                            } else if (!resourceChainHostIdEvaluated) {
                                evaluateResourceChainHostId(resource, host->id);
                                resourceChainHostIdEvaluated = true;
                            }
                            if (resource) {
                                if (resource->type == RT_VDR_REC) {
                                    vdrRec = (vdrRec_t *) resource;
                                } else if (resource->type == RT_COLLECTION) {
                                    //Investigamos mas niveles, independientemente de depth, para ver si contiene vdrRec
                                    resource->isGetContentDone = true;
                                    resource_t *last = addResourceChain(resource, host->id, remoteRsPath + rsPathLength, -1);
                                    if (last && last->type == RT_VDR_REC) {
                                        vdrRec = (vdrRec_t *) last;
                                    }
                                }
                                if (vdrRec != NULL) {
                                    *e = ' ';
                                    parseVdrRecLine(vdrRec, line, true, false);
                                }
                            }
                        }
                    }
                }
            }
            if (resource && depth == 0) {
                break;
            }
        }
        free(data);
    }
    return resource;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

static void freeVdrRec(void *this) {
    vdrRec_t *vdrRec = this;
    free(vdrRec->path);
    free(vdrRec->vdrName);
    freeEpgEvent(&vdrRec->event);
    freeResource(this);
}

static vdrRec_t *initVdrRec(vdrRec_t *vdrRec, const int hostId, resource_t *parent, const char *name) {
    resource_t *resource = &vdrRec->r;
    struct tm sdate;
    time_t tdate = 0;
    char *r;
    //TS:   YYYY-MM-DD-hh[.:]mm.pr.lt.rec (pr=priority, lt=lifetime)
    //PES:  YYYY-MM-DD-hh.mm.ch-ri.rec (ch=channel, ri=resumeId)
    warn_if((r = strptime(name, "%Y-%m-%d.%H.%M", &sdate)) == NULL);
    if (r != NULL) {
        vdrRec->isTS = boolean(strchr(r, '-') != NULL);
        tdate = mktime(&sdate);
    }
    initResource(resource, hostId, RT_VDR_REC, parent, name, vdrRecMimeType, (r != NULL) ? &tdate : NULL);
    resource->cssPfx = vdrRecCssPfx;
    resource->free = &freeVdrRec;
    resource->prepare = &prepareVdrRec;
    resource->renameResource = &renameVdrRec;
    resource->deleteResource = &deleteVdrRec;
    resource->getDisplayName = &getVdrRecDisplayName;
    resource->printHtmlHierarchy = &printMediaResourceHtmlHierarchy;
    resource->printHtmlList = &printMediaResourceHtmlList;
    resource->printHtmlInsteadOfParent = &printVdrRecHtmlInsteadOfParent;
    resource->printControls = &printVdrRecControls;
    resource->printEditForm = &printVdrRecEditForm;
    resource->confirmEditForm = &confirmVdrRecEditForm;
    resource->getContent = &getVdrRecContent;
    resource->printContent = &printVdrRecContent;
#ifdef VDRWI_WEBDAV
    resource->printThisPropFind = &printMediaResourcePropFind;
#endif
    resource->addFragments = &addVdrRecFragments;
    initEpgEvent(&vdrRec->event);
    vdrRec->vdrId = 0;
    vdrRec->path = NULL;
    vdrRec->vdrName = NULL;
    vdrRec->edited = false;
    return vdrRec;
}

void *newVdrRec(const int hostId, resource_t *parent, const char *name) {
    return initVdrRec(malloc(sizeof (vdrRec_t)), hostId, parent, name);
}

