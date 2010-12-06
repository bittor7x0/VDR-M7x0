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
 * $Id: o7o_flasher.c 362 2008-08-17 17:14:52Z andreas $
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "display.h"
#include "image.h"
#include "flash.h"
#include "tools.h"
#include "o7o_flasher.h"
#define DEV_PIC "/dev/pic"
#define PIC_REBOOT 0x07
#define M740_ROOT_SIZE (6 * 1024 * 1024)
#define M750S_ROOT_SIZE (8 * 1024 * 1024)

struct pic_param {
	int count;
	int param[10];
};

struct flasher {
	struct flash flash;
	struct image image;
};

#define FLASHER_INIT_STEP 1
#define FLASHER_READ_STEP 2
#define FLASHER_WRITE_STEP 3
#define FLASHER_VERIFY_STEP 4

static int flashing_step = 0;
static struct flasher *flasher = NULL;
struct o7o_flasher_options fopts;

static int parse_options(int argc, char **argv)
{
	int c;
	fopts.test_mode = 0;
	fopts.no_reboot = 0;
	struct option opts[] = {
		{"test-mode", 0, NULL, 't'},
		{"no-reboot", 0, NULL, 'n'},
		{NULL, 0, NULL, 0}
	};

	while((c = getopt_long(argc, argv, "tn", opts, NULL)) != -1) {
		switch (c) {
			case 't':
				fopts.test_mode = 1;
				break;
			case 'n':
				fopts.no_reboot = 1;
				break;
			default:
				PRINT_ERROR_NE("Illegal option");
				return -1;
		}
	}
	return 0;
}

static void do_reboot()
{
	int pic_fd;
	int try;
	struct pic_param par;
	for (try = 0; try < 10; try++) {
		pic_fd = open(DEV_PIC, O_RDWR);
		if (pic_fd < 0) {
			PRINT_ERROR("Cannot open pic device");
			break;
		}
		memset(&par, 0, sizeof(par));
		par.count = 2;
		par.param[0] = 2;
		par.param[1] = 3;
		display_msg(tr("Rebooting ..."));
		if (ioctl(pic_fd, PIC_REBOOT, &par)) {
			PRINT_ERROR("pic ioctl failed");
		}
		close(pic_fd);
		sleep(5);
	}
	PRINT_ERROR_NE("Pic does not reboot");
	display_deinit();
	exit(1);
}

static void error_out(const char *msg)
{
	if (!fopts.no_reboot && flashing_step >= FLASHER_WRITE_STEP) {
		flash_set_rescue_reboot(&flasher->flash);
	}

	if (flashing_step >= FLASHER_READ_STEP) {
		close_image(&flasher->image);
	}

	if (flashing_step) {
		flash_close(&flasher->flash);
	}

	if (flasher) {
		mmap_free(flasher, sizeof(*flasher));
	}

	display_msg(msg);
	sleep(2);
	if (fopts.no_reboot) {
		display_deinit();
		exit(1);
	}

	do_reboot();
}

static void check_init(void)
{
	if (flasher->image.system_type == SYSTEM_TYPE_UNKNOWN ||
			(flasher->image.system_type == SYSTEM_TYPE_M740 &&
			flasher->flash.rootfs.info.size != M740_ROOT_SIZE) ||
			(flasher->image.system_type == SYSTEM_TYPE_M750S &&
			flasher->flash.rootfs.info.size != M750S_ROOT_SIZE)) {
		error_out(tr("Image is not for this system"));
	}

	if (flasher->image.rootfs.size > flasher->flash.rootfs.info.size ||
			flasher->image.jffs.size > flasher->flash.jffs.info.size ||
			flasher->image.kernel.size > 12 * 64 * 1024) {
		error_out(tr("Image exceeds flash size"));
	}

	if (display_set_bar_max(DISPLAY_KERNEL_BAR, 12 * 64 * 1024)) {
		error_out(tr("Initialization error"));
	}

	if (display_set_bar_max(DISPLAY_JFFS_BAR, flasher->flash.jffs.info.size)) {
		error_out(tr("Initialization error"));
	}

	if (display_set_bar_max(DISPLAY_ROOTFS_BAR, flasher->flash.rootfs.info.size)) {
		error_out(tr("Initialization error"));
	}

	if (flasher->image.kernel.size) {
		if (display_set_bar_free(DISPLAY_KERNEL_BAR, flasher->image.kernel.size)) {
			error_out(tr("Initialization error"));
		}
	}

	if (flasher->image.jffs.size) {
		if (display_set_bar_free(DISPLAY_JFFS_BAR, flasher->image.jffs.size)) {
			error_out(tr("Initialization error"));
		}
	}

	if (flasher->image.rootfs.size) {
		if (display_set_bar_free(DISPLAY_ROOTFS_BAR, flasher->image.rootfs.size)) {
			error_out(tr("Initialization error"));
		}
	}
}

int main(int argc, char **argv)
{
	int fd;
	int r;

	if (parse_options(argc, argv)) {
		return 1;
	}

	if (display_init()) {
		return 1;
	}

	flasher = mmap_alloc(sizeof(*flasher));
	memset(flasher,0,sizeof(*flasher));
	if (!flasher) {
		error_out(tr("Initialization error"));
	}

	fd = open(argv[0], O_RDONLY);
	if (fd < 0) {
		error_out(tr("Initialization error"));
	}

	if (read_image_footer(fd, &flasher->image)) {
		close(fd);
		error_out(tr("Error reading firmware image"));
	}

	r = check_flasher(fd, &flasher->image);
	if (r < 0) {
		close(fd);
		error_out(tr("Error reading firmware image"));
	}

	if (r == 0) {
		close(fd);
		error_out(tr("Image checksum wrong"));
	}

	if (flash_open(&flasher->flash)) {
		close(fd);
		error_out(tr("Initialization error"));
	}

	flashing_step = FLASHER_INIT_STEP;
	check_init();

	display_msg(tr("Reading Firmware Image ..."));
	r = read_image(fd,&flasher->image, flasher->flash.conf.info.erasesize);
	close(fd);

	if (r < 0) {
		error_out(tr("Error reading firmware image"));
	}

	if (r == 0) {
		error_out(tr("Image checksum wrong"));
	}

	flashing_step = FLASHER_READ_STEP;
	if (protect_mem(flasher, sizeof(*flasher))) {
		PRINT_ERROR("Cannot protect flasher struct");
		error_out(tr("Error reading firmware image"));
	}

	flashing_step = FLASHER_WRITE_STEP;
	display_msg(tr("Writing Firmware Image ..."));
	if (flash_write_image(&flasher->flash, &flasher->image)) {
		error_out(tr("Error writing firmware image"));
	}

	flashing_step = FLASHER_VERIFY_STEP;
	display_msg(tr("Verifying Written Firmware Image..."));
	if (flash_verify_image(&flasher->flash, &flasher->image)) {
		error_out(tr("Firmware not correctly written"));
	}
	display_msg(tr("Update successfully completed!"));

	close_image(&flasher->image);
	flash_close(&flasher->flash);
	mmap_free(flasher, sizeof(*flasher));
	if (fopts.no_reboot) {
		sleep(2);
		display_deinit();
		return 0;
	}
	do_reboot();
	return 1;
}
