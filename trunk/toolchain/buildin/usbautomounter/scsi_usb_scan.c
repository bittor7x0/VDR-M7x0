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
 * $Id: scsi_usb_scan.c 133 2007-07-16 13:52:26Z andreas $
 *
 */

#include <scsi/scsi.h>
#include <scsi/scsi_ioctl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "scsi_usb_scan.h"
#include "tools.h"

#define MAX_SCSI_DISKS 128
#define SCSI_DISK_DEV_MAX_LENGTH 10


union scsi_probe_host {
  unsigned int length;
  char name[1024];
};

static void scsi_disk_devname(int no, char devname[SCSI_DISK_DEV_MAX_LENGTH])
{
	memcpy(devname, SCSI_DISK_DEV_PREFIX, SCSI_DISK_DEV_PREFIX_LEN);

	if (no < 26) {
		devname[SCSI_DISK_DEV_PREFIX_LEN] = 'a' + no;
		devname[SCSI_DISK_DEV_PREFIX_LEN + 1] = 0;
	} else {
		devname[SCSI_DISK_DEV_PREFIX_LEN] = 'a' + (no / 26);
		devname[SCSI_DISK_DEV_PREFIX_LEN + 1] = 'a' + (no % 26);
		devname[SCSI_DISK_DEV_PREFIX_LEN + 2] = 0;
	}
}

static int check_for_scsi_usb_disk(char *devname)
{
	union scsi_probe_host host;
	int sd_fd;
	int r;

	sd_fd = open(devname, O_RDONLY | O_LARGEFILE);

	if (sd_fd < 0) {
		if (errno != ENODEV && errno != ENOENT && errno != ENXIO) {
			SYSLOG_ERR("Unexpected error while open device '%s'", devname);
			return -1;
		}
		return -1;
	}

	r = ioctl(sd_fd, SCSI_IOCTL_TEST_UNIT_READY);
	if (r < 0) {
		SYSLOG_ERR("Unexpected error while testing device '%s' ready", devname);
		if (close(sd_fd)) {
			SYSLOG_ERR("Cannot close device '%s'", devname);
		}
		return -1;
	}

	if (r) {
		if (close(sd_fd)) {
			SYSLOG_ERR("Cannot close device '%s'", devname);
		}
		return 0;
	}

	host.length = 1024;
	r = ioctl(sd_fd, SCSI_IOCTL_PROBE_HOST,&host);
	if (r < 0) {
		SYSLOG_ERR("Unexpected error while testing device '%s' is usb", devname);
		if (close(sd_fd)) {
			SYSLOG_ERR("Cannot close device '%s'", devname);
		}
		return -1;
	}

	if (close(sd_fd)) {
		SYSLOG_ERR("Cannot close device '%s'", devname);
	}

	host.name[1023] = 0;

	if (!strcasestr(host.name,"USB"))
		return 0;
	else
		return 1;
}

int get_all_scsi_usb_disks(char ***devs)
{
	int no;
	int avail;
	char tmp[SCSI_DISK_DEV_MAX_LENGTH];

	avail = 0;
	*devs = NULL;
	for (no = 0; no < MAX_SCSI_DISKS; no++) {
		scsi_disk_devname(no, tmp);
		if (check_for_scsi_usb_disk(tmp) > 0) {
			char **tmp2;
			avail++;
			tmp2 = realloc(*devs, sizeof(char *) * avail);
			if (!tmp2)
				DIE("Cannot alloc memory");
			*devs = tmp2;
			(*devs)[avail - 1] = pmalloc(SCSI_DISK_DEV_MAX_LENGTH);
			memcpy((*devs)[avail - 1], tmp, SCSI_DISK_DEV_MAX_LENGTH);
		}
	}
	return avail;
}

