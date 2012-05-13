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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "setup.h"
#include "mount.h"
#include "scsi_usb_scan.h"
#include "partitions_reader.h"
#include "fs_id_reader.h"
#include "tools.h"
#include "config.h"
#include "display.h"

int useSVDRP=0;
struct usbmounter_context {
	int ashotplug;
	int autoboot;
	struct usbautomounter_config conf;
	FILE *table_file;
	struct mount_table table;
	struct partition_list part_list;
	struct mounted_list mounted_list;
};

static void setup_part_asso(struct usbmounter_context *cont) {
	struct partition *part;
	struct mount_table_entry *tab_ent;
	int r;

	part = cont->part_list.first;
	while (part) {
		tab_ent = mount_table_entry_by_fs_id(&cont->table,part->fsid);
		if (!tab_ent) {
			tab_ent = new_mount_table_entry(&cont->table, part->fsid, part->label);
		} else if (part->label &&
				((tab_ent->mounter_flags & VOL_MOUNT_UNKNOWN) ||
				((tab_ent->mounter_flags & VOL_MOUNT_LABEL) &&
				strcmp(tab_ent->mount_suffix, part->label)))) {
			tab_ent->mounter_flags &= ~VOL_MOUNT_UNKNOWN;
			tab_ent->mounter_flags |= VOL_MOUNT_LABEL;
			free(tab_ent->mount_suffix);
			tab_ent->mount_suffix = strdup(part->label);
			if (!tab_ent->mount_suffix)
				DIE("Cannot alloc memory");
			tab_ent->changed = 1;
			cont->table.changed =1;
		}
		part->tab_entry = tab_ent;

		if (tab_ent->mounter_flags & VOL_MOUNT_UNKNOWN)
			r = asprintf(&part->mountpoint,  "%s/%s%d", cont->conf.mount_prefix,
					cont->conf.mount_unknown_prefix, tab_ent->unknown_n);
		else
			r = asprintf(&part->mountpoint,  "%s/%s", cont->conf.mount_prefix,
					tab_ent->mount_suffix);
		if (r < 0)
			DIE("Cannot alloc memory");

		part->mounted = get_mounted_by(&cont->mounted_list, part->devname,
				part->mountpoint);
		part = part->next;
	}
}

static void get_context(struct usbmounter_context *cont,int mounting)
{
	int scsi_usb_devs_count;
	char **scsi_usb_devs;
	int i;

	memset(cont, 0, sizeof(*cont));
	if (read_config_file(&cont->conf))
		DIE("Config file could not be readed");

	cont->table_file = open_mount_table_file(&cont->conf);
	if (!cont->table_file)
		DIE("Table file could not be opened");

	if (read_mount_table(cont->table_file, &cont->table))
		DIE("Table file could not be readed");

	if (get_all_mounted(&cont->mounted_list))
		DIE("Mounted file systems could not be readed");

	scsi_usb_devs_count = get_all_scsi_usb_disks(&scsi_usb_devs);
	for (i = 0; i < scsi_usb_devs_count; i++) {
		read_partitions(scsi_usb_devs[i], &cont->part_list);
		free(scsi_usb_devs[i]);
	}
	free(scsi_usb_devs);
	read_all_fs_ids(&cont->part_list,mounting);
	setup_part_asso(cont);
}

static void check_devs_mounted(struct usbmounter_context *cont)
{
	struct mounted *cur_mnt;
	struct mounted *tmp_mnt;
	int mnt_prefix_len;
	char *mnt_prefix;

	mnt_prefix_len = strlen(cont->conf.mount_prefix) ;
	mnt_prefix = pmalloc(mnt_prefix_len + 2);
	memcpy(mnt_prefix, cont->conf.mount_prefix, mnt_prefix_len);
	mnt_prefix[mnt_prefix_len] = '/';
	mnt_prefix_len++;
	mnt_prefix[mnt_prefix_len] = 0;

	cur_mnt = cont->mounted_list.last;
	while (cur_mnt) {
		struct partition *part;
		if (strncmp(cur_mnt->dev, SCSI_DISK_DEV_PREFIX,
				SCSI_DISK_DEV_PREFIX_LEN)) {
			tmp_mnt = cur_mnt->prev;
			cur_mnt->dev_prefix_match = 0;
			if (strncmp(cur_mnt->mountpoint, mnt_prefix, mnt_prefix_len))
				delete_mounted(&cont->mounted_list, cur_mnt);
			cur_mnt = tmp_mnt;
			continue;
		}

		cur_mnt->dev_prefix_match = 1;
		part = cont->part_list.first;
		while (part && strcmp(part->devname,cur_mnt->dev))
			part = part->next;


		if (strncmp(cur_mnt->mountpoint, mnt_prefix, mnt_prefix_len)) {
			if (part && !part->mounted) {
				/* Dev is mouned elsewhere never do fsck in this case.
				 * Clean markers of fs may be wrong. */
				part->dev_mounted = 1;
				part->flags &= ~(PART_FS_UNCLEAN | PART_FS_HAS_ERROR |
					PART_FS_MOUNT_INTERVAL);
			}
			tmp_mnt = cur_mnt->prev;
			delete_mounted(&cont->mounted_list, cur_mnt);
			cur_mnt = tmp_mnt;
			continue;
		} else if (part && !part->mounted) {
			/* okay dev need remount, as mountpoint has changed */
			SYSLOG_INFO("Mount point for '%s' has changed from '%s' to '%s'",
					part->devname, cur_mnt->mountpoint, part->mountpoint);
			part->mounted = cur_mnt;
			part->remount = 1;
		}
		cur_mnt = cur_mnt->prev;
	}
	free(mnt_prefix);
}

static void umount_lost_volumes(struct usbmounter_context *cont)
{
	struct mounted *cur_mounted;
	struct mounted *tmp;

	cur_mounted = cont->mounted_list.last;
	while (cur_mounted) {
		struct partition *part;
		if (!cur_mounted->dev_prefix_match) {
			cur_mounted = cur_mounted->prev;
			continue;
		}

		part = cont->part_list.first;
		while (part && part->mounted != cur_mounted)
			part = part->next;

		if (part) {
			cur_mounted = cur_mounted->prev;
			continue;
		}

		SYSLOG_INFO("Found lost volume %s mounted to %s", cur_mounted->dev,
				cur_mounted->mountpoint);
		if (cont->ashotplug && cont->conf.action_volume_lost)
			EXEC_SIMPLE(cont->conf.action_volume_lost, 2, cur_mounted->dev,
					cur_mounted->mountpoint);

		do_umount(cont->conf.mount_prefix, cur_mounted, 1);

		tmp = cur_mounted->prev;
		delete_mounted(&cont->mounted_list, cur_mounted);
		cur_mounted = tmp;
	}

}

static void check_dup_mountpoints(struct usbmounter_context *cont)
{
	struct partition *part;
	struct partition *part2;
	struct mounted *cur_mnt;
	int first;
	int r;
	int i;
	int changed;

	for (i = 0; i < 3; i++) {
		part = cont->part_list.first;
		while (part) {
			if (part->mounted && !part->remount){
				part = part->next;
				continue;
			}
			if (i == 0 && !(part->tab_entry->mounter_flags & VOL_MOUNT_UNKNOWN)) {
				part = part->next;
				continue;
			}
			if (i == 1 && !(part->tab_entry->mounter_flags & VOL_MOUNT_LABEL)) {
				part = part->next;
				continue;
			}
			if (i == 2 && (part->tab_entry->mounter_flags &
					(VOL_MOUNT_UNKNOWN | VOL_MOUNT_LABEL))) {
				part = part->next;
				continue;
			}

			first = 1;
			do {
				changed = 0;
				part2 = cont->part_list.first;
				while (part2) {
					if (part == part2 ||
							strcmp(part->mountpoint,part2->mountpoint)) {
						part2 = part2->next;
						continue;
					}
					if (first)
						SYSLOG_WARN("Found two devices with the same "
								" mount point '%s'", part->mountpoint);

					part->tab_entry->unknown_n = cont->table.next_unknown_n;
					cont->table.next_unknown_n++;

					free(part->mountpoint);
					r = asprintf(&part->mountpoint, "%s/%s%d",
							cont->conf.mount_prefix,
							cont->conf.mount_unknown_prefix,
							part->tab_entry->unknown_n);
					if (r < 0)
						DIE("Cannot alloc memory");

					/* Mount point for part has changed need rechecking*/
					first = 0;
					if (part->mounted &&
							!strcmp(part->mountpoint, part->mounted->mountpoint)) {
						part->remount = 0;
					}
 					part2 = cont->part_list.first;
				}

				cur_mnt = cont->mounted_list.first;
				while (cur_mnt) {
					if (cur_mnt->dev_prefix_match ||
						strcmp(part->mountpoint,cur_mnt->mountpoint)) {
						/* Have already been checked in part list or not matching*/
						cur_mnt = cur_mnt->next;
						continue;
					}

					if (first)
						SYSLOG_WARN("Found mount with the same mount point '%s'",
								part->mountpoint);

					part->tab_entry->unknown_n = cont->table.next_unknown_n;
					cont->table.next_unknown_n++;

					free(part->mountpoint);
					r = asprintf(&part->mountpoint,  "%s/%s%d",
							cont->conf.mount_prefix,
							cont->conf.mount_unknown_prefix,
							part->tab_entry->unknown_n);
					if (r < 0)
						DIE("Cannot alloc memory");

					/* Mount point for part has changed need full rechecking */
					first = 0;
					if (part->mounted &&
							!strcmp(part->mountpoint, part->mounted->mountpoint)) {
						part->remount = 0;
					}
					changed = 1;
					break;
				}

			} while(changed);
			part = part->next;
		}
	}
}

static void setup_mount_order(struct usbmounter_context *cont)
{
	struct partition *part;
	struct partition *part2;
	struct partition *ins_before;
	int mp_len;

	part = cont->part_list.first;
	while (part) {
		mp_len = strlen(part->mountpoint);
		ins_before = NULL;
		part2 = part->prev;
		while (part2) {
			if (!strncmp(part->mountpoint, part2->mountpoint, mp_len) &&
					part2->mountpoint[mp_len] == '/') {
				ins_before = part2;
			}
			part2 = part2->prev;
		}
		part2 = part->next;
		if (ins_before) {
			if(!part->mounted || part->remount) {
				part2 = ins_before;
				while (part2) {
					if (part2->mounted && !part2->remount &&
							!strncmp(part->mountpoint, part2->mountpoint, mp_len) &&
							part2->mountpoint[mp_len] == '/') {
						part2->remount = 1;
					}
					part2 = part2->next;
				}
				part2 = part->next;
			}
			part->prev->next = part->next;
			if (part->next) {
				part->next->prev = part->prev;
			} else {
				cont->part_list.last = part->prev;
			}

			part->prev = ins_before->prev;
			ins_before->prev = part;
			part->next = ins_before;
			if (part->prev) {
				part->prev->next = part;
			} else {
				cont->part_list.first = part;
			}
		}
		part = part2;
	}
}

#define MAX_PROGRESS_STEP 5
static int fsck_calc_percent(int type, int step, uint32_t val, uint32_t max)
{
	static const int percent_tab[2][MAX_PROGRESS_STEP + 1] = {
		{ 0, 70, 90, 92, 95, 100},
		{ 0, 10, 20, 55, 80, 100}
	};
	uint32_t per;
	uint32_t diff;
	if (step <= 0)
		return 0;
	if (step > MAX_PROGRESS_STEP)
		return 100;
	if (max == 0)
		return percent_tab[type][step];
	if (val > max)
      val = max;

	diff = percent_tab[type][step] - percent_tab[type][step -1];
	per = ((val * diff) + (max >> 1)) / max;
	per += percent_tab[type][step -1];
	return per;
}

static void update_progress(const char *line, void *arg)
{
	int step;
	int type;
	uint32_t val;
	uint32_t max;
	int per;

	type = *((int *)arg);
	if (sscanf(line,"%d %u %u", &step, &val, &max) != 3)
		return;
	per = fsck_calc_percent(type, step, val, max);
	display_update_bar(per);
}

static void syslog_cmd_out(const char *line, void *arg)
{
	const char *cmd_line;
	cmd_line = (const char *) arg;
	SYSLOG_INFO("output of '%s': %s", cmd_line, line);
}

static int read_line_exec(int fd, char **buf, int *size,
	int *used, void (*lexec)(const char*, void *), void *arg)
{
	char *tmp;
	int r;
	int ret_val;
	ret_val = 0;

	if (*size - *used <= 1) {
		tmp = realloc(*buf, *size + 128);
		if (!tmp) {
			SYSLOG_ERR("Cannot alloc mem");
			return -1;
		}
		*buf = tmp;
		(*size) += 128;
	}

	r = read(fd, *buf + *used, *size - *used - 1);
	if (r < 0) {
		if (errno == EINTR || errno == EAGAIN || errno == EBUSY)
			return 0;
		SYSLOG_ERR("Read failed");
		return -1;
	}

	if (r == 0) {
		close(fd);
		ret_val = 1;
		if (*used > 0) {
			(*buf)[*used] = '\n';
			(*used)++;
		}
	}
	(*used) += r;

	while ((tmp = memchr(*buf, '\n', *used))) {
		*tmp = 0;
		tmp++;
		lexec(*buf, arg);
		(*used) -= tmp - *buf;
		memmove(*buf, tmp, *used);
	}
	return ret_val;
}

static void handle_fsck_case(struct usbmounter_context *cont,
	struct partition *part, int force_fsck)
{
	int check_ret;
	char checker_bin[128];
	check_ret = 4;

	SYSLOG_INFO("Device '%s' is unclean needs checking",
			part->devname);

	if(force_fsck<0)
		return;

	snprintf(checker_bin, 128, "/sbin/fsck.%s",part->fstypename);
	checker_bin[127] = 0;
	if((cont->ashotplug)&&((useSVDRP<=0)||(!cont->conf.fsck_as_hotplug))){
		if (!cont->conf.fsck_as_hotplug)
			return;

		if (cont->conf.action_fsck)
			EXEC_SIMPLE(cont->conf.action_fsck, 3, part->devname,
				part->mountpoint, "start");

		if (force_fsck && !strncasecmp(part->fstypename,"ext",3))
			EXEC_SIMPLE_RET(check_ret,checker_bin, 2, "-yf", part->devname);
		else
			EXEC_SIMPLE_RET(check_ret,checker_bin, 2, "-y", part->devname);

		if (cont->conf.action_fsck)
			EXEC_SIMPLE(cont->conf.action_fsck, 3, part->devname,
				part->mountpoint, "stop");
	} else {
		int rpipefd;
		int wpipefd;
		pid_t cpid;
		int progress_pipe[2];
		int progress_type;
		char *msg;
		char *tmp;
		int r;
		int reof; int peof;
		char *rbuf; char *pbuf;
		int rsize; int psize;
		int rlen; int plen;

		display_init(cont->autoboot);

		r = asprintf(&msg,(part->flags & PART_FS_MOUNT_INTERVAL) ?
			tr("Disk %s too long unchecked") :
			tr("Disk %s unclean unmounted"), part->devname);
		if (r < 0) {
			SYSLOG_ERR("unclean msg generation failed");
			goto out;
		}
		display_msg(msg);
		free(msg);

		progress_type = 0;
		if (strncasecmp(part->fstypename,"ext",3))
			progress_type = 1;

		if (pipe(progress_pipe)) {
			SYSLOG_ERR("Cannot open pipe");
			goto out;
		}

		r = asprintf(&tmp,"%d",progress_pipe[1]);
		if (r < 0) {
			SYSLOG_ERR("No mem");
			close(progress_pipe[0]);
			close(progress_pipe[1]);
			goto out;
		}

		if (force_fsck && !strncasecmp(part->fstypename,"ext",3))
			exec_cmd(&wpipefd, &rpipefd, progress_pipe[1], &cpid, checker_bin, 4,
				"-yf", "-C", tmp, part->devname);
		else
			exec_cmd(&wpipefd, &rpipefd, progress_pipe[1], &cpid, checker_bin, 4,
				"-y", "-C", tmp, part->devname);

		close(wpipefd);
		close(progress_pipe[1]);

		reof = 0; peof = 0;
		r = fcntl(rpipefd, F_SETFL, fcntl(rpipefd, F_GETFL) | O_NONBLOCK);
		if (r) {
			SYSLOG_ERR("Fcntl failed");
			reof = 1;
			close(rpipefd);
		}

		r = fcntl(progress_pipe[0], F_SETFL,
			fcntl(progress_pipe[0], F_GETFL) | O_NONBLOCK);
		if (r) {
			SYSLOG_ERR("Fcntl failed");
			close(progress_pipe[0]);
			peof = 1;
		}

		rbuf = NULL; pbuf = NULL;
		rsize = 0; psize = 0;
		rlen = 0; plen = 0;
		while (!reof || !peof) {
			fd_set rfds;
			int nfd;
			nfd = 0;
			FD_ZERO(&rfds);
			if (!reof) {
				FD_SET(rpipefd, &rfds);
				nfd = rpipefd;
			}
			if (!peof) {
				FD_SET(progress_pipe[0], &rfds);
				if (progress_pipe[0] > nfd)
					nfd = progress_pipe[0];
			}
			nfd++;

			r = select(nfd, &rfds, NULL, NULL, NULL);
			if (r < 0) {
				if (errno == EINTR || errno == EAGAIN || errno == EBUSY)
					continue;
				SYSLOG_ERR("Select failed");
				break;
			}

			if (FD_ISSET(rpipefd, &rfds)) {
				r = read_line_exec(rpipefd, &rbuf, &rsize, &rlen,
					syslog_cmd_out, checker_bin);
				if (r < 0) {
					break;
				}
				if (r == 1) {
					reof = 1;
				}
			}

			if (FD_ISSET(progress_pipe[0], &rfds)) {
				r = read_line_exec(progress_pipe[0], &pbuf, &psize, &plen,
					update_progress, &progress_type);
				if (r < 0) {
					break;
				}
				if (r == 1) {
					peof = 1;
				}
			}
		}
		free(rbuf);
		free(pbuf);
		if (!reof)
			close(rpipefd);
		if (!peof)
			close(progress_pipe[0]);
		check_ret = wait_on_cmd(cpid, checker_bin);
	}

out:
	if (check_ret >= 0 && check_ret < 4) {
		if((!cont->ashotplug)||(useSVDRP>0)){
			display_msg(tr("Check successful"));
			sleep(2);
		}
		part->flags &= ~(PART_FS_UNCLEAN | PART_FS_HAS_ERROR |
			 PART_FS_MOUNT_INTERVAL);
		if (part->flags & PART_FS_CLEAN_UNKNOWN) {
			part->tab_entry->mounter_flags &= ~(VOL_UNCLEAN |
				VOL_UNCLEAN_MOUNTED);
			part->tab_entry->changed = 1;
			cont->table.changed = 1;
		}
	} else {
		SYSLOG_INFO("Device '%s' remains unclean after fsck",
			part->devname);
		if((!cont->ashotplug)||(useSVDRP>0)){
			display_msg(tr("Check failed"));
			sleep(2);
		}
	}
}

static void mount_partitions(struct usbmounter_context *cont, int force_fsck)
{
	struct partition *part;
	struct mounted *cur_mnt;

	part = cont->part_list.first;
	while (part) {
		struct partition *tmp_part = part->next;
		if (part->mounted && !part->remount) {
			delete_mounted(&cont->mounted_list, part->mounted);
			delete_partition(&cont->part_list, part);
		}
		part = tmp_part;
	}

	cur_mnt = cont->mounted_list.first;
	while (cur_mnt) {
		struct mounted *tmp_mnt = cur_mnt->next;
		if (!cur_mnt->dev_prefix_match) {
			delete_mounted(&cont->mounted_list, cur_mnt);
		}
		cur_mnt = tmp_mnt;
	}

	do_umount_all(cont->conf.mount_prefix, &cont->mounted_list, 1);

	part = cont->part_list.first;
	while (part) {
		struct partition *tmp_part = part->next;
		if (!part->mounted) {
			if ((part->flags & PART_FS_CLEAN_UNKNOWN) &&
					(part->tab_entry->mounter_flags & (VOL_UNCLEAN_MOUNTED | VOL_UNCLEAN))) {
				part->flags |= PART_FS_UNCLEAN;
			}
			if ((force_fsck>0) || ((part->flags & (PART_FS_UNCLEAN | PART_FS_HAS_ERROR)) && !part->dev_mounted)) {
				handle_fsck_case(cont, part, force_fsck);
			}

			if (part->flags & (PART_FS_UNCLEAN | PART_FS_HAS_ERROR)) {
				if (!cont->conf.mount_unclean) {
					SYSLOG_INFO("Found unclean fs %s -> %s -- not mounting unclean",
						part->devname,part->mountpoint);
					if (cont->ashotplug && cont->conf.action_volume_found_unclean)
						EXEC_SIMPLE(cont->conf.action_volume_found_unclean, 2,
							part->devname,part->mountpoint);
					delete_partition(&cont->part_list, part);
					part = tmp_part;
					continue;
				} else {
					SYSLOG_INFO("Found unclean fs %s -> %s -- mounting unclean",
						part->devname,part->mountpoint);
					if (part->flags & PART_FS_CLEAN_UNKNOWN) {
						part->tab_entry->mounter_flags |= VOL_UNCLEAN_MOUNTED;
						part->tab_entry->changed = 1;
						cont->table.changed = 1;
					}
				}
			}
		}

		if (!do_mount(cont->conf.mount_prefix,part->devname,
				part->mountpoint, part->fstypename,
				part->tab_entry->mount_opts, cont->conf.mount_default_flags)) {
			if (part->flags & PART_FS_CLEAN_UNKNOWN) {
				part->tab_entry->mounter_flags |= VOL_UNCLEAN;
				part->tab_entry->changed = 1;
				cont->table.changed = 1;
			}

			if (cont->ashotplug && cont->conf.action_volume_found)
				EXEC_SIMPLE(cont->conf.action_volume_found, 2, part->devname,
						part->mountpoint);
		}

		delete_partition(&cont->part_list, part);
		part = tmp_part;
	}
}

static int do_auto_all(int ashotplug, int autoboot, int force_fsck)
{
	struct usbmounter_context context;
	openlog("usbautomounter", LOG_PID | LOG_CONS, LOG_DAEMON);
	SYSLOG_INFO("usb auto mounter started in auto mode");
	get_context(&context,1);
	context.ashotplug = ashotplug;
	context.autoboot = autoboot;
	check_devs_mounted(&context);
	umount_lost_volumes(&context);
	check_dup_mountpoints(&context);
	setup_mount_order(&context);
	mount_partitions(&context, force_fsck);
	write_mount_table(&context.conf, context.table_file, &context.table);
	close_mount_table_file(context.table_file);
	free_config(&context.conf);
	display_deinit();
	SYSLOG_INFO("usb auto mounter finished");
	closelog();
	return 0;
}

static int umount_partitions(struct usbmounter_context *cont, int force_fsck ,int lazy)
{
	struct partition *part;
	struct mounted *cur_mnt;

	part = cont->part_list.first;
	while (part) {
		struct partition *tmp_part = part->next;
		if (!part->mounted) {
			delete_partition(&cont->part_list, part);
		}
		part = tmp_part;
	}

	cur_mnt = cont->mounted_list.first;
	while (cur_mnt) {
		struct mounted *tmp_mnt = cur_mnt->next;
		if (!cur_mnt->dev_prefix_match) {
			delete_mounted(&cont->mounted_list, cur_mnt);
		}
		cur_mnt = tmp_mnt;
	}

	int res=do_umount_all(cont->conf.mount_prefix, &cont->mounted_list, lazy);

	part = cont->part_list.first;
	while (part) {
		struct partition *tmp_part = part->next;
		if ((force_fsck>0) || ((part->flags & PART_FS_MOUNT_INTERVAL) && !part->dev_mounted)) {
			handle_fsck_case(cont, part, force_fsck);
		}

		if ((part->flags & PART_FS_CLEAN_UNKNOWN) &&
				(part->tab_entry->mounter_flags & VOL_UNCLEAN)) {
			part->tab_entry->mounter_flags &= ~VOL_UNCLEAN;
			part->tab_entry->changed = 1;
			cont->table.changed = 1;
		}
		delete_partition(&cont->part_list, part);
		part = tmp_part;
	}
	return res;
}
static int do_auto_umount_mounted(int force_fsck, int lazy)
{
	struct usbmounter_context context;
	openlog("usbautomounter", LOG_PID | LOG_CONS, LOG_DAEMON);
	SYSLOG_INFO("usb auto mounter started in umount mode");
	get_context(&context,0);
	check_devs_mounted(&context);
	umount_lost_volumes(&context);
	int res=umount_partitions(&context, force_fsck, lazy);
	write_mount_table(&context.conf, context.table_file, &context.table);
	close_mount_table_file(context.table_file);
	free_config(&context.conf);
	SYSLOG_INFO("usb auto mounter finished");
	closelog();
	return res;
}

static void print_usage() {
	fprintf(stderr, "usbautomounter " USB_MOUNTER_VERSION_STR "\n\n");
	fprintf(stderr, "usbautomounter <command> [fsck|nofsck] [nolazy] [SVDRP|noSVDRP]\n");
	fprintf(stderr, "\t<command> := usb | mount | remount | umount\n");
	fprintf(stderr, "\tusb | mount | remount\tmounts/remounts all usb storage devices\n");
	fprintf(stderr, "\tumount\t\tumounts all mounted usb storage devices\n");
	fprintf(stderr, "\tfsck|nofsck\toptional, force or disable filesystems check\n");
	fprintf(stderr, "\tnolazy\t\toptional, not unmount busy devices\n");
	fprintf(stderr, "\tSVDRP|noSVDRP\toptional, use or disable SVDRP\n");
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		print_usage();
		return 1;
	}

	int lazy=1;
	int force_fsck = 0;
	int f;
	for(f=2;f<argc;f++)
	{
		if(!strcasecmp(argv[f],"fsck"))
			force_fsck=1;
		else if(!strcasecmp(argv[f],"nofsck"))
			force_fsck=-1;
		else if(!strcasecmp(argv[f],"nolazy"))
			lazy=0;
		else if(!strcasecmp(argv[f],"SVDRP"))
			useSVDRP=1;
		else if(!strcasecmp(argv[f],"noSVDRP"))
			useSVDRP=-1;
	}

	int file=open("/var/usbautomounter",O_CREAT|O_WRONLY);
	if(file!=-1)
		if(flock(file,LOCK_EX))
			SYSLOG_ERR("cannot lock usbautomounter access");
			
	int res=1;
	if (!strcasecmp(argv[1],"mount")) {
		res=do_auto_all(0,1,force_fsck);
	}
	if (!strcasecmp(argv[1],"usb") ||
			!strcasecmp(argv[1],"remount")) {
		res=do_auto_all(1,1,force_fsck);
	}

	if (!strcasecmp(argv[1],"umount")) {
		res=do_auto_umount_mounted(force_fsck,lazy);
	}
	if(file!=-1)
	{
		flock(file,LOCK_UN);
		close(file);
	}
	return res;
}
