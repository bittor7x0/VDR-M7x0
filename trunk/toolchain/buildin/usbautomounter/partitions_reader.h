/*
 * Copyright (C) 2007 Andreas Koch - the open7x0.org group
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 * $Id: partitions_reader.h 201 2007-09-25 17:42:55Z andreas $
 *
 */
#ifndef __PARTITIONS_READER_H
#define __PARTITIONS_READER_H 1
#include <stdint.h>
#include "setup.h"
#include "mount.h"
#define PART_FS_UNCLEAN 1
#define PART_FS_HAS_ERROR 2
#define PART_FS_MOUNTED_UNCLEAN 4
#define PART_FS_MOUNT_INTERVAL 8
#define PART_FS_CLEAN_UNKNOWN 16

struct partition {
	char *devname;
	uint8_t type;
	char *fstypename;
	char *fsid;
	char *mountpoint;
	char *label;
	int flags;
	int remount;
	int dev_mounted;
	struct mount_table_entry *tab_entry;
	struct mounted *mounted;
	struct partition *prev;
	struct partition *next;
};

struct partition_list {
	struct partition *first;
	struct partition *last;
};

void delete_partition(struct partition_list *parts, struct partition *part);
int read_partitions(char *devname, struct partition_list *parts);
#endif
