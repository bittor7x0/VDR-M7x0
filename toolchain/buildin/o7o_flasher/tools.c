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
 * $Id: tools.c 149 2007-09-04 15:41:55Z andreas $
 *
 */
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "tools.h"

void *mmap_alloc(size_t size)
{
	void *p;
	size_t page_size;

	page_size = sysconf(_SC_PAGESIZE) - 1;
	if (page_size <= 0)
		return NULL;
	size = (size + page_size) & ~page_size;
	p = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (p == MAP_FAILED)
		return NULL;

	return p;
}

void mmap_free(void *buf, size_t size)
{
	size_t page_size;

	page_size = sysconf(_SC_PAGESIZE) - 1;
	size = (size + page_size) & ~page_size;

	if (munmap(buf, size)) {
		PRINT_ERROR("Cannot munmap!");
	}
}

int protect_mem(void *buf, size_t size)
{
	size_t page_size;

	page_size = sysconf(_SC_PAGESIZE) - 1;
	if (page_size <= 0)
		return -1;
	size = (size + page_size) & ~page_size;
	return mprotect(buf, size, PROT_READ);
}

int unprotect_mem(void *buf, size_t size)
{
	size_t page_size;

	page_size = sysconf(_SC_PAGESIZE) - 1;
	if (page_size <= 0)
		return -1;
	size = (size + page_size) & ~page_size;
	return mprotect(buf, size, PROT_READ | PROT_WRITE);
}

ssize_t read_safer(int fd, void *buf, size_t count)
{
	ssize_t readed;
	ssize_t r;
	uint8_t *p;

	r = 0;
	readed = 0;
	p = buf;
	while (readed < count) {
		r = read (fd, p + readed, count - readed);
		if (r < 0 && FATAL_ERRNO) {
			return r;
		} else if (r == 0) {
			return readed;
		}
		readed += r;
	}
	return readed;
}
