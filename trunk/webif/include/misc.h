/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_MISC_H
#define WI_MISC_H

#include <stdbool.h>
#include <time.h>
#include "conf.h"
#include "message.h"

extern const char *checked[2];
extern const char *selected[2];
extern const char *videoTypeStr[7];
extern const char *classCurrent[2];
extern const char *tabs;
#define boolean(i) ((i)?true:false)
#define isFlagSet(flag,flags)   boolean(((flag) & (flags)) == (flag) )
#define hasFlag(flag,flags)     boolean(((flag) & (flags)) != 0 )  //one of the flags in 'flag' is set in 'flags'
#define setFlag(flag,flags)     ((flags) |=  (flag))
#define clearFlag(flag,flags)   ((flags) &= ~(flag))
#define setOrClearFlag(isSet,flag,flags)  if (isSet) {setFlag(flag,flags);} else {clearFlag(flag,flags);}
#define AJAX_REPLACE_PREFIX(id) ((wctx->isAjaxRequest)? "replace_" id : id)

#define WCTX_CURRENT_PAGE(p) boolean(wctx->page==p)
#define WCTX_CURRENT_ACTION(a) boolean(wctx->action==a)
#define WCTX_SORT_BY(sf) boolean(wctx->sortField==sf)

#define ARG_ACTION "a"
#define ARG_PAGE   "p"
#define ARG_TITLE  "title"
#define AMP "&amp;"
#define URLWI "/vdrwi.kl1"
#define URLWI_P  URLWI "?" ARG_PAGE "=%d"
#define URLWI_PA(amp)  URLWI_P amp ARG_ACTION "=%d"

#ifdef IGNORE_TABS
#define WCTX_IO_INDENT()
#define WCTX_TAB(n) 
#define HTMLCOMENT(coment)
#else
#define WCTX_IO_INDENT() \
    if (wctx->ntabs<0) { warn("Revisar markup"); wctx->ntabs=0; } \
    else if (wctx->ntabs>0) io_printf(wctx->out,"%.*s",wctx->ntabs,tabs)
#define WCTX_TAB(n) wctx->ntabs += n
#define HTMLCOMENT(coment) "<!--" coment "-->"
#endif

#define WCTX_PRINTN(str,l) io_write(wctx->out, str, l)
#define WCTX_PRINT(str) WCTX_PRINTN(str, strlen(str))
#define WCTX_PRINTT(str,postinc) \
    { \
        WCTX_PRINT(str); \
        WCTX_TAB(postinc); \
    }
#define WCTX_IPRINTN(str,l) \
    { \
        WCTX_IO_INDENT(); \
        WCTX_PRINTN(str, l); \
    }
#define WCTX_IPRINT(str) WCTX_IPRINTN(str,strlen(str))
#define WCTX_IPRINTT(str,preinc,postinc) \
    { \
        WCTX_TAB(preinc); \
        WCTX_IPRINT(str); \
        WCTX_TAB(postinc); \
    }

#define WCTX_PRINTF(str,...) io_printf(wctx->out,str,##__VA_ARGS__)
#define WCTX_PRINTFT(str,postinc,...) \
    { \
        WCTX_PRINTF(str,##__VA_ARGS__); \
        WCTX_TAB(postinc); \
    }
#define WCTX_IPRINTF(str,...) \
    { \
        WCTX_IO_INDENT(); \
        WCTX_PRINTF(str,##__VA_ARGS__); \
    }
#define WCTX_IPRINTFT(str,preinc,postinc,...) \
    { \
        WCTX_TAB(preinc); \
        WCTX_IPRINTF(str, ##__VA_ARGS__); \
        WCTX_TAB(postinc); \
    }
#define WCTX_INCTAB() WCTX_TAB(1)
#define WCTX_DECTAB() WCTX_TAB(-1)

#define WCTX_MESSAGE(type, title, msg,...) (wctx)?addMessage(&wctx->messages, type, title, msg, ##__VA_ARGS__):NULL
#define STATUS_ICON(status)  "<img src=\"/images/blank.gif\" class=\"ui-icon " status "Status\" alt=\"\"/>"

typedef void printHtmlExtraFunction_t(wcontext_t *wctx, const char *title);

bool initWCtx(wcontext_t *wctx, request_t *request, response_t *response, session_t *session, io_t *out);
void freeWCtx(wcontext_t *wctx);
int wctxBufferReserve(wcontext_t *wctx, int len);
char *wctxBufferPrintf(wcontext_t *wctx, const char * str, ...) __attribute__((format(printf, 2, 3)));
char *wctxHtmlEncode(wcontext_t *wctx, const char * str);
char *wctxHtmlEncodeN(wcontext_t *wctx, const char * str, int l);
char *wctxHtmlDecode(wcontext_t *wctx, const char * str);
char *wctxUrlEncode(wcontext_t *wctx, const char * url, const char * keep);
char *wctxUrlEncodeN(wcontext_t *wctx, const char * url, int l, const char * keep);
char *wctxUrlDecode(wcontext_t *wctx, const char * url);
char *getRequestFieldValue(wcontext_t *wctx, vars_t *args, const char *argName, bool *isACopy);
bool isDST(time_t * aTime);
bool sameDay(time_t oneDate, time_t anotherDate);
bool sameString(const char * s1, const char * s2);
bool sameInt(const int i1, const int i2);
bool sameIntEx(const char * s, const int i);
bool parseRequestStr(request_t *request, char ** pathStr, char ** queryStr);
char *strreplace(char *s, const char *s1, const char *s2);

bool fileExists(const char * fileName);
bool dirExists(const char * dirName);
char *stringIconv(const enum charset_e toCode, const enum charset_e fromCode, const char *fromStr, const int fromLen);
char *vdrDecode(char *name);
char *vdrEncode(char *name);
void initHtmlDoc(wcontext_t *wctx);
void initHtmlPage(wcontext_t *wctx, const char *title,
        printHtmlExtraFunction_t *printHtmlHeadExtra,
        printHtmlExtraFunction_t * printTopInnerHtml);
void finishHtmlPage(wcontext_t *wctx);
void openJavascriptTag(wcontext_t *wctx);
void closeJavascriptTag(wcontext_t *wctx);
void openCssStyleTag(wcontext_t *wctx, const char * media);
void closeCssStyleTag(wcontext_t *wctx);
void printJavascriptTag(wcontext_t *wctx, const char *relsrc);
void printCssLinkTag(wcontext_t *wctx, const char *relsrc, const char *media);
void printMenu(wcontext_t *wctx);
void printMessage(wcontext_t * wctx, message_t *message);
void printMessages(wcontext_t *wctx);
void printList1TH(wcontext_t *wctx, sortField_t aSortField, const char *label);
void openInputTag(wcontext_t *wctx, const char *tag, const char * type, const char *id, const char *name, int idx, const char *cssClass, char ** ptr_id, bool ownLine);
void printInput(wcontext_t *wctx, const char *type, const char *id, const char *name, const char *cssClass, int idx, const char *value, int l);
void printSetDatePickerDefaults(wcontext_t *wctx);
void printVDRSelect(wcontext_t *wctx, const char * name, const int hostId);
void printErrorPage(wcontext_t *wctx, int errorn);

bool extractEmbededFile(const char *src, const char *dst, bool overwrite);
bool createParentFolders(const char *path, mode_t mode);
bool extractLogosFromFile(wcontext_t *wctx, const char *logos_tgz);
bool extractLogosFromRequest(wcontext_t *wctx, const char *fieldName);

char *extendPath(const char *path, const char *name);
#endif
