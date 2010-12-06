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
 * $Id: image.h 362 2008-08-17 17:14:52Z andreas $
 *
 */
#ifndef __IMAGE_H
#define __IMAGE_H 1
#include <stdint.h>

#define SYSTEM_TYPE_UNKNOWN 0
#define SYSTEM_TYPE_M740 1
#define SYSTEM_TYPE_M750S 2
struct image_part {
	int bar;
	const char *part_name;
	uint8_t *img;
	uint32_t sha1[5];
	int offset;
	int size;
};

struct image {
	int system_type;
	uint32_t flasher_sha1[5];
	uint32_t flasher_size;
	struct image_part rootfs;
	struct image_part jffs;
	struct image_part kernel;
};

int read_image(int fd, struct image *img, int step_size);
int read_image_footer(int fd, struct image *img);
int check_flasher(int fd,  struct image *img);
void close_image(struct image *img);
#endif
