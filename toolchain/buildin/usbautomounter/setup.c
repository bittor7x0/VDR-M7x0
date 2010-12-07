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
 * $Id: setup.c 201 2007-09-25 17:42:55Z andreas $
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/file.h>
#include <unistd.h>
#include "config.h"
#include "setup.h"
#include "mount.h"
#include "tools.h"


#ifndef PATH_MAX
#warning "PATH_MAX not defined"
#define PATH_MAY 4096
#endif

struct mounter_opts {
	const char *name;
	int value;
};

#define MOUNTER_OPTS_DEF_COUNT 5
static const struct mounter_opts mounter_opts_def[MOUNTER_OPTS_DEF_COUNT] = {
	{ "defaults", 0 },
	{ "label", VOL_MOUNT_LABEL },
	{ "unknown", VOL_MOUNT_UNKNOWN },
	{ "unclean", VOL_UNCLEAN },
	{ "unclean_mounted", VOL_UNCLEAN_MOUNTED }
};

static int read_config_file_entry(FILE *conf_file, char **key, char **value,
 	int *line_n)
{
	char *line;
	char *p, *p2, *p3;
	int len;
	size_t line_len;
	line = NULL;
	line_len = 0;

	do {
		(*line_n)++;
		if (getline(&line, &line_len, conf_file) < 0) {
			free(line);
			if (feof(conf_file)) {
				return 0;
			}
			SYSLOG_ERR("Cannot read config file line number %d", *line_n);
			return -1;
		}


		/* Comment in line */
		p = line;
		while ((p = strchr(p, '#'))) {
			if (p == line || p[-1] != '\\') {
				*p = 0;
				break;
			}
			p++;
		}

		/* Skip white spaces */
		p = line;
		while (p[0] && (p[0] == '\t' || p[0] == '\r' || p[0] == '\n'
				|| p[0] == ' ')) {
			p++;
		}

		/* Line empty */
		if (!p[0])
			continue;

		p2 = strchr(p, '=');
		if (!p2 || p == p2) {
			SYSLOG_WARN("Config line number %d illegal", *line_n);
			continue;
		}

		p3 = p2 + 1;
		do  {
			p2 --;
		} while (p2[0] == '\t' || p2[0] == ' ');

		len = p2 - p + 1;
		*key = pmalloc(len + 1);
		memcpy(*key, p, len);
		(*key)[len] = 0;

		while (p3[0] && (p3[0] == '\t' || p3[0] == '\r' || p3[0] == '\n'
				|| p3[0] == ' ')) {
			p3++;
		}

		len = strlen(p3);
		p2 = p3 + len;
		do  {
			p2[0] = 0;
			p2 --;
		} while (p2[0] == '\t' || p2[0] == '\n' || p2[0] == '\r' ||
				(p2[0] == ' ' && p2[-1] != '\\'));

		len = p2 - p3 + 1;
		*value = pmalloc(len + 1);
		p = *value;

		/* Handle escapes only allowed in value */
		p2 = p3;
		while (len && (p2 = memchr(p2, '\\', len))) {
			int n = p2 - p3;
			char c;
			memcpy(p, p3, n);

			p += n;
			len -= n;

			n = 2;
			switch (p2[1]) {
				case ' ':	c = ' '; break;
				case '#':	c = '#'; break;
				case '\\':	c = '\\'; break;
				case 't':	c = '\t'; break;
				case 'n':	c = '\n'; break;
				case 'r':	c = '\r'; break;
				default:
					c = '\\';
					n = 1;
			}
			p[0] = c;
			p++;
			p2 += n;
			p3 = p2;
			len -= n;
		}
		memcpy(p, p3, len);
		p[len] = 0;
		free (line);
		return 1;
	} while (1);
}

static void set_config_defaults(struct usbautomounter_config *conf)
{
	free(conf->mount_table_file);
	conf->mount_table_file = strdup(CONFIG_DEFAULT_TABLE_FILE);
	if (!conf->mount_table_file)
		DIE("Cannot alloc memory");

   free(conf->mount_prefix);
	conf->mount_prefix = strdup(CONFIG_DEFAULT_MOUNT_PREFIX);
	if (!conf->mount_prefix)
		DIE("Cannot alloc memory");

	free(conf->mount_unknown_prefix);
	conf->mount_unknown_prefix = strdup(CONFIG_DEFAULT_MOUNT_UNKNOWN_PREFIX);
	if (!conf->mount_unknown_prefix)
		DIE("Cannot alloc memory");

	free(conf->mount_default_opts);
	conf->mount_default_opts = NULL;


	free(conf->action_fsck);
	conf->action_fsck = NULL;

	free(conf->action_volume_found);
	conf->action_volume_found = NULL;

	free(conf->action_volume_found_unclean);
	conf->action_volume_found = NULL;

	free(conf->action_volume_lost);
	conf->action_volume_lost = NULL;
}

static int check_config(struct usbautomounter_config *conf)
{
	char path_res[PATH_MAX + 1];
	if (!conf->mount_table_file || !conf->mount_table_file[0]) {
		SYSLOG_WARN("Mount table file wrong");
		free(conf->mount_table_file);
		conf->mount_table_file = strdup(CONFIG_DEFAULT_TABLE_FILE);
		if (!conf->mount_table_file)
			DIE("Cannot alloc memory");
	}

	if (realpath(conf->mount_table_file, path_res)) {
		free(conf->mount_table_file);
		conf->mount_table_file = strdup(path_res);
		if (!conf->mount_table_file)
			DIE("Cannot alloc memory");
	} else {
		SYSLOG_ERR("Cannot resolve mount table file '%s'", conf->mount_table_file);
		return -1;
	}

	if (!conf->mount_prefix || !conf->mount_prefix[0]) {
		SYSLOG_WARN("Mount prefix wrong");
   	free(conf->mount_prefix);
		conf->mount_prefix = strdup(CONFIG_DEFAULT_MOUNT_PREFIX);
		if (!conf->mount_prefix)
			DIE("Cannot alloc memory");
	}

	if (realpath(conf->mount_prefix, path_res)) {
		free(conf->mount_prefix);
		conf->mount_prefix = strdup(path_res);
		if (!conf->mount_prefix)
			DIE("Cannot alloc memory");
	} else {
		SYSLOG_ERR("Cannot resolve mount prefix '%s'", conf->mount_prefix);
		return -1;
	}

	if (conf->mount_unknown_prefix && conf->mount_unknown_prefix[0] == '/') {
		int i;
		for (i = 1; conf->mount_unknown_prefix[i] == '/'; i++)
			;
		memmove(conf->mount_unknown_prefix, conf->mount_unknown_prefix + i,
					strlen(conf->mount_unknown_prefix + i) + 1);
	}

	if (!conf->mount_unknown_prefix || !conf->mount_unknown_prefix[0]) {
		SYSLOG_WARN("Mount unknown prefix wrong");
		free(conf->mount_unknown_prefix);
		conf->mount_unknown_prefix = strdup(CONFIG_DEFAULT_MOUNT_UNKNOWN_PREFIX);
		if (!conf->mount_unknown_prefix)
			DIE("Cannot alloc memory");
	}

	if (!conf->mount_default_opts) {
		conf->mount_default_flags =
				parse_mount_opts(CONFIG_DEFAULT_MOUNT_OPTS, 0);
	} else {
		conf->mount_default_flags =
				parse_mount_opts(conf->mount_default_opts, 0);
		free(conf->mount_default_opts);
		conf->mount_default_opts = NULL;
	}

	if (conf->action_fsck && !conf->action_fsck[0]) {
		free(conf->action_fsck);
		conf->action_fsck = NULL;
	}

	if (conf->action_volume_found && !conf->action_volume_found[0]) {
		free(conf->action_volume_found);
		conf->action_volume_found = NULL;
	}

	if (conf->action_volume_found_unclean && !conf->action_volume_found_unclean[0]) {
		free(conf->action_volume_found);
		conf->action_volume_found = NULL;
	}

	if (conf->action_volume_lost && !conf->action_volume_lost[0]) {
		free(conf->action_volume_lost);
		conf->action_volume_lost = NULL;
	}
	return 0;
}

void free_config(struct usbautomounter_config *conf)
{
	free(conf->mount_table_file);
	free(conf->mount_prefix);
	free(conf->mount_unknown_prefix);
	free(conf->mount_default_opts);
	free(conf->action_fsck);
	free(conf->action_volume_found);
	free(conf->action_volume_found_unclean);
	free(conf->action_volume_lost);
}

extern int useSVDRP;
int read_config_file(struct usbautomounter_config *conf)
{
	FILE *conf_file;
	char *key;
	char *value;
	int line_n;
	int r;

	set_config_defaults(conf);
	conf_file = fopen(CONFIG_CONFIG_FILE, "r");
	if (!conf_file) {
		SYSLOG_ERR("Cannot open config file " CONFIG_CONFIG_FILE ". Using defaults ...");
		return -1;
	}

	line_n = 0;
	while ((r = read_config_file_entry(conf_file, &key, &value, &line_n)) > 0) {
		if (!strcasecmp(key, "MounterTableFile")) {
			free(conf->mount_table_file);
			conf->mount_table_file = value;
		} else if (!strcasecmp(key, "MountPrefix")) {
			free(conf->mount_prefix);
			conf->mount_prefix = value;
		} else if (!strcasecmp(key, "MountUnknownPrefix")) {
			free(conf->mount_unknown_prefix);
			conf->mount_unknown_prefix = value;
		} else if (!strcasecmp(key, "MountDefaultOptions")) {
			free(conf->mount_default_opts);
			conf->mount_default_opts = value;
		} else if (!strcasecmp(key, "ActionFsck")) {
			free(conf->action_fsck);
			conf->action_fsck = value;
		} else if (!strcasecmp(key, "ActionVolumeFound")) {
			free(conf->action_volume_found);
			conf->action_volume_found = value;
		} else if (!strcasecmp(key, "ActionVolumeFoundUnclean")) {
			free(conf->action_volume_found_unclean);
			conf->action_volume_found_unclean = value;
		} else if (!strcasecmp(key, "ActionVolumeLost")) {
			free(conf->action_volume_lost);
			conf->action_volume_lost = value;
		} else if (!strcasecmp(key, "MountUnclean")) {
			conf->mount_unclean = atoi(value);
         free(value);
		} else if (!strcasecmp(key, "FsckAsHotplug")) {
			conf->fsck_as_hotplug = atoi(value);
         free(value);
		}
		else if(!strcasecmp(key,"SVDRP"))
		{
			if(useSVDRP==0)
				useSVDRP=atoi(value);
			free(value);
		} else {
			SYSLOG_WARN("Unknown config file Entry '%s = %s'", key, value);
			free(value);
		}
		free(key);
	}

	r = !r ? check_config(conf) : r;
	fclose(conf_file);
	return r;
}

FILE *open_mount_table_file(struct usbautomounter_config *conf)
{
	FILE *tab;
	tab = fopen(conf->mount_table_file, "r");
	if (!tab) {
		SYSLOG_ERR("Cannot open mount table file '%s'", conf->mount_table_file);
		return NULL;
	}

	if (flock(fileno(tab), LOCK_EX)) {
		SYSLOG_ERR("Cannot lock mount table file '%s'", conf->mount_table_file);
		fclose(tab);
		return NULL;
	}
	return tab;
}

void close_mount_table_file(FILE *tab_file)
{

	if (flock(fileno(tab_file), LOCK_UN)) {
		SYSLOG_ERR("Cannot unlock mount table file");
	}
	if (fclose(tab_file)) {
		SYSLOG_ERR("Cannot close mount table file");
	}
}

static void parse_mounter_opts(struct mount_table_entry *ent, char *mounter_opts)
{
	char *opt;
	char *next_opt;
	int i;
	ent->mounter_flags = 0;
	ent->unknown_n = -1;

	opt = next_opt = mounter_opts;
	while (next_opt && next_opt[0]) {
		opt = next_opt;
		next_opt = strchr(opt,',');
		if (next_opt) {
			next_opt[0] = 0;
			next_opt++;
		}

		for (i = 0; i < MOUNTER_OPTS_DEF_COUNT; i++) {
			if (!strcasecmp(opt, mounter_opts_def[i].name)) {
				ent->mounter_flags |= mounter_opts_def[i].value;
				break;
			}
		}

		if (i == MOUNTER_OPTS_DEF_COUNT) {
			SYSLOG_WARN("Unknown mounter option '%s'", opt);
		}
	}

	if (ent->mounter_flags & VOL_MOUNT_UNKNOWN) {
		char *p;
		ent->unknown_n = strtol(ent->mount_suffix, &p ,10);

		if (p[0]) {
			SYSLOG_WARN("Mount number '%s' wrong", ent->mount_suffix);
			ent->unknown_n = -1;
		}
		free (ent->mount_suffix);
		ent->mount_suffix = NULL;
	}
}

static char *unparse_mounter_opts(int mounter_flags)
{
	static char opts[512];
	int len;
	int i;

	if (!mounter_flags) {
		strcpy(opts,"defaults");
		return opts;
	}

	len = 0;
	for (i = 0; i < MOUNTER_OPTS_DEF_COUNT; i++) {
		  if (mounter_opts_def[i].value & mounter_flags) {
				int n = strlen(mounter_opts_def[i].name);
				if (len > 0) {
					opts[len] = ',';
					len++;
				}
				memcpy(opts + len, mounter_opts_def[i].name, n);
				len+=n;
			}
	}
	opts[len] = 0;
	return opts;
}

static void append_tab_entry(struct mount_table *tab_list,
		struct mount_table_entry *ent)
{
	struct mount_table_entry *ent2;

	/* Check for duplicates, only last entry is used
	 * all other get deleted in write
	 */
	ent2 = tab_list->first;
	while (ent2) {
		if (!strcasecmp(ent2->fs_id, ent->fs_id)) {
			ent2->duplicate = 1;
			tab_list->changed = 1;
		}
		ent2 = ent2->next;
	}

	if (tab_list->last) {
		tab_list->last->next = ent;
		ent->prev = tab_list->last;
		tab_list->last = ent;
	} else {
		tab_list->first = ent;
		tab_list->last = ent;
	}
}

static int read_mount_table_entry(FILE *tab_file, struct mount_table *tab_list, int *line_n)
{
	int fields;
	char *line;
	char *mounter_opts;
	char *mount_suffix;
	size_t line_size;
	int r;

	struct mount_table_entry *ent;
	ent = pmalloc(sizeof(*ent));
	memset(ent, 0, sizeof(*ent));

	line = NULL;
	line_size = 0;
	mount_suffix = NULL;
	mounter_opts = NULL;
	while ((r = getline(&line, &line_size, tab_file)) > 0) {
		int i;
		(*line_n)++;
		for (i = 0; line[i]; i++) {
			if (line[i] != '\t' && line[i] != '\n' && line[i] != '\r'
					 && line[i] != ' ')
				break;
		}

		if (!line[i] || line[i] == '#') {
			/* empty or comment line */
			continue;
		}

		fields = sscanf(line + i, "%as %as %as %as ", &ent->fs_id,
				&mount_suffix, &ent->mount_opts, &mounter_opts);

		if (fields >= 2)
			break;

		if (fields < 0) {
			if (errno == ENOMEM)
				DIE("Cannot alloc memory");
			SYSLOG_ERR("Illegal mount table line '%s' no %d", line, *line_n);
		}
		if (fields > 0) {
			SYSLOG_WARN("Illegal mount table line '%s' no %d", line, *line_n);
		}

		free(ent->fs_id);
		ent->fs_id = NULL;
		free(mount_suffix);
		mount_suffix = NULL;
		free(ent->mount_opts);
		ent->mount_opts = NULL;
		free(mounter_opts);
		mounter_opts = NULL;
	}

	free(line);
	if (r < 0) {
		free(ent);
		if (feof(tab_file))
			return 0;
		SYSLOG_ERR("Cannot read mount table file");
		return -1;
	}

	ent->mount_suffix = strmntunescape(mount_suffix);
	free(mount_suffix);

	parse_mounter_opts(ent, mounter_opts);
	free (mounter_opts);

	if (!ent->mount_suffix) {
		if (!(ent->mounter_flags & VOL_MOUNT_UNKNOWN)) {
			ent->mounter_flags |= VOL_MOUNT_UNKNOWN;
			ent->mounter_flags &= ~VOL_MOUNT_LABEL;
			ent->changed = 1;
			tab_list->changed = 1;
		}
	}
	else {
		if (ent->mount_suffix[0] == '/') {
			int i;
			for (i = 1; ent->mount_suffix[i] == '/'; i++)
				;
			memmove(ent->mount_suffix, ent->mount_suffix + i,
					strlen(ent->mount_suffix + i) + 1);
			ent->changed = 1;
			tab_list->changed = 1;
		}

		if (!ent->mount_suffix[0]) {
			SYSLOG_WARN("Empty mount suffix");
			ent->mounter_flags |= VOL_MOUNT_UNKNOWN;
			ent->mounter_flags &= ~VOL_MOUNT_LABEL;
			ent->changed = 1;
			tab_list->changed = 1;
		}
	}
	ent->line_n = *line_n;
	append_tab_entry(tab_list, ent);
	return 1;
}

int read_mount_table(FILE *tab_file, struct mount_table *tab_list)
{
	int r;
	int line_n;
	struct mount_table_entry *ent;

	line_n = 0;
	while ((r = read_mount_table_entry(tab_file, tab_list, &line_n)) > 0)
		;

	if (r < 0) {
		return r;
	}

	/* Get next unknown number, check duplicate numbers */
	ent = tab_list->first;
	tab_list->next_unknown_n = 0;
	while (ent) {
		if (!ent->duplicate && ent->unknown_n > 0) {
			struct mount_table_entry *ent2;
			ent2 = tab_list->first;
			while (ent2 != ent) {
				if (!ent2->duplicate && ent2->unknown_n == ent->unknown_n) {
					ent->unknown_n = -1;
					break;
				}
				ent2 = ent2->next;
			}
			if (ent->unknown_n >= tab_list->next_unknown_n)
				tab_list->next_unknown_n = ent->unknown_n + 1;
		}
		ent = ent->next;
	}


	ent = tab_list->first;
	while (ent) {
		if (!ent->duplicate && (ent->mounter_flags & VOL_MOUNT_UNKNOWN)
				&& ent->unknown_n == -1) {
			ent->unknown_n = tab_list->next_unknown_n;
			tab_list->next_unknown_n++;
			ent->changed = 1;
			tab_list->changed = 1;
		}
		ent = ent->next;
	}
	return 0;
}

inline static void del_mount_table_entry(struct mount_table_entry *ent)
{
	free(ent->fs_id);
	free(ent->mount_suffix);
	free(ent->mount_opts);
}

static int write_mount_table_entry(FILE *file, struct mount_table_entry *ent)
{
	int r;
	char *tmp;
	if (ent->mounter_flags & VOL_MOUNT_UNKNOWN) {
		if (asprintf(&ent->mount_suffix,"%d",ent->unknown_n) < 0)
			DIE("Cannot alloc memory");
	}

	tmp = strmntescape(ent->mount_suffix);
	r = fprintf(file, "%s\t%s\t%s\t%s\n", ent->fs_id, tmp,
			ent->mount_opts ? ent->mount_opts : "defaults",
			unparse_mounter_opts(ent->mounter_flags));
	if (r < 0)
		SYSLOG_ERR("Cannot write mount table entry");
	free(tmp);
	return r < 0 ? r : 0;
}

int write_mount_table(struct usbautomounter_config *conf, FILE *tab_file,
		 struct mount_table *tab_list)
{
	char *new_tab_filename;
	FILE *new_tab_file;
	int line_n;
	size_t line_size;
	char *line;
	int r;

	if (!tab_list->changed) {
		while (tab_list->first) {
			struct mount_table_entry *ent = tab_list->first;
			tab_list->first = ent->next;
			del_mount_table_entry(ent);
		}
		return 0;
	}

	if (asprintf(&new_tab_filename,"%s.NEW",conf->mount_table_file) < 0)
		DIE("Cannot alloc memory");

	new_tab_file = fopen(new_tab_filename, "w");
	if (!new_tab_file) {
		SYSLOG_ERR("Cannot open new table file '%s'", new_tab_filename);
		goto out_err_noclose;
	}

	rewind(tab_file);
	line = NULL;
	line_size = 0;
	line_n = 0;
	while ((r = getline (&line, &line_size, tab_file)) > 0) {
		line_n++;
		if (tab_list->first && tab_list->first->line_n == line_n) {
			struct mount_table_entry *ent = tab_list->first;
			tab_list->first = ent->next;
			if (ent->duplicate) {
				del_mount_table_entry(ent);
				continue;
			}
			if (ent->changed) {
				if (write_mount_table_entry(new_tab_file, ent)) {
					del_mount_table_entry(ent);
					goto out_err;
				}
				del_mount_table_entry(ent);
				continue;
			}
			del_mount_table_entry(ent);
		}

		if (fputs(line, new_tab_file)  < 0) {
			SYSLOG_ERR("Cannot write tab file line");
			goto out_err;
		}
	}

	if (r < 0 && !feof(tab_file)) {
		SYSLOG_ERR("Cannot read tab file line");
		goto out_err;
	}

	while (tab_list->first) {
		struct mount_table_entry *ent = tab_list->first;
		tab_list->first = ent->next;
		if (ent->duplicate) {
			del_mount_table_entry(ent);
			continue;
		}
		if (write_mount_table_entry(new_tab_file, ent)) {
			del_mount_table_entry(ent);
			goto out_err;
		}
		del_mount_table_entry(ent);
	}


	if	(fclose(new_tab_file))
		SYSLOG_ERR("Cannot close new mount table file '%s'", new_tab_filename);

	if (rename(new_tab_filename, conf->mount_table_file)) {
		SYSLOG_ERR("Cannot rename mount table file");
		remove(new_tab_filename);
	}

	free(line);
	return 0;
out_err:
	free(line);
	fclose(new_tab_file);
	remove(new_tab_filename);
out_err_noclose:
	while (tab_list->first) {
		struct mount_table_entry *ent = tab_list->first;
		tab_list->first = ent->next;
		del_mount_table_entry(ent);
	}
	free(new_tab_filename);
	return -1;
}

struct mount_table_entry *mount_table_entry_by_fs_id(
	struct mount_table *tab_list, const char *fs_id)
{
	struct mount_table_entry *ent;
	ent = tab_list->first;
	while (ent) {
		if (!ent->duplicate && !strcasecmp(ent->fs_id, fs_id))
			return ent;
		ent = ent->next;
	}
	return NULL;
}

struct mount_table_entry *new_mount_table_entry(struct mount_table *tab_list,
		 const char *fs_id, const char *label)
{
	struct mount_table_entry *ent;
	ent = pmalloc(sizeof(*ent));
	memset(ent, 0, sizeof(*ent));

	ent->fs_id = strdup(fs_id);
	if (!ent->fs_id)
		DIE("Cannot alloc memory");

	append_tab_entry(tab_list, ent);

	if (label) {
		ent->unknown_n = -1;
		ent->mount_suffix = strdup(label);
		if (!ent->mount_suffix)
			DIE("Cannot alloc memory");
		ent->mounter_flags = VOL_MOUNT_LABEL;
	} else {
		ent->unknown_n = tab_list->next_unknown_n;
		tab_list->next_unknown_n++;
		ent->mounter_flags = VOL_MOUNT_UNKNOWN;
	}

	ent->changed = 1;
	tab_list->changed = 1;
	return ent;
}
