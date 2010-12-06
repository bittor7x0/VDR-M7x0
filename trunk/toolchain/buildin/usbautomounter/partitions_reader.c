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
 * $Id: partitions_reader.c 161 2007-09-12 06:48:56Z andreas $
 *
 */
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "partitions_reader.h"
#include "tools.h"

#define EXTENDED_PARTITION_TYPE1 0x05
#define EXTENDED_PARTITION_TYPE2 0x0f
#define MAX_PARTITIONS_NO 15
#define PART_SECTOR_SIZE 512
#define IS_EXT_PART(type) ((type) == EXTENDED_PARTITION_TYPE1 || \
									(type) == EXTENDED_PARTITION_TYPE2)

struct partition_table_entry {
	uint8_t boot_flag;
	uint8_t start_chs[3];
	uint8_t type;
	uint8_t end_chs[3];
	uint32_t start_sector;
	uint32_t sector_count;
} __attribute__ ((packed));

struct boot_record {
	uint8_t boot_code[446];
	struct partition_table_entry entries[4];
	uint8_t signature[2]; // Always 0x55 0xAA
} __attribute__ ((packed));

static void append_partition(struct partition_list *parts, char *devprefix,
		int partno, int type)
{
	struct partition *new_part;
	if (partno > MAX_PARTITIONS_NO) {
		SYSLOG_WARN("Partition number %d exceeds maximum %d", partno,
				MAX_PARTITIONS_NO);
		return;
	}

	new_part = pmalloc(sizeof(struct partition));
	memset(new_part, 0, sizeof(struct partition));

	if (asprintf(&new_part->devname,"%s%d",devprefix,partno) == -1) {
		DIE("Cannot generate partition device name");
	}

	new_part->type = type;
	if (parts->last) {
		parts->last->next = new_part;
		new_part->prev = parts->last;
		parts->last = new_part;
	} else {
		parts->first = new_part;
		parts->last = new_part;
	}
}

void delete_partition(struct partition_list *parts, struct partition *part)
{
	free(part->devname);
	free(part->fstypename);
	free(part->fsid);
	free(part->mountpoint);
	free(part->label);

	if (part->prev) {
		part->prev->next = part->next;
	} else {
		parts->first = part->next;
	}

	if (part->next) {
		part->next->prev = part->prev;
	}	else {
		parts->last = part->prev;
	}
	free(part);
}

static int read_boot_record(int fd, off_t offset, struct boot_record *brec)
{
	int i;
	if (lseek(fd, offset, SEEK_SET) != offset) {
		SYSLOG_ERR("Cannot seek on device");
		return -1;
	}

	if (read(fd, brec, sizeof(*brec)) != sizeof(*brec)) {
		SYSLOG_ERR("Cannot read boot record");
		return -1;
	}

	if (brec->signature[0] != 0x55 || brec->signature[1] != 0xAA) {
		SYSLOG_ERR_NE("Invalid boot record; wrong signature: 0x%hhx 0x%hhx",
				brec->signature[0], brec->signature[1]);
		return -2;
	}
	for (i = 0; i < 4; i++) {
		brec->entries[i].start_sector = LE2HOST32(brec->entries[i].start_sector);
		brec->entries[i].sector_count = LE2HOST32(brec->entries[i].sector_count);
	}
	return 0;
}

static int read_ext_part(int fd, char *dev, struct partition_list *parts,
		 int *part_no, uint32_t first_start_sec)
{
	struct boot_record brec;
	int r;
	uint32_t start_sec;
	uint32_t last_sec;
	start_sec = first_start_sec;

	do {
		if ((r = read_boot_record(fd, (off_t)start_sec * PART_SECTOR_SIZE,
				 &brec))	< 0)
			return r;

		if (brec.entries[0].type && brec.entries[0].sector_count &&
				brec.entries[0].start_sector &&
				!IS_EXT_PART(brec.entries[0].type)) {
			append_partition(parts, dev, *part_no, brec.entries[0].type);
			(*part_no)++;
			last_sec = start_sec + brec.entries[0].start_sector +
					brec.entries[0].sector_count;
			}
		else {
			SYSLOG_WARN("Wrong logical partition type 0x%02hhX start 0x%04X "
					"size 0x%04X on device %s", brec.entries[0].type,
					brec.entries[0].start_sector,
					brec.entries[0].sector_count, dev);
			last_sec = start_sec + 1;
		}

		start_sec = first_start_sec + brec.entries[1].start_sector;
	} while(IS_EXT_PART(brec.entries[1].type) && brec.entries[1].start_sector
			&& brec.entries[1].sector_count && last_sec <= start_sec);

	if (IS_EXT_PART(brec.entries[1].type) && brec.entries[1].start_sector
			&& brec.entries[1].sector_count && last_sec > start_sec) {
		SYSLOG_WARN("EBR start 0x%04X in logical partion (last sector 0x%04X) "
				"on device %s", start_sec, last_sec, dev);
	}

	return 0;
}

int read_partitions(char *devname, struct partition_list *parts)
{
	struct boot_record brec;
	int fd;
	int i;
	int r;
	int part_no;

	fd = open(devname, O_RDONLY);
	if (fd < 0) {
		SYSLOG_ERR("Cannot open device %s", devname);
		return -1;
	}

	if ((r = read_boot_record(fd, 0, &brec)) < 0) {
		if (close(fd))
			SYSLOG_ERR("Cannot close device %s", devname);
		return r;
	}

	for (i = 0; i < 4; i++) {
		if (brec.entries[i].type && brec.entries[i].sector_count &&
				brec.entries[i].start_sector &&
				!IS_EXT_PART(brec.entries[i].type)) {
		append_partition(parts, devname, i + 1, brec.entries[i].type);
		}
	}

	part_no = 5;

	for (i = 0; i < 4; i++) {
		if (IS_EXT_PART(brec.entries[i].type) &&
				brec.entries[i].sector_count && brec.entries[i].start_sector) {
			read_ext_part(fd, devname, parts, &part_no,
					brec.entries[i].start_sector);
		}
	}

   if (close(fd))
		SYSLOG_ERR("Cannot close device %s", devname);
	return 0;
}
