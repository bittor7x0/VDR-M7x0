/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_CONF_H
#define WI_CONF_H

#include <klone/io.h>
#include <klone/session.h>
#include <klone/response.h>
#include <klone/request.h>
#include <regex.h>
#include <stdbool.h>
#include <sys/types.h>

#include "css.h"
#include "fragment.h"
#include "message.h"

enum charset_e {
    CHARSET_MIN,
    UTF8=CHARSET_MIN,
    ISO885915,
    CHARSET_MAX=ISO885915
};

extern const char *charset[];

typedef enum sortField_e {
    SF_NONE,
    SF_DEFAULT,
    SF_START,
    SF_TYPE,
    SF_TITLE,
    SF_RC_NUMBER,
    SF_CH_NUMBER,
    SF_TM_NUMBER,
    SF_SE_NUMBER,
    SF_CH_NAME,
    SF_SEARCH,
    SF_PRIORITY,
    SF_LIFETIME,
    SF_MUX,
    SF_HOST,
    SF_DIRECTORY
} sortField_t;

typedef enum sortDirection_e {
    SD_DESC = -1,
    SD_NONE,
    SD_ASC = 1
} sortDirection_t;

typedef enum pageNumber_e {
    PN_NONE,
    PN_EPG,
    PN_CHANNELS,
    PN_TIMERS,
    PN_SEARCHES,
    PN_RESOURCES,
    PN_SETTINGS,
    PN_FILES,
    PN_LIVE_STREAM,
    PN_CURRENT_IMAGE,
    PN_PLAYLIST_RSR,
    PN_PLAYLIST_CHN,
    PN_COMMANDS,
    PN_ERROR,
    PN_INDEX = PN_EPG
} pageNumber_t;

typedef enum pageAction_e {
    PA_NO_ACTION,
    PA_DEFAULT,
    PA_LIST,
    PA_CANCEL,
    PA_EDIT_FORM,
    PA_EDIT_CONFIRM,
    PA_NEW,
    PA_ADD,
    PA_DELETE,
    PA_SHOW,
    PA_SERVE_PLAYLIST,
    PA_SERVE_RSR,
    PA_PRINT,
    PA_DOWNLOAD,
    PA_DOWNLOAD_ALL,
    PA_SAVE_CONFIG,
    PA_READ_CONFIG,
    PA_TIMER_CREATE_FROM_EVENT,
    PA_SEARCH_CREATE_FROM_EVENT,
    PA_GET_RSR_CONTENT,
    PA_GET_EPG_NN,
    PA_GET_EPG_BY_ID,
    PA_COMMANDS_SHOW,
    PA_COMMANDS_EXECUTE,
    PA_DELETE_PLUS,
} pageAction_t;

typedef enum playlistType_e {
    PL_MIN,
    PL_M3U = PL_MIN,
    PL_XSPF,
    PL_MAX = PL_XSPF
} playlistType_t;

typedef enum systemType_e {
    ST_UNKNOWN,
    ST_M740AV,
    ST_M750S,
    ST_M750C
} systemType_t;

typedef enum cfgFileId_e { //indexes in fileMapping
    CF_MIN,
    CF_WEBIFCONF = CF_MIN,
    CF_RCCONF,
    CF_VDRCONF,
#ifdef ENABLE_BOXAMP_CONF
    CF_BOXAMPCONF,
    CF_MAX = CF_BOXAMPCONF
#else
    CF_MAX = CF_VDRCONF
#endif
} cfgFileId_t;

typedef struct settingConfig_s settingConfig_t;

typedef struct setting_s {
    char *name;
    char *value;
    char *oldValue;
    char *comment;
    settingConfig_t *config;
    bool isValid;
    bool written;
    bool saveCookie;
    bool deleteCookie;
} setting_t;

typedef struct wcontext_s wcontext_t;

typedef void settingValidateInputFunction_t(const settingConfig_t * const cfg, setting_t * const setting);
typedef bool settingPrintInputFunction_t(wcontext_t *wctx, const settingConfig_t * const cfg, const char *id
        , const char *name, const char *cssClass, int idx, const char *value);
typedef bool settingSetFieldFunction_t(void *configurable, setting_t * const setting);

struct settingConfig_s {
    int displayOrder; //Display order
    const char *name; // Name of the param
    const char *defaultValue; // Default value if needed
    const char *options; // If multiple choice enter the options here, seperated by |
    bool isIndex; // Value is index in options
    int indexOffset; // Add to value to get index
    int size; // Input size
    bool alreadySet; // Param is present in conf file
    bool cookie; // Only for webif: parameter can be stored in browser as cookie
    // For this to have effect, setField must also be assigned.
    settingValidateInputFunction_t *validate; // Function to validate a param
    settingPrintInputFunction_t *printInput; // Function to print an input for the param in a form
    settingSetFieldFunction_t *setField; // Set a field in one of the conf structs
};

typedef struct settingConfigList_s {
    bool exclusive; //Ignore params not listed
    int length;
    settingConfig_t * const entry;
} settingConfigList_t;

typedef struct settingList_s {
    int hostId;
    cfgFileId_t fileId;
    int length;
    setting_t *entry;
} settingList_t;

typedef struct cfgFile_s {
    const char *i18nKey;
    const char *fileName;
    bool canCreateNew;
} cfgFile_t;

enum hostId_e {
    HI_NOT_ASSIGNED = -2,
    HI_ANY_HOST
};

typedef struct hostConf_s {
    int id;
    char *name; //public name of the server
    char *ip; //ip as seen by the webif server
    uint16_t port; //port where SVDRP is listening
    char *video0; //path to video0
    bool isVdr; //VDR is running in host
    int webifVersion; //version of webif in host, 0 if none
    int socket;
} hostConf_t;

typedef struct hostList_s {
    int length;
    int numVDRs;
    hostConf_t * entry;
} hostList_t;

typedef struct webifConf_s {
    bool alreadySet;
    time_t mtime; //st_mtime of conf file in last read
    int langId;
    hostList_t hosts;
    playlistType_t playlistType;
    bool deletionDisabled;
    bool configChangeDisabled;
    bool configViewDisabled;
    bool useExternalWwwFolder;
    bool displayHostId; //display host id in recordings
    bool printRecFolderSummary;
    bool ajaxDisabled;
    int maxDepth; //maximum directory depth while browsing
    int defaultMarginStart;
    int defaultMarginStop;
    int videoWidth;
    int videoHeight;
    int epgGridHours;
    bool noChannelLogos;
    char *user;
    char *password;
    char *eventSearchUrl;
    char *bodyFontSize;
    cssTheme_t cssTheme;
    bool useHtml5VideoTag;
    char *version;
    enum charset_e filesystemEncoding; //encoding of filenames in this host
    bool noCssColumns;
    bool noTextShadow;
    bool noBoxShadow;
} webifConf_t;

struct wcontext_s {
    session_t *session;
    request_t *request;
    response_t *response;
    void *pageContext;
    io_t *out;
    int ntabs;
    int rsHostId;
    char *rsPath; //URL extra path info
    resourceType_t rsType;
    pageNumber_t page;
    pageAction_t action;
    int channelNum;
    sortField_t sortField;
    sortDirection_t sortDirection;
    int bufferLength;
    char *buffer; //TODO usar u_string_t
    bool isAjaxRequest;
    bool isReload;
    bool decoratePage;
    messageList_t messages;
    webifConf_t conf; //needed in case values are assigned by cookies
};

extern webifConf_t webifConf;
extern const cfgFile_t fileMapping[];
extern const int fileMappingLength;
extern settingConfigList_t settingConfigList[];

settingConfig_t *getSettingConfig(cfgFileId_t cfgFileId, const char *name);
bool validateCnfFileId(int fileId, wcontext_t *wctx);
bool readConf(settingList_t * const params);
bool writeConf(int hostId, settingList_t * const params);
void initWebifConf();
void freeWebifConf();
bool readWebifConf();
hostConf_t *getHost(int hostId);
hostConf_t *getFirstVdrHost();
bool isLocalhost(hostConf_t *host);
const char *getHostHttpAddr(hostConf_t *host, wcontext_t *wctx);
settingPrintInputFunction_t settingPrintInputText;
settingPrintInputFunction_t settingPrintInputPassword;
settingPrintInputFunction_t settingPrintCheckbox;
settingPrintInputFunction_t settingPrintSelect;
settingPrintInputFunction_t settingPrintRadio;


#endif
