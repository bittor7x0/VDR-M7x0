/*
 * Copyright (C) 2007 Andreas Koch - the open7x0.org group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 *
 * The author can be reached at andreas@open7x0.org
 *
 * The project's page is at http://www.open7x0.org
 *
 * $Id: gen_kernel_header.c 151 2007-09-05 09:32:38Z andreas $
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "tools.h"

#define KERNEL_SIZE_OFFSET 0x74
#define KERNEL_LOADADDR_OFFSET 0x7c
#define KERNEL_ENTRYPOINT_OFFSET 0x80
#define KERNEL_HEADER_SIZE 0x20c

uint8_t kernel_header_start[0x80] = {
   // Rest remains zero or should replaced any way
   'D',  'I',  'S',  'C',  'V',  'I',  'S',  'I',
   'O',  'N',  ' ',  ' ', 0x00, 0x00, 0x00, 0x00,

   'D',  'I',  'S',  'C',  'V',  'I',  'S',  'I',
   'O',  'N', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x02, 0x0c, 0x00, 0x00, 0x00, 0x00,
  0x81, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define SYSTEM_MAP_FORMAT_STR "%x %*c %255s\n"
#define KERNEL_LOADADDR_SYM "_ftext"
#define KERNEL_ENTRY_SYM "kernel_entry"

void usage(void)
{
  printf("gen_kernel_header <kernel gziped binary> <kernel system map>\n");
  printf("\tgenerates header for kernel <kernel gziped binary>  \n"
         "\twith <kernel system map> and print it to stdout\n");
}

void gen_header(uint32_t kernel_size, uint32_t kernel_loadaddr,
                uint32_t kernel_entrypoint)
{
  uint8_t kernel_header[KERNEL_HEADER_SIZE];

  memset(kernel_header, 0, KERNEL_HEADER_SIZE);
  memcpy(kernel_header, kernel_header_start, sizeof(kernel_header_start));

  kernel_size = HOST2BE32(kernel_size);
  kernel_loadaddr = HOST2BE32(kernel_loadaddr);
  kernel_entrypoint = HOST2BE32(kernel_entrypoint);

  memcpy(kernel_header + KERNEL_SIZE_OFFSET, &kernel_size, 4);
  memcpy(kernel_header + KERNEL_LOADADDR_OFFSET, &kernel_loadaddr, 4);
  memcpy(kernel_header + KERNEL_ENTRYPOINT_OFFSET, &kernel_entrypoint, 4);

  if (fwrite((void *) kernel_header, KERNEL_HEADER_SIZE, 1,stdout) != 1)
    die("Cannot write header");

  if (fflush(stdout))
    die("Cannot flush stdout");
}

void parse_system_map(const char *map_filename, uint32_t *kernel_loadaddr,
                      uint32_t *kernel_entrypoint)
{
  FILE *map_file;
  char sym_name[256];
  uint32_t sym_addr;
  int loadaddr_found = 0;
  int entrypoint_found = 0;

  if (!(map_file = fopen(map_filename,"r")))
    die("Cannot open system map file %s", map_filename);

  while ((!loadaddr_found || !entrypoint_found)) {
    if (fscanf(map_file, SYSTEM_MAP_FORMAT_STR, &sym_addr, sym_name) != 2)
      die("Cannot parse system map");

    sym_name[255] = 0;
    if (!strcmp(sym_name, KERNEL_LOADADDR_SYM)) {
      loadaddr_found = 1;
      *kernel_loadaddr = sym_addr;
    } else if (!strcmp(sym_name, KERNEL_ENTRY_SYM)) {
      entrypoint_found = 1;
      *kernel_entrypoint = sym_addr;
    }
  }
  fclose(map_file);
}

int main(int argc, char **argv)
{
  uint32_t kernel_size;
  uint32_t kernel_loadaddr;
  uint32_t kernel_entrypoint;
  struct stat kernel_stat;

  if (argc < 3) {
    usage();
    return 1;
  }

  if (stat(argv[1],&kernel_stat))
    die("Cannot get size of gziped kernel image %s", argv[1]);

  kernel_size = (uint32_t) kernel_stat.st_size;
  parse_system_map(argv[2],&kernel_loadaddr, &kernel_entrypoint);
  gen_header(kernel_size, kernel_loadaddr, kernel_entrypoint);
  return 0;
}
