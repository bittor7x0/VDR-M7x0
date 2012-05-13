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

#ifndef __TOOLS_H
#define __TOOLS_H 1

#include <endian.h>
#include <stdio.h>
#include <byteswap.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#define SYSLOG_ERR_NE(msg...) syslog(LOG_ERR, msg)
#define SYSLOG_WARN(msg...) syslog(LOG_WARNING, msg)
#define SYSLOG_INFO(msg...) syslog(LOG_INFO, msg)
#define SYSLOG_DEBUG(msg...) syslog(LOG_DEBUG, msg)

#define SYSLOG_ERR(msg...) 												\
	do {																			\
		char _sl_buf[512];													\
		int _en_save = errno;												\
		snprintf(_sl_buf, 512, msg);										\
		_sl_buf[511] = 0;														\
		SYSLOG_ERR_NE("%s (ERROR %d: %s)", _sl_buf, _en_save, 	\
				strerror(_en_save)); 										\
		errno = _en_save;														\
	} while(0)

#define DIE(msg...)		\
	do {						\
		SYSLOG_ERR(msg);	\
		exit(1);				\
	} while(0)

#if BYTE_ORDER != BIG_ENDIAN
#	define HOST2BE16(x) bswap_16((x))
#	define HOST2BE32(x) bswap_32((x))
#	define LE2HOST16(x) (x)
#	define LE2HOST32(x) (x)
#else
#	define HOST2BE16(x) (x)
#	define HOST2BE32(x) (x)
#	define LE2HOST16(x) bswap_16((x))
#	define LE2HOST32(x) bswap_32((x))
#endif

#define EXEC_SIMPLE(cmd,a...)									\
	do {																\
		int __wfd, __rfd;											\
		pid_t __cpid;												\
		exec_cmd(&__wfd, &__rfd, -1, &__cpid, cmd, a);	\
		__cmd_out_for_exec(__rfd, cmd);						\
		wait_on_cmd(__cpid, cmd);								\
		close(__rfd);												\
		close(__wfd);												\
	} while (0)

#define EXEC_SIMPLE_RET(ret,cmd,a...)						\
	do {																\
		int __wfd, __rfd;											\
		pid_t __cpid;												\
		exec_cmd(&__wfd, &__rfd, -1, &__cpid, cmd, a);	\
		__cmd_out_for_exec(__rfd, cmd);						\
		ret = wait_on_cmd(__cpid, cmd);						\
		close(__rfd);												\
		close(__wfd);												\
	} while (0)

void __cmd_out_for_exec(int fd, char *cmd);
void *pmalloc(size_t size);
int rm_all_dirs_prefixed(char *prefix, char *dir);
int mk_all_dirs_prefixed(const char *prefix, const char *dir);
void exec_cmd(int *wpipefd, int *rpipefd, int apipefd, pid_t *cpid, char *cmd, int argc, ...);
int wait_on_cmd(pid_t cpid, const char *cmd);



#endif
