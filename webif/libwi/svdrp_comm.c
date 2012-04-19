/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: Christian Kelinski <k@kille.cx>
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <klone/utils.h>

#include "misc.h"
#include "svdrp_comm.h"

//TODO utilizar toolbox/net.h para gestion de sockets !!!

/**
 * Regex to detect end of data in svdrp commands.
 * Allocated and deallocated in hooks.c.
 */
regex_t eod_regex; //regex to detect end of data in svdrp commands

int initSvdrpParser() {
    return regcomp(&eod_regex, "^[0-9]{3} ", REG_EXTENDED | REG_NOSUB | REG_NEWLINE);
}

void freeSvdrpParser() {
    regfree(&eod_regex);
}

void closeSocket(hostConf_t *host) {
    errno = 0;
    if (shutdown(host->socket, SHUT_RDWR) < 0) {
        warn(strerror(errno));
        switch (errno) {
            case EBADF:
            case ENOTSOCK:
            case ENOTCONN:
                host->socket = 0;
                return;
            default:
                break;
        }
        errno = 0;
    }
    if (close(host->socket) < 0) {
        warn(strerror(errno));
        errno = 0;
    }
    host->socket = 0;
}

char *readSvdrp(hostConf_t *host) {

    enum {
        BUFFERSZ = 4096
    };
    char buffer[BUFFERSZ] = "";
    int l = 0; //current length without trailing 0
    int n = 0;
    bool eod = false; //end of data code already received (but last line may be still incomplete)

    if (host->socket < 1) {
        warn("No socket found!");
        return NULL;
    }
    char *data = NULL; //TODO usar u_buf
    while ((n = recv(host->socket, buffer, BUFFERSZ - 1, 0)) > 0) {
        buffer[n] = '\0';
        crit_goto_if((data = realloc(data, l + n + 1)) == NULL, outOfMemory);
        data[l] = 0;
        strcat(data, buffer);
        l += n;
        if (!eod) {
            int offset = (l > n + 5) ? l - n - 5 : 0;
            eod = boolean(regexec(&eod_regex, data + offset, 0, NULL, 0) == 0);
        }
        if (eod && (data[l - 1] == '\n')) {
            break;
        }
    }
    if (n < 0 || !eod) {
        if (n < 0)
            warn(strerror(errno));
        else
            warn("SVDRP response truncated [%s]. Discarding everything", data);
        free(data);
        data = NULL;
        errno = 0;
    }
    return data;
outOfMemory:
    crit("Out of memory");
    exit(EXIT_FAILURE);
}

bool openSvdrp(hostConf_t *host) {
    if (host->socket < 1) {
        struct sockaddr_in hostAddress;
        //dbg("Opening SVDRP connection to %s:%d",host->ip,host->port);
        hostAddress.sin_family = AF_INET;
        hostAddress.sin_port = htons(host->port);
        inet_aton(host->ip, &hostAddress.sin_addr);

        // create tcp socket
        errno = 0;
        if ((host->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            warn(strerror(errno));
            host->socket = 0;
            return false;
        }

        // Connect to VDR
        errno = 0;
        if (connect(host->socket, (struct sockaddr*) &hostAddress, sizeof (hostAddress)) < 0) {
            warn(strerror(errno));
            closeSocket(host);
            return false;
        }
        char *data = readSvdrp(host);
        if (data == NULL || strtol(data, NULL, 10) != SVDRP_VDR_READY) {
            warn("Unexpected SVDRP response: %s", data);
            closeSocket(host);
        }
        free(data);
        return boolean(host->socket > 0);
    } else {
        return true;
    }
}

void closeSvdrp(hostConf_t *host) {
    if (host && host->isVdr && host->socket > 0) {
        //dbg("Closing SVDRP connection to %s:%d",host->ip,host->port);
        free(execSvdrp(host, "QUIT"));
    }
}

void closeSvdrpAll() {
    int h;
    hostConf_t *host;
    for (h = 0, host = webifConf.hosts.entry; h < webifConf.hosts.length; h++, host++) {
        if (host->isVdr && host->socket > 0) {
            closeSvdrp(host);
        }
    }
}

bool writeSvdrp(hostConf_t *host, const char *cmd) {
    dbg("SVDRP cmd [%s]", cmd);
    if (host->socket < 1) {
        warn("socket is closed");
        return false;
    } else {
        int l = strlen(cmd);
        int w = write(host->socket, cmd, l + 1);
        if (w < l) warn("Written only %d bytes from %d", w, l);
        return boolean(w >= l);
    }
}

char *execSvdrp(hostConf_t * const host, const char *fmt, ...) {
    warn_return_if(host == NULL || !host->isVdr, NULL);
    warn_return_if(fmt == NULL, NULL);
    bool isQuit = boolean(strncmp(fmt, "QUIT", 4) == 0);
    warn_return_if(isQuit && host->socket < 1, NULL);
    char *data = NULL;
    if (isQuit || openSvdrp(host)) {
        char *cmd = NULL;
        va_list ap;
        va_start(ap, fmt);
        crit_goto_if(vasprintf(&cmd, fmt, ap) == -1, outOfMemory);
        va_end(ap);
        if (writeSvdrp(host, cmd)) {
            data = readSvdrp(host);
        }
        if (isQuit) {
            closeSocket(host);
        }
        free(cmd);
    } else {
        warn("Can't establish SVDRP connection");
    }
    return data;
outOfMemory:
    exit(EXIT_FAILURE);
}

