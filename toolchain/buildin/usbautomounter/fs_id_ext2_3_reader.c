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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "tools.h"
#include "fs_id_ext2_3_reader.h"

#define EXT2_3_SUPER_BLOCK_OFFSET 1024
#define EXT2_3_MAGIC 0xEF53
#define EXT2_3_COMPAT_FEATURE_JOURNAL 0x0004

#define EXT2_3_CLEAN_UNMOUNTED 1
#define EXT2_3_FS_HAS_ERRORS 2

#define UUID_PRINT_FORMAT_STR "%08x-%04x-%04x-%04x-%08x%04x"

struct ext2_3_super_block {
	uint32_t inode_count;
	uint32_t block_count;
	uint32_t reserved_blocks;
	uint32_t free_blocks;

	uint32_t free_inodes;
	uint32_t data_start;
	uint32_t block_size;
	int32_t fragment_size;

	uint32_t blocks_per_group;
	uint32_t fragments_per_group;
	uint32_t inodes_per_group;
	uint32_t mount_time;

	uint32_t write_time;
	uint16_t mount_count;
	int16_t max_mount_count;
	uint16_t fs_magic;
	uint16_t fs_state_flags;
	uint16_t error_behave;
	uint16_t revision_minor;

	uint32_t last_check;
	uint32_t check_interval;
	uint32_t os;
	uint32_t revision;

	uint16_t reversed_block_uid;
	uint16_t reversed_block_gid;
	uint32_t frist_inode;
	uint16_t inode_size;
	uint16_t group_no;
	uint32_t feature_compat;

	uint32_t feature_incompat;
	uint32_t feature_read_only_compat;
	uint8_t uuid[16];
	/* In real this field is 16 chars long.
	 * but nothing of the following data is need.
	 * for copying we can simply set volume_label[16] = 0 this way */
	char volume_label[17];

/* Rest is unneed for us as many of the fields above*/
};

static int ext2_3_read_super_block(int fd,
		struct ext2_3_super_block *ext2_3_sb, int probe)
{
	if (lseek(fd, EXT2_3_SUPER_BLOCK_OFFSET, SEEK_SET) != EXT2_3_SUPER_BLOCK_OFFSET) {
		SYSLOG_ERR("Cannot seek to ext2/3 super block");
		return -1;
	}

	if (read(fd, ext2_3_sb, sizeof(*ext2_3_sb)) != sizeof(*ext2_3_sb)) {
		SYSLOG_ERR("Cannot read ext2/3 super block");
		return -1;
	}

	ext2_3_sb->fs_magic = LE2HOST16(ext2_3_sb->fs_magic);
	if (ext2_3_sb->fs_magic != EXT2_3_MAGIC) {
		if (!probe)
			SYSLOG_WARN("Cannot find ext2/3 magic. Really a ext2/3 volume?");
		return 1;
	}

	/* only correct endian for need fields */
	ext2_3_sb->inode_count = LE2HOST32(ext2_3_sb->inode_count);
	ext2_3_sb->block_count = LE2HOST32(ext2_3_sb->block_count);
	ext2_3_sb->block_size = LE2HOST32(ext2_3_sb->block_size);
	ext2_3_sb->mount_count = LE2HOST16(ext2_3_sb->mount_count);
	ext2_3_sb->max_mount_count = LE2HOST16(ext2_3_sb->max_mount_count);
	ext2_3_sb->fs_state_flags = LE2HOST16(ext2_3_sb->fs_state_flags);
	ext2_3_sb->last_check = LE2HOST32(ext2_3_sb->last_check);
	ext2_3_sb->check_interval = LE2HOST32(ext2_3_sb->check_interval);
	ext2_3_sb->feature_compat = LE2HOST32(ext2_3_sb->feature_compat);
	return 0;
}

int fs_id_ext2_3_reader(struct partition *part, int probe)
{
	struct ext2_3_super_block ext2_3_sb;
	int fd;
	int r;
	time_t next_check_time;
	time_t now;
	uint32_t uuid_p1;
	uint16_t uuid_p2;
	uint16_t uuid_p3;
	uint16_t uuid_p4;
	uint32_t uuid_p5;
	uint16_t uuid_p6;

	fd = open(part->devname, O_RDONLY);
	if (fd < 0) {
		SYSLOG_ERR("Cannot open device '%s'", part->devname);
		return -1;
	}

	r = ext2_3_read_super_block(fd, &ext2_3_sb, probe);
	if (r) {
		if (close(fd))
			SYSLOG_ERR("Cannot close device '%s'", part->devname);
		return r;
	}

	if (ext2_3_sb.feature_compat & EXT2_3_COMPAT_FEATURE_JOURNAL) {
		part->fstypename = strdup("ext3");
	} else {
		part->fstypename = strdup("ext2");
	}

	SYSLOG_INFO("%s partition '%s' with %u inodes and %u blocks of "
		"size %u found.", part->fstypename, part->devname,
		ext2_3_sb.inode_count, ext2_3_sb.block_count,
		1 << (ext2_3_sb.block_size + 10));

	if (!part->fstypename) {
		DIE("Cannot alloc memory for fs type ext2/3");
	}

	uuid_p1 = (ext2_3_sb.uuid[0] << 24) | (ext2_3_sb.uuid[1] << 16) |
			(ext2_3_sb.uuid[2] << 8) | ext2_3_sb.uuid[3];
	uuid_p2 = (ext2_3_sb.uuid[4] << 8) | ext2_3_sb.uuid[5];
	uuid_p3 = (ext2_3_sb.uuid[6] << 8) | ext2_3_sb.uuid[7];
	uuid_p4 = (ext2_3_sb.uuid[8] << 8) | ext2_3_sb.uuid[9];
	uuid_p5 = (ext2_3_sb.uuid[10] << 24) | (ext2_3_sb.uuid[11] << 16) |
			(ext2_3_sb.uuid[12] << 8) | ext2_3_sb.uuid[13];
	uuid_p6 = (ext2_3_sb.uuid[14] << 8) | ext2_3_sb.uuid[15];
	if (asprintf(&part->fsid, UUID_PRINT_FORMAT_STR, uuid_p1, uuid_p2,
			uuid_p3, uuid_p4, uuid_p5, uuid_p6) == -1) {
		DIE("Cannot alloc memory for uuid");
	}

	ext2_3_sb.volume_label[16] = 0;
	part->label = NULL;
	if (ext2_3_sb.volume_label[0]) {
		part->label = strdup(ext2_3_sb.volume_label);
		if (!part->label)
			DIE("Cannot alloc memory ext2/3 volume label");
	}

	part->flags = 0;
	if (!(ext2_3_sb.fs_state_flags & EXT2_3_CLEAN_UNMOUNTED)) {
		part->flags |= PART_FS_UNCLEAN;
	}
	if (ext2_3_sb.fs_state_flags & EXT2_3_FS_HAS_ERRORS) {
		part->flags |= PART_FS_HAS_ERROR | PART_FS_UNCLEAN;
	}

	next_check_time = ext2_3_sb.last_check + ext2_3_sb.check_interval;
	now = time(NULL);
	if (ext2_3_sb.check_interval && next_check_time < now) {
		SYSLOG_INFO("device '%s' ext2/3 mount time interval reached "
				"check forced", part->devname);
		part->flags |= PART_FS_MOUNT_INTERVAL;
	}
	if (ext2_3_sb.max_mount_count > 0 && ext2_3_sb.max_mount_count
			<= ext2_3_sb.mount_count) {
		SYSLOG_INFO("device '%s' ext2/3 max mount count reached "
				"check forced", part->devname);
		part->flags |= PART_FS_MOUNT_INTERVAL;
	}

	if (close(fd))
		SYSLOG_ERR("Cannot close device '%s'", part->devname);
	return 0;
}


