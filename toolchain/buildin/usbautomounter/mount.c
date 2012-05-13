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
#include <mntent.h>
#include <string.h>
#include <sys/mount.h>
#include <errno.h>
#include "mount.h"
#include "config.h"
#include "tools.h"

#ifndef MNT_DETACH
#warning "MNT_DETACH not defined. Lazy umount supported?"
/* Lazy umounting */
#define MNT_DETACH 2
#endif

struct mount_flag {
	char *name;
	int flag;
};

static struct mount_flag mount_flags[] = {
	{"defaults", 0}, /* Needs to be allways the first entry */
	{"nosuid", MS_NOSUID},
	{"suid", ~MS_NOSUID},
	{"dev", ~MS_NODEV},
	{"nodev", MS_NODEV},
	{"exec", ~MS_NOEXEC},
	{"noexec", MS_NOEXEC},
	{"sync", MS_SYNCHRONOUS},
	{"async", ~MS_SYNCHRONOUS},
	{"atime", ~MS_NOATIME},
	{"noatime", MS_NOATIME},
	{"diratime", ~MS_NODIRATIME},
	{"nodiratime", MS_NODIRATIME},
	{"ro", MS_RDONLY},
	{"rw", ~MS_RDONLY}
};

#define MOUNT_FLAGS_COUNT (sizeof(mount_flags) / sizeof(mount_flags[0]))

int parse_mount_opts(const char *opt_in, int default_flags)
{
	char *opts;
	char *p;
	char *nextopt;
	int flags;
	int count;
	int i;

	if (!opt_in)
		return default_flags;

	p = opts = strdup(opt_in);
	if (!opts) {
		DIE("Cannot alloc memory for mount options");
	}
	mount_flags[0].flag = default_flags;

	flags = 0;
	count = 0;
	nextopt = opts;
	while (nextopt && *nextopt) {
		opts = nextopt;
		nextopt = strchr(opts, ',');
		if (nextopt) {
			*nextopt = 0;
			nextopt++;
			if (nextopt - opts <= 1)
				continue;
		}
		for (i = 0; i < MOUNT_FLAGS_COUNT; i++) {
			if(!strcasecmp(mount_flags[i].name, opts)) {
				if (mount_flags[i].flag < 0)
					flags &= mount_flags[i].flag;
				else
					flags |= mount_flags[i].flag;
				count++;
				break;
			}
		}
		if (i == MOUNT_FLAGS_COUNT)
			SYSLOG_WARN("Unrecognized mount option %s decteced", opts);
	}

	if (!count) {
		flags = default_flags;
	}

	free(p);
	return flags;
}

char *strmntunescape(const char *buf)
{
	int len;
	char *ret_buf;
	const char *buf_pos;
	char *ret_buf_pos;

	len = strlen(buf);
	ret_buf = pmalloc(len + 1);
	buf_pos = buf;
	ret_buf_pos = ret_buf;
	while (len >= 4 && (buf_pos = memchr(buf, '\\', len))) {
		memcpy(ret_buf_pos, buf, buf_pos - buf);
		ret_buf_pos += buf_pos - buf;
		len -= buf_pos - buf;
		buf = buf_pos;
		if (len >= 4) {
			char c = '\\';
			int n = 1;
			if (!strncmp(buf,"\\011", 4)) {
				c = '\t';
				n = 4;
			} else if (!strncmp(buf,"\\012", 4)) {
				c = '\n';
				n = 4;
			} else if (!strncmp(buf,"\\040", 4)) {
				c = ' ';
				n = 4;
			} else if (!strncmp(buf,"\\134", 4)) {
				n = 4;
			}
			ret_buf_pos[0] = c;
			ret_buf_pos++;
			len -= n;
			buf += n;
		}
	}
	memcpy(ret_buf_pos, buf, len + 1);
	return ret_buf;
}

char *strmntescape(const char *buf)
{
	int len;
	char *ret_buf;
	const char *buf_pos;
	char *ret_buf_pos;

	len = strlen(buf);
	ret_buf = pmalloc(len * 4 + 1);
	buf_pos = buf;
	ret_buf_pos = ret_buf;
	while (len) {
		switch (buf_pos[0]) {
			case '\t':
				memcpy(ret_buf_pos,"\\011",4);
				ret_buf_pos += 4;
				break;
			case '\n':
				memcpy(ret_buf_pos,"\\012",4);
				ret_buf_pos += 4;
				break;
			case ' ':
				memcpy(ret_buf_pos,"\\040",4);
				ret_buf_pos += 4;
				break;
			case '\\':
				memcpy(ret_buf_pos,"\\134",4);
				ret_buf_pos += 4;
				break;
			default:
				ret_buf_pos[0] = buf_pos[0];
				ret_buf_pos++;
		}
		buf_pos++;
		len--;
	}
	ret_buf_pos[0] = 0;
	return ret_buf;
}

static void append_mounted(struct mounted_list *mounts, char *dev, char *dir)
{
	struct mounted *mounted;

	mounted = pmalloc(sizeof(*mounted));
	mounted->dev = dev;
	mounted->mountpoint = dir;

	if (mounts->last) {
		mounted->prev = mounts->last;
		mounted->next = NULL;
		mounts->last->next = mounted;
		mounts->last = mounted;
	} else {
		mounted->prev = NULL;
		mounted->next = NULL;
		mounts->first = mounted;
		mounts->last = mounted;
	}
}

int get_all_mounted (struct mounted_list *mounts)
{
	FILE *mtab_file;
	struct mntent *mnt;
	int r;

	mtab_file = setmntent(CONFIG_MTAB, "r");
	if (!mtab_file) {
		DIE("Cannot open mtab '" CONFIG_MTAB "'");
	}

	while ((mnt = getmntent(mtab_file))) {
		char *fsname;
		char *dir;

		/* Need to unescape tabs, spaces, backslashes and newlines */
		fsname = strmntunescape(mnt->mnt_fsname);
		dir = strmntunescape(mnt->mnt_dir);
		append_mounted(mounts, fsname, dir);
	}

	r = 0;
	if (ferror(mtab_file) && !feof(mtab_file)){
		SYSLOG_ERR("Cannot read mtab '" CONFIG_MTAB "'");
		r = -1;
	}

	endmntent(mtab_file);
	return r;
}

int do_mount(char *mnt_prefix, char *dev, char *mnt_point, char *type,
		const char *mnt_flags, int default_flags)
{
	int r;
	int flags;

	if ((r = mk_all_dirs_prefixed(mnt_prefix, mnt_point))) {
		return r;
	}

	flags = parse_mount_opts(mnt_flags, default_flags);

	SYSLOG_INFO("mounting '%s' on '%s'",dev,mnt_point);
	if (mount(dev, mnt_point, type, flags, NULL)) {
		SYSLOG_ERR("Cannot mount '%s' to '%s'", dev, mnt_point);
		rm_all_dirs_prefixed(mnt_prefix, mnt_point);
		return -1;
	}

	return 0;
}

int do_umount(char *mnt_prefix, struct mounted *mounted, int lazy)
{
	int r;
	SYSLOG_INFO("unmounting '%s' on '%s'",mounted->dev,mounted->mountpoint);
	if ((r = umount2(mounted->mountpoint, 0))) {
		if (errno != EBUSY) {
			SYSLOG_ERR("Cannot umount '%s' on '%s'", mounted->dev,
					mounted->mountpoint);
			return r;
		}
		SYSLOG_WARN("Mount point '%s' on '%s' busy try lazy umount.",
				mounted->dev, mounted->mountpoint);
		if(!lazy)
			return r;
		r = umount2(mounted->mountpoint, MNT_DETACH);
		if (r && errno != EBUSY) {
			SYSLOG_ERR("Lazy umount '%s' on '%s' failed", mounted->dev,
					mounted->mountpoint);
			return r;
		}
	}

	rm_all_dirs_prefixed(mnt_prefix, mounted->mountpoint);
	return 0;
}

int do_umount_all(char *mnt_prefix, struct mounted_list *mounts, int lazy)
{
	struct mounted *cur_mnt;
	cur_mnt = mounts->last; /* Umount in reverse order */
	int res=0;
	while (cur_mnt) {
		if(do_umount(mnt_prefix,cur_mnt,lazy))
		{
			cur_mnt=cur_mnt->prev;
			res++;
			continue;
		}
		struct mounted *prev=cur_mnt->prev;
		delete_mounted(mounts,cur_mnt);
		cur_mnt=prev;
	}
	return res;
}

struct mounted *get_mounted_by(struct mounted_list *mounts,
		const char *dev, const char *mountpoint)
{
	struct mounted *cur_mnt;

	cur_mnt = mounts->first;
	while (cur_mnt) {
		if((!dev || !strcmp(cur_mnt->dev, dev)) &&
				!strcmp(cur_mnt->mountpoint, mountpoint))
			return cur_mnt;
		cur_mnt = cur_mnt->next;
	}
	return NULL;
}

void delete_mounted(struct mounted_list *mounts, struct mounted *mounted)
{
	free(mounted->dev);
	free(mounted->mountpoint);

	if (mounted->prev)
		mounted->prev->next = mounted->next;
	else
		mounts->first = mounted->next;

	if (mounted->next)
		mounted->next->prev = mounted->prev;
	else
	   mounts->last = mounted->prev;

	free(mounted);
}
