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
 * $Id: gen_wsw.c 152 2007-09-05 09:55:48Z andreas $
 *
 */

#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <errno.h>
#include "tools.h"

#define M7X0_TYPE_M740AV 0
#define M740AV_WSW_NAME "M740AV"
#define M7X0_TYPE_M750S 1
#define M750S_WSW_NAME "M750AV"
#define M7X0_TYPE_COUNT 2
#define SECTOR_SIZE 65536
#define MD5_START_OFFSET 0x400
#define MD5_SUM_LENGTH 32
#define RSA_CODE1_LENGTH 44
#define RSA_CODE2_LENGTH 129

struct flash_map {
  uint8_t rootfs_start_sector;
  uint8_t data_start_sector;
  uint8_t etc_start_sector;
  uint8_t boot_sector;
  uint8_t kernel_start_sector;
  uint8_t rescue_kernel_start_sector;
  uint8_t rescue_rootfs_start_sector;
  uint8_t last_sector;
};

const struct flash_map flash_mappings[M7X0_TYPE_COUNT] =
{
  {	0,	96,	188,	192,	193,	205,	216,	255},
  {	0,	128,	188,	192,	193,	205,	216,	255}
};

struct wsw_header {
  char magic[4];
  uint16_t md5_length_1;
  uint16_t rsa_length_1;
  uint8_t rsa_code_1[56];
  uint16_t md5_length_2;
  uint16_t rsa_length_2;
  uint8_t rsa_code_2[956];
  char system_name[64];
  char firmware_name[60];
  uint8_t unknown[4];
  uint32_t timestamp;
  uint32_t sector_count;
  uint8_t sector_mapping[256];
} __attribute__ ((packed));


struct gen_wsw_context {
  char *md5_cmd;
  char *rsaencode_cmd;
  char *rsakey1_filename;
  char *rsakey2_filename;
  int m7x0_type;
  char *output_filename;
  char *root_img_filename;
  size_t root_img_size;
  char *data_img_filename;
  size_t data_img_size;
  char *kernel_img_filename;
  size_t kernel_img_size;
  char md5_csum[MD5_SUM_LENGTH + 1];
  struct wsw_header header;
};



void gen_header(struct gen_wsw_context *context)
{
  context->header.magic[0] = 'W';
  context->header.magic[1] = 'S';
  context->header.magic[2] = 'W';
  context->header.magic[3] = ' ';

  switch (context->m7x0_type) {
    case M7X0_TYPE_M740AV:
      strncpy(context->header.system_name, M740AV_WSW_NAME,
        sizeof(context->header.system_name));
      break;
    case M7X0_TYPE_M750S:
      strncpy(context->header.system_name, M750S_WSW_NAME,
        sizeof(context->header.system_name));
      break;
    default:
      die("Unknown m7x0 type %d", context->m7x0_type);
  }
  context->header.system_name[sizeof(context->header.system_name)-1] = 0;
  context->header.timestamp = HOST2BE32(context->header.timestamp);
}

void gen_sector_mapping(struct gen_wsw_context *context, char *filename,
    size_t *file_size, int start_sector, int last_sector)
{
  struct stat st;
  int fsector;
  int fsector_count;

  if (stat(filename, &st))
    die("Cannot get size of image file '%s'",filename);

  *file_size = st.st_size;
  fsector_count = (st.st_size + SECTOR_SIZE - 1) / SECTOR_SIZE;

  if (last_sector - start_sector + 1 < fsector_count)
    die("Image file '%s' too big!", filename);

  for (fsector = 0; fsector < fsector_count; fsector++) {
    context->header.sector_mapping[context->header.sector_count] =
        fsector + start_sector;
    context->header.sector_count++;
  }
}

void gen_all_sector_mappings(struct gen_wsw_context *context)
{
  const struct flash_map *flash_map;

  flash_map = &flash_mappings[context->m7x0_type];

  if (context->root_img_filename) {
    gen_sector_mapping(context, context->root_img_filename,
        &context->root_img_size, flash_map->rootfs_start_sector,
        flash_map->data_start_sector -1);
    }

  if (context->data_img_filename) {
    gen_sector_mapping(context, context->data_img_filename,
        &context->data_img_size, flash_map->data_start_sector,
        flash_map->etc_start_sector - 1);
  }

  if (context->kernel_img_filename) {
    gen_sector_mapping(context, context->kernel_img_filename,
        &context->kernel_img_size, flash_map->kernel_start_sector,
        flash_map->rescue_kernel_start_sector - 1);
  }

  context->header.sector_count = HOST2BE32(context->header.sector_count);
}

void write_file_padded(int outfd, const char *filename, size_t filesize)
{
  int infd;
  uint8_t buf[SECTOR_SIZE];
  infd = open(filename, O_RDONLY);
  if (infd < 0)
    die("Cannot open file '%s'", filename);

  while (filesize >= SECTOR_SIZE) {
    if (read(infd, buf, SECTOR_SIZE) != SECTOR_SIZE)
      die("Cannot read from file '%s'", filename);

    filesize -= SECTOR_SIZE;
    if (write(outfd, buf, SECTOR_SIZE) != SECTOR_SIZE)
      die("Cannot write data");

  }

  if (filesize) {
    memset(buf, 0xff, SECTOR_SIZE);
    if (read(infd, buf, filesize) != filesize)
      die("Cannot read from file '%s'", filename);

    if (write(outfd, buf, SECTOR_SIZE) != SECTOR_SIZE)
      die("Cannot write date");
  }

  if (close(infd))
    die("Cannot close file '%s'", filename);
}

void gen_md5_sum(struct gen_wsw_context *context)
{
  char buf[1024];
  char *sep;
  uint8_t *raw_header;
  int write_md5_fd;
  int read_md5_fd;
  pid_t md5_pid;
  int md5_read_size;

  exec_cmd(&write_md5_fd, &read_md5_fd, &md5_pid, context->md5_cmd, 0);

  raw_header = (uint8_t *) &context->header;
  raw_header += MD5_START_OFFSET;
  if (write(write_md5_fd, raw_header, sizeof(context->header) - MD5_START_OFFSET)
      != sizeof(context->header) - MD5_START_OFFSET)
    die("Cannot write wsw header to md5 process");

  if (context->root_img_filename) {
    write_file_padded(write_md5_fd, context->root_img_filename,
        context->root_img_size);
  }

  if (context->data_img_filename) {
    write_file_padded(write_md5_fd, context->data_img_filename,
        context->data_img_size);
  }

  if (context->kernel_img_filename) {
    write_file_padded(write_md5_fd, context->kernel_img_filename,
        context->kernel_img_size);
  }

  if (close(write_md5_fd))
    die("Cannot close pipe for writing to md5 process");

  if ((md5_read_size = read(read_md5_fd, buf, 1023)) <= 0)
    die("Cannot read md5 checksum!");

  if (close(read_md5_fd))
    die("Cannot close pipe for read from md5 process");

  wait_on_cmd(md5_pid, context->md5_cmd);

  buf[md5_read_size] = 0;
  sep = strchr(buf,' ');
  if (!sep)
    sep = strchr(buf,'\r');

  if (!sep)
    sep = strchr(buf,'\n');

  if (sep)
    md5_read_size = sep - buf;

  if (md5_read_size != MD5_SUM_LENGTH)
    die("md5 checksum has wrong length %d!", md5_read_size);

  memcpy(context->md5_csum, buf, MD5_SUM_LENGTH);
  context->md5_csum[MD5_SUM_LENGTH] = 0;
  context->header.md5_length_1 = HOST2BE16(MD5_SUM_LENGTH + 4);
  context->header.md5_length_2 = HOST2BE16(MD5_SUM_LENGTH + 4);
}

void sign_wsw(struct gen_wsw_context *context)
{
  char md5_csum_rsa[MD5_SUM_LENGTH + 4];
  char buf[1024];
  int write_rsa_fd;
  int read_rsa_fd;
  pid_t rsa_pid;
  int read_bytes;

  memcpy(md5_csum_rsa, context->md5_csum, MD5_SUM_LENGTH);
  md5_csum_rsa[MD5_SUM_LENGTH] = ' ';
  md5_csum_rsa[MD5_SUM_LENGTH + 1] = ' ';
  md5_csum_rsa[MD5_SUM_LENGTH + 2] = '-';
  md5_csum_rsa[MD5_SUM_LENGTH + 3] = 0x0a;

  exec_cmd(&write_rsa_fd, &read_rsa_fd, &rsa_pid, context->rsaencode_cmd, 1,
      context->rsakey1_filename);

  if (write(write_rsa_fd, md5_csum_rsa, MD5_SUM_LENGTH + 4) !=
      MD5_SUM_LENGTH + 4)
    die("Cannot write md5 checksum to rsaencode process");

  if (close(write_rsa_fd))
    die("Cannot close pipe for writing to rsaencode process");

  if ((read_bytes = read(read_rsa_fd, buf, 1024)) < 0)
    die("Cannot read rsa code 1");

  if (close(read_rsa_fd))
    die("Cannot close pipe for reading from rsaencode process");

  wait_on_cmd(rsa_pid, context->rsaencode_cmd);

  if (read_bytes != RSA_CODE1_LENGTH)
    die("rsa code 1 has wrong length %d!", read_bytes);

  memcpy(context->header.rsa_code_1, buf, RSA_CODE1_LENGTH);
  context->header.rsa_length_1 = HOST2BE16(RSA_CODE1_LENGTH);

  exec_cmd(&write_rsa_fd, &read_rsa_fd, &rsa_pid, context->rsaencode_cmd, 1,
      context->rsakey2_filename);

  if (write(write_rsa_fd, md5_csum_rsa, MD5_SUM_LENGTH + 4) !=
      MD5_SUM_LENGTH + 4)
    die("Cannot write md5 checksum to rsaencode process");

  if (close(write_rsa_fd))
    die("Cannot close pipe for writing to rsaencode process");

  if ((read_bytes = read(read_rsa_fd, buf, 1024)) < 0)
    die("Cannot read rsa code 1");

  if (close(read_rsa_fd))
    die("Cannot close pipe for reading from rsaencode process");

  wait_on_cmd(rsa_pid, context->rsaencode_cmd);

  if (read_bytes != RSA_CODE2_LENGTH)
    die("rsa code 2 has wrong length %d!", read_bytes);

  memcpy(context->header.rsa_code_2, buf, RSA_CODE2_LENGTH);
  context->header.rsa_length_2 = HOST2BE16(RSA_CODE2_LENGTH);
}

void write_wsw(struct gen_wsw_context *context)
{
  int outfd;
  if ((outfd = open(context->output_filename, O_WRONLY | O_CREAT  | O_TRUNC, 0644)) < 0)
    die("Cannot open output file '%s'", context->output_filename);

  if (write(outfd, &context->header, sizeof(context->header)) !=
      sizeof(context->header))
    die("Cannot write header to output file");

  if (context->root_img_filename) {
    write_file_padded(outfd, context->root_img_filename,
        context->root_img_size);
  }

  if (context->data_img_filename) {
    write_file_padded(outfd, context->data_img_filename,
        context->data_img_size);
  }

  if (context->kernel_img_filename) {
    write_file_padded(outfd, context->kernel_img_filename,
        context->kernel_img_size);
  }

  if (close(outfd))
    die("Cannot close output file '%s'", context->output_filename);
}

void __attribute__ ((noreturn)) usage(void)
{
  fprintf(stderr,
    "gen_wsw [Options]\n\n"
    "Options:\n "
    "\t-o <file>\tWrite wsw to <file>\n"
    "\t\t\t(default: <firmware name>.wsw)\n"
    "\t-n|--firmware-name <firmware name>\tSet name of firmware to <firmware name>\n"
    "\t\t\t(default: open7x0.org-%%Y-%%m-%%d-%%H%%M)\n"
    "\t-t|--m7x0-type <type>\tType of m7x0 to <type> :=  m740av or m750s\n"
    "\t-r|--rootfs-image <image>\tSet image file name of rootfs to <image>\n"
    "\t-d|--data-image <image>\tSet image file name of data to <image>\n"
    "\t-k|--kernel-image <image>\tSet image file name of kernel to <image>\n"
    "\t-1|--rsa-key1 <file>\tRead rsa private key 1 from <file>\n"
    "\t-2|--rsa-key2 <file>\tRead rsa private key 2 from <file>\n"
    "\t-m|--md5-cmd <cmd>\tUse <cmd> to calculate md5 (default: md5sum)\n"
    "\t-c|--rsaencode-cmd <cmd>\tUse <cmd> for rsa encoding (default: rsaencode)\n"
    "\t-s|--timestamp <time>\tTimestamp of firmware (default: current)\n"
    "\t--m740av\tSet type to m740av\n"
    "\t--m750s\t\tSet type to m750s\n");
  exit(1);
}

void parse_args(struct gen_wsw_context *context, int argc, char *argv[])
{
  char *env_ent;
  int opt_val;
  int length;
  char opts_str[] = "o:n:r:d:k:t:m:c:1:2:s:h";
  struct option long_opts[] = {
    {"firmware-name", 1, NULL, 'n'},
    {"m7x0-type", 1, NULL, 't'},
    {"rootfs-image", 1, NULL, 'r'},
    {"data-image", 1, NULL, 'd'},
    {"kernel-image", 1, NULL, 'k'},
    {"rsa-key1", 1, NULL, '1'},
    {"rsa-key2", 1, NULL, '2'},
    {"md5-cmd", 1, NULL, 'm'},
    {"rsaencode-cmd", 1, NULL, 'c'},
    {"timestamp", 1, NULL, 's'},
    {"m740av", 0, &context->m7x0_type, M7X0_TYPE_M740AV},
    {"m740", 0, &context->m7x0_type, M7X0_TYPE_M740AV},
    {"m750s", 0, &context->m7x0_type, M7X0_TYPE_M750S},
    {"help", 0, NULL, 'h'},
    {NULL,0 ,NULL, 0}
  };


  context->m7x0_type = -1;
  if ((env_ent = getenv("MD5"))) {
    int len = strlen(env_ent) + 1;
    context->md5_cmd = malloc(len);
    if (!context->md5_cmd)
      die("Cannot alloc memory");

    memcpy(context->md5_cmd, env_ent, len);
  }

  if ((env_ent = getenv("RSAENCODE"))) {
    int len = strlen(env_ent) + 1;
    context->rsaencode_cmd = malloc(len);
    if (!context->rsaencode_cmd)
      die("Cannot alloc memory");

    memcpy(context->rsaencode_cmd, env_ent, len);
  }

  while((opt_val = getopt_long(argc, argv, opts_str, long_opts, NULL)) != -1) {
    switch (opt_val) {
      case 0:
        break;
      case 'o':
        length = strlen(optarg) + 1;
        context->output_filename = malloc(length);
        if (!context->output_filename)
          die("Cannot alloc memory");
        memcpy(context->output_filename,optarg,length);
        break;
      case 'n':
        length = strlen(optarg) + 1;
        if (length > 60)
          die("Firmware name too long, only 59 characters are allowed");
        memcpy(context->header.firmware_name,optarg,length);
        break;
      case 't':
        if (!strcasecmp("m740av",optarg)) {
          context->m7x0_type = M7X0_TYPE_M740AV;
        } else if (!strcasecmp("m750s",optarg)) {
          context->m7x0_type = M7X0_TYPE_M750S;
        } else {
          fprintf(stderr,"Unknown system type '%s'\n\n", optarg);
          usage();
        }
        break;
      case 'r':
        length = strlen(optarg) + 1;
        context->root_img_filename = malloc(length);
        if (!context->root_img_filename)
          die("Cannot alloc memory");
        memcpy(context->root_img_filename,optarg,length);
        break;
      case 'd':
        length = strlen(optarg) + 1;
        context->data_img_filename = malloc(length);
        if (!context->data_img_filename)
          die("Cannot alloc memory");
        memcpy(context->data_img_filename,optarg,length);
        break;
      case 'k':
        length = strlen(optarg) + 1;
        context->kernel_img_filename = malloc(length);
        if (!context->kernel_img_filename)
          die("Cannot alloc memory");
        memcpy(context->kernel_img_filename,optarg,length);
        break;
      case '1':
        length = strlen(optarg) + 1;
        context->rsakey1_filename = malloc(length);
        if (!context->rsakey1_filename)
          die("Cannot alloc memory");
        memcpy(context->rsakey1_filename,optarg,length);
        break;
      case '2':
        length = strlen(optarg) + 1;
        context->rsakey2_filename = malloc(length);
        if (!context->rsakey2_filename)
          die("Cannot alloc memory");
        memcpy(context->rsakey2_filename,optarg,length);
        break;
      case 'm':
        free(context->md5_cmd);
        length = strlen(optarg) + 1;
        context->md5_cmd = malloc(length);
        if (!context->md5_cmd)
          die("Cannot alloc memory");
        memcpy(context->md5_cmd,optarg,length);
        break;
      case 'c':
        free(context->rsaencode_cmd);
        length = strlen(optarg) + 1;
        context->rsaencode_cmd = malloc(length);
        if (!context->rsaencode_cmd)
          die("Cannot alloc memory");
        memcpy(context->rsaencode_cmd,optarg,length);
        break;
      case 's': {
        char *endptr;
        context->header.timestamp=strtol(optarg,&endptr,0);
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
  if (context->m7x0_type == -1) {
    fprintf(stderr,"System type not given!\n\n");
    usage();
  }

  if (!context->root_img_filename && !context->data_img_filename &&
      !context->kernel_img_filename) {
    fprintf(stderr,"At least one image has to be given (empty wsw are not very usable)!\n\n");
    usage();
  }

  if (!context->rsakey1_filename || !context->rsakey2_filename) {
    fprintf(stderr,"You need to specify both rsa key files!\n\n");
    usage();
  }

  /* defaults */
  if (!context->header.timestamp) {
    context->header.timestamp = (uint32_t) time(NULL);
  }

  if (!context->header.firmware_name[0]) {
    time_t fwtime = (time_t)context->header.timestamp;
    strftime(context->header.firmware_name,
      sizeof(context->header.firmware_name),
      "open7x0.org-%Y-%m-%d-%H%M",localtime(&fwtime));
  }
  context->header.firmware_name[sizeof(context->header.firmware_name) - 1] = 0;

  if (!context->output_filename) {
    const int flen = sizeof(context->header.firmware_name) + 4;
    context->output_filename = malloc(flen);
    if (!context->output_filename)
      die("Cannot alloc memory!");
    snprintf(context->output_filename, flen, "%s.wsw",
      context->header.firmware_name);
  }

  if (!context->md5_cmd) {
    context->md5_cmd = malloc(7);
    if (!context->md5_cmd)
      die("Cannot alloc memory!");
    memcpy(context->md5_cmd, "md5sum",7);
  }

  if (!context->rsaencode_cmd) {
    context->rsaencode_cmd = malloc(10);
    if (!context->rsaencode_cmd)
      die("Cannot alloc memory!");
    memcpy(context->rsaencode_cmd, "rsaencode",10);
  }
}

void free_context(struct gen_wsw_context *context)
{
  free(context->md5_cmd);
  free(context->rsaencode_cmd);
  free(context->rsakey1_filename);
  free(context->rsakey2_filename);
  free(context->output_filename);
  free(context->root_img_filename);
  free(context->data_img_filename);
  free(context->kernel_img_filename);
}

int main(int argc, char *argv[])
{
  struct gen_wsw_context context;
  memset(&context, 0, sizeof(context));

  parse_args(&context,argc, argv);
  gen_header(&context);
  gen_all_sector_mappings(&context);
  gen_md5_sum(&context);
  sign_wsw(&context);
  write_wsw(&context);
  free_context(&context);
  return 0;
}
