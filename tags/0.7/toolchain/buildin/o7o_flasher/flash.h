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
#ifndef __FLASH_H
#define __FLASH_H 1
#include <stdint.h>
#include <linux/mtd/mtd.h>
#include "image.h"

struct flash_part {
	int fd_block;
	int fd_char;
	mtd_info_t info;
	uint8_t *buf;
};

struct flash {
	struct flash_part rootfs;
	struct flash_part jffs;
	struct flash_part kernel;
	struct flash_part conf;
};

int flash_open(struct flash *flash);
void flash_close(struct flash *flash);
int flash_write_image(struct flash *flash, struct image *image);
int flash_verify_image(struct flash *flash, struct image *image);
int flash_set_rescue_reboot(struct flash *flash);
#endif
