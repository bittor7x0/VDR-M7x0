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
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "image.h"
#include "display.h"
#include "tools.h"
#include "sha1.h"

#define IMG_MAGIC "o7o flash img"
int read_image_footer(int fd, struct image *img)
{
	uint32_t buf[256];
	char magic[16];
   int r;

	memset(img, 0, sizeof(*img));
	if (lseek(fd, -1024, SEEK_END) < 0) {
		PRINT_ERROR("Cannot seek to flash image footer");
		return -1;
	}

	r = read_safer(fd, buf, 1024);
	if (r < 0) {
		PRINT_ERROR("Cannot read flash image footer");
		return -1;
	}

	if (r != 1024) {
		PRINT_ERROR_NE("Cannot read flash image footer");
		return -1;
	}

	memcpy(magic, buf, 16);
	magic[15] = 0;
	if (strcmp(magic, IMG_MAGIC)) {
		PRINT_ERROR_NE("Magic not found!");
		return -1;
	}
	img->system_type = BE2HOST32(buf[4]);
	img->flasher_size = BE2HOST32(buf[5]);
	img->rootfs.part_name = "rootfs";
	img->rootfs.bar = DISPLAY_ROOTFS_BAR;
	img->rootfs.offset = img->flasher_size;
	img->rootfs.size = BE2HOST32(buf[6]);
	img->jffs.part_name = "jffs2";
	img->jffs.bar = DISPLAY_JFFS_BAR;
	img->jffs.offset = img->rootfs.offset + img->rootfs.size;
	img->jffs.size = BE2HOST32(buf[7]);
	img->kernel.part_name = "kernel";
	img->kernel.bar = DISPLAY_KERNEL_BAR;
	img->kernel.offset = img->jffs.offset + img->jffs.size;
	img->kernel.size = BE2HOST32(buf[8]);


	img->flasher_sha1[0] = BE2HOST32(buf[12]);
	img->flasher_sha1[1] = BE2HOST32(buf[13]);
	img->flasher_sha1[2] = BE2HOST32(buf[14]);
	img->flasher_sha1[3] = BE2HOST32(buf[15]);
	img->flasher_sha1[4] = BE2HOST32(buf[16]);

	img->rootfs.sha1[0] = BE2HOST32(buf[20]);
	img->rootfs.sha1[1] = BE2HOST32(buf[21]);
	img->rootfs.sha1[2] = BE2HOST32(buf[22]);
	img->rootfs.sha1[3] = BE2HOST32(buf[23]);
	img->rootfs.sha1[4] = BE2HOST32(buf[24]);

	img->jffs.sha1[0] = BE2HOST32(buf[28]);
	img->jffs.sha1[1] = BE2HOST32(buf[29]);
	img->jffs.sha1[2] = BE2HOST32(buf[30]);
	img->jffs.sha1[3] = BE2HOST32(buf[31]);
	img->jffs.sha1[4] = BE2HOST32(buf[32]);

	img->kernel.sha1[0] = BE2HOST32(buf[36]);
	img->kernel.sha1[1] = BE2HOST32(buf[37]);
	img->kernel.sha1[2] = BE2HOST32(buf[38]);
	img->kernel.sha1[3] = BE2HOST32(buf[39]);
	img->kernel.sha1[4] = BE2HOST32(buf[40]);

	return 0;
}

int check_flasher(int fd,  struct image *img)
{
	char *buf;
	int r;

	buf = malloc(img->flasher_size);
	if (!buf) {
		PRINT_ERROR("Cannot alloc memory for flasher");
		return -1;
	}

	if (lseek(fd, 0, SEEK_SET) != 0) {
		PRINT_ERROR("Cannot seek to flasher in image");
		free(buf);
		return -1;
	}

	r = read_safer(fd, buf, img->flasher_size);
	if (r < 0) {
		PRINT_ERROR("Cannot read flasher in image");
		free(buf);
		return -1;
	}

	if (r !=  img->flasher_size) {
		PRINT_ERROR_NE("Cannot read flasher in image");
		free(buf);
		return -1;
	}

	if (!check_sha1_hash(buf, img->flasher_size, img->flasher_sha1)) {
		PRINT_ERROR_NE("Flasher sha1-hash incorrect");
		free(buf);
		return 0;
	}

	free(buf);
	return 1;
}


static int read_image_part(int fd, struct image_part *imgp, int step_size)
{
	int readed;
	int to_read;
	int r;

	if (!imgp->size)
		return 1;

	imgp->img = mmap_alloc(imgp->size);
	if (!imgp->img) {
		PRINT_ERROR("Cannot alloc memory for %s image", imgp->part_name);
		return -1;
	}

	if (lseek(fd, imgp->offset, SEEK_SET) != imgp->offset) {
		PRINT_ERROR("Cannot seek to %s image", imgp->part_name);
		mmap_free(imgp->img, imgp->size);
		return -1;
	}

	readed = 0;
	while (readed < imgp->size) {
		to_read = step_size;
		if (to_read > imgp->size - readed)
			to_read = imgp->size - readed;

		r = read_safer(fd, imgp->img + readed, to_read);
		if (r < 0) {
			PRINT_ERROR("Cannot read %s image", imgp->part_name);
			mmap_free(imgp->img, imgp->size);
			return -1;
		}

		if (r != to_read) {
			PRINT_ERROR_NE("Cannot read %s image", imgp->part_name);
			mmap_free(imgp->img, imgp->size);
			return -1;
		}

		if (protect_mem(imgp->img + readed, to_read)) {
			PRINT_ERROR("Cannot protect %s image", imgp->part_name);
			mmap_free(imgp->img, imgp->size);
			return -1;
		}

		readed += r;
		if (display_update_bar(imgp->bar, DISPLAY_BAR_READ, readed)) {
			PRINT_ERROR_NE("Cannot update %s image bar", imgp->part_name);
			mmap_free(imgp->img, imgp->size);
			return -1;
		}
	}

	/* For safety do it again for complete buffer */
	if (protect_mem(imgp->img, imgp->size)) {
		PRINT_ERROR("Cannot protect %s image", imgp->part_name);
		mmap_free(imgp->img, imgp->size);
		return -1;
	}

	if (!check_sha1_hash(imgp->img, imgp->size, imgp->sha1)) {
		PRINT_ERROR_NE("%s image sha1 hash incorrect", imgp->part_name);
		mmap_free(imgp->img, imgp->size);
		return 0;
	}

	return 1;
}

int read_image(int fd, struct image *img, int step_size)
{
	int r;

	r = read_image_part(fd, &img->kernel,step_size);
	if (r <= 0) {
		return r;
	}

	r = read_image_part(fd, &img->jffs, step_size);
	if (r <= 0) {
		if (img->kernel.size)
			mmap_free(img->kernel.img, img->kernel.size);
		return r;
	}

	r = read_image_part(fd, &img->rootfs, step_size);
	if (r <= 0) {
		if (img->kernel.size)
			mmap_free(img->kernel.img, img->kernel.size);
		if (img->jffs.size)
			mmap_free(img->jffs.img, img->jffs.size);
		return r;
	}

	return 1;
}

void close_image(struct image *img)
{
	if (img->kernel.size)
		mmap_free(img->kernel.img, img->kernel.size);
	if (img->jffs.size)
		mmap_free(img->jffs.img, img->jffs.size);
	if (img->rootfs.size)
		mmap_free(img->rootfs.img, img->rootfs.size);
}
