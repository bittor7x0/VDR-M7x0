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
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define PRINT_ERROR(a...)								\
	do {														\
		int errno_save = errno;							\
		fprintf(stderr,"ERROR: ");						\
		fprintf(stderr, a);								\
		fprintf(stderr,"('%s' Errno %d)\n",			\
				strerror(errno_save), errno_save);	\
		fflush(stderr);									\
		errno = errno_save;								\
	} while(0)

#define PRINT_ERROR_NE(a...)							\
	do {														\
		fprintf(stderr,"ERROR: ");						\
		fprintf(stderr, a);								\
		fprintf(stderr,"\n");							\
		fflush(stderr);									\
	} while(0)

#define PRINT_WARNING(a...)							\
	do {														\
		fprintf(stderr,"WARNING: ");					\
		fprintf(stderr, a);								\
		fprintf(stderr,"\n");							\
		fflush(stderr);									\
	} while(0)

#endif
