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
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include "tools.h"

#define COPY_BUFFER_SIZE (64 * 1024)

int opt_skip_special = 0;
int opt_only_mode = 0;
struct list_entry {
	char *file1;
	char *file2;
	char action;
	mode_t mode;
	uid_t uid;
	gid_t gid;
	int major_no;
	int minor_no;
	int dev_start;
	int minor_inc;
	int dev_count;
};

struct copy_context {
	const char *root_fs_path;
	const char *target_root_path;
	char *strip_tool;
	const char *cur_filename;
	FILE *cur_file;
	int cur_line;
	int valid_entry_fields;
	struct list_entry cur_entry;
};

static void remove_all(const char *rm_name)
{
	struct stat st;
	int i;

	for (i = 0; rm_name[i] == '/'; i++)
		;

	if (!rm_name[i])
		die("Security error: Don't want to delete /");

	if (lstat(rm_name, &st)) {
		if (errno == ENOENT)
			return;

		die("Cannot stat '%s'", rm_name);
	}

	if (S_ISDIR(st.st_mode)) {
		DIR *dir;
		struct dirent *ent;

		dir = opendir(rm_name);
		if (!dir)
			die("Cannot open dir '%s'", rm_name);

		errno = 0;
		while ((ent = readdir(dir))) {
			char *new_name;
			if (!strcmp(ent->d_name,".") || !strcmp(ent->d_name,".."))
				continue;
			if (asprintf(&new_name, "%s/%s", rm_name, ent->d_name) <  0)
				die("Cannot alloc memory");
			remove_all(new_name);
			free(new_name);
		}
		if (errno != 0)
			die("Cannot read dir '%s'",rm_name);
		closedir(dir);
	}

	if (remove(rm_name))
		die("Cannot remove '%s'", rm_name);

}
static void copy_file(const char *from, const char *to)
{
	uint8_t buffer[COPY_BUFFER_SIZE];
	int fd_from;
	int fd_to;
	int r,r2;

	fd_from = open(from, O_RDONLY);
	if (fd_from < 0)
		die("Cannot open file '%s'", from);

	fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_to < 0)
		die("Cannot open file '%s'", to);

	do {
		r2 = r = read(fd_from, buffer, COPY_BUFFER_SIZE);
		if (r < 0) {
			if (errno == EBUSY || errno == EINTR || errno == EAGAIN)
				continue;
			die("Cannot read from file '%s'", from);
		}

		r2 = 0;
		while (r > 0) {
			r2 = write(fd_to, buffer, r);
			if (r2 < 0) {
				if (errno == EBUSY || errno == EINTR || errno == EAGAIN)
					continue;
				die("Cannot write to file '%s'", to);
			}
			r -= r2;
		}
	} while (r2);

	if (close(fd_from))
		fprintf(stderr, "Cannot close file '%s'\n", from);
	if (close(fd_to))
		fprintf(stderr, "Cannot close file '%s'\n", to);
}

static void change_permissons(char *file, uid_t owner, gid_t group,
		mode_t mode)
{
	if (opt_skip_special)
		return;

	if (!opt_only_mode)
		if (lchown(file, owner, group))
			die("Cannot set owner of file '%s'", file);

	if (mode == -1)
		return;

	if (chmod(file, mode))
		die("Cannot set mode of file '%s'", file);
}


static void mk_special_file(char *devname, char type, int major, int minor)
{
	mode_t mod;

	if (opt_skip_special || opt_only_mode)
		return;

	if (type == 'b')
		mod = S_IFBLK;
	else if (type == 'c')
		mod = S_IFCHR;
	else
		mod = S_IFIFO;

	mod |= 0644;
	if (mknod(devname, mod, makedev(major, minor)))
		die("Cannot make special file '%s'", devname);
}

static void generate_device_files(struct copy_context *cont)
{
	char *full_name;

	if (cont->valid_entry_fields < 9) {
		if (asprintf(&full_name,"%s/%s", cont->root_fs_path,
				cont->cur_entry.file1) < 0)
			die("Cannot alloc memory");

		remove_all(full_name);
		mk_special_file(full_name, cont->cur_entry.action,
				cont->cur_entry.major_no, cont->cur_entry.minor_no);
		change_permissons(full_name, cont->cur_entry.uid,
				cont->cur_entry.gid, cont->cur_entry.mode);
		free(full_name);
		return;
	}

	cont->cur_entry.dev_count += cont->cur_entry.dev_start;
	while (cont->cur_entry.dev_start < cont->cur_entry.dev_count) {
		if (asprintf(&full_name,"%s/%s%d", cont->root_fs_path,
				cont->cur_entry.file1, cont->cur_entry.dev_start) < 0)
			die("Cannot alloc memory");

		remove_all(full_name);
		mk_special_file(full_name, cont->cur_entry.action,
				cont->cur_entry.major_no, cont->cur_entry.minor_no);
		change_permissons(full_name, cont->cur_entry.uid,
				cont->cur_entry.gid, cont->cur_entry.mode);
		free(full_name);

		cont->cur_entry.dev_start++;
		cont->cur_entry.minor_no += cont->cur_entry.minor_inc;
	}
}

static void generate_dir(struct copy_context *cont)
{
	struct stat st;
	char *full_name;

	if (asprintf(&full_name,"%s/%s", cont->root_fs_path,
			cont->cur_entry.file1) < 0)
		die("Cannot alloc memory");

	if (lstat(full_name, &st)) {
		if (errno != ENOENT)
			die("Cannot stat '%s'", full_name);
	} else {
		if (S_ISDIR(st.st_mode)){
			free(full_name);
			return;
		}
		remove_all(full_name);
	}

	if (mkdir(full_name, 0755))
		die("Cannot mkdir '%s'",full_name);

	change_permissons(full_name, cont->cur_entry.uid,
			cont->cur_entry.gid, cont->cur_entry.mode);
	free(full_name);
}

static void generate_link(struct copy_context *cont)
{
	char *linkname;

	if (asprintf(&linkname,"%s/%s", cont->root_fs_path,
			cont->cur_entry.file1) < 0)
		die("Cannot alloc memory");

	remove_all(linkname);

	if (symlink(cont->cur_entry.file2, linkname))
		die("Cannot generate sym link %s -> %s", linkname,
				cont->cur_entry.file2);

	change_permissons(linkname, cont->cur_entry.uid,
			cont->cur_entry.gid, -1);
	free(linkname);
}

static void do_strip(struct copy_context *cont)
{
	char *full_name;

	if (asprintf(&full_name,"%s/%s", cont->root_fs_path,
			cont->cur_entry.file1) < 0)
		die("Cannot alloc memory");

	EXEC_SIMPLE(cont->strip_tool, 2, "-s", full_name);

	free(full_name);
}

static void do_copy(struct copy_context *cont)
{
	char *from;
	char *to;

	if (cont->cur_entry.file2[0] == '/') {
		from = strdup(cont->cur_entry.file2);
		if (!from)
			die("Cannot alloc memory");
	} else if (asprintf(&from,"%s/%s", cont->target_root_path,
			cont->cur_entry.file2) < 0)
		die("Cannot alloc memory");

	if (asprintf(&to,"%s/%s", cont->root_fs_path,
			cont->cur_entry.file1) < 0)
		die("Cannot alloc memory");

	remove_all(to);
	copy_file(from, to);
	change_permissons(to, cont->cur_entry.uid,
			cont->cur_entry.gid, cont->cur_entry.mode);
	free(from);
	free(to);
}

static char *strcpyunescape(char **dest, char *src)
{
	char *d;

	while (src[0] && (src[0] == ' ' || src[0] == '\n' || src[0] == '\t'
			|| src[0] == '\r'))
			src++;

	if (!src[0] || src[0] == '#') {
		*dest = NULL;
		return src;
	}

	*dest = d = malloc(strlen(src) + 1);
	if (!d)
		die("Cannot alloc memory");

	while (src[0] && src[0] != ' ' && src[0] != '\n' && src[0] != '\t' &&
			src[0] != '\r') {
		char c;
		int i;
		if (src[0] != '\\') {
			d[0] = src[0];
			d++;
			src++;
			continue;
		}

		i = 2;
		switch (src[1]) {
			case 'n':
				c = '\n';
				break;
			case 't':
				c = '\t';
				break;
			case 'r':
				c = '\r';
				break;
			case ' ':
				c = ' ';
				break;
			case '#':
				c = '#';
				break;
			case '\\':
				c = '\\';
				break;
			default:
				c = '\\';
				i = 1;
		}
		d[0] = c;
		d++;
		src += i;
	}
	d[0] = 0;
	return src;
}

static int read_list_entry(struct copy_context *cont)
{
	char *line;
	size_t line_size;

	line = NULL;
	line_size = 0;
	while (getline(&line, &line_size, cont->cur_file) >= 0) {
		char *p;
		cont->cur_line++;

		free(cont->cur_entry.file1);
		cont->cur_entry.file1 = NULL;
		p = strcpyunescape(&cont->cur_entry.file1, line);

		if (!cont->cur_entry.file1)
			continue;

		free(cont->cur_entry.file2);
		cont->cur_entry.file2 = NULL;
		p = strcpyunescape(&cont->cur_entry.file2, p);

		if (!cont->cur_entry.file2)
			die("Invalid line no %d in file '%s'", cont->cur_line,
					cont->cur_filename);

		cont->valid_entry_fields =
				sscanf(p," %c %o %u %u %u %u %u %u %u ",
					&cont->cur_entry.action, &cont->cur_entry.mode,
					&cont->cur_entry.uid, &cont->cur_entry.gid,
					&cont->cur_entry.major_no, &cont->cur_entry.minor_no,
					&cont->cur_entry.dev_start, &cont->cur_entry.minor_inc,
					&cont->cur_entry.dev_count);

		if (cont->valid_entry_fields < 1)
			die("Invalid line no %d in file '%s'", cont->cur_line,
					cont->cur_filename);

		free(line);
		return 1;
	}

	if (feof(cont->cur_file))
		return 0;

	die("Cannot read file '%s'", cont->cur_filename);
}

static void handle_list_entry(struct copy_context *cont) {
	switch (cont->cur_entry.action) {
		case 'b':
		case 'c':
			if (cont->valid_entry_fields < 6)
				die("Invalid line no %d in file '%s'", cont->cur_line,
					cont->cur_filename);
			generate_device_files(cont);
			break;
		case 'd':
			if (cont->valid_entry_fields < 4)
				die("Invalid line no %d in file '%s'", cont->cur_line,
					cont->cur_filename);
			generate_dir(cont);
			break;
		case 'f':
			if (cont->valid_entry_fields < 4)
				die("Invalid line no %d in file '%s'", cont->cur_line,
					cont->cur_filename);
			do_copy(cont);
			break;
		case 'l':
			if (cont->valid_entry_fields < 4)
				die("Invalid line no %d in file '%s'", cont->cur_line,
					cont->cur_filename);
			generate_link(cont);
			break;
		case 'p':
			if (cont->valid_entry_fields < 4)
				die("Invalid line no %d in file '%s'", cont->cur_line,
					cont->cur_filename);
			generate_device_files(cont);
			break;
		case 's':
			do_strip(cont);
			break;
		default:
			die("Invalid action/filetype flag %c", cont->cur_entry.action);
	}
}

static void handle_file(struct copy_context *cont, const char *filename)
{
	if (!filename) {
		cont->cur_filename = "stdin";
		cont->cur_file = stdin;
	} else {
		cont->cur_filename = filename;
		cont->cur_file = fopen(filename, "r");
		if (!cont->cur_file)
			die("Cannot open file '%s'", filename);
	}

	while (read_list_entry(cont)) {
		handle_list_entry(cont);
	}

	fclose(cont->cur_file);
}

static void print_usage(void)
{
	printf("Usage: copy_lists [-s|-m] <rootfs path> <target root path> <strip util> <input file list>");
}

int main(int argc, char **argv)
{
	struct copy_context cont;
	struct stat st;
	int i = 1;
	memset(&cont, 0, sizeof(cont));

	if (argc > 1) {
		i = 2;
		if (!strcasecmp(argv[1],"-s"))
			opt_skip_special = 1;
		else if (!strcasecmp(argv[1],"-m"))
			opt_only_mode = 1;
	}

	if (argc < i + 3) {
		print_usage();
		return 1;
	}

	cont.root_fs_path = argv[i];
	if (stat(cont.root_fs_path, &st) || !S_ISDIR(st.st_mode)) {
		die("rootfs path '%s' not usable", cont.root_fs_path);
	}
	i++;

	cont.target_root_path = argv[i];
	if (stat(cont.target_root_path, &st) || !S_ISDIR(st.st_mode)) {
		die("target root path '%s' not usable", cont.target_root_path);
	}
	i++;

	cont.strip_tool = argv[i];
	if (stat(cont.strip_tool, &st) || !S_ISREG(st.st_mode)) {
		die("strip tool '%s' not usable", cont.strip_tool);
	}
	i++;

	if (argc == i) {
		handle_file(&cont, NULL);

		free(cont.cur_entry.file1);
		free(cont.cur_entry.file2);
		return 0;
	}


	for (; i < argc; i++) {
		handle_file(&cont, argv[i]);
	}

	free(cont.cur_entry.file1);
	free(cont.cur_entry.file2);
	return 0;
}
