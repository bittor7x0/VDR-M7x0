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
 * $Id: sha1_test.c 148 2007-08-30 15:58:15Z andreas $
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../sha1.h"
#define BUFFER_SIZE 4096

int main(int argc, char **argv)
{
	struct sha1_context sha1_context;
	int fd;
	uint8_t buf[BUFFER_SIZE];
	int len;
	int r;

	if (argc > 2) {
		uint8_t *buf2;
		uint32_t hash[5];
		struct stat st;

		r = sscanf(argv[2],"%08x%08x%08x%08x%08x",&hash[0],
				&hash[1],&hash[2],&hash[3],&hash[4]);
		if (r != 5) {
			printf("Illegal hash %s\n",argv[2]);
			return 1;
		}
		fd = open(argv[1],O_RDONLY);
		if (fd < 0) {
			printf("Cannot open file '%s'\n",argv[1]);
			return 1;
		}
		if (fstat(fd,&st)) {
			printf("Cannot file size '%s'\n",argv[1]);
			close(fd);
			return 1;
		}
		buf2 = malloc(st.st_size);
		if (!buf2) {
			printf("Cannot alloc memory\n");
			close(fd);
			return 1;
		}
		if (read(fd, buf2,st.st_size) != st.st_size) {
			printf("Cannot read file '%s'\n",argv[1]);
			close(fd);
			free(buf2);
			return 1;
		}
		if (check_sha1_hash(buf2, st.st_size, hash)) {
			printf("Hash correct!\n");
		} else {
			printf("Hash incorrect!\n");
		}
		close(fd);
		free(buf2);
		return 0;
	}

	if (argc < 2) {
		fd = fileno(stdin);
	} else {
		fd = open(argv[1],O_RDONLY);
		if (fd < 0) {
			printf("Cannot open file '%s'\n",argv[1]);
			return 1;
		}
	}

	sha1_init_context(&sha1_context);

	len = 0;
	while ((r = read(fd, buf + len, BUFFER_SIZE - len)) > 0) {
		len += r;
		r = sha1_calc_hash_part(&sha1_context, buf, len, 0);
		len -= r;
		if (len) {
			memmove(buf, buf + r, len);
		}
	}

	if (r < 0) {
		printf ("Cannot read file '%s'\n",argv[1]);
		return 1;
	}

	sha1_calc_hash_part(&sha1_context, buf, len, 1);
	printf("%08x%08x%08x%08x%08x\n", sha1_context.hash_words[0],
			sha1_context.hash_words[1], sha1_context.hash_words[2],
			sha1_context.hash_words[3], sha1_context.hash_words[4]);
	close(fd);
	return 0;
}
