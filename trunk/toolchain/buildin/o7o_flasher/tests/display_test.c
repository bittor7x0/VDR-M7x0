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
 * $Id: display_test.c 362 2008-08-17 17:14:52Z andreas $
 *
 */

#include <stdio.h>
#include <unistd.h>
#include "../display.h"

#define min(a,b) ((a) < (b) ? (a) : (b))

int main(int argc, char **argv)
{
	int i,j;
	display_init();
	display_set_bar_max(DISPLAY_KERNEL_BAR, 768 * 1024);
	display_set_bar_max(DISPLAY_JFFS_BAR, 5888 * 1024);
	display_set_bar_max(DISPLAY_ROOTFS_BAR, 6144 * 1024);

	display_set_bar_free(DISPLAY_KERNEL_BAR, 718534);
	display_set_bar_free(DISPLAY_JFFS_BAR, 5888 * 1024);
	display_set_bar_free(DISPLAY_ROOTFS_BAR, 4187945);


	for (j = DISPLAY_BAR_READ; j <= DISPLAY_BAR_VERIFIED; j++) {
		switch (j) {
			case DISPLAY_BAR_READ:
				display_msg(tr("Reading Firmware Image ..."));
				break;
			case DISPLAY_BAR_WRITTEN:
				display_msg(tr("Writing Firmware Image ..."));
				break;
			case DISPLAY_BAR_VERIFIED:
				display_msg(tr("Verifying Written Firmware Image..."));
				break;
		}
		for (i = 0; i < 718534;) {
			i += 64 * 1024;
			usleep(100000);
			display_update_bar(DISPLAY_KERNEL_BAR, j, min (718534, i));
		}

		for (i = 0; i < 5888 * 1024;) {
			i += 64 * 1024;
			usleep(100000);
			display_update_bar(DISPLAY_JFFS_BAR, j, min (5888 * 1024, i));
		}

		for (i = 0; i < 4187945;) {
			i += 64 * 1024;
			usleep(100000);
			display_update_bar(DISPLAY_ROOTFS_BAR, j, min (4187945, i));
		}
	}

	display_msg(tr("Update successfully completed!"));
	sleep(10);
	display_msg(tr("Rebooting ..."));
	sleep(2);
	display_deinit();
	return 0;
}
