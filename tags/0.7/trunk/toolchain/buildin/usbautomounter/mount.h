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
#ifndef __MOUNT_H
#define __MOUNT_H 1

struct mounted {
	char *dev;
	char *mountpoint;
	int dev_prefix_match;
	struct mounted *prev;
	struct mounted *next;
};

struct mounted_list {
	struct mounted *first;
	struct mounted *last;
};

char *strmntunescape(const char *buf);
char *strmntescape(const char *buf);
int parse_mount_opts(const char *opt_in, int default_flags);
int get_all_mounted(struct mounted_list *mounts);
struct mounted *get_mounted_by(struct mounted_list *mounts,
		const char *dev, const char *mountpoint);
int do_mount(char *mnt_prefix, char *dev, char *mnt_point, char *type,
		const char *mnt_flags, int default_flags);
int do_umount(char *mnt_prefix, struct mounted *mounted, int lazy);
int do_umount_all(char *mnt_prefix, struct mounted_list *mounts, int lazy);
void delete_mounted(struct mounted_list *mounts, struct mounted *mounted);
#endif
