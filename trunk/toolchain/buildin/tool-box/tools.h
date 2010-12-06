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
 * $Id: tools.h 169 2007-09-17 17:36:43Z andreas $
 *
 */
#ifndef __TOOLS_H
#define __TOOLS_H 1
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>

extern int use_syslog;
#define ERROR_LOG_NE(msg...)							\
	do {														\
		if (use_syslog) {									\
			syslog(LOG_ERR, msg);						\
		} else {												\
			fprintf(stderr,"ERROR: ");					\
			fprintf(stderr, msg);						\
			fprintf(stderr,"\n");						\
			fflush(stderr);								\
		}														\
	} while(0)

#define ERROR_LOG(msg...)												\
	do {																		\
		char _sl_buf[512];												\
		int _en_save = errno;											\
		snprintf(_sl_buf, 512, msg);									\
		_sl_buf[511] = 0;													\
		ERROR_LOG_NE("%s (ERROR %d: %s)", _sl_buf, _en_save, 	\
				strerror(_en_save)); 									\
		errno = _en_save;													\
	} while(0)

#define WARN_LOG(msg...)								\
	do {														\
		if (use_syslog) {									\
			syslog(LOG_WARNING, msg);					\
		} else {												\
			fprintf(stderr,"WARNING: ");				\
			fprintf(stderr, msg);						\
			fprintf(stderr,"\n");						\
			fflush(stderr);								\
		}														\
	} while(0)

#define INFO_LOG(msg...)								\
	do {														\
		if (use_syslog) {									\
			syslog(LOG_INFO, msg);						\
		} else {												\
			fprintf(stderr,"INFO: ");					\
			fprintf(stderr, msg);						\
			fprintf(stderr,"\n");						\
			fflush(stderr);								\
		}														\
	} while(0)

void init_syslog(const char *ident);
void close_syslog(void);
#endif
