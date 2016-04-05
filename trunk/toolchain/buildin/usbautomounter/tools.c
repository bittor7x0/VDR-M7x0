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
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include "tools.h"



void *pmalloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);
	if (ptr)
		return ptr;

	DIE("Cannot alloc memory");
	return NULL; // Never reached
}

int rm_all_dirs_prefixed(char *prefix, char *dir)
{
	int dir_len;
	int prefix_len;
	char *part_dir;
	dir_len = strlen(dir);
	prefix_len = strlen(prefix);
	if (dir_len <= prefix_len || memcmp(prefix, dir, prefix_len)) {
		SYSLOG_ERR_NE("rm dirs: Prefix '%s' not found in dir '%s'",
				prefix, dir);
		return -1;
	}

	part_dir = pmalloc(dir_len + 1);
	memcpy(part_dir, dir, dir_len + 1);

	while (dir_len > prefix_len) {
		char *p;
		if (part_dir[dir_len] == '/') {
			part_dir[dir_len] = 0;
			dir_len --;
			continue;
		}
		if (remove(part_dir) && errno != ENOENT && errno != EBUSY) {
			SYSLOG_ERR("Cannot remove dir '%s'", part_dir);
			free(part_dir);
			return -1;
		}
		p = memrchr(part_dir, '/', dir_len);
		*p = 0;
		dir_len = p - part_dir;
	}
	free(part_dir);
	return 0;
}

int mk_all_dirs_prefixed(const char *prefix, const char *dir)
{
	int dir_len;
	int prefix_len;
	char *part_dir;
	dir_len = strlen(dir);
	prefix_len = strlen(prefix);

	part_dir = pmalloc(dir_len + 1);
	memcpy(part_dir , dir, dir_len + 1);

	while (prefix_len < dir_len) {
		char *p;
		if (part_dir[prefix_len] == '/') {
			prefix_len++;
			continue;
		}

		p = memchr(part_dir + prefix_len, '/', dir_len - prefix_len);
		if (p) {
			*p = 0;
		}

		if (mkdir(part_dir, 0755)) {
			struct stat st;
			if (errno != EEXIST) {
				SYSLOG_ERR("Cannot mkdir '%s'", part_dir);
				free(part_dir);
				return -1;
			}
			if (lstat(part_dir, &st)) {
				SYSLOG_ERR("Cannot lstat '%s'", part_dir);
				free(part_dir);
				return -1;
			}
			if (!S_ISDIR(st.st_mode)) {
				if (remove(part_dir) || mkdir(part_dir, 0755)) {
					SYSLOG_ERR("Cannot remove file and mkdir '%s'", part_dir);
					free(part_dir);
					return -1;
				}
			}
		}
		if (p) {
			*p = '/';
			prefix_len = p - part_dir + 1;
		}
		else
			break;
	}
	free(part_dir);
	return 0;
}

void exec_cmd(int *wpipefd, int *rpipefd, int apipefd, pid_t *cpid, char *cmd, int argc, ...)
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
    DIE("Cannot open pipe for writing to command %s", cmd);

  if (pipe(pipe_from_child))
    DIE("Cannot open pipe for reading (stdin) from command %s", cmd);

  if ((pid = fork()) < 0)
    DIE("Cannot fork child for command %s", cmd);

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

    if (dup2(pipe_from_child[1], STDERR_FILENO) == -1) {
      _exit(-1);
    }

    // close unneed fds in child;
    fds_count = getdtablesize();
    for (i = STDERR_FILENO + 1; i < fds_count; i++)
        if (i != apipefd)
           close(i);

    if (execvp(cmd, argv) == -1) {
      _exit(-1);
    }
  }
}

int wait_on_cmd(pid_t cpid, const char *cmd)
{
  int ret;
  int status;

  ret = waitpid(cpid, &status, 0);

  if (ret < 0 || !WIFEXITED(status) || WEXITSTATUS(status))
    SYSLOG_WARN("Command %s exits with status %d", cmd, WEXITSTATUS(status) );

  if (WIFEXITED(status)) {
     return WEXITSTATUS(status);
  }
  return ret < 0 ? ret : -1;
}

void __cmd_out_for_exec(int fd, char *cmd)
{
	char *rbuf;
	int rbytes;
	int rbuf_size;
	int r;

	r = 0;
	rbytes = 0;
	rbuf_size = 0;
	rbuf = NULL;
	do {

		char *tbuf_nl;

		tbuf_nl = NULL;
		while (!rbytes || !(tbuf_nl = memchr(rbuf, '\n', rbytes))) {
			if (!rbuf_size || rbytes == rbuf_size - 1) {
				char *tmp_rbuf;

				rbuf_size += 512;
				tmp_rbuf = realloc(rbuf, rbuf_size);
				if (!tmp_rbuf)
					DIE("Cannot alloc memory");
				rbuf = tmp_rbuf;
			}

			r = read(fd, rbuf + rbytes, rbuf_size - rbytes - 1);
			if (r < 0) {
				if (errno == EBUSY || errno == EINTR || errno == EAGAIN)
					continue;

				SYSLOG_ERR("Cannot read from pipe");
				rbytes = 0;
				break;
			}

			rbytes += r;
			if (rbytes && !r) {
				rbuf[rbytes] = 0;
				SYSLOG_INFO("output of '%s': %s", cmd, rbuf);
			}

			if (!r)
				break;
		}

		if (tbuf_nl) {
			tbuf_nl[0] = 0;
			if (rbytes && !r) {
				rbuf[rbytes] = 0;
				SYSLOG_INFO("output of '%s': %s", cmd, rbuf);
			}
			tbuf_nl++;
			memmove(rbuf, tbuf_nl, rbytes - (tbuf_nl - rbuf));
			rbytes -= tbuf_nl - rbuf;
		}
	} while (r > 0);
	free(rbuf);
}