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

#ifndef __SETUP_H
#define __SETUP_H 1
#include <stdio.h>
#define VOL_MOUNT_LABEL 1
#define VOL_MOUNT_UNKNOWN 2
#define VOL_UNCLEAN 4
#define VOL_UNCLEAN_MOUNTED 8

struct usbautomounter_config {
	char *mount_prefix;
	char *mount_table_file;
	char *mount_unknown_prefix;
	char *mount_default_opts;
	int mount_default_flags;
	int mount_unclean;
	int fsck_as_hotplug;
	char *action_fsck;
	char *action_volume_found;
	char *action_volume_found_unclean;
	char *action_volume_lost;
};

struct mount_table_entry {
	char *fs_id;
	char *mount_suffix;
	char *mount_opts;
	int unknown_n;
	int mounter_flags;
	int changed;
	int line_n;
	int duplicate;
	struct mount_table_entry *prev;
	struct mount_table_entry *next;
};

struct mount_table {
	int changed;
	int next_unknown_n;
	struct mount_table_entry *first;
	struct mount_table_entry *last;
};

int read_config_file(struct usbautomounter_config *conf);
FILE *open_mount_table_file(struct usbautomounter_config *conf);
void close_mount_table_file(FILE *tab_file);
int read_mount_table(FILE *tab_file, struct mount_table *tab_list);
struct mount_table_entry *mount_table_entry_by_fs_id(
	struct mount_table *tab_list, const char *fs_id);
struct mount_table_entry *new_mount_table_entry(struct mount_table *tab_list,
		 const char *fs_id, const char *label);
int write_mount_table(struct usbautomounter_config *conf, FILE *tab_file,
		 struct mount_table *tab_list);
void free_config(struct usbautomounter_config *conf);
#endif
