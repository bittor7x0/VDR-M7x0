/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 * Implementation of PROPFIND command from WebDAV.
 * HTTP request example:
 * -----------------
PROPFIND /recordings HTTP/1.1
Host: localhost
Connection: Keep-Alive
User-Agent: Mozilla/5.0 (compatible; Konqueror/4.6; Linux) KHTML/4.6.5 (like Gecko) SUSE
Pragma: no-cache
Cache-control: no-cache
Accept: text/html, image/jpeg;q=0.9, image/png;q=0.9,...
Accept-Encoding: x-gzip, x-deflate, gzip, deflate
Accept-Charset: utf-8, utf-8;q=0.5, *;q=0.5
Accept-Language: es,en-US;q=0.9,en;q=0.8
Depth: 0
Content-Type: text/xml; charset=utf-8
Content-Length: 303

<?xml version="1.0" encoding="utf-8" ?>
<D:propfind xmlns:D="DAV:">
	<D:prop>
		<D:creationdate/>
		<D:getcontentlength/>
		<D:displayname/>
		<D:source/>
		<D:getcontentlanguage/>
		<D:getcontenttype/>
		<D:executable/>
		<D:getlastmodified/>
		<D:getetag/>
		<D:supportedlock/>
		<D:lockdiscovery/>
		<D:resourcetype/>
	</D:prop>
</D:propfind>
-----------------
 */

#include <strings.h>

#include "resource.h"
#include "webdav.h"

const char *PropFind = "propfind";
const char *Prop = "prop";
const char *PropName = "propname";
const char *AllProps = "allprops";
const char *CreationDate = "creationdate";
const char *GetContentLength = "getcontentlength";
const char *DisplayName = "displayname";
const char *Source = "source";
const char *GetContentLanguage = "getContentlanguage";
const char *GetContentType = "getcontenttype";
const char *Executable = "executable";
const char *GetLastModified = "getlastmodified";
const char *GetETag = "getetag";
const char *SupportedLock = "supportedlock";
const char *LockDiscovery = "lockdiscovery";
const char *ResourceType = "resourcetype";
const char *WIRsType ="wi:rstype";

static void initPropFind(wdPropFind_t *pf) {
    pf->status = PFS_start;
    pf->allProps = false;
    pf->propName = false;
    pf->props.creationDate = false;
    pf->props.displayName = false;
    pf->props.executable = false;
    pf->props.getContentLanguage = false;
    pf->props.getContentLength = false;
    pf->props.getContentType = false;
    pf->props.getETag = false;
    pf->props.getLastModified = false;
    pf->props.lockDiscovery = false;
    pf->props.resourceType = false;
    pf->props.source = false;
    pf->props.supportedLock = false;
}

static void XMLCALL startElement(void *userData, const char *name, const char **atts) {
    if (strncmp(name, "DAV::", 5) == 0) {
        name += 5;
        wdPropFind_t *pf = (wdPropFind_t *) userData;
        if (pf->status == PFS_start) {
            pf->status = (strcasecmp(name, PropFind) == 0) ? PFS_propfind : PFS_error;
        } else if (pf->status == PFS_propfind) {
            if (strcasecmp(name, Prop) == 0) {
                pf->status = PFS_prop;
            } else if (strcasecmp(name, PropName) == 0) {
                pf->propName = true;
                pf->status = PFS_propname;
            } else {
                pf->status = PFS_error;
            }

        } else if (pf->status == PFS_prop) {
            if (strcasecmp(name, AllProps) == 0) {
                pf->allProps = true;
                pf->status++;
            } else if (strcasecmp(name, CreationDate) == 0) {
                pf->props.creationDate = true;
                pf->status++;
            } else if (strcasecmp(name, GetContentLength) == 0) {
                pf->props.getContentLength = true;
                pf->status++;
            } else if (strcasecmp(name, DisplayName) == 0) {
                pf->props.displayName = true;
                pf->status++;
            } else if (strcasecmp(name, Source) == 0) {
                pf->props.source = true;
                pf->status++;
            } else if (strcasecmp(name, GetContentLanguage) == 0) {
                pf->props.getContentLanguage = true;
                pf->status++;
            } else if (strcasecmp(name, GetContentType) == 0) {
                pf->props.getContentType = true;
                pf->status++;
            } else if (strcasecmp(name, Executable) == 0) {
                pf->props.executable = true;
                pf->status++;
            } else if (strcasecmp(name, GetLastModified) == 0) {
                pf->props.getLastModified = true;
                pf->status++;
            } else if (strcasecmp(name, GetETag) == 0) {
                pf->props.getETag = true;
                pf->status++;
            } else if (strcasecmp(name, SupportedLock) == 0) {
                pf->props.supportedLock = true;
                pf->status++;
            } else if (strcasecmp(name, LockDiscovery) == 0) {
                pf->props.lockDiscovery = true;
                pf->status++;
            } else if (strcasecmp(name, ResourceType) == 0) {
                pf->props.resourceType = true;
                pf->status++;
            } else {
                dbg(name); //TODO almacenar propiedades desconocidas
            }
        }
    }
}

static void XMLCALL endElement(void *userData, const char *name) {
    wdPropFind_t *pf = (wdPropFind_t *) userData;
    if (pf->status > PFS_error)
        pf->status--;
}

const char *xmldec = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
const char *xmlmimetype = "application/xml; charset=\"utf-8\"";

void propFind(wcontext_t * wctx) {
    XML_Parser parser = XML_ParserCreateNS(NULL, ':');
    int isFinal;
    wdPropFind_t pf;
    initPropFind(&pf);
    request_t *rq = wctx->request;
    response_t *rs = wctx->response;
    const int contentLength = request_get_content_length(rq);
    const char *content = (contentLength > 0) ? request_get_content(rq) : NULL;

    XML_SetUserData(parser, &pf);
    XML_SetElementHandler(parser, startElement, endElement);
    if (XML_Parse(parser, content, contentLength, isFinal) == XML_STATUS_OK) {
        const char *fv = request_get_field_value(rq, "Depth");
        bool infDepth = (fv) && (strcasecmp(fv, "infinite") == 0);
        int depth = (infDepth) ? -1 : (fv) ? atoi(fv) : -1;
        response_set_status(rs, HTTP_STATUS_MULTI_STATUS);
        response_set_content_type(rs, xmlmimetype);
        u_string_t *pstr;
        crit_goto_if(u_string_create(NULL, 0, &pstr) != 0, outOfMemory);
        u_string_cat(pstr, xmldec);
        u_string_cat(pstr, "<multistatus xmlns=\"DAV:\">\n");
        printResourcePropFind(wctx, &pf, pstr, depth);
        u_string_cat(pstr, "</multistatus>\n");
        response_set_content_length(rs, u_string_len(pstr));
        io_write(wctx->out, u_string_c(pstr), u_string_len(pstr));
        u_string_free(pstr);
    }
    XML_ParserFree(parser);
    return;
outOfMemory:
    exit(1);
}
