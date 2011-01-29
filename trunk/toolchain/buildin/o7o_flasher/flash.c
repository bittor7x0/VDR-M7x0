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
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/mtd/mtd.h>
#include <linux/fs.h>
#include "flash.h"
#include "image.h"
#include "display.h"
#include "o7o_flasher.h"

#include "tools.h"

#define MTD_DEV_CHAR "/dev/mtd"
#define MTD_DEV_BLOCK "/dev/mtdblock"
#define MTD_ROOTFS "1"
#define MTD_JFFS "2"
#define MTD_CONF "3"
#define MTD_KERNEL "4"

static int flash_open_part(const char *cdev, const char *bdev,
	struct flash_part *part, int char_wr, int block_wr)
{
	int r;
	size_t page_size;

	page_size = sysconf(_SC_PAGESIZE) - 1;
	if (page_size <= 0) {
		PRINT_ERROR("Cannot get page size");
		return -1;
	}

	if (!fopts.test_mode)
		part->fd_char = open(cdev, char_wr ? O_RDWR : O_RDONLY);
	else
		part->fd_char = open(cdev, O_RDONLY);

	if (part->fd_char < 0) {
		PRINT_ERROR("Cannot open char mtd %s", cdev);
		return -1;
	}

	r = ioctl(part->fd_char, MEMGETINFO, &part->info);
	if (r) {
		PRINT_ERROR("Cannot get flash info for %s", cdev);
		goto err_close_char;
	}

	if (part->info.type != MTD_NORFLASH) {
		PRINT_ERROR_NE("Flash type is not nor flash");
		goto err_close_char;
	}

	if (part->info.erasesize & page_size) {
		PRINT_ERROR_NE("Flash erase size is not page aligned");
		goto err_close_char;
	}

	if (!fopts.test_mode)
		part->fd_block = open(bdev, block_wr ? O_RDWR : O_RDONLY);
	else
		part->fd_block = open(bdev, O_RDONLY);

	if (part->fd_block < 0) {
		PRINT_ERROR("Cannot open block mtd %s", bdev);
		goto err_close_char;
	}

	part->buf = mmap(NULL, part->info.size, PROT_READ, MAP_SHARED,
		part->fd_block, 0);
	if (part->buf == MAP_FAILED) {
		PRINT_ERROR("Cannot mmap block mtd %s", bdev);
		goto err_close_block;
	}

	return 0;
err_close_block:
	close(part->fd_block);
err_close_char:
	close(part->fd_char);
	return -1;
}

static int flash_close_part(struct flash_part *part)
{
	int r;
	int ret;

	ret = 0;
	close(part->fd_char);

	r = munmap(part->buf, part->info.size);
	if (r) {
		PRINT_ERROR("Cannot munmap flash partition");
		ret = -1;
	}

	r = fsync(part->fd_block);
	if (r) {
		PRINT_ERROR("Cannot fsync flash partition");
		ret = -1;
	}

	r = ioctl(part->fd_block, BLKFLSBUF);
	if (r) {
		PRINT_ERROR("Cannot flush flash partition");
		ret = -1;
	}

	close(part->fd_block);
	return ret;
}

int flash_open(struct flash *flash)
{
	int r;

	r = flash_open_part(MTD_DEV_CHAR MTD_ROOTFS, MTD_DEV_BLOCK MTD_ROOTFS,
		&flash->rootfs, 0, 1);
	if (r) {
		return -1;
	}

	r = flash_open_part(MTD_DEV_CHAR MTD_JFFS, MTD_DEV_BLOCK MTD_JFFS,
		&flash->jffs, 0, 1);
	if (r) {
		goto err_out_root;
	}


	r = flash_open_part(MTD_DEV_CHAR MTD_KERNEL, MTD_DEV_BLOCK MTD_KERNEL,
		&flash->kernel, 0, 1);
	if (r) {
		goto err_out_jffs;
	}

	r = flash_open_part(MTD_DEV_CHAR MTD_CONF, MTD_DEV_BLOCK MTD_CONF,
		&flash->conf, 1, 0);
	if (r) {
		goto err_out_kernel;
	}

	return 0;
err_out_kernel:
	flash_close_part(&flash->kernel);
err_out_jffs:
	flash_close_part(&flash->jffs);
err_out_root:
	flash_close_part(&flash->rootfs);
	return -1;
}

void flash_close(struct flash *flash)
{
	flash_close_part(&flash->conf);
	flash_close_part(&flash->kernel);
	flash_close_part(&flash->jffs);
	flash_close_part(&flash->rootfs);
}

static int unprotect_part(struct flash *flash, struct flash_part *part,
	struct image_part *imgp)
{
	int offset;
	int flash_size;
	int size;

	offset = 0;
	flash_size = part->info.size;
	if (part == &flash->kernel) {
		offset = 64 * 1024;
		flash_size = 12 * 64 * 1024;
	}

	size = (imgp->size + part->info.erasesize - 1) & ~(part->info.erasesize - 1);
	if (flash_size < size) {
		PRINT_ERROR_NE("Size of %s image too big (%d)", imgp->part_name, size);
		return -1;
	}

	if (!fopts.test_mode) {
		if (unprotect_mem(part->buf + offset, size)) {
			PRINT_ERROR("Cannot unprotect %s", imgp->part_name);
			return -1;
		}
	} else {
		PRINT_INFO("Unprotect %s buffer 0x%p (offset 0x%x) size 0x%x",
			imgp->part_name, part->buf + offset, offset, size);
	}

	/* for safety protect rest again */
	if (offset) {
		if (protect_mem(part->buf,offset)) {
			PRINT_ERROR("Cannot protect flash %s size 0x%x", imgp->part_name,
				 offset);
			return -1;
   	}
	}

	if (part->info.size - offset - size) {
		if (protect_mem(part->buf + offset + size,
				part->info.size - offset - size)) {
			PRINT_ERROR("Cannot protect flash %s offset 0x%x size 0x%x",
				imgp->part_name, offset + size, part->info.size - offset - size);
			return -1;
   	}
	}

	/* for boot sector and rescue kernel be extra safe, protect them always */
	if (protect_mem(flash->kernel.buf, 64 * 1024)) {
		PRINT_ERROR("Cannot protect boot sector");
		return -1;
   }
	if (protect_mem(flash->kernel.buf + 13 * 64 * 1024, 11 * 64 * 1024)) {
		PRINT_ERROR("Cannot protect rescue kernel");
		return -1;
   }
	return 0;
}

static int write_part(struct flash *flash, struct flash_part *part,
	struct image_part *imgp)
{
	int flash_offset;
	int flash_size;
	int img_offset;
	int to_write;

	flash_offset = 0;
	flash_size = part->info.size;
	if (part == &flash->kernel) {
		flash_offset = 64 * 1024;
		flash_size = 12 * 64 * 1024;
	}

	if (flash_size < imgp->size) {
		PRINT_ERROR_NE("Image %s execced size of flash (%d > %d)",
			imgp->part_name, imgp->size, flash_size);
		return -1;
	}

	if (unprotect_part(flash, part, imgp)) {
		return -1;
	}

	img_offset = 0;
	while (img_offset < imgp->size) {
		to_write = part->info.erasesize;
		if (to_write > imgp->size - img_offset)
			to_write = imgp->size - img_offset;

		if (!fopts.test_mode) {
			memcpy(part->buf + flash_offset, imgp->img + img_offset, to_write);
			if (to_write < part->info.erasesize) {
				memset(part->buf + flash_offset + to_write, 0xff,
					part->info.erasesize - to_write);
			}
		} else {
			PRINT_INFO("Write %s 0x%p (0x%x) from 0x%p (0x%x) size 0x%x",
				imgp->part_name, part->buf + flash_offset, flash_offset,
				imgp->img + img_offset, img_offset, to_write);
			if (to_write < part->info.erasesize) {
				PRINT_INFO("Memset %s 0x%p (0x%x) to 0xff size 0x%x", imgp->part_name,
					part->buf + flash_offset + to_write, flash_offset + to_write,
					part->info.erasesize - to_write);
			}
			usleep(50000);
		}

		if (msync(part->buf + flash_offset, part->info.erasesize, MS_SYNC)) {
			PRINT_ERROR("Cannot msync %s", imgp->part_name);
			return -1;
		}

		if (protect_mem(part->buf + flash_offset, part->info.erasesize)) {
			PRINT_ERROR("Cannot protect %s", imgp->part_name);
			return -1;
		}
		if (protect_mem(part->buf, flash_offset + part->info.erasesize)) {
			PRINT_ERROR("Cannot protect %s", imgp->part_name);
			return -1;
		}
		display_update_bar(imgp->bar, DISPLAY_BAR_WRITTEN, img_offset + to_write);
      img_offset += part->info.erasesize;
		flash_offset += part->info.erasesize;
	}
	return 0;
}

int flash_write_image(struct flash *flash, struct image *image)
{
	if (image->kernel.size) {
		if (write_part(flash, &flash->kernel, &image->kernel)) {
			return -1;
		}
	}

	if (image->jffs.size) {
		if (write_part(flash, &flash->jffs, &image->jffs)) {
			return -1;
		}
	}

	if (image->rootfs.size) {
		if (write_part(flash, &flash->rootfs, &image->rootfs)) {
			return -1;
		}
	}

	return 0;
}

static int verify_part(struct flash *flash, struct flash_part *part,
	struct image_part *imgp)
{
	int flash_offset;
	int flash_size;
	int img_offset;
	int to_verify;

	flash_offset = 0;
	flash_size = part->info.size;
	if (part == &flash->kernel) {
		flash_offset = 64 * 1024;
		flash_size = 12 * 64 * 1024;
	}

	if (flash_size < imgp->size) {
		PRINT_ERROR_NE("Image %s execced size of flash (%d > %d)",
			imgp->part_name, imgp->size, flash_size);
		return -1;
	}

	img_offset = 0;
	while (img_offset < imgp->size) {
		to_verify = part->info.erasesize;
		if (to_verify > imgp->size - img_offset)
			to_verify = imgp->size - img_offset;

		if (memcmp(part->buf + flash_offset, imgp->img + img_offset, to_verify)) {
			PRINT_ERROR_NE("Image %s and flash differ at offset 0x%x",
				imgp->part_name, img_offset);
			return -1;
		}

		display_update_bar(imgp->bar, DISPLAY_BAR_VERIFIED, img_offset + to_verify);
      img_offset += part->info.erasesize;
		flash_offset += part->info.erasesize;
	}
	return 0;
}

int flash_verify_image(struct flash *flash, struct image *image)
{
	if (image->kernel.size) {
		if (verify_part(flash, &flash->kernel, &image->kernel)) {
			return -1;
		}
	}

	if (image->jffs.size) {
		if (verify_part(flash, &flash->jffs, &image->jffs)) {
			return -1;
		}
	}

	if (image->rootfs.size) {
		if (verify_part(flash, &flash->rootfs, &image->rootfs)) {
			return -1;
		}
	}

	return 0;
}

#define REBOOT_FLAG_OFFSET (3 * 64 * 1024)
static int check_conf_files(struct flash_part *part)
{
	int file_count;
	int offset;
	int file_len;

	for (offset = 0; offset < part->info.size; offset++) {
		if (part->buf[offset] != 0xff)
			break;
	}

	file_count = (part->buf[offset] << 8) | part->buf[offset + 1];
	offset += 2;
	while (file_count && offset < part->info.size) {
		file_len = (part->buf[offset] << 8) | part->buf[offset + 1];
		offset += 2;

		for (;part->buf[offset] && offset < part->info.size; offset++)
			;
		offset += file_len + 1;
		file_count--;
	}

	if (file_count || offset > REBOOT_FLAG_OFFSET) {
		PRINT_ERROR_NE("Config files saved in reboot flag, do not overwrite");
		return -1;
	}
	return 0;
}

static int write_reboot_flag(struct flash_part *part)
{
	int offset;
	int r;
	uint8_t buf[16];
	int i;

	offset = lseek(part->fd_char, REBOOT_FLAG_OFFSET, SEEK_SET);
	if (offset != REBOOT_FLAG_OFFSET) {
		PRINT_ERROR("Cannot seek to reboot flag");
		return -1;
	}

	if (fopts.test_mode) {
		PRINT_INFO("Not writing reboot flag to offset 0x%x in test mode", offset);
	} else {
		memset(buf, 0, 16);
		r = write(part->fd_char, buf, 16);
		if (r != 16) {
			PRINT_ERROR("Cannot write reboot flag");
			return -1;
		}
	}

	offset = lseek(part->fd_char, REBOOT_FLAG_OFFSET, SEEK_SET);
	if (offset != REBOOT_FLAG_OFFSET) {
		PRINT_ERROR("Cannot seek to reboot flag");
		return -1;
	}

	r = read(part->fd_char, buf, 16);
	if (r != 16) {
		PRINT_ERROR("Cannot reread reboot flag");
		return -1;
	}

	for (i = 0; i < 16 && buf[i] == 0 ; i++)
		;

	if (i != 16) {
		PRINT_ERROR_NE("Reboot flag  not correct written");
		return -1;
	}
	return 0;
}

int flash_set_rescue_reboot(struct flash *flash)
{
	if (check_conf_files(&flash->conf)) {
		return -1;
	}

	if (write_reboot_flag(&flash->conf)) {
		return -1;
	}
	return 0;
}
