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
 * $Id: fs_id_fat_reader.c 196 2007-09-22 10:46:58Z andreas $
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <iconv.h>
#include <langinfo.h>
#include <string.h>

#include "tools.h"
#include "fs_id_fat_reader.h"

#define FAT_ATTR_VOLUME 0x08
#define FAT_ATTR_LFN 0x0F
#define FAT_LFN_START 0x40
#define FAT_LFN_SLOT_MASK 0x1f
#define FAT_DIR_ENTRY_IS_FREE(d) ((d).name[0] == '\xe5' || (d).name[0] == 0x00)

struct fat_boot_sector {
	uint8_t jump_inst[3];
	char generator_id[8];
	uint16_t sector_size;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fats;
	uint16_t root_entries;
	uint16_t sector_count;
	uint8_t media_type;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t heads;
	uint32_t hidden_sectors;
	uint32_t total_sectors;
	union {
		struct {
			uint8_t drive_no;
			uint8_t res;
			uint8_t sig;
			uint32_t fsid;
			char label[11];
			char fat_type[8];
		} __attribute__((packed)) fat1x;
		struct {
			uint32_t sectors_per_fat;
			uint16_t flags;
			uint16_t version;
			int32_t	root_cluster;
			uint16_t info_sector;
			uint16_t boot_copy;
			uint8_t res1[12];
			uint8_t drive_no;
			uint8_t res2;
			uint8_t sig;
			uint32_t fsid;
			char label[11];
			char fat_type[8];
		} __attribute__((packed)) fat32;
	} f;
} __attribute__((packed));

struct fat_dir_entry {
	char name[11];
	uint8_t attr;
	uint8_t res;
	uint8_t ctime_10ms;
	uint16_t ctime;
	uint16_t cdate;
	uint16_t adate;
	uint16_t start_hi;
	uint16_t mtime;
	uint16_t mdate;
	uint16_t start;
	uint32_t size;
} __attribute__((packed));

struct fat_lfn_entry {
	uint8_t slot_no;
	char name1[10];
	uint8_t attr;
	uint8_t res;
	uint8_t csum;
	char name2[12];
	uint16_t start;
	char name3[4];
} __attribute__((packed));


struct fat_fs {
	int fat_bits;
	uint32_t cluster_size;
	off_t fat_start;
	size_t fat_size;
	off_t root_start;
	int32_t root_entries;
	int32_t root_cluster;
	off_t data_start;
	int32_t clusters;
	uint32_t fsid;
	char *label;
	time_t label_mtime;
};

static int fat_read_boot_sector(int fd, struct fat_fs *fs, int probe)
{
	struct fat_boot_sector boot;
	const char *msg;
	uint32_t sector_size;
	uint32_t sector_count;
	uint32_t data_start;
	uint32_t data_size;
	int i;

	if (lseek(fd, 0, SEEK_SET) != 0) {
		SYSLOG_ERR("Cannot seek to fat boot sector");
		return -1;
	}

	if (read(fd, &boot, sizeof(boot)) != sizeof(boot)) {
		SYSLOG_ERR("Cannot read fat boot sector");
		return -1;
	}

	sector_size = LE2HOST16(boot.sector_size);
	if (!sector_size || (sector_size & (sector_size -1))) {
		msg = "logical sector size invalid";
		goto out_not_fat;
	}

	fs->cluster_size = boot.sectors_per_cluster;
	if (!fs->cluster_size || (fs->cluster_size & (fs->cluster_size -1))) {
		msg = "cluster size invalid";
		goto out_not_fat;
	}

	if (!boot.sectors_per_fat && !boot.f.fat32.sectors_per_fat) {
		msg = "invalid fat size";
		goto out_not_fat;
	}

	fs->root_entries = LE2HOST16(boot.root_entries);
	if ((fs->root_entries * sizeof(struct fat_dir_entry)) & (sector_size - 1)) {
		msg = "invalid number of root directory entries";
		goto out_not_fat;
	}

	sector_count = boot.sector_count ? LE2HOST16(boot.sector_count) :
			LE2HOST32(boot.total_sectors);

	fs->fat_start = LE2HOST16(boot.reserved_sectors);
	fs->fat_size = boot.sectors_per_fat ? LE2HOST16(boot.sectors_per_fat) :
			LE2HOST32(boot.f.fat32.sectors_per_fat);

	fs->root_start = fs->fat_size * boot.fats + fs->fat_start;

	data_start = fs->root_start +
			((fs->root_entries * sizeof(struct fat_dir_entry)) / sector_size);
	data_size = sector_count - data_start;
	fs->clusters = data_size / fs->cluster_size;

	fs->root_cluster = 0;
	fs->label_mtime = 0;
	fs->label = NULL;
	if (!boot.sectors_per_fat) {
		fs->root_cluster = LE2HOST32(boot.f.fat32.root_cluster);

		if(fs->root_cluster && fs->root_entries) {
			msg = "FAT32 has two root dirs defined";
			goto out_not_fat;
		}

		if (fs->root_entries) {
			SYSLOG_WARN("FAT32 root dir not in cluster chain");
		}

		fs->fat_bits = 32;
		fs->fsid = LE2HOST32(boot.f.fat32.fsid);

		for (i = 11; i > 0; i --) {
			if (boot.f.fat32.label[i - 1] != ' ' &&
					boot.f.fat32.label[i - 1]) {
				fs->label = pmalloc(i + 1);
				memcpy(fs->label, boot.f.fat32.label, i);
				fs->label[i] = 0;
				break;
			}
		}
	} else {
		if (!memcmp(boot.f.fat1x.fat_type,"FAT12",5)) {
			fs->fat_bits = 12;
		} else if (!memcmp(boot.f.fat1x.fat_type,"FAT16",5)) {
			fs->fat_bits = 16;
		} else {
			fs->fat_bits = fs->clusters > 4084 ? 16 : 12;
		}
		fs->fsid = LE2HOST32(boot.f.fat1x.fsid);

		for (i = 11; i > 0; i --) {
			if (boot.f.fat1x.label[i - 1] != ' ' &&
					boot.f.fat1x.label[i - 1]) {
				fs->label = pmalloc(i + 1);
				memcpy(fs->label, boot.f.fat1x.label, i);
				fs->label[i] = 0;
				break;
			}
		}

	}

	if (!fs->root_cluster && !fs->root_entries) {
		msg = "no root directory";
		goto out_not_fat;
	}

	fs->cluster_size *= sector_size;
	fs->fat_start *= sector_size;
	fs->fat_size *= sector_size;
	fs->root_start *= sector_size;
	fs->data_start = (off_t)data_start * sector_size;

	if (((fs->fat_size * 8) / fs->fat_bits) - 2 < fs->clusters) {
		msg = "fat size too small";
		goto out_not_fat;
	}

	return 0;
out_not_fat:
	if (!probe)
		SYSLOG_ERR_NE("Invalid fat boot sector (%s). Really a fat volume?", msg);
	return 1;
}

/* we need access fat only for fat32 as the root dir is in cluster chain
 * only in fat32.
 */
static int32_t fat32_get_next_cluster(struct fat_fs *fs,
		uint32_t *fat_mmap_buf, int32_t cluster)
{
	uint32_t raw_fat_ent;
	if (cluster >= fs->clusters + 2) {
		SYSLOG_WARN("Cluster %d exceeds limit %d", cluster, fs->clusters + 2);
		return -1;
	}

	if (fs->fat_bits != 32) {
		DIE("Internal error: fat32_get_next_cluster called on none fat32");
	}

	raw_fat_ent = fat_mmap_buf[cluster];
	raw_fat_ent = LE2HOST32(raw_fat_ent);
	raw_fat_ent = raw_fat_ent & ((1 << 28) - 1); // upper 4 bits are reserved

	if (raw_fat_ent == 0x0FFFFFF7) {
		SYSLOG_WARN("Bad cluster in chain");
	}

	return (raw_fat_ent < 0x0FFFFFF7 ? (int32_t) raw_fat_ent : -1);
}

static time_t fat_convtime(uint16_t date_val, uint16_t time_val)
{
	struct tm tm_rep;
	memset(&tm_rep, 0, sizeof(tm_rep));
	tm_rep.tm_sec = (time_val & 0x1f) * 2;
	tm_rep.tm_min = (time_val >> 5) & 0x3f;
	tm_rep.tm_hour = time_val >> 11;
	tm_rep.tm_mday = date_val & 0x1f;
	tm_rep.tm_mon = ((date_val >> 5) - 1) & 0x0f;
	tm_rep.tm_year = (date_val >> 9)  + 80;
	tm_rep.tm_isdst = -1;
	return mktime(&tm_rep);
}

struct fat_lfn_context {
	int slots_left;
	char *unicode;
	uint8_t csum;
};

static void fat_decode_lfn(struct fat_lfn_context *context,
		struct fat_dir_entry *dent)
{
	const char *msg;
	struct fat_lfn_entry *lfn;
	int slot;
	int offset;

	lfn = (struct fat_lfn_entry *) dent;
	slot = lfn->slot_no & FAT_LFN_SLOT_MASK;

	if (lfn->slot_no & FAT_LFN_START) {
		const int size = slot * 26;

		if (context->slots_left != -1)
			SYSLOG_WARN("FAT-LFN started within another");

		context->slots_left = slot;
		free(context->unicode);
		context->unicode = pmalloc(size + 2);
		context->unicode[size] = 0;
		context->unicode[size + 1] = 0;
		context->csum = lfn->csum;
	}

	if (slot != context->slots_left) {
		msg = "FAT-LFN entry out of sequence";
		goto out_free_context;
	}

	if (lfn->csum != context->csum) {
		msg = "FAT-LFN checksum mismatch";
		goto out_free_context;
	}

	slot--;
	context->slots_left = slot;
	offset = slot * 26;

	memcpy(context->unicode + offset, lfn->name1, 10);
	memcpy(context->unicode + offset + 10, lfn->name2, 12);
	memcpy(context->unicode + offset + 22, lfn->name3, 4);

	return;
out_free_context:
	SYSLOG_WARN(msg);
	context->slots_left = -1;
	free(context->unicode);
	context->unicode = NULL;
	context->csum = 0;
}

static char *fat_get_lfn(struct fat_lfn_context *context,
		struct fat_dir_entry *dent)
{
	int size;
	char *long_name;
	uint8_t csum;
	iconv_t utf16_2_local;
	char *ibuf;
	char *obuf;
	size_t ibytes;
	size_t obytes;
	int i;

	if (context->slots_left == -1) {
		return NULL;
	}

	long_name = NULL;
	if (context->slots_left != 0) {
		SYSLOG_WARN("FAT-LFN unfinished");
		goto out_free_context;
	}

	csum = 0;
	for (i = 0; i < 11; i++)
		csum = (((csum & 1) << 7) | ((csum & 0xfe) >> 1)) +
				(uint8_t)dent->name[i];

	if (context->csum != csum) {
		SYSLOG_WARN("FAT-LFN checksum wrong");
		goto out_free_context;
	}

	utf16_2_local = iconv_open(nl_langinfo(CODESET),"UTF-16LE");
	if (utf16_2_local == (iconv_t)(-1)) {
		SYSLOG_ERR("FAT-LFN: Cannot init iconv");
		goto out_free_context;
	}

	for (size = 0; context->unicode[size] || context->unicode[size + 1];
			size +=2)
		;

	size += 2;
	long_name = pmalloc(size / 2 + 1);
	long_name[size / 2] = 0;
	ibytes = size;
	obytes = size / 2;
	ibuf = context->unicode;
	obuf = long_name;

	if (iconv(utf16_2_local, &ibuf, &ibytes, &obuf, &obytes) == -1) {
		SYSLOG_ERR("FAT-LFN: Cannot convert lfn");
		free(long_name);
		long_name = NULL;
	}
	iconv_close(utf16_2_local);

out_free_context:
	context->slots_left = -1;
	free(context->unicode);
	context->unicode = NULL;
	context->csum = 0;
	return long_name;
}

static void fat_lfn_reset(struct fat_lfn_context *context, int silent)
{
	if (context->slots_left != -1) {
		if (!silent)
			SYSLOG_WARN("FAT-LFN unused");

		context->slots_left = -1;
		free(context->unicode);
		context->unicode = NULL;
		context->csum = 0;
	}
}
static int fat_read_dir_entry(int fd, off_t offset,
		struct fat_lfn_context *lfn_c, struct fat_dir_entry *dent)
{
	if (lseek(fd, offset, SEEK_SET) != offset) {
		SYSLOG_ERR("Cannot seek for directory entry");
		return -1;
	}

	if (read(fd, dent, sizeof(*dent)) != sizeof(*dent)) {
		SYSLOG_ERR("Cannot read directory entry");
		return -1;
	}

	if (FAT_DIR_ENTRY_IS_FREE(*dent)) {
		fat_lfn_reset(lfn_c, 0);
		return 0;
	}

	if (dent->attr == FAT_ATTR_LFN) {
		fat_decode_lfn(lfn_c, dent);
		return 0;
	}

	return 1;
}

static void fat_check_dir_entry_label(struct fat_fs *fs,
	 	struct fat_lfn_context *lfn_c, struct fat_dir_entry *dent)
{
	time_t label_mtime;
	char *long_name;
	int i;

	if (!(dent->attr & FAT_ATTR_VOLUME)) {
		fat_lfn_reset(lfn_c, 1);
		return;
	}

	label_mtime = fat_convtime(LE2HOST16(dent->mdate), LE2HOST16(dent->mtime));
	if (label_mtime < fs->label_mtime) {
		fat_lfn_reset(lfn_c, 1);
		return;
	}

	long_name = fat_get_lfn(lfn_c, dent);
	if (!long_name) {
		for (i = 11; i > 0; i --) {
			if (dent->name[i - 1] != ' ' && dent->name[i - 1]) {
				long_name = pmalloc(i + 1);
				memcpy(long_name, dent->name, i);
				long_name[i] = 0;
				break;
			}
		}
	}
	if (long_name) {
		fs->label_mtime = label_mtime;
		free(fs->label);
		fs->label = long_name;
	}
}

static int fat_scan_root_for_label(int fd, struct fat_fs *fs)
{
	struct fat_lfn_context lfn_c;
	struct fat_dir_entry dir_ent;
	lfn_c.slots_left = -1;
	lfn_c.unicode = NULL;
	lfn_c.csum = 0;

	if (fs->root_cluster) {
		uint32_t *fat_mmap_buf;
		int32_t cluster;
		fat_mmap_buf = mmap(NULL, fs->fat_size, PROT_READ, MAP_SHARED,
				fd, fs->fat_start);
		if (fat_mmap_buf == MAP_FAILED) {
			SYSLOG_ERR("Cannot mmap fat");
			return -1;
		}

		cluster = fs->root_cluster;
		while (cluster != -1) {
			off_t offset;
			int32_t data_left;

			offset = fs->data_start + ((off_t)cluster - 2) * fs->cluster_size;
			data_left = fs->cluster_size;
			while(data_left > 0) {
				int r = fat_read_dir_entry(fd, offset, &lfn_c, &dir_ent);
				if (r < 0) {
					munmap(fat_mmap_buf, fs->fat_size);
					fat_lfn_reset(&lfn_c, 1);
					return r;
				}

				if (r) {
					fat_check_dir_entry_label(fs, &lfn_c, &dir_ent);
				}
				offset += sizeof(struct fat_dir_entry);
				data_left -= sizeof(struct fat_dir_entry);
			}
			cluster = fat32_get_next_cluster(fs, fat_mmap_buf, cluster);
		}
		munmap(fat_mmap_buf, fs->fat_size);
	} else {
		int i;
		off_t offset;

		offset = fs->root_start;
		for (i = 0; i < fs->root_entries; i++) {
			int r = fat_read_dir_entry(fd, offset, &lfn_c, &dir_ent);
			if (r < 0) {
				fat_lfn_reset(&lfn_c, 1);
				return r;
			}

			if (r) {
				fat_check_dir_entry_label(fs, &lfn_c, &dir_ent);
			}
			offset += sizeof(struct fat_dir_entry);
		}
	}
	fat_lfn_reset(&lfn_c, 0);
	return 0;
}

int fs_id_fat_reader(struct partition *part, int probe)
{
	struct fat_fs fs;
	int fd;
	int r;

	memset(&fs, 0, sizeof(fs));
	fd = open(part->devname, O_RDONLY);
	if (fd < 0) {
		SYSLOG_ERR("Cannot open device '%s'", part->devname);
		return -1;
	}

	r = fat_read_boot_sector(fd, &fs, probe);
	if (r)
		goto out_err;

	r = fat_scan_root_for_label(fd, &fs);
	if (r)
		goto out_err;

	SYSLOG_INFO("FAT%d partition '%s' with %d clusters of size %d found.",
		fs.fat_bits, part->devname, fs.clusters, fs.cluster_size);
	/* Assume always vfat */
	part->fstypename = strdup("vfat");
	if (!part->fstypename) {
		DIE("Cannot alloc memory for fs type 'vfat'");
	}

	if (asprintf(&part->fsid, "%08x", fs.fsid) == -1) {
		DIE("Cannot generate fs id 0x%08x entry", fs.fsid);
	}

	part->label = fs.label;
	part->flags = PART_FS_CLEAN_UNKNOWN;

	r = 0;
out_err:
	if (close(fd))
		SYSLOG_ERR("Cannot close device '%s'", part->devname);
	return r;
}
