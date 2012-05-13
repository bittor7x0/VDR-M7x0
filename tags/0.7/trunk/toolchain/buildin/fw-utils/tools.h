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
#include <byteswap.h>

#if BYTE_ORDER != BIG_ENDIAN
#	define HOST2BE16(x) bswap_16((x))
#	define HOST2BE32(x) bswap_32((x))
#else
#	define HOST2BE16(x) (x)
#	define HOST2BE32(x) (x)
#endif

#define EXEC_SIMPLE(cmd,a...)								\
	do {															\
		int __wfd, __rfd;										\
		pid_t __cpid;											\
		exec_cmd(&__wfd, &__rfd, &__cpid, cmd, a);	\
		close(__rfd);											\
		close(__wfd);											\
		wait_on_cmd(__cpid, cmd);							\
	} while (0)

void  die(char *msg, ...) __attribute__ ((format(printf,1,2),noreturn));

void exec_cmd(int *wpipefd, int *rpipefd, pid_t *cpid, char *cmd, int argc, ...);
void wait_on_cmd(pid_t cpid, const char *cmd);

#endif
