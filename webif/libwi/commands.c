/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 * Execute commands in an VDR host.
 */

#include <fcntl.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <toolbox/buf.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "commands.h"
#include "i18n.h"

char* commandsFile = "/etc/webif/commands.conf";

void printCommandsMenuLI(wcontext_t *wctx) {
    if (fileExists(commandsFile)) {
        WCTX_PRINTFT("<li%s><a href=\"" URLWI_PA(AMP) "\">%s</a>\n", 1
                , classCurrent[WCTX_CURRENT_PAGE(PN_COMMANDS)]
                , PN_COMMANDS, PA_COMMANDS_SHOW, tr("commands")
                );
        WCTX_IPRINTT("</li>", -1, 0);
    }
}

static int cgi_set_blocking(int fd) {
    int flags;
    warn_err_sif((flags = fcntl(fd, F_GETFL)) < 0);
    nop_err_if(fcntl(fd, F_SETFL, flags & (~O_NONBLOCK)) < 0);
    return 0;
err:
    return ~0;
}

static void doExecuteCommand(wcontext_t *wctx, const char *command) {
    //adapted from cgi_sup.c:

    enum {
        RD_END, WR_END
    };
    int ipc[2] = {-1, -1};
    pid_t pid;

    crit_err_if(pipe(ipc) < 0);
    crit_err_if((pid = fork()) < 0);
    if (pid == 0) { /* child */
        close(ipc[RD_END]);
        crit_err_if(dup2(ipc[WR_END], STDOUT_FILENO) < 0);
        //crit_err_if(dup2(ipc[WR_END], STDERR_FILENO) < 0);
        info("comando: [%s]", command);
        cgi_set_blocking(STDOUT_FILENO);
        crit_err_sif(execl("/bin/sh", "/bin/sh", "-c", command, (char*) NULL));
    } else if (pid > 0) { /* parent */
        int status;
        int cont;
        char buffer[256];

        // establecer un sighandler para poder matar el proceso hijo si el cliente web cierra la conexion
        struct sigaction sa;
        struct sigaction oldsa;
        sa.sa_handler = SIG_IGN;
        sigfillset(&sa.sa_mask);
        sigaction(SIGPIPE, &sa, &oldsa);
        info("sigpipe handler establecido en padre");

        close(ipc[WR_END]);
        bool cancel = false;
        while (!cancel && (cont = read(ipc[RD_END], buffer, sizeof (buffer))) > 0) {
            if (WCTX_PRINTF("%s", wctxHtmlEncodeN(wctx, buffer, cont)) != 0) {
                cancel = true;
                break;
            }
            if (io_flush(wctx->out) != 0) {
                cancel = true;
                break;
            }
        }
        if (cancel) {
            //el cliente web ha cerrado el socket
            //tenemos que indicar al proceso hijo que termine
            info("Enviando SIGTERM a pid[%d]", pid);
            kill(pid, SIGTERM);
        }
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status)) {
            warn("proceso hijo terminado con estado [%d]", WEXITSTATUS(status));
        }

        // restore SIGPIPE handler
        sigaction(SIGPIPE, &oldsa, NULL);
        info("sigpipe handler restaurado");
    } else {
        warn_err("fork error");
    }
err:
    if (pid == 0) { /*child*/
        _exit(EXIT_FAILURE);
    }
    if (ipc[0] != -1) close(ipc[0]);
    if (ipc[1] != -1) close(ipc[1]);
}

typedef int parseCommandFunction_t(wcontext_t *wctx, const char *command, bool confirm,
        int n, const char *title, int level, int levelant);

int parseCommandsFile(wcontext_t *wctx, parseCommandFunction_t *parseCommand) {
    int n = 0;
    u_buf_t *ubuf;
    crit_goto_if(u_buf_create(&ubuf), outOfMemory);
    if (u_buf_load(ubuf, commandsFile) == 0) {
        char *s = u_buf_ptr(ubuf);
        if (s && s[0]) {
            int level = 0;
            int levelant = -1;
            char *sep;
            char *title;
            char *command;
            bool confirm;
            char * line;
            for (line = strtok(s, "\r\n"); line != 0; line = strtok(0, "\r\n")) {
                sep = strchr(line, ':');
                if (sep) {
                    *sep = 0;
                    title = line;
                    command = sep + 1;
                    for (sep--; sep > line && *sep == ' '; sep--) {
                        *sep = 0;
                    }
                    if (*sep == '?') {
                        confirm = true;
                        *sep = 0;
                    } else {
                        confirm = false;
                    }
                    for (level = 0; *title == '-'; title++) level++;
                    while (*command == ' ') command++;
                    if (strlen(title) > 0 && strlen(command) > 0) {
                        n++;
                        if (!parseCommand(wctx, command, confirm, n, title, level, levelant)) {
                            break;
                        }
                    }
                    levelant = level;
                }
            }
        }
    }
    u_buf_free(ubuf);
    return n;
outOfMemory:
    exit(EXIT_FAILURE);
}

int printCommand(wcontext_t *wctx, const char *command, bool confirm,
        int n, const char *title, int level, int levelant) {
    if (level > levelant) {
        WCTX_IPRINT("<ul class=\"commands\">");
    } else if (n > 0) {
        if (level < levelant) {
            WCTX_IPRINTT("</li></ul>\n", -1, 0);
        }
        WCTX_IPRINTT("</li>", -1, 0);
    }
    WCTX_PRINTFT("<li class=\"command\" title=\"%s\">\n", 1
            , wctxHtmlEncode(wctx, command));
    WCTX_IPRINTF("<a class=\"command%s\" href=\"" URLWI_PA(AMP) AMP "n=%d" AMP "title=%s" "\">%s</a>\n"
            , (confirm) ? " confirm" : "", PN_COMMANDS, PA_COMMANDS_EXECUTE, n, wctxUrlEncode(wctx, title, NULL), title);
    return 1;
}

void printCommands(wcontext_t *wctx) {
    WCTX_IPRINTT("<table id=\"commands\" class=\"layout\">\n", 0, 1);
    WCTX_IPRINT("<col class=\"menu\"/>\n");
    WCTX_IPRINT("<col class=\"result\"/>\n");
    WCTX_IPRINTT("<tbody>\n", 0, 1);
    WCTX_IPRINTT("<tr>\n", 0, 1);
    WCTX_IPRINTT("<td>\n", 0, 1);
    int n = parseCommandsFile(wctx, printCommand);
    if (n > 0) {
        WCTX_IPRINTT("</li></ul>\n", -1, 0);
    }
    WCTX_IPRINTT("</td>\n", -1, 0);
    WCTX_IPRINTT("<td id=\"resultContainer\" class=\"level3Div\">\n", 0, 1);
    WCTX_IPRINTF("<h3 class=\"level3Head\">%s</h3>\n", tr("response"));
    WCTX_IPRINT("<iframe id=\"result\" class=\"level3\"></iframe>\n");
    WCTX_IPRINTT("</td>" HTMLCOMENT("level3Div") "\n", -1, 0);
    WCTX_IPRINTT("</tr>\n", -1, 0);
    WCTX_IPRINTT("</tbody>\n", -1, 0);
    WCTX_IPRINTT("</table>\n", -1, 0);
}

/**
 * Executes command only if parameters passed match with file contents.
 * 
 * @param wctx
 * @param argCommandTitle
 * @param argCommandNum
 */
void executeCommand(wcontext_t *wctx, const char *argCommandTitle, const int argCommandNum) {
    char *execCommand = NULL;

    int checkCommand(wcontext_t *wctx, const char *command, bool confirm,
            int n, const char *title, int level, int levelant) {
        if (n == argCommandNum) {
            if (strcmp(argCommandTitle, title) == 0) {
                execCommand = strdup(command);
            } else {
                WCTX_MESSAGE(MT_ALERT, tr("commands"), tr("commands.err.titleMismatch"));
            }
            return 0;
        }
        return (n < argCommandNum);
    }

    parseCommandsFile(wctx, checkCommand);
    if (execCommand) {
        doExecuteCommand(wctx, execCommand);
        free(execCommand);
    }
}

static void printHtmlHeadExtra(wcontext_t * wctx, const char *title) {
    if (wctx->action == PA_COMMANDS_SHOW) {
        printCssLinkTag(wctx, "commands.css", "screen");
        printJavascriptTag(wctx, "wi.commands.js");
        openJavascriptTag(wctx);
        WCTX_IPRINT("$(function(){webif.commandsPageInit();});\n");
        WCTX_IPRINTT("$.extend(webif.messages,{\n", 0, 1);
        WCTX_IPRINTF("commandConfirm:'%s',\n", tr("command.execute.confirm"));
        WCTX_IPRINTT("});\n", -1, 0);
        closeJavascriptTag(wctx);
    }
}

void printCommandsPage(wcontext_t *wctx) {
    if (!fileExists(commandsFile)) {
        response_set_status(wctx->response, HTTP_STATUS_NO_CONTENT);
        return;
    }
    vars_t *rqargs = request_get_args(wctx->request);
    wctx->action = (vars_countn(rqargs, ARG_ACTION) > 0) ? vars_get_value_i(rqargs, ARG_ACTION) : PA_COMMANDS_SHOW;
    if (wctx->action == PA_COMMANDS_EXECUTE) {
        const int argCommandNum = vars_get_value_i(rqargs, "n");
        const char *argCommandTitle = vars_get_value(rqargs, "title");
        if (argCommandNum > 0 && argCommandTitle) {
            response_disable_caching(wctx->response);
            io_codecs_remove(response_io(wctx->response));
            wctx->decoratePage = false;
            initHtmlPage(wctx, tr("commands"), printHtmlHeadExtra, NULL);
            WCTX_IPRINTT("<pre>", 0, 1);
            executeCommand(wctx, argCommandTitle, argCommandNum);
            WCTX_IPRINTT("</pre>", -1, 0);
            finishHtmlPage(wctx);
        } else {
            //TODO enviar respuesta de error
        }
    } else if (wctx->action == PA_COMMANDS_SHOW) {
        initHtmlPage(wctx, tr("commands"), printHtmlHeadExtra, NULL);
        printCommands(wctx);
        finishHtmlPage(wctx);
    }
}
