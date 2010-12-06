/*
 * Copyright (C) 2007 Andreas Koch - the open7x0.org group
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 * $Id: tools.c 224 2007-10-13 14:03:44Z andreas $
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "tools.h"


void __attribute__ ((format(printf,1,2),noreturn)) die(char *msg, ...)
{
  va_list ap;
  fprintf(stderr,"Error: ");
  va_start(ap,msg);
  vfprintf(stderr, msg, ap);
  va_end(ap);

  fprintf(stderr,"\n");
  exit(1);
}

void exec_cmd(int *wpipefd, int *rpipefd, pid_t *cpid, char *cmd, int argc, ...)
{
  va_list ap;

  char *argv[argc + 2];
  int i;

  int pipe_to_child[2];
  int pipe_from_child[2];
  pid_t pid;

  argv[0] = cmd;
  argv[argc + 1] = NULL;

  va_start(ap, argc);
  for (i = 1; i <= argc; i++) {
    argv[i] = va_arg(ap,char *);
  }
  va_end(ap);

  if (pipe(pipe_to_child))
    die("Cannot open pipe for writing to command %s", cmd);

  if (pipe(pipe_from_child))
    die("Cannot open pipe for reading from command %s", cmd);

  // Otherwise writeing on a pipe results sometimes
  // in a termination without an error msg (if child already terminated)
  signal(SIGPIPE, SIG_IGN);
  if ((pid = fork()) < 0)
    die("Cannot fork child for command %s", cmd);

  if (pid > 0) {
    *cpid = pid;

    close(pipe_to_child[0]);
    *wpipefd = pipe_to_child[1];

    close(pipe_from_child[1]);
    *rpipefd = pipe_from_child[0];

  } else {
    int fds_count;
    if (dup2(pipe_to_child[0],STDIN_FILENO) == -1) {
      _exit(-1);
    }

    if (dup2(pipe_from_child[1],STDOUT_FILENO) == -1) {
      _exit(-1);
    }

    // close unneed fds in child;
    fds_count = getdtablesize();
    for (i = STDERR_FILENO + 1; i < fds_count; i++)
      close(i);

    execvp(cmd, argv);
    _exit(-1);
  }
}

void wait_on_cmd(pid_t cpid, const char *cmd)
{
  int ret;
  int status;

  ret = waitpid(cpid, &status, 0);

  if (ret <= 0 || !WIFEXITED(status) || WEXITSTATUS(status))
    die("Command %s exits with status %d", cmd, WEXITSTATUS(status) );
  signal(SIGPIPE, SIG_DFL);
}
