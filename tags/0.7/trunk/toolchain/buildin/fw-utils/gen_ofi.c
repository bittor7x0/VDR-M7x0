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
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "tools.h"

#define BUFFER_SIZE (64*1024)
#define SYSTEM_TYPE_M740 1
#define SYSTEM_TYPE_M750S 2

#define IMG_MAGIC "o7o flash img"

struct system_type {
	int rootfs_size;
	int jffs_size;
	int kernel_size;
};

static const struct system_type sys_types[] = {
	{ 0, 0, 0 },
	{ 6 * 1024 * 1024, 5888 * 1024, 12 * 64 * 1024},
	{ 8 * 1024 * 1024, 3840 * 1024, 12 * 64 * 1024}
};

static struct {
	char *sha1_cmd;
	int sys_type;
	time_t timestamp;
	char *output_filename;
	char *flasher_filename;
	size_t flasher_size;
	uint32_t flasher_sha1[5];
	char *rootfs_filename;
	size_t rootfs_size;
	uint32_t rootfs_sha1[5];
	char *jffs_filename;
	size_t jffs_size;
	uint32_t jffs_sha1[5];
	char *kernel_filename;
	size_t kernel_size;
	uint32_t kernel_sha1[5];
} gen_ofi_ctx;

static void write_ofi_footer(int out_fd)
{
	uint32_t buf[256];
	int r;

	memset(buf, 0, 1024);
	strcpy((char *)buf, IMG_MAGIC);

	buf[4] = HOST2BE32(gen_ofi_ctx.sys_type);
	buf[5] = HOST2BE32(gen_ofi_ctx.flasher_size);
	buf[6] = HOST2BE32(gen_ofi_ctx.rootfs_size);
	buf[7] = HOST2BE32(gen_ofi_ctx.jffs_size);
	buf[8] = HOST2BE32(gen_ofi_ctx.kernel_size);

	buf[12] = HOST2BE32(gen_ofi_ctx.flasher_sha1[0]);
	buf[13] = HOST2BE32(gen_ofi_ctx.flasher_sha1[1]);
	buf[14] = HOST2BE32(gen_ofi_ctx.flasher_sha1[2]);
	buf[15] = HOST2BE32(gen_ofi_ctx.flasher_sha1[3]);
	buf[16] = HOST2BE32(gen_ofi_ctx.flasher_sha1[4]);

	buf[20] = HOST2BE32(gen_ofi_ctx.rootfs_sha1[0]);
	buf[21] = HOST2BE32(gen_ofi_ctx.rootfs_sha1[1]);
	buf[22] = HOST2BE32(gen_ofi_ctx.rootfs_sha1[2]);
	buf[23] = HOST2BE32(gen_ofi_ctx.rootfs_sha1[3]);
	buf[24] = HOST2BE32(gen_ofi_ctx.rootfs_sha1[4]);

	buf[28] = HOST2BE32(gen_ofi_ctx.jffs_sha1[0]);
	buf[29] = HOST2BE32(gen_ofi_ctx.jffs_sha1[1]);
	buf[30] = HOST2BE32(gen_ofi_ctx.jffs_sha1[2]);
	buf[31] = HOST2BE32(gen_ofi_ctx.jffs_sha1[3]);
	buf[32] = HOST2BE32(gen_ofi_ctx.jffs_sha1[4]);

	buf[36] = HOST2BE32(gen_ofi_ctx.kernel_sha1[0]);
	buf[37] = HOST2BE32(gen_ofi_ctx.kernel_sha1[1]);
	buf[38] = HOST2BE32(gen_ofi_ctx.kernel_sha1[2]);
	buf[39] = HOST2BE32(gen_ofi_ctx.kernel_sha1[3]);
	buf[40] = HOST2BE32(gen_ofi_ctx.kernel_sha1[4]);

	r = write(out_fd, buf, 1024);
	if (r != 1024)
		die("Cannot write ofi footer!");
}

static void write_part(int out_fd, const char *name, int size, uint32_t sha1[5])
{
	int in_fd;
	int sha1_wfd;
	int sha1_rfd;
	pid_t sha1_pid;
	uint8_t buffer[BUFFER_SIZE];
	int readed;
	int r;
	int r2;

	in_fd = open(name, O_RDONLY);
	if (in_fd < 0)
		die("Cannot open file '%s'", name);

	exec_cmd(&sha1_wfd, &sha1_rfd, &sha1_pid, gen_ofi_ctx.sha1_cmd, 0);

	readed = 0;
	while (readed < size) {
		r = read(in_fd, buffer, BUFFER_SIZE);
		if (r < 0) {
			die("Cannot read file '%s'", name);
		}

		r2 = write(out_fd, buffer, r);
		if (r2 != r)
			die("Cannot write to output file");

		r2 = write(sha1_wfd, buffer, r);
		if (r2 != r)
			die("Cannot write to sha1 cmd");
		readed += r;
	}

	close(in_fd);
	close(sha1_wfd);

	r = read(sha1_rfd, buffer, BUFFER_SIZE);
	if (r < 0) {
		die("Cannot read from sha1 cmd");
	}

	buffer[r] = 0;
	close(sha1_rfd);

	wait_on_cmd(sha1_pid, gen_ofi_ctx.sha1_cmd);
	r = sscanf((char *)buffer," %8x%8x%8x%8x%8x ", &sha1[0], &sha1[1], &sha1[2],
		 &sha1[3], &sha1[4]);
	if (r != 5) {
		die("Cannot get sha1 checksum for file '%s'", name);
	}

}

static void write_image(void)
{
	int out_fd;
	struct stat st;

	if (stat(gen_ofi_ctx.flasher_filename, &st)) {
		die("Cannot get size of flasher '%s'",gen_ofi_ctx.flasher_filename);
	}
	gen_ofi_ctx.flasher_size = st.st_size;

	gen_ofi_ctx.rootfs_size = 0;
	if (gen_ofi_ctx.rootfs_filename) {
		if (stat(gen_ofi_ctx.rootfs_filename, &st)) {
			die("Cannot get size of rootfs image '%s'",gen_ofi_ctx.rootfs_filename);
		}
		gen_ofi_ctx.rootfs_size = st.st_size;
		if (sys_types[gen_ofi_ctx.sys_type].rootfs_size <
				gen_ofi_ctx.rootfs_size)
			die("Rootfs image too big");
	}

	gen_ofi_ctx.jffs_size = 0;
	if (gen_ofi_ctx.jffs_filename) {
		if (stat(gen_ofi_ctx.jffs_filename, &st)) {
			die("Cannot get size of rootfs image '%s'",gen_ofi_ctx.jffs_filename);
		}
		gen_ofi_ctx.jffs_size = st.st_size;
		if (sys_types[gen_ofi_ctx.sys_type].jffs_size <
				gen_ofi_ctx.jffs_size)
			die("Jffs2 image too big");
	}

	gen_ofi_ctx.kernel_size = 0;
	if (gen_ofi_ctx.kernel_filename) {
		if (stat(gen_ofi_ctx.kernel_filename, &st)) {
			die("Cannot get size of kernel image '%s'",gen_ofi_ctx.kernel_filename);
		}
		gen_ofi_ctx.kernel_size = st.st_size;
		if (sys_types[gen_ofi_ctx.sys_type].kernel_size <
				gen_ofi_ctx.kernel_size)
			die("Kernel image too big");
	}

	out_fd = open(gen_ofi_ctx.output_filename,
		O_WRONLY | O_CREAT | O_TRUNC, 0755);
	if (out_fd < 0)
		die("Cannot open output file '%s'", gen_ofi_ctx.output_filename);

	write_part(out_fd, gen_ofi_ctx.flasher_filename, gen_ofi_ctx.flasher_size,
		gen_ofi_ctx.flasher_sha1);

	if (gen_ofi_ctx.rootfs_filename)
		write_part(out_fd, gen_ofi_ctx.rootfs_filename, gen_ofi_ctx.rootfs_size,
			gen_ofi_ctx.rootfs_sha1);

	if (gen_ofi_ctx.jffs_filename)
		write_part(out_fd, gen_ofi_ctx.jffs_filename, gen_ofi_ctx.jffs_size,
			gen_ofi_ctx.jffs_sha1);

	if (gen_ofi_ctx.kernel_filename)
		write_part(out_fd, gen_ofi_ctx.kernel_filename, gen_ofi_ctx.kernel_size,
			gen_ofi_ctx.kernel_sha1);

	write_ofi_footer(out_fd);
	close(out_fd);
}

static void __attribute__ ((noreturn)) usage(void)
{
	fprintf(stderr,
		"gen_ofi [Options]\n\n"
		"Options:\n "
		"\t-o <file>\tWrite ofi to <file>\n"
		"\t\t\t(default: open7x0.org-%%Y-%%m-%%d-%%H%%M.ofi)\n"
		"\t-f|--flasher-bin <bin>\tSet binary file name of flasher to <bin>\n"
		"\t-t|--m7x0-type <type>\tType of m7x0 to <type> :=  m740av or m750s\n"
		"\t-r|--rootfs-image <image>\tSet image file name of rootfs to <image>\n"
		"\t-j|--jffs2-image <image>\tSet image file name of jffs2 to <image>\n"
		"\t-k|--kernel-image <image>\tSet image file name of kernel to <image>\n"
		"\t-s|--sha1-cmd <cmd>\tUse <cmd> to calculate sha1 (default: sha1sum)\n"
		"\t-d|--timestamp <time>\tTimestamp of firmware (default: current)\n"
		"\t--m740av\tSet type to m740av\n"
		"\t--m750s\tSet type to m750s\n");
	exit(1);
}

static void parse_args(int argc, char *argv[])
{
	char *env_ent;
	int opt_val;
	char opts_str[] = "o:f:t:r:j:k:s:d:";
	struct option long_opts[] = {
		{"flasher-bin", 1, NULL, 'f'},
		{"m7x0-type", 1, NULL, 't'},
		{"rootfs-image", 1, NULL, 'r'},
		{"jffs2-image", 1, NULL, 'j'},
		{"kernel-image", 1, NULL, 'k'},
		{"sha1-cmd", 1, NULL, 's'},
		{"timestamp", 1, NULL, 'd'},
		{"m740av", 0, &gen_ofi_ctx.sys_type, SYSTEM_TYPE_M740},
		{"m740", 0, &gen_ofi_ctx.sys_type, SYSTEM_TYPE_M740},
		{"m750s", 0, &gen_ofi_ctx.sys_type, SYSTEM_TYPE_M750S},
		{"help", 0, NULL, 'h'},
		{NULL,0 ,NULL, 0}
	};

	memset(&gen_ofi_ctx, 0, sizeof(gen_ofi_ctx));
	gen_ofi_ctx.timestamp = time(NULL);
	if ((env_ent = getenv("SHA1"))) {
		gen_ofi_ctx.sha1_cmd = strdup(env_ent);
	} else {
		gen_ofi_ctx.sha1_cmd = strdup("sha1sum");
	}

	if (!gen_ofi_ctx.sha1_cmd)
		die("Cannot alloc memory");

	while((opt_val = getopt_long(argc, argv, opts_str, long_opts, NULL)) != -1) {
		switch (opt_val) {
			case 0:
				break;

			case 'o':
				free(gen_ofi_ctx.output_filename);
				gen_ofi_ctx.output_filename = strdup(optarg);
				if (!gen_ofi_ctx.output_filename)
					die("Cannot alloc memory");
				break;

			case 'f':
				free(gen_ofi_ctx.flasher_filename);
				gen_ofi_ctx.flasher_filename = strdup(optarg);
				if (!gen_ofi_ctx.flasher_filename)
					die("Cannot alloc memory");
				break;

			case 't':
				if (!strcasecmp("m740av",optarg)) {
					gen_ofi_ctx.sys_type = SYSTEM_TYPE_M740;
				} else if (!strcasecmp("m750s",optarg)) {
					gen_ofi_ctx.sys_type = SYSTEM_TYPE_M750S;
				} else if (!strcasecmp("m740",optarg)) {
					gen_ofi_ctx.sys_type = SYSTEM_TYPE_M740;
				} else {
					fprintf(stderr,"Unknown system type '%s'\n\n", optarg);
					usage();
				}
				break;

			case 'r':
				free(gen_ofi_ctx.rootfs_filename);
				gen_ofi_ctx.rootfs_filename = strdup(optarg);
				if (!gen_ofi_ctx.rootfs_filename)
					die("Cannot alloc memory");
				break;

			case 'j':
				free(gen_ofi_ctx.jffs_filename);
				gen_ofi_ctx.jffs_filename = strdup(optarg);
				if (!gen_ofi_ctx.jffs_filename)
					die("Cannot alloc memory");
				break;

			case 'k':
				free(gen_ofi_ctx.kernel_filename);
				gen_ofi_ctx.kernel_filename = strdup(optarg);
				if (!gen_ofi_ctx.kernel_filename)
					die("Cannot alloc memory");
				break;

			case 's':
				free(gen_ofi_ctx.sha1_cmd);
				gen_ofi_ctx.sha1_cmd = strdup(optarg);
				if (!gen_ofi_ctx.sha1_cmd)
					die("Cannot alloc memory");
				break;

			case 'd': {
				char *endptr;
				gen_ofi_ctx.timestamp=strtol(optarg,&endptr,0);
				if (*endptr) {
					fprintf(stderr,"Timestamp has to be a numeric value\n\n");
					usage();
				}
				}
				break;

			case '?':
				fprintf(stderr, "Invalid option\n\n");
				usage();
				break;
			case 'h':
				usage();
				break;
			default:
				fprintf(stderr, "Unknown option '%c'\n\n",opt_val);
				usage();
				break;
		}
	}

	/* Arg checking */
	if (!gen_ofi_ctx.flasher_filename) {
		fprintf(stderr,"Flasher binary not given!\n\n");
		usage();
	}

	if (!gen_ofi_ctx.sys_type) {
		fprintf(stderr,"System type not given!\n\n");
		usage();
	}

	if (!gen_ofi_ctx.rootfs_filename && !gen_ofi_ctx.jffs_filename &&
			!gen_ofi_ctx.kernel_filename) {
		fprintf(stderr,"At least one image has to be given (empty ofis are not very usable)!\n\n");
		usage();
	}

	if (!gen_ofi_ctx.output_filename) {
		gen_ofi_ctx.output_filename = malloc(128);
		if (!gen_ofi_ctx.output_filename)
			die("Cannot alloc memory");
		strftime(gen_ofi_ctx.output_filename,128,
			"open7x0.org-%Y-%m-%d-%H%M.ofi",localtime(&gen_ofi_ctx.timestamp));
		gen_ofi_ctx.output_filename[127] = 0;
	}

}

int main(int argc, char *argv[])
{
	parse_args(argc, argv);
	write_image();
	free(gen_ofi_ctx.output_filename);
	free(gen_ofi_ctx.flasher_filename);
	free(gen_ofi_ctx.rootfs_filename);
	free(gen_ofi_ctx.jffs_filename);
	free(gen_ofi_ctx.kernel_filename);
	free(gen_ofi_ctx.sha1_cmd);
	return 0;
}

