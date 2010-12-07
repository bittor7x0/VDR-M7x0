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
 * $Id: fs_id_reader.c 134 2007-07-16 15:39:47Z andreas $
 *
 */

#include <stdint.h>
#include <string.h>
#include "fs_id_reader.h"
#include "fs_id_fat_reader.h"
#include "fs_id_ext2_3_reader.h"
#include "tools.h"

struct fs_id_reader {
	const uint8_t *partition_types;
	int (*reader)(struct partition *, int);
};

#define FS_ID_READER_COUNT 2
const struct fs_id_reader fs_id_readers[FS_ID_READER_COUNT] = {
	{ fs_id_fat_types,		fs_id_fat_reader		},
	{ fs_id_ext2_3_types,	fs_id_ext2_3_reader	}
};

static int read_fs_id(struct partition *part)
{
	if(part->type==130) /*usbautomaounter don't touch swap partitions*/
		return 1;
	int i;
	int r;

	for (i = 0 ; i < FS_ID_READER_COUNT; i++) {
		if (strchr(fs_id_readers[i].partition_types, part->type)) {
			r = fs_id_readers[i].reader(part, 0);
			if (r <= 0)
				return r;
		}
	}

	SYSLOG_WARN("Type 0x%hhx of partition for device '%s' unknown or wrong. Probing ... ",
		part->type, part->devname);

	for (i = 0 ; i < FS_ID_READER_COUNT; i++) {
		if (!strchr(fs_id_readers[i].partition_types, part->type)) {
			r = fs_id_readers[i].reader(part, 1);
			if (r <= 0)
				return r;
		}
	}

	SYSLOG_WARN("Cannot determinate fs type for device '%s'.", part->devname);
	return 1;
}

void read_all_fs_ids(struct partition_list *parts)
{
	struct partition *part;
	part = parts->first;
	char *tmp;
	while (part) {
		struct partition *part_cur = part;
		part = part->next;
		if (read_fs_id(part_cur)) {
			delete_partition(parts, part_cur);
		} else if (part_cur->label) {
			tmp = part_cur->label;
			while ((tmp = strchr(tmp, '/'))) {
				*tmp = '_';
				tmp++;
			}
		}
	}
}
